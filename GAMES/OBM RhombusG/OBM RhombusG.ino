// Rhombus
// Version 1.00
// December 11th 24 to December 18th 24
// Onebit Productions
// Matthew Bryan

#pragma once

#define ABG_IMPLEMENTATION
#define ABG_SYNC_PARK_ROW
#define ABG_TIMER4

#include <ArduboyFX.h>  
#include "ArduboyG.h"

#define SPRITESU_IMPLEMENTATION
#define SPRITESU_PLUSMASK
#define SPRITESU_FX

#include "fxdta.h"
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "sound.h"
#include "SpritesU.hpp"
#include "fxdata.h"
#include "variables.h"
#include "data.h"
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

extern ArduboyGBase_Config<ABG_Mode::L4_Triplane> a;
decltype(a) a;

void setup()
{
  a.begin();
  a.startGray();
  FX::begin(FX_DATA_PAGE);
}

void loop()
{

  FX::enableOLED();
  a.waitForNextPlane(BLACK);
  FX::disableOLED();

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

  anim50++;
  if (anim50 == 51){anim50=1;}

  if(start)
  {

    anim3++;
    if (anim3 == 4){anim3=1;}

    if(anim3==3)
    {
      anim4++;
      if (anim4 == 5){anim4=1;}
    }    

    if(changeDelay>0)
    {
      changeDelay--;

      if(changeDelay==0)
      {       
        sound.tones(rainFx);

        boardXY[xChangePos][yChangePos]=2;
      }
    }

    if(moveCountdown>0)
    moveCountdown--;

    if(cycleCountdown>0)
    cycleCountdown--;

    if(levelCountdown>0&&deathTimer==0)
    levelCountdown--;

    if(flipCountdown>0&&deathTimer==0)
    flipCountdown--;

    if(stomp>0)
    stomp--;

    if(gameOverTimer>0)
    {
      gameOverTimer--;
      if(gameOverTimer==0)
      resetVariables();
    }

    if(deathTimer>0)
    {
      deathTimer--;

      if(deathTimer==0)
      {
        gameOver();
      }
    }
  }

////////////////// Menu anim
if(!start)
{

  anim25++;
  if(anim25==25){anim25=1;}

  if(anim25==1)
  {
    xOffset=0;
    yOffset=0;
  }

  xOffset--;

  if(anim2==2)
  {
    yOffset--;
  }
}
////////////////////////
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
    loadLevel();
    cycleCountdown=cycleLength;
	  start=true;
  }
}

void updateGame()
{
  if(gameOverTimer==0)
  {
    if(deathTimer==0)
    getInput();

    calcY();
    moveRain();
    progressGame();
  }
}

void getInput()
{
  if(moveCountdown==0)
  {
    if (arduboy.pressed(RIGHT_BUTTON))
    {
      if(playerX<4&&boardXY[playerX+1][playerY]!=0)
      {
	      playerX=playerX+1;
        moveCountdown=4;
      }
    }
	
    if (arduboy.pressed(LEFT_BUTTON))
    {
      if(playerX>0&&boardXY[playerX-1][playerY]!=0)
      {
	      playerX=playerX-1;
        moveCountdown=4;
      }
    }

    if (arduboy.pressed(DOWN_BUTTON))
    {
      if(playerY<4&&boardXY[playerX][playerY+1]!=0)
      {
        playerY=playerY+1;
        moveCountdown=4;
      }
    }
    if (arduboy.pressed(UP_BUTTON))
    {
      if(playerY>0&&boardXY[playerX][playerY-1]!=0)
      {
	      playerY=playerY-1;
        moveCountdown=4;
      }
    }
  }

  if (arduboy.pressed(A_BUTTON)&&(!buttonA))
  {
    stomp = 6;
    mendBlocks();
    buttonA=true;
  }

  if(arduboy.notPressed(A_BUTTON))
  buttonA = false;

  //if (arduboy.pressed(B_BUTTON)&&(!buttonB))
  //{
  //  buttonB=true;
  //  loadLevel();
  //}

  //if(arduboy.notPressed(B_BUTTON))
  //buttonB = false;
}

