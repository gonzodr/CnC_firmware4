# CnC Pinball - jelenlegi jatekszabaly- es feedback-audit

Audit datum: 2026-08-23

Vizsgalt allapot:

- firmware: `ade55b8` (`main`)
- GUI: `238bd1f` (`main`)
- firmware hanghivasok: WAV Trigger track ID-k alapjan
- videok: a GUI `src/video_catalog.py` katalogusa es a tenyleges
  `src/assets/Videos` konyvtarak alapjan
- fenyek: a normal LED-rutinok, az `effectID` hivasok es az
  `effect_data.h` alapjan

## Fontos korlatok

- A WAV Trigger SD-kartya tartalma nincs a repoban. Az audit azt tudja
  igazolni, hogy a firmware ker-e hangot; azt nem, hogy a kert track tenyleg
  rajta van-e a gep SD-kartyajan, es a tartalma megfelelo-e.
- A jelenlegi SIM regresszios teszt a pontokat, soros triggereket es nehany
  hangvalasztast latja, a WS2812 fenyeket nem rogzititi.
- A videoknal a mappa letezeset ellenoriztuk. A klip dramaturgiai tartalmat
  (peldaul tenylegesen mondja-e, hogy mit kell kovetkezoleg loni) kulon
  vizualis tartalomauditban kell ellenorizni.

## Jeloles

- **OK**: van ertelmes, az esemenyhez kotheto visszajelzes.
- **Reszleges**: van valamilyen visszajelzes, de nem eleg egyertelmu vagy
  generikus.
- **Hianyzik**: nincs implementalt visszajelzes vagy nincs tenyleges asset.
- **Nem kell**: mikroesemeny, amelynel a mechanikai reakcio es egy rovid
  feny/hang eleg; teljes kepernyos video inkabb zavarna a jatekot.

## 1. Esemennyel leltar

