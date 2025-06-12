// Game:      Laser Maze v2.0
// Copyright: 2025 Frank van de Ven
// Licence:   MIT

 #include <Arduboy2.h>                         // installeer de Arduboy2 bibliotheek
 #include "GameData.h"
 Arduboy2 arduboy;                             // maakt een Arduboy object aan

// variabelen
 const byte level_max = 70;                    // max aantal levels in game data
 const int adresEEPROM = 0x1FB;                // adres in EEPROM

 byte x, y, z;                                 // teller / tijdelijk geheugen
 byte nr0, nr1, nr2, nr3, nr4, nr5;            // tijdelijk geheugen stack
 int  t;                                       // tijdelijk geheugen
 byte spel[5][6];                              // opslag speelveld (volgorde y-as,x-as)
 byte bezet[5][6];                             // 0 = geen pion, 1 = pion niet verplaatsbaar
 byte einde2[5][5];                            // lijnen teller (voorkomt een laser loep)
 int stack[10][4];                             // stack waarop alle te tekenen lasers komen
 byte stack_nr = 0;                            // stackpointer (houdt aantal laserstralen bij
 byte level = 0;                               // level nummer
 byte level2;                                  // tijdelijk opslag level nr uit EEPROM
 bool knipper = 0;                             // knipperende pion (1 = aan, 0 = uit)
 bool laser = 0;                               // 1 = laat laserstraal zien. 0 = geen laser
 byte lijn;                                    // laser 1= rechte lijn 2= gestippelde lijn
 int Sx, Sy;                                   // x, y positie geselecteerde pion
 int Cx, Cy;                                   // x, y positie cursor
 int Lx, Ly;                                   // x, y positie laser pion
 int LSx, LSy;                                 // x, y positie laserstraal
 int Rx = 0, Ry = 0;                           // x, y richting laserstraal (-1,0,1)
 int Lr;                                       // richting laserstraal 0 t/m 3
 int zoek;                                     // locatie in opzoektabel waar laserstraal is
 byte einde = 0;                               // bepaald of alle stralen getekend zijn
 byte doel;                                    // teller aantal doelen
 byte doel2 = 1;                               // teller aantal doelen,bij 0 is alles geraakt
  
//===========================================================================================

void setup() {
 arduboy.begin();                              // initialiseert de Arduboy2 bibliotheek
 arduboy.setFrameRate(40);                     // FrameRate: 60 beelden per sec.
 startscherm();                                // laat startscherm + teksten zien
 new_game();                                   // new game scherm + laad level uit EEPROM
 arduboy.initRandomSeed();                     // voor random getallen
 load_level(level);                            // laad level data in RAM
}
//===========================================================================================

void loop() {
 if (!(arduboy.nextFrame())) {return;}         // wacht tot het tijd is voor volgende frame
 ESP.wdtFeed();
 if (arduboy.everyXFrames(8)){knipper = !knipper;}  // zet om 8 frames knipper aan / uit
 arduboy.pollButtons();                        // controle of er een knop wordt gedrukt
 arduboy.clear();                              // wis het video geheugen
 print_teksen();                               // zet teksten op LCD
 knoppen();                                    // lees en verwerk knoppen
 print_speelveld();                            // zet pionnen uit RAM in speelveld + voorraad
 if (laser == 0 && level < level_max){         // bij 0 geen laserstraal tekenen
  print_cursor();                              // zet cursor op LCD
 }
 else{                                         // teken laserstraal
  knipper =  0;                                // zet knipperen uit
  print_speelveld();                           // zet pionnen uit RAM in speelveld
  if (level < level_max) {
  teken_laser();                               // teken laserstraal
  teken_icon();                                // teken duim of kruis als doelen zijn geraakt
  }
 }
 arduboy.display();                            // zet de inhoud van het video geheugen op LCD
}
//===========================================================================================


