# CnC Pinball - gameplay TODO

A reszletes indoklas es az esemenyenkenti leltar a `GAMEPLAY_AUDIT.md` fajlban
van. Ez a lista a jelenlegi jatek rendbetetelehez es az utana kovetkezo
WEED-stack fejleszteshez tartozik. A tavoli collectibles-kapu/VUK bovites nincs
ebben a merfoldkoben.

## P0 - audit es forrasigazsag

- [ ] A balance pass utan a teljes SIM-demot ujra futtatni es az uj,
      szandekosan megvaltozott pontozasi golden baseline-t rogzitani.

- [ ] WAV Trigger SD-kartya teljes track-manifestje: ID, fajlnev, tartalom,
      hossz, hangerő, hasznalo esemenyek.
- [ ] A tenyleges videoasset-keszlet manifestje frame-szammal es hosszal.
- [ ] Minden video elso/kozepso/utolso kockajanak vizualis tartalomauditja.
- [ ] Dontes az arva triggerekről: `Point1`, `Point3..8`, `Jackpot1`.
- [ ] Az UFO regi, elcsuszott neveinek kesobbi migracios terve
      (`Ufo6`/`Ufo7` aliasok megorzese a kompatibilitashoz).

## P0 - hianyzo visszajelzesek

- [x] Dedikalt fenyeffekt a sima, aktivalt kis- es nagyhid talalatara:
      ID31 a kis hidra, ID32 a nagy hidra. Az ID5 Weedblast tovabbra is
      kizarolag a WEED kigyujtes esemenye.

- [x] Tilt firmware: ket warning utan a harmadik kulon kontaktus Tilt.
- [x] Tilt firmware: uj warning csak 750 ms stabilan nyitott kontaktus utan.
- [x] Tilt firmware: warningok golyonkent nullazodnak.
- [x] Tilt firmware: Tilt eseten nincs golyovegi bonusz.
- [ ] Multiball1 video: `Acapulco Gold` (2 golyo).
- [ ] Multiball2 video: `Michokan` (3 golyo).
- [ ] Multiball3 video: `Thai Stick`.
- [ ] Multiball4 video: `Labrador`.
- [x] Pontos osszegu jackpot-videok: 10000/15000/20000/25000/30000/50000/100000.
      A firmware `Jackpot_<jovairt pont>` sort kuld a hidakon es a loopon;
      a GUI a 125 frame-es, 30 fps-es sequence-et egyszer jatssza le.
- [x] Jackpot-emeles a Hurry Up szorzo utan: 40000 helyett 50000,
      60000 helyett 100000 pont es egyezo video. A bonusz valtozatlan;
      mas pontforrasokra nem ervenyes az emeles.
- [x] Jackpot-feny: ID19, ket teljes 2 s-os kor (osszesen kb. 4 s),
      a ket hid es a loop kozos video/hang/feny inditasaval. Jackpotnal
      a regi ID1/ID5 effekt nem irja felul; uj jackpot ujrainditja a ket kort.
- [x] Cheech es Chong hid-combo videok: karakterenkent 6 sequence,
      2500/5000/7500/10000/15000/20000 ponttal (2026-08-24).
- [x] Oldalspecifikus combo trigger: kishid = Chong, nagyhid = Cheech.
- [x] `Ufo8` bekerult a normal UFO-lotterybe mint `EXTRA BALL LIT`.
- [x] A nagy hid/high ramp begyujti a lit extra ballt es `ExtraB` triggert
      kuld; az azonnali `Ufo5` extra ball tovabbra is megmaradt.
- [x] Az `Ufo5` es `Ufo8` lottery-kimenet tiltott, ha mar van eltett vagy
      kigyujtesre varo extra ball.
- [x] Dedikalt `EXTRA BALL LIT` fenyjelzes a high rampon (ID36), majd
      tartos pink-arany high-ramp jelzes a collectig.
