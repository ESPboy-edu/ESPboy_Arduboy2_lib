// The Fall
// Version 1.00
// Febuairy 14th 25 to March 14th 25
// Onebit Productions
// Matthew Bryan

//#pragma once

#define ABG_IMPLEMENTATION
#define ABG_SYNC_PARK_ROW
#define ABG_TIMER4

#include "fxdta.h"
#include <ArduboyFX.h>  
#include "ArduboyG.h"

#define SPRITESU_IMPLEMENTATION
#define SPRITESU_PLUSMASK
#define SPRITESU_FX

#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "sound.h"
#include "SpritesU.hpp"
#include "fxdata.h"
#include "variables.h"
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;
decltype(a) a;

void setup()
{
  Serial.begin(115200);
  a.boot();
  a.startGray();
  a.setFrameRate(250);
  FX::begin(FX_DATA_PAGE);
  loadBlocks();
}

void loop()
{
  if(start)
  {
    //FX::enableOLED();
    a.waitForNextPlane(DARK_GRAY);
    //FX::disableOLED();
  }
  else
  {
    //FX::enableOLED();
    a.waitForNextPlane(BLACK);
    //FX::disableOLED();
  } 
  
  if (a.needsUpdate())
  {
    update();
  }

  render();
}

void update()
{
  animations();

  if (firstRun)
  {
    updateIntro();
  }

  if (!firstRun&&!start)
  {
    updateMenu();
  }

  if(start)
  updateGame();
}

void animations()
{
  anim2++;
  if (anim2 == 3){anim2=1;}

  if(start)
  {

    anim3++;
    if (anim3 == 4){anim3=1;}

    if (score==99&&winTimer==0) // detect win
    {
      winTimer=500;
    }

    if(arduboy.notPressed(A_BUTTON)||fuel==0)
    {
      if (acceleration<15) // the larger the number the slower the acceleration
      {
        acceleration++;
      }

      if (acceleration==15&&zoom>5) // limit zoom to 5 (change to 3 fror maximum)
      {
  	    zoom=zoom-2;
        playerC--;
        acceleration=0;
      }
    }

  ////////// Falling
  if(!pressedA&&fuel>0) // normal fall
  {
    if (zoom==15) megaY=megaY+3;

    if (zoom==13) megaY=megaY+4;

    if (zoom==11) megaY=megaY+5;

    if (zoom==9) megaY=megaY+6;

    if (zoom==7) megaY=megaY+7;

    if (zoom==5) megaY=megaY+8;
  }

  if(pressedA&&fuel==0) // normal fall
  {
    if (zoom==15) megaY=megaY+3;

    if (zoom==13) megaY=megaY+4;

    if (zoom==11) megaY=megaY+5;

    if (zoom==9) megaY=megaY+6;

    if (zoom==7) megaY=megaY+7;

    if (zoom==5) megaY=megaY+8;
  }

  if(pressedA&&fuel>0) // controlled fall
  {
    if (zoom==15) megaY=megaY+1;

    if (zoom==13) megaY=megaY+1;

    if (zoom==11) megaY=megaY+1;

    if (zoom==9) megaY=megaY+1;

    if (zoom==7) megaY=megaY+1;

    if (zoom==5) megaY=megaY+1;
  }

      scrollY=megaY/10;

      if (zoom == 15)
      {
        if (scrollY >= 15)
        {
          shiftValues();
        }
      }

      if (zoom == 13)
      {
        if (scrollY >= 13)
        {
          shiftValues();
        }
      }

      if (zoom == 11)
      {
        if (scrollY >= 11)
        {
          shiftValues();
        }
      }

      if (zoom == 9)
      {
        if (scrollY >= 9)
        {
          shiftValues();
        }
      }

      if (zoom == 7)
      {
        if (scrollY >= 7)
        {
          shiftValues();
        }
      }

      if (zoom == 5)
      {
        if (scrollY >= 5)
        {
          shiftValues();
        }
      }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    anim6++;
    if (anim6 == 7){anim6=1;}
 

    if(gameOverTimer>0&&anim3==2)
    {
      gameOverTimer--;
      if(gameOverTimer==0)
      resetVariables();
    }

    if(winTimer>0)
    {
      winTimer--;
      if(winTimer==0)
      resetVariables();
    }

    if(deathTimer>0)
    {
      deathTimer--;
      deathFall++;

      if(deathTimer==0)
      {
        gameOver();
      }
    }
  }
}

