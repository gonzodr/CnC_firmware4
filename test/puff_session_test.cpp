// Host-side behavioral tests of the ACTUAL Arduino session/coordinator code.
// Run: g++ -std=c++11 test/puff_session_test.cpp -o /tmp/puff-test && /tmp/puff-test
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#define F(x) x
#define HIGH 1
#define LOW 0
typedef bool boolean;
unsigned long clockMs = 100;
unsigned long millis() { return clockMs; }
struct SerialStub {
  std::ostringstream out;
  void print(uint8_t v) { out << unsigned(v); }
  template<class T> void print(T v) { out << v; }
  void println(uint8_t v) { out << unsigned(v) << '\n'; }
  template<class T> void println(T v) { out << v << '\n'; }
} Serial;
struct WavStub {
  void trackPause(int) {} void trackResume(int) {} void stopAllTracks() {}
} wTrig;
enum { leftFlipperButton, rightflipperButton, ballShooterButton,
       leftFlipperBat, rightFlipperBat, ufoCoil, pop1Coil, pop2Coil,
       pop3Coil, leftSlingshotCoil, rightSlingshotCoil, ballTroughCoil,
       shooterlaneCoil, PIN_A5 };
int inputs[3] = {HIGH, HIGH, HIGH};
int outputs[14] = {};
int highWrites = 0;
int SimDigitalRead(int pin) { return inputs[pin]; }
void digitalWrite(int pin, int state) { outputs[pin] = state; if (state) ++highWrites; }
void DisableGameplayCoilsForService() {
  for (int pin = leftFlipperBat; pin <= shooterlaneCoil; ++pin) digitalWrite(pin, LOW);
}
enum { MUNCHIES_FROM_VUK, MUNCHIES_STANDALONE_CHALLENGE, PUFF_STANDALONE_CHALLENGE };
enum { GAME_STANDARD, GAME_MUNCHIES };
enum { ARCADE_MUNCHIES, ARCADE_PUFF_N_RIFF };
int runningGameMode = GAME_MUNCHIES, runningArcadeGame = ARCADE_PUFF_N_RIFF;
int firstplay, shoot, BIP, bonus, bonusx, player = 1, ball, numofplayers = 2, intmon;
unsigned long score[5] = {};
int effect, effectID, TRK_THEME, ufoanalog = 1000, ballsaversw = LOW;
int analogThreshold[6] = {120,120,120,120,120,120};
unsigned long ballsavetimer, UFO_EJECT_BALL_SAVE_MS = 5000;
int ufoshoot, ufoInactivesw, initlight;
unsigned long ufoInactiveTimer;
void StopLightTest() {} void RestorePartyShotsForPlayer() {} void Initlights() {}
void EnsureBallSave(unsigned long) { ballsaversw = HIGH; }
int AnalogSensorReadStable(int) { return 1000; }
void AddAwardedScore(unsigned long amount, int) { score[player] += amount; }
void Score(unsigned long amount, int) { score[player] += amount * 2; }
void SendData() {}
// FastLED stubs; lights are compiled too, but not the subject of these tests.
#define NUM_LEDS 115
struct CRGB {
  CRGB(int=0, int=0, int=0) {} static CRGB Black;
};
CRGB CRGB::Black, leds[NUM_LEDS];
enum { LED_CNC_AMBIENT, LED_LEFT_RAMP_AMBIENT, LED_FISHTANK_AMBIENT,
       LED_RIGHT_RAMP_AMBIENT, LED_GATE3_AMBIENT, LED_GATE32_AMBIENT,
       LED_GATE21_AMBIENT, LED_GATE1_AMBIENT, LED_CAR_AMBIENT,
       LED_UFO_ARROW_1, LED_UFO_ARROW_2 };
void fill_solid(CRGB*, int, CRGB) {}
uint8_t scale8(uint8_t a, uint8_t b) { return (unsigned(a)*b) >> 8; }
uint8_t sin8(uint8_t a) { return a; }
void SendMunchiesAbort(const char*);
void BeginMunchiesEject();
uint8_t ReadMunchiesInputMask();
void StandaloneMunchiesRunFinished();
#include "../g_munchies_mode.ino"
#include "../j_munchies_challenge.ino"

void command(const std::string& line) { HandleMunchiesCommand(line.c_str()); }
std::string sid() { return std::to_string(munchiesSession); }

int main() {
  StartStandaloneMunchiesChallenge();
  assert(StandaloneMunchiesOwnsGameLoop());
  assert(munchiesStartContext == PUFF_STANDALONE_CHALLENGE);
  assert(Serial.out.str().find("GUITAR_SOLO_START,1") != std::string::npos);
  // Long Pi asset loading is kept alive without starting gameplay early.
  for (int n = 0; n < 20; ++n) {
    clockMs += 1000;
    command("MG_ALIVE," + sid());
    MunchiesUpdate();
    assert(munchiesMode == MG_WAIT_READY);
  }
  command("MG_READY," + sid());
  assert(munchiesMode == MG_ACTIVE);
  unsigned long beganAt = munchiesModeStartedAt;
  clockMs += 5;
  command("MG_READY," + sid());
  assert(munchiesModeStartedAt == beganAt); // duplicate ready is not a reset
  inputs[leftFlipperButton] = LOW;
  MunchiesUpdate();
  assert(puffStableInputMask == 0);
  clockMs += 11;
  MunchiesUpdate();
  assert(puffStableInputMask == 1);
  inputs[leftFlipperButton] = HIGH;
  MunchiesUpdate();
  clockMs += 11;
  MunchiesUpdate();
  assert(puffStableInputMask == 0); // sustain release reaches GUI too
  command("MG_DONE,65535,99999");
  assert(score[1] == 0); // wrong session ignored
  std::string oldSid = sid();
  command("MG_DONE," + oldSid + ",42000");
  assert(score[1] == 42000); // no normal-game 2x modifier
  assert(player == 2 && munchiesMode == MG_WAIT_READY);
  command("MG_DONE," + oldSid + ",42000");
  assert(score[1] == 42000 && score[2] == 0); // idempotent retry
  command("MG_READY," + sid());
  command("MG_DONE," + sid() + ",12000");
  assert(score[2] == 12000);
  assert(intmon == 2 && standaloneMunchiesState == SMC_COMPLETE);
  assert(Serial.out.str().find("MUNCHIES_FINISH,1") != std::string::npos);
  assert(highWrites == 0 && ballsaversw == LOW); // no phantom serving/VUK kick
  // Silent/broken GUI terminates safely; it cannot hold the machine forever.
  numofplayers = 1;
  StartStandaloneMunchiesChallenge();
  clockMs += 10001;
  MunchiesUpdate();
  assert(munchiesMode == MG_IDLE && standaloneMunchiesState == SMC_COMPLETE);
  StartStandaloneMunchiesChallenge();
  command("MG_READY," + sid());
  clockMs += MG_LINK_TIMEOUT_MS;
  MunchiesUpdate();
  assert(munchiesMode == MG_IDLE);
  StartStandaloneMunchiesChallenge();
  AbortMunchiesForService();
  assert(munchiesMode == MG_IDLE);
  assert(highWrites == 0);
  std::cout << "Puff firmware behavioral tests: PASS\n";
}
