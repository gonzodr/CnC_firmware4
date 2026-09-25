#include <Servo.h>
#include <Wire.h>

// CnC weed-meter / auxiliary servo controller
//
// Hardware recovered from the machine-deployed legacy Uno firmware:
//   D4 = servo 1 signal (legacy weed meter)
//   D5 = servo 2 signal
//   D6 = servo 3 signal
//   D7 = shared servo-power relay, active HIGH
//   A4/A5 = I2C slave bus to the Mega
//
// Legacy packet, preserved unchanged: ['a'][0..180]
// Extended packets:                 ['b'][0..180], ['c'][0..180]
// Emergency stop:                   ['x'][any value]

namespace {

constexpr uint8_t UNO_I2C_ADDRESS = 8;
constexpr uint8_t SERVO_COUNT = 3;
constexpr uint8_t RELAY_PIN = 7;
constexpr bool RELAY_ACTIVE_HIGH = true;

// A full 180-step ramp takes 2.7 seconds, leaving watchdog margin for power-up
// and settling.
constexpr unsigned long POWER_STABILIZE_MS = 150;
constexpr unsigned long MOVE_STEP_MS = 15;
constexpr unsigned long SETTLE_MS = 450;
constexpr unsigned long SIGNAL_OFF_DELAY_MS = 60;
constexpr unsigned long POWER_OFF_GAP_MS = 100;
constexpr unsigned long MAX_POWER_ON_MS = 4000;

// Keep disabled in the cabinet. Never print from an I2C callback: Wire invokes
// it from interrupt context.
#define SERVO_UNO_SERIAL_DEBUG 0

struct ServoConfig {
  uint8_t pin;
  uint16_t minPulseUs;
  uint16_t maxPulseUs;
  bool reverse;
};

// The legacy sketch used Servo.write(0..180), approximately 544..2400 us.
// 900..2100 us is deliberately narrower to protect SG90 end stops. Tune each
// channel only with the horn/mechanism disconnected.
constexpr ServoConfig SERVO_CONFIG[SERVO_COUNT] = {
    {4, 900, 2100, false},
    {5, 900, 2100, false},
    {6, 900, 2100, false},
};

enum MotionState : uint8_t {
  IDLE_POWERED_OFF = 0,
  WAITING_FOR_POWER,
  MOVING,
  SETTLING,
  WAITING_TO_POWER_OFF,
};

enum FaultFlag : uint8_t {
  FAULT_NONE = 0,
  FAULT_BAD_PACKET = 1 << 0,
  FAULT_BAD_COMMAND = 1 << 1,
  FAULT_POWER_TIMEOUT = 1 << 2,
  FAULT_EMERGENCY_STOP = 1 << 3,
};

Servo servos[SERVO_COUNT];

// I2C ISR mailbox. The callback only copies bytes; all hardware work happens
// in loop(). Newer values replace older unprocessed values for that channel.
volatile uint8_t receivedTargets[SERVO_COUNT] = {180, 0, 0};
volatile uint8_t receivedMask = 0;
volatile bool stopRequested = false;
volatile uint8_t receiveFaults = FAULT_NONE;

uint8_t targetPosition[SERVO_COUNT] = {180, 0, 0};
uint8_t currentPosition[SERVO_COUNT] = {180, 0, 0};
bool positionKnown[SERVO_COUNT] = {false, false, false};
uint8_t scheduledMask = 0;

MotionState motionState = IDLE_POWERED_OFF;
int8_t activeChannel = -1;
uint8_t faultFlags = FAULT_NONE;
unsigned long stateStartedAt = 0;
unsigned long lastMoveStepAt = 0;
unsigned long powerEnabledAt = 0;
unsigned long lastPowerOffAt = 0;

uint8_t relayLevel(bool powered) {
  const bool high = RELAY_ACTIVE_HIGH ? powered : !powered;
  return high ? HIGH : LOW;
}

void setRelay(bool powered) {
  digitalWrite(RELAY_PIN, relayLevel(powered));
}

uint16_t positionToPulse(uint8_t channel, uint8_t position) {
  const ServoConfig &config = SERVO_CONFIG[channel];
  uint8_t adjusted = position;
  if (config.reverse) adjusted = 180 - adjusted;

  const uint16_t span = config.maxPulseUs - config.minPulseUs;
  return config.minPulseUs +
         static_cast<uint16_t>((static_cast<uint32_t>(adjusted) * span) / 180UL);
}

void commandServo(uint8_t channel, uint8_t position) {
  servos[channel].writeMicroseconds(positionToPulse(channel, position));
}

void attachChannel(uint8_t channel) {
  const ServoConfig &config = SERVO_CONFIG[channel];
  servos[channel].attach(config.pin, config.minPulseUs, config.maxPulseUs);
  commandServo(channel, currentPosition[channel]);
}

void detachChannel(uint8_t channel) {
  servos[channel].detach();

  // Servo.detach() can otherwise leave a pin HIGH if it happens during a
  // pulse. Force LOW to avoid back-powering an unpowered servo via its signal.
  digitalWrite(SERVO_CONFIG[channel].pin, LOW);
  pinMode(SERVO_CONFIG[channel].pin, OUTPUT);
}

void detachAllChannels() {
  for (uint8_t channel = 0; channel < SERVO_COUNT; ++channel) {
    detachChannel(channel);
  }
}

void powerOffNow(unsigned long now, bool forgetActivePosition) {
  if (activeChannel >= 0 && forgetActivePosition) {
    positionKnown[activeChannel] = false;
  }

  detachAllChannels();
  setRelay(false);
  activeChannel = -1;
  motionState = IDLE_POWERED_OFF;
  lastPowerOffAt = now;
}

void emergencyStop(unsigned long now) {
  scheduledMask = 0;
  for (uint8_t channel = 0; channel < SERVO_COUNT; ++channel) {
    positionKnown[channel] = false;
  }
  faultFlags |= FAULT_EMERGENCY_STOP;
  powerOffNow(now, true);
}

void startChannel(uint8_t channel, unsigned long now) {
  activeChannel = static_cast<int8_t>(channel);
  scheduledMask &= static_cast<uint8_t>(~(1U << channel));

  // There is no position sensor. On the first command do not sweep from a
  // guessed position; command the requested point directly.
  if (!positionKnown[channel]) {
    currentPosition[channel] = targetPosition[channel];
  }

  // Preload the pulse while detached. After relay stabilization attach()
  // starts from this value instead of a default centre pulse.
  commandServo(channel, currentPosition[channel]);
  setRelay(true);
  powerEnabledAt = now;
  stateStartedAt = now;
  motionState = WAITING_FOR_POWER;
}

void startNextScheduledChannel(unsigned long now) {
  if (motionState != IDLE_POWERED_OFF || scheduledMask == 0) return;
  if (now - lastPowerOffAt < POWER_OFF_GAP_MS) return;

  for (uint8_t channel = 0; channel < SERVO_COUNT; ++channel) {
    if (scheduledMask & (1U << channel)) {
      startChannel(channel, now);
      return;
    }
  }
}

void serviceReceivedCommands(unsigned long now) {
  uint8_t mask;
  uint8_t values[SERVO_COUNT];
  bool stop;
  uint8_t newFaults;

  noInterrupts();
  mask = receivedMask;
  receivedMask = 0;
  for (uint8_t channel = 0; channel < SERVO_COUNT; ++channel) {
    values[channel] = receivedTargets[channel];
  }
  stop = stopRequested;
  stopRequested = false;
  newFaults = receiveFaults;
  receiveFaults = FAULT_NONE;
  interrupts();

  faultFlags |= newFaults;

  if (stop) {
    emergencyStop(now);
    return;
  }

  for (uint8_t channel = 0; channel < SERVO_COUNT; ++channel) {
    const uint8_t bit = 1U << channel;
    if (!(mask & bit)) continue;

    targetPosition[channel] = values[channel];

    if (activeChannel == static_cast<int8_t>(channel)) {
      scheduledMask &= static_cast<uint8_t>(~bit);
    } else if (!positionKnown[channel] ||
               currentPosition[channel] != targetPosition[channel]) {
      scheduledMask |= bit;
    } else {
      scheduledMask &= static_cast<uint8_t>(~bit);
    }
  }
}

void serviceMotion(unsigned long now) {
  if (motionState == IDLE_POWERED_OFF || activeChannel < 0) return;

  const uint8_t channel = static_cast<uint8_t>(activeChannel);

  if (now - powerEnabledAt >= MAX_POWER_ON_MS) {
    faultFlags |= FAULT_POWER_TIMEOUT;
    powerOffNow(now, true);
    return;
  }

  switch (motionState) {
    case IDLE_POWERED_OFF:
      break;

    case WAITING_FOR_POWER:
      if (now - stateStartedAt >= POWER_STABILIZE_MS) {
        attachChannel(channel);
        lastMoveStepAt = now;
        if (currentPosition[channel] == targetPosition[channel]) {
          stateStartedAt = now;
          motionState = SETTLING;
        } else {
          motionState = MOVING;
        }
      }
      break;

    case MOVING:
      if (now - lastMoveStepAt >= MOVE_STEP_MS) {
        lastMoveStepAt = now;

        if (currentPosition[channel] < targetPosition[channel]) {
          ++currentPosition[channel];
        } else if (currentPosition[channel] > targetPosition[channel]) {
          --currentPosition[channel];
        }

        commandServo(channel, currentPosition[channel]);

        if (currentPosition[channel] == targetPosition[channel]) {
          stateStartedAt = now;
          motionState = SETTLING;
        }
      }
      break;

    case SETTLING:
      if (currentPosition[channel] != targetPosition[channel]) {
        lastMoveStepAt = now;
        motionState = MOVING;
      } else if (now - stateStartedAt >= SETTLE_MS) {
        positionKnown[channel] = true;
        detachChannel(channel);
        stateStartedAt = now;
        motionState = WAITING_TO_POWER_OFF;
      }
      break;

    case WAITING_TO_POWER_OFF:
      if (currentPosition[channel] != targetPosition[channel]) {
        attachChannel(channel);
        lastMoveStepAt = now;
        motionState = MOVING;
      } else if (now - stateStartedAt >= SIGNAL_OFF_DELAY_MS) {
        setRelay(false);
        activeChannel = -1;
        motionState = IDLE_POWERED_OFF;
        lastPowerOffAt = now;
      }
      break;
  }
}

int8_t commandToChannel(uint8_t command) {
  if (command >= 'a' && command <= 'c') {
    return static_cast<int8_t>(command - 'a');
  }
  return -1;
}

void receiveFromMega(int byteCount) {
  if (byteCount < 2) {
    while (Wire.available()) Wire.read();
    receiveFaults |= FAULT_BAD_PACKET;
    return;
  }

  while (Wire.available() >= 2) {
    const uint8_t command = Wire.read();
    uint8_t value = Wire.read();

    if (command == 'x' || command == 'X') {
      stopRequested = true;
      continue;
    }

    const int8_t channel = commandToChannel(command);
    if (channel < 0) {
      receiveFaults |= FAULT_BAD_COMMAND;
      continue;
    }

    if (value > 180) value = 180;
    receivedTargets[channel] = value;
    receivedMask |= static_cast<uint8_t>(1U << channel);
  }

  if (Wire.available()) {
    while (Wire.available()) Wire.read();
    receiveFaults |= FAULT_BAD_PACKET;
  }
}

void sendStatusToMega() {
  uint8_t knownMask = 0;
  for (uint8_t channel = 0; channel < SERVO_COUNT; ++channel) {
    if (positionKnown[channel]) knownMask |= 1U << channel;
  }

  const uint8_t status[] = {
      'S',
      2,
      static_cast<uint8_t>(motionState),
      activeChannel < 0 ? 0xFF : static_cast<uint8_t>(activeChannel),
      faultFlags,
      knownMask,
      scheduledMask,
      currentPosition[0],
      currentPosition[1],
      currentPosition[2],
      targetPosition[0],
      targetPosition[1],
      targetPosition[2],
  };
  Wire.write(status, sizeof(status));
}

}  // namespace

void setup() {
  // Establish safe levels before making the pins outputs. There is no startup
  // servo movement: the first valid command determines the first position.
  digitalWrite(RELAY_PIN, relayLevel(false));
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);

  for (uint8_t channel = 0; channel < SERVO_COUNT; ++channel) {
    digitalWrite(SERVO_CONFIG[channel].pin, LOW);
    pinMode(SERVO_CONFIG[channel].pin, OUTPUT);
    commandServo(channel, currentPosition[channel]);
  }

#if SERVO_UNO_SERIAL_DEBUG
  Serial.begin(115200);
#endif

  Wire.begin(UNO_I2C_ADDRESS);
  Wire.onReceive(receiveFromMega);
  Wire.onRequest(sendStatusToMega);
  lastPowerOffAt = millis();
}

void loop() {
  const unsigned long now = millis();
  serviceReceivedCommands(now);
  serviceMotion(now);
  startNextScheduledChannel(now);
}