// laat startscherm en tekst zien + druk op een knop is nodig om random getallen te krijgen
void startscherm(){
 arduboy.clear();                            // wis het video geheugen
 arduboy.drawCompressed(0,0, plaatje,WHITE); // teken gecomprimeerd plaatje
 arduboy.setCursor(0,56);                    // positioneer tekst cursor
 arduboy.print("A:");                        // print tekst
 arduboy.setCursor(13,56);                   // positioneer tekst cursor
 arduboy.print("Play game");                 // print tekst
 arduboy.setCursor(90,56);                   // positioneer tekst cursor
 arduboy.print("B:");                        // print tekst
 arduboy.setCursor(103,56);                  // positioneer tekst cursor
 arduboy.print("Code");                      // print tekst
 arduboy.display();                          // zet de inhoud van het video geheugen op LCD
  while(arduboy.pressed(A_BUTTON) == false){ // wacht zolang er NIET op A wordt gedrukt?
   ESP.wdtFeed();
   if (arduboy.pressed(B_BUTTON) == true) {  // wordt er op B gedrukt?
    ESP.wdtFeed();
    arduboy.fillRect (0,56,127,8, BLACK);    // wis oude tekstblok
    arduboy.setCursor(0,56);                 // positioneer tekst cursor
    arduboy.print("Code:");                  // print tekst
    arduboy.setCursor(32,56);                // positioneer tekst cursor
    arduboy.print("Frank van de Ven");       // print tekst
    arduboy.display();                       // zet de inhoud van het video geheugen op LCD
   }
  }
}
//-------------------------------------------------------------------------------------------

// Laat new game scherm zien + laad level uit EEPROM of start bij level 1
void new_game() {
 level = 100;                                 // laad level met niet bestaand level nummer
 if ((EEPROM_lees(adresEEPROM) == 0x46) && (EEPROM_lees(adresEEPROM+1) == 0x56)) {
                                              // kijk of de letters "F" "V" in EEPROM staan.
  level2 = EEPROM_lees(adresEEPROM+2);}       // ja: haal laatste level uit EEPROM
 else {                                       // nee:
  EEPROM_schrijf(adresEEPROM,0x46);           // zet "F" in EEPROM
  EEPROM_schrijf(adresEEPROM+1,0x56);         // zet "V" in EEPROM
  EEPROM_schrijf(adresEEPROM+2,0x00);         // zet level "0" in EEPROM
  level = 0;                                  // geen game data. start level 1 direct
 }
 if (level2 == 0){level = 0;}                 // als EEPROM level = 1: sla scherm EEPROM over
 if (level == 100) {                          // laat dit scherm zien bij geen EEPROM data
  arduboy.fillRect (0,56,127,8, BLACK);       // wis oude tekstblok
  arduboy.setCursor(0,56);                    // positioneer tekst cursor
  arduboy.print("A:");                        // print tekst
  arduboy.setCursor(13,56);                   // positioneer tekst cursor
  arduboy.print("Continue");                  // print tekst
  arduboy.setCursor(70,56);                   // positioneer tekst cursor
  arduboy.print("B:");                        // print tekst
  arduboy.setCursor(83,56);                   // positioneer tekst cursor
  arduboy.print("New");                       // print tekst
  arduboy.setCursor(105,56);                  // positioneer tekst cursor
  arduboy.print("game");                      // print tekst
  arduboy.display();                          // zet de inhoud van het video geheugen op LCD
  while(arduboy.pressed(A_BUTTON) == true) {ESP.wdtFeed();} // wacht tot A knop wordt losgelaten.
  while(arduboy.pressed(B_BUTTON) == true) {ESP.wdtFeed();} // wacht tot B knop wordt losgelaten.
  while (level == 100) {                      // wacht op input A of B knop
   ESP.wdtFeed(); 
   if (arduboy.pressed(A_BUTTON) == true) {   // wordt er op A gedrukt?
    level = level2;                           // laad level met waarde uit EEPROM
  }
   if (arduboy.pressed(B_BUTTON) == true) {   // wordt er op B gedrukt?
    level = 0;                                // laad level met "level 1"
    EEPROM_schrijf(adresEEPROM+2,0x00);       // zet level "0" in EEPROM
   }
  }
  while(arduboy.pressed(A_BUTTON) == true) {ESP.wdtFeed();} // wacht tot A knop wordt losgelaten.
  while(arduboy.pressed(B_BUTTON) == true) {ESP.wdtFeed();} // wacht tot B knop wordt losgelaten.
 }
}
//-------------------------------------------------------------------------------------------

