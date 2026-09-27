// Standalone Munchies Challenge lifecycle. The minigame itself remains in
// g_munchies_mode.ino; this coordinator only sequences players and results.

enum StandaloneMunchiesState : uint8_t {
  SMC_IDLE = 0,
  SMC_RUNNING,
  SMC_COMPLETE
};

StandaloneMunchiesState standaloneMunchiesState = SMC_IDLE;

boolean StandaloneMunchiesOwnsGameLoop() {
  return runningGameMode == GAME_MUNCHIES &&
         standaloneMunchiesState != SMC_IDLE;
}

void BeginStandaloneMunchiesPlayer() {
  firstplay = LOW;
  shoot = 0;
  BIP = 1;
  bonus = 0;
  bonusx = 0;
  standaloneMunchiesState = SMC_RUNNING;
  Serial.print(F("MUNCHIES_PLAYER,"));
  Serial.println(player);
  // A minijateknak sajat introja es 3-2-1 visszaszamlalasa van, ezert a
  // firmware nem var elotte meg egy masodik countdownra.
  StartMunchiesMode(MUNCHIES_STANDALONE_CHALLENGE);
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
  // A kozos main loop ezt minden frame-ben hivja. Standalone modban az
  // aktiv munkat maga a MunchiesUpdate vegzi; itt nincs kulso countdown.
}