| Esemény | Jelenlegi pont / jutalom | Palyafeny | GUI-video | Hang | Jatekosi irany | Minosites / teendo |
|---|---:|---|---|---|---|---|
| Golyokiloves | 15 s ball save | Shooter ID3 full effekt | Nincs | Tema indul az elso kilovesnel | Ballsave LED | **OK**, video nem kell |
| Sling, normal | 250 +25 bonus | Nincs kulon jatekfeny | Nem kell | `BIGYONG` | Nincs | **OK** mikroesemeny |
| Sling, Hurry Up | 500 +25 bonus | Borostyan chase; a talalt sling feher-ciankek villanas utan 1,4 s alatt kihuny; felette az ID6 transparent overlay loopol | Nincs video-spam | `BIGYONG` | A folyamatos korfutas jelzi a modot | **OK/Reszleges**: a 2x mod hatralevo idejet kesobb HUD-on is erdemes mutatni |
| Pop bumper, normal | 250 +10 bonus | Kek alap, talalatkor piros/kek villanas | Nem kell | Nincs kulon WAV-hivas; a tekercs/mechanika hallatszik | Nincs | **OK/Reszleges**: rovid pop-hang opcionális |
| Pop bumper, Hurry Up | 500 +10 bonus | A Hurry Up chase fut; a pop megtartja sajat mechanikai reakciojat | Nincs | Nincs kulon | A korfutas jelzi a modot | **Reszleges**: pop nincs a chase-utvonalban, a 2x mod idejet kesobb HUD mutassa |
| C/N/C betutalalat | 1500 +50 bonus | A betu feherre valt | Nincs | Betunkent kulon track | A betuk mutatjak a progresszt | **OK** |
| C&N&C teljes | 5000 +500 bonus | Harom betu villog; Cheech es Chong indikator aktiv | Nincs | Nincs completion/callout | Nem mondja ki, hogy Cheechet vagy Chongot kell loni | **Reszleges**: pontozas kesz; completion hang, rovid feny-overlay es `SHOOT CHEECH OR CHONG` GUI-uzenet hianyzik |
| Aktiv Cheech talalat | 10k/15k/20k +500/1000/2000 bonus | Collectible LED-ek lepnek; CheechCollect ID8 egyszer lefut | `CheechC1..3`, mind megvan | Beszed + collect + targyhang | A karakterindikator villog | **OK**, a 3. utan kulon sorozat-complete jelzes kell |
| Aktiv Chong talalat | 10k/15k/20k +500/1000/2000 bonus | Collectible LED-ek lepnek; ChongCollect ID7 egyszer lefut | `ChongC1..3`, mind megvan | Beszed + collect + targyhang | A karakterindikator villog | **OK**, a 3. utan kulon sorozat-complete jelzes kell |
| Inaktiv Cheech/Chong | 200 +50 bonus | Nincs aktiv karakterfeny | Nem kell | Veletlen karakterbeszed | Inaktiv lampak | **OK** |
| WEED betutalalat | 1000 +50 bonus | Betu feherre valt | Nincs | Betunkent kulon hang | A betuk mutatjak a progresszt | **OK** |
| WEED teljes | 5000 +500 bonus | Weedblast ID5; UFO, GET HIGH es beer eseten ROLL A JOINT kigyullad | `Weed`, megvan | `WEEDFULL` + veletlen completion hang | Party HUD kiirja a valaszthato utakat | **OK**: a qualification jatekosonkent, golyok kozott megmarad |
| Spinner inaktiv | Fordulatonkent 250 +10 bonus | GET HIGH sotet | Nem kell | `PLUMB` minden fordulatnal | Inaktiv lampak | **OK** pontozas; eros porgetesnel a hangterheles meg tesztelendo |
| Spinner aktiv | 250 +10, valamint multiball-meter csokken | GET HIGH zold/narancs; high-table flasher; kulso I2C meter | Nem kell minden spinre | `PLUMB` | A hatralevo ertek csak az I2C meterbol latszik | **Reszleges**: GUI-n is kell `GET HIGH n/180` vagy szazalek |
| Multiball 1..4 indul | 10k/20k/30k/40k +500/1000/1500/2000 bonus | Hidak es loop jackpotra villognak; ball save LED; nincs sajat start-effekt | Trigger van, de a `Michokan`, `Acapulco Gold`, `Thai Stick`, `Labrador` PNG-k hianyoznak | Szintenkent sajat zene/hang | A jackpotnyilak vilagitanak | **Hianyzik**: 4 videoasset + 4 rovid start-fenyeffekt/callout vagy egy parameterezett kozos effekt |
| Multiball vege | Nincs end bonus | Jackpotfenyek kialszanak, tema visszaall | Nincs | Multiball-zene leall, tema vissza | Nincs | **Reszleges**: rovid `MULTIBALL ENDED` allapotu GUI-uzenet hasznos, video nem kell |
| Normal teljes loop | 2500 +250 bonus | Nincs aktiv loopnyil | Nincs | `BANANA`/`BLOB` | Nincs | **OK**, teljes video nem kell |
| Multiball loop jackpot | 30000 +2000 bonus, multiballonként egyszer | Loop nyil villog; Jackpot ID1 overlay | `Jackpot6` trigger van, PNG hianyzik | `JACKPOT` + `BLOB` | Loop nyil egyertelmu | **Hianyzik**: Jackpot6 videoasset; a fenyes/hangos reakcio jo |
| Normal hidtalalat | 1000 +100 bonus | Hidrampa ambient villan | Nincs | Hidankent kulon hang | Nincs | **OK** pontozas; iranyjelzes csak comboablakban kell |
| Hid-combo 1..6 | 2500/5k/7.5k/10k/15k/20k +100/150/200/250/300/500 bonus | Kishid/Chong: ID9 egyszer; nagyhid/Cheech: ID10 overlay egyszer | 6 Cheech + 6 Chong sequence; a befejezo hid karaktere szerint | Ket generikus combo hang valtakozik | A karakteres video visszajelez, de az 5,5 s-os kovetkezo celablakot nem mutatja | **Reszleges**: pont/video/fenyeffekt bekotve; az ellenkezo hid 5,5 s-ig villogjon, GUI-n `COMBO - SHOOT LEFT/RIGHT` |
| WEED+beer ROLL A JOINT | 5k/10k/20k +250/500/1000 bonus | Kishid borostyan/arany | GUI PartyEvent; Joint-videok hianyoznak | `BIGJOINT` | HUD mutatja a beer/joint keszletet es az UFO tiert | **Reszleges**: szabaly kesz; dedikalt videok/fenyeffektek hianyoznak |
| Multiball hid-jackpot | Szinttol/hidtol fuggoen 10k..30k +200/500 bonus | Mindket hid zold/sarga villogas; talalatkor Weedblast ID5 | `Jackpot2..6` trigger van, mind az 5 PNG hianyzik | `JACKPOT` | Mindket hid egyertelmuen vilagit | **Hianyzik**: jackpot videok; ID5 neve/tartalma Weedblast, nem biztos hogy hid-jackpothoz illik |
| FISH vagy TANK | 1500 +50 bonus | A talalt cel feher | Nincs | `PING` vagy `BEER` | Celprogressz latszik | **OK** |
| FISH+TANK par, 1/2 | 2500 +250 bonus; +1 beer-kredit | Betuk villognak, ambient kek | `Beer1/Beer2`, megvan | Nincs kulon completion hang a ket cel hangjan felul | Party HUD mutatja a tarolt beert | **OK/Reszleges**: pont/video/HUD kesz, completion hang opcionális |
| FISH+TANK par, 3 | 2500 +250 bonus; beer-keszlet 3/3 | Beer villogas + ciankek ambient | `Beer3`, megvan | Celhangok | A beer nem nullazodik es nem elesit onallo jackpotot | **OK**: a beer a ROLL A JOINT eroforrasa |
| D/A/V/E rollover | Nincs pont; betuprogressz | Betu feher | Nincs | `DAVE` | Flipperrel lane-change mukodik | **OK**: nem celzott loves, hanem lane/outlane mentomechanika |
| DAVE teljes | Nincs pont; legalabb 10 s ball save | Betuk villognak; ballsave LED villog | Nincs | Csak a reset vegen `DAVENOTHERE` | A ballsave LED jelez, de nincs activation callout | **Reszleges**: a save kesz; `BALL SAVE LIT` hang/GUI hianyzik |
| Gate rollover | 500 +25 bonus | Kapu feher; ambient narancs villanas | Nincs | `KVAKK` | Flipperes lane-change latszik | **OK** |
| Narancs Drift gate | 1500 +50 bonus | Narancs jeloles + ambient | `Drift`, megvan | Talalat + ket celebration hang | Narancs cel egyertelmu | **OK** |
| Harom gate teljes | Bonus X: x2/x4/x6/x8 | Harom kapu villog; Bonus X lampak | `Bonus1..4`, mind megvan | `GATESUCCESS` + reset hang | Szorzo lampak egyertelmuek | **OK** |
| Gift helyes cel | 5000 +100 vagy +500 bonus | Kijelolt cel zold/sarga + sarga futofeny | `Point2` -> 5000, megvan | Celhang + celebration + `SHOOTOUTUFO` | Nem magyarazza el a gift szabalyat | **Reszleges**: sajat `GIFT` uzenet/callout kell; a Point2 tul generikus |
| Gift rossz cel | Normal talalati pont; gift elveszik | A gift-feny megszunik | Nincs | Nincs kudarcjelzes | A jatekos nem tudja, miert tunt el | **Szabaly-/kommunikacios hiba**: ne vesszen el rossz talalatra, vagy legyen idozitett es jol kommunikalt |
| UFO inaktiv, gyors kidobas | 0 +300 bonus; kidobaskor 5 s vedelmi save | UFO-nyil sotet | Ket agon nincs video; 1/3 agon `Ufo6` -> `Ufofuck` megvan es ID4 | Harom veletlen hangvarians | Inaktiv nyil | **OK**: a save csak a VUK ket kar koze ejteset vedi |
| UFO-lottery indul | Tierfuggo jutalom; kidobaskor 5 s vedelmi save | UFO Lottery ID2 full effekt; tierfuggo nyilszin | A 2-jointos Feature Wheelhez 3 kulon 6 s-os sequence, utana Extra Ball/Hurry Up agban 5 s-os eredmenyvideo; LovePack placeholder | `HAPPYUFO`, jutalomhang | Cash Out / Super / Feature / Love Pack HUD | **OK/Reszleges**: a Wheel vegen indul az eredmenyvideo es vele parhuzamosan a jutalom/kidobas; LovePack meg hianyzik |
| UFO extra ball | +1 golyo | Lottery effekt + ballsave | `Ufo5`, megvan | Firework + Extra Ball | Nincs kulon playfield insert az extra ballhoz | **OK**, a GUI-video potolja |
| UFO Extra Ball Lit | Nem ad azonnal golyot; a nagy hidat kvalifikalja | Lottery effekt; dedikalt high-ramp feny meg nincs | `Ufo8`, megvan | Firework + atmeneti `SHOOTBRIDGE` | A video jelzi a lit allapotot, a collect helye meg nincs palyafenynel kiemelve | **Reszleges**: lottery/high-ramp szabaly kesz; dedikalt rampafeny kell |
| High-ramp Extra Ball collect | +1 golyo; a lit allapot megszunik | Dedikalt light show meg nincs | `ExtraB` -> `Extraball`, megvan | `EXTRABALL` | Ugyanazon a lovesen az extra-ball feedback elsoseget kap a combo/jackpothoz kepest | **Reszleges**: collect video kesz, dedikalt light show meg hianyzik |
| UFO Hurry Up | 5000 +100 bonus es 80 s mod | Borostyan korfutas CnC/Chong/WEED/Cheech/Fishtank/sling/ballsave utvonalon; talalati falloff; legfelul az ID6 transparent overlay loopol | Uj `HURRY_UP_MASTER` eredmenyvideo a Wheel utan | Firework, Hurry Up, modzene | A chase es az ID6 folyamatosan jelzi az aktiv modot | **OK/Reszleges**: pontozas egységesen 2x; hatralevo ido kesobb GUI-HUD-ra kerulhet |
| UFO pontjutalmak | 15k/20k/25k/30k + bonus | Lottery ID2 | `Ufo1..4`, megvannak | Firework + ertekhang | Jutalom egyertelmu a videobol/hangbol | **OK** |
| UFO SpaceCoke | Love Pack collect: 40000 +2000 bonus; 5 golyos MB; 30 s save | Cián/feher UFO, majd lottery effekt es jackpotfenyek | `LovePack` placeholder + `Ufo9` | 4 reszes sajat hang/zene | `LOVE PACK READY - SHOOT UFO` | **OK/Reszleges**: sajat Love Pack es SpaceCoke start-fenyeffekt/video kell |
| UFO pontlopas | Masik jatekos -10000; Super Cashoutnal -20000 | Lottery effekt | `Ufo10..13`, megvannak | Firework + aldozat hangja | A video jelzi az aldozatot, `Steal` protokoll a pontos osszeget | **OK** |
| Munchies indul | GUI altal szamolt bonus; kidobaskor 5 s vedelmi save | Saját sotet idle reteg; good/bad/collection effektek | Teljes minijatek, nem sima klip | GUI/minijatek hangjai; tema pause | Minijatek UI vezeti | **OK** |
| Danger 1 | Nincs pont | Tilt ID11 harom teljes kor | `Danger`, 102 frame | `MELLOWOUT` + Tilt1 | Elso warning kulon animaciot es 3x fenyeffektet kap | **OK** |
| Danger 2 | Nincs pont | Tilt ID11 harom teljes kor | `Danger2`, 180 frame | `MELLOWOUT` + Tilt1 | Masodik warning kulon animaciot es 3x fenyeffektet kap | **OK** |
| Tilt (3. kulon kontaktus) | Golyó es golyovegi bonusz elveszik, tekercsek leallnak | Tilt ID11 folyamatosan loopol a drainig | `Tilt`: 0-59 intro egyszer, 60-179 loop 30 fps-sel a drain `Next`/`End` uzeneteig | Harom tilt hang | Folyamatos Tilt-feny, video es hang | **OK** |
| Golyó vege / bonus count | Felhalmozott bonus x2/x4/x6/x8 | Nincs bonus-count fenyfolyam | `Next`/`End` csak allapotvaltas | `LARDASS` vagy end hang | GUI frissiti a score-t, de nincs latvanyos bonus-elszamolas | **Reszleges**: kesobb bonus-count animacio; nem blokkolja a WEED-stack fejlesztest |

