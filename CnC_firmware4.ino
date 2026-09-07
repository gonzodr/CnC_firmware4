////////////////////////////////////
// Cheech & Chong Pinball
// Firmware version 4
////////////////////////////////////
// Catalog:
// Ballhandler - 640
// Timers - 1000
// Light handling - 1090
// Init mode and player select - 1240
// Init table - 1425
// Data send - 1505
// Score - 1530
// Initlights - 1550
// Flippers - 1630
// Slingshots - 1670
// CnC - 1765
// Loop - 1990
// Weed - 2050
// Fish Tank - 2360
// DAVE - 2520
// Gate - 2680
// POPs - 2890
// Bonus X - 3060
// Weed Spinner - 3220
// UFO - 3470
// Cheech and Chong Targets - 3740
// Collectives - 4020
// Gift - 4100
// Bridge Low - 4150
// Bridge High - 4335
// Add Player - 4520
// Hurry Up mode - 4550
// Tilt - 4610

#define FX_CODEC_SUPPORTED 1
#include <FastLED.h>          // For the leds
#include <Wire.h>             // For send data to Servo UNO
#include "wavTrigger.h"       // Repo-local, fix Serial1 backend (TX1=18, RX1=19)
#include "effect_data.h"      // fenyeffekt-adat-tablak (kulon fajlban)

//////////////////////////////////////////////////////////////////////
// !!! PROBAPADI SZIMULATOR MOD (f_sim_mode.ino) !!!
// Probapadi teszthez vedd ki a kommentet - ELES GEPRE SOHA ne keruljon
// fel bekapcsolva! (sim modban a golyo-infrak szimulalt erteket adnak,
// a valodi erzekeloket a firmware NEM olvassa!)
// A main agon MINDIG kikommentelve tartjuk, mert a gepbeli F7-es
// firmware update automatikusan a main-t huzza es flasheli!
//#define SIM_MODE
// Hard biztonsagi kapu: egy veletlenul helyben maradt #define SIM_MODE vagy
// Arduino IDE extra flag ELES feltolteskor nem eleg a simulatorhoz. A SIM
// buildet csak a test/flash_sim.sh altal adott MASODIK flag engedelyezi.
#if defined(SIM_MODE) && !defined(SIM_BUILD_CONFIRMED)
#warning "SIM_MODE ignored: SIM_BUILD_CONFIRMED is required"
#undef SIM_MODE
#endif
#ifdef SIM_MODE
int simForceLottery = 0; // cinkelt UFO-lotto: 7=SpaceCoke, 8=pontlopas, 9=minigame
#endif
//////////////////////////////////////////////////////////////////////

#define NUM_LEDS 115

// --- LED-index nevek (az Initlights() autoritativ terkepebol) ---
#define LED_DAVE_V               2
#define LED_DAVE_E               3
#define LED_BONUS_X8             6
#define LED_BONUS_X6             7
#define LED_BALLSAVE             8
#define LED_BONUS_X4             9
#define LED_BONUS_X2             10
#define LED_DAVE_A               15
#define LED_DAVE_D               16
#define LED_CNC_AMBIENT          17
#define LED_CNC_C1               18
#define LED_CNC_AMP              19
#define LED_CNC_C2               20
#define LED_LEFT_LOOP_1          21
#define LED_LEFT_LOOP_2          22
#define LED_LEFT_RAMP_AMBIENT    23
#define LED_LEFT_RAMP_1          24
#define LED_LEFT_RAMP_2          25
#define LED_CHONG_IND            26
#define LED_GET_HIGH_1           27
#define LED_GET_HIGH_2           28
#define LED_WEED_W               29
#define LED_WEED_E1              30
#define LED_WEED_E2              31
#define LED_WEED_D               32
#define LED_CHEECH_IND           33
#define LED_RRAMP_ARROW_22       34
#define LED_RRAMP_ARROW_21       35
#define LED_RRAMP_ARROW_12       36
#define LED_RRAMP_ARROW_11       37
#define LED_CHEECH_WHEEL         38
#define LED_CHONG_PIPE           39
#define LED_CHONG_ROACH          40
#define LED_CHEECH_CIGAR         41
#define LED_UFO_ARROW_1          42
#define LED_UFO_ARROW_2          43
#define LED_RIGHT_LOOP_2         44
#define LED_RIGHT_LOOP_1         45
#define LED_MUF_DVR              46
#define LED_CHONG_KEY            47
#define LED_FISH                 48
#define LED_TANK                 49
#define LED_FISHTANK_AMBIENT     50
#define LED_RIGHT_RAMP_AMBIENT   51
#define LED_GATE3_AMBIENT        52
#define LED_GATE3                53
#define LED_GATE32_AMBIENT       54
#define LED_GATE2                55
#define LED_GATE21_AMBIENT       56
#define LED_GATE1                57
#define LED_GATE1_AMBIENT        58
#define LED_MICHOAKAN            59
#define LED_ACAPULCO             60
#define LED_HIGHTABLE_FLASHER    61
#define LED_THAI                 62
#define LED_LABRADOR             63
#define LED_POP1                 64
#define LED_POP2                 65
#define LED_POP3                 66
#define LED_CAR_AMBIENT          67

#define DATA_PIN 3
#define BRIGHTNESS  64

// --- wTrig track-nevek (a WAV-fajlnevekbol) ---
#define TRK_THEME              1
#define TRK_BEER               2
#define TRK_PLUMB              3
#define TRK_CHINGA             4
#define TRK_BLOB               6
#define TRK_PING               10
#define TRK_LARDASS            11
#define TRK_WEED               15
#define TRK_CHEECHYEAH         17
#define TRK_BANANA             19
#define TRK_DAVE               23
#define TRK_GATESUCCESS        27
#define TRK_KVAKK              28
#define TRK_BIGYONG            29
#define TRK_CHEECHBEAUTY       36
#define TRK_CHEECHFART         37
#define TRK_SHOOTOUTUFO        42
#define TRK_HAPPYUFO           45
#define TRK_MISSU              46
#define TRK_LETSPLAY           47
#define TRK_DAVENOTHERE        51
#define TRK_MUS_TRIPLE_LOOP3   64
#define TRK_MUS_SWING_LOOP4    65
#define TRK_WEEDFULL           72
#define TRK_JACKPOT            73
#define TRK_MUS_ROCKFIGHT      88
#define TRK_MUS_STRAWBERRY2    89
#define TRK_BOOT               90
#define TRK_FIREWORK           91
#define TRK_SCORE_5000         92
#define TRK_COMBO1             95
#define TRK_COMBO2             96
#define TRK_HISCORE_SKIP       102
#define TRK_ADDPLAYER          103
#define TRK_ADDSCORE           104
#define TRK_KEYLEFT            105
#define TRK_KEYRIGHT           106
#define TRK_NEXTLETTER         107
#define TRK_MUS_HURRY          108
#define TRK_MUS_SPACECOKE      109
#define TRK_CHEECH_SPACECOKE   110
#define TRK_MELLOWOUT          111
#define TRK_TILT1              112
#define TRK_TILT2              113
#define TRK_TILT3              114
#define TRK_CHAINWHEEL         117
#define TRK_BIGJOINT           118
#define TRK_LICENSEPLATE       119
#define TRK_WEEDPIPE           120
#define TRK_COCKROACH          121
#define TRK_PIPEWRENCH         122
#define TRK_COLLECT            123
#define TRK_UFO_WHEEL_BG       128

// --- Uj Cheech/Chong es UFO/Alien voice-over csomag (OrigySD 2026) ---
// Az UFO/egyeb esemenyek harom, a jackpotok ket egymas utani hangbol allnak.
// Jackpot: az elso track Cheech, a kovetkezo Chong; egy talalat egy hang.
// A trackek
// 255 fole is mennek, ezert a kezdoazonositok es a lejatszo helper 16 bitesek.
#define TRK_VO_CHEECH_BALL_LAUNCH_A    206
#define TRK_VO_CHEECH_BALL_SAVE_A      215
#define TRK_VO_CHONG_EXTRA_BALL_A     218
#define TRK_VO_CHONG_BEER_COLLECTED_A 230
#define TRK_VO_CHONG_JOINT_ROLLED_1_A 239
#define TRK_VO_UFO_WEED_NEED_BEER_A   224
#define TRK_VO_UFO_WEED_CHOOSE_A      227
#define TRK_VO_UFO_BEER_FULL_A        233
#define TRK_VO_UFO_SPINNER_SHOOT_A    236
#define TRK_VO_UFO_JOINT_ROLLED_2_A   242
#define TRK_VO_UFO_LOVE_PACK_READY_A  245
#define TRK_VO_UFO_NO_WEED_LONG_A     248
#define TRK_VO_UFO_NO_WEED_EJECT_A    251
#define TRK_VO_UFO_NO_WEED_ALARM_A    254
#define TRK_VO_UFO_CASHOUT_15000_A    257
#define TRK_VO_UFO_CASHOUT_20000_A    260
#define TRK_VO_UFO_CASHOUT_25000_A    263
#define TRK_VO_UFO_CASHOUT_30000_A    266
#define TRK_VO_UFO_EXTRA_BALL_LIT_A   275
#define TRK_VO_UFO_FEATURE_WHEEL_A    278
#define TRK_VO_UFO_WHEEL_HURRY_UP_A   284
#define TRK_VO_UFO_WHEEL_MUNCHIES_A   287
#define TRK_VO_UFO_SPACE_COKE_START_A 290
#define TRK_VO_UFO_WHEEL_EXTRA_BALL_A 293

#define TRK_VO_JACKPOT_10000_A       209
#define TRK_VO_JACKPOT_15000_A       211
#define TRK_VO_JACKPOT_20000_A       213
#define TRK_VO_JACKPOT_25000_A       221
#define TRK_VO_JACKPOT_30000_A       299
#define TRK_VO_JACKPOT_50000_A       305
#define TRK_VO_JACKPOT_100000_A      307

// A Ballhandler mar az elso golyo kilovesekor hasznalja, ezert explicit
// deklaracio kell az Arduino automatikus prototipus-generalasa ele.
void PlaySpeechRange(uint16_t firstTrack, uint8_t count = 3);

// --- Kozponti pontozasi profil (balance pass 1, 2026-08-24) ---
// A direkt pont es a golyovegi bonusz kulon ertek. A Hurry Up szorzojat
// kizarolag a Score() alkalmazza a direkt pontra; a bonusz nem duplazodik.
namespace Scoring {
  const uint8_t HURRY_UP_MULTIPLIER = 2;

  const unsigned long SLING_POINTS = 250UL;
  const unsigned int  SLING_BONUS = 25U;
  const unsigned long POP_POINTS = 250UL;
  const unsigned int  POP_BONUS = 10U;
  const unsigned long SPINNER_POINTS = 250UL;
  const unsigned int  SPINNER_BONUS = 10U;

  const unsigned long GATE_POINTS = 500UL;
  const unsigned int  GATE_BONUS = 25U;

  const unsigned long CNC_LETTER_POINTS = 1500UL;
  const unsigned int  CNC_LETTER_BONUS = 50U;
  const unsigned long CNC_COMPLETE_POINTS = 5000UL;
  const unsigned int  CNC_COMPLETE_BONUS = 500U;
  const unsigned long WEED_LETTER_POINTS = 1000UL;
  const unsigned int  WEED_LETTER_BONUS = 50U;
  const unsigned long WEED_COMPLETE_POINTS = 5000UL;
  const unsigned int  WEED_COMPLETE_BONUS = 500U;
  const unsigned long FISHTANK_TARGET_POINTS = 1500UL;
  const unsigned int  FISHTANK_TARGET_BONUS = 50U;
  const unsigned long FISHTANK_PAIR_POINTS = 2500UL;
  const unsigned int  FISHTANK_PAIR_BONUS = 250U;
  const unsigned long JOINT_POINTS[3] = { 5000UL, 10000UL, 20000UL };
  const unsigned int  JOINT_BONUS[3] = { 250U, 500U, 1000U };

  const unsigned long LOOP_POINTS = 2500UL;
  const unsigned int  LOOP_BONUS = 250U;
  const unsigned long BRIDGE_POINTS = 1000UL;
  // Hurry Up alatt a hid alapja; a 2x szorzoval ez a kifizetett 10000.
  // Hurry Up alatti fix kifizetesek. A Score() duplaz, ezert itt a FELE all:
  // 7500 -> 15000 (kishid), 12500 -> 25000 (nagyhid), 15000 -> 30000 (loop).
  // Ezekhez a sima pontozo videok mennek (Point5/7/8), nem a jackpot-klipek.
  const unsigned long HURRY_BRIDGE_LOW_POINTS  = 7500UL;
  const unsigned long HURRY_BRIDGE_HIGH_POINTS = 12500UL;
  const unsigned long HURRY_LOOP_POINTS        = 15000UL;
  const unsigned int  BRIDGE_BONUS = 100U;
  const unsigned long INACTIVE_CHARACTER_POINTS = 200UL;
  const unsigned int  INACTIVE_CHARACTER_BONUS = 50U;
  const unsigned long GIFT_CNC_POINTS = 5000UL;
  const unsigned int  GIFT_CNC_BONUS = 500U;
  const unsigned long GIFT_OTHER_POINTS = 5000UL;
  const unsigned int  GIFT_OTHER_BONUS = 100U;
  const unsigned long DRIFT_POINTS = 1500UL;
  const unsigned int  DRIFT_BONUS = 50U;
  const unsigned long MULTIBALL_SPINNER_POINTS = 1000UL;
  const unsigned int  MULTIBALL_SPINNER_BONUS = 10U;
  const unsigned long LOOP_JACKPOT_POINTS = 30000UL;
  const unsigned int  LOOP_JACKPOT_BONUS = 2000U;
  // Nagyhid utan azonnal loop: multiball-szinthez kotott nagy kifizetes.
  // Csak olyan osszegek szerepelnek, amikhez van jackpot-video es -bemondas
  // (a 200000-et a PsychedelicJackpot klip mutatja).
  //                                           mb:  0      1      2      3       4       5
  const unsigned long HIGH_LOOP_COMBO_SCR[6] = { 0, 25000, 30000, 50000, 100000, 200000 };
  const unsigned long UFO_EJECT_POINTS = 0UL;
  const unsigned int  UFO_EJECT_BONUS = 300U;
  const unsigned long COLLECTIBLE_POINTS[3] = { 10000UL, 15000UL, 20000UL };
  const unsigned int  COLLECTIBLE_BONUS[3] = { 500U, 1000U, 2000U };

  // A letezo jackpot-osszegek: mindegyikhez van klip az assets/Videos alatt
  // es bemondas a firmware-ben. A Hurry Up duplazasa nem mindig esik a
  // keszletre (20000-bol 40000, 30000-bol 60000 lenne), ezert a
  // JackpotScorePoints felfele igazit a legkozelebbi letezo osszegre. A
  // regi, kezzel beirt 40000/60000 eset ennek volt ket peldanya - azok az
  // int-korszakbol maradtak, es most kikerultek.
  const unsigned long JACKPOT_TIERS[8] = {
    10000UL, 15000UL, 20000UL, 25000UL, 30000UL, 50000UL, 100000UL, 200000UL
  };

  const unsigned long COMBO_POINTS[6] = {
    2500UL, 5000UL, 7500UL, 10000UL, 15000UL, 20000UL
  };
  const unsigned int COMBO_BONUS[6] = { 100U, 150U, 200U, 250U, 300U, 500U };
}

// A DAVE nem pontfeature: a lane-ek veletlen/mento jelleguek, a teljesites
// egyetlen jutalma a jatek folytatasat segito ball save.
const unsigned long DAVE_BALL_SAVE_MS = 10000UL;
// Celzott híd-lovesre is maradjon ido: a regi 4 mp helyett 5,5 mp.
const unsigned long BRIDGE_COMBO_WINDOW_MS = 5500UL;
// A nagyhid->loop kombo SAJAT, a hidak kozti komboablaktol fuggetlen ideje.
const unsigned long HIGH_LOOP_COMBO_WINDOW_MS = 5000UL;
// Minden VUK-kidobas ugyanazt az 5 mp-es fizikai vedelmet kapja. A
// SpaceCoke 30 mp-e kulon multiball-szabaly, nem erosebb UFO-jutalom.
const unsigned long UFO_EJECT_BALL_SAVE_MS = 5000UL;
const unsigned long SPACECOKE_BALL_SAVE_MS = 30000UL;

#define LED_TYPE    WS2812B
#define COLOR_ORDER RGB
#define UPDATES_PER_SECOND 100
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

wavTrigger wTrig;

String incomeMsg = "";



/////////////////////////////////////////////////////////////////
/// Variables for the gate
// Pins
int gateSwitch1 = 45; // gate 1
int gateSwitch2 = 44; // gate 2
int gateSwitch3 = 40; // gate 3
// Integers
int bonusx = 0;
// Booleans
boolean gatesw1 = LOW;
boolean gatesw2 = LOW;
boolean gatesw3 = LOW;
boolean gateamb1 = LOW;
boolean gateamb2 = LOW;
boolean gateamb3 = LOW;
boolean gateoffsw = LOW;
boolean gatetimesw = LOW;
boolean gsrtimesw = LOW;
boolean gsltimesw = LOW;
    // Bonus multiplier
boolean bonusx1sw = LOW;
boolean bonusx2sw = LOW;
boolean bonusx3sw = LOW;
boolean bonusx4sw = LOW;
// Timers
unsigned long gateambtimer = 0;
unsigned long gateofftimer = 0;
unsigned long gatetimer = 0;
// Arrays
int gatearr[] = { 0, 0, 0, 0, 0 };
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for the POPs
// Pins
int pop1Coil = 33; // output pop1
int pop2Coil = 36; // output pop 2
int pop3Coil = 32; // output pop3
int pop1Switch = 34; // input pop 1
int pop2Switch = 35; // input pop 2
int pop3Switch = 39; // input pop 3
// Booleans
boolean pop1LogicBool = LOW;
boolean pop2LogicBool = LOW;
boolean pop3LogicBool = LOW;
boolean popsw1 = LOW;
boolean popsw2 = LOW;
boolean popsw3 = LOW;
// Timers
unsigned long poptimer1 = 0;
unsigned long poptimer2 = 0;
unsigned long poptimer3 = 0;
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
/// Variables for the Bridges
// Pins
int bridgeLowSwitch = 53; // Bridgelow
int bridgeHighSwitch = 52; // BridgeHigh
// Booleans
boolean BridgeLowEffbool = LOW;
boolean BrdgHighActive = 0;
boolean BrdgHighSw = 0;
boolean BrdgLowActive = 0;
boolean BrdgLowSw = 0;
// Integers
int BridgeLowEffcounter = 0;
int comboCounter = 0;
// Timers
unsigned long comboTimerH = 0;
unsigned long comboTimerL = 0;
// A nagyhid utolso jackpot-talalatanak ideje; 0 = nincs elarmozva.
unsigned long highLoopArmT = 0;
unsigned long BrdgLowT = 0;
unsigned long BrdgHighT = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for the Fish tank
// Pins
int fishTankSwitch1 = 16; // Fish
int fishTankSwitch2 = 17; // tank
// Booleans
boolean fishoff = LOW;
// Integers
int fishTankLightState1 = 0;
int fishTankLightState2 = 0;
// A sorok jatekosonkent, golyok kozott is megmaradnak. Egy sort a
// ROLL A JOINT loves hasznal fel; harom jointbol lesz a Love Pack.
uint8_t beerCredits[] = { 0, 0, 0, 0, 0 };
uint8_t jointStack[] = { 0, 0, 0, 0, 0 };
boolean weedQualified[] = { LOW, LOW, LOW, LOW, LOW };
// Timers
unsigned long fishtimer = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for CnC
// Pins
int cncLetterC1st = 14; // Letter 1st C
int cncLetterN = 15; // Letter n
int cncLetterC3rd = 25; // Letter 3rd C
// Booleans
boolean cncoff = LOW;
boolean cnchurryswitch1 = LOW;
boolean cnchurryswitch2 = LOW;
boolean cnchurryswitch3 = LOW;
// Integers
int cncswitch1 = 0;
int cncswitch2 = 0;
int cncswitch3 = 0;
// Timers
unsigned long cnctimer = 0;
unsigned long cnchurryTimer1 = 0;
unsigned long cnchurryTimer2 = 0;
unsigned long cnchurryTimer3 = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Dave
// Pins
int daveLaneSwitchA = 50; // A (18-rol atkotve! 18/19 = Serial1 a wavTriggernek)
                          // NEM a 42-re: azon a (kodban nem hasznalt, de
                          // BEKOTOTT) Cheech Alley kapcsolo vezeteke ul!
int daveLaneSwitchD = 49; // D (19-rol atkotve!)
int daveLaneSwitchV = 22; // V
int daveLaneSwitchE = 23; // E
// Booleans
boolean daveoff = LOW;
boolean slltimesw = LOW;
boolean slrtimesw = LOW;
// Integers
// Timers
unsigned long davetimer = 0;
unsigned long slltimer = 0;
unsigned long slrtimer = 0;
// Arrays
int davearr[] = { 0, 0, 0, 0, 0, 0 };
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Slingshots
// Pins
int rightSlingshotCoil = 29; // Right Slingshot
int leftSlingshotCoil = 28; // Left Slingshot
int leftSlingshotSwitch = 31; // Left Slingshot
int rightSlingshotSwitch = 4; // Right Slingshot
// Booleans
boolean slingl = LOW;
boolean slingr = LOW;
// Timers
unsigned long slingrtimer = 0;
unsigned long slingltimer = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Flipper Bats
// Pins
int leftFlipperBat = 41; // Left flipper
int rightFlipperBat = 24; // Right flipper
int rightflipperButton = 26; // Right flipper button
int leftFlipperButton = 27; // Left flipper button
// Booleans
boolean flipl = LOW;
boolean flipr = LOW;
// Timers
unsigned long flipltimer = 0;
unsigned long fliprtimer = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Spinner
// Pins
int spinnerSwitch = 38; // get High spinner
int a8 = 2;
// Integers
int cigar = 0;
// Booleans
boolean weedspsw = LOW;
// Integers
int spinnersw = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Weedtable
// Pins

// Arrays
int weedmeter[] = { 180, 180, 180, 180, 180 }; // index 1..4 = players, [0] unused


/////////////////////////////////////////////////////////////////
/// Variables for Balltrough
// Pins
int ballTroughCoil = 5; // ball trough
int a1 = 1;
int a2 = 2;
int a3 = 7;
int a4 = 4;
int a5 = 5;
int a6 = 6;
int a7 = 3;
int a10 = 10;
// Booleans
boolean ballsaversw = HIGH; // Turn on the ballsaver
boolean BallReadState = LOW;
// A lecsorgas (BIS == 5) megerositese. A golyotarolo infra-szenzorait az
// UFO-lottery fenyeffektje es a tekercsek zaja egy-egy pillanatra atbillenti
// a kuszob ala, es egyetlen ilyen minta korabban azonnal "golyo lement"-et
// jelentett - akkor is, ha a golyo eppen az UFO-ban ult. Ezert tobb egymas
// utani meres ES egy minimalis ido is kell hozza.
const uint8_t BALL_DRAIN_CONFIRM_READS = 6;
const unsigned long BALL_DRAIN_CONFIRM_MS = 120UL;
uint8_t bisFiveReads = 0;
unsigned long bisFiveSince = 0;
boolean ballHandlerSkip = 0;
boolean shoot = LOW;
boolean aftermulti = LOW;
boolean startmus = LOW; // Start game music when ball ejected
boolean sidelaneBallsaverSw = LOW;
boolean maxBallSw = LOW; // SpaceCoke: az utolso tarolt golyo keslelteto kapcsoloja
// Integers
int miv1 = 0;
int miv2 = 0;
int miv3 = 0;
int miv4 = 0;
int miv5 = 0;
int ball1 = 0;
int ball2 = 0;
int ball3 = 0;
int ball4 = 0;
int ball5 = 0;
int ballsavetime = 15000;
int extraball = 0;
boolean extraBallLit = LOW;
int multiball = 0;
// A szenzoronkenti kuszobok a h_analog_test.ino-ban elnek (EEPROM-bol
// toltve). Az Arduino preprocesszor csak FUGGVENY-prototipust general
// automatikusan, valtozohoz nem - a .ino-k osszefuzesekor pedig ez a fajl
// van elol, ezert kell ide az explicit extern.
extern uint16_t analogThreshold[];

