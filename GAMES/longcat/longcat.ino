#include <Arduboy2.h>
Arduboy2 arduboy;
#include "Direction.h"
#include "Bitmaps.h"
#include "Splash.h" 
#include "GameState.h"
#include "TileTypes.h"
#include "maps.h"
#include "Hero.h"
#define SAVELOCATION  EEPROM_STORAGE_SPACE_START + 100

constexpr uint8_t mapWidth = 8;
constexpr uint8_t mapHeight = 8;
constexpr uint8_t tileSize = 8;

TileType current[mapWidth*mapHeight] {TileType::Wall};
Hero hero;
uint8_t stage = 0;
uint8_t savedLevel = 0;

GameState gamestate = GameState::SplashScreen;

TileType getTile(int8_t x, int8_t y){
  if((x < 0)||(x > mapWidth)||(y < 0)||(y > mapHeight)) return TileType::Wall;
  return current[(y * mapWidth)+x];
}

void setTile(int8_t x, int8_t y,TileType tile){
  if((x < 0)||(x > mapWidth)||(y < 0)||(y > mapHeight)) return;
  current[(y * mapWidth)+x] = tile;
}

void loadEeprom(){
  EEPROM.get(SAVELOCATION, savedLevel);
  if(savedLevel >= 45) {
    savedLevel = 0;
    EEPROM.put(SAVELOCATION, savedLevel);
    EEPROM.commit();
  }
  stage = savedLevel;
}

void nextStage(){
  //arduboy.clear();
  arduboy.setCursor(70,35);
  arduboy.print("PURRFECT!");
  arduboy.display();
  arduboy.delayShort(1500);
}

void gameEnd(){
  arduboy.clear();
  arduboy.setCursor(10,30);
  arduboy.print(F("Game Completed!"));
  arduboy.display();
  arduboy.delayShort(3000);
  stage = 0;
  savedLevel = 0;
  EEPROM.put(SAVELOCATION, savedLevel);
  EEPROM.commit();
}

void renderLevelData(){
  arduboy.setCursor(70,0);
  arduboy.print(F("LVL: "));
  arduboy.print(stage+1);
  arduboy.setCursor(70,15);
  arduboy.print("B: Retry");
}

bool copyLevel(){
  for(uint8_t j=0; j<8; j++){
    //current[i]=levels[stage][i];
    byte mapValues = pgm_read_byte_near(levels[stage]+j);
    for(uint8_t i=0; i < 8; i++){
      if(bitRead(mapValues,7-i)){
        setTile(i,j,TileType::Wall);
      }
      else{
        setTile(i,j,TileType::Empty);
      }
    }
  }
  hero.x = pgm_read_byte_near(levels[stage]+8);
  hero.y = pgm_read_byte_near(levels[stage]+9);
  hero.dx = 0;
  hero.dy = 0;
  return (0);
}

bool isStuck(){
  if((getTile(hero.x+1,hero.y) != TileType::Empty)&&(getTile(hero.x-1,hero.y) != TileType::Empty)&&(getTile(hero.x,hero.y+1) != TileType::Empty)&&(getTile(hero.x,hero.y-1) != TileType::Empty))
    return true;
  return false;
}

void renderLevel(){
  for(int j=0;j<8;j++){
    for(int i=0;i<8;i++){
      uint8_t tileIndex = toTileIndex(getTile(i,j));
      Sprites::drawOverwrite(i * tileSize, j * tileSize, Cats, tileIndex);
    }
  }
}

void placeCatPiece(int8_t x, int8_t y){
  switch(hero.direction){
    case Direction::Left:
      switch(hero.lastDirection){
        case Direction::Center: setTile(x,y,TileType::Cat_Tail_Left); break;
        case Direction::Left: setTile(x,y,TileType::Cat_Body_Horizontal); break;
        case Direction::Right: break; 
        case Direction::Up: setTile(x,y,TileType::Cat_Body_Down_Left); break;
        case Direction::Down: setTile(x,y,TileType::Cat_Body_Up_Left); break;
      }
      break;
    case Direction::Right:
      switch(hero.lastDirection){
        case Direction::Center: setTile(x,y,TileType::Cat_Tail_Right); break;
        case Direction::Left: break;
        case Direction::Right: setTile(x,y,TileType::Cat_Body_Horizontal); break;
        case Direction::Up: setTile(x,y,TileType::Cat_Body_Down_Right); break;
        case Direction::Down: setTile(x,y,TileType::Cat_Body_Up_Right); break;
      }
      break;
    case Direction::Up:
       switch(hero.lastDirection){
        case Direction::Center: setTile(x,y,TileType::Cat_Tail_Up); break;
        case Direction::Left: setTile(x,y,TileType::Cat_Body_Up_Right); break;
        case Direction::Right: setTile(x,y,TileType::Cat_Body_Up_Left); break;
        case Direction::Up: setTile(x,y,TileType::Cat_Body_Vertical); break;
        case Direction::Down: break;
      }
      break;
    case Direction::Down:
      switch(hero.lastDirection){
        case Direction::Center: setTile(x,y,TileType::Cat_Tail_Down); break;
        case Direction::Left: setTile(x,y,TileType::Cat_Body_Down_Right); break;
        case Direction::Right: setTile(x,y,TileType::Cat_Body_Down_Left); break;
        case Direction::Up: break;
        case Direction::Down: setTile(x,y,TileType::Cat_Body_Vertical); break;
      }
      break;
  }
}

