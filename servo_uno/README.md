# CnC Servo Uno

Az Uno a flipper Mega vezerlojenek I2C slave eszkoze a `0x08` cimen. A kod a
gepbol kiolvasott legacy firmware valodi bekoteset hasznalja, nem PCA9685
shieldet.

## Bekotes

- Uno `D4` -> 1. szervo jel (legacy weed meter)
- Uno `D5` -> 2. szervo jel
- Uno `D6` -> 3. szervo jel
- Uno `D7` -> szervotap rele bemenet, aktiv HIGH
- Uno `A4 / SDA` -> Mega `D20 / SDA`
- Uno `A5 / SCL` -> Mega `D21 / SCL`
- Uno GND -> Mega GND -> kulso 5 V szervotap GND
- a rele csak a szervok kulso `+5 V` agat kapcsolja

A szervokat ne az Uno 5 V pinjerol taplald. Harom SG90-hez stabil 5 V-os,
legalabb 3 A-es tap javasolt. A szervok kozelebe erdemes 1000-2200 uF
pufferkondenzatort tenni a tap es GND koze.

A Deek-Robot motor shield D4-D7 labjai a shield aramkorehez is csatlakoznak.
Ezeken a motorcsatornakat nem szabad a szervokkel egy idoben hasznalni.

## I2C protokoll

Minden parancs ket bajt:

| Parancs | Jelentes |
|---|---|
| `'a', 0..180` | 1. szervo / weed meter (legacy kompatibilis) |
| `'b', 0..180` | 2. szervo |
| `'c', 0..180` | 3. szervo |
| `'x', 0` | azonnali leallitas es szervotap-lekapcsolas |

A Mega jelenlegi `weedmetersend()` fuggvenye az eredeti ketbajtos `'a'` + ertek
csomagot kuldi, ez valtoztatas nelkul mukodik.

I2C request eseten az Uno 13 bajtos statuszt ad vissza:

`'S', version, state, activeChannel, faults, knownMask, scheduledMask,`
`currentA, currentB, currentC, targetA, targetB, targetC`

## Szervovedelem

- nincs automatikus indulas kori mozgatas
- nincs `String`, memoriafoglalas vagy soros kiiras az I2C interruptban
- 150 ms tapstabilizalasi ido a rele bekapcsolasa utan
- 15 ms-onkent egyfokos, finom poziciorampa
- bekapcsolas utan az elso parancs kozvetlenul a kert, korlatozott
  impulzusu poziciot adja ki, mert pozicio-visszajelzes nelkul nincs biztos
  kiindulopont; a kesobbi mozgasok mar rampazottak
- 450 ms beallasi ido a celpozicioban
- a PWM-jel ezutan leall, majd a rele lekapcsolja a szervotapot
- 4 masodperces tap-watchdog megszorulas vagy hibas parancssor ellen
- detach utan a jelkimenet kenyszeritetten LOW, hogy ne taplalja vissza a szervot
- a pozicioparancsok csatornankent osszevonodnak: mindig a legfrissebb cel ervenyes

Az alap impulzustartomany mindharom csatornan `900..2100 us`, lenyegesen
szukebb a regi Servo library kb. `544..2400 us` tartomanyanal. A mechanikai
vegallasokat csatornankent a `SERVO_CONFIG` tablaban lehet finomhangolni.

## Elso teszt

1. A szervokart vagy mutatot vedd le.
2. Ellenorizd a barna=GND, piros=kulso +5 V, sarga/narancs=jel polaritast.
3. Eloszor csak az 1. szervot csatlakoztasd.
4. Kuldd ra az `a,90` parancsot, majd probald az `a,30` es `a,150` ertekeket.
5. Csak ezutan allitsd be a mechanikai vegallasokat es szereld vissza a kart.

A Servo library Timer1-et hasznal, ezert D9 es D10 hardveres PWM funkcioja nem
hasznalhato mellette, bar digitalis I/O-kent tovabbra is hasznalhatok.
