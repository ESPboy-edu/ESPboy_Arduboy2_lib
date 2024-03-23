#include <Arduboy2.h>
#include "fxdta.h"
#include <ArduboyFX.h> 
#include "fxdata.h"

Arduboy2 arduboy;

//==================================================================
//Definitions and gamemode
uint8_t gamemode = 0;
uint8_t menu = 1;
int8_t unit = 0; 
int8_t weapon = 0; 
int8_t rare = 0;
int8_t team = 0;
//battle related
uint8_t area = 0;
uint8_t battleround = 0;
uint8_t battleturn = 0;
uint8_t enemylevel = 1;
uint8_t enemy[3] = {6, 7, 8};
uint8_t enemyweapon[3] = {1, 0, 3};
long AteamHP = 1;
long AteamHPmax = 1;
uint16_t AteamPS = 0;
uint16_t AteamMP = 0;
long EteamHP = 1;
long EteamHPmax = 1;
uint16_t EteamPS = 0;
uint16_t EteamMP = 0;
uint8_t action[3] = {0, 0, 0};
//global: ATK0, SHDflat1, SHD%2 (removed), HEAL3, ENTflat4
uint16_t Aaction[5] = {0, 0, 0, 0, 0};
uint16_t Eaction[5] = {0, 0, 0, 0, 0};
uint8_t battlephase = 32;


uint8_t maphold[2] = {3, 4};
uint8_t mapoffset = 0;
uint8_t stepcnt = 0;
uint8_t timecntT = 0;
uint8_t previous = 0;
//===================================================================
//Arrays for default unit and weapon status (fixed)
uint16_t unitStat[14] = {3422, 1215, 2341, 3323, 2314, 1161, 3322, 1223, 2232, 2341, 3512, 1233, 3422, 2343};

uint16_t weaponStat[72] = {12010, 21100, 33001, 43001, 31100, 22010, 11001, 43100, 12020, 21200, 33002, 43101, 11110, 13011, 23101, 43002, 11011, 12011, 31101, 41101, 
23011, 22020, 12101, 11200, 43102, 21300, 12030, 33003, 23012, 13111, 12021, 11300, 11120, 22120, 11111, 43003, 13003, 33102, 12111, 41201, 
43103, 21400, 12040, 33004, 23004, 13112, 12022, 11400, 11211, 33202, 23202, 43004, 11022, 12031, 13103, 41301, 11130, 21301, 12040, 42022, 13004, 
33103, 12121, 11121, 31301, 22130, 21310, 11310, 13013, 12202, 32112, 12220};


//===================================================================
//Game save and load

struct GameState {        
  uint8_t playerlevel;                //save playerlevel
  uint16_t playerexp;                  //save playerexp
  uint8_t unitlock[6];                //save unitlock (0: locked, 1: unlocked)
  uint8_t weaponlock[72];             //save weaponlock (0: locked, 1: unlocked)
  uint8_t ally[3];                    //store default/current team members 
  uint8_t weapon[3];                  //store default/current weapons
};

GameState gameState;
//===================================================================

//Arduboy Setup and declarations
void setup() {
 arduboy.begin();
  arduboy.setFrameRate(20);
  FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);  // initialise FX chip
      if(!FX::loadGameState(gameState)){
        //unlock default units(first 3) and weapons(first 4) 
      for (int x = 0; x < 3; x++){
        gameState.unitlock[x] = 1;
      }
      //unlock default weapons
      for (int y = 0; y < 4; y++){
        gameState.weaponlock[y] = 1;
      }
      //default members
      gameState.ally[0] = 0;        //kaire
      gameState.ally[1] = 2;        //lyra
      gameState.ally[2] = 1;        //hana     
       //default weapons  
      gameState.weapon[0] = 1;      //wooden buckler
      gameState.weapon[1] = 0;      //dull daggers
      gameState.weapon[2] = 2;      //vinewood staff
      //default level and exp
      gameState.playerlevel = 1;
      gameState.playerexp = 0;
      
    }
  FX::saveGameState(gameState);         // save the gameState to FX 4K save block

  
  //Seed the random number generator----------------------------
  arduboy.initRandomSeed();


}



//==================================================================
//Arduboy loop
void loop() {

  if (!arduboy.nextFrame()) {
    return;
  }
  if (arduboy.everyXFrames(3))
    arduboy.pollButtons();
  
  gameloop();

  // arduboy.display();
  FX::display(CLEAR_BUFFER);
}


//==================================================================
//Gameloop
void gameloop(){

  switch(gamemode) {
  
    //Logo 1============================================================================
    case 0:
      //load game 
      //logo();
      FX::loadGameState(gameState);
      gamemode = 3;
      break;

    /*//Logo 2============================================================================
    case 1:
      FX::drawBitmap(0, 0, Full, 0, dbmNormal);
      if (arduboy.pressed(A_BUTTON))      //To Title
        gamemode = 2;    
      break;


    //Title screen 1======================================================================
    case 2:
      title();
      break;
    */
    
    //Title screen 2======================================================================
    case 3:
      FX::drawBitmap(0, 0, Full, 1, dbmNormal);
      FX::drawBitmap(25, 21, Gsname, 0, dbmMasked);
      if (arduboy.pressed(A_BUTTON))      //To Menu
        gamemode = 4;
      break;

    //Menu screen====================================================================== 
    case 4:
      arduboy.setFrameRate(20);   
      battlephase = 32;
      battleround = 0;
      //unlock 3 units at level 10
      if(gameState.playerlevel == 10){
        gameState.unitlock[3] = 1;
        gameState.unitlock[4] = 1;
        gameState.unitlock[5] = 1;
      }

      if (arduboy.pressed(RIGHT_BUTTON) && arduboy.everyXFrames(3))
        menu = menu + 1;
      if (arduboy.pressed(LEFT_BUTTON) && arduboy.everyXFrames(3))
        menu = menu - 1;
      if (menu == 7)
        menu = 1;
      if (menu == 0)
        menu = 6;

      menuDisplay(menu, 0);
      
      // Change to namecard
      if (menu == 2 && arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 5;
      }

      // Change to inve
      if (menu == 3 && arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 6;
      }

      // Change to team
      if (menu == 4 && arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 13;
      }

      //change area to start
      if(menu == 5){
        FX::drawBitmap(32, 32, Background, 2, dbmNormal);
        arduboy.setCursor(36, 36);
        arduboy.print(F("Book cart to?"));
        arduboy.setCursor(36, 46);
        arduboy.print(F("Area "));
        if(area == 0){
          arduboy.print(area + 1);
        }
        else 
          arduboy.print(area);
        if (area >= 0 && area <= 80){
          if (arduboy.pressed(UP_BUTTON) && arduboy.everyXFrames(3)){
            area = area + 10;
          }
        }
        if (area % 10 != 0 || area > 80)
          area = 0;
      }
      // Change to adventure
      if (menu == 5 && arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        if (area == 0){
            area++;
        }
        gamemode = 14;
      }
      //cabin display
      if (menu == 6){
            arduboy.fillRect(0, 0, 128, 32, WHITE);
            drawIdle(13, 0, gameState.ally[0], 1);
            drawIdle(47, 0, gameState.ally[1], 1);
            drawIdle(87, 0, gameState.ally[2], 1);
            FX::drawBitmap(0, 0, Cabinroom ,0 , dbmMasked);
            FX::drawBitmap(33, 32, Background, 2, dbmNormal);
            arduboy.setCursor(36, 36);
            arduboy.print(F("Calm time"));
            arduboy.setCursor(36, 46);
            arduboy.print(F("together."));
      }

      break;

    //Name card====================================================================== 
    case 5:
      changeUnit(5);
      namecardDisplay(unit, 0);
      //Back to menu 
      if (arduboy.pressed(B_BUTTON)){
        gamemode = 4;
      }
      break;

    //Inve ====================================================================== 
    case 6:

      menuDisplay(menu, 0);
      drawRarity(64, 0);
      //Goto rarity 1
      if (rare == 0 &&arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 7;
      }
      //Goto rarity 2
      if (rare == 1 &&arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 8;
      }
      //Goto rarity 3
      if (rare == 2 &&arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 9;
      }
      //Goto rarity 4
      if (rare == 3 &&arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 10;
      }

      //Back to menu 
      if (arduboy.pressed(B_BUTTON)){
        gamemode = 4;
      }
      break;

    //rarity 1====================================================================== 
    case 7:
      //set to first weapon of the rarity
      changeWeapon(0, 7);
      weaponDisplay(0, weapon);
      //Back to inve 
      if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
        gamemode = 6;
      }
      break;

    //rarity 2====================================================================== 
    case 8:
      //set to first weapon of the rarity
      changeWeapon(8, 23);
      weaponDisplay(1, weapon);
      //Back to inve 
      if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
        gamemode = 6;
      }
      break;

    //rarity 3====================================================================== 
    case 9:
      //set to first weapon of the rarity
      changeWeapon(24, 39);
      weaponDisplay(2, weapon);
      //Back to inve 
      if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
        gamemode = 6;
      }
      break;

    //rarity 4====================================================================== 
    case 10:
      //set to first weapon of the rarity
      changeWeapon(40, 71);
      weaponDisplay(3, weapon);
      //Back to inve 
      if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
        gamemode = 6;
      }
      break;
      
    //team bulding (units)====================================================================== 
    case 11: 
      changeTeam();          //change team slot number
      drawUnitinfo(unit, 0);   //display current unit info card
      confirmTeam (0);       //confirm unit choice 
      //Back to namecard 
      if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
        gamemode = 5;
      }
      break;

    //team bulding (weapons)====================================================================== 
    case 12: 
      changeTeam();              //change team slot number
      drawWeaponinfo ();   //display current weapon info card
      confirmTeam (1);          //confirm unit choice 
      if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
        gamemode = 6;
      }
      break;

    //team display ====================================================================== 
    case 13: 
      teamDisplay();
      //return to menu
      if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
        FX::saveGameState(gameState);   
        gamemode = 4;
      }
      break;

    //adventure====================================================================== 
    case 14: 
      battleCases();
      //show team HP
      if (arduboy.pressed(UP_BUTTON) && battlephase >= 20 && battlephase <= 25) {
        arduboy.setTextColor(WHITE);
        arduboy.setTextBackground(BLACK);
        arduboy.fillRect(0, 0, 128, 11, BLACK);
        arduboy.setCursor(2, 1);
        arduboy.print(F("HP: "));
        arduboy.print(AteamHP);
        arduboy.print(F("/"));
        arduboy.print(AteamHPmax);
      }
 
      break;

  }

}





