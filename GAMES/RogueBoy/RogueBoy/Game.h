void BarrelBreak(uint8_t x,uint8_t y);

#include "Player.h"

#include "Sprites.h"
SpriteClass Objects[MAXOBJECT];

byte ONum = 0;
byte Diff = 1;
byte Timer = 255;
bool GameType = true;


void FillRandom(){
  uint8_t r = 0;
  for(uint16_t i = 0; i<MAP_SIZE; i++){
    r = rand() % 2;
    Map[i] = r;
  }
}

uint8_t GetSurround8(uint8_t x,uint8_t y,uint8_t b){
  uint8_t a = 0;
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x+1,y))&&(GetBlock(x+1,y) == b)) { a++; }
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x-1,y))&&(GetBlock(x-1,y) == b)) { a++; }
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x,y+1))&&(GetBlock(x,y+1) == b)) { a++; }
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x,y-1))&&(GetBlock(x,y-1) == b)) { a++; }
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x+1,y+1))&&(GetBlock(x+1,y+1) == b)) { a++; }
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x+1,y-1))&&(GetBlock(x+1,y-1) == b)) { a++; }
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x-1,y+1))&&(GetBlock(x-1,y+1) == b)) { a++; }
  if ((Between(0,0,MAP_WIDTH,MAP_HEIGHT,x-1,y-1))&&(GetBlock(x-1,y-1) == b)) { a++; }
  return a;
}

void DropItem(uint16_t x,uint16_t y,bool EnDrop){
  bool NewSlot = true;
  uint8_t Found = 0;
  for (byte i=0;i<ONum;i++){
    if (!(Objects[i].IsActive())){
      NewSlot = false;
      Found = i;
      break;
    }
  }
  if ((ONum == MAXOBJECT)&&(NewSlot)) return;
  if (NewSlot){
    if (ONum < MAXOBJECT){ONum++;}
    uint8_t ID = random(1,5);
    uint8_t Offs = 0;
    if ((ID!=4)&&(ID!=2)){
      switch (ID){
          case 1: Offs=12; break; //Coin
          case 3: Offs=9; break; //Jelly Filled Doughnut
          case 5: Offs=11; break; //Ham
        }
      if (!EnDrop){
      Objects[ONum-1].setSprite((x*16)+8,(y*16)+8,1,ID,Offs,true);
      }else{
      Objects[ONum-1].setSprite(x,y,1,ID,Offs,true);
      }
    }
  } else {
    uint8_t ID = random(1,5);
    uint8_t Offs = 0;
    if ((ID!=4)&&(ID!=2)){
      switch (ID){
          case 1: Offs=12; break; //Coin
          case 3: Offs=9; break; //Jelly Filled Doughnut
          case 5: Offs=11; break; //Ham
        }
      if (!EnDrop){
      Objects[Found].setSprite((x*16)+8,(y*16)+8,1,ID,Offs,true);
      }else{
      Objects[Found].setSprite(x,y,1,ID,Offs,true);
      }
    }
  }
}

Point setRandomBlock(uint8_t rep,uint8_t blk){
  bool Done = false;
  Point out;
  do{    
  for(uint8_t i = 0; i < MAP_WIDTH; i++)
      for(uint8_t j = 0; j < MAP_HEIGHT; j++){
        if ((GetBlock(i,j) == rep)&&(random(0,100)==0)) {
          SetBlock(i,j,blk);
          out.x = i;
          out.y = j;
          Done = true;
          return out;
        }
      }
  }while(Done == false);
return out;
}

Point setRandomItem(uint8_t blk){
  bool Done = false;
  Point out;
  do{    
  for(uint8_t i = 0; i < MAP_WIDTH; i++)
      for(uint8_t j = 0; j < MAP_HEIGHT; j++){
        if ((GetBlock(i,j) == 8)&&(random(0,100)==0)) {
          DropItem(i,j,false);
          out.x = i;
          out.y = j;
          Done = true;
          return out;
        }
      }
  }while(Done == false);
return (out);
}