void calcY()
{
  yDestination = -playerScopeY;

  if(yDestination>14)
  yDestination=14;

  if(yDestination<-9)
  yDestination=-9;

  if(anim2==2)
  {
    if(yLocation<yDestination)
    {
      yLocation++;
    }

    if(yLocation>yDestination)
    {
      yLocation--;
    }
  }
}

void progressGame()
{
  if(cycleCountdown==1)
  {
    int random;


    for(int x = 0; 5 > x; x++)
    {
      for(int y = 0; 5 > y; y++)
      {
        if(boardXY[x][y]==1)
        {
          boardXY[x][y]=0;
        }
      }
    }


    for(int x = 0; 5 > x; x++)
    {
      for(int y = 0; 5 > y; y++)
      {
        if(boardXY[x][y]==2)
        {
          boardXY[x][y]=1;
        }
      }
    }


    for(int x = 0; 5 > x; x++)
    {
      for(int y = 0; 5 > y; y++)
      {
        if(boardXY[playerX][playerY]==0&&deathTimer==0)
        {
          deathTimer=100;
          sound.tones(playerDieFx);
        }
        
        random=rand()%(35-level); // melt blocks
        if(random==0&&boardXY[x][y]==3&&changeDelay==0)
        {
          xChangePos = x;
          yChangePos = y;
          changeDelay = 40;
          xPass = x;
          yPass = y;
          spawnRain();
        }
      }
    }

    for(int x = 0; 5 > x; x++)
    {
      for(int y = 0; 5 > y; y++)
      {
        if(boardXY[x][y]==1)
        {
          if(x<4)
          {
            if(boardXY[x+1][y]==3)
            boardNextXY[x+1][y]=2;
          }

          if(x>0)
          {
            if(boardXY[x-1][y]==3)
            boardNextXY[x-1][y]=2;
          }

          if(y<4)
          {
            if(boardXY[x][y+1]==3)
            boardNextXY[x][y+1]=2;
          }

          if(y>0)
          {
            if(boardXY[x][y-1]==3)
            boardNextXY[x][y-1]=2;
          }
        }
      }
    }

    for(int x = 0; 5 > x; x++)
    {
      for(int y = 0; 5 > y; y++)
      {
        if(boardXY[x][y]==3&&boardNextXY[x][y]==2)
        boardXY[x][y]=2;
      }
    }

    for(int x = 0; 5 > x; x++)
    {
      for(int y = 0; 5 > y; y++)
      {
        boardNextXY[x][y]=0;
      }
    }

    cycleCountdown=cycleLength; // reset cycle countdown for next level
  }

  if(levelCountdown==1)
  {
    //level++;
    loadLevel();
  }
}

void spawnRain()
{
  for (int i = 0; rainNumber > i ; i++)
  {
    if(rainExist[i]==0)
    {
      rainExist[i]=40;
      xStep=0;
      for(int y = 0; 5 > y; y++)
      {
        yStep=0;
        for(int x = 0; 5 > x; x++)
        {
          {
            if(xPass==x&&yPass==y)
            {
              rainX[i]=(x*12)+57+xStep+rand()%16;
              rainY[i]=(y*6)+yStep+yLocation-150+rand()%32;
            }
          }
          yStep=yStep+6;
        }
        xStep=xStep-12;
      }
    }
  }
}

void moveRain()
{
  for (int i = 0; rainNumber > i ; i++)
  {
    if(rainExist[i]>0)
    {
      rainY[i]=rainY[i]+3;
      rainExist[i]--;
    }
  }
}

void drawRain()
{
  for (int i = 0; rainNumber > i ; i++)
  {
    if(rainExist[i]>0)
    {
      SpritesU::drawPlusMaskFX(rainX[i],rainY[i]+yLocation, lifei, frameNumber);
    }
  }
}