## 2. Tenylegesen hianyzo GUI-videoassetek

A GUI-katalogus mar ismeri a triggereket, de a helyi `src/assets/Videos`
konyvtarban ezekhez nincs sequence:

- `Acapulco Gold`
- `Michokan`
- `Thai Stick`
- `Labrador`
- `Jackpot2`
- `Jackpot3`
- `Jackpot4`
- `Jackpot5`
- `Jackpot6`
- `Extraball` (a katalogusban van es a V4 mar `ExtraB` collectkor kuldi,
  de a sequence meg hianyzik)

Az alabbi katalogusbejegyzesek arva/regiek:

- `Point1`, `Point3..Point8`: a V4 a balance pass ota csak `Point2` triggert
  kuld; a regi Hurry Up mikro-switch `Point1` video-spam megszunt.
- `Jackpot1` / `PsychedelicJackpot`: a V4 sosem kuldi.

A regi `Combo1..Combo6` triggernevek csak visszamenoleges kompatibilitashoz
maradtak a GUI-katalogusban. A V4 most `ComboCheech1..6` es
`ComboChong1..6` neveket kuld; a 12 beagyazott sequence jelen van.

## 3. Fenyeffekt-leltar

A baked effektbankban jelenleg tizenegy effekt van:

| ID | Nev | Hasznalat | Megjegyzes |
|---:|---|---|---|
| 1 | Loop - Jackpot | Multiball loop-jackpot | Jo esemeny-egyezes; overlaynek van deklaralva, de a jatek `effect=HIGH` full utjan inditja. Ezt technikailag ellenorizni kell. |
| 2 | UFO Lottery | UFO-jutalmak | Jo |
| 3 | Shooter | Az aktualis golyo elso kilovese | Tema + effekt egyszer; visszagurulas es ball-save utani automatikus ujrarugas csendes |
| 4 | UFO FUCK | Az egyik inaktiv UFO-ag | Jo |
| 5 | Weedblast | WEED complete es minden kvalifikalt hid/jackpot | WEED-hez jo, hid-jackpothoz tematikusan generikus/teves |
| 6 | Hurry UP | Hurry Up es minden Hurry Up talalat | Jo mod-alap, de minden talalatnal ujraindulhat/megzavarhatja az olvashatosagot |
| 7 | ChongCollect | Aktiv Chong collectible | Full effekt, egyszer fut le; a Light Editor nem canvasnak exportalta |
| 8 | CheechCollect | Aktiv Cheech collectible | Full effekt, egyszer fut le; a Light Editor nem canvasnak exportalta |
| 9 | ComboLowBridge | Kishid/Chong sikeres combo | Full effekt, egyszer fut le; a Light Editor nem canvasnak exportalta |
| 10 | ComboHighBridge | Nagyhid/Cheech sikeres combo | Overlay/canvas effekt; 666 transzparens es 276 festett cella az exportban |
| 11 | Tilt | Danger es Tilt | Dangernel 3 teljes kor; Tiltnel drainig loop |