void Init(uint16_t x, uint16_t y){
  sound.tone(NOTE_C7H,150, NOTE_REST,100, NOTE_C6,150);
  playerobj.x = x;
  playerobj.y = y;
  playerobj.d = 0;
  playerobj.Coins = 0;
  playerobj.Keys = 0;
  playerobj.Kill = 0;
  playerobj.H = 100;
  for (uint8_t i = 0; i < 6; i++){
    Bullet[i].Kill();
  }
  for (uint8_t i = 0; i < MAXOBJECT; i++){
    Objects[i].SetActive(false);
  }
  Timer = 255;
}
/*
void Flood(bool * Ref,uint8_t x,uint8_t y){
  if (Between(0,0,MAXX-1,MAXY-1,x,y)){
    if ((GetShipBlock(x,y) != 0)&&(Ref[(x+(y*MAXX))] == false)){
      Ref[(x+(y*MAXX))] = true; 
      Flood(Ref,x+1,y);
      Flood(Ref,x-1,y);
      Flood(Ref,x,y-1);
      Flood(Ref,x,y+1);
    }
  }
}*/

void GenCave(){
  uint8_t Temp[MAP_SIZE];
  MAP_HEIGHT = 15;
  MAP_WIDTH = 15;
  for(uint8_t i = 0; i < MAP_SIZE; i++) {Temp[i] = 8; Map[i] = 8;}
  FillRandom();
  for(uint8_t count = 0; count < 6; count++) {
    for(uint8_t j = 0; j < MAP_WIDTH; j++)
      for(uint8_t i = 0; i < MAP_HEIGHT; i++){
        if(((GetBlock(i,j) == 0)&&(GetSurround8(i,j,0) >= 3))||((GetBlock(i,j) == 1)&&(GetSurround8(i,j,0) >= 5)))
          Temp[GetOffset(i,j)] = 0;
        else
          Temp[GetOffset(i,j)] = 1;
    }
    for(uint16_t i = 0; i<MAP_SIZE; i++){ Map[i] = Temp[i]; }
  }
  for(uint8_t i = 0; i < MAP_WIDTH; i++)
      for(uint8_t j = 0; j < MAP_HEIGHT; j++){
        if (GetBlock(i,j) == 0) {
          SetBlock(i,j,8);
        } else {
          SetBlock(i,j,BARREL);
        }
      }
  setRandomBlock(8,DOWN_STAIRS);
  Point pl = setRandomBlock(8,UP_STAIRS);
  Init((pl.x*16)+8,(pl.y*16)+8);
  for(uint8_t count = 0; count < 6; count++) {
    setRandomItem(8);
  }
  Timer = 150;
}


void BarrelBreak(uint8_t x,uint8_t y){
  SetBlock(x,y,18);
  DropItem(x,y,false);
  sound.tone(NOTE_C3,50, NOTE_C2,50, NOTE_E3,150);
}

bool Intersect(uint16_t Min0, uint16_t Max0, uint16_t Min1, uint16_t Max1){return ((Max0 > Min1) && (Min0 < Max1));}

bool Collision(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1) {return (Intersect(x,x+8,x1,x1+8)&&Intersect(y,y+8,y1,y1+8));}




void DisplayEnviroment()
{
  int tileX = GetTileX(playerobj.x);
  int tileY = GetTileY(playerobj.y);
  int offsetX = GetTileXOffset(playerobj.x);
  int offsetY = GetTileYOffset(playerobj.y);
    
  for (int i = -5; i < 5; i++)
  {
    for(int j = -3; j < 3; j++)
    {
      uint8_t block = GetBlock(tileX + i, tileY + j);
      
      int drawX = (i * 16) + CENTERX - offsetX;
      int drawY = (j * 16) + CENTERY - offsetY;
      sprites.drawSelfMasked(drawX, drawY, BackgroundEnviroment, block);
    }
  }
}

void DisplayPlayer(){	
	const uint8_t PlayerFrames[] = { 0, 1, 0, 2 };
  if(!playerobj.Moving)
  {
    sprites.drawExternalMask(CENTERX - 4, CENTERY - 4, SpriteEnviroment,SpriteMask,0,0);
  }
  else
  {
    if(ard.everyXFrames(5))
    {
      ++playerobj.Frame;
      playerobj.Frame %= 4;
    }
    sprites.drawExternalMask(CENTERX - 4, CENTERY - 4, SpriteEnviroment,SpriteMask,PlayerFrames[playerobj.Frame],PlayerFrames[playerobj.Frame]);
  }
}

