// Big Wave Dave
// Version 1.00
// June 8th 24 - June 22nd 24E
// Matthew Bryan

#define ABG_IMPLEMENTATION
#define ABG_SYNC_PARK_ROW
#define ABG_TIMER4
#include "ArduboyG.h"
ArduboyG_Config<ABG_Mode::L4_Triplane> a;
#include <Arduboy2.h>
#include "graphics.h"
#include <ArduboyTones.h>
#include "sound.h"
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

// Global Variables

bool start;
bool dead;
bool win;
bool riding; // riding wave or not

int screenScX;
double screenScY;

int frameNumber = 0;
int counter = 0;
int anim4;
int anim5;
int anim2;
int anim64;

int phase = 0;

int starNumber = 18;
int starX[19];
double starY[19];

int wakeNumber = 15;
int wakeExist[16];
int wakeX[16];
double wakeY[16];

int waveGet;
int startGet;
int endGet;

int waveNumber = 3;
int waveExist[4];
double waveStart[4];
double waveEnd[4];
double waveY[4];

// breaking wave

int waveTop;
int waveLength;
int lineNumber = 9;
int lines[10];

// player variables

int daveX = 55;
int daveY = 40;
int daveXvel;
int daveYvel;
int daveDirection = 1;
int spoolUp;
int daveFloat = 0;
int rideWave;
int droppingIn;
int deathTimer;

// debris
bool debrisExist;
int debrisType;
int debrisX;
int debrisY;

void setup()
{
  //Serial.begin(115200);
  a.begin();
  arduboy.display();
  /*
  for (int i = 1; i < 2000; i++)  // Random dissolve for ARDUBOY title
  {
    int x = rand() % 89 + 1;
    int y = rand() % 17 + 1;
    a.drawPixel(x + 18, y + 22, 0);
    arduboy.display();
  }
  */
  int y = -10;
  for (int i = 0; 30 > i; i++) // Ident
  {
    a.clear();
    Sprites::drawOverwrite(20, y, Onebit, 0);
    y++;
    arduboy.display();
  }
  int x = 128;
  for (int i = 0; 79 > i; i++)
  {
    a.clear();
    Sprites::drawOverwrite(20, 20, Onebit, 0);
    Sprites::drawOverwrite(x, 20, Productions, 0);
    x--;
    arduboy.display();
  }
  y = 65;
  for (int i = 0; 36 > i; i++)
  {
    a.clear();
    Sprites::drawOverwrite(20, 20, Onebit, 0);
    Sprites::drawOverwrite(49, 20, Productions, 0);
    Sprites::drawOverwrite(49, y, Presents, 0);
    y--;
    arduboy.display();
  }
  a.delayShort(1500);
  x = 0;
      
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

  if (phase==0){menuUpdate();}
  if (phase==1){surfingUpdate();}
}

void render()
{
  //a.setCursor(0,20);
  //a.print(droppingIn);  

  if (start==false){menuDraw();}
  if (start){surfingDraw();}

  if(++frameNumber==3) frameNumber=0;
}

void animations()
{
  if (deathTimer>0){deathTimer--;}
  if (deathTimer==1)
  {
    gameOver();
  }

  anim5++;
  if (anim5 == 6){anim5=0;}

  if (anim5 == 5)
  {
    anim4++;
    if (anim4 == 5){anim4=0;}
  }

  anim2++;
  if (anim2 == 3){anim2=0;}

  anim64++;
  if (anim64 == 65){anim64=0;}

  counter++;
  if (counter == 2500){counter=0;}

  if (daveXvel > 0)
  {
    daveXvel = daveXvel - 1;
  }
  if (daveXvel < 0)
  {
    daveXvel = daveXvel + 1;
  }

  if (daveYvel > 0)
  {
    daveYvel = daveYvel - 1;
  }
  if (daveYvel < 0)
  {
    daveYvel = daveYvel + 1;
  }

  if (daveFloat < 0)
  {
    daveFloat = daveFloat + 1;
  }

  if (rideWave > 0) // catching a wave
  {
    rideWave = rideWave - 1;
  }

  if (droppingIn > 0) // length of wave ride
  {
    droppingIn--;

    if (droppingIn == 0) // ending wave surf
    {
      riding = false;
      screenScX = 0;
      screenScY = 0;
      daveY = 40;
    }
  }
}

