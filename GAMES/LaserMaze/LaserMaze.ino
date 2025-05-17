// Game:      Laser Maze
// Copyright: 2025 Frank van de Ven 
// Licence:   MIT

 #include <Arduboy2.h>                         // installeer de Arduboy2 bibliotheek
 #include "GameData.h"
 Arduboy2 arduboy;                             // maakt een Arduboy object aan

// variabelen
 byte x, y, z;                                 // teller / tijdelijk geheugen
 byte nr0, nr1, nr2, nr3, nr4, nr5;            // tijdelijk geheugen stack               
 int  t;                                       // tijdelijk geheugen
 byte spel[5][6];                              // opslag speelveld (volgorde y-as,x-as)
 byte bezet[5][6];                             // 0 = geen pion, 1 = pion niet verplaatsbaar
 byte einde2[5][5];                            // lijnen teller (voorkomt een laser loep)
 int stack[10][3];                             // stack waarop alle te tekenen lasers komen
 byte stack_nr = 0;                            // stackpointer (houdt aantal laserstralen bij
 byte level = 0;                               // level nummer
 bool knipper = 0;                             // knipperende pion (1 = aan, 0 = uit)
 bool laser = 0;                               // 1 = laat laserstraal zien. 0 = geen laser
 int Sx, Sy;                                   // x, y positie geselecteerde pion
 int Cx, Cy;                                   // x, y positie cursor
 int Lx, Ly;                                   // x, y positie laser pion
 int LSx, LSy;                                 // x, y positie laserstraal
 int Rx = 0, Ry = 0;                           // x, y richting laserstraal 1 (-1,0,1)
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
 arduboy.initRandomSeed();                     // voor random getallen
 load_level(level);                            // laad level data in RAM
}
//===========================================================================================

void loop() {
 if (!(arduboy.nextFrame())) {return;}         // wacht tot het tijd is voor volgende frame
 if (arduboy.everyXFrames(8)){knipper = !knipper;}  // zet om 8 frames knipper aan / uit
 arduboy.pollButtons();                        // controle of er een knop wordt gedrukt
 arduboy.clear();                              // wis het video geheugen
 print_teksen();                               // zet teksten op LCD
 knoppen();                                    // lees en verwerk knoppen
 print_speelveld();                            // zet pionnen uit RAM in speelveld + voorraad
 if (laser == 0 && level < 27){                // bij 0 geen laserstraal tekenen
  print_cursor();                              // zet cursor op LCD
 }
 else{                                         // teken laserstraal
  knipper =  0;                                // zet knipperen uit
  print_speelveld();                           // zet pionnen uit RAM in speelveld
  if (level < 27) {
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
  while(arduboy.pressed(A_BUTTON) == false){ // zolang er NIET op A wordt gedrukt?
   ESP.wdtFeed();
   if (arduboy.pressed(B_BUTTON) == true) {  // wordt er op B gedrukt?
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

// teken laserstraal
void teken_laser(){
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
 stack[stack_nr][2] = spel[Ly][Lx] - 12;     // zet richting laserstraal op stack
 while (stack_nr > 0) {                      // teken laser stralen tot stack leeg is
  einde = 0;                                 // reset einde laserstraal marker
  LSx = stack[stack_nr][0];                  // haal startpositie x laserstraal van stack
  LSy = stack[stack_nr][1];                  // haal startpositie y laserstraal van stack
  Lr = stack[stack_nr][2];                   // haal richting laserstraal van stack
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
     arduboy.drawFastHLine ((LSx*12+47+nr0),(LSy*12+7),nr1); // teken de lijn
     einde2[LSy][LSx]++;                      // verhoog lijnen teller met 1
    }
    if (nr3 > 0) {                            // moet er een verticale lijn getekend worden?
     arduboy.drawFastVLine ((LSx*12+53),(LSy*12+1+nr2),nr3); // teken de lijn
     einde2[LSy][LSx]++;                      // verhoog lijnen teller met 1
    }
    if (nr5 == 5) {                           // moet er een ingekleurd doel komen?
     Sprites::drawSelfMasked((LSx*12+49),(LSy*12+3), pion, spel[LSy][LSx]+9); // print doel
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
 ESP.wdtFeed();
 if (arduboy.justPressed(B_BUTTON)){           // is knop B ingedrukt
  laser = !laser;                              // teken laserstraal aan (1) / uit (0) zetten
   if (doel2 == 0 && laser == 0){              // heb je alle doelen geraakt?
    level++;                                   // volgende level
    if (level == 28) {level = 0;}              // start na laatste level weer opnieuw
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
void load_level(byte level2){
 doel = 0;                                     // reset doel teller
 for (x = 0; x < 6; x++) {                     // loep horizontaal door speelveld
  for (y = 0; y < 5; y++) {                    // loep verticaal door speelveld
  spel[y][x] = pgm_read_byte (&levels[level][y][x]);  // zet level data in speelveld
    if (spel[y][x] == 1) {                     // staat hier een spiegel?
     z = random(0,4);                          // kies een getal tussen 0 en 3
     spel[y][x] = spel[y][x] + z;              // pas de spiegel aan zodat die random is
    } 
    if (spel[y][x] == 5) {                     // staat hier een halfdoorlatende spiegel?
     z = random(0,2);                          // kies een getal tussen 0 en 1
     spel[y][x] = spel[y][x] + z;              // pas de spiegel aan zodat die random is
    } 
   bezet[y][x] = 0;                            // 0 = vrij (selecteerbare plaats)
   if (x < 5 && spel[y][x] > 0 && spel[y][x] < 7) { // staat hier een spiegel?
    bezet[y][x] = 1;}                          // 1 = hier staat een spiegel
   if (x < 5 && spel[y][x] > 6 && spel[y][x] < 11) {// staat hier een doel?
    bezet[y][x] = 2;                           // 2 (of 3) hier staat een doel
    doel++;}                                   // verhoog doel teller met 1
   if (x < 5 && spel[y][x] > 10) {             // staat hier een laser of hindernis?
    bezet[y][x] = 4;}                          // 4 = hier staat een hindernis of laser
   if ((spel[y][x] > 11) && (spel[y][x] < 16)){// is dit de laser?
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
 if (level == 27) {
  arduboy.setCursor(52,16);                    // positioneer tekst cursor
  arduboy.print("Well done");                  // print tekst
  arduboy.setCursor(52,36);                    // positioneer tekst cursor
  arduboy.print("Game over");                  // print tekst
 }
 else {
  arduboy.setCursor(0,26);                     // positioneer tekst cursor
  arduboy.print("Level");                      // print tekst
  arduboy.setCursor(32,26);                    // positioneer tekst cursor
  arduboy.print(level+1);                      // print level nummer
 }
}
//-------------------------------------------------------------------------------------------