Hianyzo, prioritasos fenyeffektek:

1. multiball-start (lehet egy kozos effekt negy szinvariacioval);
2. híd-jackpot kulon overlay;
3. C&C complete / `SHOOT CHEECH OR CHONG`;
4. DAVE complete / ball save lit;
5. combo iranyjelzo (nem baked full effekt: a kovetkezo hid nyilai villogjanak);
6. SpaceCoke start;
7. Cheech/Chong 3-as gyujtemeny complete (kesobbi kapu elott is kell).

## 4. Hang-audit kovetkeztetesei

Szinte minden fizikai celpontnak van WAV Trigger hívasa. A fo hianyok nem a
talalati hangok, hanem az allapotvaltasokat magyarazo calloutok:

- `SHOOT UFO OR SPINNER`
- `SHOOT CHEECH OR CHONG`
- `BALL SAVE LIT`
- `MULTIBALL`
- `JACKPOT LIT` / `JACKPOT`
- `COMBO - SHOOT LEFT BRIDGE`
- `COMBO - SHOOT RIGHT BRIDGE`
- `HURRY UP` + 10/5 masodperces figyelmeztetes
- `GIFT - SHOOT THE FLASHING TARGET`
- Cheech/Chong collection complete

Kulon feladat a WAV Trigger SD-kartya track-manifestjenek elkeszitese:

- track ID;
- fajlnev;
- rovid tartalom/leirat;
- hossz;
- mono/stereo es hangerő;
- mely esemenyek hasznaljak.

Enelkul csak a kodban levo numerikus trackhivasok lete igazolhato.

## 5. Pontozasi audit

### A balance pass elott azonositott problemak

Az 1-6. pontot a 2026-08-24-i elso pontozasi profil megoldotta. A 7-8.
pont tovabbra is merest igenyel.

1. **A spinner normal modban 0 direkt pontot, de fordulatonkent +50 bonuszt adott.**
   Egy eros porgetes sok bonuszt termel, amit a x8 meg megszoroz, mikozben a
   jatekos a futas kozben 0 pontot lat.
2. **A sling normal modban ugyanez kisebb leptekkel:** 0 direkt pont, +50
   bonusz. A mechanikailag veletlenszeru esemeny bonuszban tul eros lehet.
3. **A DAVE es a gate rolloverek nem adtak direkt pontot.** A gate pontot
   kapott; a DAVE-et viszont szandekosan hagyjuk pont nelkul, mert lane/outlane
   mentomechanika, amelynek jutalma maga a ball save.
4. **A C&C completion nem adott direkt pontot**, csak +750 bonuszt, pedig fontos
   feature-t kvalifikal.