void loadLevel()
{
  if (levelRead==0)
  level=0;

  level++;

  sound.tones(changeFx);
  flipCountdown = 40;

  levelCountdown = 1000;
  cycleLength = 100;

  playerX = pgm_read_byte(&board[levelRead]);
  levelRead++;
  playerY = pgm_read_byte(&board[levelRead]);
  levelRead++;

  for(int x = 0; 5 > x; x++)
  {
    for(int y = 0; 5 > y; y++)
    {
      boardObject = pgm_read_byte(&board[levelRead]);
      levelRead++;
      boardXY[x][y]=boardObject;
    }
  }

  if (level == 25) // repeat levels // use last level number from data.h
  {
    levelRead = 0;
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
      if(gameOverTimer==0)
      {
        renderGame();
      }
      else
      {
        renderGameOver();
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
  for(int x = 0 ; 160 > x ; x = x + 16)
  {
    for(int y = 0; 88 > y ; y = y + 24)
    {
      SpritesU::drawPlusMaskFX(x+xOffset, y+yOffset, tes, frameNumber);
    }
  }

  SpritesU::drawPlusMaskFX(2, 8, title, frameNumber);

  SpritesU::drawPlusMaskFX(2, 45, speaker, frameNumber);

  if(Arduboy2Audio::enabled())
  SpritesU::drawPlusMaskFX(22, 46, ssound, frameNumber);
}

void renderGame()
{
  drawBackground();
  drawBlocks();
  drawPlayer();
  drawRain();
  drawHud();
}

void mendBlocks()
{
  if(boardXY[playerX][playerY]<3)
  {
    if(boardXY[playerX][playerY]==2)
    {
      score = score + 10;
      sound.tones(cureFx);
    }

    if(boardXY[playerX][playerY]==1)
    {
      score = score + 25;
      sound.tones(fixFx);
    }

    boardXY[playerX][playerY]=3;
  }
  else
  {
    sound.tones(stompFx);
  }
}

void drawBackground()
{
  SpritesU::drawPlusMaskFX(4, 8+yLocation, grid, frameNumber);
}

void drawBlocks()
{
  blockCount=0;
  xStep=0;
  for(int y = 0; 5 > y; y++)
  {
    yStep=0;
    for(int x = 0; 5 > x; x++)
    {
      if(flipCountdown==0)
      {
        if(boardXY[x][y] == 3)
        {
          blockCount++;
          SpritesU::drawPlusMaskFX((x*12)+51+xStep, (y*6)+yStep+yLocation, scube, frameNumber);
        }

        if(boardXY[x][y] == 2)
        {
          blockCount++;
          SpritesU::drawPlusMaskFX((x*12)+51+xStep, (y*6)+yStep+yLocation, dcube, frameNumber);
        }

        if(boardXY[x][y] == 1)
        {
          blockCount++;
          SpritesU::drawPlusMaskFX((x*12)+51+xStep, (y*6)+yStep+yLocation, vcube, frameNumber);
        }
      }
      else
      {
        if(boardXY[x][y] != 0)
        {
          blockCount++;
          SpritesU::drawPlusMaskFX((x*12)+51+xStep, (y*6)+yStep+yLocation, box, frameNumber);
        }
      }
      yStep=yStep+6;
    }
    xStep=xStep-12;
  }

  if(blockCount<3&&deathTimer==0)
  {
    deathTimer=100;
    sound.tones(playerDieFx);
  }
}

void drawPlayer()
{
  xStep=0;
  for(int y = 0; 5 > y; y++)
  {
    yStep=0;
    for(int x = 0; 5 > x; x++)
    {
      if(playerX==x&&playerY==y&&deathTimer==0)
      {
        if(flipCountdown==0)
        {
        if(boardXY[x][y]==3)
        {
          if(anim4==1)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, lshaddow, frameNumber);

          if(anim4==2)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, lshaddowb, frameNumber);

          if(anim4==3)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, lshaddowc, frameNumber);

          if(anim4==4)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, lshaddowd, frameNumber);
        }

        if(boardXY[x][y]==2)
        {
          if(anim4==1)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, shaddow, frameNumber);

          if(anim4==2)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, shaddowb, frameNumber);

          if(anim4==3)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, shaddowc, frameNumber);

          if(anim4==4)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, shaddowd, frameNumber);
        }

        if(boardXY[x][y]==1)
        {
          if(anim4==1)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, dshaddow, frameNumber);

          if(anim4==2)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, dshaddowb, frameNumber);

          if(anim4==3)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, dshaddowc, frameNumber);

          if(anim4==4)
          SpritesU::drawPlusMaskFX((x*12)+57+xStep, (y*6)+yStep+yLocation+2, dshaddowd, frameNumber);
        }
        }

        if(anim4==1)
        SpritesU::drawPlusMaskFX((x*12)+55+xStep, (y*6)+yStep-12+yLocation+stomp, player, frameNumber);

        if(anim4==2)
        SpritesU::drawPlusMaskFX((x*12)+55+xStep, (y*6)+yStep-12+yLocation+stomp, playerb, frameNumber);

        if(anim4==3)
        SpritesU::drawPlusMaskFX((x*12)+55+xStep, (y*6)+yStep-12+yLocation+stomp, playerc, frameNumber);

        if(anim4==4)
        SpritesU::drawPlusMaskFX((x*12)+55+xStep, (y*6)+yStep-12+yLocation+stomp, playerd, frameNumber);

        playerScopeY=(y*6)+yStep-30;
      }

      if(playerX==x&&playerY==y&&deathTimer>0)
      {
        if(deathTimer<100&&deathTimer>95)
        SpritesU::drawPlusMaskFX((x*12)+50+xStep, (y*6)+yStep-15+yLocation, explodea, frameNumber);

        if(deathTimer<96&&deathTimer>90)
        SpritesU::drawPlusMaskFX((x*12)+50+xStep, (y*6)+yStep-15+yLocation, explodeb, frameNumber);

        if(deathTimer<91&&deathTimer>85)
        SpritesU::drawPlusMaskFX((x*12)+50+xStep, (y*6)+yStep-15+yLocation, explodec, frameNumber);

        if(deathTimer<86&&deathTimer>80)
        SpritesU::drawPlusMaskFX((x*12)+50+xStep, (y*6)+yStep-15+yLocation, exploded, frameNumber);

        if(deathTimer<81&&deathTimer>75)
        SpritesU::drawPlusMaskFX((x*12)+50+xStep, (y*6)+yStep-15+yLocation, explodee, frameNumber);

        if(deathTimer<76&&deathTimer>70)
        SpritesU::drawPlusMaskFX((x*12)+50+xStep, (y*6)+yStep-15+yLocation, explodef, frameNumber);

        playerScopeY=(y*6)+yStep-30;
      }
      yStep=yStep+6;
    }
    xStep=xStep-12;
  }
}