void UpdateMainMenu(){
  sprites.drawSelfMasked(0,0,Logo,0);
  if (showarrow){sprites.drawOverwrite(20,55,Arrow,0);}
  if (Audio){sprites.drawOverwrite(2,1,Speaker,1);} else {sprites.drawOverwrite(2,1,Speaker,0);}
  if (ard.justPressed(A_BUTTON)){
    gameState = GameState::LoadMap;
  }
  if (ard.justPressed(B_BUTTON)){
    Audio = !Audio;
    ard.audio.on();
    sound.tone(NOTE_C4,50);
    ard.audio.off();
    if (Audio){ard.audio.on();}else{ard.audio.off();}
  }
  if (ard.everyXFrames(30)) {
      showarrow = !showarrow;
      }
}


void TitleText(){
  if (ard.justPressed(A_BUTTON)) {sound.tone(NOTE_C4,500); Audio = true; showarrow = 0; gameState = GameState::MainMenu; }
  if (!Audio) {
      ard.setCursor(0, 0);
      uint8_t i = 0;
      while(i < 151){
          if(ard.nextFrame()){
            ard.pollButtons();

            if (ard.everyXFrames(5)){
            ard.print((char)pgm_read_byte(&TitleSequenceText[i]));
            if ((pgm_read_byte(&TitleSequenceText[i]) != 32)&&(pgm_read_byte(&TitleSequenceText[i]) != 13))
              {
              sound.tone(NOTE_C5,150);
              }
            i++;
            }

            ard.display();
            
            if (ard.justPressed(A_BUTTON)) {break;}
          }
      }
      Audio = true; 
      showarrow = 0;
  }
  else {
      for(uint8_t i = 0; i < 151; i++) {
          ard.print((char)pgm_read_byte(&TitleSequenceText[i]));
      }
      sprites.drawOverwrite(0,64-showarrow,Logo,0);
      showarrow++;
      if (showarrow >= 64){sound.tone(NOTE_D2H,150);Audio = true; showarrow = 0; gameState = GameState::MainMenu;}
  }
}

const uint8_t PROGMEM offsets[] = { 0, 12, 8, 9, 10, 11, 3, 4, 5, 6 };

