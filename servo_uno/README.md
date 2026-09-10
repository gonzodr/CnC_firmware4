# CnC weed-meter Servo Uno

Az Arduino Uno a Mega I2C slave eszkoze a `0x08` cimen. A Mega ket bajtot
kuld: az ASCII `a` parancsot, majd a `0..180` kozotti weedmeter-erteket.

## Alap bekotes

- Uno A4 / SDA -> Mega D20 / SDA
- Uno A5 / SCL -> Mega D21 / SCL
- Uno GND -> Mega GND -> kulso 5 V szervotap GND
- PCA9685 shield V+ -> kulso, stabil 5 V / legalabb 2 A
- SG90 -> PCA9685 channel 0
- relemodul IN -> Uno D7
- a rele csak a szervo V+ agat szakitsa meg, a PCA9685 logikai tapjat ne

Az SG90-et ne az Uno 5 V pinjerol taplald.

## Amit elso feltoltes elott ellenorizni kell

A `servo_uno.ino` elejen allithato:

- `RELAY_PIN`
- `RELAY_ACTIVE_LOW`
- `SERVO_CHANNEL`
- `REVERSE_POINTER`
- `POINTER_MIN_DEG` es `POINTER_MAX_DEG`

Eloszor lekapcsolt mechanikaval vagy levett mutatoval ellenorizd az iranyt es
a vegallasokat. A kod csak valodi ertekvaltozaskor mozgat, utana PCA9685
FULL-OFF allapotba teszi a jelet, majd lekapcsolja a szervo tapjat.