void drawHud()
{

  if(frameNumber==2)
  {
    a.drawRect(125,58-(levelCountdown/20),2,(levelCountdown/20),DARK_GRAY);
    a.drawRect(124,8,4,51,LIGHT_GRAY);
  }

  Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

  if(frameNumber==1)
  {
  tinyfont.setCursor(10,1);
  tinyfont.print(score);

  tinyfont.setCursor(92,1);
  tinyfont.print("LEVEL");

  tinyfont.setCursor(118,1);
  tinyfont.print(level);
  }
}

void gameOver()
{
  gameOverTimer=220;
}

void renderGameOver()
{
  SpritesU::drawPlusMaskFX(0, 15, gameover, frameNumber);

  Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());
  tinyfont.setCursor(10,55);
  tinyfont.print(F("YOUR SCORE:"));
  tinyfont.setCursor(70,55);
  tinyfont.print(score);
}

void resetVariables()
{
  start=false;

dataRead = 0;

  for(int x = 0; 5 > x; x++)
  {
    for(int y = 0; 5 > y; y++)
    {
      boardXY[x][y]=0;
    }
  }

  for (int i = 0; rainNumber > i ; i++)
  {
    rainExist[i]=0;
  }

moveCountdown = 0;

levelRead = 0;

buttonA = false;
//buttonB = false;

changeDelay=0;

deathTimer = 0;
gameOverTimer = 0;
score=0;
}
