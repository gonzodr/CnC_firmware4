# Refaktor-tesztpad (regressziós háló)

A firmware alrendszerenkénti refaktorjához: minden változtatás után a
tesztpadi Mega SIM-demója ugyanazt a soros eseménysort kell adja, mint a
`golden_demo.events.txt` referencia. Ha a diff üres, a refaktor
bizonyítottan megőrizte a viselkedést.

## Előfeltétel: determinizmus

A SIM-init fix `randomSeed()`-et állít (`0xC0FFEE`), így a demó minden
lefutása BITRE azonos. Enélkül a 6 `random()`-hívás miatt a diff zajos
lenne. (Éles buildben — SIM_MODE ki — nincs seed, marad a változatosság.)

## Egy iteráció (a Pi-n)

```bash
# 1. refaktor a forrasban (F: gepen), commit+push
# 2. Pi: pull + flash a padra
cd ~/CnC_firmware4 && git pull && bash test/flash_sim.sh
# 3. golden rogzitese egy teljes ciklusra (~300s)
~/CnC-Pinball-GUI-V3/venv/bin/python3 test/watch.py 300 /tmp/fw/uj
# 4. diff a golden ellen (a "Start"-tol)
awk '/^Start$/{p=1} p' /tmp/fw/uj.events.txt > /tmp/fw/uj.trim
diff test/golden_demo.events.txt /tmp/fw/uj.trim && echo "TISZTA - a refaktor OK"
```

## Mit fed le és mit NEM

### Pontozasi smoke test

A kezi `H` parancs SIM modban ki-/bekapcsolja a Hurry Up allapotot. Ugyanazt
a kapcsolot normal, majd Hurry Up modban megnyomva a `T,score` sor `scr`
erteke pontosan ketszeres, a `bns` pedig valtozatlan kell legyen. Pelda:

```text
s   -> scr=250,bns=25
H
s   -> scr=500,bns=25
```

Az `E`, majd `u`, vegul `x` sorozat cinkelt `Ufo8` EXTRA BALL LIT
lottery-kimenetet, UFO-kidobast es nagyhidas `ExtraB` collectet tesztel.

A balance pass szandekosan megvaltoztatja a golden demo pontsorait. A
kovetkezo padi futasnal uj `golden_demo.events.txt` baseline-t kell rogzitani;
a regi fajl a refaktor elotti pontozas torteneti referenciaja addig.

### Jackpot-pontozas es video (2026-09-04)

A jackpot kulon emelest kap a Hurry Up 2x szorzo UTAN: 40000 helyett 50000,
60000 helyett 100000. A `T,score` es a `Jackpot_<pont>` sor ugyanazt az
osszeget kell tartalmazza. Az alap 10000/15000/20000/25000/30000 jackpotok
normal modban valtozatlanok; Hurry Up alatt 20000/30000/50000/50000/100000
jar ertuk. A loop alap 30000-es jackpotja Hurry Up alatt 100000.

A padon ellenorizendo: bonusz valtozatlan; jackpot-emeles nem kap ujabb 2x
szorzot; a sima `Score(20000, ...)` Hurry Up alatt tovabbra is 40000-et,
a `Score(30000, ...)` 60000-et ad. Az emeles csak `ScoreJackpot`-ra ervenyes.

Jackpotnal a `Jackpot_<pont>` utan egy `T,speech,<track>` jon SIM buildben.
A bemondo minden talalatnal fuggetlen 50/50 valasztas: Cheech vagy Chong.

| Jovairt pont | Cheech | Chong |
|---|---|---|
| 10000 | 209 | 210 |
| 15000 | 211 | 212 |
| 20000 | 213 | 214 |
| 25000 | 221 | 222 |
| 30000 | 299 | 300 |
| 50000 | 305 | 306 |
| 100000 | 307 | 308 |

Az uj hang melle nem indul el a regi 073-as bemondas. Hurry Up alatt a
20000-es alapbol 50000 pont es 305/306 hang, a 30000-es alapbol 100000 pont
es 307/308 hang kell legyen. Mindket hid es a loop ugyanazt a feedback
helpert hasznalja. A forrasszintu kiosztas-ellenorzes kulon futtathato:
`python -m unittest discover -s test -p test_jackpot_audio.py`.

Jackpot-feny: ID19 FULL, 40 frame x 50 ms, ket teljes lejatszas (kb. 4 s).
Uj jackpot ujrainditja a ket kort. A masodik kor utan visszaall a jatekfeny;
a regi loop-ID1 / hid-ID5 nem indul ra. Elozo UFO hold/vegtelen allapot nem
oroklodhet. A Danger tovabbra is 3 kort fut, a Tilt pedig drainig vegtelent.
Forrasszintu ellenorzes: `python -m unittest discover -s test -p test_jackpot_lights.py`.

- **Lefedi:** pontozás (Score choke-point trace), játékfolyam, videó-
  triggerek, hangválasztás ott, ahol soros-látható.
- **NEM fedi:** a LED-rutinokat (a pad nem capture-özi a szalagot) és a
  hardver-közeli időzítést (MIV/debounce - a SIM kiváltja a bemeneteket).
  Ezeket érveléssel + a gépen ellenőrizzük, nem a diffel.

## Fájlok

Highscore/I2C regresszio: `python -m unittest discover -s test -p test_highscore_serial_i2c.py`.
A GUI `Exit1\n` parancsat most a kozos parser kezeli highscore modban is.
A Wire timeout 25 ms, a servo-meter legfeljebb haromszor probalkozik,
250 ms idokozzel. A fizikai A13 resetet es a szervo UNO hibaesetet csak a
gepen lehet veglegesen igazolni.

- `golden_demo.events.txt` - a referencia eseménysor (2 játékos, teljes
  parti multiballal + SpaceCoke-kal + névbeírással)
- `watch.py` - soros-figyelő, kanonikus eseménysort ír
- `flash_sim.sh` - padi SIM-flash; a `SIM_MODE` es `SIM_BUILD_CONFIRMED`
  build flageket egyutt adja at, a forrast nem modositja