5. **A FISH+TANK par completionje nem adott kulon pontot.** A harmadik kor utan
   ugyan kigyullad a híd, de az elso ket par jutalma foleg video.
6. **A Hurry Up ad hoc ertekeket irt minden subsystembe.** Ez nehezen
   karbantarthato es nem alkot egyertelmu szabalyhierarchiat.
7. **A jackpotok hidankent eltero gorbeje nincs dokumentalva a jatekosnak.** A
   kis hid 10k..30k, a nagy hid 10k..20k; ez csak akkor jo, ha a kis hid
   bizonyithatoan nehezebb.
8. **Nincs pontozasi telemetria.** Egyetlen SIM-demo nem eleg az atlagos
   golyopont, feature-hozzajarulas es extra ball gyakorisag balanszahoz.

### Javasolt pontozasi hierarchia

Ez meg nem vegleges erteklista, hanem rendezesi elv. A konkret szamokat 20-30
rogzitett tesztparti utan kell lezarni.

| Eseményszint | Javasolt tartomany |
|---|---:|
| Veletlenszeru mikro-switch (pop, sling) | 100-500 |
| Rollover / spinnerfordulat | 100-500 |
| Celzott alaploves | 1000-3000 |
| Kisebb bank/par completion | 2500-7500 |
| Fo feature kvalifikalasa | 5000-10000 |
| Multiball inditasa | 10000-40000 |
| Jackpot | 15000-50000 |
| Nagy/korlatozott jackpot vagy teljes gyujtemeny | 30000-75000 |
| Minijatek / wizard-jellegu eredmeny | 0-100000, teljesitmeny szerint |

### Bevezetett elso balanszprofil

| Esemény | Regi ertek | Bevezetett ertek |
|---|---:|---:|
| Sling normal | 0 +50 bonus | 250 +25 bonus |
| Pop normal | 200 +10 | 250 +10 |
| Spinner inaktiv/aktiv, fordulat | 0 +50 | 250 +10 |
| DAVE betu | 0 | 0; szandekosan csak progressz |
| DAVE complete | csak 5 s save | 0 pont es legalabb 10 s save |
| Gate rollover | 0 | 500 +25 |
| C/N/C betu | 1500 +50 | Maradhat |
| C&C complete | 0 +750 | 5000 +500 |
| WEED betu | 1000 +50 | Maradhat |
| WEED complete | 1000 +500 | 5000 +500 |
| FISH/TANK cel | 1500 +10 | 1500 +50 |
| FISH+TANK par | 0 | 2500 +250 |
| Normal loop | 2000 +500 | 2500 +250 |
| Normal hid | 500 +50 | 1000 +100 |
| Combo 1..6 | 2.5k..20k +300 | Direkt pont maradt; bonus 100/150/200/250/300/500 |
| Multiball 1..4 start | 10k/20k/30k/40k | Maradhat elso tesztre |
| Loop jackpot | 30k +2000 | Maradhat, mert egyszeri |
| Collectible 1..3 | 10k/15k/20k | Maradhat |
| UFO pontjutalmak | 5k..40k | A WEED-stack tervezesnel ujraosztando |