int ballPresent1 = 0;
int ballPresent2 = 0;
int ballPresent3 = 0;
int ballPresent4 = 0;
int ballPresent5 = 0;
int shootfail = 0;
int BIS;
int BIP;
int player = 1;       // Player
int ball = 1;         // Actual Ball number
int numofplayers = 1; // Number of players
// Timers
unsigned long shoottimer = 0;
unsigned long shoottimer2 = 0;
unsigned long ballsavetimer = 0;
unsigned long ballHandlerSkipTimer = 0;
unsigned long maxBallSwTimer = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Cheech and Chong 
// Pins
int chongSwitch = 7; // Chong Trigger
int cheechSwitch = 12; // Cheech Trigger
// Boolean
boolean chongLightActiveSw = LOW;
boolean cheechLightActiveSw = LOW;
boolean chongoffsw = LOW;
boolean cheechoffsw = LOW;
boolean chongactive = LOW;
boolean CollectSw = 0;
// Integers
// (cheechspeech/chongspeech valtozok megszuntek: a beszedvalasztas mostmar
//  a PlaySpeech() helper + track-tombok dolga)
// Timers
unsigned long cheechoffswtimer = 0;
unsigned long chongoffswtimer = 0;
// Arrays
int cheechCollectives[] = { 0, 0, 0, 0, 0 };
int chongCollectives[] = { 0, 0, 0, 0, 0 };
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Shooterlane 
// Pins
int shooterLaneSwitch = 13; // shooterlane switch
int shooterlaneCoil = 6; // shooterlane kicker
int ballShooterButton = 51; // ballShooterSwitch
// Booleans
boolean shootfailchk = 0;
boolean AutoKick = LOW;
boolean kick = LOW;
boolean firstplay = HIGH;
boolean shooterLaneWasClosed = LOW;
// Timers
unsigned long kicktimer = 0;
unsigned long shooterLaneClosedAt = 0;
const unsigned long SHOOTER_LANE_REKICK_MS = 1000UL;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Ufo
// Pins
int ufoCoil = 37; // output ufo
// Booleans
boolean ufosw = 0;
boolean ufoInactivesw = 0;
boolean ufoEjectSaveStarted = LOW;
// Integers
int lottery = 0;
int ufoshoot = 0;
int ufoanalog = 0;
int ufoMinus = 0; // pontlopasnal (lottery 8): a kirabolt jatekos sorszama
enum UfoPartyTier : uint8_t {
  UFO_PARTY_NONE = 0,
  UFO_PARTY_CASHOUT = 1,
  UFO_PARTY_SUPER_CASHOUT = 2,
  UFO_PARTY_FEATURE_WHEEL = 3,
  UFO_PARTY_LOVE_PACK = 4
};
uint8_t ufoAwardTier = UFO_PARTY_NONE;
uint16_t ufoWheelSession = 0;
boolean ufoWheelWaiting = LOW;
boolean ufoWheelResultVoicePlayed = LOW;
unsigned long ufoWheelStartedAt = 0;
unsigned long ufoWheelLastSendAt = 0;
unsigned long ufoWheelChaseStartedAt = 0;
const unsigned long UFO_WHEEL_RETRY_MS = 500UL;
const unsigned long UFO_WHEEL_RESULT_VOICE_MS = 5000UL;
// A GUI 6 mp-es Wheel sequence-e utan erkezik a DONE; az Extra Ball/Hurry
// eredmenyvideo mar a jutalommal parhuzamosan fut. Kapcsolathibanal 12 mp
// utan is tovabblepunk, hogy a golyo ne ragadjon a VUK-ban.
const unsigned long UFO_WHEEL_TIMEOUT_MS = 12000UL;
const unsigned long UFO_LOTTERY_HOLD_MS = 5500UL;
// Timers
unsigned long ufoInactiveTimer = 0;
unsigned long ufoshoottimer = 0;
unsigned long ufoshoottimer2 = 0;
unsigned long ufoDetectStartedAt = 0;
const unsigned long UFO_DETECT_STABLE_MS = 30UL;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Weed
// Pins
int d8 = 8;
int d9 = 9;
int d10 = 10;
int d11 = 11;
// Integers
int weedswitch1 = 0;
int weedswitch2 = 0;
int weedswitch3 = 0;
int weedswitch4 = 0;
// Booleans
boolean weedoff = LOW;
boolean weedmultibool = LOW;
boolean weedblastbool = LOW;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Loop
// Pins
int loopSwitchTop = 30; // Felső loop
int loopSwitchSide = 43; // loop alsó
// Booleans
boolean looplightbool = LOW;
boolean loopsw = LOW;
// Timers
unsigned long looptimer = 0;
unsigned long loopswt = 0;
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
/// Variables for Light Effect
// (Az EffectID tablak az effect_data.h-ba kerultek.)



/////////////////////////////////////////////////////////////////
/// Misc