void spawnStars()
{
  int random;
  for (int i = 0; starNumber > i; i++)
  {
    random = rand() % 128;
    starX[i] = random;

    random = rand() % 5;
    starY[i] = random;
  }
}

void spawnWake()
{
  int random;
  int wakeWidth;
  wakeWidth = startGet - endGet;
  for (int i = 0; (wakeNumber/2) > i; i++)
  {
    random = rand()%3;
    if (wakeExist[i]==0&&random==0)
    {
      random = rand() % wakeWidth + startGet;
      wakeX[i] = random;
      wakeY[i] = waveGet-1;
      wakeExist[i] = 50;
    }
  }
}

void spawnBoardWake()
{
  int random;
  for (int i = 0; wakeNumber > i; i++)
  {
    random = rand()%3;
    if (wakeExist[i]==0&&random==0)
    {
      random = rand()%3;
      wakeX[i] = daveX+6+random;
      wakeY[i] = daveY+13+random;
      wakeExist[i] = 35;
    }
  }
}

void menuUpdate()
{
    if (arduboy.pressed(RIGHT_BUTTON))
	  arduboy.audio.on();
	
    if (arduboy.pressed(LEFT_BUTTON))
	  arduboy.audio.off();

    if (arduboy.pressed(A_BUTTON))
    {
      start = true;
      sound.tones(startTune);
      arduboy.delayShort(500);
      phase = 1;
    }

    if (arduboy.pressed(B_BUTTON))
    {
      start = true;
      sound.tones(startTune);
      arduboy.delayShort(500);
      phase = 1;
    }
}

void getInput()
{
  if (riding)
  {
    if (waveTop<58&&waveTop>(daveY+16)) // ajust to top of wave
    {
      daveY=waveTop-16;
    }

    if (daveDirection==1)
    {
      if (arduboy.pressed(RIGHT_BUTTON)&&daveX<waveLength)
      {
        daveXvel = 2;
      }
      if (arduboy.pressed(LEFT_BUTTON)&&daveX>2)
      {
        daveXvel = -2;
      }
    }
    else
    {
      if (arduboy.pressed(RIGHT_BUTTON)&&daveX<109)
      {
        daveXvel = 2;
      }
      if (arduboy.pressed(LEFT_BUTTON)&&daveX+24>waveLength)
      {
        daveXvel = -2;
      }
    }


    if (arduboy.pressed(DOWN_BUTTON)&&daveY<45)
    {
      daveYvel = 2;
    }

    if (arduboy.pressed(UP_BUTTON)&&daveY>waveTop-10)
    {
      daveYvel = -2;
    }
  }
  else
  {
    if (arduboy.pressed(RIGHT_BUTTON)&&daveDirection==2) // turning
    {
      daveDirection = 1;
      daveXvel=0;
      spoolUp=0;
    }
    if (arduboy.pressed(LEFT_BUTTON)&&daveDirection==1)
    {
      daveDirection = 2;
      daveXvel=0;
      spoolUp=0;
    }

    if (arduboy.pressed(RIGHT_BUTTON))
    {
      if (spoolUp<10)
      {
        spoolUp++;
      }
      if (spoolUp<8)
      {
        daveXvel = 2;
      }
      if (spoolUp>8)
      {
        daveXvel = 2;
      }
    }
    if (arduboy.pressed(LEFT_BUTTON))
    {
      if (spoolUp<10)
      {
        spoolUp++;
      }
      if (spoolUp<8)
      {
        daveXvel = -2;
      }
      if (spoolUp>8)
      {
        daveXvel = -2;
      }
    }

    if (arduboy.notPressed(LEFT_BUTTON)||arduboy.notPressed(RIGHT_BUTTON))
    {
      spoolUp=0;
    }
    if (arduboy.pressed(B_BUTTON))
    {
      rideWave = 20;
    }
  }
}