//functions


// Return sprite1 (fullbody) uint64_t address by feeding the corresponding unitcode
uint64_t callSprite1 (int unitcode){

  uint64_t sprite1;

  if (unitcode == 0){
    sprite1 = kaire1;
  }

  if (unitcode == 1){
    sprite1 = hana1;
  }

  if (unitcode == 2){
    sprite1 = lyra1;
  }

  if (unitcode == 3){
    sprite1 = zol1;
  }

  if (unitcode == 4){
    sprite1 = draco1;
  }
  
  if (unitcode == 5){
    sprite1 = exa1;
  }

  if (unitcode == 6){
    sprite1 = chloroslime1;
  }

  if (unitcode == 7){
    sprite1 = aquaslime1;
  }

  if (unitcode == 8){
    sprite1 = blazeslime1;
  }

  if (unitcode == 9){
    sprite1 = thorny1;
  }

  if (unitcode == 10){
    sprite1 = orchid1;
  }

  if (unitcode == 11){
    sprite1 = coyote1;
  }

  if (unitcode == 12){
    sprite1 = spiro1;
  }

  if (unitcode == 13){
    sprite1 = alpha1;
  }


  return sprite1;

}

// Return sprite2 (portrait) uint64_t address by feeding the corresponding unitcode
uint64_t callSprite2 (int unitcode){

  uint64_t sprite2;

  if (unitcode == 0){
    sprite2 = kaire2;
  }

  if (unitcode == 1){
    sprite2 = hana2;
  }

  if (unitcode == 2){
    sprite2 = lyra2;
  }

  if (unitcode == 3){
    sprite2 = zol2;
  }

  if (unitcode == 4){
    sprite2 = draco2;
  }
  
  if (unitcode == 5){
    sprite2 = exa2;
  }

  if (unitcode == 6){
    sprite2 = chloroslime2;
  }

  if (unitcode == 7){
    sprite2 = aquaslime2;
  }

  if (unitcode == 8){
    sprite2 = blazeslime2;
  }

  if (unitcode == 9){
    sprite2 = thorny2;
  }

  if (unitcode == 10){
    sprite2 = orchid2;
  }

  if (unitcode == 11){
    sprite2 = coyote2;
  }

  if (unitcode == 12){
    sprite2 = spiro2;
  }

  if (unitcode == 13){
    sprite2 = alpha2;
  }


  return sprite2;

}

//call rarity of weapon
int callRarity(int code){
  int x = 0;

  if (code >= 0 && code < 8)
    x = 1;
  if (code >= 8 && code < 24)
    x = 2;
  if (code >= 24 && code < 40)
    x = 3;
  if (code >= 40 && code < 72)
    x = 4;
  
  return x;
}

//Call action of weapon
uint8_t callAction (int code){

  int x = weaponStat[code] / 10000 % 10;

  return x;
}

//Call AT or weapon scale (mode 0: units, 1: weapons)
int callATorScale (int code, int mode){

  int x;
  if(mode == 0){
    x = unitStat[code] / 1000 % 10;
  }
  if(mode == 1){
    x = weaponStat[code] / 1000 % 10;
  }
  return x;
}
//Call HP or HPmod (mode 0: units, 1: weapons)
int callHP (int code, int mode){

  int x;
  if(mode == 0){
    x = unitStat[code] / 100 % 10;
  }
  if(mode == 1){
    x = weaponStat[code] / 100 % 10;
  }
  return x;
}
//Call PS or PSmod (mode 0: units, 1: weapons)
int callPS (int code, int mode){

  int x;
  if(mode == 0){
    x = unitStat[code] / 10 % 10;
  }
  if(mode == 1){
    x = weaponStat[code] / 10 % 10;
  }
  return x;
}
//Call MP or MPmod (mode 0: units, 1: weapons)
int callMP (int code, int mode){

  int x;
  if(mode == 0){
    x = unitStat[code] % 10;
  }
  if(mode == 1){
    x = weaponStat[code] % 10;
  }
  return x;
}

//Draw unit names on screen
void drawName (int x, int y, int unitcode){
  
  arduboy.setCursor(x, y);
  if (unitcode == 0){
    arduboy.print(F("Kaire"));
  }

  if (unitcode == 1){
    arduboy.print(F("Hana"));
  }

  if (unitcode == 2){
    arduboy.print(F("Lyra"));
  }

  if (unitcode == 3){
    arduboy.print(F("Zol"));
  }

  if (unitcode == 4){
    arduboy.print(F("Draco"));
  }
  
  if (unitcode == 5){
    arduboy.print(F("EXA-II"));
  }

  if (unitcode == 6){
    arduboy.print(F("Chloroslime"));
  }

  if (unitcode == 7){
    arduboy.print(F("Aquaslime"));
  }

  if (unitcode == 8){
    arduboy.print(F("Blazeslime"));
  }

  if (unitcode == 9){
    arduboy.print(F("Thorny Euglena"));
  }

  if (unitcode == 10){
    arduboy.print(F("Orchid Cicada"));
  }

  if (unitcode == 11){
    arduboy.print(F("Plated Coyote"));
  }

  if (unitcode == 12){
    arduboy.print(F("Spirodendrobium"));
  }

  if (unitcode == 13){
    arduboy.print(F("Plains Warwolf"));
  }
}


//Draw attribute sprite (x, y : coords, unitcode)
void drawAT (int x, int y, int unitcode){

  //draw AQ sprite
  if (callATorScale (unitcode, 0) == 1)
    FX::drawBitmap(x, y, Action, 0, dbmNormal);

  //draw BL sprite
  if (callATorScale (unitcode, 0) == 2)
    FX::drawBitmap(x, y, Action, 1, dbmNormal);

  //draw BL sprite
  if (callATorScale (unitcode, 0) == 3)
    FX::drawBitmap(x, y, Action, 2, dbmNormal);

}

void drawAction(int x, int y, int weaponcode){

  //draw ATK sprite
  if (callAction(weaponcode) == 1)
    FX::drawBitmap(x, y, Action, 3, dbmNormal);

  //draw SHD sprite
  if (callAction(weaponcode) == 2)
    FX::drawBitmap(x, y, Action, 4, dbmNormal);

  //draw HEAL sprite
  if (callAction(weaponcode) == 3)
    FX::drawBitmap(x, y, Action, 5, dbmNormal);

  //draw ENT sprite
  if (callAction(weaponcode) == 4)
    FX::drawBitmap(x, y, Action, 6, dbmNormal);

}

//Draw idle animation of units (x, y : coords, unitcode, mode: 1 = sprite1, 2 = sprite2)
void drawIdle (int x, int y, int unitcode, int mode){
  static int timecnt = 1;
  if(arduboy.everyXFrames(3) == true){
    timecnt ++;
  }
  if (timecnt > 30)
    timecnt = 1;
  
  //draw full body sprite 
  if (mode == 1){
    if (timecnt <= 15){
      FX::drawBitmap(x, y, callSprite1(unitcode), 0, dbmMasked);
    }
    else
      FX::drawBitmap(x, y, callSprite1(unitcode), 1, dbmMasked);
  }

  //draw portrait sprite 
  if (mode == 2){
    if (timecnt <= 15){
      FX::drawBitmap(x, y, callSprite2(unitcode), 0, dbmNormal);
    }
    else
      FX::drawBitmap(x, y, callSprite2(unitcode), 1, dbmNormal);
  }

}

//Draw walk animation of units (x, y : coords, unitcode)
void drawWalk (int x, int y, int unitcode){
  static int timecnt = 1;
  if(arduboy.everyXFrames(2) == true){
    timecnt ++;
  }
  if (timecnt > 30)
    timecnt = 1;
  
  //draw full body sprite 
    if (timecnt <= 15){
      FX::drawBitmap(x, y, callSprite1(unitcode), 2, dbmMasked);
    }
    else
      FX::drawBitmap(x, y, callSprite1(unitcode), 3, dbmMasked);

}

//Draw angry animation of units (x, y : coords, unitcode)
void drawAngry (int x, int y, int unitcode, int mode){

  if (mode == 1){
      FX::drawBitmap(x, y, callSprite1(unitcode), 7, dbmMasked);
  }
  if (mode == 2){
      FX::drawBitmap(x, y, callSprite2(unitcode), 4, dbmNormal);
  }
  //boss
  if (mode == 3){
      FX::drawBitmap(x, y, callSprite2(unitcode), 3, dbmNormal);
  }


}

//Draw Hngry animation of units (x, y : coords, unitcode)
void drawHappy (int x, int y, int unitcode, int mode){

  if (mode == 1){
      FX::drawBitmap(x, y, callSprite1(unitcode), 4, dbmMasked);
  }
  if (mode == 2){
      FX::drawBitmap(x, y, callSprite2(unitcode), 2, dbmNormal);
  }
  //boss
  if (mode == 3){
      FX::drawBitmap(x, y, callSprite2(unitcode), 1, dbmNormal);
  }
}

//Draw sad animation of units (x, y : coords, unitcode)
void drawSad (int x, int y, int unitcode, int mode){

  if (mode == 1){
      FX::drawBitmap(x, y, callSprite1(unitcode), 5, dbmMasked);
  }
  if (mode == 2){
      FX::drawBitmap(x, y, callSprite2(unitcode), 3, dbmNormal);
  }
}



