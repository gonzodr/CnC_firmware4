// MULTIBALL MAYHEM challenge coordinator.
// The existing weed multiballs remain the only physical multiball engine.

enum MayhemState : uint8_t {
  MAYHEM_IDLE = 0,
  MAYHEM_WAIT_TROUGH,
  MAYHEM_READY,
  MAYHEM_STAGE,
  MAYHEM_SETTLE,
  MAYHEM_COMPLETE
};

MayhemState mayhemState = MAYHEM_IDLE;
uint8_t mayhemStage = 0;
uint8_t mayhemJackpots = 0;
uint8_t mayhemCarrySeconds = 0;
uint8_t mayhemLastCountdown = 255;
unsigned long mayhemStateStartedAt = 0;
unsigned long mayhemStageDurationMs = 0;

// Central balancing parameters: safe to tune without touching the lifecycle.
const uint32_t MAYHEM_BASE_STAGE_MS = 25000UL;
const uint8_t MAYHEM_MAX_BONUS_SECONDS = 10;
const uint8_t MAYHEM_REQUIRED_JACKPOTS[4] PROGMEM = { 3, 4, 5, 6 };

boolean MayhemOwnsGameLifecycle() {
  return runningGameMode == GAME_MULTIBALL_MAYHEM && mayhemState != MAYHEM_IDLE;
}

boolean MayhemTroughStable() {
  return shoot == 0 && BIS == 5 &&
         bisFiveReads >= BALL_DRAIN_CONFIRM_READS &&
         millis() - bisFiveSince >= BALL_DRAIN_CONFIRM_MS;
}

uint8_t MayhemRequiredJackpots() {
  return pgm_read_byte(MAYHEM_REQUIRED_JACKPOTS + mayhemStage);
}

void SendMayhemReady(uint8_t secondsLeft) {
  Serial.print(F("MAYHEM_READY,"));
  Serial.print(player);
  Serial.print(',');
  Serial.println(secondsLeft);
}

void SendMayhemStage() {
  Serial.print(F("MAYHEM_STAGE,"));
  Serial.print(player);
  Serial.print(',');
  Serial.print(mayhemStage + 1);
  Serial.print(',');
  Serial.print(mayhemStageDurationMs / 1000UL);
  Serial.print(',');
  Serial.println(MayhemRequiredJackpots());
}

void SendMayhemProgress() {
  const uint8_t required = MayhemRequiredJackpots();
  Serial.print(F("MAYHEM_PROGRESS,"));
  Serial.print(mayhemStage + 1);
  Serial.print(',');
  Serial.print(mayhemJackpots);
  Serial.print(',');
  Serial.print(required);
  Serial.print(',');
  Serial.println(mayhemJackpots >= required ? 1 : 0);
}

void StopMayhemMultiball() {
  wTrig.trackPause(TRK_MUS_STRAWBERRY2);
  wTrig.trackLoop(TRK_MUS_STRAWBERRY2, 0);
  wTrig.trackPause(TRK_MUS_ROCKFIGHT);
  wTrig.trackLoop(TRK_MUS_ROCKFIGHT, 0);
  wTrig.trackPause(TRK_MUS_THAI_STICK);
  wTrig.trackLoop(TRK_MUS_THAI_STICK, 0);
  wTrig.trackPause(TRK_MUS_LABRADOR);
  wTrig.trackLoop(TRK_MUS_LABRADOR, 0);
  wTrig.trackResume(TRK_THEME);
  multiball = 0;
  multiloopsw = 0;
  highLoopArmT = 0;
  BrdgLowActive = LOW;
  BrdgHighActive = LOW;
  BIP = 1;
  ballsaversw = LOW;
  sidelaneBallsaverSw = LOW;
  maxBallSw = LOW;
}

void BeginMayhemPlayer() {
  mayhemStage = 0;
  mayhemJackpots = 0;
  mayhemCarrySeconds = 0;
  mayhemLastCountdown = 255;
  ball = 1;
  bonus = 0;
  bonusx = 0;
  firstplay = LOW;
  mayhemState = MAYHEM_WAIT_TROUGH;
  mayhemStateStartedAt = millis();
  Serial.print(F("MAYHEM_PLAYER,"));
  Serial.println(player);
}

void StartMayhemChallenge() {
  player = 1;
  shoot = 0;
  BIP = 1;
  MIV(LOW);
  BeginMayhemPlayer();
}

