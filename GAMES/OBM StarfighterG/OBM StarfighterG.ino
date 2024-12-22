
// Starfighter
// Version 1.00
// October 26th 24 - November 13th 24
// Onebit Productions
// Matthew Bryan

#define ABG_IMPLEMENTATION
#define ABG_SYNC_PARK_ROW
#define ABG_TIMER4
#include "ArduboyG.h"
ArduboyG_Config<ABG_Mode::L4_Triplane> a;
#include <Arduboy2.h>
#include "data.h"
#include "graphics.h"
#include <ArduboyTones.h>
#include "sound.h"
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
#include "variables.h"

void setup()
{
  a.begin();
  arduboy.display();

  a.startGray();
  newGame();
}

void loop()
{
  a.waitForNextPlane();
  if(a.needsUpdate())
  update();
  render();
}

void newGame()
{
  a.clear();
  spawnStars();
}

void update()
{
  animations();

  if (start)
  {
    gameUpdate();
  }
  else
  {
    menuUpdate();
  }
}

void render()
{
  if (start)
  {
    gameDraw();
  }
  else
  {
    menuDraw();
  }

  if(++frameNumber==3) frameNumber=0;
}

void animations()
{

  anim2++;
  if (anim2 == 3){anim2=1;}


  anim3++;
  if (anim3 == 4){anim3=1;}

  anim4++;
  if (anim4 == 5){anim4=1;}

  anim7++;
  if (anim7 == 8){anim7=1;}

  anim14++;
  if (anim14 == 15){anim14=1;}

  anim33++;
  if (anim33 == 34){anim33=1;}

  anim50++;
  if (anim50 == 51){anim50=1;}

  anim80++;
  if (anim80 == 81){anim80=1;}

  anim250++;
  if (anim250 == 251){anim250=1;}

  if (deathTimer>0)
  {
    deathTimer--;
    if (deathTimer==1)
    die();
  }

  if (bombTimer>0)
  {
    bombTimer--;
  }

  if (goTimer>0)
  {
    goTimer--;
    if (goTimer==1)
    resetVariables();
  }

  if (winTimer>0)
  {
    winTimer--;
    if (winTimer==1)
    resetVariables();
  }

  if ((laserTimer>0)&&(anim2==1))
  {
    laserTimer--;
  }


  if(anim7==7)
  {
    for(int i = 0 ; explodeNumber > i; i++)
    {
      if (explodeExist[i]>0)
      {
        explodeExist[i]--;
      }
    }
  }

  if(respawnTimer>0)
  respawnTimer--;

  if (eventTime>0)
  eventTime--;

  if (eventTime==0)
  triggerEvent();
}

void menuUpdate()
{

  dataRead = 0; // fix progression bug

  if(menuSongTimer==0)
  {
    sound.tones(MAYDAYTune);
  }

  menuSongTimer++;
  if(menuSongTimer==745) // 745
  menuSongTimer=0;

  if (menuY>12)
  menuY--;

  if (arduboy.pressed(RIGHT_BUTTON))
	arduboy.audio.on();
	
  if (arduboy.pressed(LEFT_BUTTON))
	arduboy.audio.off();

  if(arduboy.pressed(A_BUTTON))
  {
    arduboy.delayShort(100);
    start = true;
  }
}

void menuDraw()
{

  moveMenuStars();
  drawStars();

  Sprites::drawExternalMask(3, menuY, STARFIGHTER, STARFIGHTERMask, frameNumber, 0);

  if(menuY==12)
  {
    Sprites::drawExternalMask(89, 47, speaker1, speaker1Mask, frameNumber, 0);

    if(Arduboy2Audio::enabled())
	  Sprites::drawExternalMask(108, 47, sound1, sound1Mask, frameNumber, 0);
  }

  //a.setCursor(1,1);
  //a.print(menuSongTimer);
}

void gameUpdate()
{
  counter++;

  if(deathTimer==0&&winTimer==0)
  getInput();

  if(shotExist)
  moveShot();

  if(endExist!=50)
  {
    endGameSequence();
  }

  moveSigma();
  moveEnemy();
  movePlasma();
  moveIcon();
  moveStars();
}

void gameDraw()
{
  drawStars();
  drawPlayer();
  drawLaser();
  drawSigma();
  drawEnemy();
  drawPlasma();
  drawExplode();
  drawIcon();

  if(shotExist&&winTimer==0)
  drawShot();

  drawHud();
}