void updateIntro()
{
  introCountdown--;
  if (introCountdown==0)
  {
    firstRun=false;
  }
}

void updateMenu()
{
  int random=rand()%1000;
  
  if (arduboy.pressed(RIGHT_BUTTON))
	arduboy.audio.on();
	
  if (arduboy.pressed(LEFT_BUTTON))
	arduboy.audio.off();

  if (arduboy.pressed(A_BUTTON))
  {
    sound.tones(tuneFx);
	  start=true;
  }
}

void updateGame()
{
  if(gameOverTimer==0)
  {
    if(deathTimer==0)
    getInput();
    moveDebris();
    detectCollisions();
  }
}

void getInput()
{
  if (arduboy.pressed(RIGHT_BUTTON)&&screenX>-8)
  {
    right=true;

    rightInputTree();
  }

  if (arduboy.pressed(LEFT_BUTTON)&&screenX<8)
	{
    right=false;

    leftInputTree();
  }

  if (arduboy.pressed(A_BUTTON)&&fuel>0)
	{
    pressedA = true;

    fuel--;

    if(zoom<15&&anim6==3)
    {
      zoom=zoom+2;
      playerC++;
    }
  }

  if (arduboy.notPressed(A_BUTTON))
  {
    pressedA = false;

    if (fuel < 100)
    fuel++;
  }
}

void rightInputTree()
{
  if(zoom == 5)
  {
    if(scrollX>-5)
    {
      scrollX--;
    }
    else
    {
      moveRight();
    }
  }

  if(zoom == 7)
  {
    if(scrollX>-7)
    {
      scrollX--;
    }
    else
    {
      moveRight();
    }
  }

  if(zoom == 9)
  {
    if(scrollX>-9)
    {
      scrollX--;
    }
    else
    {
      moveRight();
    }
  }

  if(zoom == 11)
  {
    if(scrollX>-11)
    {
      scrollX--;
    }
    else
    {
      moveRight();
    }
  }

  if(zoom == 13)
  {
    if(scrollX>-13)
    {
      scrollX--;
    }
    else
    {
      moveRight();
    }
  }

  if(zoom == 15)
  {
    if(scrollX>-15)
    {
      scrollX--;
    }
    else
    {
      moveRight();
    }
  }
}

void leftInputTree()
{
  if(zoom == 5)
  {
    if(scrollX<5)
    {
      scrollX++;
    }
    else
    {
      moveLeft();
    }
  }

  if(zoom == 7)
  {
    if(scrollX<7)
    {
      scrollX++;
    }
    else
    {
      moveLeft();
    }
  }

  if(zoom == 9)
  {
    if(scrollX<9)
    {
      scrollX++;
    }
    else
    {
      moveLeft();
    }
  }

  if(zoom == 11)
  {
    if(scrollX<11)
    {
      scrollX++;
    }
    else
    {
      moveLeft();
    }
  }

  if(zoom == 13)
  {
    if(scrollX<13)
    {
      scrollX++;
    }
    else
    {
      moveLeft();
    }
  }

  if(zoom == 15)
  {
    if(scrollX<15)
    {
      scrollX++;
    }
    else
    {
      moveLeft();
    }
  }  
}

void detectCollisions()
{
    if(blockR[0][2]==5)
    {
      deathTimer=100;
    }

    if(blockL[0][2]==5)
    {
      deathTimer=100;
    }

    if(blockR[0][2]==4)
    {
      blockR[0][2]=0;
      score++;
      spawnDebris();
    }

    if(blockR[-1][2]==4)
    {
      blockR[-1][2]=0;
      score++;
      spawnDebris();
    }

    if(blockR[+1][2]==4)
    {
      blockR[+1][2]=0;
      score++;
      spawnDebris();
    }

    if(blockL[0][2]==4)
    {
      blockL[0][2]=0;
      score++;
      spawnDebris();
    }

    if(blockL[-1][2]==4)
    {
      blockL[-1][2]=0;
      score++;
      spawnDebris();
    }

    if(blockL[1][2]==4)
    {
      blockL[1][2]=0;
      score++;
      spawnDebris();
    }
}