//Menu display
void menuDisplay (int menu, int mode){
  static int timecnt = 1;
  if (arduboy.everyXFrames(2) == true){
      timecnt ++;
  }
  //  Reset timecnt
  if (timecnt == 31){
      timecnt = 1;
  }

  //  Set bg white
  arduboy.fillScreen(WHITE);

  // Textbox
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);

  //In guild hall
  if(mode == 0){
    //Staff animation + msg
    if (timecnt == 7 || timecnt == 17 || timecnt == 19){
      FX::drawBitmap(0, 0, Menu, 8, dbmNormal);
    }
    else
      FX::drawBitmap(0, 0, Menu, 7, dbmNormal);

    arduboy.setCursor(36, 36);
    arduboy.print(F("What do you"));
    arduboy.setCursor(36, 46);
    arduboy.print(F("need today?"));
  }

  //In battle
  if (mode == 1){
    FX::drawBitmap(0, 0, Text, 7, dbmNormal);
    arduboy.setCursor(36, 36);
    arduboy.print(F("Take position!"));
  }

  //  smallarrow animation
  if (timecnt <= 15){
    FX::drawBitmap(32, 0, Background, 0, dbmNormal);
  }


  //  Icons
  //Guildhall
  if (menu == 1) {
    FX::drawBitmap(32, 0, Background, 1, dbmNormal);
    FX::drawBitmap(0, 32, Text, 13, dbmNormal);
  }
  //Namecard
  if (menu == 2) {
    FX::drawBitmap(64, 0, Menu, 0, dbmNormal);
    FX::drawBitmap(0, 32, Text, 0, dbmNormal);
  }
  //Inve
  if (menu == 3) {
    FX::drawBitmap(64, 0, Menu, 1, dbmNormal);
    FX::drawBitmap(0, 32, Text, 1, dbmNormal);
  }
  //Team
  if (menu == 4) {
    FX::drawBitmap(64, 0, Menu, 2, dbmNormal);
    FX::drawBitmap(0, 32, Text, 2, dbmNormal);
  }
  //Adventure
  if (menu == 5) {
    FX::drawBitmap(64, 0, Menu, 3, dbmNormal);
    FX::drawBitmap(0, 32, Text, 3, dbmNormal);
  }
  //Cabin
  if (menu == 6) {
    FX::drawBitmap(0, 32, Text, 5, dbmNormal);
  }

}

//Change current unit 
void changeUnit(int max){
      if (arduboy.pressed(RIGHT_BUTTON) && arduboy.everyXFrames(3))
        unit = unit + 1;
      if (arduboy.pressed(LEFT_BUTTON) && arduboy.everyXFrames(3))
        unit = unit - 1;
      if (unit > max)
        unit = 0;
      if (unit < 0)
        unit = max;
}

//change current team value
void changeTeam(){
      if (arduboy.pressed(RIGHT_BUTTON) && arduboy.everyXFrames(3))
        team = team + 1;
      if (arduboy.pressed(LEFT_BUTTON) && arduboy.everyXFrames(3))
        team = team - 1;
      if (team > 2)
        team = 0;
      if (team < 0)
        team = 2;
}

//Change current weapon
void changeWeapon(int min, int max){
      if (arduboy.pressed(RIGHT_BUTTON) && arduboy.everyXFrames(3))
        weapon = weapon + 1;
      if (arduboy.pressed(LEFT_BUTTON) && arduboy.everyXFrames(3))
        weapon = weapon - 1;
      if (weapon > max)
        weapon = min;
      if (weapon < min)
        weapon = max;
}

//draw unit info card on screen by unit code
void drawUnitinfo (int unitcode, int mode) {
    arduboy.fillRect(0, 0, 32, 64, WHITE);
    drawIdle(0, 0, unitcode, 1);
    drawIdle(0, 32, unitcode, 2);
    drawAT(32, 0, unitcode);
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);
    drawName(72, 2, unitcode);
    arduboy.setCursor(72, 14);
    arduboy.print(F("LV:  "));
    arduboy.print(gameState.playerlevel);
    arduboy.setCursor(72, 24);
    arduboy.print(F("EXP: "));
    arduboy.print(gameState.playerexp);
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE); 
    printAllyslot(mode);
    arduboy.setCursor(36, 36);
    arduboy.print(F("HP: "));
    arduboy.print(callHP(unitcode, 0)*gameState.playerlevel*10);
    arduboy.setCursor(36, 48);
    arduboy.print(F("PS: "));
    arduboy.print(callPS(unitcode, 0)*gameState.playerlevel);
    arduboy.setCursor(82, 48);
    arduboy.print(F("MP: "));
    arduboy.print(callMP(unitcode, 0)*gameState.playerlevel);
}
//draw weapon info card on screen
void drawWeaponinfo (){
    // 1 star
    if (rare == 0)
      FX::drawBitmap(0, 32, rarity, 0, dbmNormal);
    // 2 star
    if (rare == 1)
      FX::drawBitmap(0, 32, rarity, 1, dbmNormal);
    // 3 star
    if (rare == 2)
      FX::drawBitmap(0, 32, rarity, 2, dbmNormal);
    // 4 star
    if (rare == 3)
      FX::drawBitmap(0, 32, rarity, 3, dbmNormal);
  FX::drawBitmap(0, 0, weapons, weapon, dbmNormal);
  drawAction(32, 0, weapon);
  arduboy.setTextColor(WHITE);
  arduboy.setTextBackground(BLACK);
  arduboy.setCursor(68, 12);
  arduboy.print(F("SCALE: "));
    if (callATorScale(weapon, 1) == 1)
        arduboy.print(F("HP"));
    if (callATorScale(weapon, 1) == 2)
        arduboy.print(F("PS"));
    if (callATorScale(weapon, 1) == 3)
        arduboy.print(F("MP"));
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);
    printWeaponslot();
    arduboy.setCursor(36, 36);
    arduboy.print(F("HP "));
    printPlus(callHP (weapon, 1));
    arduboy.setCursor(36, 48);
    arduboy.print(F("PS "));
    printPlus(callPS (weapon, 1));
    arduboy.setCursor(82, 48);
    arduboy.print(F("MP "));
    printPlus(callMP (weapon, 1));
}

//print teamslot number
void printAllyslot(int mode){
  arduboy.setCursor(100, 36);
  if(mode == 0){
    if(unit == gameState.ally[0])
    arduboy.print(F("[T1]"));
    if(unit == gameState.ally[1])
    arduboy.print(F("[T2]"));
    if(unit == gameState.ally[2])
    arduboy.print(F("[T3]"));
  }
  if(mode == 1){
    if(gameState.ally[unit] == gameState.ally[0])
    arduboy.print(F("[T1]"));
    if(gameState.ally[unit] == gameState.ally[1])
    arduboy.print(F("[T2]"));
    if(gameState.ally[unit] == gameState.ally[2])
    arduboy.print(F("[T3]"));
  }

}
//print teamslot number
void printWeaponslot(){
  arduboy.setCursor(100, 36);
  if(weapon == gameState.weapon[0] )
    arduboy.print(F("[T1]"));
  if(weapon == gameState.weapon[1] )
    arduboy.print(F("[T2]"));
  if(weapon == gameState.weapon[2] )
    arduboy.print(F("[T3]"));
}

//Namecard display
void namecardDisplay(int unit, int mode){

  if (mode == 0){
    if (gameState.unitlock[unit] == 1){
    drawUnitinfo (unit, mode);
    //to team confirmation screen
    if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 11;
      }
    }
   else{
    FX::drawBitmap(0, 0, lock, 0, dbmNormal);
    FX::drawBitmap(0, 32, lock, 0, dbmNormal);
    FX::drawBitmap(32, 0, lock, 0, dbmNormal);
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);
    arduboy.setCursor(72, 2);
    arduboy.print(F("?????"));
    arduboy.setCursor(72, 14);
    arduboy.print(F("LV:  "));
    arduboy.print(F("??"));
    arduboy.setCursor(72, 24);
    arduboy.print(F("EXP: "));
    arduboy.print(F("???"));
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);
    arduboy.setCursor(36, 36);
    arduboy.print(F("HP: "));
    arduboy.print(F("???"));
    arduboy.setCursor(36, 48);
    arduboy.print(F("PS: "));
    arduboy.print(F("???"));
    arduboy.setCursor(82, 48);
    arduboy.print(F("MP: "));
    arduboy.print(F("???"));
    }
  }

  //in battle
  if (mode == 1){
      drawUnitinfo (gameState.ally[unit], mode);
    //to team confirmation screen
    if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        battlephase = 18;
      }
  }

}


//Confirm unit/weapon allocation to team (mode 0: unit, mode 1: weapon)
void confirmTeam (int mode){

  uint8_t tmp1;
  uint8_t tmp2;

  //unit mode
  if (mode == 0)
    drawUnitinfo (unit, mode);
  //weapon mode
  if (mode == 1)
    drawWeaponinfo ();
  //battle mode 
  if (mode == 2)
    drawUnitinfo (gameState.ally[unit], 1);
  //cover bottm text box
  FX::drawBitmap(32, 32, Background, 2, dbmNormal);
  arduboy.setCursor(36, 36);
  arduboy.print(F("Change to ?"));
  arduboy.setCursor(36, 48);
  arduboy.print(F("Team slot "));
  arduboy.print(team + 1);
  //confirm unit choice 
  if (mode == 0 && arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3)){
    if (findSlot(unit, 0) != -1 || findSlot(unit, 0) == team){      //if unit alr present in team or in current slot
      gameState.ally[findSlot(unit, 0)] = gameState.ally[team];  //exchange position first
      gameState.ally[team] = unit;
      //return to namecard
      gamemode = 5;
    }
    else{                              //if unit not present in team
      gameState.ally[team] = unit;
      //return to namecard
      gamemode = 5;
    }                              
  }

  //confirm weapon choice 
  if (mode == 1 && arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3)){
    if (findSlot(weapon, 1) != -1 || findSlot(weapon, 1) == team){
      gameState.weapon[findSlot(weapon, 1)] = gameState.weapon[team];  //exchange position first
      gameState.weapon[team] = weapon;
      //return to inve
      gamemode = 6;
    }
    else{                              //if weapon not present in team
      gameState.weapon[team] = weapon;
      //return to namecard
      gamemode = 6;
    }   
  }

  //battle mode (change weapon together with units)
  if (mode == 2 && arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3)){
    if (unit != team){      //if changing to different position
      //exchange position
      tmp1 = gameState.ally[unit];
      tmp2 = gameState.weapon[unit];
      gameState.ally[unit] = gameState.ally[team];
      gameState.weapon[unit] = gameState.weapon[team];
      gameState.ally[team] = tmp1; 
      gameState.weapon[team] = tmp2; 

      //return to battle namecard
      battlephase = 17;
    }    
    else{
      battlephase = 17;  
    }
                    
  }
}

//find target slot, return position in array
int8_t findSlot(int target, int mode){
  int8_t pos = -1; //return error value if not found
  //for units
  if (mode == 0){
    for (int x = 0; x < 3; x++){
      if(gameState.ally[x] == target){
        pos = x;
      }
    }
  }
  if (mode == 1){
    for (int x = 0; x < 3; x++){
      if(gameState.weapon[x] == target){
        pos = x;
      }
    }
  }
  return pos;
}