bool movePlayer(){
  placeCatPiece(hero.x,hero.y);
  hero.lastDirection = hero.direction;
  int nextX = constrain(hero.x+hero.dx, 0, 7);
  int nextY = constrain(hero.y+hero.dy, 0, 7);
  if(getTile(nextX,nextY) == TileType::Empty){
    hero.x =  nextX;
    hero.y =  nextY;
    return true;
  } else {
    hero.dx =  0;
    hero.dy =  0;
    return false;
  }
}

void renderPlayer(){
  TileType type;
  switch(hero.lastDirection){
    case Direction::Center: type = TileType::Cat_Head_Down; break;
    case Direction::Down: type = TileType::Cat_Head_Down; break;
    case Direction::Up: type = TileType::Cat_Head_Up; break;
    case Direction::Left: type = TileType::Cat_Head_Left; break;
    case Direction::Right: type = TileType::Cat_Head_Right; break;
  }
  Sprites::drawOverwrite(hero.x * tileSize, hero.y * tileSize, Cats, toTileIndex(type));
}

bool checkWin(){
  for(uint8_t i = 0; i < (mapWidth*mapHeight); i++){
    if(current[i] == TileType::Empty){
      return false;
    }
  }
  return true;
}

void readPlayerInput(){
  if (arduboy.pressed(UP_BUTTON)) {
    if(getTile(hero.x,hero.y-1) == TileType::Empty){
      hero.dx = 0;
      hero.dy = -1;
      hero.lastDirection = hero.direction;
      hero.direction = Direction::Up;
      gamestate = GameState::Simulate;
    }
  }
  else if (arduboy.pressed(RIGHT_BUTTON)) {
    if(getTile(hero.x+1,hero.y) == TileType::Empty){
      hero.dx = 1;
      hero.dy = 0;
      hero.lastDirection = hero.direction;
      hero.direction = Direction::Right;
      gamestate = GameState::Simulate;
    }
  }
  else if (arduboy.pressed(DOWN_BUTTON)) {
    if(getTile(hero.x,hero.y+1) == TileType::Empty){
      hero.dx = 0;
      hero.dy = 1;
      hero.lastDirection = hero.direction;
      hero.direction = Direction::Down;
      gamestate = GameState::Simulate;
    }
  }
  else if (arduboy.pressed(LEFT_BUTTON)) {
    if(getTile(hero.x-1,hero.y) == TileType::Empty){
      hero.dx = -1;
      hero.dy = 0;
      hero.lastDirection = hero.direction;
      hero.direction = Direction::Left;
      gamestate = GameState::Simulate;
    }
  }
  else if (arduboy.pressed(B_BUTTON)) {
    gamestate = GameState::LoadLevel;
  }
}

void renderPlayArea(){
  renderLevel();
  renderPlayer();
  renderLevelData();
}

void updateGameplay(){
  readPlayerInput();
  arduboy.clear();
  if(isStuck()){
    arduboy.setCursor(70,30);  
    arduboy.print(F("STUCK!"));
  }
  renderPlayArea();
  arduboy.display();
}

void updateSimulation(){
  
  if(arduboy.everyXFrames(2)){
    if(movePlayer() == false){
      gamestate = GameState::GamePlay;
    }
  }
  arduboy.clear();
  renderPlayArea();
  arduboy.display();
  
  if(checkWin()){
    if(stage == maxLevel){
      gameEnd();
      gamestate = GameState::SplashScreen;
    }else{
      nextStage();
      stage++;
      gamestate = GameState::LoadLevel;
      //if(stage >= savedLevel) {
        savedLevel = stage;
        EEPROM.put(SAVELOCATION, savedLevel);
        EEPROM.commit();
      //}
    }
  }

  
  
}

void updateLoadLevel(){
  hero.dx = 0;
  hero.dy = 0;
  hero.x = 0;
  hero.y = 0;
  hero.direction = Direction::Center;
  hero.lastDirection = Direction::Center;
  copyLevel();
  gamestate = GameState::GamePlay;
}

void updateSplashScreen(){
  arduboy.clear();
  
  arduboy.drawBitmap(10,0,splash,27,24);
  arduboy.fillRect(14,23,19,40,1);
  arduboy.setCursor(40,25);
  arduboy.setTextSize(2);
  arduboy.println("LongCat");
  arduboy.setTextSize(1);
  arduboy.setCursor(45,45);
  arduboy.print("A: Continue");
  arduboy.setCursor(45,55);
  arduboy.print("B: New game");
  arduboy.display();
  if(arduboy.pressed(A_BUTTON)){
    //stage = 0;
    gamestate = GameState::LoadLevel;
  }
  if(arduboy.pressed(B_BUTTON)){
    stage = 0;
    EEPROM.put(SAVELOCATION, 0);
    EEPROM.commit();
    gamestate = GameState::LoadLevel;
  }
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);
  arduboy.clear(); 
  loadEeprom();
   
}

void loop() {
  if(!arduboy.nextFrame()){
    return;
  }
  arduboy.pollButtons();
  switch(gamestate){
    case GameState::SplashScreen: updateSplashScreen(); break;
    case GameState::LoadLevel: updateLoadLevel(); break;
    case GameState::GamePlay: updateGameplay(); break;
    case GameState::Simulate: updateSimulation(); break;
    default: gamestate = GameState::SplashScreen;
  }
}