// teken laserstraal
void teken_laser(){
 lijn = 1;                                   // stel laserstraal in op rechte lijn
 for (x = 0; x < 5; x++){                    // loep loopt horizontaal door de posities
  for (y = 0; y < 5; y++){                   // loep loopt verticaal door de posities
    einde2[y][x] = 0;                        // wis de lijnenteller
    if (bezet[y][x] == 3){                   // is de vorige keer een doel geraakt?
     bezet[y][x] = 2;}                       // ja, wis de doel teller 
  }
 }
 doel2 = doel;                               // reset de doel teller
 stack_nr = 1;                               // reset stack nr. (1 is 1e plaats. 0 is klaar)
 stack[stack_nr][0] = Lx;                    // zet x startpositie laser op stack
 stack[stack_nr][1] = Ly;                    // zet y startpositie laser op stack
 stack[stack_nr][2] = spel[Ly][Lx] - 18;     // zet richting laserstraal op stack
 stack[stack_nr][3] = lijn;                  // zet soort laserstaal (recht/ punten) op stack
 while (stack_nr > 0) {                      // teken laser stralen tot stack leeg is
  einde = 0;                                 // reset einde laserstraal marker
  LSx = stack[stack_nr][0];                  // haal startpositie x laserstraal van stack
  LSy = stack[stack_nr][1];                  // haal startpositie y laserstraal van stack
  Lr = stack[stack_nr][2];                   // haal richting laserstraal van stack
  lijn = stack[stack_nr][3];                 // haal soort laserstraal van stack
  stack_nr--;                                // verlaag stackpointer met 1
  do {
    if (Lr == 0) {Rx = 0;  Ry = -1;}          // stel richting laser vast (= richting 0)
    if (Lr == 1) {Rx = 1;  Ry = 0;}           // stel richting laser vast (= richting 1)
    if (Lr == 2) {Rx = 0;  Ry = 1;}           // stel richting laser vast (= richting 2)
    if (Lr == 3) {Rx = -1; Ry = 0;}           // stel richting laser vast (= richting 3)
   LSx = LSx + Rx;                            // bereken nieuwe x positie
   LSy = LSy + Ry;                            // bereken nieuwe y positie
    if (LSx < 0 || LSx > 4) {einde = 1;}      // laserstraal buiten speelveld is einde
    if (LSy < 0 || LSy > 4) {einde = 1;}      // laserstraal buiten speelveld is einde
   if (einde == 0) {
    zoek = spel[LSy][LSx] * 4 + Lr;            // locatie laserstraal in opzoektabel
    nr0 = pgm_read_byte (&tabel[zoek][0]);
    nr1 = pgm_read_byte (&tabel[zoek][1]);
    nr2 = pgm_read_byte (&tabel[zoek][2]);
    nr3 = pgm_read_byte (&tabel[zoek][3]);
    nr4 = pgm_read_byte (&tabel[zoek][4]);
    nr5 = pgm_read_byte (&tabel[zoek][5]);
    if (nr1 > 0) {                            // moet er een horizontale lijn getekend worden
     for (z = 0; z < nr1; z = z + lijn)        // loep om lijn van pixels te tekenen
      {arduboy.drawPixel((LSx*12+47+nr0+z),(LSy*12+7));}  // teken een lijn van pixels
     einde2[LSy][LSx]++;                      // verhoog lijnen teller met 1
    }
    if (nr3 > 0) {                            // moet er een verticale lijn getekend worden?
     for (int z = 0; z < nr3; z = z + lijn)      // loep om lijn van pixels te tekenen
      {arduboy.drawPixel((LSx*12+53),(LSy*12+1+nr2+z));}  // teken een lijn van pixels
     einde2[LSy][LSx]++;                      // verhoog lijnen teller met 1
    }
    if (nr5 == 5 && lijn == 1) {              // moet er een ingekleurd doel komen?
     Sprites::drawSelfMasked((LSx*12+49),(LSy*12+3), pion, spel[LSy][LSx]+13); // print doel
      if (bezet[LSy][LSx] == 2) {             // kijk of dit doel nog niet geraakt is
       doel2--;                               // verlaag doel teller
       bezet[LSy][LSx] = 3;                   // noteer dat dit doe geraakt is
      }
    }
    if (nr5 == 6 && lijn == 2) {              // moet er een ingekleurd doel komen?
     Sprites::drawSelfMasked((LSx*12+49),(LSy*12+3), pion, spel[LSy][LSx]+13); // print doel
      if (bezet[LSy][LSx] == 2) {             // kijk of dit doel nog niet geraakt is
       doel2--;                               // verlaag doel teller
       bezet[LSy][LSx] = 3;                   // noteer dat dit doe geraakt is
      }
    }
   if (nr4 < 4){Lr = nr4;}                    // pas richting aan
   if (nr4 == 4) {einde = 1;}                 // indien obstakel einde laserstraal
   if (nr5 < 4) {                             // is er een halfdoorlatende spiegel?
    stack_nr++;                               // verhoog stack pointer met 1
    stack[stack_nr][0] = LSx;                 // zet x positie laser op stack
    stack[stack_nr][1] = LSy;                 // zet y positie laser op stack
    stack[stack_nr][2] = nr5;                 // zet richting laserstraal op stack
    stack[stack_nr][3] = lijn;                // set soort laserstraal op stack
     if(spel[LSy][LSx] > 6 && spel[LSy][LSx] < 9) { // staat hier een prisma?
      stack[stack_nr][3] = 2;}                // sla gestippelde laserstraal op
   }
    if(einde2[LSy][LSx] > 10) {einde = 1;}    // zit de laserstraal in een loep?
  }
  }
   while (einde == 0);                         // is einde laserstraal bereikt?
  }
}
//-------------------------------------------------------------------------------------------