//Draw and Change rarity
void drawRarity(int x, int y){
  if (arduboy.pressed(RIGHT_BUTTON) && arduboy.everyXFrames(3))
     rare = rare + 1;
  if (arduboy.pressed(LEFT_BUTTON) && arduboy.everyXFrames(3))
    rare = rare - 1;
  if (rare > 3)
    rare = 0;
  if (rare < 0)
    rare = 3;

  // 1 star
  if (rare == 0)
    FX::drawBitmap(x, y, rarity, 0, dbmNormal);
  // 2 star
  if (rare == 1)
    FX::drawBitmap(x, y, rarity, 1, dbmNormal);
  // 3 star
  if (rare == 2)
    FX::drawBitmap(x, y, rarity, 2, dbmNormal);
  // 4 star
  if (rare == 3)
    FX::drawBitmap(x, y, rarity, 3, dbmNormal);
}


void weaponDisplay(int rareI, int weapon){

    // 1 star
    if (rareI == 0)
      FX::drawBitmap(0, 32, rarity, 0, dbmNormal);
    // 2 star
    if (rareI == 1)
      FX::drawBitmap(0, 32, rarity, 1, dbmNormal);
    // 3 star
    if (rareI == 2)
      FX::drawBitmap(0, 32, rarity, 2, dbmNormal);
    // 4 star
    if (rareI == 3)
      FX::drawBitmap(0, 32, rarity, 3, dbmNormal);

  if (gameState.weaponlock[weapon] == 1){
    drawWeaponinfo ();
    //to weapon confirmation screen
    if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
        gamemode = 12;
    }
  }
  else{
    FX::drawBitmap(0, 0, lock, 0, dbmNormal);
    FX::drawBitmap(32, 0, lock, 0, dbmNormal);
    arduboy.setTextColor(WHITE);
    arduboy.setTextBackground(BLACK);
    arduboy.setCursor(68, 12);
    arduboy.print(F("SCALE: "));
    arduboy.print(F("??"));
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);
    arduboy.setTextColor(BLACK);
    arduboy.setTextBackground(WHITE);
    arduboy.setCursor(36, 36);
    arduboy.print(F("HP "));
    arduboy.print(F("???"));
    arduboy.setCursor(36, 48);
    arduboy.print(F("PS "));
    arduboy.print(F("???"));
    arduboy.setCursor(82, 48);
    arduboy.print(F("MP "));
    arduboy.print(F("???"));
  }
}

//Print plus signs
void printPlus(int mod){
  if (mod == 1)
    arduboy.print(F("+"));
  if (mod == 2)
    arduboy.print(F("++"));
  if (mod == 3)
    arduboy.print(F("+++"));
  if (mod == 4)
    arduboy.print(F("++++"));
}

//Draw the team template
void teamDisplay(){
  //draw background 
  arduboy.fillScreen(WHITE);
  drawMini(0, 0);
  drawIdle(32, 0, gameState.ally[0], 2);
  drawIdle(64, 0, gameState.ally[1], 2);
  drawIdle(96, 0, gameState.ally[2], 2);
  FX::drawBitmap(32, 32, weapons, gameState.weapon[0], dbmNormal); 
  FX::drawBitmap(64, 32, weapons, gameState.weapon[1], dbmNormal); 
  FX::drawBitmap(96, 32, weapons, gameState.weapon[2], dbmNormal); 
  FX::drawBitmap(0, 32, Text, 2, dbmNormal); 
}

//print mini icons
void drawMini (int x, int y){
  FX::drawBitmap(x, y, mframe, 0, dbmNormal); 
  //draw for units
  drawMiniicons (x + 3,y + 6, gameState.ally[0], 0);
  drawMiniicons (x + 12,y + 6, gameState.ally[1], 0);
  drawMiniicons (x + 21,y + 6, gameState.ally[2], 0);
  //draw for weapons
  drawMiniicons (x + 3,y + 18, gameState.weapon[0], 1);
  drawMiniicons (x + 12,y + 18, gameState.weapon[1], 1);
  drawMiniicons (x + 21,y + 18, gameState.weapon[2], 1);
}
void drawMiniicons (int x, int y, int code, int mode){
  //for units
  if (mode == 0){
      if (callATorScale(code, 0) == 1){
        FX::drawBitmap(x, y, micons, 0, dbmNormal); 
      }
      if (callATorScale(code, 0) == 2){
        FX::drawBitmap(x, y, micons, 1, dbmNormal); 
      }
      if (callATorScale(code, 0) == 3){
        FX::drawBitmap(x, y, micons, 2, dbmNormal); 
      }
  }
  //for weapons
  if (mode == 1){
      if (callAction(code) == 1){
        FX::drawBitmap(x, y, micons, 3, dbmNormal); 
      }
      if (callAction(code) == 2){
        FX::drawBitmap(x, y, micons, 4, dbmNormal); 
      }
      if (callAction(code) == 3){
        FX::drawBitmap(x, y, micons, 5, dbmNormal); 
      }
      if (callAction(code) == 4){
        FX::drawBitmap(x, y, micons, 6, dbmNormal); 
      }
  }
}

//============================================================================
//Battle main body

//1. Decide eteam /w composition (non-boss)
void enemyRoster(){
    //decide enemylevel by area
	int x = area/10;
	if (area <= 5)
	    enemylevel = 1;
	if (area > 5)
	    enemylevel = random(5*x+1, 5*x+6);
	//decide enemy units and (weapons) by area
	//1-29: slimes only
	if (area < 30){
	    for (int x = 0; x < 3; x++)
	     enemy[x] = random (6, 9);
	}
	//30-59: 2 slimes + 1 advanced enemy
	if (area >= 30 && area < 60){
	    //random slot for the advanced enemy
	    int slot = random(0, 3);
	    enemy[slot] = random (9, 12);
	    //fill rest of slots with slimes
	    for (int x = 0; x < 3; x++){
	        if(x != slot)
	            enemy[x] = random (6, 9);
	    }   
	}
    //60-89: atleast 2 advanced enemy
	if (area >= 60 && area < 90){
	    for (int x = 0; x < 3; x++){
        	enemy[x] = random (9, 11);
      }
      enemy[random(0, 3)] = random(6, 12);
	}
	//90-100: all advanced enemies
	if (area >= 90){
	    for (int x = 0; x < 3; x++)
	        enemy[x] = random (9, 12);
	}

  //boss1
  if (area == 79 || area == 59 || area == 39){
    	for (int x = 0; x < 3; x++)
	      enemy[x] = 12;
  }
  //boss2
  if (area == 100){
    	for (int x = 0; x < 3; x++)
	      enemy[x] = 13;
  }

}

//Decide enemyweapon composition (non-boss)
void enemyWeaponroster(){

  //area 1 - 15: 1*
	if (area < 16){
	    for (int x = 0; x < 3; x++)
	     enemyweapon[x] = random (0, 8);
  }
  //area 16 - 19: 1* - 2*
  if (area >= 16 && area < 20){
	    for (int x = 0; x < 3; x++)
	     enemyweapon[x] = random (0, 24);
  }
  //area 20 - 36: 2*
  if (area >= 20 && area < 37){
	    for (int x = 0; x < 3; x++)
	     enemyweapon[x] = random (8, 24);
  }
  //area 37 - 44: 2* - 3*
  if (area >= 37 && area < 45){
	    for (int x = 0; x < 3; x++)
	     enemyweapon[x] = random (8, 40);
  }
  //area 45 - 64: 3*
  if (area >= 45 && area < 65){
	    for (int x = 0; x < 3; x++)
	     enemyweapon[x] = random (24, 40);
  }
  //area 65 - 79: 3* - 4*
  if (area >= 65){
	    for (int x = 0; x < 3; x++)
	     enemyweapon[x] = random (24, 72);
  }

  //true spirodendrobium
  if (area == 79){
	    enemyweapon[0] = 57;
      enemyweapon[1] = 67;
      enemyweapon[2] = 64;
  }

  //Alpha plains warwolf
  if (area == 100){
	    enemyweapon[0] = 59;
      enemyweapon[1] = 42;
      enemyweapon[2] = 70;
  }

}

//Return modification value (%) of AT interactions by self and opp unitcodes 
uint8_t ATmod(uint8_t self, uint8_t opp){
    
    uint8_t mod = 10;
    //AQ vs BL
    if (callATorScale(self, 0) == 1 && callATorScale(opp, 0) == 2)
        mod = mod + 1;
    //BL vs CH
    if (callATorScale(self, 0) == 2 && callATorScale(opp, 0) == 3)
        mod = mod + 1;
    //CH vs AQ
    if (callATorScale(self, 0) == 3 && callATorScale(opp, 0) == 1)
        mod = mod + 1;  
    
    return mod;
}

//initialize stats (in case)

