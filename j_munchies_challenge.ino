// Standalone Munchies Challenge lifecycle. The minigame itself remains in
// g_munchies_mode.ino; this coordinator only sequences players and results.

enum StandaloneMunchiesState : uint8_t {
  SMC_IDLE = 0,
  SMC_READY,
  SMC_RUNNING,
  SMC_COMPLETE
};

StandaloneMunchiesState standaloneMunchiesState = SMC_IDLE;
uint8_t standaloneMunchiesLastCountdown = 255;
unsigned long standaloneMunchiesStateAt = 0;
const uint8_t STANDALONE_MUNCHIES_COUNTDOWN_SECONDS = 3;

boolean StandaloneMunchiesOwnsGameLoop() {
  return runningGameMode == GAME_MUNCHIES &&
         standaloneMunchiesState != SMC_IDLE;
}

void SendStandaloneMunchiesReady(uint8_t secondsLeft) {
  Serial.print(F("MUNCHIES_READY,"));
  Serial.print(player);
  Serial.print(',');
  Serial.println(secondsLeft);
}

void BeginStandaloneMunchiesPlayer() {
  firstplay = LOW;
  shoot = 0;
  BIP = 1;
  bonus = 0;
  bonusx = 0;
  standaloneMunchiesState = SMC_READY;
  standaloneMunchiesStateAt = millis();
  standaloneMunchiesLastCountdown = STANDALONE_MUNCHIES_COUNTDOWN_SECONDS;
  SendStandaloneMunchiesReady(standaloneMunchiesLastCountdown);
}

void StartStandaloneMunchiesChallenge() {
  player = 1;
  ball = 1;
  BeginStandaloneMunchiesPlayer();
}

void FinishStandaloneMunchiesChallenge() {
  uint8_t winner = 1;
  for (uint8_t p = 2; p <= numofplayers; p++) {
    if (score[p] > score[winner]) winner = p;
  }
  Serial.print(F("MUNCHIES_FINISH,"));
  Serial.println(winner);
  standaloneMunchiesState = SMC_COMPLETE;
  intmon = 2;
}

void StandaloneMunchiesRunFinished() {
  if (standaloneMunchiesState != SMC_RUNNING) return;
  Serial.print(F("MUNCHIES_RESULT,"));
  Serial.print(player);
  Serial.print(',');
  Serial.println(score[player]);

  if (player < numofplayers) {
    player++;
    BeginStandaloneMunchiesPlayer();
    SendData();
  }
  else {
    FinishStandaloneMunchiesChallenge();
  }
}

void StandaloneMunchiesUpdate() {
  if (!StandaloneMunchiesOwnsGameLoop()) return;
  if (standaloneMunchiesState != SMC_READY) return;

  const unsigned long now = millis();
  uint8_t elapsed = (uint8_t)((now - standaloneMunchiesStateAt) / 1000UL);
  uint8_t remaining = elapsed >= STANDALONE_MUNCHIES_COUNTDOWN_SECONDS
                        ? 0
                        : STANDALONE_MUNCHIES_COUNTDOWN_SECONDS - elapsed;
  if (remaining != standaloneMunchiesLastCountdown) {
    standaloneMunchiesLastCountdown = remaining;
    SendStandaloneMunchiesReady(remaining);
  }
  if (elapsed >= STANDALONE_MUNCHIES_COUNTDOWN_SECONDS) {
    standaloneMunchiesState = SMC_RUNNING;
    StartMunchiesMode(MUNCHIES_STANDALONE_CHALLENGE);
  }
}