// controle of er op een knop is gedrukt en verwerk deze.
void knoppen(){
 if (arduboy.justPressed(B_BUTTON)){           // is knop B ingedrukt
  laser = !laser;                              // teken laserstraal aan (1) / uit (0) zetten
   if (doel2 == 0 && laser == 0){              // heb je alle doelen geraakt?
    level++;                                   // volgende level
    if (level == level_max + 1) {              // is het laatste level gespeeld?
     level = 0;                                // begin weer bij level 1
    }
    EEPROM_schrijf(adresEEPROM+2,level);       // zet nieuwe level in EEPROM
    load_level(level);                         // laad volgende level
   }
 }
 if (laser == 0){                              // staat laser uit (0) dan scan andere knoppen
  if (arduboy.justPressed(UP_BUTTON)){         // is knop omhoog ingedrukt
   t = Cy;                                     // sla y positie cursor tijdelijk op
   do {t--;}                                   // verlaag y positie met 1
   while (bezet[t][Cx] > 1);                   // blijf verlagen als er een vaste pion staat
   if (t >= 0) {Cy = t;}                       // indien niet buiten speelveld sla y pos op
  }
  if (arduboy.justPressed(DOWN_BUTTON)){       // is knop omlaag ingedrukt
   t = Cy;                                     // sla y positie cursor tijdelijk op
   do {t++;}                                   // verhoog y positie met 1
   while (bezet[t][Cx] > 1);                   // blijf verhogen als er een vaste pion staat
   if (t < 5) {Cy = t;}                        // indien niet buiten speelveld sla y pos op
  }
  if (arduboy.justPressed(LEFT_BUTTON)){       // is knop naar links ingedrukt
   t = Cx;                                     // sla x positie cursor tijdelijk op
   do {t--;}                                   // verlaag x positie met 1
   while (bezet[Cy][t] > 1);                   // blijf verlagen als er een vaste pion staat
   if (t >= 0) {Cx = t;}                       // indien niet buiten speelveld sla x pos op
  }
  if (arduboy.justPressed(RIGHT_BUTTON)){      // is knop naar rechts ingedrukt
   t = Cx;                                     // sla x posite cursor tijdelijk op
   do {t++;}                                   // verhoog x positie met 1
   while (bezet[Cy][t] > 1);                   // blijf verhogen als er een vaste pion staat
   if (t < 6) {Cx = t;}                        // indien niet buiten speelveld sla x pos op
  }
  if (arduboy.justPressed(A_BUTTON)){          // is knop A ingedrukt
  if ((Cx < 5) && (spel[Cy][Cx] == 0) && (bezet[Sy][Sx] == 0)){ // verplaatsbare pion?
    spel[Cy][Cx] = spel[Sy][Sx];               // verplaats pion van selectie naar cursor pos
    spel[Sy][Sx] = 0;                          // wis pion op selectie plaats
    Sy = Cy; Sx = Cx;                          // maak cursor pos selectie pos
   return;                                     // einde afhandeling knop A
   }
   if (spel[Cy][Cx] > 0){                      // kijk of er onder de cursor iets staat
    if ((Sy == Cy)&&(Sx == Cx)){               // kijk of het een geselecteerde pion is
     z = spel[Cy][Cx];                         // haal pion nummer op cursor positie op
     if (Cx < 5 && z > 0 && z < 5){            // spiegel in speelveld?
      z++;                                     // volgende spiegel
      if (z > 4) {z = 1;}                      // indien laatste spiegel. neem de 1e spiegel
      }
     if (Cx < 5 && z > 4 && z < 7){            // halfdoorlatende spiegel in speelveld?
      z++;                                     // volgende spiegel
      if (z > 6) {z = 5;}                      // indien laatste spiegel. neem de 1e spiegel
     }
     if (Cx < 5 && z > 6 && z < 9){            // prisma in speelveld?
      z++;                                     // volgende prisma
      if (z > 8) {z = 7;}                      // indien laatste prisma. neem de 1e prisma
     }
    spel[Cy][Cx] = z;                          // sla nieuwe spiegel op
    return;                                    // einde afhandeling knop A
    }
    Sy = Cy; Sx = Cx;                          // bij geen spiegel selecteer pion onder cursor
   }
  }
 }
}
//-------------------------------------------------------------------------------------------