void initTeamstats (int mode){
    //first time init
    if (mode == 0){
        AteamHP = (((((callHP(gameState.ally[0], 0)*gameState.playerlevel))*ATmod(gameState.ally[0], enemy[0])/10)* (((callHP(gameState.weapon[0], 1)*10) + 10)/10)) + 
         (((callHP(gameState.ally[1], 0)*gameState.playerlevel)*ATmod(gameState.ally[1], enemy[1])/10)* (((callHP(gameState.weapon[1], 1)*10) + 10)/10)) + 
         (((callHP(gameState.ally[2], 0)*gameState.playerlevel)*ATmod(gameState.ally[2], enemy[2])/10)* (((callHP(gameState.weapon[2], 1)*10) + 10)/10)));
         
         EteamHP = (((((callHP(enemy[0], 0)*enemylevel))*ATmod(enemy[0], gameState.ally[0])/10)* (((callHP(enemyweapon[0], 1)*10) + 10)/10)) + 
         (((callHP(enemy[1], 0)*enemylevel)*ATmod(enemy[1], gameState.ally[1])/10)* (((callHP(enemyweapon[1], 1)*10) + 10)/10)) + 
         (((callHP(enemy[2], 0)*enemylevel)*ATmod(enemy[2], gameState.ally[2])/10)* (((callHP(enemyweapon[2], 1)*10) + 10)/10)));
        
    }

         
AteamPS = ((((callPS(gameState.ally[0], 0)*gameState.playerlevel)*ATmod(gameState.ally[0], enemy[0])/10)* (((callPS(gameState.weapon[0], 1)*10) + 10)/10)) + 
         ((((callPS(gameState.ally[1], 0)*gameState.playerlevel))*ATmod(gameState.ally[1], enemy[1])/10)* (((callPS(gameState.weapon[1], 1)*10) + 10)/10)) + 
         (((callPS(gameState.ally[2], 0)*gameState.playerlevel)*ATmod(gameState.ally[2], enemy[2])/10)* (((callPS(gameState.weapon[2], 1)*10) + 10)/10)));

AteamMP = ((((callMP(gameState.ally[0], 0)*gameState.playerlevel)*ATmod(gameState.ally[0], enemy[0])/10)* (((callMP(gameState.weapon[0], 1)*10) + 10)/10)) + 
         (((callMP(gameState.ally[1], 0)*gameState.playerlevel)*ATmod(gameState.ally[1], enemy[1])/10)* (((callMP(gameState.weapon[1], 1)*10) + 10)/10)) + 
         ((((callMP(gameState.ally[2], 0)*gameState.playerlevel))*ATmod(gameState.ally[2], enemy[2])/10)* (((callMP(gameState.weapon[2], 1)*10) + 10)/10)));
         
         
EteamPS = ((((callPS(enemy[0], 0)*enemylevel)*ATmod(enemy[0], gameState.ally[0])/10)* (((callPS(enemyweapon[0], 1)*10) + 10)/10)) + 
         ((((callPS(enemy[1], 0)*enemylevel))*ATmod(enemy[1], gameState.ally[1])/10)* (((callPS(enemyweapon[1], 1)*10) + 10)/10)) + 
         (((callPS(enemy[2], 0)*enemylevel)*ATmod(enemy[2], gameState.ally[2])/10)* (((callPS(enemyweapon[2], 1)*10) + 10)/10)));

EteamMP = ((((callMP(enemy[0], 0)*enemylevel)*ATmod(enemy[0], gameState.ally[0])/10)* (((callMP(enemyweapon[0], 1)*10) + 10)/10)) + 
         (((callMP(enemy[1], 0)*enemylevel)*ATmod(enemy[1], gameState.ally[1])/10)* (((callMP(enemyweapon[1], 1)*10) + 10)/10)) + 
         ((((callMP(enemy[2], 0)*enemylevel))*ATmod(enemy[2], gameState.ally[2])/10)* (((callMP(enemyweapon[2], 1)*10) + 10)/10))); 

}

//return % checkers; 0: not hit, 1 :hit

uint8_t roll(int chance){
    uint8_t x = random(1, 101);
    if (x <= chance)
        return 1;
    else
        return 0;
}

//Actions 
//call scale value
uint16_t scaleValue(int weaponcode, int mode){
  uint16_t x;
      //Ally
    if(mode == 0){
        //scale HP
        if (callATorScale(weaponcode, 1) == 1){
          //nerf HP scale
            x = AteamHPmax/11;
        } 
        //scale PS
        if (callATorScale(weaponcode, 1) == 2){
            x = AteamPS;
        } 
        //scale MP
        if (callATorScale(weaponcode, 1) == 3){
            x = AteamMP;
        } 
    }
    
    //Enemy
    if(mode == 1){
        //scale HP
        if (callATorScale(weaponcode, 1) == 1){
            x = EteamHPmax/11;
        } 
        //scale PS
        if (callATorScale(weaponcode, 1) == 2){
            x = EteamPS;
        } 
        //scale MP
        if (callATorScale(weaponcode, 1) == 3){
            x = EteamMP;
        } 
    }
    
    return x;
}
//ATK, generate raw damage value, by weapon code; mode 0 ally, mode 1 enemy
uint16_t ATK(int weaponcode, int mode){
    uint16_t x = scaleValue(weaponcode, mode);
    //Scaler*1 + rarity scale
    x = (x * 15 / 10);
    
    return x;
    
}

//SHD, generate flat reduction 
uint16_t SHDflat(int weaponcode, int mode){
    uint16_t x = scaleValue(weaponcode, mode);
    //Scaler = * 1
    return x;
}


//HEAL, generate flat healing value
uint16_t HEAL(int weaponcode, int mode){
    uint16_t x = scaleValue(weaponcode, mode); 
    //Scaler = * 1
    return x;
}

//ENT, return % to be added to flat ATK/HEAL, i.e. +30%, return 130
uint16_t ENTflat(int weaponcode, int mode){
    uint16_t x = scaleValue(weaponcode, mode);
    //scale = * 1
    return x;
}


//Battle phases by cases

void battleCases(){

    switch (battlephase){
        
        case 1:
            //reset stepcnt
            stepcnt = 0;
            //reset team stats
            AteamHP = 1;
            AteamPS = 0;
            AteamMP = 0;
            EteamHP = 1;
            EteamPS = 0;
            EteamMP = 0;
            for(int x = 0 ; x< 5; x++){
                Aaction[x] = 0;
                Eaction[x] = 0;
            }
            battleround = 0;
            //decide enemy Roster 
            enemyRoster();
            //decide enemyweapon roster
            for (int x = 0; x < 3; x++){
    	        if (callAction(enemyweapon[x] == 1))
                battlephase = 12;
            }
            enemyWeaponroster();
            break;
        
        case 2:
            //initialize team stats for the first time
            initTeamstats (0);
            AteamHP = AteamHP * 10;
            AteamHPmax = AteamHP;
            EteamHP = EteamHP * 10;
            EteamHPmax = EteamHP;
            //go to decide action
            battlephase = 20;
            break;
        
        case 3:
            //increase round by 1 
            battleround = battleround + 1;
            //reset SHD and ENTflat
            for(int x = 0 ; x< 5; x++){
                Aaction[x] = 0;
                Eaction[x] = 0;
            }

            battlephase = 5;
            break;
            
        case 4:
            break;
        
        case 5:
            //enforce ally action 1
            actionEnforce(gameState.weapon[action[0]], 0);
            updateValues(gameState.weapon[action[0]], 0);
            previous = 5;
            battleturn = 0;
            battlephase = 24;
            break;
            
        case 6:
            //enforce enemy action 1
            actionEnforce(enemyweapon[0], 1);
            updateValues(enemyweapon[0], 1);
            previous = 6;
            battleturn = 0;
            battlephase = 25;
            break;
        
        case 7:
            //enforce ally action 2
            actionEnforce(gameState.weapon[action[1]], 0);
            updateValues(gameState.weapon[action[1]], 0);
            previous = 7;
            battleturn = 1;
            battlephase = 24;
            break;
            
        case 8:
            //enforce enemy action 2
            actionEnforce(enemyweapon[1], 1);
            updateValues(enemyweapon[1], 1);
            previous = 8;
            battleturn = 1;
            battlephase = 25;
            break;
            
        case 9:
            //enforce ally action 3
            actionEnforce(gameState.weapon[action[2]], 0);
            updateValues(gameState.weapon[action[2]], 0);
            previous = 9;
            battleturn = 2;
            battlephase = 24;
            break;
            
        case 10:
            //enforce enemy action 3
            actionEnforce(enemyweapon[2], 1);
            updateValues(enemyweapon[2], 1);
            //to next round
            previous = 10;
            battleturn = 2;
            battlephase = 25;
            break;
            
        case 11: 
            //reset framerate
            arduboy.setFrameRate(20);
            //timeout
            if (checkEnd() == 1)
                battlephase = 30;
            //win
            if (checkEnd() == 2){
                arduboy.print(F("Won!"));
                //max level 70
                if (gameState.playerlevel < 70){
                  //gain exp
                  gameState.playerexp = gameState.playerexp + enemylevel;
                  //check level up
                  if(gameState.playerexp >= (gameState.playerlevel * 10)){
                      gameState.playerlevel++;
                    //reset exp
                    gameState.playerexp = 0;
                  }
                }
                //save here
                FX::saveGameState(gameState);  
                //to win screen
                battlephase = 26;
            }
            //lost (0HP)
            if (checkEnd() == 3)
                battlephase = 29;

            break;
        
        //enemy appear animation
        case 12:
          arduboy.clear();
          drawbattleBg(1);          
          if (area != 39 && area != 59 && area != 79 && area != 100){
            appearAnimation1(1);
          }
          else
            appearAnimation1(2);

          arduboy.setTextColor(WHITE);
          arduboy.setTextBackground(BLACK);
          arduboy.setCursor(98, 39);
          arduboy.print(F("Enemy"));
          arduboy.setCursor(98, 49);
          arduboy.print(F("rose!"));
          if (arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3))
            battlephase = 13;
          break;
        case 13:
          if (arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3))
            battlephase = 14;
          drawbattleBg(1);
          if (area != 39 && area != 59 && area != 79 && area != 100){
            appearAnimation2(1);
          }
          else
            appearAnimation2(2);
          arduboy.setCursor(98, 39);
          arduboy.print(F("Est."));
          arduboy.setCursor(98, 49);
          arduboy.print(F("Lv"));
          arduboy.print(enemylevel);
          break;
        //ally appear animation
        case 14:
          drawbattleBg(0);
          appearAnimation1(0);
          if (arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3))
            battlephase = 15;
          break;
        case 15:
          if (arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3))
            battlephase = 16;
          drawbattleBg(0);
          appearAnimation2(0);
          arduboy.setCursor(1, 39);
          arduboy.print(F("Stand"));
          arduboy.setCursor(1, 49);
          arduboy.print(F("firm!"));
          break;
        
        //battle menu
        case 16:
          battleTeamplan();
          // Change to battle namecard
          if (menu == 2 && arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
             battlephase = 17;
          }
          // Change to battle team
          if (menu == 4 && arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
             battlephase = 19;
          }
          break;
        //Change unit
        case 17:
          changeUnit(2);
          namecardDisplay(unit, 1);
           //return to battle menu
          if (arduboy.pressed(B_BUTTON)){
            battlephase = 16;
          }
          break;
        //change unit together with weapon : confirm
        case 18:
          changeTeam();
          confirmTeam (2);
          //Back to battle namecard 
          if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
            battlephase = 17;
          }
          break;
        //confirm battle team
        case 19:
          teamDisplay();
          FX::drawBitmap(0, 32, Text, 9, dbmNormal);
          if (arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3)){
              //init team stats
                battlephase = 2;
          }
           //return to battle menu
          if (arduboy.pressed(B_BUTTON)&& arduboy.everyXFrames(3)){
              battlephase = 16;
          }
          break;

        //decide action 1  
        case 20:
          arduboy.setFrameRate(20);
          arduboy.setTextColor(WHITE);
          arduboy.setTextBackground(BLACK);
          drawbattleBg(0);
          appearAnimation2(3);
          arduboy.setCursor(4, 39);
          arduboy.print(F("1st"));
          arduboy.setCursor(4, 49);
          arduboy.print(F("move"));
          rollAction(32, 32, 0);
          //give up
          if(arduboy.pressed(B_BUTTON) && arduboy.everyXFrames(3)){
              battlephase = 31;
          }
          break;
        //decide action 2
        case 21:
          drawbattleBg(0);
          appearAnimation2(3);
          drawAction (32, 32, gameState.weapon[action[0]]);
          arduboy.setCursor(4, 39);
          arduboy.print(F("2nd"));
          arduboy.setCursor(4, 49);
          arduboy.print(F("move"));
          rollAction(64, 32, 1);
          //give up
          if(arduboy.pressed(B_BUTTON) && arduboy.everyXFrames(3)){
              battlephase = 31;
          }
          break;
        //decide action 3
        case 22:
          drawbattleBg(0);
          appearAnimation2(3);
          drawAction  (32, 32, gameState.weapon[action[0]]);
          drawAction  (64, 32, gameState.weapon[action[1]]);
          arduboy.setCursor(4, 39);
          arduboy.print(F("3nd"));
          arduboy.setCursor(4, 49);
          arduboy.print(F("move"));
          rollAction(96, 32, 2);
          //give up
          if(arduboy.pressed(B_BUTTON) && arduboy.everyXFrames(3)){
              battlephase = 31;
          }
          break;
        //confirm actions
        case 23:
          drawbattleBg(0);
          appearAnimation2(3);
          FX::drawBitmap(0, 32, Text, 10, dbmNormal);
          drawAction  (32, 32, gameState.weapon[action[0]]);
          drawAction  (64, 32, gameState.weapon[action[1]]);
          drawAction  (96, 32, gameState.weapon[action[2]]);
          if (arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(3)){
            battlephase = 3;
          }
          break;

        //for animations===============================================
        //ally center
        case 24:
          if (area != 39 && area != 59 && area != 79 && area != 100){
            battleAnimation(action[battleturn], 0);
          }
          else
            battleAnimation(action[battleturn], 2);

          if (arduboy.pressed(A_BUTTON)&& arduboy.everyXFrames(4)){
            timecntT = 0;   
            if(checkEnd() == 0){
              battlephase = previous + 1;
            }
            else
              battlephase = 11;

          }
          break;
        //enemy center
        case 25:
          if (area != 39 && area != 59 && area != 79 && area != 100){
            battleAnimation(battleturn, 1);
          }
          else
            battleAnimation(battleturn, 3);

          if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(4)){
            timecntT = 0;   
            if(checkEnd() == 0){
              if(battleturn == 2){
                battlephase = 20;
              }
              else
                battlephase = previous + 1;
            }
            else
              battlephase = 11;
          }
          break;

        //win screen
        case 26:
          battleEnd(1);
          if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
            //if drop weapon (5%) or 100% drop if defeated boss
            if(roll(10) == 1 || area == 39 || area == 59 || area == 79 || area == 100){
              battlephase = 27;
            }
            else
            //continue game
              battlephase = 28;
          }
          break;
        //drop screen
        case 27:
          //unlock the weapon held by second enemy
          gameState.weaponlock[enemyweapon[2]] = 1;
          battleEnd(4);
          if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
            //save here
            FX::saveGameState(gameState);  
            battlephase = 28;
          }
          break;
        //continue?
        case 28:
          battleEnd(5);
          //yes
          if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
            //next area
            area++;
            //back to menu if defeated final boss
            if (area > 100){
              gamemode = 4;
            }
            else
              battlephase = 32;
          }
          //no 
          if (arduboy.pressed(B_BUTTON) && arduboy.everyXFrames(3)){
            //back to menu
            gamemode = 4;
          }
          break;
        
        //Lost (0 HP)
        case 29:
          battleEnd(2);
          if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
            //back to menu
              gamemode = 4;
          }
          break;

        //Lost (Timeout)
        case 30:
          battleEnd(3);
          if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
            //back to menu
              gamemode = 4;
          }
          break;

        //give up
        case 31:
          battleEnd(6);
          if (arduboy.pressed(A_BUTTON) && arduboy.everyXFrames(3)){
            //back to menu
              gamemode = 4;
          }
          if (arduboy.pressed(B_BUTTON) && arduboy.everyXFrames(3)){
            //back to roll action
              battlephase = 20;
          }
          break;

        //map animation
        case 32:
          map();

          //draw template
          FX::drawBitmap(0, 0, Text, 14, dbmNormal);
          FX::drawBitmap(0, 32, Text, 8, dbmNormal);
          arduboy.setTextColor(WHITE);
          arduboy.setTextBackground(BLACK);
          arduboy.setCursor(5, 5);
          arduboy.print(F("Area"));
          arduboy.setCursor(10, 17);
          arduboy.print(area);
          arduboy.fillRect(0, 32, 128, 1, BLACK);

          if(stepcnt >= random(55, 128)){
            battlephase = 1;
          }
          break;

    }
        
}