void triggerEvent()
{
if(winTimer==0)
{
  if((eventType>0)&&(eventType<20))
  {
    spawnEnemy();
  }

  if((eventType>19)&&(eventType<30))
  {
    spawnIcon();
  } 

  eventTime = pgm_read_byte(&events[dataRead]); // this is for next spawn
  dataRead++;
  eventType = pgm_read_byte(&events[dataRead]);
  dataRead++;
  yCords = pgm_read_byte(&events[dataRead]);
  dataRead++;

  if(dataRead > 582)// looping = EVENTS * 3 - 1 TO PREVENT JUNK SPAWNS
  dataRead=0;
}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void endGameSequence()
{
  if(winTimer == 0)
  {
    if (anim33==33) // shoot frequency
    {
      dataPass=50; //
      spawnPlasma();
    }

    if (anim250==125) // shoot frequency
    {
      eventTime = 100;
      eventType = 1;
      yCords = 22;
      spawnEnemy();
    }

    if (anim250==250) // shoot frequency
    {
      eventTime = 100;
      eventType = 1;
      yCords = 88;
      spawnEnemy();
    }
  }
}

void spawnStars()
{
  offsetY = -32;
  int random;
  for (int i = 0; starNumber > i; i++)
  {
    random = rand() % 126 + 1;
    starX[i] = random;

    random = rand() % 126 + 1;
    starY[i] = random;

    random = rand() % 3 + 1;
    starZ[i] = random;
  }
}

void moveMenuStars()
{
  for (int i = 0; starNumber > i; i++)
  {
    if ((starZ[i] == 3)&&(anim2==2))
    starY[i] = starY[i] + 1;

    if ((starZ[i] == 2)&&(anim3 == 3))
    starY[i] = starY[i] + 1;

    if ((starZ[i] == 1)&&(anim4 == 4))
    starY[i] = starY[i] + 1;

    if (starY[i]>127)
    { 
      starX[i] = rand() % 126 + 1;
      starY[i] = 0;
      starZ[i] = rand() % 3 + 1;
    }
  }
}

void moveStars()
{
  for (int i = 0; starNumber > i; i++)
  {
    if ((starZ[i] == 3)&&(anim2==2))
    starX[i] = starX[i] - 1;

    if ((starZ[i] == 2)&&(anim3 == 3))
    starX[i] = starX[i] - 1;

    if ((starZ[i] == 1)&&(anim4 == 4))
    starX[i] = starX[i] - 1;

    if (starX[i]<1)
    { 
      starX[i] = 126;
      starY[i] = rand() % 126 + 1;
      starZ[i] = rand() % 3 + 1;
    }
  }
}

void drawStars()
{
  for (int i = 0; starNumber > i; i++)
  {
    if (starZ[i]==1)
    a.drawPixel(starX[i], starY[i]+offsetY, DARK_GRAY);

    if (starZ[i]==2)
    a.drawPixel(starX[i], starY[i]+offsetY, LIGHT_GRAY);

    if (starZ[i]==3)
    a.drawPixel(starX[i], starY[i]+offsetY, WHITE);
  }
}

void drawHud()
{

  //a.setCursor(1,1);
  //a.print(winTimer);

  if(goTimer>0)
  {
    Sprites::drawExternalMask(32, 25, GAMEOVER, GAMEOVERMask, frameNumber, 0);
  }

  if(winTimer==0)
  {
  a.drawRect(0,0,128,63,DARK_GRAY);

  if(lives>0)
  a.drawRect(2,61,2,2,WHITE);

  if(lives>1)
  a.drawRect(5,61,2,2,WHITE);

  if(lives>2)
  a.drawRect(8,61,2,2,WHITE);

  if(lives>3)
  a.drawRect(11,61,2,2,WHITE);

  if(lives>4)
  a.drawRect(14,61,2,2,WHITE);

  if(lives>5)
  a.drawRect(17,61,2,2,WHITE);

  if(bomb>0)
  a.drawRect(124,61,2,2,LIGHT_GRAY);

  if(bomb>1)
  a.drawRect(121,61,2,2,LIGHT_GRAY);

  if(bomb>2)
  a.drawRect(118,61,2,2,LIGHT_GRAY);

  if(bomb>3)
  a.drawRect(115,61,2,2,LIGHT_GRAY);

  if(bomb>4)
  a.drawRect(112,61,2,2,LIGHT_GRAY);

  if(bomb>5)
  a.drawRect(109,61,2,2,LIGHT_GRAY);
  }
}

void getInput()
{
  if (arduboy.pressed(RIGHT_BUTTON))
  {
    if(playerX<111)
    playerX++;
  }

  if (arduboy.pressed(LEFT_BUTTON))
  {
    if(playerX>1)
    playerX--;
  }

  if (arduboy.pressed(DOWN_BUTTON))
  {
    frameUp = false;
    frameCentre = false;
    frameDown = true;

    if(offsetY>-64&&playerY==24) // scroll screen
    offsetY--;

    if(offsetY==-64&&playerY<46) // below scroll
    playerY++;

    if(playerY<24) // above scroll
    playerY++;

    if(playerY==46)
    {
      frameDown = false;
      frameCentre = true;
      frameUp = false;
    }
  }

  if (arduboy.pressed(UP_BUTTON))
  {
    frameDown = false;
    frameCentre = false;
    frameUp = true;

    if(offsetY<0&&playerY==24) // scroll screen
    offsetY++;

    if(offsetY==0&&playerY>1) // above scroll
    playerY--;

    if(playerY>24) // below scroll
    playerY--;

    if(playerY==1)
    {
      frameUp = false;
      frameCentre = true;
    }
  }

  if((arduboy.notPressed(UP_BUTTON))&&(arduboy.notPressed(DOWN_BUTTON)))
  {
    frameCentre = true;
    frameUp = false;
    frameDown = false;
  }

  if(powerLevel==0)
  {
    if(arduboy.pressed(A_BUTTON)&& anim14 == 14)
    spawnSigma();  
  }

  if(powerLevel>0)
  {
    if(arduboy.pressed(A_BUTTON)&& anim7 == 7)
    spawnSigma();
  }

  if(arduboy.pressed(A_BUTTON)&&powerLevel>1)
  {
    if(laserTimer == 0)
    {
      laserTimer=5;
      sound.tones(laserFx);
    }
  }  

  if(arduboy.pressed(B_BUTTON)&&bombTimer==0)
  {
    if(bomb>0)
    {
      bombTimer = 25;
      bomb--;
      sound.tones(bombFx);
      
      for (int i = 0; enemyNumber > i ; i++)
      {
        if (enemyExist[i])
        {
          if((bossExist!=i)&&(endExist!=i)) // normal bomb Explode
          {
            enemyExist[i] = 0;
            dataPass=i;
            spawnExplode();
          }
        }
      }

      for (int i = 0; plasmaNumber > i ; i++)
      {
        plasmaExist[i] = false;
      }     

    }
  }
}

void spawnIcon()
{
  if(eventType==20)
  {
    heartX=128;
    heartY=yCords;
    heartExist=true;
  }

  if(powerLevel<3)
  {
    if(eventType==21)
    {
      powerUpX=128;
      powerUpY=yCords;
      powerUpExist=true;
    }
  }

    if(eventType==22)
    {
      bombUpX=128;
      bombUpY=yCords;
      bombUpExist=true;
    }
}

void moveIcon()
{
  if(heartExist)
  {
    heartX--;
    if(deathTimer==0)
    {
      Rect heartCol(heartX,heartY+offsetY, 7, 7);
      Rect playerCol(playerX,playerY, 16, 16);
    
      if (arduboy.collide(heartCol, playerCol))
      {
        heartExist=false;
        lives++;
        sound.tones(iconFx);
      }
    }
  }

  if(powerUpExist)
  {
    powerUpX--;
    if(deathTimer==0)
    {
      Rect powerUpCol(powerUpX,powerUpY+offsetY, 8, 8);
      Rect playerCol(playerX,playerY, 16, 16);
    
      if (arduboy.collide(powerUpCol, playerCol))
      {
        powerUpExist=false;
        powerLevel++;

        if(powerLevel == 3)
        spawnShot();

        sound.tones(iconFx);
      }
    }
  }

  if(bombUpExist)
  {
    bombUpX--;
    if(deathTimer==0)
    {
      Rect bombUpCol(bombUpX,bombUpY+offsetY, 13, 10);
      Rect playerCol(playerX,playerY, 16, 16);
    
      if (arduboy.collide(bombUpCol, playerCol))
      {
        bombUpExist=false;
        bomb++;
        sound.tones(iconFx);
      }
    }
  }

  if(heartX<-10)
  heartExist=false;

  if(powerUpX<-10)
  powerUpExist=false;
}

void spawnShot()
{
  shotExist = true;
  for(int i = 0; shotNumber > i; i++)
  {
    int random;
    random = rand() % 10;
    shotX[i] = playerX + 8 + random;
    shotY[i] = playerY + 8 - random;
  }
}

void moveShot()
{
  for(int i = 0; shotNumber > i; i++)
  {
    for(int c = 0; shotNumber > c; c++)
    {
      if(shotX[i]>shotX[c])
      {
        shotXvel[i]--;
      }
      if(shotX[i]<shotX[c])
      {
        shotXvel[i]++;
      }
      if(shotY[i]>shotY[c])
      {
        shotYvel[i]--;
      }
      if(shotY[i]<shotY[c])
      {
        shotYvel[i]++;
      }


      if(shotX[i]>playerX+4)
      {
        shotXvel[i]=shotXvel[i]-1;
      }
      if(shotX[i]<playerX+4)
      {
        shotXvel[i]=shotXvel[i]+1;
      }
      if(shotY[i]>playerY+4)
      {
        shotYvel[i]=shotYvel[i]-1;
      }
      if(shotY[i]<playerY+4)
      {
        shotYvel[i]=shotYvel[i]+1;
      }      
    }

    if(shotXvel[i]>40)
    shotXvel[i]=40;

    if(shotXvel[i]<-40)
    shotXvel[i]=-40;

    if(shotYvel[i]>40)
    shotYvel[i]=40;

    if(shotYvel[i]<-40)
    shotYvel[i]=-40;

    shotX[i]=shotX[i]+(shotXvel[i]/10);
    shotY[i]=shotY[i]+(shotYvel[i]/10);

    
    if(anim7==7) // nerf shot
    {
    for (int c = 0; enemyNumber > c ; c++)
    {
       if (enemyExist[c]>0)
       {
        Rect enemyCol(enemyX[c],enemyY[c]+offsetY, 16, 16);

        Rect bossCol(enemyX[c],enemyY[c]+offsetY, 32, 32);

        Rect shotCol(shotX[i],shotY[i], 7, 7);
    

        if(bossExist!=c)
        {
        if (arduboy.collide(enemyCol, shotCol))
        {
          sound.tones(hitFx);

          enemyExist[c] = enemyExist[c] - 1;

          if (enemyExist[c] == 0)
          {
            dataPass=c;
            spawnExplode();
          }
        }
        }

        if(bossExist==c)
        {
        if (arduboy.collide(bossCol, shotCol))
        {
          sound.tones(hitFx);

          enemyExist[c] = enemyExist[c] - 1;

          if (enemyExist[c] == 0)
          {
            dataPass=c;
            spawnExplode();
          }
        }
        }
      }
    }
    }

    for (int c = 0; plasmaNumber > c ; c++)
    {
      if (plasmaExist[c])
      {
        Rect plasmaCol(plasmaX[c],plasmaY[c]+offsetY, 7, 7);

        Rect shotCol(shotX[i],shotY[i], 7, 7);
    
        if (arduboy.collide(plasmaCol, shotCol))
        {
          plasmaExist[i] = false;
          sound.tones(hitFx);
        }
      }
    }
  }
}

void drawShot()
{
  for(int i = 0; shotNumber > i; i++)
  {
    Sprites::drawExternalMask(shotX[i], shotY[i], shot, shotMask, frameNumber, 0);
  }
}

void drawIcon()
{
  if(heartExist)
  {
    Sprites::drawExternalMask(heartX, heartY+offsetY, heart, heartMask, frameNumber, 0);
  }

  if(powerUpExist)
  {
    Sprites::drawExternalMask(powerUpX, powerUpY+offsetY, powerUp, powerUpMask, frameNumber, 0);
  }

  if(bombUpExist)
  {
    Sprites::drawExternalMask(bombUpX, bombUpY+offsetY, bomb1, bomb1Mask, frameNumber, 0);
  }
}

void spawnSigma()
{
  for (int i = 0; sigmaNumber > i ; i++)
  {
    if (!sigmaExist[i])
    {
      if(powerLevel<2)
      sound.tones(sigmaFx);

      sigmaExist[i] = true;
      sigmaX[i] = playerX + 16;
      sigmaY[i] = playerY + 4 - offsetY;
      break;
    }
  }
}

void moveSigma()
{
  for (int i = 0; sigmaNumber > i ; i++)
  {
    if (sigmaExist[i])
    {
      sigmaX[i]=sigmaX[i]+2;

      for (int c = 0; enemyNumber > c ; c++)
      {
        if (enemyExist[c]>0)
        {
          Rect enemyCol(enemyX[c],enemyY[c]+offsetY, 16, 16);

          Rect bossCol(enemyX[c],enemyY[c]+offsetY, 32, 32);

          Rect sigmaCol(sigmaX[i],sigmaY[i]+offsetY, 7, 7);


          if(bossExist!=c)
          {
          if (arduboy.collide(enemyCol, sigmaCol))
          {
            sound.tones(hitFx);

            enemyExist[c] = enemyExist[c] - 1;

            if (enemyExist[c] == 0)
            {
              dataPass=c;
              spawnExplode();
            }
            sigmaExist[i] = false;
          }
          }

          if (bossExist==c)
          {
          if (arduboy.collide(bossCol, sigmaCol))
          {
            sound.tones(hitFx);

            enemyExist[c] = enemyExist[c] - 1;

            if (enemyExist[c] == 0)
            {
              dataPass=c;
              spawnExplode();
            }
            sigmaExist[i] = false;
          }
          }
        }
      }

      if (sigmaX[i]>128)
      sigmaExist[i] = false;
    }
  }
}

void drawSigma()
{
  for (int i = 0; sigmaNumber > i ; i++)
  {
    if (sigmaExist[i])
    {
      Sprites::drawExternalMask(sigmaX[i], sigmaY[i]+offsetY, sigma, sigmaMask, frameNumber, 0);
    }
  }
}

void drawLaser()
{
  if (laserTimer>0)
  {
    if(frameCentre)
    {
      Sprites::drawExternalMask(playerX+10,playerY+1, flash, flashMask, frameNumber, 0);

      Sprites::drawExternalMask(playerX+10,playerY+12, flash, flashMask, frameNumber, 0);      

      if((laserTimer==1)||(laserTimer==5))
      {
        arduboy.drawLine(playerX+13,playerY+2,128,playerY+2, WHITE);
        arduboy.drawLine(playerX+13,playerY+13,128,playerY+13, WHITE);
      }
      if((laserTimer==2)||(laserTimer==3))
      {
        arduboy.drawLine(playerX+13,playerY+2,128,playerY+2, LIGHT_GRAY);
        arduboy.drawLine(playerX+13,playerY+13,128,playerY+13, LIGHT_GRAY);
      }
      if(laserTimer==3)
      {
        arduboy.drawLine(playerX+13,playerY+2,128,playerY+2, DARK_GRAY);
        arduboy.drawLine(playerX+13,playerY+13,128,playerY+13, DARK_GRAY);
      }
    }
    else
    {

      Sprites::drawExternalMask(playerX+10,playerY+2, flash, flashMask, frameNumber, 0);

      Sprites::drawExternalMask(playerX+10,playerY+11, flash, flashMask, frameNumber, 0);   

      if((laserTimer==1)||(laserTimer==5))
      {
        arduboy.drawLine(playerX+13,playerY+3,128,playerY+3, WHITE);
        arduboy.drawLine(playerX+13,playerY+12,128,playerY+12, WHITE);
      }
      if((laserTimer==2)||(laserTimer==3))
      {
        arduboy.drawLine(playerX+13,playerY+3,128,playerY+3, LIGHT_GRAY);
        arduboy.drawLine(playerX+13,playerY+12,128,playerY+12, LIGHT_GRAY);
      }
      if(laserTimer==3)
      {
        arduboy.drawLine(playerX+13,playerY+3,128,playerY+3, DARK_GRAY);
        arduboy.drawLine(playerX+13,playerY+12,128,playerY+12, DARK_GRAY);
      }    
    }

    if(anim7==7)
    {
      for (int i = 0; enemyNumber > i ; i++)
      {
        if (enemyExist[i]>0)
        {
          Rect enemyCol(enemyX[i],enemyY[i]+offsetY, 16, 16);

          Rect bossCol(enemyX[i],enemyY[i]+offsetY, 32, 32);

          Rect laserCol(playerX+5,playerY+5, 128, 6);


          if(bossExist!=i)
          {
          if (arduboy.collide(enemyCol, laserCol))
          {
            sound.tones(hitFx);

            enemyExist[i] = enemyExist[i] - 1;

            if (enemyExist[i] == 0)
            {
              dataPass=i;
              spawnExplode();
            }
          }
          }

          if(bossExist==i)
          {
          if (arduboy.collide(bossCol, laserCol))
          {
            sound.tones(hitFx);

            enemyExist[i] = enemyExist[i] - 1;

            if (enemyExist[i] == 0)
            {
              dataPass=i;
              spawnExplode();
            }
          }
          }
        }
      }
    }
  }
}

void spawnPlasma()
{
  for (int i = 0; plasmaNumber > i ; i++)
  {
    if (!plasmaExist[i])
    {
      plasmaExist[i] = true;
      sound.tones(plasmaFx);

      if(dataPass!=50) // normal plasma tracked to enemy
      {
        plasmaX[i] = enemyX[dataPass];
        plasmaY[i] = enemyY[dataPass]+5;
      }

      if(dataPass==bossExist) // boss plasma starting position
      {
        plasmaX[i] = enemyX[dataPass]+12;
        plasmaY[i] = enemyY[dataPass]+12;
      }

      if (enemyType[dataPass] == 2) // plasma shoot AI
      {
        plasmaXvel[i]=-1;

        if(enemyY[dataPass]+offsetY>playerY)
        {plasmaYvel[i]=-1;}

        if(enemyY[dataPass]+offsetY<playerY)
        {plasmaYvel[i]=1;}

        if((enemyY[dataPass]+offsetY<playerY+10)&&(enemyY[dataPass]+offsetY>playerY-10))
        {plasmaYvel[i]=0;}

        if(enemyY[dataPass]+offsetY==playerY)
        {plasmaYvel[i]=0;}
      }

      if ((enemyType[dataPass] == 3)||(enemyType[dataPass] == 4))
      {
        plasmaXvel[i]=-2;
        plasmaYvel[i]=0;
      }

      if (enemyType[dataPass] == 11) // boss plasma velo
      {
        plasmaXvel[i]=-1;
        plasmaYvel[i]=0;
      }

      if (dataPass == 50) // end sequence plasma
      {
        int rY;
        rY = rand() % 127;
        int S;
        S = rand() % 2-2;


        plasmaX[i] = 128;
        plasmaY[i] = rY;
        plasmaXvel[i]=S;
        plasmaYvel[i]=0;
      }

      break;
    }
  }
}

void movePlasma()
{
  for (int i = 0; plasmaNumber > i ; i++)
  {
    if (plasmaExist[i])
    {
      plasmaX[i]=plasmaX[i]+plasmaXvel[i];
      plasmaY[i]=plasmaY[i]+plasmaYvel[i];

      if (plasmaX[i]<-10)
      plasmaExist[i] = false;

      if((deathTimer==0)&&(respawnTimer==0))
      {
        Rect plasmaCol(plasmaX[i],plasmaY[i]+offsetY, 7, 7);
        Rect playerCol(playerX+4,playerY+4, 8, 8);
    
        if (arduboy.collide(plasmaCol, playerCol))
        {
          plasmaExist[i] = false;
          sound.tones(hitFx);

          if(deathTimer==0)
          startDeath();
        }
      }
    }
  }
}

void drawPlasma()
{
  for (int i = 0; plasmaNumber > i ; i++)
  {
    if (plasmaExist[i])
    {
      Sprites::drawExternalMask(plasmaX[i], plasmaY[i]+offsetY, plasma1, plasmaMask, frameNumber, 0);
    }
  }
}

void spawnEnemy()
{
  for (int i = 0; enemyNumber > i ; i++)
  {
    if(enemyExist[i]==0)
    {
      enemyType[i] = eventType;

      if(enemyType[i]==1)
      enemyExist[i] = 3;

      if(enemyType[i]==2)
      enemyExist[i] = 6;

      if((enemyType[i]==3)||(enemyType[i]==4))
      enemyExist[i] = 2;

      if((enemyType[i]>4)&&(enemyType[i]<11))
      enemyExist[i] = 4;

      if(enemyType[i]==11)
      enemyExist[i] = 25;

      if(enemyType[i]==12)
      enemyExist[i] = 50;

      if(enemyType[i]<5)
      {
        enemyX[i] = 128;
        enemyY[i] = yCords;
      }

      enemyDataRead[i]=0;

      if(enemyType[i]==5)
      {
        enemyX[i] = 100;
        enemyY[i] = -16;
        enemyTimer[i] = 0;
      }

      if(enemyType[i]==6)
      {
        enemyX[i] = 100;
        enemyY[i] = 128;
        enemyTimer[i] = 0;
      }

      if(enemyType[i]==7)
      {
        enemyX[i] = 5;
        enemyY[i] = -16;
        enemyTimer[i] = 0;
      }

      if(enemyType[i]==8)
      {
        enemyX[i] = 5;
        enemyY[i] = 128;
        enemyTimer[i] = 0;
      }

      if(enemyType[i]==9)
      {
        enemyX[i] = 128;
        enemyY[i] = 56;
        enemyTimer[i] = 0;
      }

      if(enemyType[i]==10)
      {
        enemyX[i] = -16;
        enemyY[i] = 56;
        enemyTimer[i] = 0;
      }

      if(enemyType[i]==11)
      {
        bossExist = i; // save enemy number for boss for colisions etc.
        enemyX[i] = 128;
        enemyY[i] = 48;
        enemyTimer[i] = 0;
      }

      if(enemyType[i]==12)
      {
        endExist = i; // save enemy number for boss for colisions etc.
        enemyX[i] = 128;
        enemyY[i] = 58;
        enemyTimer[i] = 0;
      }

      break;
    }
  }
}

void moveEnemy()
{
  for (int e = 0; enemyNumber > e ; e++)
  {
    if (enemyExist[e]>0)
    {

      if(enemyType[e]==1) // different types act differently
      {

        enemyX[e]=enemyX[e]-1;

        if((deathTimer==0)&&(respawnTimer==0)&&(anim3==1))
        {
          if ((enemyY[e]+offsetY)>playerY)
          enemyY[e]--;
        
          if ((enemyY[e]+offsetY)<playerY)
          enemyY[e]++;
        }
      }

      if(enemyType[e]==2)
      {
        if(anim3==1)
        enemyX[e]=enemyX[e]-1;

        if (anim50 == 50)
        {
          dataPass=e;
          spawnPlasma();
        }
      }


      if(enemyType[e]==3)
      {
        enemyX[e]=enemyX[e]-1;

        if(anim80==1)
        enemyYvel[e] = 1;

        if(anim80==20)
        enemyYvel[e] = 0;        

        if(anim80==40)
        enemyYvel[e] = -1;

        if(anim80==60)
        enemyYvel[e] = 0;       

        enemyY[e]=enemyY[e]+enemyYvel[e];

        if ((anim80 == 20)||(anim80 == 60))
        {
          dataPass=e;
          spawnPlasma();
        }
      }

      if(enemyType[e]==4)
      {
        enemyX[e]=enemyX[e]-1;

        if(anim80==1)
        enemyYvel[e] = -1;

        if(anim80==20)
        enemyYvel[e] = 0;        

        if(anim80==40)
        enemyYvel[e] = 1;

        if(anim80==60)
        enemyYvel[e] = 0;       

        enemyY[e]=enemyY[e]+enemyYvel[e];

        if ((anim80 == 20)||(anim80 == 60))
        {
          dataPass=e;
          spawnPlasma();
        }
      }

      if(enemyType[e]==5)
      {
        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0)
        {
          enemyTimer[e] = pgm_read_byte(&pattern5[enemyDataRead[e]]);
          enemyDataRead[e]++;          
          enemyXvel[e] = (pgm_read_byte(&pattern5[enemyDataRead[e]]))-100;
          enemyDataRead[e]++;
          enemyYvel[e] = (pgm_read_byte(&pattern5[enemyDataRead[e]]))-100;
          enemyDataRead[e]++;
        }
      }

      if(enemyType[e]==6)
      {
        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0)
        {        
          enemyTimer[e] = pgm_read_byte(&pattern6[enemyDataRead[e]]);
          enemyDataRead[e]++;          
          enemyXvel[e] = (pgm_read_byte(&pattern6[enemyDataRead[e]]))-100;
          enemyDataRead[e]++;
          enemyYvel[e] = (pgm_read_byte(&pattern6[enemyDataRead[e]]))-100;
          enemyDataRead[e]++;
        }
      }

      if(enemyType[e]==7)
      {
        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0)
        {      
            enemyTimer[e] = pgm_read_byte(&pattern7[enemyDataRead[e]]);
            enemyDataRead[e]++;          
            enemyXvel[e] = (pgm_read_byte(&pattern7[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
            enemyYvel[e] = (pgm_read_byte(&pattern7[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
        }
      }

      if(enemyType[e]==8)
      {
        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0)
        {      
            enemyTimer[e] = pgm_read_byte(&pattern8[enemyDataRead[e]]);
            enemyDataRead[e]++;          
            enemyXvel[e] = (pgm_read_byte(&pattern8[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
            enemyYvel[e] = (pgm_read_byte(&pattern8[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
        }
      }

      if(enemyType[e]==9)
      {
        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0)
        {      
            enemyTimer[e] = pgm_read_byte(&pattern9[enemyDataRead[e]]);
            enemyDataRead[e]++;          
            enemyXvel[e] = (pgm_read_byte(&pattern9[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
            enemyYvel[e] = (pgm_read_byte(&pattern9[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
        }
      }

      if(enemyType[e]==10)
      {
        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0)
        {      
            enemyTimer[e] = pgm_read_byte(&pattern10[enemyDataRead[e]]);
            enemyDataRead[e]++;          
            enemyXvel[e] = (pgm_read_byte(&pattern10[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
            enemyYvel[e] = (pgm_read_byte(&pattern10[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
        }
      }

      if(enemyType[e]==11) ////// BOSS //////
      {

        if (anim33==33) // shoot frequency
        {
          dataPass=e;
          spawnPlasma();
        }

        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0) // movement
        {      
            enemyTimer[e] = pgm_read_byte(&pattern11[enemyDataRead[e]]);
            enemyDataRead[e]++;          
            enemyXvel[e] = (pgm_read_byte(&pattern11[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
            enemyYvel[e] = (pgm_read_byte(&pattern11[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
        }
      }

      if(enemyType[e]==12) ////// END //////
      {

        if (enemyTimer[e]>0)
        enemyTimer[e]--;

        if(enemyTimer[e]==0) // movement
        {      
            enemyTimer[e] = pgm_read_byte(&pattern12[enemyDataRead[e]]);
            enemyDataRead[e]++;          
            enemyXvel[e] = (pgm_read_byte(&pattern12[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
            enemyYvel[e] = (pgm_read_byte(&pattern12[enemyDataRead[e]]))-100;
            enemyDataRead[e]++;
        }

        if(enemyDataRead[e]>60)
        enemyDataRead[e]=3;
      }

      if((enemyType[e]>4)&&(enemyType[e]<13))
      {
        enemyX[e]=enemyX[e]+enemyXvel[e];
        enemyY[e]=enemyY[e]+enemyYvel[e];
      }

      if (enemyX[e]<-26)
      {
        enemyExist[e] = 0;
        enemyDataRead[e] = 0;
        if(bossExist==e)
        bossExist=50; // is not existing...
      }

      if((deathTimer==0)&&(respawnTimer==0))
      {
        Rect enemyCol(enemyX[e],enemyY[e]+offsetY, 16, 16);

        Rect bossCol(enemyX[e],enemyY[e]+offsetY, 32, 32);

        Rect playerCol(playerX+4,playerY+4, 8, 8);
    
        if (arduboy.collide(enemyCol, playerCol))
        {
          dataPass=e;

          if (bossExist!=e) // normal Death/Explosion
          {
            if(endExist!=e) // exclude end death
            {
              spawnExplode();
              enemyExist[e]=0;
            }
            if(deathTimer==0)
            startDeath();
          }
        }

        if (arduboy.collide(bossCol, playerCol))
        {
          dataPass=e;

          if (bossExist==e)
          {

            if(deathTimer==0)
            startDeath();
          }
        }
      }
    }
  }
}

void drawEnemy()
{
  for (int i = 0; enemyNumber > i ; i++)
  {
    if (enemyExist[i]>0)
    {
      if(enemyType[i]==1)
      Sprites::drawExternalMask(enemyX[i], enemyY[i]+offsetY, enemy, enemyMask, frameNumber, 0);

      if(enemyType[i]==2)
      Sprites::drawExternalMask(enemyX[i], enemyY[i]+offsetY, tank, tankMask, frameNumber, 0);

      if((enemyType[i]==3)||(enemyType[i]==4))
      Sprites::drawExternalMask(enemyX[i], enemyY[i]+offsetY, swerver, swerverMask, frameNumber, 0);

      if((enemyType[i]>4)&&(enemyType[i]<11))
      Sprites::drawExternalMask(enemyX[i], enemyY[i]+offsetY, ring, ringMask, frameNumber, 0);

      if(enemyType[i]==11)
      Sprites::drawExternalMask(enemyX[i], enemyY[i]+offsetY, boss, bossMask, frameNumber, 0);

      if(enemyType[i]==12)
      Sprites::drawExternalMask(enemyX[i], enemyY[i]+offsetY, end, endMask, frameNumber, 0);
    }
  }
}


void drawPlayer()
{
  if((deathTimer==0)&&(respawnTimer==0)&&(winTimer==0))
  {

    if(frameCentre)
    Sprites::drawExternalMask(playerX, playerY, player, playerMask, frameNumber, 0);

    if(frameUp)
    Sprites::drawExternalMask(playerX, playerY, playerUp, playerUpMask, frameNumber, 0);

    if(frameDown)
    Sprites::drawExternalMask(playerX, playerY, playerDown, playerDownMask, frameNumber, 0);     
  }

  if((deathTimer==0)&&(respawnTimer>0)&&(winTimer==0))
  {
    if(anim7==1)
    {
      if(frameCentre)
      Sprites::drawExternalMask(playerX, playerY, player, playerMask, frameNumber, 0);

      if(frameUp)
      Sprites::drawExternalMask(playerX, playerY, playerUp, playerUpMask, frameNumber, 0);

      if(frameDown)
      Sprites::drawExternalMask(playerX, playerY, playerDown, playerDownMask, frameNumber, 0);
    }  
  }

  if((bombTimer>4)&&(bombTimer>0)&&(winTimer==0))
  {
    arduboy.fillCircle(playerX+7,playerY+7,bombTimer,WHITE);

    arduboy.drawCircle(playerX+7,playerY+7,12,WHITE);
    arduboy.drawCircle(playerX+7,playerY+7,18,WHITE);
  }

  if(winTimer>0)
  {
    Sprites::drawExternalMask(39, 5, endScreen, endScreenMask, frameNumber, 0);
  }
}

void spawnExplode()
{
  for(int i = 0 ; explodeNumber > i ; i++)
  {
    if(explodeExist[i] == 0)
    {
      explodeExist[i] = 3;
      sound.tones(playerDieFx);

      if(bossExist!=dataPass) // normal explode
      {      
        explodeX[i] = enemyX[dataPass];
        explodeY[i] = enemyY[dataPass]+offsetY;
      }

      if(bossExist==dataPass) // boss Explode
      {
        explodeX[i] = enemyX[dataPass]+8;
        explodeY[i] = enemyY[dataPass]+offsetY+8;
        bossExist=50;
      }

      if (endExist==dataPass)
      gameWin();

      break;
    }
  }
}

void spawnPlayerExplode()
{
  for(int i = 0 ; explodeNumber > i ; i++)
  {
    if(explodeExist[i] == 0)
    {
      explodeExist[i] = 3;
      explodeX[i] = playerX;
      explodeY[i] = playerY;
      break;
    }
  }
}

void drawExplode()
{
  for(int i = 0 ; explodeNumber > i ; i++)
  {
    if(explodeExist[i] > 0)
    {
      if(explodeExist[i]==3)
      {
        Sprites::drawExternalMask(explodeX[i], explodeY[i], explode1, explode1Mask, frameNumber, 0);
      }
      if(explodeExist[i]==2)
      {
        Sprites::drawExternalMask(explodeX[i], explodeY[i], explode2, explode2Mask, frameNumber, 0);
      }
      if(explodeExist[i]==1)
      {
        Sprites::drawExternalMask(explodeX[i], explodeY[i], explode3, explode3Mask, frameNumber, 0);        
      }
    }
  }
}

void startDeath()
{
  sound.tones(playerDieFx);
  deathTimer = 50;
  spawnPlayerExplode();
  powerLevel = 0;
  lives--;
}

void die()
{
  deathTimer=0;
  respawnTimer = 250;
  shotExist = false;
  bomb = 3;

  if (lives==0)
  gameOver();
}

void gameOver()
{
  goTimer = 350;
  deathTimer = 1000;
}

void gameWin()
{
  winTimer = 350;

  for(int i = 0; enemyNumber>i; i++)
  {
    if (enemyExist[i]>0)
    {
      enemyExist[i]=0;
      dataPass=i;
      spawnExplode();
    }
  }


  for(int i = 0; plasmaNumber>i; i++)
  {
    if (plasmaExist[i])
    {
      enemyExist[i]=false;
    }
  }
}

void resetVariables()
{
  lives = 3;
  dataRead = 0;
  goTimer = 0;
  winTimer = 0;
  deathTimer = 0;

  playerX = 8;
  playerY = 24;
  offsetY = -32;
  frameCentre = true;

  heartExist = false;
  powerUpExist = false;
  bombUpExist = false;

  shotExist = false;

  bossExist=50;
  endExist=50;


  eventTime = 150; //countdown untill next event
  eventType = 1; //type of event
  yCords = 60;

  bomb = 3;

  for(int i = 0 ; enemyNumber > i ; i++)
  {
    enemyExist[i] = 0;
  }

  for(int i = 0 ; plasmaNumber > i ; i++)
  {
    plasmaExist[i] = false;
  }

  for(int i = 0 ; sigmaNumber > i ; i++)
  {
    sigmaExist[i] = false;
  }

  powerLevel = 0;
  menuY = 70;
  menuSongTimer=0;
  start = false;
}