// laad nieuw level in RAM
void load_level(byte level3){
 doel = 0;                                     // reset doel teller
 for (x = 0; x < 6; x++) {                     // loep horizontaal door speelveld
  for (y = 0; y < 5; y++) {                    // loep verticaal door speelveld
  spel[y][x] = pgm_read_byte (&levels[level3][y][x]);  // zet level data in speelveld
    if (spel[y][x] == 1) {                     // staat hier een spiegel?
     z = random(0,4);                          // kies een getal tussen 0 en 3
     spel[y][x] = spel[y][x] + z;              // pas de spiegel aan zodat die random is
    }
    if (spel[y][x] == 5) {                     // staat hier een halfdoorlatende spiegel?
     z = random(0,2);                          // kies een getal tussen 0 en 1
     spel[y][x] = spel[y][x] + z;              // pas de spiegel aan zodat die random is
    }
    if (spel[y][x] == 7) {                     // staat hier een prisma?
     z = random(0,2);                          // kies een getal tussen 0 en 1
     spel[y][x] = spel[y][x] + z;              // pas de spiegel aan zodat die random is
    }
   bezet[y][x] = 0;                            // 0 = vrij (selecteerbare plaats)
   if (x < 5 && spel[y][x] > 0 && spel[y][x] < 9) { // staat hier een spiegel?
    bezet[y][x] = 1;}                          // 1 = hier staat een spiegel
   if (x < 5 && spel[y][x] > 8 && spel[y][x] < 17) {// staat hier een doel?
    bezet[y][x] = 2;                           // 2 (of 3) hier staat een doel
    doel++;}                                   // verhoog doel teller met 1
   if (x < 5 && spel[y][x] > 16) {             // staat hier een laser of hindernis?
    bezet[y][x] = 4;}                          // 4 = hier staat een hindernis of laser
   if ((spel[y][x] > 17) && (spel[y][x] < 22)){// is dit de laser?
    Lx = x; Ly = y;}                           // sla laser positie op
 } }
 Sx = 5; Sy = 0;                               // x, y positie geselecteerde pion goed zetten
 Cx = 5; Cy = 0;                               // x, y positie cursor goed zetten
}
//-------------------------------------------------------------------------------------------