//map background animation
void map(){
  arduboy.fillScreen(WHITE);


  //change to next random map 
  if(mapoffset >= 96){
    maphold[0] = maphold[1];
    maphold[1] = random(3, 7);
    //reset offset
    mapoffset = 0;
  }

  //maps 
  FX::drawBitmap(32 + mapoffset, 0, Background, maphold[0], dbmNormal);
  FX::drawBitmap(-64 + mapoffset, 0, Background, maphold[1], dbmNormal);
  //idle units
  drawIdle(32, 32, gameState.ally[0], 1);
  drawIdle(64, 32, gameState.ally[1], 1);
  drawIdle(96, 32, gameState.ally[2], 1);
  //move animation
  if(arduboy.pressed(LEFT_BUTTON)){
    arduboy.fillRect(32, 32, 96, 32, WHITE);
    drawWalk(32, 32, gameState.ally[0]);
    drawWalk(64, 32, gameState.ally[1]);
    drawWalk(96, 32, gameState.ally[2]);
    mapoffset = mapoffset + 1;
    stepcnt++;
  }

}

//battle end screen 1: won, 2: 0HP, 3: timeout, 4: drop loot, 5: continue?, 6: give up?
void battleEnd(int mode){
  arduboy.fillScreen(WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.setTextBackground(WHITE);
  arduboy.setCursor(36, 36);
  //win screen
  if (mode == 1 || mode == 5 || mode == 7){
    FX::drawBitmap(0, 32, Text, 11, dbmNormal);
    drawHappy(32, 0, gameState.ally[0], 1);
    drawHappy(64, 0, gameState.ally[1], 1);
    drawHappy(96, 0, gameState.ally[2], 1);
    drawHappy(32, 32, gameState.ally[0], 2);
    drawHappy(64, 32, gameState.ally[1], 2);
    drawHappy(96, 32, gameState.ally[2], 2);
    //continue?
    if (mode == 5){
      FX::drawBitmap(32, 32, Background, 2, dbmNormal);
      arduboy.print(F("Next area?"));
      arduboy.setCursor(36, 48);
      arduboy.print(F("[A] yes [B] no"));
    }
  }

  //lost screen
  if (mode == 2 || mode == 3 || mode == 6){
    FX::drawBitmap(0, 32, Text, 12, dbmNormal);
    drawSad(32, 0, gameState.ally[0], 1);
    drawSad(64, 0, gameState.ally[1], 1);
    drawSad(96, 0, gameState.ally[2], 1);
    drawSad(32, 32, gameState.ally[0], 2);
    drawSad(64, 32, gameState.ally[1], 2);
    drawSad(96, 32, gameState.ally[2], 2);
    if (mode == 3){
        FX::drawBitmap(32, 32, Background, 2, dbmNormal);
        arduboy.print(F("Time out!"));
    }
    if (mode == 6){
        FX::drawBitmap(0, 32, Text, 9, dbmNormal);
        FX::drawBitmap(32, 32, Background, 2, dbmNormal);
        arduboy.print(F("Retire?"));
        arduboy.setCursor(36, 48);
        arduboy.print(F("[A] yes [B] no"));
    }
  }

  //drop loop
  if(mode == 4){
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);
    FX::drawBitmap(0, 32, Action, 8, dbmNormal);
    arduboy.print(F("Rare Drop !!"));
    //show drop
    FX::drawBitmap(48, 0, weapons, enemyweapon[2], dbmNormal);
  }


}

