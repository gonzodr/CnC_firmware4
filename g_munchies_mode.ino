/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// MUNCHIES ABDUCTION - VUK / PI PROTOKOLL
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
// Uzenetek (mindegyik ujsorral zarva):
//   Arduino -> Pi: MG_START,sid
//                 MG_INPUT,sid,seq,mask
//                 MG_ACK,sid
//                 MG_ABORT,sid,reason
//   Pi -> Arduino: MG_READY,sid
//                  MG_ALIVE,sid
//                  MG_PICKUP,sid,GOOD|BAD
//                  MG_COLLECTION,sid
//                  MG_DONE,sid,bonus
//
// mask bit0=bal flipper, bit1=jobb flipper, bit2=kilovo/vonosugar.
// Az INPUT allapotvaltozaskor azonnal, egyebkent 50 ms-onkent megy. A Pi
// heartbeatje nelkul 5 masodperc utan, abszolut 230 masodperc utan mindenkepp
// biztonsagosan visszaadjuk a golyot.

enum MunchiesModeState {
  MG_IDLE = 0,
  MG_WAIT_READY,
  MG_ACTIVE,
  MG_EJECTING
};

enum MunchiesLightState {
  MG_LIGHT_IDLE = 0,
  MG_LIGHT_GOOD,
  MG_LIGHT_BAD,
  MG_LIGHT_COLLECTION
};

uint8_t munchiesMode = MG_IDLE;
uint8_t munchiesLight = MG_LIGHT_IDLE;
uint16_t munchiesSession = 0;
uint16_t munchiesLastDoneSession = 0;
uint16_t munchiesInputSequence = 0;
uint8_t munchiesLastInputMask = 0xFF;
unsigned long munchiesModeStartedAt = 0;
unsigned long munchiesLastPiAt = 0;
unsigned long munchiesLastStartAt = 0;
unsigned long munchiesLastInputAt = 0;
unsigned long munchiesEjectAt = 0;
unsigned long munchiesLightAt = 0;
unsigned long munchiesPausedAt = 0;
boolean munchiesWaitForUfoClear = false;

const unsigned long MG_READY_TIMEOUT_MS = 3000UL;
const unsigned long MG_LINK_TIMEOUT_MS = 5000UL;
const unsigned long MG_ABSOLUTE_TIMEOUT_MS = 230000UL;
const unsigned long MG_INPUT_PERIOD_MS = 50UL;

boolean MunchiesOwnsGameLoop() {
  return munchiesMode != MG_IDLE;
}

// Kulso, biztonsagos megszakitas a fizikai szervizmodhoz. A VUK-kidobast
// maga a service abort kezeli, ez a rutin csak a minijatek allapotgepet
// engedi el es ertesiti a GUI-t, ha epp aktiv volt.
void AbortMunchiesForService() {
  if (munchiesMode != MG_IDLE) SendMunchiesAbort("SERVICE");
  munchiesMode = MG_IDLE;
  munchiesLight = MG_LIGHT_IDLE;
  munchiesWaitForUfoClear = false;
  digitalWrite(ufoCoil, LOW);
}

uint16_t MunchiesUfoReleaseThreshold() {
  uint16_t releaseAt = analogThreshold[5] + 40U;
  return releaseAt > 1023U ? 1023U : releaseAt;
}

boolean MunchiesWaitingForUfoClear() {
  if (!munchiesWaitForUfoClear) return false;
  if (ufoanalog >= MunchiesUfoReleaseThreshold()) {
    munchiesWaitForUfoClear = false;
    return false;
  }
  return true;
}

void SendMunchiesStart() {
  Serial.print("MG_START,");
  Serial.println(munchiesSession);
  munchiesLastStartAt = millis();
}

void SendMunchiesAbort(const char* reason) {
  Serial.print("MG_ABORT,");
  Serial.print(munchiesSession);
  Serial.print(',');
  Serial.println(reason);
}