void LoadMAP(byte L){

    const uint8_t * CLevel = Maps[L];

    MAP_WIDTH = pgm_read_byte(&CLevel[0]) >> 4;
    MAP_HEIGHT = pgm_read_byte(&CLevel[0]) & 0x0F;
    uint8_t UsedMap = MAP_WIDTH*MAP_HEIGHT; 
    uint16_t px = ((pgm_read_byte(&CLevel[1]) >> 4)*16)+8;
    uint16_t py = ((pgm_read_byte(&CLevel[1]) & 0x0F)*16)+8;
    //uint8_t index = OFFSET+UsedMap;
    uint8_t index = OFFSET;
    Init(px, py);


    // Read map data ..
    {

      uint8_t cursor = 0;

      while (true) {

        uint8_t data = pgm_read_byte(&CLevel[index]);
        uint8_t tile = data >> 3;
        uint8_t run = data & 0x07;

        index++;

        if (run > 0) {

          for (uint8_t x = 0; x < run; x++) {

            Map[cursor] = tile;
            cursor++;

          }

        }
        else {
        
          break;
        
        }

      }

    }
    
    ONum = pgm_read_byte(&CLevel[index++]);
    for (int i=0; i<MAXOBJECT; i++){

        uint8_t ID = 0;
        uint8_t H = 0;
        uint8_t Offs = 0;
        uint16_t px = 0;
        uint16_t py = 0;
        bool active = false;

        if (i < ONum) {
          ID = pgm_read_byte(&CLevel[index++]);
          px = ((pgm_read_byte(&CLevel[index]) >> 4) * 16) + 8;
          py = ((pgm_read_byte(&CLevel[index++]) & 0x0f) * 16) + 8;
          H = pgm_read_byte(&CLevel[index++]);
          active = true;
        }

        Objects[i].setSprite(px, py, H, ID, pgm_read_byte(&offsets[ID]),active);

    }

    ENum = pgm_read_byte(&CLevel[index++]);
    for (int i=0; i<MAXENVIROMENT; i++){
        uint8_t x1 = 0;
        uint8_t y1 = 0;
        uint8_t x2 = 0;
        uint8_t y2 = 0;
        bool active = false;
        if (i < ENum) {
          x1 = pgm_read_byte(&CLevel[index]) >> 4;
          y1 = pgm_read_byte(&CLevel[index++]) & 0x0f;
          x2 = pgm_read_byte(&CLevel[index]) >> 4;
          y2 = pgm_read_byte(&CLevel[index++]) & 0x0f;
          active = true;
        }

        Envi[i].SetEnv(x1, y1, x2, y2, active);

    }

/*

  Serial.println("-----------------------------");
  Serial.print("Map: ");
  Serial.print(L);
  Serial.print(", W: ");
  Serial.print(MAP_WIDTH);
  Serial.print(", H: ");
  Serial.println(MAP_HEIGHT);
  Serial.println("-----------------------------");

  uint8_t i = 0;
  for (int x=0; x<MAP_WIDTH; x++){
    for (int y=0; y<MAP_HEIGHT; y++){
      Serial.print(Map[i]);
      Serial.print(" ");
      i++;
    }
    Serial.println(" ");
  }

  Serial.print("Obj: ");
  Serial.print(ONum);
  Serial.print(", Env: ");
  Serial.println(ENum);

  for (int i=0; i<MAXOBJECT; i++){

    if (Objects[i].IsActive()) {
      Serial.print("obj[");
      Serial.print(i);
      Serial.print("] px:");
      Serial.print(Objects[i].GetX());
      Serial.print(", py:");
      Serial.print(Objects[i].GetY());
      Serial.print(", type:");
      Serial.print(Objects[i].GetType());
      Serial.print(", active:");
      Serial.println(Objects[i].IsActive());
    }

  }

  Serial.println(" ");
  for (int i=0; i<MAXENVIROMENT; i++){

    if (Envi[i].IsActive()) {
      Serial.print("env[");
      Serial.print(i);
      Serial.print("] x:");
      Serial.print(Envi[i].FinishX());
      Serial.print(", p:");
      Serial.print(Envi[i].FinishY());
      Serial.print(", active:");
      Serial.println(Envi[i].IsActive());
    }

  }
*/
}

void NextLevelLoad(){

  if (GameType) {
    if (Level < MAXLEVEL){
      LoadMAP(Level);
      Level++; 
      gameState = GameState::Game;
      GameType = !GameType;
    } else {gameState = GameState::WinState;}
  } else {
    gameState = GameState::Game;
    GameType = !GameType;
    GenCave();
  }
}