void StartMayhemStage() {
  mayhemJackpots = 0;
  mayhemStageDurationMs = MAYHEM_BASE_STAGE_MS +
                          (uint32_t)mayhemCarrySeconds * 1000UL;
  mayhemCarrySeconds = 0;
  mayhemState = MAYHEM_STAGE;
  mayhemStateStartedAt = millis();
  firstplay = LOW;
  SendMayhemStage();
  SendMayhemProgress();
  StartWeedMultiball(mayhemStage, WEED_MB_FROM_CHALLENGE);
  // Refreshed every frame, so a drain can never end the timed stage.
  ballsaversw = HIGH;
  ballsavetimer = millis();
  ballsavetime = MAYHEM_BASE_STAGE_MS + 15000UL;
}

void FinishMayhemStage() {
  const uint8_t required = MayhemRequiredJackpots();
  if (mayhemStage < 3 && mayhemJackpots > required) {
    uint8_t excess = mayhemJackpots - required;
    mayhemCarrySeconds = min(excess, MAYHEM_MAX_BONUS_SECONDS);
  }
  else {
    mayhemCarrySeconds = 0;
  }
  Serial.print(F("MAYHEM_STAGE_END,"));
  Serial.print(mayhemStage + 1);
  Serial.print(',');
  Serial.print(mayhemJackpots);
  Serial.print(',');
  Serial.println(mayhemCarrySeconds);
  StopMayhemMultiball();
  mayhemState = MAYHEM_SETTLE;
  mayhemStateStartedAt = millis();
}

void FinishMayhemPlayer() {
  Serial.print(F("MAYHEM_RESULT,"));
  Serial.print(player);
  Serial.print(',');
  Serial.println(score[player]);

  if (player < numofplayers) {
    player++;
    BeginMayhemPlayer();
    SendData();
    return;
  }

  uint8_t winner = 1;
  for (uint8_t p = 2; p <= numofplayers; p++) {
    if (score[p] > score[winner]) winner = p;
  }
  Serial.print(F("MAYHEM_FINISH,"));
  Serial.println(winner);
  // Keep lifecycle ownership until the GUI finishes results/name entry.
  // intmon=2 prevents future gameplay loops; COMPLETE also protects the
  // remainder of this already-running loop from a normal End/ball launch.
  mayhemState = MAYHEM_COMPLETE;
  intmon = 2;
}

void MayhemJackpotCollected() {
  if (mayhemState != MAYHEM_STAGE) return;
  if (mayhemJackpots < 255) mayhemJackpots++;
  SendMayhemProgress();
  if (mayhemJackpots == MayhemRequiredJackpots()) {
    Serial.println(F("MAYHEM_SUPER_LIT"));
  }
}

void MayhemUpdate() {
  if (!MayhemOwnsGameLifecycle()) return;
  const unsigned long now = millis();

  if (mayhemState == MAYHEM_WAIT_TROUGH) {
    MIV(LOW);
    if (MayhemTroughStable()) {
      mayhemState = MAYHEM_READY;
      mayhemStateStartedAt = now;
      mayhemLastCountdown = 5;
      SendMayhemReady(5);
    }
    return;
  }

  if (mayhemState == MAYHEM_READY) {
    uint8_t elapsed = (uint8_t)((now - mayhemStateStartedAt) / 1000UL);
    uint8_t remaining = elapsed >= 5 ? 0 : 5 - elapsed;
    if (remaining != mayhemLastCountdown) {
      mayhemLastCountdown = remaining;
      SendMayhemReady(remaining);
    }
    if (elapsed >= 5) StartMayhemStage();
    return;
  }

  if (mayhemState == MAYHEM_STAGE) {
    ballsaversw = HIGH;
    ballsavetimer = now;
    if (now - mayhemStateStartedAt >= mayhemStageDurationMs) {
      FinishMayhemStage();
    }
    return;
  }

  if (mayhemState == MAYHEM_SETTLE) {
    MIV(LOW);
    if (!MayhemTroughStable()) return;
    if (mayhemStage < 3) {
      mayhemStage++;
      StartMayhemStage();
    }
    else {
      FinishMayhemPlayer();
    }
  }
}

void MayhemEnforceSafety() {
  if (!MayhemOwnsGameLifecycle()) return;
  if (mayhemState == MAYHEM_WAIT_TROUGH || mayhemState == MAYHEM_READY ||
      mayhemState == MAYHEM_SETTLE || mayhemState == MAYHEM_COMPLETE) {
    digitalWrite(leftFlipperBat, LOW);
    digitalWrite(rightFlipperBat, LOW);
  }
}
