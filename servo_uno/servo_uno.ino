#include <Wire.h>

// CnC weed-meter controller
// Arduino Uno I2C slave + PCA9685 servo shield + switched SG90 supply.
// Mega packet: ['a'][weedmeter], where weedmeter is 0..180.

// -------------------------- Hardware configuration --------------------------
constexpr uint8_t UNO_I2C_ADDRESS = 8;
constexpr uint8_t PCA9685_ADDRESS = 0x40;
constexpr uint8_t SERVO_CHANNEL = 0;

constexpr uint8_t RELAY_PIN = 7;
constexpr bool RELAY_ACTIVE_LOW = true;

// Set true if the pointer moves in the wrong direction.
constexpr bool REVERSE_POINTER = false;

// Mechanical safe range. Tune these without ever driving into a hard stop.
constexpr uint8_t POINTER_MIN_DEG = 15;
constexpr uint8_t POINTER_MAX_DEG = 165;

// Conservative SG90 pulse range. Narrow it if the mechanism reaches an end stop.
constexpr uint16_t SERVO_MIN_US = 700;
constexpr uint16_t SERVO_MAX_US = 2300;
constexpr uint16_t SERVO_FREQUENCY_HZ = 50;

constexpr unsigned long POWER_STABILIZE_MS = 150;
constexpr unsigned long MOVE_STEP_MS = 15;
constexpr unsigned long SETTLE_MS = 450;
constexpr unsigned long SIGNAL_OFF_DELAY_MS = 60;

// ------------------------------- Runtime state ------------------------------
enum MotionState : uint8_t {
  IDLE_POWERED_OFF,
  WAITING_FOR_POWER,
  MOVING,
  SETTLING,
  WAITING_TO_POWER_OFF
};

volatile uint8_t receivedMeter = 180;
volatile bool receivedMeterPending = false;

MotionState motionState = IDLE_POWERED_OFF;
unsigned long stateStartedAt = 0;
unsigned long lastMoveStepAt = 0;

uint8_t currentMeter = 180;
uint8_t targetMeter = 180;
uint8_t lastCompletedMeter = 255; // force one positioning cycle after boot

// ------------------------------ PCA9685 driver ------------------------------
constexpr uint8_t PCA_MODE1 = 0x00;
constexpr uint8_t PCA_MODE2 = 0x01;
constexpr uint8_t PCA_LED0_ON_L = 0x06;
constexpr uint8_t PCA_PRESCALE = 0xFE;