//Battle animations mode 0: ally-enemy, 1: enemy-ally, 2: ally-boss, 3: boss-ally
void battleAnimation(int slot, int mode){
  arduboy.setCursor(32, 48);
  arduboy.print(timecntT);
  arduboy.setFrameRate(40);
  if(arduboy.everyXFrames(1) == true){
        timecntT++;
  }
  arduboy.fillScreen(WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.setTextBackground(WHITE);

  //ALLY=========================================================================================
  if (mode == 0 || mode == 2){   
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);

    //allyATK 
    if(callAction(gameState.weapon[slot]) == 1){
      drawAngry(0, 32, gameState.ally[slot], 2);
      //print action
      if (timecntT <= 228){
      drawName (36, 36, gameState.ally[slot]);
      arduboy.setCursor(36, 46);
      arduboy.print(F("attack!"));
      }
      else{
      arduboy.setCursor(36, 36);
      arduboy.print(Aaction[0]);
      arduboy.setCursor(36, 48);
      arduboy.print(F("damage!"));
      }


      //appear
      if(timecntT <= 80){
        drawIdle(128 - timecntT, 0, gameState.ally[slot], 1);
      }
      //fight1
      if(timecntT >= 81 && timecntT <= 84){
        FX::drawBitmap(48, 0, callSprite1(gameState.ally[slot]), 8, dbmMasked);
      }
      //fight2
      if(timecntT >= 85 && timecntT <= 93){
        FX::drawBitmap(48, 0, callSprite1(gameState.ally[slot]), 9, dbmMasked);
      }
      //nth
      //enemy appear
      if(timecntT >= 94 && timecntT <= 221){
        //vs normal enemy
        if (mode == 0){
          FX::drawBitmap((-48 + timecntT - 94), 0, callSprite1(enemy[0]), 0, dbmMasked);
          FX::drawBitmap((-80 + timecntT - 94), 0, callSprite1(enemy[1]), 0, dbmMasked);
          FX::drawBitmap((-112 + timecntT - 94), 0, callSprite1(enemy[2]), 0, dbmMasked);
        }
        //vs boss
        if (mode == 2){
          FX::drawBitmap((-111 + timecntT - 94), 0, callSprite1(enemy[0]), 0, dbmMasked);
        }

      }
      //ATk first 2 frame
      if(timecntT >= 220 && timecntT <= 221){
        FX::drawBitmap(0, 0, ATKanime, (timecntT - 220) , dbmMasked);
      }
      //enemy hit
      if(timecntT >= 222 && timecntT <= 251){
        //vs normal enemy
        if (mode == 0){
          FX::drawBitmap(80, 0, callSprite1(enemy[0]), 6, dbmMasked);
          FX::drawBitmap(48, 0, callSprite1(enemy[1]), 6, dbmMasked);
          FX::drawBitmap(16, 0, callSprite1(enemy[2]), 6, dbmMasked);
        }
        //vs boss
        if (mode == 2){
          FX::drawBitmap(17, 0, callSprite1(enemy[0]), 2, dbmMasked);
        }
      }
      //ATk rest of frames
      if(timecntT >= 222 && timecntT <= 228){
        FX::drawBitmap(0, 0, ATKanime, (timecntT - 222), dbmMasked);
      }

      //fix
      if(timecntT >= 228){
        timecntT = 228;
      }
    }//allyATK

    //allySHD
    if(callAction(gameState.weapon[slot]) == 2){
      drawAngry(0, 32, gameState.ally[slot], 2);
      //print action
      if (timecntT <= 50){
      drawName (36, 36, gameState.ally[slot]);
      arduboy.setCursor(36, 46);
      arduboy.print(F("shield!"));
      }
      else{
      arduboy.setCursor(36, 36);
      arduboy.print(Aaction[1]);
      arduboy.setCursor(36, 48);
      arduboy.print(F("mitigation!"));
      }

      //standby 
      if(timecntT <= 35){
        drawIdle(48, 0, gameState.ally[slot], 1);
      }
      //fight1
      if(timecntT >= 36){
        FX::drawBitmap(48, 0, callSprite1(gameState.ally[slot]), 8, dbmMasked);
      }

      //shield appear 
      if(timecntT <= 40){
        FX::drawBitmap(0, -40 + timecntT, SHDanime, 0 , dbmMasked);
      }
      //shield animation
      if(timecntT >= 41 && timecntT <= 55){
        FX::drawBitmap(0, 0, SHDanime, 1 + (timecntT - 41) , dbmMasked);
      }

      //fix
      if(timecntT >= 55){
        FX::drawBitmap(0, 0, SHDanime, 0 , dbmMasked);
        timecntT = 55;
      }


    }//allySHD

    //allyHEAL
    if(callAction(gameState.weapon[slot]) == 3){
      //print action


      //fight1
      if(timecntT <= 9){
        FX::drawBitmap(48, 0, callSprite1(gameState.ally[slot]), 8, dbmMasked);
      }
      //fight2
      if(timecntT >= 10){
        FX::drawBitmap(48, 0, callSprite1(gameState.ally[slot]), 9, dbmMasked);
      }

      //base animation1
      if(timecntT >= 0 && timecntT <= 5){
        FX::drawBitmap(0, 0, HEALanime, timecntT, dbmMasked);
      }
      if(timecntT >= 6 && timecntT <= 10){
        FX::drawBitmap(0, 0, HEALanime, (timecntT - 5), dbmMasked);
      }
      if(timecntT >= 11 && timecntT <= 17){
        FX::drawBitmap(0, 0, HEALanime, (-6 + timecntT), dbmMasked);
      }
      //star animation
      if(timecntT >= 18){
        FX::drawBitmap(0, 32 - (timecntT - 18), HEALanimeS, 0, dbmMasked);
      }

      drawHappy(0, 32, gameState.ally[slot], 2);
      FX::drawBitmap(32, 32, Background, 2, dbmNormal);
      if (timecntT <= 50){
      drawName (36, 36, gameState.ally[slot]);
      arduboy.setCursor(36, 46);
      arduboy.print(F("heal!"));
      }
      else{
      arduboy.setCursor(36, 36);
      arduboy.print(Aaction[3]);
      arduboy.setCursor(36, 48);
      arduboy.print(F("recovery!"));
      }

      //fix
      if(timecntT >= 120){
        timecntT = 120;
      }
    }//allyHEAL

    //allyENT
    if(callAction(gameState.weapon[slot]) == 4){
      //print action
      arduboy.setFrameRate(20);

      //fight1
      if(timecntT <= 10){
        FX::drawBitmap(48, 0, callSprite1(gameState.ally[slot]), 8, dbmMasked);
      }
      //idle1
      if(timecntT >= 11){
        FX::drawBitmap(48, 0, callSprite1(gameState.ally[slot]), 0, dbmMasked);
      }

      //base animation1
      if(timecntT >= 0 && timecntT <= 20){
        FX::drawBitmap(0, 0, ENTanime, timecntT, dbmMasked);
      }

      drawHappy(0, 32, gameState.ally[slot], 2);
      FX::drawBitmap(32, 32, Background, 2, dbmNormal);
      if (timecntT <= 20){
      drawName (36, 36, gameState.ally[slot]);
      arduboy.setCursor(36, 46);
      arduboy.print(F("enchant!"));
      }
      else{
      arduboy.setCursor(36, 36);
      arduboy.print(Aaction[4]);
      arduboy.setCursor(36, 48);
      arduboy.print(F("buff!"));
      }

      //fix
      if(timecntT >= 21){
        timecntT = 21;
      }
    }//allyENT

  }//Ally

 //ENEMY============================================================================================
  if (mode == 1 or mode == 3){   
    FX::drawBitmap(0, 32, Background, 2, dbmNormal);

    //enemyATK
    if(callAction(enemyweapon[slot]) == 1){
      //draw portrait
      if(mode == 1)
          drawAngry(96, 32, enemy[slot], 2);
      if(mode == 3)
          drawAngry(96, 32, enemy[slot], 3);

      arduboy.setTextColor(BLACK);
      arduboy.setTextBackground(WHITE);
      if (timecntT <= 228){
      drawName (4, 36, enemy[slot]);
      arduboy.setCursor(4, 46);
      arduboy.print(F("attack!"));
      }
      else{
      arduboy.setCursor(4, 36);
      arduboy.print(Eaction[0]);
      arduboy.setCursor(4, 48);
      arduboy.print(F("damage!"));
      }
      //appear
      if(timecntT <= 80){
        //by norm enemy
        if (mode == 1){
            drawIdle(-32 + timecntT, 0, enemy[slot], 1);
        }
        //by boss
        if(mode == 3){
            drawIdle(-48 + timecntT, 0, enemy[slot], 1);
        }
      }
      //fight1
      if(timecntT >= 81 && timecntT <= 84){
        //by normal enemy
        if (mode == 1){
            FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 8, dbmMasked);
        }
        //by boss
        if (mode == 3){
            FX::drawBitmap(32, 0, callSprite1(enemy[slot]), 3, dbmMasked);
        }
      }
      //fight2
      if(timecntT >= 85 && timecntT <= 93){
        //by norm enemy
        if (mode == 1){
            FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 9, dbmMasked);
        }
        //by boss
        if (mode == 3){
            FX::drawBitmap(32, 0, callSprite1(enemy[slot]), 4, dbmMasked);
        }
      }
      //nth
      //ally appear
      if(timecntT >= 94 && timecntT <= 221){
          FX::drawBitmap((144 - (timecntT - 94)), 0, callSprite1(gameState.ally[0]), 0, dbmMasked);
          FX::drawBitmap((176 - (timecntT - 94)), 0, callSprite1(gameState.ally[1]), 0, dbmMasked);
          FX::drawBitmap((208 - (timecntT - 94)), 0, callSprite1(gameState.ally[2]), 0, dbmMasked);
      }
      //ATk first 2 frame
      if(timecntT >= 220 && timecntT <= 221){
        FX::drawBitmap(0, 0, ATKanimeR, (timecntT - 220) , dbmMasked);
      }
      //ally hit
      if(timecntT >= 222 && timecntT <= 251){
          FX::drawBitmap(16, 0, callSprite1(gameState.ally[0]), 6, dbmMasked);
          FX::drawBitmap(48, 0, callSprite1(gameState.ally[1]), 6, dbmMasked);
          FX::drawBitmap(80, 0, callSprite1(gameState.ally[2]), 6, dbmMasked);

      }
      //ATk rest of frames
      if(timecntT >= 222 && timecntT <= 228){
        FX::drawBitmap(0, 0, ATKanimeR, (timecntT - 222), dbmMasked);
      }

      //fix
      if(timecntT >= 228){
        timecntT = 228;
      }
      
    }//enemyATK

    //enemySHD
    if(callAction(enemyweapon[slot]) == 2){
      //draw portrait
      if(mode == 1)
          drawAngry(96, 32, enemy[slot], 2);
      if(mode == 3)
          drawAngry(96, 32, enemy[slot], 3);

      //print action
      if (timecntT <= 50){
      drawName (4, 36, enemy[slot]);
      arduboy.setCursor(4, 46);
      arduboy.print(F("shield!"));
      }
      else{
      arduboy.setCursor(4, 36);
      arduboy.print(Eaction[1]);
      arduboy.setCursor(4, 48);
      arduboy.print(F("mitigation!"));
      }

      //standby 
      if(timecntT <= 35){
        //norm enemy
        if(mode == 1){
          drawIdle(48, 0, enemy[slot], 1);
        }
        //boss
        if(mode == 3){
          drawIdle(0, 0, enemy[slot], 1);
        }

      }
      //fight1
      if(timecntT >= 36){
        //norm enemy
        if (mode == 1){
          FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 8, dbmMasked);
        }
        //boss (fight2)
        if (mode == 3){
          FX::drawBitmap(0, 0, callSprite1(enemy[slot]), 4, dbmMasked);
        }

      }

      //shield appear 
      if(timecntT <= 40){
        FX::drawBitmap(0, -40 + timecntT, SHDanimeR, 0 , dbmMasked);
      }
      //shield animation
      if(timecntT >= 41 && timecntT <= 55){
        FX::drawBitmap(0, 0, SHDanimeR, 1 + (timecntT - 41) , dbmMasked);
      }

      //fix
      if(timecntT >= 55){
        FX::drawBitmap(0, 0, SHDanimeR, 0 , dbmMasked);
        timecntT = 55;
      }


    }//enemySHD

    //enemyHEAL
    if(callAction(enemyweapon[slot]) == 3){

      //fight1
      if(timecntT >= 0 && timecntT <= 10){
        if(mode == 1){
          FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 8, dbmMasked);
        }
        if(mode == 3){
          FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 0, dbmMasked);
        }

      }
      //fight2
      if(timecntT >= 10){
        if(mode == 1){
          FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 9, dbmMasked);
        }
        if(mode == 3){
          FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 4, dbmMasked);
        }

      }

      //base animation1
      if(timecntT <= 5){
        FX::drawBitmap(0, 0, HEALanimeR, timecntT, dbmMasked);
      }
      if(timecntT >= 6 && timecntT <= 10){
        FX::drawBitmap(0, 0, HEALanimeR, (timecntT - 6), dbmMasked);
      }
      if(timecntT >= 11 && timecntT <= 17){
        FX::drawBitmap(0, 0, HEALanimeR, (-6 + timecntT), dbmMasked);
      }
      //star animation
      if(timecntT >= 18){
        FX::drawBitmap(0, 32 - (timecntT - 18), HEALanimeSR, 0, dbmMasked);
      }
      //norm enemy
      if (mode == 1){
        drawHappy(0, 32, enemy[slot], 2);
      }
      //boss
      if (mode == 3){
        drawHappy(0, 32, enemy[slot], 3);
      }
      FX::drawBitmap(32, 32, Background, 2, dbmNormal);
      //print action
      if (timecntT <= 50){
      drawName (36, 36, enemy[slot]);
      arduboy.setCursor(36, 46);
      arduboy.print(F("heal!"));
      }
      else{
      arduboy.setCursor(36, 36);
      arduboy.print(Eaction[3]);
      arduboy.setCursor(36, 48);
      arduboy.print(F("recovery!"));
      }
      //fix
      if(timecntT >= 120){
        timecntT = 120;
      }
    }//enemyHEAL

   //enemyENT
    if(callAction(enemyweapon[slot]) == 4){
      //print action
      arduboy.setFrameRate(20);

      FX::drawBitmap(48, 0, callSprite1(enemy[slot]), 0, dbmMasked);

      //base animation1
      if(timecntT >= 0 && timecntT <= 20){
        FX::drawBitmap(0, 0, ENTanime, timecntT, dbmMasked);
      }

      //norm enemy
      if (mode == 1){
        drawHappy(0, 32, enemy[slot], 2);
      }
      //boss
      if (mode == 3){
        drawHappy(0, 32, enemy[slot], 3);
      }
      FX::drawBitmap(32, 32, Background, 2, dbmNormal);
      if (timecntT <= 20){
      drawName (36, 36, enemy[slot]);
      arduboy.setCursor(36, 46);
      arduboy.print(F("enchant!"));
      }
      else{
      arduboy.setCursor(36, 36);
      arduboy.print(Eaction[4]);
      arduboy.setCursor(36, 48);
      arduboy.print(F("buff!"));
      }

      //fix
      if(timecntT >= 21){
        timecntT = 21;
      }
    }//enemyENT



  }//enemy mode


}
//let player decide 3 ally actions
void rollAction(int x, int y, int pos){
  static int cnt = 0;
  if(arduboy.everyXFrames(7) == true){
    cnt++;
  }
  if (cnt > 2){
    cnt = 0;
  }
  drawAction(x, y, gameState.weapon[cnt]);
  if (arduboy.pressed(DOWN_BUTTON)&& arduboy.everyXFrames(3)){
    action[pos] = cnt;
    battlephase++;
  }
}