void surfingUpdate()
{
  int random;
  getInput();

  if (anim5 == 3) // random bobbing in water
  {
    random = rand()%3;
    daveFloat = daveFloat - random;
  }

  for (int i = 0; starNumber > i; i++)
  {
    random = rand() % 4 + 1;
    if (random == 1){starX[i] = starX[i]+1;}
    if (random == 2){starX[i] = starX[i]-1;}
    if (random == 3){starY[i] = starY[i]+0.01;}
    if (random == 4){starY[i] = starY[i]-0.01;}

    starX[i] = starX[i] + screenScX;

    if (starX[i]>127){starX[i]=0;}
    if (starX[i]<0){starX[i]=127;}

    starY[i] = starY[i] + screenScY;

    if (starY[i]>5){starY[i]=0;} // looping screen
    if (starY[i]<0){starY[i]=5;}
  }

  if (riding)
  {

    daveX = daveX + daveXvel;
    daveY = daveY + daveYvel;
    
    random = rand() % 3;
    if (random == 0){spawnBoardWake();}
  }
  else
  {
    if (daveDirection == 1&&daveX < 110)
    {
      daveX = daveX + daveXvel;
    }
    if (daveDirection == 2&&daveX > 2)
    {
      daveX = daveX + daveXvel;
    }

    random = rand() % 10;
    if (random == 0){spawnWave();}

    for (int i = 0; waveNumber > i; i++)
    {
      if (waveExist[i] > 0)
      {
        waveY[i] = waveY[i] + 0.01;
        double waveWidth = (exp(waveY[i]));
        waveStart[i] = waveStart[i] - waveWidth/100;
        waveEnd[i] = waveEnd[i] + waveWidth/100;
        waveExist[i]--;

        random = rand()%3;
        if (random==0)
        {
          int waveWakeY = (exp(waveY[i]))+9;
          waveGet = waveWakeY;
          startGet = waveStart[i];
          endGet = waveEnd[i];
          spawnWake();
        }
      }
    }
  }

  for (int i = 0; wakeNumber > i; i++)
  {
    wakeX[i] = wakeX[i] + screenScX; // scroll wake

    if (wakeExist[i]>0)
    {
      wakeExist[i]--;
    }
  }
}

void spawnWave()
{
  int random;
  int randomX;
  for (int i = 0; waveNumber > i; i++)
  {
    random = rand() % 50;
    if (waveExist[i] == 0&&random==0)
    {
      randomX = rand() % 74 + 22;
      waveExist[i] = 400;
      waveStart[i] = randomX;
      waveEnd[i] = randomX;
      waveY[i] = 0;
    }
  }
}

void catchWave() // Catch Wave
{
  int random;
  random = rand()%1000+100;
  riding = true;
  droppingIn = random;
  if (daveDirection == 1){screenScX=-1;}
  if (daveDirection == 2){screenScX=1;}
  screenScY=-0.01;

  startPeeling();

  for (int i = 0; waveNumber > i; i++)
  {
    waveExist[i] = 0;
  }
}

void startPeeling()
{
  int random;
  random = rand() % 10 + 64;

  waveTop = daveY;
  waveLength = random;

  for (int i = 0; lineNumber > i; i++)
  {
    lines[i] = (waveTop/(i+1)) + waveTop;
  }
}