void render()
{
  if(firstRun)
  {
    renderIntro();
  }
  else
  {
    if(start)
    {
      if(gameOverTimer==0&&winTimer==0)
      {
        renderGame();
      }
      else
      {
        if(gameOverTimer>0)
        renderGameOver();

        if(winTimer>0)
        renderWin();        
      }
    }
    else
    {
      renderMenu(); 
    }
  }

  if(++frameNumber==3) frameNumber=0;
}

void renderIntro()
{
  if(introCountdown>395)
  {
    SpritesU::drawPlusMaskFX(8, 24, hglitch, frameNumber);
  }

  if(introCountdown>380&&introCountdown<396)
  {
    SpritesU::drawPlusMaskFX(20, 24, aboy, frameNumber);
  }

  if(introCountdown>375&&introCountdown<381)
  {
    SpritesU::drawPlusMaskFX(20, 16, vglitch, frameNumber);
  }

  if(introCountdown>350&&introCountdown<376)
  {
    SpritesU::drawPlusMaskFX(20, 24, aboy, frameNumber);
  }

  if(introCountdown<270&&anim2==2)
  {
    yIntro++;
  }

  if(introCountdown<351)
  {
    SpritesU::drawPlusMaskFX(20, 24+yIntro, newaboy, frameNumber);
  }

  if(introCountdown==351)
  sound.tones(newaboyFx);

  if(introCountdown<200)
  {

    if(anim2==2)
    {
      if(xIntro<126)
      xIntro++;
    }

    if(xIntro==126&&flashTimer>0)
    {
      if(flashTimer==23)
      sound.tones(onebitFx);

      flashTimer--;
      SpritesU::drawPlusMaskFX(-102+xIntro, 10, wonebit, frameNumber);
      SpritesU::drawPlusMaskFX(128-xIntro, 36, wproductions, frameNumber);
    }
    else
    {
      SpritesU::drawPlusMaskFX(-102+xIntro, 10, onebit, frameNumber);
      SpritesU::drawPlusMaskFX(128-xIntro, 36, productions, frameNumber);
    }
  }
}

void renderMenu()
{
  SpritesU::drawOverwriteFX(0, 0, menuscreen, frameNumber);

  SpritesU::drawPlusMaskFX(71, 47, speaker, frameNumber);

  if(Arduboy2Audio::enabled())
  SpritesU::drawPlusMaskFX(91, 48, ssound, frameNumber);
}

void renderGame()
{
  drawBlocks();
  drawPlayer();
  drawDebris();
  drawHud();
}

void spawnDebris()
{
  if(deathTimer==0)
  {
  for (uint8_t i = 0; debrisNumber > i ; i++)
  {
    if(debrisExist[i]==0)
    {
      debrisExist[i]=rand()%50;
      debrisX[i]=63;
      debrisY[i]=15;
      debrisXvel[i]=rand()%5-3;
      debrisYvel[i]=rand()%5-3;
    }
  }
  }
}

void moveDebris()
{
  for (uint8_t i = 0; debrisNumber > i ; i++)
  {
    if(debrisExist[i]>0)
    {
      debrisX[i]=debrisX[i]+debrisXvel[i];
      debrisY[i]=debrisY[i]+debrisYvel[i];
      debrisExist[i]--;
    }
  }
}

void drawDebris()
{
  for (uint8_t i = 0; debrisNumber > i ; i++)
  {
    if(debrisExist[i]>0)
    {
      arduboy.drawPixel(debrisX[i],debrisY[i],WHITE);
      //Serial.print("i="); Serial.print(i); Serial.print("  X="); Serial.print(debrisX[i]); Serial.print("  Y="); Serial.println(debrisY[i]);
    }
  }
}

void loadBlocks()
{
  //for(uint8_t y = 0; 20 > y; y++)
  //{
  //  blockR[9][y] = 1;
  //  blockL[9][y] = 1;
  //}

  int random;

  for(uint8_t x = 0; blockW > x; x++)
  {
    for(uint8_t y = 0; blockH > y; y++)
    {
      random = rand()%20;

      if(random==10)
      blockR[x][y] = 2;
    }
  }

  for(uint8_t x = 0; blockW > x; x++)
  {
    for(uint8_t y = 0; blockH > y; y++)
    {
      random = rand()%20;

      if(random==10)
      blockL[x][y] = 2;
    }
  }

}