- [ ] `ExtraB` collect videoasset elkeszitese.
- [x] Extra ball collect light show (ID37) es dedikalt callout.
- [ ] A WEED-stack bevezetesekor az azonnali extra ball es az extra-ball-lit
      lottery-sulyanak/retegenek meghatarozasa.
- [x] Uj `Danger` video jelen van: 102 frame (2026-08-24).
- [x] Az elso warning a `Danger` videot kapja.
- [x] Uj `Danger2` video jelen van: 180 frame; a masodik warning ezt kapja
      (2026-08-25).
- [x] Mindket Danger warning az ID11 Tilt-fenyeffektet harom teljes korig
      jatsza.
- [x] Tilt video: 180 frame, 30 fps; a 0-59 intro egyszer fut le, majd a
      60-179 szakaszt a GUI a drain `Next`/`End` uzeneteig loopolja.
- [x] Tilt alatt az ID11 Tilt-fenyeffekt a drain erzekeleseig loopol.
- [ ] C&C complete: feny + hang + `SHOOT CHEECH OR CHONG` GUI-uzenet.
- [ ] WEED complete: `SHOOT UFO OR SPINNER` GUI-uzenet.
- [ ] DAVE complete: `BALL SAVE LIT` feny/hang/GUI-uzenet.
- [x] Combo alatt az ellenkezo híd teljes celzonaja cyan-ibolya ritmusban
      jelez az 5,5 masodperces ablakban.
- [x] Dedikalt Chong combo-fenyeffekt (ID9) a kishid sikeres kombotalalatara.
- [x] Dedikalt Cheech combo-fenyeffekt (ID10) a nagyhid sikeres kombotalalatara.
- [ ] Light Editorban canvasra allitani es ujraexportalni az ID7 ChongCollect,
      ID8 CheechCollect es ID9 ComboLowBridge effekteket, ha ezeknek is a
      jatekfenyre kell rajzolniuk. Jelenleg csak az ID10 ComboHighBridge canvas.
- [ ] Hurry Up folyamatos GUI-statusz es visszaszamlalas.
- [ ] Spinner progressz a GUI-n is latszodjon.
- [ ] Multiball alatt `SHOOT JACKPOTS` statusz.

## P0 - Cheech/Chong hangcsomag (OrigySD 2026)

- [x] Cheech BALL_LAUNCH: 206-208, veletlen A/B/C a 2./3. normal golyonal;
      az elso golyonal megmarad a klasszikus 017-es "yeah man".
- [x] Cheech BALL_SAVE: 215-217, csak tenyleges egygolyos mentesnel;
      elso kilovesnel es multiball-potlasnal nem szol.
- [x] Chong EXTRA_BALL_AWARDED: 218-220 a high-ramp collectnel; a kovetkezo
      extra golyo inditasakor fixen 218.
- [x] Chong BEER_COLLECTED: 230-232 a teljes FISH+TANK par utan.
- [x] Chong JOINT_ROLLED_1: 239-241 az elso joint sodrasakor.
- [x] Jackpot hangok: ertekenkent 50/50 Cheech vagy Chong, egy bemondas
      talalatonkent; a regi 073 nem szol ra az uj dumara. Trackparok:
      10000=209/210, 15000=211/212, 20000=213/214, 25000=221/222,
      30000=299/300, 50000=305/306, 100000=307/308. A video, hang es
      pontozas ugyanazt az emelt jackpot-osszeget hasznalja.
- [ ] Jackpot audio/video gepi ellenorzes: a WAV-ok 3,16-6,13 masodpercesek,
      a video 4,17 s. A hosszabb hangok a pontkepernyore visszaterve is futhatnak;
      a bekotes nem vagja vagy gyorsitja oket.
- [x] Cheech MULTIBALL_ACAPULCO: 293-295, az elso/ketgolyos multiball.
- [x] Chong MULTIBALL_MICHOACAN: 296-298, a masodik/haromgolyos multiball.
- [x] Chong MULTIBALL_LABRADOR: 302-304, a negyedik/otgolyos multiball.
- [x] Combo-beszed: az SD-n nem letezo 095/096 helyett a befejezo hid
      meglevo 009/036 reakcioja szol.