void pcaWrite8(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(PCA9685_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t pcaRead8(uint8_t reg) {
  Wire.beginTransmission(PCA9685_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(PCA9685_ADDRESS, static_cast<uint8_t>(1));
  return Wire.available() ? Wire.read() : 0;
}

void pcaSetChannel(uint8_t channel, uint16_t onCount, uint16_t offCount) {
  const uint8_t reg = PCA_LED0_ON_L + 4 * channel;
  Wire.beginTransmission(PCA9685_ADDRESS);
  Wire.write(reg);
  Wire.write(onCount & 0xFF);
  Wire.write((onCount >> 8) & 0x0F);
  Wire.write(offCount & 0xFF);
  Wire.write((offCount >> 8) & 0x1F); // bit 12 is FULL OFF
  Wire.endTransmission();
}

void servoSignalOff() {
  // FULL OFF is preferable to continuously commanding the last position.
  pcaSetChannel(SERVO_CHANNEL, 0, 4096);
}

void pcaSetFrequency(uint16_t frequencyHz) {
  // prescale = round(25 MHz / (4096 * frequency)) - 1
  uint32_t denominator = 4096UL * frequencyHz;
  uint8_t prescale = static_cast<uint8_t>((25000000UL + denominator / 2) /
                                           denominator - 1);
  uint8_t oldMode = pcaRead8(PCA_MODE1);
  pcaWrite8(PCA_MODE1, (oldMode & 0x7F) | 0x10); // sleep
  pcaWrite8(PCA_PRESCALE, prescale);
  pcaWrite8(PCA_MODE1, oldMode);
  delay(5);
  pcaWrite8(PCA_MODE1, oldMode | 0xA1); // restart + auto-increment + all-call
}

void pcaBegin() {
  pcaWrite8(PCA_MODE1, 0x00);
  pcaWrite8(PCA_MODE2, 0x04); // totem-pole output
  delay(10);
  pcaSetFrequency(SERVO_FREQUENCY_HZ);
  servoSignalOff();
}

// ------------------------------- Servo logic --------------------------------
void setRelay(bool powered) {
  bool pinHigh = RELAY_ACTIVE_LOW ? !powered : powered;
  digitalWrite(RELAY_PIN, pinHigh ? HIGH : LOW);
}

uint8_t meterToAngle(uint8_t meter) {
  meter = constrain(meter, 0, 180);
  if (REVERSE_POINTER) meter = 180 - meter;
  return map(meter, 0, 180, POINTER_MIN_DEG, POINTER_MAX_DEG);
}

uint16_t angleToPcaCount(uint8_t angle) {
  uint16_t pulseUs = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  // One 50 Hz PCA9685 period is 20000 us and contains 4096 counts.
  return static_cast<uint16_t>((static_cast<uint32_t>(pulseUs) * 4096UL) /
                               (1000000UL / SERVO_FREQUENCY_HZ));
}

void commandMeter(uint8_t meter) {
  pcaSetChannel(SERVO_CHANNEL, 0, angleToPcaCount(meterToAngle(meter)));
}

void startPositioning(uint8_t meter, unsigned long now) {
  targetMeter = constrain(meter, 0, 180);
  servoSignalOff();
  setRelay(true);
  stateStartedAt = now;
  motionState = WAITING_FOR_POWER;
}

void serviceServo() {
  unsigned long now = millis();

  noInterrupts();
  bool hasNewValue = receivedMeterPending;
  uint8_t newValue = receivedMeter;
  receivedMeterPending = false;
  interrupts();

  if (hasNewValue) {
    newValue = constrain(newValue, 0, 180);
    if (motionState == IDLE_POWERED_OFF) {
      if (newValue != lastCompletedMeter) startPositioning(newValue, now);
    } else {
      // Coalesce updates received while the pointer is already moving.
      targetMeter = newValue;
    }
  }

  switch (motionState) {
    case IDLE_POWERED_OFF:
      break;

    case WAITING_FOR_POWER:
      if (now - stateStartedAt >= POWER_STABILIZE_MS) {
        // The previous physical position is our safest starting estimate.
        if (lastCompletedMeter == 255) currentMeter = targetMeter;
        commandMeter(currentMeter);
        lastMoveStepAt = now;
        motionState = MOVING;
      }
      break;

    case MOVING:
      if (now - lastMoveStepAt >= MOVE_STEP_MS) {
        lastMoveStepAt = now;
        if (currentMeter < targetMeter) ++currentMeter;
        else if (currentMeter > targetMeter) --currentMeter;
        commandMeter(currentMeter);

        if (currentMeter == targetMeter) {
          stateStartedAt = now;
          motionState = SETTLING;
        }
      }
      break;

    case SETTLING:
      // A changed target restarts smooth movement without power-cycling.
      if (currentMeter != targetMeter) {
        lastMoveStepAt = now;
        motionState = MOVING;
      } else if (now - stateStartedAt >= SETTLE_MS) {
        lastCompletedMeter = currentMeter;
        servoSignalOff();
        stateStartedAt = now;
        motionState = WAITING_TO_POWER_OFF;
      }
      break;

    case WAITING_TO_POWER_OFF:
      if (currentMeter != targetMeter) {
        commandMeter(currentMeter);
        lastMoveStepAt = now;
        motionState = MOVING;
      } else if (now - stateStartedAt >= SIGNAL_OFF_DELAY_MS) {
        setRelay(false);
        motionState = IDLE_POWERED_OFF;
      }
      break;
  }
}

// ------------------------------- I2C receive ---------------------------------
void receiveFromMega(int byteCount) {
  if (byteCount < 2 || !Wire.available()) {
    while (Wire.available()) Wire.read();
    return;
  }

  char command = static_cast<char>(Wire.read());
  uint8_t value = Wire.read();
  while (Wire.available()) Wire.read();

  if (command == 'a') {
    receivedMeter = constrain(value, 0, 180);
    receivedMeterPending = true;
  }
}

void setup() {
  // Set the safe relay state before initializing anything else.
  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);

  Wire.begin(UNO_I2C_ADDRESS);
  Wire.onReceive(receiveFromMega);
  pcaBegin();

  // Position once after startup even if the Mega sends the default 180.
  receivedMeter = 180;
  receivedMeterPending = true;
}

void loop() {
  serviceServo();
}