void moveRight()
{
  scrollX = 0;
  screenX--;

  for(uint8_t y = 0; 19 > y; y++)
  {
    blockL[0][y] = blockR[0][y];
  }

  for(uint8_t x = 0; 20 > x; x++)
  {
    for(uint8_t y = 0; 19 > y; y++)
    {
      blockR[x][y] = blockR[x+1][y];
    }
  }

  for(uint8_t x = 20; 0 < x; x--)
  {
    for(uint8_t y = 0; 19 > y; y++)
    {
      blockL[x][y] = blockL[x-1][y];
    }
  }

  for(uint8_t y = 0; 19 > y; y++)
  {
    blockL[0][y] = 0;
    blockL[20][y] = 0;
  }
}

void moveLeft()
{
  scrollX = 0;
  screenX++;

  for(uint8_t y = 0; 19 > y; y++)
  {
    blockR[0][y] = blockL[0][y];
  }

  for(uint8_t x = 20; 0 < x; x--)
  {
    for(uint8_t y = 0; 19 > y; y++)
    {
      blockR[x][y] = blockR[x-1][y];
    }
  }

  for(uint8_t x = 0; 20 > x; x++)
  {
    for(uint8_t y = 0; 19 > y; y++)
    {
      blockL[x][y] = blockL[x+1][y];
    }
  }

  for(uint8_t y = 0; 19 > y; y++)
  {
    blockR[0][y] = 0;
    blockR[20][y] = 0;
  }
}

void shiftValues() // Y access
{
  int cloudspawn;
  //int coinspawn;
  int spikespawn;

  for(uint8_t x = 0; blockW+screenX > x; x++) // spawn new right blocks
  {
    blockR[x][20] = 0; // clear block

    cloudspawn = rand()%30;
    //coinspawn = rand()%frequency/3;
    spikespawn = rand()%frequency;

    if (cloudspawn == 2)
    blockR[x][20] = 2;

    if (spikespawn == 4)
    blockL[x][20] = 4;

    if (spikespawn == 5)
    blockR[x][20] = 5;
  }

  for(uint8_t x = 0; blockW-screenX > x; x++) // spawn new left blocks
  {
    blockL[x][20] = 0; // clear block

    cloudspawn = rand()%30;
    //coinspawn = rand()%frequency/3;
    spikespawn = rand()%frequency;

    if (cloudspawn == 2)
    blockL[x][20] = 2;

    if (spikespawn == 4)
    blockL[x][20] = 4;

    if (spikespawn == 5)
    blockL[x][20] = 5;
  }

  for(uint8_t x = 0; blockW+screenX > x; x++) // shift right blocks up
  {
    for(uint8_t y = 0; 20 > y; y++)
    {
      blockR[x][y] = blockR[x][y+1];
    }
  }

  for(uint8_t x = 0; blockW-screenX > x; x++) // shift left blocks up
  {
    for(uint8_t y = 0; 20 > y; y++)
    {
      blockL[x][y] = blockL[x][y+1];
    }
  }

  blockR[9+screenX][20] = 1; // draw edges
  blockL[9-screenX][20] = 1;

  megaY = 0;
  scrollY = 0; // reset Y scroll

  if (frequency>25)
  frequency--;
}