- [x] MULTIBALL3 / Thai Stick: 311-313, a harmadik/negygolyos multiball.

## P0 - pontozas rendbetetele

- [x] A pont- es bonuszertekek nevesitett `Scoring` profilban vannak.
- [x] Spinner: 250 direkt/+10 bonus.
- [x] Sling: 250/+25.
- [x] Pop bumper: 250/+10.
- [x] DAVE lane es completion: nincs pont; a feature egyetlen jutalma a
      legalabb 10 s ball save.
- [x] Minden UFO/VUK-kidobas a tekercs indulasakor legalabb 5 s vedelmi
      ball save-et kap; SpaceCoke multiballnal marad a 30 s.
- [x] Gate rollover: 500/+25.
- [x] C&C complete: 5000/+500.
- [x] WEED complete: 5000/+500.
- [x] FISH/TANK cel: 1500/+50.
- [x] FISH+TANK par: 2500/+250.
- [x] Normal híd: 1000/+100.
- [x] Normal loop: 2500/+250.
- [x] Combo bonusgorbe: 100/150/200/250/300/500.
- [ ] Kis/nagy híd jackpot-arany fizikai nehezseg alapjan.
- [ ] Bonus maximum vagy elvart golyovegi bonusztartomany meghatarozasa.
- [x] Hurry Up kozponti szabaly: direkt pont 2x, a bonusz valtozatlan,
      mikro-switch video- es effekt-ujrainditas nelkul. Jackpot-kivetel:
      a szorzas utan 40000 -> 50000, 60000 -> 100000.

## P1 - GUI allapot/HUD

- [x] Firmware->GUI `Party` allapotprotokoll: jatekos, beer, joint,
      UFO-tier es WEED qualification.
- [ ] `WEED n/4`.
- [ ] `GET HIGH n/180` vagy szazalek.
- [x] UFO-tier kijelzes: Cash Out / Super Cash Out / Feature Wheel / Love Pack.
- [x] `BEER n/3` es `JOINTS n/3` tartos, jatekosonkenti HUD.
- [ ] `SHOOT CHEECH OR CHONG`.
- [ ] `COMBO: SHOOT LEFT/RIGHT - n.n s`.
- [ ] `BALL SAVE n s`.
- [ ] `HURRY UP n s` es az aktiv pontszabaly.
- [ ] `MULTIBALL n - SHOOT JACKPOTS`.
- [ ] A HUD prioritasi szabalyai: veszely > jackpot > idozitett mod > fo cel >
      gyujtesi progressz.

## P1 - fenyrendszer

- [ ] Egységes palyafeny-szotar dokumentalasa es alkalmazasa.
- [ ] Multiball-start feny.
- [x] Dedikalt híd-jackpot feny: ID19 teljes palyas effekt, 2 kor;
      jackpotnal mar nem a Weedblast indul.
- [x] SpaceCoke start feny: ID14 sajat SpaceCoke MB show.
- [x] Cheech/Chong harmadik collectible completion feny: Chong ID38,
      Cheech ID39.
- [ ] Effect ID1 overlay/full inditas ellenorzese es javitasa, ha szukseges.
- [x] Hurry Up talalat nem inditja ujra minden mikro-switchnel a teljes palyas
      effektet.
- [x] Hurry Up proceduralis korfutasa: CnC -> Chong -> WEED -> Cheech ->
      Fishtank -> jobb sling -> ballsave -> bal sling; a talalt zona feher-ciankek
      felvillanast es 1,4 s-os lecsengest kap (2026-08-28).
- [x] Hurry Up ID6 megtartva legfelso, mod-vegeig loopolo retegkent; a jelenlegi
      fekete hatteret es a kesobbi magenta sentinelt is transzparensen kezeli,
      igy az alatta futo post/chase/falloff fenyek elnek (2026-08-28).

## P1 - szabalykommunikacio