void StartMunchiesLight(uint8_t state) {
  // A collection-complete hullamot egy kozben beesett pickup ne vagja felbe.
  if (munchiesLight == MG_LIGHT_COLLECTION
      && millis() - munchiesLightAt < 1000UL
      && state != MG_LIGHT_COLLECTION) {
    return;
  }
  munchiesLight = state;
  munchiesLightAt = millis();
}

void StartMunchiesMode() {
  if (munchiesMode != MG_IDLE) return;

  munchiesSession++;
  if (munchiesSession == 0) munchiesSession = 1; // 0 a legacy/nincs-session
  munchiesMode = MG_WAIT_READY;
  munchiesModeStartedAt = millis();
  munchiesPausedAt = munchiesModeStartedAt;
  munchiesLastPiAt = 0;
  munchiesInputSequence = 0;
  munchiesLastInputMask = 0xFF;
  munchiesLight = MG_LIGHT_IDLE;
  munchiesWaitForUfoClear = false;
  effect = LOW;
  effectID = 0;
  StopLightTest();

  // A fizikai flipper ne mozogjon a videojatek gombnyomasaira.
  digitalWrite(leftFlipperBat, LOW);
  digitalWrite(rightFlipperBat, LOW);
  digitalWrite(ufoCoil, LOW);
  wTrig.trackPause(TRK_THEME);
  SendMunchiesStart();
}

void BeginMunchiesEject() {
  if (munchiesMode == MG_EJECTING || munchiesMode == MG_IDLE) return;

  unsigned long now = millis();
  // A minijatek ne egye meg a meg aktiv ballsave idejet.
  if (ballsaversw == HIGH) ballsavetimer += now - munchiesPausedAt;
  // Ugyanaz az 5 mp-es VUK-kidobasi vedelem jar a minijatek utan is.
  EnsureBallSave(UFO_EJECT_BALL_SAVE_MS);

  munchiesMode = MG_EJECTING;
  munchiesEjectAt = now;
  digitalWrite(leftFlipperBat, LOW);
  digitalWrite(rightFlipperBat, LOW);
  digitalWrite(ufoCoil, HIGH);
  wTrig.trackResume(TRK_THEME);
}

uint8_t ReadMunchiesInputMask() {
  uint8_t mask = 0;
  if (SimDigitalRead(leftFlipperButton) == LOW) mask |= 0x01;
  if (SimDigitalRead(rightflipperButton) == LOW) mask |= 0x02;
  if (SimDigitalRead(ballShooterButton) == LOW) mask |= 0x04;
  return mask;
}

void SendMunchiesInput(uint8_t mask) {
  Serial.print("MG_INPUT,");
  Serial.print(munchiesSession);
  Serial.print(',');
  Serial.print(munchiesInputSequence++);
  Serial.print(',');
  Serial.println(mask);
  munchiesLastInputMask = mask;
  munchiesLastInputAt = millis();
}

boolean ParseMunchiesSession(const char* token, uint16_t &session) {
  if (token == NULL || *token == '\0') return false;
  char* endp;
  unsigned long value = strtoul(token, &endp, 10);
  if (*endp != '\0' || value == 0 || value > 65535UL) return false;
  session = (uint16_t)value;
  return true;
}