void wavePeeling()
{
  int random;

  random = rand() % 3;
  if (random==0)
  {
    if (waveTop<58)
    {
      waveTop++;
    }
  }
  if (random==2)
  {
    if (waveTop>20)
    {
      waveTop--;
    }
  }

  random = rand() % 1000;
  if (random==0&&debrisExist==false) // spawn Debris
  {
    debrisType= rand() % 3;
    debrisY = rand() % 30+30;
    debrisExist = true;

    if (daveDirection==1)
    {
      debrisX = 128;
    }
    else
    {
      debrisX = -16;
    }
  }

  if (debrisExist) // move Debris
  {
    if (daveDirection==1)
    {
      debrisX--;
      if (debrisX < -16)
      {
        debrisExist=false;
      }
    }
    else
    {
      debrisX++;
      if (debrisX > 128)
      {
        debrisExist=false;
      }
    }

    Rect playerCol(daveX, daveY+10, 16, 6);
    Rect debrisCol(debrisX,debrisY, 16, 8);
    if (arduboy.collide(playerCol, debrisCol))
    {
      deathTimer=100;
      sound.tones(dieTune);
    }
  }

  if (daveDirection==1) // wave going Right
  {
    a.drawFastHLine(0, waveTop, waveLength,  DARK_GRAY);
    a.drawFastHLine(0, waveTop+1, waveLength,  LIGHT_GRAY);
    a.drawFastHLine(0, waveTop+2, waveLength,  WHITE);

    for (int i = 0; lineNumber > i; i++)
    {
      if ((lines[i]-waveTop)<62&&lines[i]>waveTop)
      {
        random = rand() % 3;
        if (random == 0)
        {
          a.drawFastHLine(0, lines[i], waveLength+(lines[i]-waveTop),  DARK_GRAY);
        }
        if (random == 1)
        {
          a.drawFastHLine(0, lines[i], waveLength+(lines[i]-waveTop),  LIGHT_GRAY);
        }
        if (random == 3)
        {
          a.drawFastHLine(0, lines[i], waveLength+(lines[i]-waveTop),  WHITE);
        }
      }
      lines[i] = lines[i] - 1;
      if (lines[i]<waveTop){lines[i] = waveTop+50;}
    }
  }
  else // wave going Left
  {
    a.drawFastHLine(127-waveLength, waveTop, waveLength,  DARK_GRAY);
    a.drawFastHLine(127-waveLength, waveTop+1, waveLength,  LIGHT_GRAY);
    a.drawFastHLine(127-waveLength, waveTop+2, waveLength,  WHITE);

    for (int i = 0; lineNumber > i; i++)
    {
      if ((lines[i]-waveTop)<62&&lines[i]>waveTop)
      {
        random = rand() % 3;
        if (random == 0)
        {
          a.drawFastHLine(127-(waveLength+(lines[i]-waveTop)), lines[i], waveLength+(lines[i]-waveTop),  DARK_GRAY);
        }
        if (random == 1)
        {
          a.drawFastHLine(127-(waveLength+(lines[i]-waveTop)), lines[i], waveLength+(lines[i]-waveTop),  LIGHT_GRAY);
        }
        if (random == 2)
        {
          a.drawFastHLine(127-(waveLength+(lines[i]-waveTop)), lines[i], waveLength+(lines[i]-waveTop),  WHITE);
        }        
      }

      lines[i] = lines[i] - 1;
      if (lines[i]<waveTop){lines[i] = waveTop+50;}
    }
  }
}

void menuDraw()
{
  Sprites::drawOverwrite(0, 0, titleScreen, frameNumber);
  deathTimer = 0;
  if(Arduboy2Audio::enabled())
	Sprites::drawOverwrite(108, 44, sound1, frameNumber);
}

