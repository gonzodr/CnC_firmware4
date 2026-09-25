#include <Servo.h>
#include <Wire.h>

// Reconstructed from the ATmega328P flash dump saved on 2026-09-24.
// This is a readable, functionally equivalent reconstruction; the original
// variable names and comments cannot be recovered from the compiled binary.

constexpr uint8_t UNO_I2C_ADDRESS = 8;
constexpr uint8_t SERVO_1_PIN = 4;
constexpr uint8_t SERVO_2_PIN = 5;
constexpr uint8_t SERVO_3_PIN = 6;
constexpr uint8_t RELAY_PIN = 7;

Servo servo1;
Servo servo2;
Servo servo3;

String receivedCommand;
volatile int receivedValue = 160;
volatile bool commandComplete = false;

void receiveEvent(int byteCount) {
  (void)byteCount;

  // The Mega sends the command characters followed by one raw value byte.
  while (Wire.available() > 1) {
    char c = static_cast<char>(Wire.read());
    receivedCommand += c;
    Serial.print(c);
  }

  if (Wire.available()) {
    receivedValue = Wire.read();
    Serial.println(receivedValue);
    commandComplete = true;
  }
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  delay(800);

  Wire.begin(UNO_I2C_ADDRESS);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);
  servo3.attach(SERVO_3_PIN);

  // These values and the 800 ms delay are present in the recovered binary.
  // Servo.write() constrains 190 degrees to 180 degrees.
  servo1.write(190);
  servo2.write(0);
  servo3.write(0);
  delay(800);

  servo1.detach();
  servo2.detach();
  servo3.detach();
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  if (!commandComplete) {
    return;
  }

  if (receivedCommand == "a") {
    int target = receivedValue;

    digitalWrite(RELAY_PIN, HIGH);
    servo1.attach(SERVO_1_PIN);
    servo1.write(target);
    delay(250);
    servo1.detach();
    digitalWrite(RELAY_PIN, LOW);
  }

  receivedCommand = "";
  commandComplete = false;
}