- [ ] Gift: sajat `GIFT - SHOOT FLASHING TARGET` callout/HUD.
- [ ] Gift ne tunjon el csendben egy rossz celponttol; legyen tartos vagy
      idozitett, es a kudarc legyen egyertelmu.
- [ ] Cheech/Chong harmadik collectible utan ne legyen nema azonnali reset;
      legalabb completion celebration es allapotjelzes kell.
- [ ] Multiball end rovid HUD-uzenet.
- [ ] Golyovegi bonus-count latvany/hang tervezese (nem blokkoló).

## P1 - teszt es telemetria

- [ ] SIM-demo bovítese C&C, FISH/TANK, DAVE, gate/Bonus X, combo, Hurry Up,
      mind az ot multiball es minden UFO-jutalom lefedesere.
- [ ] LED-esemenyek tesztelheto trace-e SIM_MODE-ban, hogy a regresszio a
      fenyinditasokat is lassa.
- [ ] Pontforras-telemetria: esemenyazonosito, jatekos, direkt pont, bonus.
- [ ] 20-30 teljes tesztparti rogzítese.
- [ ] Meres: atlagpont/golyo, median, feature-gyakorisag, extra ball gyakorisag,
      pontforrasok szazalekos megoszlasa.
- [ ] A meres alapjan masodik balanszkor.
- [ ] Tilt-harang gepi teszt: ket kulon kontaktus Danger, harmadik Tilt.
- [ ] Egyetlen hosszan kilengo harang ne adjon tobb warningot.
- [ ] Tilt utan a golyovegi bonusz ne adodjon hozza a score-hoz.

## P2 - ROLL A JOINT / LOVE PACK merfoldko

- [x] A kishid a bankolo loves; csak aktiv WEED + betarazott beer mellett
      veszi at a normal combo/jackpot szerepet.
- [x] Jatekosonkenti `jointStack` (0..3), `beerCredits` (0..3) es
      `weedQualified` allapot, golyok kozotti megmaradassal.
- [x] FISH+TANK egy betarazott beert ad; a harmadik beer mar nem nullaz es
      nem elesiti kozvetlenul a kishid-jackpotot.
- [x] Sima UFO: pont / multiplayer pontlopas / Extra Ball Lit.
- [x] Joint 1 UFO Super Cash Out: 30000 pont / multiplayer 20000 pontlopas /
      Extra Ball Lit.
- [x] Joint 2 UFO: Extra Ball / Hurry Up / Munchies Feature Wheel.
- [x] Joint 3: LOVE PACK; az UFO garantalt SpaceCoke multiballt indit.
- [x] Az UFO-cashout elfogyasztja a teljes joint stacket; a ROLL A JOINT
      jointonkent egy beer-kreditet fogyaszt.
- [x] Tierenkenti UFO-, beer-, spinner- es kishid-fenyszinek.
- [x] GUI Party HUD, iranymutato uzenetek es Love Pack placeholder trigger.
- [x] SIM-demo bovítese a teljes Beer -> Joint -> Love Pack -> SpaceCoke lancra.
- [ ] Dedikalt Joint 1 / Joint 2 videok.
- [x] Feature Wheel szerencsekerek-animacio: kulon Extra Ball / Hurry Up /
      Munchies sequence, firmware-sorsolassal es sessionos `WHEEL_DONE`
      visszajelzessel (2026-08-27).
- [x] A Feature Wheel vizualis vilagahoz illo 5 mp-es Extra Ball es Hurry Up
      eredmenyvideo; a regi `Ufo5`/`Ufo6` itt szandekosan nincs lejatszva
      (2026-08-27).
- [ ] LovePack PNG sequence: `LOVE PACK! SHOOT THE UFO FOR COKE!`.
- [x] Dedikalt Roll a Joint / Love Pack / SpaceCoke baked fenyek: ID12,
      ID13 es ID14, a megfelelo eventekre kotve.
- [ ] Gepen teszt: minden cashout-sorrend, drain, multiplayer jatekosvaltas.