void surfingDraw()
{
  a.fillRect(0, 0, 127, 3, WHITE); // sky
  a.fillRect(0, 3, 127, 3, LIGHT_GRAY);
  a.fillRect(0, 6, 127, 3, DARK_GRAY);

  for (int i = 0; starNumber > i; i++)
  {
    int starScreenY = (exp(starY[i]))+9;
    if (starScreenY < 20){a.drawPixel(starX[i], starScreenY, DARK_GRAY);}
    if (starScreenY > 19&&starScreenY < 30){a.drawPixel(starX[i], starScreenY, LIGHT_GRAY);}
    if (starScreenY > 29){a.drawPixel(starX[i], starScreenY, WHITE);}
  }

  if (riding) // riding the wave
  {
    if (deathTimer==0)
    {
      for (int i = 0; wakeNumber > i; i++)
      {
        if (wakeExist[i]>0)
        {
          if (wakeY[i] < 20){a.drawPixel(wakeX[i], wakeY[i], DARK_GRAY);}
          if (wakeY[i] > 19&&wakeY[i] < 30){a.drawPixel(wakeX[i], wakeY[i], LIGHT_GRAY);}
          if (wakeY[i] > 29){a.drawPixel(wakeX[i], wakeY[i], WHITE);}
        }
      }
    }
    wavePeeling();
  }
  else // not riding the wave
  {
    for (int i = 0; waveNumber > i; i++)
    {
      if (waveExist[i] > 0)
      {
        int waveScreenY = (exp(waveY[i]))+9;
        int widthWave = waveEnd[i]-waveStart[i];
        int halfWave = widthWave/2;

        if (waveScreenY-6==daveY||waveScreenY-7==daveY) // make dave float over whitewater
        {
          if (daveX+8>waveStart[i]&&daveX+8<waveEnd[i])
          {
            daveFloat=daveFloat-1;
          }
        }

        if (waveScreenY==daveY+2&&rideWave>0) // catch wave detection
        {
          if (daveX+8>waveStart[i]&&daveX+8<waveEnd[i])
          {
            sound.tones(waveTune);
            catchWave();
          }
        }        

        a.drawFastHLine(waveStart[i], waveScreenY, widthWave,  DARK_GRAY);

        if (waveScreenY >19){a.drawFastHLine(waveStart[i]+1, waveScreenY+1, widthWave-2,  LIGHT_GRAY);}
        if (waveScreenY >21){a.drawFastHLine(waveStart[i]+3, waveScreenY+2, widthWave-6,  LIGHT_GRAY);}
        if (waveScreenY >23){a.drawFastHLine(waveStart[i]+6, waveScreenY+3, widthWave-12,  WHITE);}
        if (waveScreenY >25){a.drawFastHLine(waveStart[i]+9, waveScreenY+4, widthWave-18,  WHITE);}
        if (waveScreenY >27){a.drawFastHLine(waveStart[i]+14, waveScreenY+5, widthWave-28,  LIGHT_GRAY);}
        if (waveScreenY >29){a.drawFastHLine(waveStart[i]+20, waveScreenY+6, widthWave-40,  DARK_GRAY);}
      }
    }
    for (int i = 0; wakeNumber > i; i++)
    {
      if (wakeExist[i]>0)
      {
        if (wakeY[i] < 20){a.drawPixel(wakeX[i], wakeY[i], DARK_GRAY);}
        if (wakeY[i] > 19&&wakeY[i] < 30){a.drawPixel(wakeX[i], wakeY[i], LIGHT_GRAY);}
        if (wakeY[i] > 29){a.drawPixel(wakeX[i], wakeY[i], WHITE);}
      }
    }
  }


  if(riding) // drawing Dave
  {
    if (daveDirection == 1)
    {
      if (deathTimer==0)
      {
        Sprites::drawExternalMask(daveX, daveY+daveFloat, dave1, daveMask, frameNumber, 0);
      }

      if (debrisExist)
      {
        if (debrisType==0)
        {
          Sprites::drawExternalMask(debrisX, debrisY, sharkL1, sharkLMask, frameNumber, 0);
        }
        if (debrisType==1)
        {
          Sprites::drawExternalMask(debrisX, debrisY, bar1, barMask, frameNumber, 0);
        }
        if (debrisType==2)
        {
          Sprites::drawExternalMask(debrisX, debrisY, net1, netMask, frameNumber, 0);
        }  
      }
    }
    else
    {
      if (deathTimer==0)
      {
        Sprites::drawExternalMask(daveX, daveY+daveFloat, daveLeft1, daveLeftMask, frameNumber, 0);
      }

      if (debrisExist)
      {
        if (debrisType==0)
        {
          Sprites::drawExternalMask(debrisX, debrisY, sharkR1, sharkRMask, frameNumber, 0);
        }
        if (debrisType==1)
        {
          Sprites::drawExternalMask(debrisX, debrisY, bar1, barMask, frameNumber, 0);
        }
        if (debrisType==2)
        {
          Sprites::drawExternalMask(debrisX, debrisY, net1, netMask, frameNumber, 0);
        }  
      }
    }
  }
  else // not riding
  {
    if (rideWave > 0) // dropping in
    {
      if (daveDirection == 1)
      {
        Sprites::drawExternalMask(daveX, daveY+daveFloat+10, lift1, liftMask, frameNumber, 0);
      }
      else
      {
        Sprites::drawExternalMask(daveX, daveY+daveFloat+10, liftLeft1, liftLeftMask, frameNumber, 0);
      }
    }
    else
    {
      if (daveDirection == 1)
      {
        Sprites::drawExternalMask(daveX, daveY+daveFloat+8, prone1, proneMask, frameNumber, 0);
      }
      else
      {
        Sprites::drawExternalMask(daveX, daveY+daveFloat+8, proneLeft1, proneLeftMask, frameNumber, 0);
      }
    }
  }
}

void gameOver()
{
  arduboy.clear();
  arduboy.delayShort(1000);
  resetVariables();
}

void resetVariables()
{

riding = false;

phase = 0;

// player variables

daveX = 55;
daveY = 40;

screenScX = 0;
screenScY = 0;

daveDirection = 1;
//deathTimer = 0;

// debris
debrisExist = false;

for (int i = 0; wakeNumber > i; i++)
{
  wakeExist[i]=0;
}

start = false;
}