void HandleMunchiesCommand(const char* command) {
  char work[64];
  strncpy(work, command, sizeof(work) - 1);
  work[sizeof(work) - 1] = '\0';

  char* verb = strtok(work, ",");
  char* sessionToken = strtok(NULL, ",");
  uint16_t session;
  if (verb == NULL || !ParseMunchiesSession(sessionToken, session)) return;

  // Egy mar feldolgozott DONE ujrakuldese csak uj ACK-ot kap; pontot nem.
  if (strcmp(verb, "MG_DONE") == 0 && session == munchiesLastDoneSession) {
    Serial.print("MG_ACK,");
    Serial.println(session);
    return;
  }

  if (session != munchiesSession) return;

  if (strcmp(verb, "MG_READY") == 0 && munchiesMode == MG_WAIT_READY) {
    munchiesMode = MG_ACTIVE;
    munchiesModeStartedAt = millis();
    munchiesLastPiAt = munchiesModeStartedAt;
    SendMunchiesInput(ReadMunchiesInputMask());
    return;
  }

  // A Mega a READY megjovetele elott 500 ms-onkent ujrakuldi a START-ot,
  // ezert a GUI valasza is ismetlodhet. Aktiv sessionben ez nem uj inditas,
  // hanem biztos bizonyitek arra, hogy a Pi-kapcsolat el: frissitse ugyanazt
  // a watchdogot, mint az MG_ALIVE.
  if (strcmp(verb, "MG_READY") == 0 && munchiesMode == MG_ACTIVE) {
    munchiesLastPiAt = millis();
    return;
  }

  if (strcmp(verb, "MG_BUSY") == 0 && munchiesMode == MG_WAIT_READY) {
    SendMunchiesAbort("GUI_BUSY");
    BeginMunchiesEject();
    return;
  }

  if (strcmp(verb, "MG_ALIVE") == 0 && munchiesMode == MG_ACTIVE) {
    munchiesLastPiAt = millis();
    return;
  }

  if (strcmp(verb, "MG_PICKUP") == 0 && munchiesMode == MG_ACTIVE) {
    munchiesLastPiAt = millis();
    char* kind = strtok(NULL, ",");
    if (kind != NULL && strcmp(kind, "GOOD") == 0) {
      StartMunchiesLight(MG_LIGHT_GOOD);
    }
    else if (kind != NULL && strcmp(kind, "BAD") == 0) {
      StartMunchiesLight(MG_LIGHT_BAD);
    }
    return;
  }

  if (strcmp(verb, "MG_COLLECTION") == 0 && munchiesMode == MG_ACTIVE) {
    munchiesLastPiAt = millis();
    StartMunchiesLight(MG_LIGHT_COLLECTION);
    return;
  }

  if (strcmp(verb, "MG_DONE") == 0 && munchiesMode == MG_ACTIVE) {
    char* bonusToken = strtok(NULL, ",");
    if (bonusToken == NULL || *bonusToken == '\0') return;
    char* endp;
    unsigned long earned = strtoul(bonusToken, &endp, 10);
    if (*endp != '\0') return;

    Score(earned, 0);
    munchiesLastDoneSession = session;
    Serial.print("MG_ACK,");
    Serial.println(session);
    BeginMunchiesEject();
  }
}

void MunchiesUpdate() {
  unsigned long now = millis();
  digitalWrite(leftFlipperBat, LOW);
  digitalWrite(rightFlipperBat, LOW);

  if (munchiesMode == MG_WAIT_READY) {
    if (now - munchiesLastStartAt >= 500UL) SendMunchiesStart();
    if (now - munchiesModeStartedAt >= MG_READY_TIMEOUT_MS) {
      SendMunchiesAbort("READY_TIMEOUT");
      BeginMunchiesEject();
    }
    return;
  }

  if (munchiesMode == MG_ACTIVE) {
    uint8_t mask = ReadMunchiesInputMask();
    if (mask != munchiesLastInputMask || now - munchiesLastInputAt >= MG_INPUT_PERIOD_MS) {
      SendMunchiesInput(mask);
    }
    if (now - munchiesLastPiAt >= MG_LINK_TIMEOUT_MS) {
      SendMunchiesAbort("LINK_TIMEOUT");
      BeginMunchiesEject();
    }
    else if (now - munchiesModeStartedAt >= MG_ABSOLUTE_TIMEOUT_MS) {
      SendMunchiesAbort("ABS_TIMEOUT");
      BeginMunchiesEject();
    }
    return;
  }

  if (munchiesMode == MG_EJECTING) {
    if (now - munchiesEjectAt >= 50UL) digitalWrite(ufoCoil, LOW);
    // Ha az elso loket utan meg mindig bent ul a golyo, egyetlen masodik,
    // ugyanugy tekercsor altal vedett 50 ms-os impulzust kap.
    if (now - munchiesEjectAt >= 900UL && now - munchiesEjectAt < 950UL) {
      digitalWrite(ufoCoil, HIGH);
    }
    if (now - munchiesEjectAt >= 950UL) digitalWrite(ufoCoil, LOW);
    if (now - munchiesEjectAt >= 650UL) {
      int sensor = AnalogSensorReadStable(PIN_A5);
      if (sensor < MunchiesUfoReleaseThreshold() && now - munchiesEjectAt < 1500UL) return;
      digitalWrite(ufoCoil, LOW);
      munchiesMode = MG_IDLE;
      munchiesLight = MG_LIGHT_IDLE;
      ufoshoot = 0;
      ufosw = 0;
      ufoInactivesw = 1;
      ufoInactiveTimer = now;
      ufoanalog = sensor;
      munchiesWaitForUfoClear = (sensor < MunchiesUfoReleaseThreshold());
      initlight = HIGH;
      Initlights();
    }
  }
}