void UpdateObjects(){
  for (byte i=0;i<ONum;i++){
    if (Objects[i].IsActive()) { 
      bool Ud = true;
      for (byte j=0;j<ONum;j++){
        if ((j < i)&&(Objects[j].IsActive())&&(Collision(Objects[i].GetX(),Objects[i].GetY(),Objects[j].GetX(),Objects[j].GetY()))&&(Objects[j].GetType() >= 6)){
              Ud = false;
        }
      }

      if (Ud) {Objects[i].SpriteAI();}
      
      if (Collision(Objects[i].GetX(),Objects[i].GetY(),playerobj.x,playerobj.y)){

          /*
          switch(Objects[i].GetType()){
            case 1: playerobj.Coins++; Objects[i].SetActive(false); sound.noTone(); sound.tone(NOTE_C7H,150); break;
            case 2: playerobj.Coins+=5; Objects[i].SetActive(false); sound.noTone(); sound.tone(NOTE_C7H,150); break;
            case 3: playerobj.H += 5; if (playerobj.H > 100) {playerobj.H = 100;} Objects[i].SetActive(false); sound.noTone();  sound.tone(NOTE_C3H,150); break;
            case 5: playerobj.H += 10; if (playerobj.H > 100) {playerobj.H = 100;} Objects[i].SetActive(false); sound.noTone(); sound.tone(NOTE_C3H,150); break;
            case 4: playerobj.Keys++; Objects[i].SetActive(false); sound.noTone(); sound.tone(NOTE_C7H,150); break;
            case 6: if (ard.everyXFrames(5)) {playerobj.H -= 10*Diff; sound.noTone(); sound.tone(NOTE_D3,50); ard.setRGBled(255,0,0); delay(5); ard.setRGBled(0,0,0);}  break;
            case 7: if (ard.everyXFrames(5)) {playerobj.H -= 5*Diff; sound.noTone(); sound.tone(NOTE_D3,50); ard.setRGBled(255,0,0); delay(5); ard.setRGBled(0,0,0);} break;
            case 8: if (ard.everyXFrames(5)) {playerobj.H -= 2*Diff; sound.noTone(); sound.tone(NOTE_D3,50); ard.setRGBled(255,0,0); delay(5); ard.setRGBled(0,0,0);} break;
             case 9: if (ard.everyXFrames(5)) {playerobj.H -= 1*Diff; sound.noTone(); sound.tone(NOTE_D3,50); ard.setRGBled(255,0,0); delay(5); ard.setRGBled(0,0,0);} break;
          }
          */

          {
            uint16_t note = 0;
            uint16_t duration = 0;
            uint8_t type = Objects[i].GetType();

            switch (type) {
              
              case 1: 
                playerobj.Coins++; 
                Objects[i].SetActive(false); 
                note = NOTE_C7H; duration = 150;
//                sound.tone(NOTE_C7H, 150); 
                break;

              case 2: 
                playerobj.Coins+=5; 
                Objects[i].SetActive(false); 
                note = NOTE_C7H; duration = 150;
//                sound.tone(NOTE_C7H,150); 
                break;

              case 3: 
                playerobj.H += 5; 
                if (playerobj.H > 100) {playerobj.H = 100;} 
                Objects[i].SetActive(false); 
                note = NOTE_C3H; duration = 150;
//                sound.tone(NOTE_C3H,150); 
                break;

              case 4: 
                playerobj.Keys++; 
                Objects[i].SetActive(false); 
                note = NOTE_C7H; duration = 150;
//                sound.tone(NOTE_C7H,150); 
                break;

              case 5: 
                playerobj.H += 10; 
                if (playerobj.H > 100) {playerobj.H = 100;} 
                Objects[i].SetActive(false); 
                note = NOTE_C3H; duration = 150;
//                sound.tone(NOTE_C3H,150); 
                break;

              case 6 ... 9:

                if (ard.everyXFrames(5)) { 

                  switch (type) {

                    case 6:   
                      playerobj.H -= 10*Diff; 
                      break;

                    case 7: 
                      playerobj.H -= 5*Diff; 
                      break;
              
                    case 8: 
                      playerobj.H -= 2*Diff; 
                      break;

                    case 9: 
                      playerobj.H -= 1*Diff; 
                      break;

                  }

                  sound.noTone();
                  sound.tone(NOTE_D3,50); 
                  ard.setRGBled(255,0,0); 
                  delay(5);
                  ard.setRGBled(0,0,0);

                }
                break;
              }

              // Play a note?

              if (note != 0) {
                sound.noTone();
                sound.tone(note, duration); 
              }

            }

         }

      }
      
  }
  for (uint8_t j=0;j<6;j++){
    if (Bullet[j].GetActive()){
      Bullet[j].Update();
    }
  }
    for (uint8_t i=0;i<ONum;i++){
      for (uint8_t j=0;j<6;j++){
        if ((Bullet[j].GetActive()) && (Objects[i].IsActive()) && (Objects[i].GetType() >= 6) && (Collision(Objects[i].GetX()-4,Objects[i].GetY()-4,Bullet[j].GetX()-4,Bullet[j].GetY()-4))){
          Objects[i].Damage();
          Bullet[j].Kill();
          if (!Objects[i].IsActive()){
            playerobj.Kill++;
            DropItem(Objects[i].GetX(),Objects[i].GetY(),true);
            }
        }
      }
    }
}