void drawBlocks()
{

// z buffering

  //blockH=19;

  if(zoom==5) blockH=17; //
  if(zoom==7) blockH=15;
  if(zoom==9) blockH=8;
  if(zoom==11) blockH=7;
  if(zoom==13) blockH=6;
  if(zoom==15) blockH=5;

  uint8_t RxMax; // 20

  uint8_t LxMax; // 20

  if(screenX==0){RxMax=10;LxMax=10;}

  if(screenX==-1){RxMax=9;LxMax=11;}
  if(screenX==-2){RxMax=8;LxMax=12;}
  if(screenX==-3){RxMax=7;LxMax=13;}
  if(screenX==-4){RxMax=6;LxMax=14;}
  if(screenX==-5){RxMax=5;LxMax=15;}
  if(screenX==-6){RxMax=4;LxMax=16;}
  if(screenX==-7){RxMax=3;LxMax=17;}
  if(screenX==-8){RxMax=2;LxMax=18;}

  if(screenX==1){RxMax=11;LxMax=9;}
  if(screenX==2){RxMax=12;LxMax=8;}
  if(screenX==3){RxMax=13;LxMax=7;}
  if(screenX==4){RxMax=14;LxMax=6;}
  if(screenX==5){RxMax=15;LxMax=5;}
  if(screenX==6){RxMax=16;LxMax=4;}
  if(screenX==7){RxMax=17;LxMax=3;}
  if(screenX==8){RxMax=18;LxMax=2;}

  for(uint8_t x = 0; RxMax > x; x++) // draw right blocks
  {
    if(blockR[x][20] == 1) // draw edge as one image
    {
      SpritesU::drawPlusMaskFX((63-playerC)+(x*zoom)+scrollX,-scrollY, edge[zoom], frameNumber);
    }

    for(uint8_t y = 0; (blockH) > y; y++)
    {
      if (blockR[x][y] == 2)
      { 
        SpritesU::drawPlusMaskFX((63-playerC)+(x*zoom)+scrollX,((7-playerC)+y*zoom-(scrollY))-offset, cloud[zoom], frameNumber);
      }
      if (blockR[x][y] == 4)
      {
        SpritesU::drawPlusMaskFX((63-playerC)+(x*zoom)+scrollX,((7-playerC)+y*zoom-(scrollY))-offset, coin[zoom], frameNumber);
      }
      if (blockR[x][y] == 5)
      {
        SpritesU::drawPlusMaskFX((63-playerC)+(x*zoom)+scrollX,((7-playerC)+y*zoom-(scrollY))-offset, spike[zoom], frameNumber);
      }
    }
  }

  for(uint8_t x = 0; LxMax > x; x++) // draw left blocks
  {

    if(blockL[x][20] == 1) // draw edge as one image
    {
      SpritesU::drawPlusMaskFX((63-playerC)-(x*zoom)+scrollX,-scrollY, edge[zoom], frameNumber);
    }

    for(uint8_t y = 0; (blockH) > y; y++)
    {
      if (blockL[x][y] == 2)
      { 
        SpritesU::drawPlusMaskFX((63-playerC)-(x*zoom)+scrollX,((7-playerC)+y*zoom-(scrollY))-offset, cloud[zoom], frameNumber);
      }
      if (blockL[x][y] == 4)
      {
        SpritesU::drawPlusMaskFX((63-playerC)-(x*zoom)+scrollX,((7-playerC)+y*zoom-(scrollY))-offset, coin[zoom], frameNumber);
      }
      if (blockL[x][y] == 5)
      {
        SpritesU::drawPlusMaskFX((63-playerC)-(x*zoom)+scrollX,((7-playerC)+y*zoom-(scrollY))-offset, spike[zoom], frameNumber);
      }
    }
  }
}

void drawPlayer()
{
  SpritesU::drawPlusMaskFX(63 - playerC-1, ((7 - playerC)+offset)+deathFall, baloon[zoom], frameNumber);
}

void drawHud()
{
  SpritesU::drawPlusMaskFX(0, 0, tv, frameNumber);

  arduboy.fillRect(4,10+(50-fuel/2),2,fuel/2,1);

  if(arduboy.pressed(A_BUTTON)&&fuel==0&&anim3==2)
  arduboy.fillRect(3,3,4,3,1);
  tinyfont.setCursor(118,17);
  tinyfont.print(score);
}

void gameOver()
{
  gameOverTimer=64;
  sound.tones(onebitFx);
}

void renderGameOver()
{
  SpritesU::drawOverwriteFX(0, -64+gameOverTimer, gameover, frameNumber);
}

void renderWin()
{
  SpritesU::drawOverwriteFX(0, 0, youwin, frameNumber);
}

void resetVariables()
{
  sound.tones(newaboyFx);
  start=false;

  screenX = 0; // increment when doing horiziontal block flips
  scrollX = 0;
  megaY = 0;

 fuel = 100;
 frequency = 250;

 playerC = 7; // 7
 zoom = 15; // 15

  deathTimer = 0;
  winTimer = 0;
  deathFall=0;
  gameOverTimer = 0;
  score=0;

  for(uint8_t x = 0; 20 > x; x++)
  {
    for(uint8_t y = 0; 20 > y; y++)
    {
      blockR[x][y] = 0;
    }
  }

  for(uint8_t x = 0; 20 > x; x++)
  {
    for(uint8_t y = 0; 20 > y; y++)
    {
      blockL[x][y] = 0;
    }
  }

  loadBlocks();
}