//Finalize team positions
void battleTeamplan(){
  //only show 2, 4 of menu  
  if(menu != 2 && menu != 4){
    menu = 2;
  }
  if (arduboy.pressed(RIGHT_BUTTON) && arduboy.everyXFrames(3))
      menu = 4;
  if (arduboy.pressed(LEFT_BUTTON) && arduboy.everyXFrames(3))
      menu = 2;

  menuDisplay(menu, 1);

}


//draw background of battle 
void drawbattleBg(int mode){
  arduboy.fillRect(0, 0, 128, 32, WHITE);
  //ally
  if (mode == 0){
    FX::drawBitmap(0, 32, Text, 14, dbmNormal);
    FX::drawBitmap(32, 32, Background, 2, dbmNormal);
    FX::drawBitmap(0, 0, Action, 7, dbmNormal);
  }
  //enemy
  if (mode == 1){
    FX::drawBitmap(96, 32, Text, 14, dbmNormal);
    FX::drawBitmap(0, 32, Background, 2, dbmNormal);
    FX::drawBitmap(96, 0, Action, 9, dbmNormal);

  }
}
//appear animation
void appearAnimation1(int mode){
  if(arduboy.everyXFrames(1) == true && timecntT <= 128){
    timecntT = timecntT + 1;
  }
  //ally
  if (mode == 0){
    drawWalk((160 - timecntT), 0, gameState.ally[0]);
    drawWalk((192 - timecntT), 0, gameState.ally[1]);
    drawWalk((224 - timecntT), 0, gameState.ally[2]);
    drawIdle(32, 32, gameState.ally[0], 2);
    drawIdle(64, 32, gameState.ally[1], 2);
    drawIdle(96, 32, gameState.ally[2], 2);
  }
  //enemy
  if (mode == 1){
    drawWalk((-64 + timecntT), 0, enemy[0]);
    drawWalk((-96 + timecntT), 0, enemy[1]);
    drawWalk((-128 + timecntT), 0, enemy[2]);
    drawIdle(64, 32, enemy[0], 2);
    drawIdle(32, 32, enemy[1], 2);
    drawIdle(0, 32, enemy[2], 2);
  }
  //boss
  if (mode == 2){
    arduboy.fillRect(0, 32, 96, 32, BLACK);
    drawIdle((-111 + timecntT), 0, enemy[0], 1);
    drawIdle(32, 32, enemy[0], 2);
  }

  if(timecntT >= 128){
    timecntT = 0;
    if(battlephase == 12){
      battlephase = 13;
    }
    if(battlephase == 14){
      battlephase = 15;
    }
  }

}
void appearAnimation2(int mode){
    //ally2
  if (mode == 0){
    drawAngry(32, 0, gameState.ally[0], 1);
    drawAngry(64, 0, gameState.ally[1], 1);
    drawAngry(96, 0, gameState.ally[2], 1);
    drawAngry(32, 32, gameState.ally[0], 2);
    drawAngry(64, 32, gameState.ally[1], 2);
    drawAngry(96, 32, gameState.ally[2], 2);
  }
  //enemy2
  if (mode == 1){
    drawAngry(64, 0, enemy[0], 1);
    drawAngry(32, 0, enemy[1], 1);
    drawAngry(0, 0, enemy[2], 1);
    drawAngry(64, 32, enemy[0], 2);
    drawAngry(32, 32, enemy[1], 2);
    drawAngry(0, 32, enemy[2], 2);
  }
  //boss
  if (mode == 2){
    arduboy.fillRect(0, 32, 96, 32, BLACK);
    drawIdle(17, 0, enemy[0], 1);
    drawIdle(32, 32, enemy[0], 2);
  }
  //ally portrait to upper row
  if(mode == 3){
    drawAngry(32, 0, gameState.ally[0], 2);
    drawAngry(64, 0, gameState.ally[1], 2);
    drawAngry(96, 0, gameState.ally[2], 2);
  }
}


//Check if end battle
uint8_t checkEnd(){
    //0: not ended
    int x = 0;
    //1: time out
    if(battleround > 20){
        x = 1;
    }

    //2: won
    if(EteamHP <= 0){
        x = 2;
    }

    //3: lost
    if(AteamHP <= 0){
        x = 3;
    }

    return x;
    
}
//enforce action
void actionEnforce(int weaponcode, int mode){
    //ally
    if (mode == 0){
        //ATK
        if (callAction(weaponcode) == 1)
            Aaction[0] = ATK(weaponcode, 0);
        //SHD
        if (callAction(weaponcode) == 2){
            Aaction[1] = SHDflat(weaponcode, 0);
        }
        //HEAL
        if (callAction(weaponcode) == 3){
            Aaction[3] = HEAL(weaponcode, 0);
        }
        //ENT
        if (callAction(weaponcode) == 4){
            Aaction[4] = ENTflat(weaponcode, 0);
        }
    }
    
    //enemy
      if (mode == 1){
        //ATK
        if (callAction(weaponcode) == 1)
            Eaction[0] = ATK(weaponcode, 1);
        //SHD
        if (callAction(weaponcode) == 2){
            Eaction[1] = SHDflat(weaponcode, 1);
        }
        //HEAL
        if (callAction(weaponcode) == 3){
            Eaction[3] = HEAL(weaponcode, 1);
        }
        //ENT
        if (callAction(weaponcode) == 4){
            Eaction[4] = ENTflat(weaponcode, 1);
        }
    }

}

//update battle values

void updateValues(int weaponcode, int mode){
    uint16_t dmg;
    uint16_t heal;
    
    //ally => enemy
    if (mode == 0){

        //register heal to self
        if (callAction(weaponcode) == 3){
          heal = (Aaction[3] + Aaction[4]);
          AteamHP = AteamHP + heal;
          Aaction[3] = heal;
          if (AteamHP > AteamHPmax){
            AteamHP = AteamHPmax;
          }
        }

        //register dmg to opp
        if (callAction(weaponcode) == 1){
          if(Aaction[0] > Eaction[1]){
            dmg = ((Aaction[0] + Aaction[4]) - Eaction[1]);
          }
          else
            dmg = 1;
          EteamHP = EteamHP - dmg;
          Aaction[0] = dmg;
        }
    }
    
    //enemy => ally
    if (mode == 1){
        //register heal to self
        if (callAction(weaponcode) == 3){        
          heal = (Eaction[3] + Eaction[4]);
          EteamHP = EteamHP + heal;
          Eaction[3] = heal;        
          if (EteamHP > EteamHPmax)
          EteamHP = EteamHPmax;
        }


        //register dmg to opp
        if (callAction(weaponcode) == 1){ 
          //cannot kill overlevel(>10) enemy
          if(Eaction[0] > Aaction[1]){       
            dmg = ((Eaction[0] + Eaction[4])- Aaction[1]);
          }
          else
            dmg = 1;

          AteamHP = AteamHP - dmg;
          Eaction[0] = dmg;
        }
    }
}