// teken 2 vierkanten (speelveld en voorraad bak) en zet alle pionnen uit RAM hier in
void print_speelveld(){
 arduboy.drawRect(46,0,63,63,WHITE);           // teken het vierkant van het speelveld
 arduboy.drawRect(112,0,15,63,WHITE);          // teken het vierkant van de voorraad bak
 for (x = 0; x < 6; x++) {                     // loep horizontaal door speelveld
  for (y = 0; y < 5; y++) {                    // loep verticaal door speelveld
   if (x == 5) {z = 55;} else {z = 49;}        // correctie x-as, zet pion in voorraad bak

if (!((Sx == x) && (Sy == y) && (knipper == 1))){ // laat de geselecteerde pion knipperen
   Sprites::drawSelfMasked((x*12 + z),(3+y*12), pion, spel[y][x]);} // zet pion op LCD
 } }
}
//-------------------------------------------------------------------------------------------

// zet de cursor op LCD
void print_cursor(){
 if (Cx == 5) {z = 53;} else {z = 47;}        // correctie x-as, zet cursor in voorraad bak
 Sprites::drawSelfMasked((Cx*12 + z),(1+Cy*12), cursors, 0);
}
//-------------------------------------------------------------------------------------------

// teken een duim (alle doelen geraakt) of een kruis (niet alle doelen zijn geraakt)
void teken_icon() {
 if (doel2 == 0){                              // heb je alle doelen geraakt?
  Sprites::drawSelfMasked(13,40, icon, 0);     // ja: print duim
 }
 else {
  Sprites::drawSelfMasked(13,40, icon, 1);     // nee: print kruis 
 }
}
//-------------------------------------------------------------------------------------------

// print teksten op LCD
void print_teksen(){
 arduboy.setTextSize(1);                       // font instellen
 arduboy.setCursor(3,2);                       // positioneer tekst cursor
 arduboy.print("Laser");                       // print tekst
 arduboy.setCursor(18,12);                     // positioneer tekst cursor
 arduboy.print("Maze");                        // print tekst
 if (level == level_max) {
  arduboy.setCursor(52,16);                    // positioneer tekst cursor
  arduboy.print("Well done");                  // print tekst
  arduboy.setCursor(52,36);                    // positioneer tekst cursor
  arduboy.print("Game over");                  // print tekst
  EEPROM_schrijf(adresEEPROM+2,0x00);          // zet nieuwe level in EEPROM op 1
 }
 else {
  arduboy.setCursor(0,26);                     // positioneer tekst cursor
  arduboy.print("Level");                      // print tekst
  arduboy.setCursor(32,26);                    // positioneer tekst cursor
  arduboy.print(level+1);                      // print level nummer
 }
}
//-------------------------------------------------------------------------------------------

// Routine schrijft alleen naar EEPROM als data is aangepast
void EEPROM_schrijf (int adres_eeprom, byte data_eeprom) {
 if (EEPROM.read(adres_eeprom) != data_eeprom){
    EEPROM.write(adres_eeprom, data_eeprom);
    EEPROM.commit();
 }
}
//-------------------------------------------------------------------------------------------

// Routine leest data uit EEPROM
byte EEPROM_lees (int adres_eeprom) {
 return EEPROM.read(adres_eeprom);
}
//-------------------------------------------------------------------------------------------