uint8_t MunchiesTriangle(unsigned long phase, unsigned long halfPeriod) {
  unsigned long full = halfPeriod * 2UL;
  phase %= full;
  unsigned long value = (phase <= halfPeriod) ? phase : (full - phase);
  return (uint8_t)((value * 255UL) / halfPeriod);
}

void DrawMunchiesDarkBase(unsigned long now) {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  uint8_t breathe = 3 + scale8(sin8((uint8_t)(now >> 3)), 7);
  const uint8_t ambient[] = {
    LED_CNC_AMBIENT, LED_LEFT_RAMP_AMBIENT, LED_FISHTANK_AMBIENT,
    LED_RIGHT_RAMP_AMBIENT, LED_GATE3_AMBIENT, LED_GATE32_AMBIENT,
    LED_GATE21_AMBIENT, LED_GATE1_AMBIENT, LED_CAR_AMBIENT
  };
  for (uint8_t i = 0; i < sizeof(ambient); i++) {
    leds[ambient[i]] = CRGB(0, breathe, breathe >> 1);
  }
  uint8_t ufoPulse = 10 + scale8(sin8((uint8_t)(now >> 2)), 22);
  leds[LED_UFO_ARROW_1] = CRGB(0, ufoPulse, 0);
  leds[LED_UFO_ARROW_2] = CRGB(0, ufoPulse, 0);
}

void RunMunchiesLights() {
  if (!MunchiesOwnsGameLoop()) return;
  unsigned long now = millis();
  DrawMunchiesDarkBase(now);
  unsigned long age = now - munchiesLightAt;

  if (munchiesLight == MG_LIGHT_GOOD) {
    if (age >= 360UL) { munchiesLight = MG_LIGHT_IDLE; return; }
    uint8_t bri = MunchiesTriangle(age, 180UL);
    bri = scale8(bri, 230);
    fill_solid(leds, NUM_LEDS, CRGB(0, bri, 0));
  }
  else if (munchiesLight == MG_LIGHT_BAD) {
    if (age >= 500UL) { munchiesLight = MG_LIGHT_IDLE; return; }
    uint8_t bri = 0;
    if (age < 150UL) bri = MunchiesTriangle(age, 75UL);
    else if (age >= 250UL && age < 420UL) bri = MunchiesTriangle(age - 250UL, 85UL);
    bri = scale8(bri, 235);
    fill_solid(leds, NUM_LEDS, CRGB(bri, 0, 0));
  }
  else if (munchiesLight == MG_LIGHT_COLLECTION) {
    if (age >= 1000UL) { munchiesLight = MG_LIGHT_IDLE; return; }
    // Harom, egymast kergeto zold hullam az egesz LED-terkepen. A sin8
    // miatt folyamatos, nincs frame-enkenti fenyero-ugras.
    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      uint8_t wave = sin8((uint8_t)(i * 17U - (uint8_t)(age >> 1)));
      uint8_t envelope = (age < 180UL)
        ? (uint8_t)((age * 255UL) / 180UL)
        : (age > 760UL ? (uint8_t)(((1000UL - age) * 255UL) / 240UL) : 255);
      uint8_t bri = scale8(scale8(wave, envelope), 245);
      leds[i] = CRGB(0, bri, scale8(bri, 28));
    }
  }
}