void DisplayObjects() {
  for (uint8_t i=0;i<ONum;i++){
    if (Objects[i].IsActive()) {
    Objects[i].Display();
    }
  }
  for (uint8_t i=0;i<6;i++){
    //if (Bullet[i].GetActive()){
      Bullet[i].Display();
    //}
  }
}

const uint16_t DeathNotes[] PROGMEM = {NOTE_A3,500,NOTE_REST,100,NOTE_A3,500,NOTE_REST,100,NOTE_A3,250,NOTE_REST,100,NOTE_A3,250,NOTE_REST,50,NOTE_C3,500,NOTE_REST,100,NOTE_B3,250,NOTE_REST,100,NOTE_B3,250,NOTE_REST,100,NOTE_A3,500,NOTE_REST,100,NOTE_B3,250, TONES_END};

void Death(){
  sprites.drawOverwrite(CENTERX-7,CENTERY-7,Flowers,0);
  ard.setCursor(0,0);
  ard.print(F("SCORED:"));
  ard.println(POINTS);
  ard.print(F("GotToLevel:"));
  ard.println(Level);
  if (ard.justPressed(A_BUTTON)||ard.justPressed(B_BUTTON)){sound.noTone(); gameState = GameState::MainMenu; Level = 0; POINTS = 0; GameType = true;}
  }

void win(){
  sprites.drawSelfMasked(CENTERX-7,CENTERY-7,Ardu,0);
  ard.setCursor(0,0);
  ard.println(F("WellDone!"));
  ard.print(F("YOU SCORED:"));
  ard.println(POINTS);
  if (ard.justPressed(A_BUTTON)||ard.justPressed(B_BUTTON)){sound.noTone(); gameState = GameState::MainMenu; Level = 0; POINTS = 0;}
}

void MapEnding(){
    int padd = playerobj.Coins * 5;    
    int kadd = playerobj.Keys * 10;
    int killp = playerobj.Kill * 15;
    ard.setCursor(0,0);
    ard.print(F("Level:"));
    ard.println(Level);
    ard.print(F("Kills:"));
    ard.println(playerobj.Kill);
    ard.print(F("Coins:"));
    ard.println(playerobj.Coins);
    ard.print(F("Keys:"));
    ard.println(playerobj.Keys);
    ard.print(F("TimeBonus:"));
    ard.println((Timer/10));
    ard.print(F("Level Points:"));
    ard.println(kadd+padd+killp+(Timer/10));
    
    ard.print(F("Total Points:"));
    ard.println(POINTS+kadd+padd+killp+(Timer/10));
    
    if ((ard.everyXFrames(240)) || (ard.justPressed(A_BUTTON))) {
        gameState = GameState::LoadMap;
        POINTS += padd + kadd + killp + (Timer/10);
    }
}

void DrawHealth(){
  sprites.drawOverwrite(0, 56, HealthBar, 0);
  float Drawn = (static_cast<float>(playerobj.H) / 100)*42;
  ard.drawFastHLine(9,59,Drawn,BLACK);
}


void DrawHolding(){
  sprites.drawExternalMask(98, 43 ,slots,SlotsMask, 0,0);
  ard.setCursor(108,45);
  ard.print(playerobj.Keys);
  ard.setCursor(108,55);
  ard.print(playerobj.Coins);
}

void DrawTime(){
  sprites.drawOverwrite(90, -6 ,TimeSlot,0);
  ard.setCursor(102,1);
  ard.print(Timer);
}

void DrawHud(){
  DrawHealth();
  DrawHolding();
  DrawTime();
  ard.setCursor(0,0);
  ard.print(F("L:"));
  ard.print(Level);
  }


void UpdateGame(){
  playerobj.PlayerMovement();
  UpdateObjects();
  DisplayEnviroment();
  DisplayPlayer();
  DisplayObjects();
  DrawHud();
  if (ard.everyXFrames(15)) {Timer--;}
  if (Timer == 0){playerobj.H = 0;}
  if (playerobj.H <= 0) {sound.tones(DeathNotes); gameState = GameState::Dead;}
}