// int cheechAlleySwitch = "D42"; // cheech alley
int startButton = 47; // start
// int d49 = 49; // unused
int giftsw = 0;
int intmon = 1;
int heysoundcounter = 0;
// Commercial-style plumb-bob tilt: ket figyelmeztetes utan a harmadik,
// kulonallo harangerintes tilteli az aktualis golyot.
const uint8_t TILT_WARNINGS_ALLOWED = 2;
const unsigned long TILT_SETTLE_MS = 750UL;
uint8_t tiltWarnings = 0;
int shootLightCycle;
int colorcode;
int offset = 0;
int effectID = 0;
int weedmulticounter = 0;
int weedblastcounter = 0;
int looplightcounter = 0;
int feny[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int hurryhitcounter = 0;
int fasz = 0;         // Led start at fasz
int randGift = 0;
int resetTimer = 0;

boolean hurryhitbool = 0;
boolean shooteffbool = LOW;
boolean blinky = LOW;
boolean ledState = LOW;
boolean cigarState = LOW;
boolean initlight = HIGH;
boolean Inittable = LOW;
boolean sendDsw = LOW;
boolean sltimesw = LOW;
boolean weedtableindicator = LOW;
boolean effect = LOW;
boolean effectState = LOW;
boolean multiloopsw = LOW;
boolean addPlayersw = LOW;
boolean lfHscSw = 0;
boolean rfHscSw = 0;
boolean shHscSw = 0;
boolean stHscSw = 0;
boolean hurryUp = LOW;
boolean hurryUpState = LOW;
enum HurryLightZone : uint8_t {
  HURRY_ZONE_CNC = 0,
  HURRY_ZONE_CHONG,
  HURRY_ZONE_WEED,
  HURRY_ZONE_CHEECH,
  HURRY_ZONE_FISHTANK,
  HURRY_ZONE_SLING_RIGHT,
  HURRY_ZONE_BALLSAVE,
  HURRY_ZONE_SLING_LEFT,
  HURRY_ZONE_COUNT
};
// Fizikai kor: CnC -> Chong -> WEED -> Cheech -> Fishtank -> jobb sling ->
// bonus/ballsave blokk -> bal sling -> vissza CnC. Az also, korabban
// nevesitetlen LED-ek a ket sling kornyezetenek fenycsovai.
const uint8_t hurryPathLed[] PROGMEM = {
  LED_CNC_AMBIENT, LED_CNC_C1, LED_CNC_AMP, LED_CNC_C2,
  LED_CHONG_IND,
  LED_WEED_W, LED_WEED_E1, LED_WEED_E2, LED_WEED_D,
  LED_CHEECH_IND,
  LED_FISH, LED_TANK, LED_FISHTANK_AMBIENT,
  14, 13, 12, 11,
  LED_BONUS_X2, LED_BONUS_X4, LED_BALLSAVE, LED_BONUS_X6, LED_BONUS_X8,
  5, 4, 1, 0
};
const uint8_t hurryPathZone[] PROGMEM = {
  HURRY_ZONE_CNC, HURRY_ZONE_CNC, HURRY_ZONE_CNC, HURRY_ZONE_CNC,
  HURRY_ZONE_CHONG,
  HURRY_ZONE_WEED, HURRY_ZONE_WEED, HURRY_ZONE_WEED, HURRY_ZONE_WEED,
  HURRY_ZONE_CHEECH,
  HURRY_ZONE_FISHTANK, HURRY_ZONE_FISHTANK, HURRY_ZONE_FISHTANK,
  HURRY_ZONE_SLING_RIGHT, HURRY_ZONE_SLING_RIGHT,
  HURRY_ZONE_SLING_RIGHT, HURRY_ZONE_SLING_RIGHT,
  HURRY_ZONE_BALLSAVE, HURRY_ZONE_BALLSAVE, HURRY_ZONE_BALLSAVE,
  HURRY_ZONE_BALLSAVE, HURRY_ZONE_BALLSAVE,
  HURRY_ZONE_SLING_LEFT, HURRY_ZONE_SLING_LEFT,
  HURRY_ZONE_SLING_LEFT, HURRY_ZONE_SLING_LEFT
};
const uint8_t HURRY_PATH_COUNT = sizeof(hurryPathLed) / sizeof(hurryPathLed[0]);
const unsigned long HURRY_CHASE_STEP_MS = 85UL;
const unsigned long HURRY_HIT_FADE_MS = 1400UL;
unsigned long hurryChaseStartedAt = 0;
unsigned long hurryHitAt[HURRY_ZONE_COUNT] = { 0 };
// Player select mod (intmon == 3)
boolean selArmSw = LOW;
boolean selShootSw = LOW;
unsigned long selShootTimer = 0;
unsigned long selTimeoutTimer = 0;
boolean weedhurryswitch1 = 0;
boolean weedhurryswitch2 = 0;
boolean weedhurryswitch3 = 0;
boolean weedhurryswitch4 = 0;
boolean tiltContactLatched = LOW;
boolean ballTilted = LOW;


unsigned long weedhurrytimer1 = 0;
unsigned long weedhurrytimer2 = 0;
unsigned long weedhurrytimer3 = 0;
unsigned long weedhurrytimer4 = 0;
unsigned long hurryUpTimer = 0;
// A Hurry Up hossza. Korabban ket helyen bedrotozott 80000 volt; a GUI a
// visszaszamlalashoz megkapja az erteket, igy elég itt atirni.
const unsigned long HURRY_UP_DURATION_MS = 60000UL;
unsigned long stHscTimer = 0;
unsigned long lfHscTimer = 0;
unsigned long rfHscTimer = 0;
unsigned long shHscTimer = 0;
unsigned long addPlayerTimer = 0;
unsigned long CollectTimer = 0;
unsigned long bonus = 0;
unsigned long score[] = {0, 0, 0, 0, 0}; // index 1..4 = players, [0] unused
unsigned long weedm[] = {0, 0, 0, 0, 0};
unsigned long previousMillis = 0;
unsigned long blinkypreviousMillis = 0;
unsigned long cigarpreviousMillis = 0;
unsigned long effectpreviousMillis = 0;
unsigned long BallpreviousMillis = 0;
unsigned long weedtimer = 0;
unsigned long sltimer = 0;
unsigned long gsltimer = 0;
unsigned long gsrtimer = 0;
unsigned long bonusxtimer1 = 0;
unsigned long bonusxtimer2 = 0;
unsigned long bonusxtimer3 = 0;
unsigned long bonusxtimer4 = 0;
unsigned long sendDtimer = 0;
unsigned long weedswitchtimer = 0;
unsigned long cigartime = 0;
unsigned long weedtableindicatortimer = 0;
unsigned long hurryUppreviousMillis = 0;
unsigned long heysoundtimer = 0;
unsigned long tiltOpenSince = 0;
const long interval = 100; // Ledstate blinktime intervall


void setup() {
  AnalogThresholdsLoad(); // EEPROM-ban tarolt szenzor-kuszobok (h_analog_test.ino)

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  currentPalette = RainbowColors_p;
  //  currentPalette = SetupBlackAndWhiteStripedPalette();
  currentBlending = NOBLEND;

  Wire.begin();
  Serial.begin(115200); // a Python GUI-hoz igazitva (a regi 9800 a 9600 elgepelese volt)

  pinMode(PIN_A0, INPUT); // Ball 1
  pinMode(PIN_A1, INPUT); // Ball 2
  pinMode(PIN_A2, INPUT); // Ball 3
  pinMode(PIN_A3, INPUT); // Ball 4
  pinMode(PIN_A4, INPUT); // Ball 5
  pinMode(PIN_A5, INPUT); // UFO
  pinMode(a8, OUTPUT);
  pinMode(a10, INPUT);
  pinMode(PIN_A12, INPUT_PULLUP); // Tilt SW
  pinMode(PIN_A13, OUTPUT); // Reset
  pinMode(rightSlingshotSwitch, INPUT_PULLUP);
  pinMode(ballTroughCoil, OUTPUT);
  pinMode(shooterlaneCoil, OUTPUT);
  pinMode(chongSwitch, INPUT_PULLUP);
  pinMode(d8, INPUT_PULLUP);
  pinMode(d9, INPUT_PULLUP);
  pinMode(d10, INPUT_PULLUP);
  pinMode(d11, INPUT_PULLUP);
  pinMode(cheechSwitch, INPUT_PULLUP);
  pinMode(shooterLaneSwitch, INPUT_PULLUP);
  pinMode(cncLetterC1st, INPUT_PULLUP);
  pinMode(cncLetterN, INPUT_PULLUP);
  pinMode(fishTankSwitch1, INPUT_PULLUP);
  pinMode(fishTankSwitch2, INPUT_PULLUP);
  pinMode(daveLaneSwitchA, INPUT_PULLUP);
  pinMode(daveLaneSwitchD, INPUT_PULLUP);
  // Pin 20 and 21 is reserved for Wire
  pinMode(daveLaneSwitchV, INPUT_PULLUP);
  pinMode(daveLaneSwitchE, INPUT_PULLUP);
  pinMode(rightFlipperBat, OUTPUT);
  pinMode(cncLetterC3rd, INPUT_PULLUP);
  pinMode(rightflipperButton, INPUT_PULLUP);
  pinMode(leftFlipperButton, INPUT_PULLUP);
  pinMode(rightSlingshotCoil, OUTPUT);
  pinMode(leftSlingshotCoil, OUTPUT);
  pinMode(loopSwitchTop, INPUT_PULLUP);
  pinMode(rightSlingshotSwitch, INPUT_PULLUP);
  pinMode(leftSlingshotSwitch, INPUT_PULLUP);
  pinMode(pop3Coil, OUTPUT);
  pinMode(pop1Coil, OUTPUT);
  pinMode(pop1Switch, INPUT_PULLUP);
  pinMode(pop2Switch, INPUT_PULLUP);
  pinMode(pop2Coil, OUTPUT);
  pinMode(ufoCoil, OUTPUT);
  pinMode(spinnerSwitch, INPUT_PULLUP);
  pinMode(pop3Switch, INPUT_PULLUP);
  pinMode(gateSwitch3, INPUT_PULLUP);
  pinMode(leftFlipperBat, OUTPUT);
  //pinMode(cheechAlleySwitch, INPUT_PULLUP);
  pinMode(loopSwitchSide, INPUT_PULLUP);
  pinMode(gateSwitch2, INPUT_PULLUP);
  pinMode(gateSwitch1, INPUT_PULLUP);
  // pin 18 (TX1) es 19 (RX1) a wavTrigger Serial1-e, ne hasznald masra!
  // pin 46 es 48 felszabadult (regen AltSoftSerial volt rajtuk)
  pinMode(startButton, INPUT_PULLUP);
  //pinMode(d49, INPUT_PULLUP);
  pinMode(bridgeLowSwitch, INPUT_PULLUP);
  pinMode(ballShooterButton, INPUT_PULLUP);
  pinMode(bridgeHighSwitch, INPUT_PULLUP);
  //pinMode(d53, INPUT_PULLUP);

  // Minden nagyaramu kimenet determinisztikusan LOW legyen meg a WAV Trigger
  // egy masodperces init-varakozasa es a CoilGuard megszakitas inditasa elott.
  // Ez a bootloaderbol/elozo sketchbol maradt PORT-latch impulzust is kizarja.
  const uint8_t coilOutputs[] = {
    (uint8_t)ballTroughCoil, (uint8_t)shooterlaneCoil,
    (uint8_t)leftSlingshotCoil, (uint8_t)rightSlingshotCoil,
    (uint8_t)pop1Coil, (uint8_t)pop2Coil, (uint8_t)pop3Coil,
    (uint8_t)ufoCoil, (uint8_t)leftFlipperBat, (uint8_t)rightFlipperBat
  };
  for (uint8_t i = 0; i < sizeof(coilOutputs); i++) {
    digitalWrite(coilOutputs[i], LOW);
  }

  //// Wav Trigger Init
  wTrig.start();
  delay(1000);
  wTrig.stopAllTracks();
  wTrig.masterGain(0);
  wTrig.trackPlayPoly(TRK_BOOT);

#ifndef SIM_MODE
  Serial.println("BOOT,NORMAL,115200");
#endif

  
  CoilGuardInit(); // tekercsvedelem (e_coil_guard.ino) - a pinMode-ok UTAN kell!
  SimInit();       // probapadi szimulator (f_sim_mode.ino) - eles buildben ures

  heysoundtimer = millis();
  digitalWrite(PIN_A13, HIGH);
}

void loop() {
  CoilGuardReport(); // jelzi a sorosra, ha a tekercsvedelem kozbelepett
  SimPoll();         // probapadi szimulator lepteto - eles buildben ures
  AnalogTestPoll();  // analog teszt-stream a szerviz menunek (h_analog_test.ino)
  if (intmon != 2) PollControlSerial(); // LT + MG_* soros protokoll; nevbevitelkor NEM, hogy ne utkozzon

  if (intmon != 0) { // 1 = attract, 2 = hiscore/nevbevitel, 3 = player select
    intmMode();
  }


  if (intmon == LOW) {
    if (MunchiesOwnsGameLoop()) {
      // A golyo a VUK-ban parkol: a fizikai jateklogika es a flippertekercsek
      // szunetelnek, de a soros input, watchdog es tekercsvedelem tovabb fut.
      MunchiesUpdate();
    }
    else {
      // Az egyszeri Initlights() nem eleg az overlayek ala: a teljesen
      // statikus LED-ek alapjat minden frame-ben ujra fel kell epiteni.
      RestoreStaticPlayfieldLeds();
      Multiball();
      Loopshoot();
      Ballhandler();
      Weed();
      Pops();
      Weedspinner();
      Gate();
      Initlights();
      Blinktimer();
      Left_Flipper();
      Right_Flipper();
      Right_Slingshot();
      Left_Slingshot();
      CnC();
      Fishtank();
      Chong_switch();
      Cheech_switch();
      UFOO();
      Dave_switch();
      BonusXLed();
      SendData();
      Ballsave();
      Collectives();
      BridgeLow();
      BridgeHigh();
      HurryUp();
      Tilt();
      GiftRunlight();     // gift-fazis futofeny az inaktiv postokon (Weed/CnC/Fishtank UTAN!)
#ifdef FX_PROFILE
      unsigned long fxProfileStart = micros();
#endif
      RunOverlayEffect(); // overlay/canvas effekt: rarajzol a jatek-fenyre (subsystemek UTAN!)
      RunLightEffect();   // full fenyeffekt-motor (effect == HIGH eseten atveszi a palyat)
      RunUfoWheelChaseLights(); // a Wheel szines pixelei ALATT latszo korbefuto fenynyalab
      RunHurryUpLights(); // proceduralis chase + lassan kihunyo talalati retegek
      RunHurryUpBakedOverlay(); // ID6 legfelso reteg; fekete/magenta = atlatszo
#ifdef FX_PROFILE
      static unsigned long fxProfileMax = 0, fxProfileReport = 0;
      unsigned long fxProfileElapsed = micros() - fxProfileStart;
      if (fxProfileElapsed > fxProfileMax) fxProfileMax = fxProfileElapsed;
      if (millis() - fxProfileReport >= 1000UL) {
        Serial.print("FX_US_MAX,"); Serial.println(fxProfileMax);
        fxProfileMax = 0; fxProfileReport = millis();
      }
#endif
    }
  }

  {
    static int startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    // A palettas futofeny mindig fut. effect==LOW: a fasz-tol (0 vagy 68 a
    // jatekallapottol fuggoen). effect==HIGH: a baked effekt birtokolja a
    // 0-67-et, de a 68-114 felso szalag TOVABB fut (start = 68), hogy ne
    // alljon meg a baked animacio ~masodpercei alatt.
    FillLEDsFromPaletteColors(startIndex, (effect == LOW) ? fasz : 68);
  }
  RunMunchiesLights(); // minijatek alatt a legutolso jatekfeny-reteg
  RunLightTest(); // szerviz-menu light test: ha aktiv, feluliria a leds[]-et (loopolva, csak vizualis)
  FastLED.show();
  // A FastLED.delay() a varakozas alatt ismetelten show()-t hivhat. WS2812-n
  // a show megszakitasokat tilt (~3.5 ms / 115 LED), ezert a folyamatos
  // ujrakuldes a Mega soros RX byte-jait elvesztette (AT_SAVE / MG_ALIVE).
  // A sima delay alatt az UART megszakitasok elnek, a parser a kovetkezo
  // loopban hianytalanul kiolvassa a hardveres puffert.
  delay(1000 / UPDATES_PER_SECOND);



}

///////////////////////////////////
///////////////////////////////////
//// BALLHANDLER
///////////////////////////////////
///////////////////////////////////

void Ballhandler() {
   boolean ballSaveFeedStarted = LOW;
   if(shoot==0){
    MIV(LOW); // Measure how many ball in the stack
   }

    /////////////////
    //// Firstball
    ////////////////

    if (firstplay == HIGH && shoot == 0 && BIS == 5) {
      if (BIS + BIP > 5) {
        BIS = BIS -1;
        shoottimer = millis();
        shoottimer2 = millis();
        shoot = 1;
        PlaySpeechRange(TRK_VO_CHEECH_BALL_LAUNCH_A);
      }
    }


    /////////////////
    //// Ballsave
    ////////////////

    if (ballsaversw == HIGH && shoot == 0) {
      MIV(HIGH);
      if (BIP != 5) {
        if (BIS + BIP > 5) {
          BIS = BIS - 1;
          shoottimer = millis();
          shoottimer2 = millis();
          shoot = 1;
          ballSaveFeedStarted = HIGH;
          TriggerHurryHit(HURRY_ZONE_BALLSAVE);
          if (firstplay == LOW) {
            startmus = LOW;
          }
        }
        if (sidelaneBallsaverSw == HIGH && BIS != 0) {
          sidelaneBallsaverSw = LOW;
          BIS = BIS - 1;
          shoottimer = millis();
          shoottimer2 = millis();
          shoot = 1;
          ballSaveFeedStarted = HIGH;
          TriggerHurryHit(HURRY_ZONE_BALLSAVE);
          if (firstplay == LOW) {
            startmus = LOW;
          }
        }
      }
      // SpaceCoke (BIP == 5): az utolso tarolt golyot NEM azonnal lokjuk ki,
      // hanem 1 mp varakozas utan (maxBallSw) - a gepben futott verziobol
      // atveve, a forditott idozites-feltetel javitasaval
      if (BIP == 5) {
        if (BIS + BIP > 5) {
          if (BIS > 1) {
            BIS = BIS - 1;
            shoottimer = millis();
            shoottimer2 = millis();
            shoot = 1;
            TriggerHurryHit(HURRY_ZONE_BALLSAVE);
            if (firstplay == LOW) {
              startmus = LOW;
            }
          }
          if (BIS == 1 && maxBallSw == LOW) {
            maxBallSw = HIGH;
            maxBallSwTimer = millis();
          }
        }
        if (sidelaneBallsaverSw == HIGH && BIS != 0) {
          sidelaneBallsaverSw = LOW;
          if (BIS > 1) {
            BIS = BIS - 1;
            shoottimer = millis();
            shoottimer2 = millis();
            shoot = 1;
            TriggerHurryHit(HURRY_ZONE_BALLSAVE);
            if (firstplay == LOW) {
              startmus = LOW;
            }
          }
          if (BIS == 1 && maxBallSw == LOW) {
            maxBallSw = HIGH;
            maxBallSwTimer = millis();
          }
        }
        if (maxBallSw == HIGH && millis() - maxBallSwTimer > 1000) {
          BIS = BIS - 1;
          shoottimer = millis();
          shoottimer2 = millis();
          shoot = 1;
          TriggerHurryHit(HURRY_ZONE_BALLSAVE);
          if (firstplay == LOW) {
            startmus = LOW;
          }
          maxBallSw = LOW;
        }
      }
      // Csak a tenyleges, egygolyos ball save kap Cheech-bemondast.
      // Az elso kiloves es a multiball alatti automatikus visszatoltes nem.
      if (ballSaveFeedStarted == HIGH && firstplay == LOW && multiball == 0) {
        PlaySpeechRange(TRK_VO_CHEECH_BALL_SAVE_A);
      }
    }

    ///////////////
    //// Nextball
    //////////////

    if (ballsaversw == LOW && shoot == 0 && firstplay == LOW) {
      if (BIS == 5 && bisFiveReads >= BALL_DRAIN_CONFIRM_READS &&
          millis() - bisFiveSince >= BALL_DRAIN_CONFIRM_MS) {
      digitalWrite(leftFlipperBat, LOW);
      digitalWrite(rightFlipperBat, LOW);
        wTrig.stopAllTracks();
        hurryUp = LOW;
        StopHurryUpLights();
        StopHurryUpBakedOverlay();
        maxBallSw = LOW;
        BIP = 1;

        if (numofplayers == player && ball == 3 && extraball == 0) {
          Serial.println("End");
          delay(20);
          wTrig.trackPlayPoly(TRK_MISSU);
        }
        else {
          Serial.println("Next");
          delay(20);
          Serial.flush();
          wTrig.trackPlaySolo(TRK_LARDASS);
          delay(4500);
        }
        shoottimer = millis();
        shoottimer2 = millis();

        firstplay = HIGH;
        AutoKick = LOW;
        initlight = HIGH;
        Initlights();
        Inittable = HIGH;
        inittable();

        // Commercial tilt: az aktualis golyo felhalmozott bonusza elvész.
        // Az inittable() szandekosan mar e blokk elott lefut, ezert a
        // ballTilted flaget csak ITT, a bonus dontese utan szabad torolni.
        // A jatekos utolso golyoja utan a betarazott (az UFO altal el nem
        // vett) joint a bonuszban fizet. Csak az egy-jointos allapot juthat
        // ide: kettot es harmat mar az UFO elvesz. A bonuszba tesszuk, hogy
        // a bonusz-szorzo is vonatkozzon ra.
        if (ball == 3 && extraball == 0 && jointStack[player] > 0) {
          uint8_t heldIndex = jointStack[player] - 1;
          if (heldIndex > 2) heldIndex = 2;
          Score(Scoring::JOINT_POINTS[heldIndex], Scoring::JOINT_BONUS[heldIndex]);
          wTrig.trackPlayPoly(TRK_BIGJOINT);
          uint8_t heldBeers = jointStack[player];
          jointStack[player] = 0;
          beerCredits[player] = (beerCredits[player] > heldBeers)
                                  ? (uint8_t)(beerCredits[player] - heldBeers) : 0;
          SendPartyState();
        }

        if (ballTilted == LOW) {
          if (bonusx == 1) {
            bonus = bonus * 2;
          }
          if (bonusx == 2) {
            bonus = bonus * 4;
          }
          if (bonusx == 3) {
            bonus = bonus * 6;
          }
          if (bonusx == 4) {
            bonus = bonus * 8;
          }
          score[player] = score[player] + bonus;
        }
        bonusx = 0;
        bonus = 0;
        ballTilted = LOW;
        SendData();
        if (extraball == 0)
        {
          player = player + 1;
          startmus = HIGH;
          firstplay = HIGH;
          weedmetersend();
          if (player > numofplayers) {
            player = 1;
            ball = ball + 1;
            if (ball == 4) {
              initlight = HIGH;
              Initlights();
              intmon = 2;
            }
            else {
              shoot = 1;
            }
          }
        }
        else {
          extraball = extraball - 1;
          shoot = 1;
          // Az Extra Ball kovetkezo golyojanak fix, felismerheto inditohangja.
          wTrig.trackPlaySolo(TRK_VO_CHONG_EXTRA_BALL_A);
          startmus = HIGH;
          firstplay = HIGH;
        }
        if (intmon == 0) {
          RestorePartyShotsForPlayer();
          SendPartyState();
        }
      }
    }


  ////////////////////////
  //// Ejectball
  ////////////////////////
  // A tekercs MINDEN mas esetben LOW-t kap - ha a kilokes kozben barmi
  // nullazna a shoot-ot, a tekercs korabban HIGH-ban ragadt (CoilGuard fogta meg)
  if (shoot == 1 && millis() - shoottimer <= 50) {
    digitalWrite(ballTroughCoil, HIGH);
  }
  else {
    digitalWrite(ballTroughCoil, LOW);
    if (shoot == 1) {
      firstplay = LOW;
    }
  }


  /////////////////////////////////////////////////
  //// Ballkick
  /////////////////////////////////////////////////

  const unsigned long shooterNow = millis();
  const boolean shooterLaneClosed = (SimDigitalRead(shooterLaneSwitch) == LOW);

  // A visszagurulo golyo uj zarasa inditja az 1 mp-es varakozast. Ha a
  // kontaktus felenged, a varakozas torlodik; ures savra nem rugunk ra.
  if (shooterLaneClosed) {
    if (shooterLaneWasClosed == LOW) {
      shooterLaneWasClosed = HIGH;
      shooterLaneClosedAt = shooterNow;
    }
  }
  else {
    shooterLaneWasClosed = LOW;
    shooterLaneClosedAt = 0;
  }

  if (shooterLaneClosed && kick == 0) {
    if (AutoKick == LOW && SimDigitalRead(ballShooterButton) == LOW &&
        shooterNow - shoottimer2 > 500UL) {
      kick = 1;
      kicktimer = shooterNow;
      AutoKick = HIGH;
      // Ha a golyo a gyenge rugas utan rajta marad a kapcsolon, innen
      // szamitva kapjon egy teljes masodpercet az ujrarugasig.
      shooterLaneClosedAt = shooterNow;
      ballsavetimer = millis(); // a ballsave BEALLITASANAK ideje (rollover-biztos)
      ballsaversw = HIGH;
      ballsavetime = 15000;
    }
    else if (AutoKick == HIGH &&
             shooterNow - shooterLaneClosedAt >= SHOOTER_LANE_REKICK_MS &&
             shooterNow - kicktimer >= SHOOTER_LANE_REKICK_MS) {
      kick = 1;
      kicktimer = shooterNow;
      shooterLaneClosedAt = shooterNow;
    }
  }



  if (kick == 1) {
    digitalWrite(shooterlaneCoil, HIGH);
    if (millis() - 50 > kicktimer) {
      digitalWrite(shooterlaneCoil, LOW);

      // A tema es a Shooter (ID3) csak az aktualis golyo ELSO kilovesenek
      // visszajelzese. A flaget itt elfogyasztjuk, igy egy visszagurulas
      // automatikus ujrarugasa es a ball-save visszaadasa teljesen csendes.
      if (startmus == HIGH) {
        wTrig.trackPlayPoly(TRK_THEME);
        startmus = LOW;
        PlayBakedEffectOnce(3);
      }
      kick = 0;
      shoot = 0;
      shootfail = 0;
    }
  }

  // Rescue: ha a kilokes elindult (shoot == 1), de a golyo nem ert a kilovosavba,
  // a gomb ~3 mp nyomva tartasa ujra kiloki. Jatek kozben (shoot == 0) nem csinal semmit.
  if (SimDigitalRead(ballShooterButton) == LOW && shoot == 1 && kick == 0){
    shootfail ++;
    if (shootfail > 300){
    shoottimer = millis();
    shoottimer2 = millis();
    shoot = 1;
    kick = 0;
    shootfail=0;
    }
    }
  else {
    shootfail = 0;
  }
  }
///////////////////////////////////
//// Mesure ballstack infravalue
///////////////////////////////////
void MIV(boolean m) {
  Blinktimer();
  if (BallReadState == HIGH || m == HIGH) {
    ball1 = SimAnalogRead(PIN_A0);
    ball2 = SimAnalogRead(PIN_A1);
    ball3 = SimAnalogRead(PIN_A2);
    ball4 = SimAnalogRead(PIN_A3);
    ball5 = SimAnalogRead(PIN_A4);

    // FORDITOTT logika (a gepben futott verziobol atveve):
    // alacsony analog ertek = golyo ott van! A kuszob mar nincs bedrotozva,
    // szenzoronkent az EEPROM-bol jon (analogThreshold[], h_analog_test.ino),
    // igy a szerviz menubol hangolhato ujraflashelés nelkul.
    ballPresent1 = (ball1 < analogThreshold[0]) ? 1 : 0;

    ballPresent2 = (ball2 < analogThreshold[1]) ? 1 : 0;

    ballPresent3 = (ball3 < analogThreshold[2]) ? 1 : 0;

    ballPresent4 = (ball4 < analogThreshold[3]) ? 1 : 0;

    ballPresent5 = (ball5 < analogThreshold[4]) ? 1 : 0;

      if (BIP != 5){
      BIS = ballPresent1 + ballPresent2 + ballPresent3 + ballPresent4 + ballPresent5;
      }
      else
      {
        if (ballPresent1 == 1 && ballPresent2 == 0 && ballPresent3 == 0 && ballPresent4 == 0 && ballPresent5 == 0)
        {
          BIS = 1;
          }
        if (ballPresent1 == 1 && ballPresent2 == 1 && ballPresent3 == 0 && ballPresent4 == 0 && ballPresent5 == 0)
        {
          BIS = 2;
          }
        if (ballPresent1 == 1 && ballPresent2 == 1 && ballPresent3 == 1 && ballPresent4 == 0 && ballPresent5 == 0)
        {
          BIS = 3;
          }
        if (ballPresent1 == 1 && ballPresent2 == 1 && ballPresent3 == 1 && ballPresent4 == 1 && ballPresent5 == 0)
        {
          BIS = 4;
          }
        if (ballPresent1 == 1 && ballPresent2 == 1 && ballPresent3 == 1 && ballPresent4 == 1 && ballPresent5 == 1)
        {
          BIS = 5;
          }
        if (ballPresent1 == 0 && ballPresent2 == 0 && ballPresent3 == 0 && ballPresent4 == 0 && ballPresent5 == 0)
        {
          BIS = 0;
          }
        }

    // Csak a TENYLEGES meresek szamitanak: a BallReadState 1 mp-es ablakai
    // kozott a BIS erteke valtozatlanul all, azt nem szabad megerositesnek
    // venni. Egyetlen zajos minta nullazza a szamlalot.
    if (BIS == 5) {
      if (bisFiveReads == 0) bisFiveSince = millis();
      if (bisFiveReads < 255) bisFiveReads++;
    }
    else {
      bisFiveReads = 0;
      bisFiveSince = 0;
    }
        
  }

}


/////////////////////////////////////////////////
//// Ballsaver
/////////////////////////////////////////////////

void Ballsave() {
  // kanonikus, rollover-biztos forma: eltelt-e mar a ballsave-ido?
  // A '!= 0' ort azert kell, hogy a be-nem-allitott (0) timer inaktivnak
  // szamitson - pont ugy, ahogy a regi '+30000'-es forma is tette.
  if (ballsaversw == HIGH && ballsavetimer != 0 && millis() - ballsavetimer < (unsigned long)ballsavetime) {
    Blinktimer();
    if (ledState == LOW) {
      leds[LED_BALLSAVE] = CRGB::Black; // x4
    }
    if (ledState == HIGH) {
      leds[LED_BALLSAVE] = CRGB::Red; // x4
    }
  }
  else {
    ballsaversw = LOW;
    // a sidelane-zaszlot is torolni kell, kulonben ha a mento a felhasznalas
    // elott jar le, a zaszlo beragad es a KOVETKEZO golyonal fantom-golyot lok ki
    sidelaneBallsaverSw = LOW;
  }
  if (ballsaversw == LOW) {
    leds[LED_BALLSAVE] = CRGB::Black; // x4
  }
}

/////////////////////////////////////////////////
//// Multiball
/////////////////////////////////////////////////
void Multiball() {
  // BIS >= 4: ha ket golyo egy mintaveteli ablakban (1s) folyik le, a BIS 3-rol
  // egybol 5-re ugrik es a == 4 sosem teljesulne -> a multiball beragadna!
  if (multiball != 0 && ballsaversw == LOW && BIS >= 4) {
    if (multiball == 1) {
      wTrig.trackPause(TRK_MUS_STRAWBERRY2);
      wTrig.trackLoop(TRK_MUS_STRAWBERRY2, 0);
      wTrig.trackResume(TRK_THEME);
    }
    if (multiball == 2) {
      wTrig.trackPause(TRK_MUS_ROCKFIGHT);
      wTrig.trackLoop(TRK_MUS_ROCKFIGHT, 0);
      wTrig.trackResume(TRK_THEME);
    }
    if (multiball == 3) {
      wTrig.trackPause(TRK_MUS_TRIPLE_LOOP3);
      wTrig.trackLoop(TRK_MUS_TRIPLE_LOOP3, 0);
      wTrig.trackResume(TRK_THEME);
    }
    if (multiball == 4) {
      wTrig.trackPause(TRK_MUS_SWING_LOOP4);
      wTrig.trackLoop(TRK_MUS_SWING_LOOP4, 0);
      wTrig.trackResume(TRK_THEME);
    }
    if (multiball == 5) {
      wTrig.trackPause(TRK_MUS_SPACECOKE);
      wTrig.trackLoop(TRK_MUS_SPACECOKE, 0);
      wTrig.trackResume(TRK_THEME);
    }
    multiball = 0;
    RestorePartyShotsForPlayer();
    SendPartyState();
    multiloopsw = 0;
    highLoopArmT = 0;
    BIP = 1;
    BrdgLowActive = LOW;
    BrdgHighActive = LOW;
  }
}



/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Ballhadler rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Timers
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void Blinktimer() {
  unsigned long BallcurrentMillis = millis();
  unsigned long currentMillis = millis();
  unsigned long blinkycurrentMillis = millis();
  unsigned long cigarcurrentMillis = millis();
  unsigned long effectcurrentMillis = millis();
  unsigned long hurryUpcurrentMillis = millis();

  if (BallcurrentMillis - BallpreviousMillis >= 1000) {
    // save the last time you blinked the LED
    BallpreviousMillis = BallcurrentMillis;

    // if the LED is off turn it on and vice-versa:
    if (BallReadState == LOW) {
      BallReadState = HIGH;
    } else {
      BallReadState = LOW;
    }
  }
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }
  if (blinkycurrentMillis - blinkypreviousMillis >= 500) {
    // save the last time you blinked the LED
    blinkypreviousMillis = blinkycurrentMillis;

    // if the LED is off turn it on and vice-versa:
    if (blinky == LOW) {
      blinky = HIGH;
    } else {
      blinky = LOW;
    }
  }
  if (cigarcurrentMillis - cigarpreviousMillis >= 50) {
    // save the last time you blinked the LED
    cigarpreviousMillis = cigarcurrentMillis;

    // if the LED is off turn it on and vice-versa:
    if (cigarState == LOW) {
      cigarState = HIGH;
    } else {
      cigarState = LOW;
    }
  }
  if (effectcurrentMillis - effectpreviousMillis >= 30) {
    // save the last time you blinked the LED
    effectpreviousMillis = effectcurrentMillis;

    // if the LED is off turn it on and vice-versa:
    if (effectState == LOW) {
      effectState = HIGH;
    } else {
      effectState = LOW;
    }
  }
  if (hurryUpcurrentMillis - hurryUppreviousMillis >= 1500) {
    // save the last time you blinked the LED
    hurryUppreviousMillis = hurryUpcurrentMillis;

    // if the LED is off turn it on and vice-versa:
    if (hurryUpState == LOW) {
      hurryUpState = HIGH;
    } else {
      hurryUpState = LOW;
    }
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Times rutins
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Light handling
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void FillLEDsFromPaletteColors(uint8_t colorIndex, int startLed) {
    uint8_t brightness = 255;

    for (int i = startLed; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void ChangePalettePeriodically() {
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if (lastSecond != secondHand) {
        lastSecond = secondHand;
        if (secondHand == 0) {
            currentPalette = RainbowColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 10) {
            currentPalette = RainbowStripeColors_p;
            currentBlending = NOBLEND;
        }
        if (secondHand == 15) {
            currentPalette = RainbowStripeColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 20) {
            SetupPurpleAndGreenPalette();
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 25) {
            SetupTotallyRandomPalette();
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 30) {
            SetupBlackAndWhiteStripedPalette();
            currentBlending = NOBLEND;
        }
        if (secondHand == 35) {
            SetupBlackAndWhiteStripedPalette();
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 40) {
            currentPalette = CloudColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 45) {
            currentPalette = PartyColors_p;
            currentBlending = LINEARBLEND;
        }
        if (secondHand == 50) {
            currentPalette = myRedWhiteBluePalette_p;
            currentBlending = NOBLEND;
        }
        if (secondHand == 55) {
            currentPalette = myRedWhiteBluePalette_p;
            currentBlending = LINEARBLEND;
        }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
    for (int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV(random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.

void SetupBlackAndGreenStripedPalette() {
    // 'black out' all 16 palette entries...
    fill_solid(currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::Red;
    currentPalette[4] = CRGB::Red;
    currentPalette[8] = CRGB::Red;
    currentPalette[12] = CRGB::Red;

}

void SetupBlackAndWhiteStripedPalette() {
    // 'black out' all 16 palette entries...
    fill_solid(currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB(255, 255, 255);
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
    CRGB purple = CHSV(HUE_PURPLE, 255, 255);
    CRGB green = CHSV(HUE_GREEN, 255, 255);
    CRGB black = CRGB::Black;

    currentPalette = CRGBPalette16(
        green, green, black, black,
        purple, purple, black, black,
        green, green, black, black,
        purple, purple, black, black);
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
  CRGB::Red,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Blue,
  CRGB::Black,
  CRGB::Black
};
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Light handling Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// intm mode and player select
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void intmMode() {
  if (intmon == 1) {
    fasz = 0;
    ChangePalettePeriodically();
    incomeMsg = " ";
      
      /*
      miv1 = SimAnalogRead(a1);
      miv2 = SimAnalogRead(a3);
      miv3 = SimAnalogRead(a4);
      miv4 = SimAnalogRead(a5);
      miv5 = SimAnalogRead(a6);
      Serial.println(miv1);
      Serial.println(miv2);
      Serial.println(miv3);
      Serial.println(miv4);
      Serial.println(miv5);
      Serial.println(SimAnalogRead(a7));
      Serial.println(SimAnalogRead(a2));
      
      Serial.println("----");
      delay(700);
      */

    if(millis()>heysoundtimer + 10000){
      heysoundcounter ++;
      heysoundtimer = millis();
      if(heysoundcounter == 36){
        heysoundcounter = 0;
        wTrig.trackPlayPoly(TRK_CHEECHFART);
        }
      }

    
    if (SimDigitalRead(startButton) == LOW) {
      // 1. start: jatekosvalaszto mod - a jatek meg NEM indul!
      // (V2-es CharmMode logika visszahozva)
      wTrig.trackPlayPoly(TRK_LETSPLAY);
      Serial.println("Start"); // GUI: kilep az attractbol a SCORE kepernyore
      delay(20);
      numofplayers = 1;
      selArmSw = LOW;          // az inditashoz elobb el kell engedni a startot
      selShootSw = HIGH;       // a shoot gombot is elesiteni kell
      selShootTimer = millis();
      selTimeoutTimer = millis();
      intmon = 3;
      SendData();
    }
  }

  //// Player select mod: shoot gomb = +1 jatekos, 2. start = jatek indul
  if (intmon == 3) {
    SendData(); // folyamatos pontszam/jatekosszam kuldes a GUI-nak

    if (selArmSw == LOW && SimDigitalRead(startButton) == HIGH) {
      selArmSw = HIGH; // a belepo startnyomas elengedve -> a kovetkezo mar indit
    }
    if (selShootSw == HIGH && millis() - 250 > selShootTimer && SimDigitalRead(ballShooterButton) == HIGH) {
      selShootSw = LOW;
    }

    // Shoot / player select gomb: +1 jatekos (4 utan korbefordul 1-re)
    if (SimDigitalRead(ballShooterButton) == LOW && selShootSw == LOW) {
      selShootSw = HIGH;
      selShootTimer = millis();
      selTimeoutTimer = millis();
      numofplayers = numofplayers + 1;
      if (numofplayers == 5) {
        numofplayers = 1;
      }
      wTrig.trackPlayPoly(TRK_ADDPLAYER);
      SendData();
    }

    // 2. start: jatek inditasa a kivalasztott jatekosszammal
    if (selArmSw == HIGH && SimDigitalRead(startButton) == LOW) {
      wTrig.trackPlayPoly(TRK_WEED);
      Serial.println("Zero");
      delay(300);
      intmon = 0;
      fasz = 68;
      ball = 1;
      player = 1;
      firstplay = HIGH;
      startmus = HIGH;
      BIP = 1;
      Inittable = HIGH;
      inittable();
      initlight = HIGH;
      Initlights();
      score[0] = 0;
      weedm[0] = 0;
      weedmeter[0] = 180;
      score[1] = 0;
      weedm[1] = 0;
      weedmeter[1] = 180;
      score[2] = 0;
      weedm[2] = 0;
      weedmeter[2] = 180;
      score[3] = 0;
      weedm[3] = 0;
      weedmeter[3] = 180;
      score[4] = 0;
      weedm[4] = 0;
      weedmeter[4] = 180;
      for (uint8_t p = 0; p <= 4; p++) {
        beerCredits[p] = 0;
        jointStack[p] = 0;
        weedQualified[p] = LOW;
      }
      RestorePartyShotsForPlayer();
      SendPartyState();
      weedmetersend();
      feny[0] = 960;
      miv1 = SimAnalogRead(a1);
      miv2 = SimAnalogRead(a3);
      miv3 = SimAnalogRead(a4);
      miv4 = SimAnalogRead(a5);
      miv5 = SimAnalogRead(a6);
      cheechCollectives[0] = 0;
      chongCollectives[0] = 0;
      cheechCollectives[1] = 0;
      chongCollectives[1] = 0;
      cheechCollectives[2] = 0;
      chongCollectives[2] = 0;
      cheechCollectives[3] = 0;
      chongCollectives[3] = 0;
      cheechCollectives[4] = 0;
      chongCollectives[4] = 0;
      bonus = 0;
      bonusx = 0;
      ballTilted = LOW;
    }

    // 60 mp tetlenseg: vissza az attract modba
    if (millis() - selTimeoutTimer > 60000) {
      intmon = 1;
      numofplayers = 1;
      heysoundtimer = millis();
      Serial.println("Attract"); // GUI: attract-loop ujraindul
      delay(20);
    }
  }
  if (intmon == 2)
  {

    if (Serial.available() > 0 && intmon == 2) {
      // read the incoming:
      incomeMsg = Serial.readString();
    }

    //// Send buttonstates to Highscore table
    if (SimDigitalRead(leftFlipperButton) == LOW && lfHscSw == 0) {
      lfHscSw = 1;
      lfHscTimer = millis();
      wTrig.trackPlayPoly(TRK_KEYLEFT);
      Serial.println("FLIPPER_LEFT"); // Python GUI protokoll (regi Unity-nev: "Right")
      delay(20);
    }
    if (lfHscSw == 1 && millis() - 250 > lfHscTimer && SimDigitalRead(leftFlipperButton) == HIGH) {
      lfHscSw = 0;
    }

    if (SimDigitalRead(rightflipperButton) == LOW && rfHscSw == 0) {
      rfHscSw = 1;
      rfHscTimer = millis();
      wTrig.trackPlayPoly(TRK_KEYRIGHT);
      Serial.println("FLIPPER_RIGHT"); // Python GUI protokoll (regi Unity-nev: "Left")
      delay(20);
    }
    if (rfHscSw == 1 && millis() - 250 > rfHscTimer && SimDigitalRead(rightflipperButton) == HIGH) {
      rfHscSw = 0;
    }

    if (SimDigitalRead(ballShooterButton) == LOW && shHscSw == 0) {
      shHscSw = 1;
      shHscTimer = millis();
      wTrig.trackPlayPoly(TRK_NEXTLETTER);
      Serial.println("PLAYER_PRESS"); // Python GUI protokoll: betu kivalasztasa
      delay(20);
    }
    if (shHscSw == 1 && millis() - 250 > shHscTimer && SimDigitalRead(ballShooterButton) == HIGH) {
      shHscSw = 0;
    }

    if (SimDigitalRead(startButton) == LOW && stHscSw == 0) {
      stHscSw = 1;
      stHscTimer = millis();
      Serial.println("Start"); // GUI: nevbevitel kihagyasa (skip)
      delay(20);
    }
    if (stHscSw == 1 && millis() - 250 > stHscTimer && SimDigitalRead(startButton) == HIGH) {
      stHscSw = 0;
    }


    if (incomeMsg == "Exit") {
        digitalWrite(PIN_A13, LOW);
    }
    if (incomeMsg == "Exit1") {
        wTrig.trackPlayPoly(TRK_HISCORE_SKIP);
        digitalWrite(PIN_A13, LOW);
    }
    if (incomeMsg == "Exit2") {
      wTrig.trackPlayPoly(TRK_ADDSCORE);
      digitalWrite(PIN_A13, LOW);
    }
    if (SimDigitalRead(startButton) == LOW && SimDigitalRead(ballShooterButton) == LOW) {
        resetTimer++;
        if (resetTimer > 2000) {
            digitalWrite(PIN_A13, LOW);
        }
    }
  }

}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End intm mode && player select rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Inittable
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void inittable() {
  if (Inittable == HIGH) {
    ResetTiltWarningsForBall();
    cncswitch1 = 0;
    cncswitch2 = 0;
    cncswitch3 = 0;
    weedswitch1 = 0;
    weedswitch2 = 0;
    weedswitch3 = 0;
    weedswitch4 = 0;
    fishTankLightState1 = 0;
    fishTankLightState2 = 0;
    chongLightActiveSw = 0;
    cheechLightActiveSw = 0;
    chongactive = 0;
    ufosw = 0;
    extraBallLit = LOW; // az EXTRA BALL LIT csak az aktualis golyoig el
    davearr[1] = 0;
    davearr[2] = 0;
    davearr[3] = 0;
    davearr[4] = 0;
    gatearr[1] = 0;
    gatearr[2] = 2;
    gatearr[3] = 0;
    BrdgLowActive = LOW;
    BrdgHighActive = LOW;
    highLoopArmT = 0;
    bonusx1sw = 0;
    bonusx2sw = 0;
    bonusx3sw = 0;
    bonusx4sw = 0;
    spinnersw = 0;
    Inittable = LOW;
    weedmetersend();
    randGift = random(1, 10);
    switch (randGift) {
      case 1:
        cncswitch1 = 2;
        break;
      case 2:
        cncswitch2 = 2;
        break;
      case 3:
        cncswitch3 = 2;
        break;
      case 4:
        weedswitch1 = 2;
        break;
      case 5:
        weedswitch2 = 2;
        break;
      case 6:
        weedswitch3 = 2;
        break;
      case 7:
        weedswitch4 = 2;
        break;
      case 8:
        fishTankLightState1 = 2;
        break;
      case 9:
        fishTankLightState2 = 2;
        break;
    }
    giftsw = 1;
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Inittable Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Data send
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void SendData() {
  if (sendDsw == LOW) {
    sendDsw = HIGH;
    sendDtimer = millis();
  }
  if (millis() - 350 > sendDtimer && sendDsw == HIGH) {
    char scoremsg[64];
    snprintf(scoremsg, sizeof(scoremsg), "score,%lu,%d,%d,%d,%lu,%d",
             score[player], numofplayers, player, ball, bonus, bonusx);
    Serial.println(scoremsg);
    sendDsw = LOW;
  }


}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Data send rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Score
/////////////////////////////////////////////////
/////////////////////////////////////////////////

unsigned long DirectScorePoints(unsigned long basePoints) {
  return hurryUp == HIGH ? basePoints * Scoring::HURRY_UP_MULTIPLIER : basePoints;
}

unsigned long JackpotScorePoints(unsigned long basePoints) {
  const unsigned long points = DirectScorePoints(basePoints);
  // A szorzo UTAN igazitunk, hogy a video, a bemondas es a jovairas ugyanazt
  // az osszeget mondja. Felfele lepunk a legkozelebbi letezo szintre; a
  // keszlet teteje egyben plafon is, kulonben a SpaceCoke-kombo Hurry Up
  // alatti 400000-e klip nelkul maradna.
  for (uint8_t i = 0; i < 8; i++) {
    if (Scoring::JACKPOT_TIERS[i] >= points) return Scoring::JACKPOT_TIERS[i];
  }
  return Scoring::JACKPOT_TIERS[7];
}

void PlayJackpotFeedback(unsigned long basePoints) {
  // A modszam nem azonositja a pontot: a ket hid jackpot-tablaja elter.
  // A GUI pontosan a ScoreJackpot() altal jovairt osszeghez valaszt videot.
  unsigned long points = JackpotScorePoints(basePoints);
  Serial.print("Jackpot_");
  Serial.println(points);
  // ID19: 40 x 50 ms = 2 s; ket teljes kor a jackpot video/hang mellett.
  StartFullBakedEffect(19, 2, LOW);

  uint16_t firstTrack = 0;
  switch (points) {
    case 10000UL:  firstTrack = TRK_VO_JACKPOT_10000_A;  break;
    case 15000UL:  firstTrack = TRK_VO_JACKPOT_15000_A;  break;
    case 20000UL:  firstTrack = TRK_VO_JACKPOT_20000_A;  break;
    case 25000UL:  firstTrack = TRK_VO_JACKPOT_25000_A;  break;
    case 30000UL:  firstTrack = TRK_VO_JACKPOT_30000_A;  break;
    case 50000UL:  firstTrack = TRK_VO_JACKPOT_50000_A;  break;
    case 100000UL: firstTrack = TRK_VO_JACKPOT_100000_A; break;
  }
  if (firstTrack != 0) {
    // 50/50 Cheech vagy Chong; a haromvaltozatos alapertelmezes itt TILOS.
    PlaySpeechRange(firstTrack, 2);
  }
  else {
    // Jovobeli, meg nem felmondott osszegnel se mondjunk hibas pontot.
    wTrig.trackPlayPoly(TRK_JACKPOT);
  }
}

void AddAwardedScore(unsigned long scr, unsigned long bns) {
  // Mar vegleges pontosszeg: itt sem szorzot, sem jackpot-emelest nem adunk.
  bonus = bonus + bns;
  score[player] = score[player] + scr;
#ifdef SIM_MODE
  // Tesztpadi trace: minden pontozas lathato a sorosonn (choke-point,
  // ezen fut at az osszes alrendszer). Eles buildben nincs itt semmi.
  char tb[52];
  snprintf(tb, sizeof(tb), "T,score,p%d,scr=%lu,bns=%lu,tot=%lu",
           player, scr, bns, score[player]);
  Serial.println(tb);
#endif
}

void Score(unsigned long scr, unsigned long bns) {
  // Altalanos Hurry Up: 2x kozvetlen pont, valtozatlan golyovegi bonusz.
  AddAwardedScore(DirectScorePoints(scr), bns);
}

void ScoreJackpot(unsigned long scr, unsigned long bns) {
  AddAwardedScore(JackpotScorePoints(scr), bns);
}

// Legalabb minimumMs vedelmet biztosit, de egy mar futo hosszabb save-et
// (peldaul kilovesi vagy multiball save-et) sosem rovidit le.
void EnsureBallSave(unsigned long minimumMs) {
  unsigned long now = millis();
  unsigned long remaining = 0;
  if (ballsaversw == HIGH && ballsavetimer != 0 &&
      now - ballsavetimer < (unsigned long)ballsavetime) {
    remaining = (unsigned long)ballsavetime - (now - ballsavetimer);
  }
  if (remaining < minimumMs) {
    ballsaversw = HIGH;
    ballsavetime = (int)minimumMs;
    ballsavetimer = now;
  }
}

void StartUfoEjectBallSave(unsigned long minimumMs) {
  if (ufoEjectSaveStarted == LOW) {
    ufoEjectSaveStarted = HIGH;
    EnsureBallSave(minimumMs);
  }
}

boolean ExtraBallLotteryBlocked() {
  return (extraball > 0 || extraBallLit == HIGH);
}

uint8_t CurrentUfoPartyTier() {
  // 3 joint = Love Pack (SpaceCoke), 2 joint = Feature Wheel.
  // Egy joint ONMAGABAN inaktiv, igy tovabb gyujtheto. Ha melle ujra
  // kigyullad a WEED, az UFO Super Cashout lotteryt ad, de a jointot nem
  // fogyasztja el. Joint nelkul a WEED a sima Cashoutot nyitja.
  if (jointStack[player] >= 3) return UFO_PARTY_LOVE_PACK;
  if (jointStack[player] == 2) return UFO_PARTY_FEATURE_WHEEL;
  if (jointStack[player] == 1) {
    return (weedQualified[player] == HIGH)
             ? UFO_PARTY_SUPER_CASHOUT : UFO_PARTY_NONE;
  }
  if (weedQualified[player] == HIGH) return UFO_PARTY_CASHOUT;
  return UFO_PARTY_NONE;
}

boolean RollJointLit() {
  // Az N. jointhoz N sor kell: az elsohoz 1, a masodikhoz 2, a harmadikhoz 3.
  // A jointok igy nem "fogyasztjak" a sort, hanem egyre tobbet kovetelnek meg
  // belole, tehat a ket szamlalo egyutt telik meg haromig.
  return (weedQualified[player] == HIGH &&
          beerCredits[player] >= jointStack[player] + 1 &&
          jointStack[player] < 3 && multiball == 0 && hurryUp == LOW);
}

void SendPartyState() {
  char msg[40];
  snprintf(msg, sizeof(msg), "Party,%d,%u,%u,%u,%u",
           player, beerCredits[player], jointStack[player],
           CurrentUfoPartyTier(), weedQualified[player] == HIGH ? 1 : 0);
  Serial.println(msg);
}

void SendPartyEvent(const char* eventName) {
  Serial.print("PartyEvent,");
  Serial.print(player);
  Serial.print(",");
  Serial.println(eventName);
}

// Egymas utani voice-overek kozul valaszt (alapbol 3, jackpotnal 2). uint16_t kell,
// mert az uj UFO-csomag egy resze a 255-os trackazonosito folott van.
void PlaySpeechRange(uint16_t firstTrack, uint8_t count) {
  uint16_t track = firstTrack + (uint16_t)random(0, count);
  wTrig.trackPlayPoly((int)track);
#ifdef SIM_MODE
  char tb[28];
  snprintf(tb, sizeof(tb), "T,speech,%u", track);
  Serial.println(tb);
#endif
}

void RestorePartyShotsForPlayer() {
  if (multiball != 0 || hurryUp == HIGH) {
    ufosw = 0;
    spinnersw = 2;
    return;
  }
  ufosw = (CurrentUfoPartyTier() != UFO_PARTY_NONE) ? 1 : 0;
  spinnersw = (weedQualified[player] == HIGH) ? 1 : 0;
}

void ConsumeUfoPartyReward(uint8_t tier) {
  weedQualified[player] = LOW;
  spinnersw = 0;
  // Csak a Feature Wheel es a Love Pack fogyasztja el a jointokat. A Super
  // Cashout elhasznalja a friss WEED-et, de az egy jointot meghagyja tovabbi
  // gyujteshez. A magasabb tiereknel a jointokkal EGYUTT a hozzajuk gyujtott
  // sor is elfogy - ket joint ket sort visz.
  if (tier >= UFO_PARTY_FEATURE_WHEEL) {
    uint8_t spent = jointStack[player];
    jointStack[player] = 0;
    beerCredits[player] = (beerCredits[player] > spent)
                            ? (uint8_t)(beerCredits[player] - spent) : 0;
  }
  ufosw = 0;
  SendPartyState();
}

void RollJoint() {
  if (!RollJointLit()) return;

  // A sor NEM fogy el a jointtol: a ket szamlalo egymastol fuggetlenul
  // gyulik 3-ig, es a Love Packhoz mindketto kell. A RollJointLit() tovabbra
  // is megkovetel legalabb egy sort ahhoz, hogy egyaltalan lehessen sodorni.
  jointStack[player]++;
  weedQualified[player] = LOW;
  spinnersw = 0;
  // Az UFO allapota a szintbol jon: egy jointnal inaktiv marad, kettonel-
  // haromnal viszont van mit atvennie.
  ufosw = (CurrentUfoPartyTier() != UFO_PARTY_NONE) ? 1 : 0;

  uint8_t index = jointStack[player] - 1;
  Score(Scoring::JOINT_POINTS[index], Scoring::JOINT_BONUS[index]);
  if (jointStack[player] == 1) {
    PlaySpeechRange(TRK_VO_CHONG_JOINT_ROLLED_1_A);
  }
  else if (jointStack[player] == 2) {
    PlaySpeechRange(TRK_VO_UFO_JOINT_ROLLED_2_A);
  }
  else {
    PlaySpeechRange(TRK_VO_UFO_LOVE_PACK_READY_A);
  }
  // Fenyeffekt: az elso ket jointra a JointRolled (ID12), a harmadikra -
  // ami mar Love Packot jelent - a Lovepack (ID13). Mindketto overlay,
  // tehat a jatek-fenyre rajzol; a PlayBakedEffectOnce ezt magatol kezeli.
  PlayBakedEffectOnce((jointStack[player] >= 3) ? 13 : 12);

  if (jointStack[player] == 1) {
    SendPartyEvent("JOINT1");
  }
  else if (jointStack[player] == 2) {
    SendPartyEvent("JOINT2");
  }
  else {
    SendPartyEvent("LOVE_PACK");
  }
  // A sodras animacioja: a GUI-ban JOINT_ROLLED_1..3 nevu sequence-ek
  // vannak. A PartyEvent csak szoveges allapot, video-triggert kulon kell
  // kuldeni - enelkul nem jatszott le semmit a sodrasnal.
  Serial.print("JOINT_ROLLED_");
  Serial.println(jointStack[player]);
  delay(20);
  SendPartyState();
}

int DrawUfoLottery(uint8_t tier) {
  int result = 3;

  if (tier == UFO_PARTY_LOVE_PACK) {
    result = 7; // SpaceCoke Multiball
  }
  else if (tier == UFO_PARTY_FEATURE_WHEEL) {
    // Ket joint: Munchies / Hurry Up / azonnali Extra Ball. A mar aktiv
    // vagy betarazott feature-t kihagyjuk, nehogy a jatekos ures nyeremenyt kapjon.
    const uint8_t featureResults[3] = { 9, 2, 1 };
    do {
      result = featureResults[random(0, 3)];
    } while ((result == 1 && ExtraBallLotteryBlocked()) ||
             (result == 2 && hurryUp == HIGH));
  }
  else if (tier == UFO_PARTY_SUPER_CASHOUT) {
    // Egy joint valoban tobbet erjen a sima UFO-nal: csak a 30000 pontos
    // fo dij, multiplayerben 20000 pont lopas, illetve EXTRA BALL LIT.
    const uint8_t singleResults[2] = { 5, 10 };
    const uint8_t multiResults[3]  = { 5, 8, 10 };
    do {
      result = (numofplayers == 1)
        ? singleResults[random(0, 2)]
        : multiResults[random(0, 3)];
    } while (result == 10 && ExtraBallLotteryBlocked());
  }
  else {
    // Sima WEED cashout: negy pontjutalom, multiplayerben pontlopas,
    // valamint EXTRA BALL LIT. Hurry Up/minigame/SpaceCoke nincs ebben a tierben.
    const uint8_t singleResults[5] = { 3, 4, 5, 6, 10 };
    const uint8_t multiResults[6]  = { 3, 4, 5, 6, 8, 10 };
    do {
      result = (numofplayers == 1)
        ? singleResults[random(0, 5)]
        : multiResults[random(0, 6)];
    } while (result == 10 && ExtraBallLotteryBlocked());
  }

#ifdef SIM_MODE
  if (simForceLottery > 0) {
    int forced = simForceLottery;
    simForceLottery = 0;
    if (!ExtraBallLotteryBlocked() || (forced != 1 && forced != 10)) {
      result = forced;
    }
    else {
      Serial.println("SIM,extra-ball-lottery-blokkolva");
    }
  }
#endif
  return result;
}

const char* UfoWheelResultName() {
  if (lottery == 1) return "EXTRABALL";
  if (lottery == 2) return "HURRYUP";
  return "MUNCHIES";
}

void PlayUfoWheelResultVoice() {
  if (ufoWheelResultVoicePlayed == HIGH) return;
  ufoWheelResultVoicePlayed = HIGH;

  if (lottery == 1) {
    PlaySpeechRange(TRK_VO_UFO_WHEEL_EXTRA_BALL_A);
  }
  else if (lottery == 2) {
    PlaySpeechRange(TRK_VO_UFO_WHEEL_HURRY_UP_A);
  }
  else {
    PlaySpeechRange(TRK_VO_UFO_WHEEL_MUNCHIES_A);
  }
}

void SendUfoWheelStart() {
  Serial.print("WheelStart,");
  Serial.print(ufoWheelSession);
  Serial.print(",");
  Serial.println(UfoWheelResultName());
  ufoWheelLastSendAt = millis();
}

void ApplyUfoLotteryEntryAward(boolean playLegacyVideo) {
  if (lottery == 1) {
    extraball = 1; // nem stackelunk egynel tobbet
    if (playLegacyVideo) {
      Serial.println("Ufo5");
      delay(20);
    }
    else {
      // Szoveges fallback az uj eredmenyvideo elkeszulteig. A kesobbi GUI
      // a Wheel sequence utan, meg a WHEEL_DONE elott tudja lancolni a klipet.
      SendPartyEvent("WHEEL_EXTRA_BALL");
    }
  }

  if (lottery == 10) {
    extraBallLit = HIGH;
    Serial.println("Ufo8");
    delay(20);
  }

  // lottery 2..7: pontszam + video-trigger (a video-nevek szandekosan
  // kevertek - a regi Unity-korszakbol, lasd VIDEO_MAP.md). Index = lottery-2.
  if (lottery >= 2 && lottery <= 7) {
    static const unsigned long lotScr[6] = { 5000, 15000, 20000, 30000, 25000, 40000 };
    static const unsigned long lotBns[6] = {  100,   100,   150,   250,   250,  2000 };
    static const char* const   lotVid[6] = { "Ufo7", "Ufo1", "Ufo2", "Ufo4", "Ufo3", "Ufo9" };
    Score(lotScr[lottery - 2], lotBns[lottery - 2]);
    if (playLegacyVideo) {
      Serial.println(lotVid[lottery - 2]);
      delay(20);
    }
    else if (lottery == 2) {
      // Szoveges fallback az uj Hurry Up eredmenyvideo elkeszulteig.
      SendPartyEvent("WHEEL_HURRY_UP");
    }
  }

  if (lottery == 8) {
    // PONTLOPAS: sima cashoutnal -10000, egyjointos Super Cashoutnal
    // -20000 pont egy veletlen MASIK jatekosnak.
    ufoMinus = ((player - 1 + random(1, numofplayers)) % numofplayers) + 1;
    unsigned long stealAmount = (ufoAwardTier == UFO_PARTY_SUPER_CASHOUT)
                                  ? 20000UL : 10000UL;
    if (score[ufoMinus] >= stealAmount) score[ufoMinus] -= stealAmount;
    else score[ufoMinus] = 0;
    Serial.print("Steal,");
    Serial.print(ufoMinus);
    Serial.print(",");
    Serial.println(stealAmount);
    Serial.print("Ufo");
    Serial.println(9 + ufoMinus);
    delay(20);
  }
}

void StartUfoLotteryVisuals() {
  wTrig.trackPause(TRK_THEME);
  // UFO Lottery (weed kigyujtve): baked ID2 a 0-67-en, felul palettas feny.
  effect = HIGH;
  effectID = 2;
  fasz = 68;
  SetupPurpleAndGreenPalette();
}

void BeginUfoLotteryPresentation(boolean playLegacyVideo) {
  StartUfoLotteryVisuals();
  // A sima WEED es az egyjointos Super Cashout lottery alatt ez adja a
  // hatterzenet. A Feature Wheel sajat (TRK_UFO_WHEEL_BG), a Love Pack pedig
  // SpaceCoke hangot kap, ezert azokra nem inditjuk el.
  if (ufoAwardTier == UFO_PARTY_CASHOUT ||
      ufoAwardTier == UFO_PARTY_SUPER_CASHOUT) {
    wTrig.trackPlayPoly(TRK_HAPPYUFO);
  }
  ApplyUfoLotteryEntryAward(playLegacyVideo);
  ufoshoot = 4;
  ufoshoottimer = millis();
  ufoshoottimer2 = ufoshoottimer;
}

void StartUfoWheelPresentation() {
  // A fizikai UFO-visszajelzes a teljes GUI-lanccal egyutt indul, nem csak
  // annak vege utan villan fel egy pillanatra.
  StartUfoLotteryVisuals();
  wTrig.trackPlayPoly(TRK_UFO_WHEEL_BG);
  PlaySpeechRange(TRK_VO_UFO_FEATURE_WHEEL_A);
  // A Wheel (ID15) a kerek teljes porgese alatt fut: az intro utan a
  // loop-szakaszon marad, amig a GUI vissza nem szol (lasd
  // CompleteUfoWheelPresentation -> ReleaseHoldingBakedEffect).
  StartHoldingBakedEffect(15);
  ufoWheelChaseStartedAt = millis();
  ufoWheelSession++;
  if (ufoWheelSession == 0) ufoWheelSession = 1;
  ufoWheelWaiting = HIGH;
  ufoWheelResultVoicePlayed = LOW;
  ufoWheelStartedAt = millis();
  ufoshoot = 6; // a golyo a GUI visszajelzeseig a VUK-ban parkol
  SendUfoWheelStart();
}

void CompleteUfoWheelPresentation() {
  if (!ufoWheelWaiting) return;
  // Ha a GUI a varnal korabban vagy kapcsolat-hiba miatt kesobb kuldi a
  // DONE-t, az eredmenyhang akkor sem maradhat el es nem szolhat ketszer.
  PlayUfoWheelResultVoice();
  ufoWheelWaiting = LOW;
  ufoWheelChaseStartedAt = 0;
  // Megallt a kerek: a loop-bol atlepunk a cooldown (outro) szakaszba,
  // ami meg lefut egyszer. Nem vagjuk el, mint egy Stop tenne.
  ReleaseHoldingBakedEffect();

  if (lottery == 9 && hurryUp == LOW) {
    ufoshoot = 0;
    SendPartyEvent("WHEEL_MUNCHIES");
    StartMunchiesMode();
    return;
  }

  // A GUI lejatszotta a 6 mp-es kereket, elinditotta az 5 mp-es eredmenyt,
  // es ezzel egyutt kuldte a DONE-t. A regi 5,5 mp-es video-holdot nem
  // varjuk ki ujra: a jutalom es a VUK-kidobas az animacio alatt indul.
  ApplyUfoLotteryEntryAward(LOW);
  ufoshoot = 4;
  ufoshoottimer = millis() - UFO_LOTTERY_HOLD_MS;
  ufoshoottimer2 = ufoshoottimer;
}

void HandleUfoWheelDone(const char* command) {
  if (!ufoWheelWaiting || strncmp(command, "WHEEL_DONE,", 11) != 0) return;
  char* endp;
  unsigned long session = strtoul(command + 11, &endp, 10);
  if (*endp != '\0' || session != ufoWheelSession) return;
  CompleteUfoWheelPresentation();
}

void UpdateUfoWheelPresentation() {
  if (!ufoWheelWaiting) return;
  unsigned long now = millis();
  // A firmware mar a sorsolaskor ismeri az eredmenyt, ezert a hatmasodperces
  // GUI-wheel 5. masodperceben el tudja inditani a megfelelo UFO-mondatot.
  if (ufoWheelResultVoicePlayed == LOW &&
      now - ufoWheelStartedAt >= UFO_WHEEL_RESULT_VOICE_MS) {
    PlayUfoWheelResultVoice();
  }
  if (now - ufoWheelStartedAt >= UFO_WHEEL_TIMEOUT_MS) {
    Serial.println("WheelStatus,Timeout");
    CompleteUfoWheelPresentation();
    return;
  }
  if (now - ufoWheelLastSendAt >= UFO_WHEEL_RETRY_MS) SendUfoWheelStart();
}

boolean CollectExtraBallLitAtHighRamp() {
  if (extraBallLit == LOW || extraball > 0 ||
      SimDigitalRead(bridgeHighSwitch) != LOW || BrdgHighSw != 0) {
    return false;
  }
  extraBallLit = LOW;
  extraball = 1;
  PlaySpeechRange(TRK_VO_CHONG_EXTRA_BALL_A);
  PlayBakedEffectOnce(16);
  Serial.println("ExtraB");
  delay(20);
  return true;
}

// Veletlen beszedhang lejatszasa egy listabol. RNG-ekvivalens a regi
// random(1,count+1)+switch valtozattal: EGY random() huzas, es a tracks[]
// index pontosan a switch case-eket tukrozi -> azonos hang, azonos RNG-allapot.
void PlaySpeech(const uint8_t* tracks, uint8_t count) {
  uint8_t t = tracks[random(0, count)];
  wTrig.trackPlayPoly(t);
#ifdef SIM_MODE
  char tb[24];
  snprintf(tb, sizeof(tb), "T,speech,%u", t);
  Serial.println(tb);
#endif
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Score Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Initlights
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void RestoreStaticPlayfieldLeds() {
  // Ezeket semmilyen normal jatekrutin nem animalja. Az overlay effektek
  // viszont atmenetileg irhatnak rajuk, ezert az alapallapotukat minden
  // jatek-frame elejen visszaallitjuk; maga az aktualis overlay kesobb,
  // kozvetlenul a FastLED.show() elott tovabbra is szabadon felulirhatja.
  leds[0] = CRGB::White;
  leds[1] = CRGB::White;
  leds[4] = CRGB::White;
  leds[5] = CRGB::White;
  leds[11] = CRGB::White;
  leds[12] = CRGB::White;
  leds[13] = CRGB::White;
  leds[14] = CRGB::White;

  leds[LED_RRAMP_ARROW_22] = CRGB::Black;
  leds[LED_RRAMP_ARROW_21] = CRGB::Black;
  leds[LED_RIGHT_LOOP_2] = CRGB::Black;
  leds[LED_RIGHT_LOOP_1] = CRGB::Black;
}

void Initlights() {
  if (initlight == 1) {
    leds[0] = CRGB::White;
    leds[1] = CRGB::White;
    //leds[LED_DAVE_V] = CRGB::Black; // V
    //leds[LED_DAVE_E] = CRGB::Black; // E
    leds[4] = CRGB::White;
    leds[5] = CRGB::White;
    //leds[LED_BONUS_X8] = CRGB::Black; // x8
    //leds[LED_BONUS_X6] = CRGB::Black; // x6
    //leds[LED_BALLSAVE] = CRGB::Black; // Ballsave
    //leds[LED_BONUS_X4] = CRGB::Black; // x4
    //leds[LED_BONUS_X2] = CRGB::Black; // x2
    leds[11] = CRGB::White;
    leds[12] = CRGB::White;
    leds[13] = CRGB::White;
    leds[14] = CRGB::White;
    //leds[LED_DAVE_A] = CRGB::Black; // A
    //leds[LED_DAVE_D] = CRGB::Black; // D
    leds[LED_CNC_AMBIENT] = CRGB::White; // CnC ambient
    //leds[LED_CNC_C1] = CRGB::Black; // C
    //leds[LED_CNC_AMP] = CRGB::Black; // &
    //leds[LED_CNC_C2] = CRGB::Black; // C
    leds[LED_LEFT_LOOP_1] = CRGB::Black; // Left loop
    leds[LED_LEFT_LOOP_2] = CRGB::Black; // Left loop
    leds[LED_LEFT_RAMP_AMBIENT] = CRGB::White; // Left ramp ambient
    leds[LED_LEFT_RAMP_1] = CRGB::Black; // Left ramp
    leds[LED_LEFT_RAMP_2] = CRGB::Black; // Left ramp
    leds[LED_CHONG_IND] = CRGB::Black; // Chong indicator
    leds[LED_GET_HIGH_1] = CRGB::Black; // Get High
    leds[LED_GET_HIGH_2] = CRGB::Black; // Get high
    //leds[LED_WEED_W] = CRGB::Black; // W
    //leds[LED_WEED_E1] = CRGB::Black; // E
    //leds[LED_WEED_E2] = CRGB::Black; // E
    //leds[LED_WEED_D] = CRGB::Black; // D
    leds[LED_CHEECH_IND] = CRGB::Black; // Cheech indicator
    leds[LED_RRAMP_ARROW_22] = CRGB::Black; // Right ramp arrow 2/2
    leds[LED_RRAMP_ARROW_21] = CRGB::Black; // Right ramp arrow 2/1
    leds[LED_RRAMP_ARROW_12] = CRGB::Black; // Right ramp arrow 1/1
    leds[LED_RRAMP_ARROW_11] = CRGB::Black; // Right ramp arrow 1/1
    //leds[LED_CHEECH_WHEEL] = CRGB::Orange; // Cheech wheel
    //leds[LED_CHONG_PIPE] = CRGB::Orange; // Chong Pipe
    //leds[LED_CHONG_ROACH] = CRGB::Orange; // Chong Cockroach
    //leds[LED_CHEECH_CIGAR] = CRGB::Orange; // Cheech cigar
    leds[LED_UFO_ARROW_1] = CRGB::Black; // UFO arrow 1
    leds[LED_UFO_ARROW_2] = CRGB::Black; // UFO arrow 2
    leds[LED_RIGHT_LOOP_2] = CRGB::Black; // Right loop 2
    leds[LED_RIGHT_LOOP_1] = CRGB::Black; // Right loop 1
    //leds[LED_MUF_DVR] = CRGB::Orange; // MUF dvr
    //leds[LED_CHONG_KEY] = CRGB::Orange; // Chong plunger key
    leds[LED_FISH] = CRGB::Black; // Fish
    leds[LED_TANK] = CRGB::Black; // Tank
    leds[LED_FISHTANK_AMBIENT] = CRGB::White; // Fishtank ambient
    leds[LED_RIGHT_RAMP_AMBIENT] = CRGB::White; // Right ramp ambient
    leds[LED_GATE3_AMBIENT] = CRGB::Green; // Gate 3 ambient
    leds[LED_GATE3] = CRGB::Black; // Gate 3
    leds[LED_GATE32_AMBIENT] = CRGB::Green; // Gate 32 ambient
    leds[LED_GATE2] = CRGB::White; // Gate 2
    leds[LED_GATE21_AMBIENT] = CRGB::Green; // Gate 21 ambient
    leds[LED_GATE1] = CRGB::Black; // Gate 1
    leds[LED_GATE1_AMBIENT] = CRGB::Green; // Gate 1 ambient
    leds[LED_MICHOAKAN] = CRGB::Gray; // Michoakan
    leds[LED_ACAPULCO] = CRGB::Gray; // Acapulco Gold
    leds[LED_HIGHTABLE_FLASHER] = CRGB::Gray; // Hightable flasher
    leds[LED_THAI] = CRGB::Gray; // Thai stick
    leds[LED_LABRADOR] = CRGB::Gray; // Labrador
    leds[LED_POP1] = CRGB::Blue; // Pop1
    leds[LED_POP2] = CRGB::Blue; // Pop2
    leds[LED_POP3] = CRGB::Blue; // Pop3
    leds[LED_CAR_AMBIENT] = CRGB::Yellow; // Car ambient
    fasz = 68;
    currentPalette = RainbowColors_p;
    initlight = 0;
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Initlight rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Flippers
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void Left_Flipper() {
  if (SimDigitalRead(leftFlipperButton) == LOW && flipl == 0) {
    flipl = 1;
    flipltimer = millis();
  }
  if (flipl == 1) {
    digitalWrite(leftFlipperBat, HIGH);
  }
  if (SimDigitalRead(leftFlipperButton) == HIGH && millis() - 100 > flipltimer) {
    digitalWrite(leftFlipperBat, LOW);
    flipl = 0;
  }
}


void Right_Flipper() {
  if (SimDigitalRead(rightflipperButton) == LOW && flipr == 0) {
    flipr = 1;
    fliprtimer = millis();
  }
  if (flipr == 1) {
    digitalWrite(rightFlipperBat, HIGH);
  }
  if (SimDigitalRead(rightflipperButton) == HIGH && millis() - 100 > fliprtimer) {
    digitalWrite(rightFlipperBat, LOW);
    flipr = 0;
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Flippers Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Slingshots
/////////////////////////////////////////////////
/////////////////////////////////////////////////


void Right_Slingshot() {
  if (SimDigitalRead(rightSlingshotSwitch) == LOW && slingr == 0) {
    slingr = 1;
    slingrtimer = millis();
    wTrig.trackPlayPoly(TRK_BIGYONG);
    ufoInactivesw = 1;
    ufoInactiveTimer = millis();
    ballHandlerSkip = 1;
    ballHandlerSkipTimer = millis();

    Score(Scoring::SLING_POINTS, Scoring::SLING_BONUS);
    TriggerHurryHit(HURRY_ZONE_SLING_RIGHT);

  }
  if (slingr == 1 && millis() - 12 < slingrtimer) {
    digitalWrite(rightSlingshotCoil, HIGH);
  }
  else {
    digitalWrite(rightSlingshotCoil, LOW);
  }

  if (SimDigitalRead(rightSlingshotSwitch) == HIGH && millis() - 500 > slingrtimer) {
    slingr = 0;
  }
}







void Left_Slingshot() {
  if (SimDigitalRead(leftSlingshotSwitch) == LOW && slingl == 0) {
    slingl = 1;
    slingltimer = millis();
    wTrig.trackPlayPoly(TRK_BIGYONG);
    ufoInactivesw = 1;
    ufoInactiveTimer = millis();
    ballHandlerSkip = 1;
    ballHandlerSkipTimer = millis();

    Score(Scoring::SLING_POINTS, Scoring::SLING_BONUS);
    TriggerHurryHit(HURRY_ZONE_SLING_LEFT);

  }
  if (slingl == 1 && millis() - 12 < slingltimer) {
    digitalWrite(leftSlingshotCoil, HIGH);
  }
  else {
    digitalWrite(leftSlingshotCoil, LOW);
  }
  if (SimDigitalRead(leftSlingshotSwitch) == HIGH && millis() - 500 > slingltimer) {
    slingl = 0;
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Slingshots Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// CnC
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void CnC() {
  if (SimDigitalRead(cncLetterC3rd) == HIGH) {
    Blinktimer();
    if (effect == LOW) {
      leds[LED_CNC_C1] = CRGB::Yellow;
    }
    if (ledState == LOW) {
      leds[LED_CNC_C1] = CRGB::Black;
    }
  }



  static const uint8_t cncPin[3]   = { 25, 15, 14 }; // C3rd, N, C1st
  static const uint8_t cncSound[3] = { 12, 13, 14 };
  static const uint8_t cncLed[3]   = { 18, 19, 20 };
  int* csw[3]                = { &cncswitch3, &cncswitch2, &cncswitch1 };
  boolean* chsw[3]           = { &cnchurryswitch3, &cnchurryswitch2, &cnchurryswitch1 };
  unsigned long* chtimer[3]  = { &cnchurryTimer3, &cnchurryTimer2, &cnchurryTimer1 };

  if (hurryUp == LOW) {
    // Normal mod
    for (uint8_t i = 0; i < 3; i++) {
      if (SimDigitalRead(cncPin[i]) == LOW && *csw[i] == 0) {
        *csw[i] = 1;
        wTrig.trackPlayPoly(cncSound[i]);
        Score(Scoring::CNC_LETTER_POINTS, Scoring::CNC_LETTER_BONUS);
        if (giftsw == 1) { giftsw = 0; Gift(); }
      }
    }
  }

  if (hurryUp == HIGH) {
    // HurryUp mod
    for (uint8_t i = 0; i < 3; i++) {
      if (SimDigitalRead(cncPin[i]) == LOW && *chsw[i] == 0) {
        *chsw[i] = 1;
        *chtimer[i] = millis();
        wTrig.trackPlayPoly(cncSound[i]);
        Score(Scoring::CNC_LETTER_POINTS, Scoring::CNC_LETTER_BONUS);
        TriggerHurryHit(HURRY_ZONE_CNC);
      }
    }
    for (uint8_t i = 0; i < 3; i++) {
      if (millis() - 350 > *chtimer[i]) {
        *chsw[i] = 0;
      }
    }
  }

  // Random gift: ha a sorsolt betu (== 2) talalodik el
  if (giftsw == 1) {
    for (uint8_t i = 0; i < 3; i++) {
      if (SimDigitalRead(cncPin[i]) == LOW && *csw[i] == 2) {
        *csw[i] = 1;
        wTrig.trackPlayPoly(cncSound[i]);
        wTrig.trackPlayPoly(TRK_CHEECHBEAUTY);
        wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        Score(Scoring::GIFT_CNC_POINTS, Scoring::GIFT_CNC_BONUS);
        delay(10);
        Serial.println("Point2");
        giftsw = 3;
      }
    }
  }

  // Betu-LED-ek
  if (effect == LOW) {
    for (uint8_t i = 0; i < 3; i++) {
      if (*csw[i] == 1) {
        leds[cncLed[i]] = CRGB::White;
      }
      if (*csw[i] == 0) {
        leds[cncLed[i]] = CRGB::Black;
      }
    }
    if (giftsw == 1) {
      for (uint8_t i = 0; i < 3; i++) {
        if (*csw[i] == 2) {
          leds[cncLed[i]] = (ledState == HIGH) ? CRGB::Green : CRGB::Yellow;
        }
      }
    }
  }


  if (cncswitch1 == 1 && cncswitch2 == 1 && cncswitch3 == 1 && cncoff == 0) {
    cnctimer = millis();
    cncoff = 1;
    Score(Scoring::CNC_COMPLETE_POINTS, Scoring::CNC_COMPLETE_BONUS);
  }

  if (cncoff == 1) {
    chongLightActiveSw = HIGH;
    cheechLightActiveSw = HIGH;
    if (millis() - 1000 < cnctimer) {
      Blinktimer();
      if (ledState == HIGH) {
        leds[LED_CNC_C1] = CRGB::Green; // C
        leds[LED_CNC_AMP] = CRGB::Green; // C
        leds[LED_CNC_C2] = CRGB::Green; // C
      }
      if (ledState == LOW) {
        leds[LED_CNC_C1] = CRGB::Yellow; // C
        leds[LED_CNC_AMP] = CRGB::Yellow; // C
        leds[LED_CNC_C2] = CRGB::Yellow; // C
      }


    }

    if (millis() - 1100 > cnctimer) {
      cncswitch1 = 0;
      cncswitch2 = 0;
      cncswitch3 = 0;
      cncoff = 0;
    }
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End CnC Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Loop
/////////////////////////////////////////////////
/////////////////////////////////////////////////

// A nagyhid utan nyitva van-e meg a loop-ablak? Multiballhoz kotott, mert a
// kifizetes-tabla is multiball-szintre indexel.
boolean HighLoopComboArmed() {
  return (multiball != 0 && highLoopArmT != 0 &&
          millis() - highLoopArmT < HIGH_LOOP_COMBO_WINDOW_MS);
}

void Loopshoot() {
  if (SimDigitalRead(loopSwitchTop) == LOW && loopsw == LOW) {
    looptimer = millis();
    loopswt = millis();
    loopsw = HIGH;
    wTrig.trackPlayPoly(TRK_BANANA);
  }
  if (SimDigitalRead(loopSwitchSide) == LOW && millis() - 1000 < looptimer && loopsw == LOW) {
    if (HighLoopComboArmed()) {
      // A nagyhid utani loop a sajat, multiball-szinthez kotott osszeget
      // fizeti. Ez KIVALTJA a sima loop-jackpotot ugyanazon a lovesen, hogy
      // ne fizessunk ketszer; a multiloopsw armozva marad egy kesobbi loopra.
      const unsigned long combo = Scoring::HIGH_LOOP_COMBO_SCR[multiball];
      PlayJackpotFeedback(combo);
      wTrig.trackPlayPoly(TRK_BLOB);
      ScoreJackpot(combo, Scoring::LOOP_JACKPOT_BONUS);
      highLoopArmT = 0;
    }
    else {
      if (multiloopsw == 1) {
        PlayJackpotFeedback(Scoring::LOOP_JACKPOT_POINTS);
      }
      wTrig.trackPlayPoly(TRK_BLOB);
      if (multiloopsw == 1) {
        ScoreJackpot(Scoring::LOOP_JACKPOT_POINTS, Scoring::LOOP_JACKPOT_BONUS);
      }
      else if (hurryUp == HIGH) {
        // Hurry Up alatt a loop fix 30000-et fizet (15000 duplazva), a hozza
        // tartozo sima pontozo videoval - jackpot-klip ide nem jar.
        Score(Scoring::HURRY_LOOP_POINTS, Scoring::LOOP_BONUS);
        Serial.println("Point8");
        delay(20);
      }
      else {
        Score(Scoring::LOOP_POINTS, Scoring::LOOP_BONUS);
        // sima felso loop: nincs baked effekt (az ID4/UFO FUCK mostmar
        // kizarolag a UFO-no-weed esemenye)
      }
      multiloopsw = 0;
    }
    loopswt = millis();
    loopsw = HIGH;
  }
  if (loopsw == HIGH && millis() - 100 > loopswt) {
    loopsw = LOW;
  }
  // A nagyhid utani 5 mp-es ablak sajat, feher-magenta villogast kap, hogy
  // meg lehessen kulonboztetni a sima loop-jackpot piros-sargajatol.
  if (HighLoopComboArmed()) {
    if (ledState == HIGH) {
      leds[LED_LEFT_LOOP_1] = CRGB::White;
      leds[LED_LEFT_LOOP_2] = CRGB::Magenta;
    }
    else {
      leds[LED_LEFT_LOOP_1] = CRGB::Magenta;
      leds[LED_LEFT_LOOP_2] = CRGB::White;
    }
  }
  else if (multiloopsw == 1) {
    if (ledState == HIGH) {
      leds[LED_LEFT_LOOP_1] = CRGB::Red; // Left loop
      leds[LED_LEFT_LOOP_2] = CRGB::Yellow; // Left loop
    }
    if (ledState == LOW) {
      leds[LED_LEFT_LOOP_1] = CRGB::Yellow; // Left loop
      leds[LED_LEFT_LOOP_2] = CRGB::Red; // Left loop
    }
  }
  else {
    leds[LED_LEFT_LOOP_1] = CRGB::Black; // Left loop
    leds[LED_LEFT_LOOP_2] = CRGB::Black; // Left loop
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Loop Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Weed
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void Weed() {
  static const uint8_t weedPin[4]   = { 11, 10, 9, 8 }; // d11, d10, d9, d8
  static const uint8_t weedSound[4] = { 5, 39, 40, 41 };
  static const uint8_t weedLed[4]   = { 29, 30, 31, 32 };
  int* wsw[4]                = { &weedswitch1, &weedswitch2, &weedswitch3, &weedswitch4 };
  boolean* whsw[4]           = { &weedhurryswitch1, &weedhurryswitch2, &weedhurryswitch3, &weedhurryswitch4 };
  unsigned long* whtimer[4]  = { &weedhurrytimer1, &weedhurrytimer2, &weedhurrytimer3, &weedhurrytimer4 };

  if (hurryUp == LOW) {
    // Normal mod
    for (uint8_t i = 0; i < 4; i++) {
      if (SimDigitalRead(weedPin[i]) == LOW && *wsw[i] == 0) {
        *wsw[i] = 1;
        Score(Scoring::WEED_LETTER_POINTS, Scoring::WEED_LETTER_BONUS);
        wTrig.trackPlayPoly(weedSound[i]);
        if (giftsw == 1) { giftsw = 0; Gift(); }
      }
    }
  }

  if (hurryUp == HIGH) {
    // HurryUp mod
    for (uint8_t i = 0; i < 4; i++) {
      if (SimDigitalRead(weedPin[i]) == LOW && *whsw[i] == 0) {
        *whsw[i] = 1;
        *whtimer[i] = millis();
        Score(Scoring::WEED_LETTER_POINTS, Scoring::WEED_LETTER_BONUS);
        wTrig.trackPlayPoly(weedSound[i]);
        TriggerHurryHit(HURRY_ZONE_WEED);
        if (giftsw == 1) { giftsw = 0; Gift(); }
      }
    }
    for (uint8_t i = 0; i < 4; i++) {
      if (millis() - 350 > *whtimer[i]) {
        *whsw[i] = 0;
      }
    }
  }

  // Random gift: ha a sorsolt weed-celpont (== 2) talalodik el
  if (giftsw == 1) {
    for (uint8_t i = 0; i < 4; i++) {
      if (SimDigitalRead(weedPin[i]) == LOW && *wsw[i] == 2) {
        *wsw[i] = 1;
        Score(Scoring::GIFT_OTHER_POINTS, Scoring::GIFT_OTHER_BONUS);
        wTrig.trackPlayPoly(weedSound[i]);
        wTrig.trackPlayPoly(TRK_CHEECHBEAUTY);
        wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        Serial.println("Point2");
        giftsw = 3;
      }
    }
  }

  // Celpont-LED-ek
  if (effect == LOW) {
    for (uint8_t i = 0; i < 4; i++) {
      if (*wsw[i] == 1) {
        leds[weedLed[i]] = CRGB::White;
      }
      if (*wsw[i] == 0) {
        leds[weedLed[i]] = CRGB::Black;
      }
    }
    if (giftsw == 1) {
      for (uint8_t i = 0; i < 4; i++) {
        if (*wsw[i] == 2) {
          leds[weedLed[i]] = (ledState == HIGH) ? CRGB::Green : CRGB::Yellow;
        }
      }
    }
  }

  // WEED kigyujtve -> UFO + multiball-mero elesites es hang. Szandekosan
  // NEM ad ball save-et; azt csak az UFO/VUK kidobas es a multiball indokolja.
  if (*wsw[0] == 1 && *wsw[1] == 1 && *wsw[2] == 1 && *wsw[3] == 1 && weedoff == 0) {
    weedtimer = millis();
    weedoff = 1;
    effect = HIGH;
    effectID = 5; // Weedblast - a weed kigyulesekor (korabban ID3 volt)
    boolean canChoosePartyShot =
      (beerCredits[player] > 0 && jointStack[player] < 3);
    PlaySpeechRange(canChoosePartyShot
                      ? TRK_VO_UFO_WEED_CHOOSE_A
                      : TRK_VO_UFO_WEED_NEED_BEER_A);
    Serial.println("Weed");
    weedQualified[player] = HIGH;
    if (multiball == 0 && hurryUp == LOW) {
      ufosw = 1;
      spinnersw = 1;
    }
    SendPartyEvent(canChoosePartyShot ? "CHOOSE" : "NEED_BEER");
    SendPartyState();
    delay(10);
  }

  if (weedoff == 1) {
    if (multiball == 0 && hurryUp == LOW) {
      SetupBlackAndGreenStripedPalette();
      currentBlending = NOBLEND;
    }

    if (millis() - 1000 < weedtimer) {
      Blinktimer();
      for (uint8_t i = 0; i < 4; i++) {
        leds[weedLed[i]] = (ledState == HIGH) ? CRGB::Green : CRGB::Yellow;
      }
    }

    if (millis() - 1100 > weedtimer) {
      weedswitch1 = 0;
      weedswitch2 = 0;
      weedswitch3 = 0;
      weedswitch4 = 0;
      weedoff = 0;
      Score(Scoring::WEED_COMPLETE_POINTS, Scoring::WEED_COMPLETE_BONUS);
    }
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Weed Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Fishtank
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void Fishtank() {
  static const uint8_t fishPin[2]   = { 16, 17 }; // fishTankSwitch1, 2
  static const uint8_t fishSound[2] = { TRK_PING, TRK_BEER };
  static const uint8_t fishLed[2]   = { LED_FISH, LED_TANK };
  int* fstate[2] = { &fishTankLightState1, &fishTankLightState2 };

  // Talalat (normal / hurryUp)
  for (uint8_t i = 0; i < 2; i++) {
    if (SimDigitalRead(fishPin[i]) == LOW && *fstate[i] == 0) {
      *fstate[i] = 1;
      Score(Scoring::FISHTANK_TARGET_POINTS, Scoring::FISHTANK_TARGET_BONUS);
      wTrig.trackPlayPoly(fishSound[i]);
      TriggerHurryHit(HURRY_ZONE_FISHTANK);
      if (giftsw == 1) { giftsw = 0; Gift(); }
    }
  }

  // Random gift
  if (giftsw == 1) {
    for (uint8_t i = 0; i < 2; i++) {
      if (SimDigitalRead(fishPin[i]) == LOW && *fstate[i] == 2) {
        *fstate[i] = 1;
        Score(Scoring::GIFT_OTHER_POINTS, Scoring::GIFT_OTHER_BONUS);
        wTrig.trackPlayPoly(fishSound[i]);
        wTrig.trackPlayPoly(TRK_CHEECHBEAUTY);
        wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        Serial.println("Point2");
        giftsw = 3;
      }
    }
  }

  // LED-ek
  if (effect == LOW) {
    for (uint8_t i = 0; i < 2; i++) {
      if (*fstate[i] == 1) {
        leds[fishLed[i]] = CRGB::White;
      }
      if (*fstate[i] == 0) {
        leds[fishLed[i]] = CRGB::Black;
      }
    }
    if (giftsw == 1) {
      for (uint8_t i = 0; i < 2; i++) {
        if (*fstate[i] == 2) {
          leds[fishLed[i]] = (ledState == HIGH) ? CRGB::Green : CRGB::Yellow;
        }
      }
    }
  }

  // Mind a ket celpont -> egy betarazhato sor. Maximum harom tarolhato,
  // golyok es jatekosvaltasok kozott is megmarad.
  if (fishTankLightState1 == 1 && fishTankLightState2 == 1 && fishoff == 0) {
    fishtimer = millis();
    fishoff = 1;
    Score(Scoring::FISHTANK_PAIR_POINTS, Scoring::FISHTANK_PAIR_BONUS);
    // HurryUp alatt a Fish Tank csak pontot ad: a sor-gyujtes ilyenkor
    // inaktiv, ugyanugy, ahogy a CnC ag is kulon HurryUp-modot fut.
    if (hurryUp == LOW) {
      if (beerCredits[player] < 3) {
        beerCredits[player]++;
        PlaySpeechRange(TRK_VO_CHONG_BEER_COLLECTED_A);
        PlayBakedEffectOnce(18); // Fishtank overlay a sor betarazasara
        Serial.print("Beer");
        Serial.println(beerCredits[player]);
        SendPartyEvent("BEER");
        SendPartyState();
      }
      else {
        PlaySpeechRange(TRK_VO_UFO_BEER_FULL_A);
        SendPartyEvent("BEER_FULL");
      }
    }
  }

  if (fishoff == 1) {
    if (millis() - 1000 < fishtimer) {
      Blinktimer();
      for (uint8_t i = 0; i < 2; i++) {
        leds[fishLed[i]] = (ledState == HIGH) ? CRGB::Green : CRGB::Yellow;
      }
    }
    if (millis() - 1100 > fishtimer) {
      fishTankLightState1 = 0;
      fishTankLightState2 = 0;
      fishoff = 0;
    }
  }

  if (effect == LOW) {
    if (beerCredits[player] == 0) leds[LED_FISHTANK_AMBIENT] = CRGB::Black;
    if (beerCredits[player] == 1) leds[LED_FISHTANK_AMBIENT] = CRGB(0, 0, 96);
    if (beerCredits[player] == 2) leds[LED_FISHTANK_AMBIENT] = CRGB::Blue;
    if (beerCredits[player] >= 3) leds[LED_FISHTANK_AMBIENT] = CRGB::Cyan;
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Fishtank Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
//// Gift-fazis futofeny
/////////////////////////////////////////////////
// Amig a gift aktiv (giftsw == 1) es nincs eppen nagy effekt, ket sarga,
// halvanyulo fenycsova fut ki a weed kozepetol - balra a CnC fele, jobbra
// a Fishtank fele. 1 mp sopres, majd 2 mp sotetseg (3 mp-es ciklus).
// CSAK inaktiv (fekete) postra rajzol, igy a zolden villogo gift-postot
// (es a mar meglott feher postokat) sosem irja felul.
// FONTOS: a subsystem-kijelzok (Weed/CnC/Fishtank) UTAN kell hivni a loopban!
void GiftRunlight() {
  if (giftsw != 1 || effect != LOW) {
    return;
  }
  unsigned long t = millis() % 3000;
  if (t >= 1000) {
    return; // 2 mp sotetseg (a postok feketen maradnak)
  }

  static const uint8_t trail1[] = { LED_WEED_E1, LED_WEED_W, LED_CNC_C1, LED_CNC_AMP, LED_CNC_C2 };
  static const uint8_t trail2[] = { LED_WEED_E2, LED_WEED_D, LED_FISH, LED_TANK };
  const uint8_t TAIL = 4; // csova-hossz LED-ben

  for (uint8_t pass = 0; pass < 2; pass++) {
    const uint8_t* trail = (pass == 0) ? trail1 : trail2;
    uint8_t n = (pass == 0) ? sizeof(trail1) : sizeof(trail2);
    // fejpozicio *16 fixpontban, 1000 ms alatt 0 -> (n+TAIL)
    long head16 = (long)t * (n + TAIL) * 16 / 1000;
    for (uint8_t i = 0; i < n; i++) {
      long d16 = head16 - (long)i * 16; // tavolsag a fejtol, *16
      if (d16 >= 0 && d16 < (long)TAIL * 16) {
        uint8_t bri = 255 - (uint8_t)(d16 * 255 / (TAIL * 16)); // fejnel fenyes
        if (leds[trail[i]] == CRGB::Black) { // csak inaktiv post
          leds[trail[i]] = CHSV(HUE_YELLOW, 255, bri);
        }
      }
    }
  }
}



/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// DAVE
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void Dave_switch() {
  if (SimDigitalRead(daveLaneSwitchD) == LOW && sltimesw == LOW) {
    wTrig.trackPlayPoly(TRK_DAVE);
    davearr[1] = 1;
    sltimesw = HIGH;
    sltimer = millis();
    if (ballsaversw == HIGH) {
        sidelaneBallsaverSw = HIGH;
    }
  }
  if (SimDigitalRead(daveLaneSwitchA) == LOW && sltimesw == LOW) {
    wTrig.trackPlayPoly(TRK_DAVE);
    davearr[2] = 1;
    sltimesw = HIGH;
    sltimer = millis();
  }
  if (SimDigitalRead(daveLaneSwitchV) == LOW && sltimesw == LOW) {
    wTrig.trackPlayPoly(TRK_DAVE);
    davearr[3] = 1;
    sltimesw = HIGH;
    sltimer = millis();
  }
  if (SimDigitalRead(daveLaneSwitchE) == LOW && sltimesw == LOW) {
    wTrig.trackPlayPoly(TRK_DAVE);
    davearr[4] = 1;
    sltimesw = HIGH;
    sltimer = millis();
    if (ballsaversw == HIGH) {
        sidelaneBallsaverSw = HIGH;
    }
  }

  if (millis() - 1000 > sltimer) {
    sltimesw = LOW;
  }


  if (effect == LOW) {
    if (davearr[1] == 1) {
      leds[LED_DAVE_D] = CRGB::White; // D
    }
    if (davearr[2] == 1) {
      leds[LED_DAVE_A] = CRGB::White; // A
    }
    if (davearr[3] == 1) {
      leds[LED_DAVE_V] = CRGB::White; // D
    }
    if (davearr[4] == 1) {
      leds[LED_DAVE_E] = CRGB::White; // D
    }
    if (davearr[1] == 0) {
      leds[LED_DAVE_D] = CRGB::Black; // D
    }
    if (davearr[2] == 0) {
      leds[LED_DAVE_A] = CRGB::Black; // A
    }
    if (davearr[3] == 0) {
      leds[LED_DAVE_V] = CRGB::Black; // D
    }
    if (davearr[4] == 0) {
      leds[LED_DAVE_E] = CRGB::Black; // D
    }
  }

  // Shift left
  if (slltimesw == LOW && SimDigitalRead(leftFlipperButton) == LOW) {
    slltimesw = HIGH;
    slltimer = millis();
    davearr[0] = davearr[1];
    davearr[1] = davearr[2];
    davearr[2] = davearr[3];
    davearr[3] = davearr[4];
    davearr[4] = davearr[0];
  }


  if (millis() - 200 > slltimer && SimDigitalRead(leftFlipperButton) == HIGH) {
    slltimesw = LOW;
  }



  // Shift Right
  if (slrtimesw == LOW && SimDigitalRead(rightflipperButton) == LOW) {
    slrtimesw = HIGH;
    slrtimer = millis();
    davearr[5] = davearr[4];
    davearr[4] = davearr[3];
    davearr[3] = davearr[2];
    davearr[2] = davearr[1];
    davearr[1] = davearr[5];

  }

  if (millis() - 200 > slrtimer && SimDigitalRead(rightflipperButton) == HIGH) {
    slrtimesw = LOW;
  }




  if (daveoff == 0 && davearr[1] == 1 && davearr[2] == 1 && davearr[3] == 1 && davearr[4] == 1) {
    daveoff = 1;
    davetimer = millis();
    EnsureBallSave(DAVE_BALL_SAVE_MS);
  }

  if (daveoff == 1) {
    if (millis() - 1000 < davetimer) {
      Blinktimer();
      if (ledState == HIGH) {
        leds[LED_DAVE_D] = CRGB::White; // D
        leds[LED_DAVE_A] = CRGB::White; // A
        leds[LED_DAVE_V] = CRGB::White; // D
        leds[LED_DAVE_E] = CRGB::White; // D
      }
      if (ledState == LOW) {
        leds[LED_DAVE_D] = CRGB::Black; // D
        leds[LED_DAVE_A] = CRGB::Black; // A
        leds[LED_DAVE_V] = CRGB::Black; // D
        leds[LED_DAVE_E] = CRGB::Black; // D
      }

    }

    if (millis() - 1000 > davetimer) {
      davearr[1] = 0;
      davearr[2] = 0;
      davearr[3] = 0;
      davearr[4] = 0;
      daveoff = 0;
      wTrig.trackPlayPoly(TRK_DAVENOTHERE);
    }
  }

}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End DAVE Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Gate
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void Gate() {
  static const uint8_t gatePin[3] = { 45, 44, 40 }; // gateSwitch1, 2, 3
  boolean* gateamb[3] = { &gateamb1, &gateamb2, &gateamb3 };

  for (uint8_t i = 0; i < 3; i++) {
    if (SimDigitalRead(gatePin[i]) == LOW && gatetimesw == 0) {
      uint8_t g = i + 1; // gatearr-index (1..3)
      wTrig.trackPlayPoly(TRK_KVAKK);
      Score(Scoring::GATE_POINTS, Scoring::GATE_BONUS);
      if (gatearr[g] == 2) { // megjelolt kapu -> drift-pont
        Score(Scoring::DRIFT_POINTS, Scoring::DRIFT_BONUS);
        wTrig.trackPlayPoly(TRK_CHEECHYEAH);
        wTrig.trackPlayPoly(TRK_WEEDFULL);
        Serial.println("Drift");
        delay(20);
      }
      // ha barmelyik MASIK kapu meg van jelolve (==2), toroljuk mind a kettot
      uint8_t o1 = (g == 1) ? 2 : 1;
      uint8_t o2 = (g == 3) ? 2 : 3;
      if (gatearr[o1] == 2 || gatearr[o2] == 2) {
        gatearr[o1] = 0;
        gatearr[o2] = 0;
      }
      gatearr[g] = 1;
      gatetimesw = 1;
      gatetimer = millis();
      *gateamb[i] = HIGH;
      gateambtimer = millis();
    }
  }

  if (millis() - 1000 > gatetimer) {
    gatetimesw = 0;
    gateamb1 = LOW;
    gateamb2 = LOW;
    gateamb3 = LOW;

  }

  // Gateamb
  if (effect == LOW) {
    if (gateamb1 == HIGH ) {
      leds[LED_GATE21_AMBIENT] = CRGB::Orange; // D
      leds[LED_GATE1_AMBIENT] = CRGB::Orange; // D
    }
    else if (millis() - 1000 > gateambtimer) {
      leds[LED_GATE21_AMBIENT] = CRGB::Green; // D
      leds[LED_GATE1_AMBIENT] = CRGB::Green; // D
    }
    if (gateamb2 == HIGH ) {
      leds[LED_GATE21_AMBIENT] = CRGB::Orange; // D
      leds[LED_GATE32_AMBIENT] = CRGB::Orange; // D
    }
    else if (millis() - 1000 > gateambtimer) {
      leds[LED_GATE21_AMBIENT] = CRGB::Green; // D
      leds[LED_GATE32_AMBIENT] = CRGB::Green; // D
    }
    if (gateamb3 == HIGH ) {
      leds[LED_GATE32_AMBIENT] = CRGB::Orange; // D
      leds[LED_GATE3_AMBIENT] = CRGB::Orange; // D
    }
    else if (millis() - 1000 > gateambtimer) {
      leds[LED_GATE32_AMBIENT] = CRGB::Green; // D
      leds[LED_GATE3_AMBIENT] = CRGB::Green; // D
    }
  }

  if (effect == LOW) {
    static const uint8_t gateLed[3] = { LED_GATE1, LED_GATE2, LED_GATE3 };
    for (uint8_t i = 0; i < 3; i++) {
      uint8_t g = i + 1;
      if (gatearr[g] == 1) { leds[gateLed[i]] = CRGB::White; }
      if (gatearr[g] == 2) { leds[gateLed[i]] = CRGB::Orange; }
      if (gatearr[g] == 0) { leds[gateLed[i]] = CRGB::Black; }
    }
  }

  if (gateoffsw == 0 && gatearr[1] == 1 && gatearr[2] == 1 && gatearr[3] == 1) {
    gateofftimer = millis();
    gateoffsw = 1;
    bonusx ++;
  }

  if (gateoffsw == 1 && millis() - 1000 < gateofftimer) {
    Blinktimer();
    if (ledState == LOW) {
      leds[LED_GATE1] = CRGB::Black; // D
      leds[LED_GATE2] = CRGB::Black; // A
      leds[LED_GATE3] = CRGB::Black; // D
    }
    if (ledState == HIGH) {
      leds[LED_GATE1] = CRGB::Red; // D
      leds[LED_GATE2] = CRGB::Red; // A
      leds[LED_GATE3] = CRGB::Red; // D
    }

  }




  if (gateoffsw == 1 && millis() - 1000 > gateofftimer) {

    leds[LED_GATE1] = CRGB::Black; // D
    leds[LED_GATE2] = CRGB::Black; // A
    leds[LED_GATE3] = CRGB::Black; // D
    gatearr[1] = 0;
    gatearr[2] = 0;
    gatearr[3] = 0;
    wTrig.trackPlayPoly(TRK_CHINGA);
    gateoffsw = 0;

  }

  // SHIFT LEFT
  if (gsltimesw == 0 && SimDigitalRead(leftFlipperButton) == LOW) {
    gsltimesw = 1;
    gsltimer = millis();
    gatearr[0] = gatearr[1];
    gatearr[1] = gatearr[2];
    gatearr[2] = gatearr[3];
    gatearr[3] = gatearr[0];
  }
  if (millis() - 200 > gsltimer && SimDigitalRead(leftFlipperButton) == HIGH) {
    gsltimesw = 0;
  }



  // SHIFT RIGHT
  if (gsrtimesw == 0 && SimDigitalRead(rightflipperButton) == LOW) {
    gsrtimesw = 1;
    gsrtimer = millis();
    gatearr[4] = gatearr[3];
    gatearr[3] = gatearr[2];
    gatearr[2] = gatearr[1];
    gatearr[1] = gatearr[4];
  }
  if (millis() - 200 > gsrtimer && SimDigitalRead(rightflipperButton) == HIGH) {
    gsrtimesw = 0;
  }

}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Gate Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// POPs
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void Pops() {
  // A harom pop bumper azonos logikaja egy ciklusban. A globalis
  // valtozokat NEM valtjuk ki - lokalis mutato-tombokkel hivatkozunk
  // rajuk, igy a refaktor a fuggvenyen belul marad.
  const uint8_t popSwitch[3] = { (uint8_t)pop1Switch, (uint8_t)pop2Switch, (uint8_t)pop3Switch };
  const uint8_t popCoil[3]   = { (uint8_t)pop1Coil,   (uint8_t)pop2Coil,   (uint8_t)pop3Coil };
  const uint8_t popLed[3]    = { 64, 65, 66 };
  boolean* popLogic[3]       = { &pop1LogicBool, &pop2LogicBool, &pop3LogicBool };
  boolean* popSw[3]          = { &popsw1, &popsw2, &popsw3 };
  unsigned long* popTimer[3] = { &poptimer1, &poptimer2, &poptimer3 };

  for (uint8_t i = 0; i < 3; i++) {
    if (SimDigitalRead(popSwitch[i]) == LOW && *popLogic[i] == LOW) {
      Score(Scoring::POP_POINTS, Scoring::POP_BONUS);
      *popSw[i] = HIGH;
      *popLogic[i] = HIGH;
      *popTimer[i] = millis();
      ufoInactivesw = 1;
      ufoInactiveTimer = millis();
      ballHandlerSkip = 1;
      ballHandlerSkipTimer = millis();
    }

    if (*popLogic[i] == HIGH && millis() - 50 < *popTimer[i]) {
      digitalWrite(popCoil[i], HIGH);
    }
    else {
      digitalWrite(popCoil[i], LOW);
    }

    if (SimDigitalRead(popSwitch[i]) == HIGH && millis() - 100 > *popTimer[i]) {
      *popLogic[i] = LOW;
    }
  }

  // Lights (villogas talalat utan ~1 mp-ig, aztan alap kek)
  if (effect == LOW) {
    for (uint8_t i = 0; i < 3; i++) {
      if (*popSw[i] == HIGH && millis() - 1000 < *popTimer[i]) {
        leds[popLed[i]] = (ledState == HIGH) ? CRGB::Red : CRGB::Blue;
      }
      else {
        *popSw[i] = LOW;
        leds[popLed[i]] = CRGB::Blue;
      }
    }
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End POPs Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// BonusX
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void BonusXLed() {
  if (bonusx == 0) {
    leds[LED_BONUS_X8] = CRGB::Black; // x8
    leds[LED_BONUS_X6] = CRGB::Black; // x6
    leds[LED_BONUS_X4] = CRGB::Black; // x4
    leds[LED_BONUS_X2] = CRGB::Black; // x2
  }
  // Uj bonusx-szint elerese -> egyszeri hang + "BonusN" trigger.
  // (A lentebbi kijelzo-blokkok szandekosan valtozatlanok: tobb szint-sw
  //  egyszerre aktiv lehet, egymast feluliro LED-mintakkal.)
  boolean* bxsw[4]           = { &bonusx1sw, &bonusx2sw, &bonusx3sw, &bonusx4sw };
  unsigned long* bxtimer[4]  = { &bonusxtimer1, &bonusxtimer2, &bonusxtimer3, &bonusxtimer4 };
  if (bonusx >= 1 && bonusx <= 4 && *bxsw[bonusx - 1] == 0) {
    *bxsw[bonusx - 1] = 1;
    *bxtimer[bonusx - 1] = millis();
    wTrig.trackPlayPoly(TRK_GATESUCCESS);
    Serial.print("Bonus");
    Serial.println(bonusx); // Bonus1..Bonus4
    delay(20);
  }
  if (bonusx > 4) {
    bonusx = 4;
  }




  if (bonusx1sw == 1 && millis() - 1000 < bonusxtimer1) {
    Blinktimer();
    if (ledState == HIGH) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::Black; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
    if (ledState == LOW) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::Black; // x4
      leds[LED_BONUS_X2] = CRGB::Black; // x2
    }

  }
  if (effect == LOW) {
    if (bonusx1sw == 1 && millis() - 1000 > bonusxtimer1) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::Black; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
  }


  if (bonusx2sw == 1 && millis() - 1000 < bonusxtimer2) {
    Blinktimer();
    if (ledState == HIGH) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::White; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
    if (ledState == LOW) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::Black; // x4
      leds[LED_BONUS_X2] = CRGB::Black; // x2
    }

  }
  if (effect == LOW) {
    if (bonusx2sw == 1 && millis() - 1000 > bonusxtimer2) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::White; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
  }

  if (bonusx3sw == 1 && millis() - 1000 < bonusxtimer3) {
    Blinktimer();
    if (ledState == HIGH) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::White; // x6
      leds[LED_BONUS_X4] = CRGB::White; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
    if (ledState == LOW) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::Black; // x4
      leds[LED_BONUS_X2] = CRGB::Black; // x2
    }

  }

  if (effect == LOW) {
    if (bonusx3sw == 1 && millis() - 1000 > bonusxtimer3) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::White; // x6
      leds[LED_BONUS_X4] = CRGB::White; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
  }

  if (bonusx4sw == 1 && millis() - 1000 < bonusxtimer4) {
    Blinktimer();
    if (ledState == HIGH) {
      leds[LED_BONUS_X8] = CRGB::White; // x8
      leds[LED_BONUS_X6] = CRGB::White; // x6
      leds[LED_BONUS_X4] = CRGB::White; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
    if (ledState == LOW) {
      leds[LED_BONUS_X8] = CRGB::Black; // x8
      leds[LED_BONUS_X6] = CRGB::Black; // x6
      leds[LED_BONUS_X4] = CRGB::Black; // x4
      leds[LED_BONUS_X2] = CRGB::Black; // x2
    }

  }

  if (effect == LOW) {
    if (bonusx4sw == 1 && millis() - 1000 > bonusxtimer4) {
      leds[LED_BONUS_X8] = CRGB::White; // x8
      leds[LED_BONUS_X6] = CRGB::White; // x6
      leds[LED_BONUS_X4] = CRGB::White; // x4
      leds[LED_BONUS_X2] = CRGB::White; // x2
    }
  }

}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End BonusX Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Weedspinner
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void Weedspinner() {
  if (weedspsw == 0 && SimDigitalRead(spinnerSwitch) == LOW) {
    weedspsw = 1;
    if (multiball != 0) {
      Score(Scoring::MULTIBALL_SPINNER_POINTS,
            Scoring::MULTIBALL_SPINNER_BONUS);
    }
    else {
      Score(Scoring::SPINNER_POINTS, Scoring::SPINNER_BONUS);
    }
    wTrig.trackPlayPoly(TRK_PLUMB);
    if (spinnersw == 1) {
      // Az elso spinnerfordulat valasztja ki a GET HIGH agat. Az aktualis
      // WEED qualification elfogy, a spinner viszont a golyo/multiball
      // vegeig aktiv marad. Egy korabbi joint UFO-cashoutja nem veszik el.
      if (weedQualified[player] == HIGH) {
        weedQualified[player] = LOW;
        // A spinner elhasznalta a WEED-et. Egy magaban maradt joint nem
        // aktivalhatja az UFO-t; a teljes tier-logika dontse el az allapotot.
        ufosw = (CurrentUfoPartyTier() != UFO_PARTY_NONE) ? 1 : 0;
        PlaySpeechRange(TRK_VO_UFO_SPINNER_SHOOT_A);
        SendPartyEvent("SPINNER");
        SendPartyState();
      }
      weedtableindicator = HIGH;
      weedtableindicatortimer = millis();

      // A 4 multiball-szint (weedm 0->1->2->3->4) kozos logikaja. Szintenkent
      // csak a mero-levonas, a pontszam es a hangok kulonboznek -> tablak.
      int lvl = weedm[player];
      if (lvl >= 0 && lvl <= 3) {
        static const int8_t         mbDecr[4]  = { 25, 15, 10, 8 };
        static const unsigned long  mbScr[4]   = { 10000, 20000, 30000, 40000 };
        static const unsigned long  mbBns[4]   = {   500,  1000,  1500,  2000 };
        static const uint8_t        mbPoly1[4] = { 69, 71, 64, 65 }; // elso hang
        static const uint8_t        mbLoop[4]  = { 89, 88, 64, 65 }; // loopolt zene
        static const uint8_t        mbPoly2[4] = { 89, 88, 68, 70 }; // masodik hang

        weedmeter[player] = weedmeter[player] - mbDecr[lvl];
        if (weedmeter[player] <= 0) {
          weedmeter[player] = 180;
          weedm[player] = lvl + 1;
          BIP = lvl + 2;
          Score(mbScr[lvl], mbBns[lvl]);
          Serial.print("Multiball");
          Serial.println(lvl + 1); // Multiball1..Multiball4
          delay(20);
          multiball = lvl + 1;
          ballsavetimer = millis(); // a ballsave BEALLITASANAK ideje (rollover-biztos)
          ballsaversw = HIGH;
          ballsavetime = 30000;
          ufosw = 0;
          // Multiball szint-lepes: ide korabban ID2 (UFO Lottery) jott, de az
          // mostmar kizarolag a UFO-VUK-weeddel esemenye. Egyelore nincs baked
          // effekt -> tegyunk ide masikat, ha kell.
          wTrig.trackPause(TRK_THEME);
          wTrig.trackPlayPoly(mbPoly1[lvl]);
          wTrig.trackLoop(mbLoop[lvl], 1);
          wTrig.trackPlayPoly(mbPoly2[lvl]);
          spinnersw = 2;
          multiloopsw = 1;
          BrdgLowActive = HIGH;
          BrdgHighActive = HIGH;
        }
      }

      weedmetersend();
      if (cigar < 250) {
        cigar = cigar + 10;
        cigartime = millis();
      }

    }
  }
  if (weedspsw == 1 && SimDigitalRead(spinnerSwitch) == HIGH) {
    weedspsw = 0;
  }
  if (spinnersw == 1) {
    Blinktimer();
    if (ledState ==  HIGH) {
      leds[LED_GET_HIGH_1] = CRGB::Green; // Get High
      leds[LED_GET_HIGH_2] = CRGB::Orange; // Get high
    }
    if (ledState ==  LOW) {
      leds[LED_GET_HIGH_1] = CRGB::Orange; // Get High
      leds[LED_GET_HIGH_2] = CRGB::Green; // Get high
    }
  }

  if (spinnersw == 2) {
    Blinktimer();
    if (ledState ==  HIGH) {
      leds[LED_GET_HIGH_1] = CRGB::Red; // Get High
      leds[LED_GET_HIGH_2] = CRGB::Black; // Get high
    }
    if (ledState ==  LOW) {
      leds[LED_GET_HIGH_1] = CRGB::Black; // Get High
      leds[LED_GET_HIGH_2] = CRGB::Pink; // Get high
    }
  }

  if (spinnersw == 0) {
    leds[LED_GET_HIGH_1] = CRGB::Black; // Get High
    leds[LED_GET_HIGH_2] = CRGB::Black; // Get high
  }

  if (cigar > 0 && cigarState == HIGH && cigartime + 1000 < millis()) {
    cigar = cigar - 5;
  }
  analogWrite(a8, cigar);

  if (weedtableindicator == HIGH && millis() - 1000 < weedtableindicatortimer) {
    if (ledState == HIGH) {
      leds[LED_HIGHTABLE_FLASHER] = CRGB::Red; // Hightable flasher

    }
    else {
      leds[LED_HIGHTABLE_FLASHER] = CRGB::Green; // Hightable flasher

    }
  }
  else {
    leds[LED_HIGHTABLE_FLASHER] = CRGB::Gray; // Hightable flasher

  }

  // Weed-mero kijelzo: progressziv zold kitoltes a szinttel. A vilagitas
  // sorrendje: Acapulco(60), Michoakan(59), Thai(62), Labrador(63) - az elso
  // weedm[player] darab zold, a tobbi szurke.
  static const uint8_t weedMeterLeds[4] = { 60, 59, 62, 63 };
  for (uint8_t i = 0; i < 4; i++) {
    leds[weedMeterLeds[i]] = (i < weedm[player]) ? CRGB::Green : CRGB::Gray;
  }


}
void weedmetersend() {
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write("a");        // sends five bytes
  Wire.write(weedmeter[player]);              // sends one byte
  Wire.endTransmission();
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Weedspinner Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// UFO
/////////////////////////////////////////////////
/////////////////////////////////////////////////


void UFOO() {
  unsigned long now = millis();
  if (ufoWheelWaiting) {
    UpdateUfoWheelPresentation();
    return;
  }
  if (ufoInactivesw == 1 && now - ufoInactiveTimer >= 100UL) {
    ufoInactivesw = 0;
  }

  // Csak addig merunk, amig egy uj golyot keresunk. Stabil talalat utan az
  // ufoshoot/minigame allapot reteszeli az esemenyt; ujra csak a kidobasnal
  // kell ranezni a szenzorra, hogy a golyo tenyleg elhagyta-e a VUK-ot.
  boolean stableBallPresent = false;
  if (ufoInactivesw == 0 && ufoshoot == 0) {
    ufoanalog = AnalogSensorReadStable(PIN_A5);
    if (MunchiesWaitingForUfoClear()) {
      // A kidobas utan csak a felengedesre varunk. Uj golyot addig nem
      // fogadunk el, es a 40 ADC-egyseges hiszterezis kiszuri a hataron
      // billego mintakat.
      ufoDetectStartedAt = 0;
    }
    else if (ufoanalog < analogThreshold[5]) {
      if (ufoDetectStartedAt == 0) ufoDetectStartedAt = now;
      stableBallPresent = (now - ufoDetectStartedAt >= UFO_DETECT_STABLE_MS);
    }
    else {
      ufoDetectStartedAt = 0;
    }
  }
  else {
    ufoDetectStartedAt = 0;
  }

  if (stableBallPresent && ufoshoot == 0) {
    ufoDetectStartedAt = 0;
    ufoEjectSaveStarted = LOW;
    if (ufosw == 1 && multiball == 0) {
      // Az UFO jutalmat mar a golyo beerkezesekor reteszeljuk. A tier
      // kovetkezetesen elfogy: sima WEED cashout, 1 joint Super Cashout,
      // 2 joint Feature Wheel, 3 joint/Love Pack SpaceCoke.
      ufoAwardTier = CurrentUfoPartyTier();
      lottery = DrawUfoLottery(ufoAwardTier);
      ConsumeUfoPartyReward(ufoAwardTier);
      if (ufoAwardTier == UFO_PARTY_CASHOUT) SendPartyEvent("CASHOUT");
      if (ufoAwardTier == UFO_PARTY_SUPER_CASHOUT) SendPartyEvent("SUPER_CASHOUT");
      if (ufoAwardTier == UFO_PARTY_FEATURE_WHEEL) SendPartyEvent("FEATURE_WHEEL");
      if (ufoAwardTier == UFO_PARTY_LOVE_PACK) SendPartyEvent("SPACE_COKE");
      if (ufoAwardTier == UFO_PARTY_FEATURE_WHEEL) {
        StartUfoWheelPresentation();
        return;
      }
      if (lottery == 9 && hurryUp == LOW) {
        StartMunchiesMode();
        return;
      }
      BeginUfoLotteryPresentation(HIGH);
      // FONTOS a return: a ConsumeUfoPartyReward() mar nullazta az ufosw-t,
      // igy nelkule a lenti "nincs weed" ag azonnal ratolt volna egy
      // ufoshoot = random(1,4)-et a most beallitott ufoshoot = 4-re. Ettol
      // a jutalom-blokk (ufoshoot == 4) sosem futott le - elmaradt a
      // SpaceCoke multiball es a lottery hangja -, helyette a golyot
      // kilokte a "get the fuck out" ag.
      return;
    }

    if (ufosw == 0 && multiball == 0 && hurryUp == 0) {
      ufoshoot = random(1, 4);
      if (ufoshoot == 1) {
        // Csak ez a hosszabb, 4.3 mp-es VUK-ban tartott "UFO FUCK" ag kapja
        // a GUI-videot es a hosszu ID4 fenyanimaciot. A masik ket varians
        // szandekosan gyorsan visszaadja a golyot, hogy porogjon a jatek.
        // Az ID4 (UFO FUCK) intrója egyszer megy le, utana a rajzban
        // megjelolt loop-szakaszon marad, amig a VUK vissza nem dobja a
        // golyot. (A teljes effekt ujraindítasa az introt is ismetelte.)
        StartHoldingBakedEffect(4);
        wTrig.trackPause(TRK_THEME);
        PlaySpeechRange(TRK_VO_UFO_NO_WEED_LONG_A);
        Serial.println("Ufo6");
        delay(20);
      }
      if (ufoshoot == 2) {
        wTrig.trackPause(TRK_THEME);
        PlaySpeechRange(TRK_VO_UFO_NO_WEED_EJECT_A);
      }
      if (ufoshoot == 3) {
        wTrig.trackPause(TRK_THEME);
        PlaySpeechRange(TRK_VO_UFO_NO_WEED_ALARM_A);
      }
    }

    if (ufosw == 0 && multiball != 0) {
      ufoshoot = 5;
    }
    if (ufosw == 0 && hurryUp == HIGH) {
      ufoshoot = 5;
    }

    ufoshoottimer = millis();
    ufoshoottimer2 = millis();
  }

  if (ufoshoot == 1 && ufoshoottimer2 < millis() - 4300) {
    StartUfoEjectBallSave(UFO_EJECT_BALL_SAVE_MS);
    digitalWrite(ufoCoil, HIGH);
    if (millis() - 50 > ufoshoottimer + 4300) {
      digitalWrite(ufoCoil, LOW);
      if (millis() - 500 > ufoshoottimer + 4300) {
        ufoshoot = 0;
        StopFullBakedEffect(); // a loopolo UFO FUCK effekt vege
        initlight = HIGH;
        Initlights();
        if (multiball == 0) {
          wTrig.trackResume(TRK_THEME);
        }
        wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        Score(Scoring::UFO_EJECT_POINTS, Scoring::UFO_EJECT_BONUS);
      }
    }
  }

  if (ufoshoot == 2 && ufoshoottimer2 < millis() - 2000) {
    StartUfoEjectBallSave(UFO_EJECT_BALL_SAVE_MS);
    digitalWrite(ufoCoil, HIGH);
    if (millis() - 50 > ufoshoottimer + 2000) {
      digitalWrite(ufoCoil, LOW);
      if (millis() - 500 > ufoshoottimer + 2000) {
        ufoshoot = 0;
        if (multiball == 0) {
          wTrig.trackResume(TRK_THEME);
        }
        wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        Score(Scoring::UFO_EJECT_POINTS, Scoring::UFO_EJECT_BONUS);
      }
    }
  }

  if (ufoshoot == 3 && ufoshoottimer2 < millis() - 1500) {
    StartUfoEjectBallSave(UFO_EJECT_BALL_SAVE_MS);
    digitalWrite(ufoCoil, HIGH);
    if (millis() - 50 > ufoshoottimer + 1500) {
      digitalWrite(ufoCoil, LOW);
      if (millis() - 500 > ufoshoottimer + 1500) {
        ufoshoot = 0;
        if (multiball == 0) {
          wTrig.trackResume(TRK_THEME);
        }
        wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        Score(Scoring::UFO_EJECT_POINTS, Scoring::UFO_EJECT_BONUS);
      }
    }
  }

  //// Itt ad is az ufo valamit

  if (ufoshoot == 4 && ufoshoottimer2 < millis() - UFO_LOTTERY_HOLD_MS) {
    StartUfoEjectBallSave((lottery == 7) ?
                          SPACECOKE_BALL_SAVE_MS : UFO_EJECT_BALL_SAVE_MS);
    digitalWrite(ufoCoil, HIGH);
    if (millis() - 50 > ufoshoottimer + UFO_LOTTERY_HOLD_MS) {
      digitalWrite(ufoCoil, LOW);
      if (millis() - 700 > ufoshoottimer + 4000) {
        if (lottery  == 1) {    /// ExtraBall
          wTrig.trackPlayPoly(TRK_FIREWORK);
          PlayBakedEffectOnce(16); // intro + loop + cooldown, egyszer
        }
        if (lottery  == 2) {    /// HurryUp
          wTrig.trackPlayPoly(TRK_FIREWORK);
          hurryUp = HIGH;
          hurryUpTimer = millis();
          // A GUI ebbol rakja ki a lukteto "2X"-et es a visszaszamlalot.
          Serial.print("HurryUp,");
          Serial.println(HURRY_UP_DURATION_MS / 1000UL);
          StopFullBakedEffect(); // az UFO lottery full effektje leall
          StartHurryUpBakedOverlay(); // ID6 atlatszo legfelso retegkent marad
          fasz = 68;
          initlight = HIGH;
          Initlights();
          ResetHurryUpLights();
          spinnersw = 2;
          ufosw = 0;
        }
        if (lottery  == 3) {    /// 15000
          wTrig.trackPlayPoly(TRK_FIREWORK);
          PlaySpeechRange(TRK_VO_UFO_CASHOUT_15000_A);
        }
        if (lottery  == 4) {    /// 20000
          wTrig.trackPlayPoly(TRK_FIREWORK);
          PlaySpeechRange(TRK_VO_UFO_CASHOUT_20000_A);
        }
        if (lottery  == 6) {    /// 25000
          wTrig.trackPlayPoly(TRK_FIREWORK);
          PlaySpeechRange(TRK_VO_UFO_CASHOUT_25000_A);
        }
        if (lottery  == 5) {    /// 30000
          wTrig.trackPlayPoly(TRK_FIREWORK);
          PlaySpeechRange(TRK_VO_UFO_CASHOUT_30000_A);
        }
        // A negy sima pont-kifizetesnek nincs sajat mondanivaloja, de a
        // tobbi jutalom mellett furcsa a sotetseg -> kozos strobe.
        if (lottery >= 3 && lottery <= 6) {
          PlayBakedEffectOnce(17);
        }
        if (lottery  == 7) {    /// SpaceCoke Multi
          BIP = 5;
          multiball = 5;
          // Az Ufo9 video mar a belepeskor lement; a fenyeffekt ITT indul,
          // a multiball tenyleges kezdetekor.
          PlayBakedEffectOnce(14);
          // Ide korabban ID4 (UFO FUCK) jott, de az mostmar kizarolag a
          // UFO-no-weed esemenye. Egyelore nincs baked effekt -> tegyunk ide
          // masikat, ha kell (multiball-start).
          PlaySpeechRange(TRK_VO_UFO_SPACE_COKE_START_A);
          wTrig.trackPlayPoly(TRK_FIREWORK);
          wTrig.trackPlayPoly(TRK_MUS_SPACECOKE);
          wTrig.trackPlayPoly(TRK_CHEECH_SPACECOKE); // filmes Cheech-orditas
          ufosw = 0;
          spinnersw = 2;
          BrdgLowActive = HIGH;
          BrdgHighActive = HIGH;
        }
        if (lottery  == 8) {    /// Pontlopas (Ufo10..13)
          wTrig.trackPlayPoly(TRK_FIREWORK); // Firework
          wTrig.trackPlayPoly(TRK_COLLECT + ufoMinus); // 124..127 = kirabolt jatekos hangja
          PlayBakedEffectOnce(17);
          ufoMinus = 0;
        }
        if (lottery == 10) {    /// Extra Ball Lit (Ufo8)
          wTrig.trackPlayPoly(TRK_FIREWORK);
          PlaySpeechRange(TRK_VO_UFO_EXTRA_BALL_LIT_A);
          PlayBakedEffectOnce(17); // a KIGYUJTAS; a beszedes az ID16-ot kapja
        }


        fasz = 68;
        initlight = 1;
        Initlights();
        ufoshoot = 0;

        if (lottery == 2) {
          wTrig.trackPlayPoly(TRK_MUS_HURRY);
          wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        }
        if (lottery == 1 || lottery == 3 || lottery == 4 || lottery == 5 ||
            lottery == 6 || lottery == 8 || lottery == 10)
        {
          wTrig.trackResume(TRK_THEME);
          wTrig.trackPlayPoly(TRK_SHOOTOUTUFO);
        }
        ufosw = 0;
      }
    }
  }

  if (ufoshoot == 5 && ufoshoottimer2 < millis() - 1500) {
    StartUfoEjectBallSave(UFO_EJECT_BALL_SAVE_MS);
    digitalWrite(ufoCoil, HIGH);
    if (millis() - 50 > ufoshoottimer + 1500) {
      digitalWrite(ufoCoil, LOW);
      if (millis() - 500 > ufoshoottimer + 1500) {
        ufoshoot = 0;
        Score(Scoring::UFO_EJECT_POINTS, Scoring::UFO_EJECT_BONUS);
      }
    }
  }

  uint8_t tier = CurrentUfoPartyTier();
  if (ufosw == 1 && tier != UFO_PARTY_NONE) {
    Blinktimer();
    if (ledState == HIGH) {
      if (tier == UFO_PARTY_CASHOUT) {
        leds[LED_UFO_ARROW_1] = CRGB::Purple;
        leds[LED_UFO_ARROW_2] = CRGB::Black;
      }
      else if (tier == UFO_PARTY_SUPER_CASHOUT) {
        leds[LED_UFO_ARROW_1] = CRGB::Orange;
        leds[LED_UFO_ARROW_2] = CRGB::Gold;
      }
      else if (tier == UFO_PARTY_FEATURE_WHEEL) {
        leds[LED_UFO_ARROW_1] = CRGB::Magenta;
        leds[LED_UFO_ARROW_2] = CRGB::White;
      }
      else if (tier == UFO_PARTY_LOVE_PACK) {
        leds[LED_UFO_ARROW_1] = CRGB::Cyan;
        leds[LED_UFO_ARROW_2] = CRGB::White;
      }
    }
    if (ledState == LOW) {
      if (tier == UFO_PARTY_CASHOUT) {
        leds[LED_UFO_ARROW_1] = CRGB::Black;
        leds[LED_UFO_ARROW_2] = CRGB::Purple;
      }
      else if (tier == UFO_PARTY_SUPER_CASHOUT) {
        leds[LED_UFO_ARROW_1] = CRGB::Gold;
        leds[LED_UFO_ARROW_2] = CRGB::Orange;
      }
      else if (tier == UFO_PARTY_FEATURE_WHEEL) {
        leds[LED_UFO_ARROW_1] = CRGB::White;
        leds[LED_UFO_ARROW_2] = CRGB::Magenta;
      }
      else if (tier == UFO_PARTY_LOVE_PACK) {
        leds[LED_UFO_ARROW_1] = CRGB::White;
        leds[LED_UFO_ARROW_2] = CRGB::Cyan;
      }
    }
  }
  else {
    // Az overlay effektek csak a sajat szines pixeleiket irjak felul, a
    // transparent frame-ek nem torlik a leds[] korabbi tartalmat. Ezert az
    // inaktiv UFO-nyilak alapallapotat minden korben explicit visszaallitjuk,
    // kulonben Weedblast/Joint/Combo utan halvany szin maradhat rajtuk.
    leds[LED_UFO_ARROW_1] = CRGB::Black;
    leds[LED_UFO_ARROW_2] = CRGB::Black;
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End UFO Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Cheech & Chong
/////////////////////////////////////////////////
/////////////////////////////////////////////////



/// 
///  Inactive state
///
void Chong_switch() {
  if (SimDigitalRead(chongSwitch) == LOW && chongoffsw == LOW) {
    chongoffsw = HIGH;
    chongoffswtimer = millis();
    TriggerHurryHit(HURRY_ZONE_CHONG);
    // Chong beszedhangok (a regi 10 case-es switch helyett)
    static const uint8_t chongTracks[10] = { 8, 9, 52, 53, 9, 79, 80, 82, 83, 85 };
    PlaySpeech(chongTracks, 10);
    
    /// 
    /// Active state
    ///
    if (chongLightActiveSw == HIGH) {
      CollectTimer = millis();
      CollectSw = LOW;
      chongLightActiveSw = LOW;
      cheechLightActiveSw = LOW;
      chongCollectives[player] += 1;
      PlayBakedEffectOnce(7); // ChongCollect: egyszer
      if (chongCollectives[player] == 1) {
          Serial.println("ChongC1");
          delay(20);
          Score(Scoring::COLLECTIBLE_POINTS[0], Scoring::COLLECTIBLE_BONUS[0]);
          wTrig.trackPlayPoly(TRK_COLLECT);
          wTrig.trackPlayPoly(TRK_WEEDPIPE);
      }
      if (chongCollectives[player] == 2) {
          Serial.println("ChongC2");
          delay(20);
          Score(Scoring::COLLECTIBLE_POINTS[1], Scoring::COLLECTIBLE_BONUS[1]);
          wTrig.trackPlayPoly(TRK_COLLECT);
          wTrig.trackPlayPoly(TRK_COCKROACH);
      }
      if (chongCollectives[player] == 3) {
          Serial.println("ChongC3");
          delay(20);
          Score(Scoring::COLLECTIBLE_POINTS[2], Scoring::COLLECTIBLE_BONUS[2]);
          wTrig.trackPlayPoly(TRK_COLLECT);
          wTrig.trackPlayPoly(TRK_PIPEWRENCH);
          chongCollectives[player] = 0;
      }
    }
    else {
      Score(Scoring::INACTIVE_CHARACTER_POINTS,
            Scoring::INACTIVE_CHARACTER_BONUS);
    }
  }

  if (millis() - 1000 > chongoffswtimer) {
    chongoffsw = LOW;
  }

  if (chongLightActiveSw == HIGH) {
      Blinktimer();
      if (ledState == HIGH) {
      leds[LED_CHONG_IND] = CRGB::Red; // C
    }
    if (ledState == LOW) {
      leds[LED_CHONG_IND] = CRGB::Yellow; // C
    }
  }
  if (chongLightActiveSw == LOW) {
          leds[LED_CHONG_IND] = CRGB::Black; // C
          leds[LED_CHEECH_IND] = CRGB::Black; // C
  }

}




void Cheech_switch() {
    /// 
    ///  Inactive state
    ///

    if (SimDigitalRead(cheechSwitch) == LOW && cheechoffsw == LOW) {
    cheechoffsw = HIGH;
    cheechoffswtimer = millis();
    TriggerHurryHit(HURRY_ZONE_CHEECH);
    // Cheech beszedhangok (a regi 15 case-es switch helyett)
    static const uint8_t cheechTracks[15] = { 7, 35, 36, 37, 48, 49, 50, 54, 55, 56, 57, 58, 59, 81, 87 };
    PlaySpeech(cheechTracks, 15);

    /// 
    ///  Active state
    ///

    if (cheechLightActiveSw == HIGH) {
      chongLightActiveSw = LOW;
      cheechLightActiveSw = LOW;
      CollectTimer = millis();
      CollectSw = 1;
      cheechCollectives[player] += 1;
      PlayBakedEffectOnce(8); // CheechCollect: egyszer
      if (cheechCollectives[player] == 1) {
          Serial.println("CheechC1");
          delay(20);
          Score(Scoring::COLLECTIBLE_POINTS[0], Scoring::COLLECTIBLE_BONUS[0]);
          wTrig.trackPlayPoly(TRK_COLLECT);
          wTrig.trackPlayPoly(TRK_CHAINWHEEL);

      }
      if (cheechCollectives[player] == 2) {
          Serial.println("CheechC2");
          delay(20);
          Score(Scoring::COLLECTIBLE_POINTS[1], Scoring::COLLECTIBLE_BONUS[1]);
          wTrig.trackPlayPoly(TRK_COLLECT);
          wTrig.trackPlayPoly(TRK_BIGJOINT);
      }
      if (cheechCollectives[player] == 3) {
          Serial.println("CheechC3");
          delay(20);
          Score(Scoring::COLLECTIBLE_POINTS[2], Scoring::COLLECTIBLE_BONUS[2]);
          wTrig.trackPlayPoly(TRK_COLLECT);
          wTrig.trackPlayPoly(TRK_LICENSEPLATE);
          cheechCollectives[player] = 0;

      }
    }
    else {
      Score(Scoring::INACTIVE_CHARACTER_POINTS,
            Scoring::INACTIVE_CHARACTER_BONUS);
    }
  }

  if (millis() - 1000 > cheechoffswtimer) {
    cheechoffsw = LOW;
  }

  if (cheechLightActiveSw == HIGH) {
    Blinktimer();
    if (ledState == HIGH) {
      leds[LED_CHEECH_IND] = CRGB::Yellow; // C
    }
    if (ledState == LOW) {
      leds[LED_CHEECH_IND] = CRGB::Red; // C
    }
  }
  if (cheechLightActiveSw == LOW) {
      leds[LED_CHONG_IND] = CRGB::Black; // C
      leds[LED_CHEECH_IND] = CRGB::Black; // C
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Cheech && Chong Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Cheech && Chong Collectives
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void Collectives() {
  if (cheechCollectives[player] == 0) {
    leds[LED_CHEECH_WHEEL] = CRGB::Orange; // Cheech wheel
    leds[LED_CHEECH_CIGAR] = CRGB::Orange; // Cheech cigar
    leds[LED_MUF_DVR] = CRGB::Orange; // MUF dvr
  }
  if (chongCollectives[player] == 0) {
    leds[LED_CHONG_PIPE] = CRGB::Orange; // Chong Pipe
    leds[LED_CHONG_ROACH] = CRGB::Orange; // Chong Cockroach
    leds[LED_CHONG_KEY] = CRGB::Orange; // Chong plunger key
  }
  if (cheechCollectives[player] == 1) {
    leds[LED_CHEECH_WHEEL] = CRGB::White; // Cheech wheel
    leds[LED_CHEECH_CIGAR] = CRGB::Orange; // Cheech cigar
    leds[LED_MUF_DVR] = CRGB::Orange; // MUF dvr
  }
  if (chongCollectives[player] == 1) {
    leds[LED_CHONG_PIPE] = CRGB::White; // Chong Pipe
    leds[LED_CHONG_ROACH] = CRGB::Orange; // Chong Cockroach
    leds[LED_CHONG_KEY] = CRGB::Orange; // Chong plunger key
  }
  if (cheechCollectives[player] == 2) {
    leds[LED_CHEECH_WHEEL] = CRGB::White; // Cheech wheel
    leds[LED_CHEECH_CIGAR] = CRGB::White; // Cheech cigar
    leds[LED_MUF_DVR] = CRGB::Orange; // MUF dvr
  }
  if (chongCollectives[player] == 2) {
    leds[LED_CHONG_PIPE] = CRGB::White; // Chong Pipe
    leds[LED_CHONG_ROACH] = CRGB::White; // Chong Cockroach
    leds[LED_CHONG_KEY] = CRGB::Orange; // Chong plunger key
  }
  if (cheechCollectives[player] == 3) {
    leds[LED_CHEECH_WHEEL] = CRGB::White; // Cheech wheel
    leds[LED_CHEECH_CIGAR] = CRGB::White; // Cheech cigar
    leds[LED_MUF_DVR] = CRGB::White; // MUF dvr
  }
  if (chongCollectives[player] == 3) {
    leds[LED_CHONG_PIPE] = CRGB::White; // Chong Pipe
    leds[LED_CHONG_ROACH] = CRGB::White; // Chong Cockroach
    leds[LED_CHONG_KEY] = CRGB::White; // Chong plunger key
  }

  if (CollectSw == 1 && millis() - 1000 < CollectTimer) {
    if (ledState == HIGH) {
      leds[LED_CHEECH_WHEEL] = CRGB::White; // Cheech wheel
      leds[LED_CHEECH_CIGAR] = CRGB::White; // Cheech cigar
      leds[LED_MUF_DVR] = CRGB::White; // MUF dvr
      leds[LED_CHONG_PIPE] = CRGB::White; // Chong Pipe
      leds[LED_CHONG_ROACH] = CRGB::White; // Chong Cockroach
      leds[LED_CHONG_KEY] = CRGB::White; // Chong plunger key
    }
    if (ledState == LOW) {
      leds[LED_CHEECH_WHEEL] = CRGB::Yellow; // Cheech wheel
      leds[LED_CHEECH_CIGAR] = CRGB::Yellow; // Cheech cigar
      leds[LED_MUF_DVR] = CRGB::Yellow; // MUF dvr
      leds[LED_CHONG_PIPE] = CRGB::Yellow; // Chong Pipe
      leds[LED_CHONG_ROACH] = CRGB::Yellow; // Chong Cockroach
      leds[LED_CHONG_KEY] = CRGB::Yellow; // Chong plunger key
    }
  }
  else {
    CollectSw = 0;
  }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Cheech && Chong Collectives rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// GIFT
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void Gift() {
  if (giftsw == 0) {
    switch (randGift) {
      case 1:
        cncswitch1 = 0;
        break;
      case 2:
        cncswitch2 = 0;
        break;
      case 3:
        cncswitch3 = 0;
        break;
      case 4:
        weedswitch1 = 0;
        break;
      case 5:
        weedswitch2 = 0;
        break;
      case 6:
        weedswitch3 = 0;
        break;
      case 7:
        weedswitch4 = 0;
        break;
      case 8:
        fishTankLightState1 = 0;
        break;
      case 9:
        fishTankLightState2 = 0;
        break;
    }
  }
  giftsw = 3;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End GIFT Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Bridge LOW
/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Kozos hidkezelo: a ket ~180 soros, majdnem azonos BridgeLow/High rutin
// egy parameterezett fuggvenybe vonva. FONTOS aszimmetriak, amiket megoriz:
//  - a ket hid KOMBOZIK egymassal: comboReadT/comboWriteT kereszthivatkozas
//  - a multiball-jackpot tabla KULONBOZO (lasd a ket wrappert)
//  - az elso talalat (nem-kombo) hangja kulonbozo (firstHitSound)
//  - a kombovideo karaktere a BEFEJEZO hidhoz tartozik
void BridgeCommon(uint8_t swPin, boolean* swFlag, unsigned long* swT,
                  boolean* active, uint8_t ledActA, uint8_t ledActB,
                  uint8_t ledAmbA, uint8_t ledAmbB,
                  unsigned long* comboReadT, unsigned long* comboWriteT,
                  uint8_t firstHitSound, const char* comboVideoPrefix,
                  uint8_t comboEffectId, boolean suppressFeedback,
                  const unsigned long* jpScr, const unsigned long* jpBns,
                  boolean armsLoopCombo,
                  unsigned long hurryScr, const char* hurryVideo) {
  // A hid-kombo SZANDEKOSAN csak multiballon kivul el: azert van, hogy
  // multiball nelkul is lehessen skill-lovesekbol pontot gyujteni. Ma a
  // hidlampa amugy is pontosan multiball alatt eg, de a multiball == 0
  // kiirasa a szandekot rogziti, nem erre az egybeesesre tamaszkodik.
  if (*active == LOW && multiball == 0) {
    leds[ledActA] = CRGB::Black;
    leds[ledActB] = CRGB::Black;

    if (SimDigitalRead(swPin) == LOW && *swFlag == 0) {
      *swFlag = 1;
      *swT = millis();

      if (hurryUp == HIGH) {
        // A Score() a Hurry Up alatt megduplazza a kozvetlen pontot, ezert
        // a hidankent atadott alap FELE a kiirt osszegnek - ugyanaz a "2X",
        // amit a GUI is mutat.
        Score(hurryScr, Scoring::BRIDGE_BONUS);
        Serial.println(hurryVideo);
        delay(20);
      }
      else {
        const unsigned long comboNow = millis();
        const unsigned long comboAge = comboNow - *comboReadT;
        if (*comboReadT == 0 || comboAge >= BRIDGE_COMBO_WINDOW_MS) {
          comboCounter = 0;
          Score(Scoring::BRIDGE_POINTS, Scoring::BRIDGE_BONUS);
          if (!suppressFeedback) wTrig.trackPlayPoly(firstHitSound);
        }
        else { // a masik hid ota meg nem telt el az 5,5 mp-es komboablak
          comboCounter++;
          if (comboCounter > 6) {
            comboCounter = 6;
          }
          static const uint8_t comboSound[6] = {
            TRK_COMBO1, TRK_COMBO2, TRK_COMBO1,
            TRK_COMBO1, TRK_COMBO2, TRK_COMBO1
          };
          uint8_t comboIndex = comboCounter - 1;
          Score(Scoring::COMBO_POINTS[comboIndex],
                Scoring::COMBO_BONUS[comboIndex]);
          if (!suppressFeedback) {
            Serial.print(comboVideoPrefix);
            Serial.println(comboCounter);
            wTrig.trackPlayPoly(comboSound[comboIndex]);
            PlayBakedEffectOnce(comboEffectId);
          }
        }
        *comboWriteT = millis();
      }
    }

    if (*swFlag == 1 && millis() - 1000 < *swT) {
      if (ledState == LOW) {
        leds[ledAmbA] = CRGB::Red;
        leds[ledAmbB] = CRGB::Yellow;
      }
      if (ledState == HIGH) {
        leds[ledAmbA] = CRGB::Yellow;
        leds[ledAmbB] = CRGB::Red;
      }
    }
    else {
      *swFlag = 0;
      leds[ledAmbA] = CRGB::White;
      leds[ledAmbB] = CRGB::White;
    }
  }

  if (*active == HIGH || multiball != 0) {
    if (effect == LOW) {
      if (ledState == LOW) {
        leds[ledActA] = CRGB::Yellow;
        leds[ledActB] = CRGB::Green;
      }
      if (ledState == HIGH) {
        leds[ledActA] = CRGB::Green;
        leds[ledActB] = CRGB::Yellow;
      }
    }

    // multiball 0..5 -> jackpot-tabla indexe (a 0 = sima "Point2" hidtalalat)
    if (SimDigitalRead(swPin) == LOW && *swFlag == 0) {
      *swFlag = 1;
      *swT = millis();
      if (multiball == 0) {
        Score(jpScr[multiball], jpBns[multiball]);
      }
      else {
        ScoreJackpot(jpScr[multiball], jpBns[multiball]);
        // Csak a nagyhid nyitja meg a loop-ablakot, es csak multiballban.
        if (armsLoopCombo) highLoopArmT = millis();
      }
      if (suppressFeedback) {
        // Az EXTRA BALL collect video/callout elsoseget kap ugyanazon a lovesen.
      }
      else if (multiball == 0) {
        wTrig.trackPlayPoly(TRK_SCORE_5000);
        Serial.println("Point2");
      }
      else {
        PlayJackpotFeedback(jpScr[multiball]);
      }
      delay(20);
      if (multiball == 0) {
        // A sima aktiv hid regi fenye marad; jackpotnal az ID19 fut.
        effect = HIGH;
        effectID = 5;
      }
    }

    if (*swFlag == 1 && millis() > *swT + 1000) {
      *swFlag = 0;
      if (multiball == 0) {
        *active = LOW;
      }
      leds[ledActA] = CRGB::Black;
      leds[ledActB] = CRGB::Black;
    }
  }
}

void BridgeLow() {
  // A ROLL A JOINT csak aktiv WEED + legalabb egy betarazott sor mellett
  // veszi at a kishidat. Ilyenkor nem ad ugyanarra a kapcsolozarasra normal
  // bridge/combo pontot. Multiballban a jackpot mindig elsobbseget elvez.
  if (RollJointLit()) {
    if (effect == LOW) {
      Blinktimer();
      if (ledState == HIGH) {
        leds[LED_LEFT_RAMP_1] = CRGB::Orange;
        leds[LED_LEFT_RAMP_2] = CRGB::Gold;
        leds[LED_LEFT_RAMP_AMBIENT] = CRGB(96, 24, 0);
      }
      else {
        leds[LED_LEFT_RAMP_1] = CRGB::Gold;
        leds[LED_LEFT_RAMP_2] = CRGB::Orange;
        leds[LED_LEFT_RAMP_AMBIENT] = CRGB(32, 8, 0);
      }
    }
    if (SimDigitalRead(bridgeLowSwitch) == LOW && BrdgLowSw == 0) {
      BrdgLowSw = 1;
      BrdgLowT = millis();
      RollJoint();
    }
    return;
  }

  //                                    mb: 0     1      2      3      4      5
  static const unsigned long jpScr[6] = { 5000, 10000, 15000, 20000, 25000, 30000 };
  static const unsigned long jpBns[6] = {  200,   200,   200,   200,   200,   200 };
  BridgeCommon(bridgeLowSwitch, &BrdgLowSw, &BrdgLowT, &BrdgLowActive,
               24, 25, 23, 17, &comboTimerH, &comboTimerL, 9,
               "ComboChong", 9, false, jpScr, jpBns, false,
               Scoring::HURRY_BRIDGE_LOW_POINTS, "Point5");   // 15000
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Bridge LOW Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Bridge HIGH
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void BridgeHigh() {
  // A nagyhid mindig egy fokkal feljebb fizet a kishidnal. A 0. elem a
  // multiball NELKULI, sima "Point2" hidtalalat - az a hozza tartozo 5000-es
  // hanggal es videoval egyutt jar, ezert marad 5000.
  //                                    mb: 0     1      2      3      4      5
  static const unsigned long jpScr[6] = { 5000, 15000, 20000, 25000, 30000, 50000 };
  static const unsigned long jpBns[6] = {  200,   500,   500,   500,   500,   500 };
  boolean collectedExtraBall = CollectExtraBallLitAtHighRamp();
  BridgeCommon(bridgeHighSwitch, &BrdgHighSw, &BrdgHighT, &BrdgHighActive,
               36, 37, 50, 51, &comboTimerL, &comboTimerH, 36,
               "ComboCheech", 10, collectedExtraBall, jpScr, jpBns, true,
               Scoring::HURRY_BRIDGE_HIGH_POINTS, "Point7");  // 25000
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Bridge HIGH Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Add player
/////////////////////////////////////////////////
/////////////////////////////////////////////////
// Az AddPlayer() (start gombbal jatek kozben) megszunt: a jatekosok
// hozzaadasa a player select modban tortenik (intmMode, intmon == 3).
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Add Player Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// HurryUp
/////////////////////////////////////////////////
/////////////////////////////////////////////////

void ResetHurryUpLights() {
  hurryChaseStartedAt = millis();
  for (uint8_t i = 0; i < HURRY_ZONE_COUNT; i++) hurryHitAt[i] = 0;
}

void StopHurryUpLights() {
  hurryChaseStartedAt = 0;
  for (uint8_t i = 0; i < HURRY_ZONE_COUNT; i++) hurryHitAt[i] = 0;
}

void TriggerHurryHit(uint8_t zone) {
  if (hurryUp == HIGH && zone < HURRY_ZONE_COUNT) {
    hurryHitAt[zone] = millis();
  }
}

void RunUfoWheelChaseLights() {
  if (ufoWheelWaiting != HIGH || effect != HIGH || effectID != 15) return;
  if (ufoWheelChaseStartedAt == 0) ufoWheelChaseStartedAt = millis();

  const unsigned long now = millis();
  const uint_farptr_t ledBase = pgm_get_far_address(hurryPathLed);
  static const uint8_t trailBrightness[6] = { 255, 150, 88, 48, 24, 10 };
  uint8_t head = ((now - ufoWheelChaseStartedAt) / HURRY_CHASE_STEP_MS) %
                 HURRY_PATH_COUNT;

  // Ugy viselkedik, mint egy also reteg: csak a Wheel kocka fekete helyein
  // latszik at. A szines Wheel-pixeleket nem irja vagy fakitja felul.
  for (uint8_t trail = 0; trail < 6; trail++) {
    uint8_t pathIndex = (head + HURRY_PATH_COUNT - trail) % HURRY_PATH_COUNT;
    uint8_t led = pgm_read_byte_far(ledBase + pathIndex);
    if (leds[led] == CRGB::Black) {
      CRGB comet = CRGB(255, 72, 0);
      comet.nscale8_video(trailBrightness[trail]);
      leds[led] = comet;
    }
  }
}

void RunHurryUpLights() {
  if (hurryUp != HIGH) return;
  if (hurryChaseStartedAt == 0) ResetHurryUpLights();

  const unsigned long now = millis();
  const uint_farptr_t ledBase = pgm_get_far_address(hurryPathLed);
  const uint_farptr_t zoneBase = pgm_get_far_address(hurryPathZone);

  // A mod sajat utvonala sotet alaprol indul, igy a futofeny a korabbi
  // collection/progress lampak mellett is tisztan olvashato.
  for (uint8_t i = 0; i < HURRY_PATH_COUNT; i++) {
    leds[pgm_read_byte_far(ledBase + i)] = CRGB::Black;
  }

  // Borostyan comet: egy eros fej es ot fokozatosan halvanyulo farok.
  static const uint8_t trailBrightness[6] = { 255, 150, 88, 48, 24, 10 };
  uint8_t head = ((now - hurryChaseStartedAt) / HURRY_CHASE_STEP_MS) %
                 HURRY_PATH_COUNT;
  for (uint8_t trail = 0; trail < 6; trail++) {
    uint8_t pathIndex = (head + HURRY_PATH_COUNT - trail) % HURRY_PATH_COUNT;
    uint8_t led = pgm_read_byte_far(ledBase + pathIndex);
    CRGB comet = CRGB(255, 72, 0);
    comet.nscale8_video(trailBrightness[trail]);
    leds[led] += comet;
  }

  // Talalat: az egesz erintett palyaelem feheren felvillan, majd ciankekbe
  // hajolva 1,4 masodperc alatt simul vissza a chase-be.
  for (uint8_t zone = 0; zone < HURRY_ZONE_COUNT; zone++) {
    if (hurryHitAt[zone] == 0) continue;
    unsigned long age = now - hurryHitAt[zone];
    if (age >= HURRY_HIT_FADE_MS) {
      hurryHitAt[zone] = 0;
      continue;
    }
    uint8_t strength = 255 - (uint32_t)age * 255UL / HURRY_HIT_FADE_MS;
    CRGB flash = (age < 90UL) ? CRGB::White : CRGB(0, 190, 255);
    for (uint8_t i = 0; i < HURRY_PATH_COUNT; i++) {
      if (pgm_read_byte_far(zoneBase + i) == zone) {
        uint8_t led = pgm_read_byte_far(ledBase + i);
        leds[led] = blend(leds[led], flash, strength);
      }
    }
  }
}


void HurryUp()
{
  if (hurryUp == HIGH && millis() - HURRY_UP_DURATION_MS < hurryUpTimer)
  {
    if (hurryChaseStartedAt == 0) ResetHurryUpLights();
  }
  if (hurryUp == HIGH && millis() - HURRY_UP_DURATION_MS > hurryUpTimer)
  {
    hurryUp = LOW;
    Serial.println("HurryUp,0"); // a GUI leveszi a 2X-et
    effect = LOW;
    effectID = 0;
    StopHurryUpLights();
    StopHurryUpBakedOverlay();
    initlight = HIGH;
    Initlights();
    fasz = 68;
    wTrig.trackPause(TRK_MUS_HURRY);
    wTrig.trackResume(TRK_THEME);
    RestorePartyShotsForPlayer();
    SendPartyState();
  }



}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End HurrryUp Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// Tilt
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void Tilt() {
    const unsigned long now = millis();
    const boolean contactClosed = (SimDigitalRead(PIN_A12) == LOW);

    // Egy hosszan kilengo harang csak EGY warningot adhat. Uj warninghoz a
    // kontaktusnak elobb fel kell engednie, majd 750 ms-ig folyamatosan
    // nyitva kell maradnia. Ez a commercial "tilt debounce/settle" viselkedes.
    if (tiltContactLatched == HIGH) {
      if (contactClosed) {
        tiltOpenSince = 0;
      }
      else {
        if (tiltOpenSince == 0) {
          tiltOpenSince = now;
        }
        else if (now - tiltOpenSince >= TILT_SETTLE_MS) {
          tiltContactLatched = LOW;
          tiltOpenSince = 0;
        }
      }
    }

    if (contactClosed && tiltContactLatched == LOW) {
      tiltContactLatched = HIGH;
      tiltOpenSince = 0;
      tiltWarnings++;

      if (tiltWarnings <= TILT_WARNINGS_ALLOWED) {
        wTrig.trackPlayPoly(TRK_MELLOWOUT);
        wTrig.trackPlayPoly(TRK_TILT1);
        StartFullBakedEffect(11, 3, LOW); // Danger: harom teljes Tilt-fenykor
        Serial.println(tiltWarnings == 1 ? "Danger" : "Danger2");
      }
    }

    if (tiltWarnings > TILT_WARNINGS_ALLOWED) {
        ballTilted = HIGH;
        wTrig.stopAllTracks();
        Serial.println("Tilt");
        wTrig.trackPlayPoly(TRK_TILT1);
        wTrig.trackPlayPoly(TRK_TILT2);
        wTrig.trackPlayPoly(TRK_TILT3);
        StartFullBakedEffect(11, 1, HIGH); // drainig vegtelen Tilt-feny
        ballsaversw = LOW;
        digitalWrite(leftFlipperBat, LOW);
        digitalWrite(rightFlipperBat, LOW);
        digitalWrite(rightFlipperBat, LOW);
        digitalWrite(leftSlingshotCoil, LOW);
        digitalWrite(rightSlingshotCoil, LOW);
        digitalWrite(ufoCoil, LOW);
        digitalWrite(pop1Coil, LOW);
        digitalWrite(pop2Coil, LOW);
        digitalWrite(pop3Coil, LOW);
        digitalWrite(shooterlaneCoil, LOW);
        while (BIS != 5) {
            MIV(HIGH);
            RunLightEffect();
            FastLED.show();
            delay(1000 / UPDATES_PER_SECOND);
        }
        StopFullBakedEffect();
        // A warning/allapot reset a kovetkezo golyo inittable() rutinjaban
        // tortenik. Addig maradjon reteszelve, nehogy ujra lefusson.
        tiltWarnings = TILT_WARNINGS_ALLOWED;
    }
}

void ResetTiltWarningsForBall() {
    tiltWarnings = 0;
    tiltOpenSince = 0;
    // Ha golyovaltaskor a harang meg eppen a gyurun van, ne kapjon az uj
    // jatekos azonnal fantom warningot: elobb varjuk meg a stabil felengedest.
    tiltContactLatched = (SimDigitalRead(PIN_A12) == LOW) ? HIGH : LOW;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Tilt Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////