### Bevezetett Hurry Up szabaly

A regi `minden subsystem sajat Hurry Up pontot kap` megoldast egyetlen,
jatekosnak elmondhato szabaly valtja:

- **Minden direkt pont 2x 80 masodpercig.** A szorzas a kozponti `Score()`
  fuggvenyben tortenik. A golyovegi bonusz nem duplazodik, es a mikro-
  switchek nem inditanak kulon pontvideot vagy ujra Hurry Up effektet.

A GUI-n a folyamatos `ALL SCORING 2X` HUD tovabbra is hianyzik. Egy kesobbi,
klasszikus csokkeno-erteku hurry-up redesign a WEED-stack utanra maradhat.

## 6. Jatekosi iranymutatas - javasolt rendszer

A teljes kepernyos klipek jo jutalom-visszajelzesek, de nem alkalmasak minden
aktiv cel folyamatos jelzesere. A SCORE kepernyore kell egy kicsi, nem blokkoló
statusz/HUD reteg.

### Firmware -> GUI allapottelemetria

Az elso implementalt, esemenyvezerelt allapotprotokoll:

```text
Party,<player>,<beer 0..3>,<joint 0..3>,<ufo-tier 0..4>,<weed-ready 0|1>
PartyEvent,<player>,<event-name>
```

A GUI ezt kozvetlenul dolgozza fel; nem kovetkeztet a videotriggerbol belso
firmware-allapotra. A tobbi subsystem telemetriaja kesobbi bovites.

### Palyafeny-szotar

Javasolt egységes jelentés:

- **feher, folyamatos:** mar begyujtott reszcel;
- **zold, villogo:** jelenleg ajanlott/qualified loves;
- **sarga/narancs, villogo:** mystery/gift vagy alternativ valasztas;
- **piros-sarga gyors valtakozas:** jackpot;
- **kek:** semleges/normal switch;
- **teljes palyas effekt:** csak feature-start, jackpot vagy veszely; ne minden
  2500 pontos Hurry Up switch inditsa ujra.

### Minimalisan megjelenitendo statuszok

- `WEED 2/4`
- `SHOOT UFO OR SPINNER`
- `GET HIGH 95/180`
- `SHOOT CHEECH OR CHONG`
- `BEER 2/3`
- `SHOOT LEFT BRIDGE`
- `COMBO: SHOOT RIGHT BRIDGE - 3.2 s`
- `BALL SAVE 7 s`
- `HURRY UP 42 s - ALL SCORING 2X`
- `MULTIBALL 2 - SHOOT JACKPOTS`

## 7. Javasolt munka-sorrend a WEED-stack elott

1. Asset- es track-manifest veglegesites.
2. Hianyzo multiball/jackpot/combo/tilt videok potlasa vagy tudatos dontes,
   hogy melyik marad csak HUD/overlay.
3. Pontozasi konstansok centralizalasa; ne maradjanak szetszort literalok a
   subsystemekben.
4. Spinner/sling/DAVE/gate/C&C/FISH pontozasi anomaliak elso koros javitasa.
5. Hurry Up pontozas centralizalasa.
6. Firmware allapottelemetria es GUI statusz/HUD.
7. Combo iranyfeny, C&C completion, DAVE completion es kulon híd-jackpot
   fenyeffekt.
8. SIM-regresszio bovítese minden subsystem es pontozasi ag lefedesere.
9. 20-30 tesztparti telemetria; atlagpont/golyo, feature-gyakorisag,
   pontforrasok megoszlasa.
10. Pontozas masodik koros balansza.
11. Csak ezutan a WEED-stack allapotgep.
