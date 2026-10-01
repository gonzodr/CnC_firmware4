/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// COIL GUARD - tekercsvedelem
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//
// Fuggetlen "or" a Timer5 megszakitasabol (1 kHz), ami akkor is fut,
// ha a fo loop blokkolodik (delay, while, lefagyas). Ha egy tekercs
// a limitjenel tovabb van folyamatosan behuzva, kenyszerrel leoldja,
// es GUARD_LOCKOUT ideig lehuzva tartja (hutes + ujraprobalkozas ellen).
// A loop-bol hivott CoilGuardReport() kiirja a sorosra, melyik tekercs
// oldott le -> ha ilyet latsz, valahol bug van a jateklogikaban!
//
// A jateklogikahoz NEM nyul hozza, tisztan raepulo vedelem.
// Timer5 szabad (regen az AltSoftSerial hasznalta).

#define GUARD_COILS 8
#define GUARD_LOCKOUT 3000 // ms - leoldas utan ennyi ideig tiltva

struct GuardCoil {
  volatile uint8_t* outReg; // PORT latch: firmware-parancs + kenyszer-leoldas
  uint8_t mask;
  uint16_t limit;           // max folyamatos behuzas [ms]
  uint16_t onTime;          // eddig ennyi ideje HIGH [ms]
  uint16_t lockout;         // hatralevo tiltasi ido [ms]
  uint8_t trips;            // eddigi leoldasok szama
};

GuardCoil guardCoils[GUARD_COILS];
volatile uint16_t guardTripPending = 0; // bitmaszk: melyik tekercs oldott le
const char* const guardNames[GUARD_COILS] = {
  "trough", "shooter", "slingL", "slingR",
  "pop1", "pop2", "pop3", "ufo"
};

void CoilGuardAdd(uint8_t idx, uint8_t pin, uint16_t limit_ms) {
  guardCoils[idx].outReg = portOutputRegister(digitalPinToPort(pin));
  guardCoils[idx].mask = digitalPinToBitMask(pin);
  guardCoils[idx].limit = limit_ms;
  guardCoils[idx].onTime = 0;
  guardCoils[idx].lockout = 0;
  guardCoils[idx].trips = 0;
}

// Setup vegen hivando (a pinMode-ok utan!)
void CoilGuardInit() {
  CoilGuardAdd(0, ballTroughCoil, 200);     // normal impulzus: 50 ms
  CoilGuardAdd(1, shooterlaneCoil, 200);    // normal impulzus: 50 ms
  CoilGuardAdd(2, leftSlingshotCoil, 100);  // normal impulzus: 12 ms
  CoilGuardAdd(3, rightSlingshotCoil, 100); // normal impulzus: 12 ms
  CoilGuardAdd(4, pop1Coil, 200);           // normal impulzus: 50 ms
  CoilGuardAdd(5, pop2Coil, 200);           // normal impulzus: 50 ms
  CoilGuardAdd(6, pop3Coil, 200);           // normal impulzus: 50 ms
  CoilGuardAdd(7, ufoCoil, 200);            // normal impulzus: 50 ms

  // Timer5 beallitasa: CTC mod, 16 MHz / 64 / 250 = 1 kHz
  noInterrupts();
  TCCR5A = 0;
  TCCR5B = (1 << WGM52) | (1 << CS51) | (1 << CS50); // CTC, /64 eloosztas
  TCNT5 = 0;
  OCR5A = 249;
  TIMSK5 = (1 << OCIE5A);
  interrupts();
}

// 1 ms-onkent fut, a fo loop allapotatol fuggetlenul
ISR(TIMER5_COMPA_vect) {
  for (uint8_t i = 0; i < GUARD_COILS; i++) {
    GuardCoil &c = guardCoils[i];

    if (c.lockout) {
      // tiltas alatt: barmit is ir a jateklogika, lehuzva tartjuk
      *c.outReg &= ~c.mask;
      c.lockout--;
      if (c.lockout == 0) {
        c.onTime = 0;
      }
      continue;
    }

    // A firmware altal vezerelt PORT latchot figyeljuk, nem a fizikailag
    // visszaolvasott PIN regisztert. A kabinet driver/pull-up halozata a PIN
    // biteket kikapcsolt kimenetnel is HIGH-nak mutathatja; az minden tekercsen
    // harommasodpercenkenti fantom lockoutot okozott.
    if (*c.outReg & c.mask) {
      c.onTime++;
      if (c.onTime >= c.limit) {
        *c.outReg &= ~c.mask;        // KENYSZER-LEOLDAS
        c.lockout = GUARD_LOCKOUT;
        c.trips++;
        guardTripPending |= (1 << i);
      }
    }
    else {
      c.onTime = 0;
    }
  }
}

// A loop()-bol hivando: kiirja, ha az or kozbelepett
void CoilGuardReport() {
  if (guardTripPending == 0) {
    return;
  }
  noInterrupts();
  uint16_t pending = guardTripPending;
  guardTripPending = 0;
  interrupts();

  for (uint8_t i = 0; i < GUARD_COILS; i++) {
    if (pending & (1 << i)) {
      Serial.print("CoilGuard,");
      Serial.print(guardNames[i]);
      Serial.print(",");
      Serial.println(guardCoils[i].trips);
    }
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
//// End Coil Guard Rutin
/////////////////////////////////////////////////
/////////////////////////////////////////////////
