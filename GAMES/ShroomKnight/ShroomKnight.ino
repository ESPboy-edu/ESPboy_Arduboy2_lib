// Shroom Knight
// Version 2.00
// July 31st 24 - August 27th, 24
// Onebit Productions & Filmote
// Matthew Bryan

#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "roomdata.h"
#include "graphics.h"
#include "sounds.h"
#include "variables.h"
#include "roomfill.h"
Arduboy2 arduboy;
#include <SpritesB.h>
#include <Tinyfont.h>
ArduboyTones sound(arduboy.audio.enabled);

void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.display();
}

void loop()
{
  if (!(arduboy.nextFrame()))return;
  arduboy.clear();

  if (!startGame)
  {

    Sprites::drawOverwrite(32, 16, titleScreen2,0);

    Sprites::drawOverwrite(67, 41, speaker1,0);

    if(Arduboy2Audio::enabled())
	  Sprites::drawOverwrite(86, 41, sound1,0);

    arduboy.display();
    menuInput();
  }

  if (startGame)
  {
    /// main Game loop
    update();
    draw();
  }
}

// Subroutines

void menuInput()
{
  if (arduboy.pressed(RIGHT_BUTTON))
	arduboy.audio.on();
	
  if (arduboy.pressed(LEFT_BUTTON))
	arduboy.audio.off();

  if (arduboy.pressed(A_BUTTON)||arduboy.pressed(B_BUTTON))
  {
    getRoom();
    startGame = true;
    displayDelay = 500;
    textRef = 4;
  }
}

void detectRoom() // look for room changes
{
  if(roomTiles[(playerLocateX+80)/160][(playerLocateY+80)/160] == 2) // detect door
  swapRoom = true;

  if((roomTiles[(playerLocateX+80)/160][(playerLocateY+80)/160] == 7)||(roomTiles[(playerLocateX+80)/160][(playerLocateY+80)/160] == 8)||(roomTiles[(playerLocateX+80)/160][(playerLocateY+80)/160] == 9)) // detect
  {
    startDeath();
  }

  if (swapRoom)
  newRoom();
}

// RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR

void newRoom()
{
  roomRead = 0;
  modX=0;
  roomChanged = false;
  screenChangeCountdown = 30;


  if((roomNumber==1)&&(!roomChanged))
  {
    if (playerLocateX<1000)
    {
      playerTileX = 11;
      playerTileY = 2;
      modX = -360; // entering Right side of screen
      roomNumber = 3;
    }

    if (playerLocateX>1000)
    {
      playerTileX = 1; // positions player
      playerTileY = 4;
      modX = 520; // Entering Left side of ascrren
      roomNumber = 2;
    }

    roomChanged = true;
  }

  if((roomNumber==2)&&(!roomChanged))
  {

    if ((playerLocateX>2000)&&(playerLocateX<2550))
    {
      playerTileX = 3;
      playerTileY = 1;
      modX = 0;
      roomNumber = 6;
      roomChanged = true;
    }
    if (playerLocateX<100)
    {
      playerTileX = 11;
      playerTileY = 2;
      modX = -360;
      roomNumber = 1;
      roomChanged = true;
    }

    if (playerLocateX>2600)
    {
      playerTileX = 4;
      playerTileY = 10;
      modX = 520;
      roomNumber = 4;
      roomChanged = true;
    }
  }

  if((roomNumber==3)&&(!roomChanged))
  {
    if ((playerLocateX<1000))
    {
      playerTileX = 21;
      playerTileY = 22;
      roomNumber = 10;
      modX = -360;
      roomChanged = true;
    }
    if (playerLocateX>1000)
    {
      playerTileX = 1;
      playerTileY = 10;
      roomNumber = 1;
      modX = 520;
      roomChanged = true;
    }
  }

  if((roomNumber==4)&&(!roomChanged))
  {
    if (playerLocateY>1000)
    {
      playerTileX = 21;
      playerTileY = 1;
      roomNumber = 2;
      modX = -100;
      roomChanged = true;
    }

    if ((playerLocateY<1000)&&(playerLocateX<500))
    {
      playerTileX = 8;
      playerTileY = 4;
      roomNumber = 8;
      modX = -360;
      roomChanged = true;
    }

    if ((playerLocateY<1000)&&(playerLocateX>1000))
    {
      playerTileX = 1;
      playerTileY = 10;
      roomNumber = 5;
      modX = 520;
      roomChanged = true;
    }
  }

  if((roomNumber==5)&&(!roomChanged))
  {
    playerTileX = 11;
    playerTileY = 3;
    roomNumber = 4;
    modX = -360;
    roomChanged = true;
  }

  if(roomNumber==6)
  {
    if (playerLocateY<500)
    {
      playerTileX = 14;
      playerTileY = 4;
      roomNumber = 2;
      modX = 0;
      roomChanged = true;
    }

    if (playerLocateY>1000)
    {
      playerTileX = 21;
      playerTileY = 2;
      roomNumber = 7;
      modX = 200;
      roomChanged = true;
    }
  }

  if((roomNumber==7)&&(!roomChanged))
  {

    if (playerLocateX<500)
    {
      playerTileX = 22;
      playerTileY = 4;
      modX = -460;
      roomNumber = 9;
      roomChanged = true;
    }


    if (playerLocateX>2600)
    {
      playerTileX = 2;
      playerTileY = 20;
      modX = 0;
      roomNumber = 6;
      roomChanged = true;
    }
  }

  if((roomNumber==8)&&(!roomChanged))
  {

    if (playerLocateX<600)
    {
    playerTileX = 20;
    playerTileY = 22;
    modX = -320;
    roomNumber = 11;
    roomChanged = true;
    }

    if (playerLocateX>600)
    {
    playerTileX = 2;
    playerTileY = 1;
    modX = 520;
    roomNumber = 4;
    roomChanged = true;
    }
  }

  if((roomNumber==9)&&(!roomChanged))
  {

    if (playerLocateX<600)
    {
      playerTileX = 3;
      playerTileY = 22;
      modX = 520;
      roomNumber = 10;
      roomChanged = true;
    }

    if (playerLocateX>1000)
    {
      playerTileX = 1;
      playerTileY = 3;
      modX = 520;
      roomNumber = 7;
      roomChanged = true;
    }
  }

  if((roomNumber==10)&&(!roomChanged))
  {
    if (playerLocateX<600)
    {
      playerTileX = 2;
      playerTileY = 2;
      modX = 100;
      roomNumber = 9;
      roomChanged = true;
    }

    if (playerLocateX>1000)
    {
      playerTileX = 5;
      playerTileY = 2;
      modX = -320;
      roomNumber = 3;
      roomChanged = true;
    }
  }

  if((roomNumber==11)&&(!roomChanged))
  {
    playerTileX = 2;
    playerTileY = 2;
    modX = 320;
    roomNumber = 8;
    roomChanged = true;
  }

  if(swapRoom)
  swapRoom = false;

  getRoom();
}

void getRoom()
{
  getRoomDimensions(roomNumber);
  populateRoom();
}

// RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR

void populateRoom()
{
  for (uint8_t y = 0; y < 24; y++) // clear room space
  {
    for (uint8_t x = 0; x < 24; x++)
    {
      roomTiles[x][y] = 0;
    }
  }

  enemyExist[0] = 0;
  enemyExist[1] = 0;
  enemyExist[2] = 0;

  droneExist[0] = false;
  droneExist[1] = false;
  droneExist[2] = false;

  bossExist = false;

  lookIconExist = false;
  doubleIconExist = false;
  swordIconExist = false;
  gunIconExist = false;

  roomDimWidth = roomTileWidth * 160;
  roomDimHeight = roomTileHeight * 160;
  roomDimCentreX = roomDimWidth / 2;
  roomDimCentreY = roomDimHeight / 2;

  roomReset();

  borX = 22;
  borY = 23;

  drawBorder();

  roomTiles[0][0] = 30;
  roomTiles[0][roomTileHeight-1] = 31;
  roomTiles[roomTileWidth-1][0] = 28;
  roomTiles[roomTileWidth-1][roomTileHeight-1] = 29;

  if ((roomNumber==4)||(roomNumber==5)||(roomNumber==8))
  {
    borX = 32;
    borY = 32;

    drawBorder();
  }

  if(roomNumber == 11)
  {
    borX = 0;
    borY = 0;

    drawBorder();
  }

  fillRoom(roomNumber);
}

void drawBorder()
{
  for (uint8_t x = 0; x < roomTileWidth; x++) {
    roomTiles[x][0] = borX;
    roomTiles[x][roomTileHeight-1] = borX;
  }

  for (uint8_t y = 0; y < roomTileHeight; y++) {
    roomTiles[0][y] = borY;
    roomTiles[roomTileWidth-1][y] = borY;
  }
}


void animations()
{
  counter++;
  if (counter == 6)
  counter = 1;

  if (displayDelay>0)
  displayDelay--;

  if (counter==5)
  {
    timer++;
    animSlow3++;
    if (animSlow3 == 4)
    {
      animSlow3 = 1;
      animVerySlow3++;
      if (animVerySlow3 == 4)
      animVerySlow3 = 1;
    }
  }

  timer10 = timer / 10;

  anim3++;
  if (anim3 == 4)
  anim3 = 1;

  if (playerDeathCountdown>0)
  {
    playerDeathCountdown--;
    if (playerDeathCountdown==1)
    die();
  }

  if (playerWinCountdown>0)
  {
    playerWinCountdown--;
    if (playerWinCountdown==1)
    win();
  }

  if (playerAttackCountdown>0)
  {
    playerAttackCountdown--;
  }

  if (screenChangeCountdown>0)
  {
    screenChangeCountdown--;
  }

  if (spawnBitCountdown>0)
  {
    spawnBitCountdown--;
  }

  if (shakeScreenCountdown>0)
  {
    shakeScreenCountdown--;
  }
}

void update()
{
  animations();

  if (playerDeathCountdown==0)
  {
    detectTiles();
    getInput();
    calcScroll();
    detectRoom();
  }

  applyMovement();
}

void draw()
{
  drawPlayer();
  drawTiles();
  drawEnemies();
  drawDrones();
  drawSkills();

  if((swordSkill)&&playerDeathCountdown==0)
  drawSword();

  if(gunSkill)
  {
    if(playerDeathCountdown==0)
    drawBow();
    drawArrows();
  }

  if (bossExist>0)
  drawBoss();
  drawBits();
  drawHud();

  if(screenChangeCountdown)
    arduboy.clear();

  arduboy.display();
}

void detectTiles()
{
  // ray cast

  // detect right
  tileRight = true;
  if((roomTiles[(playerLocateX+160)/160][(playerLocateY+60)/160] < 21)&&(roomTiles[(playerLocateX+160)/160][(playerLocateY+90)/160] < 21))
  tileRight = false;


  // detect left
  tileLeft = true;
  if((roomTiles[(playerLocateX-0)/160][(playerLocateY+60)/160] < 21)&&(roomTiles[(playerLocateX-0)/160][(playerLocateY+90)/160] < 21))
  tileLeft = false;



  // detect above
  tileAbove = true;

  if((roomTiles[(playerLocateX+30)/160][(playerLocateY+10)/160] < 21)&&(roomTiles[(playerLocateX+130)/160][(playerLocateY+10)/160] < 21))
  tileAbove = false;


  // detect below
  tileBelow = true;

  if((roomTiles[(playerLocateX+30)/160][(playerLocateY+160)/160] < 21)&&(roomTiles[(playerLocateX+130)/160][(playerLocateY+160)/160] < 21))
  tileBelow = false;

}

void getInput()
{

  if (arduboy.pressed(RIGHT_BUTTON))
  {
    stoppedX = false;
	  if (!faceRight)
    {
      faceRight = true;
    }
      if (playerVeloX<5)
      playerVeloX = playerVeloX + 1;
      if (playerVeloX>4&&playerVeloX<10)
      playerVeloX = playerVeloX + 2;
  }

  if (arduboy.notPressed(RIGHT_BUTTON))
  {
    if (playerVeloX>0)
    playerVeloX = playerVeloX - 1;
  }

  if ((faceRight)&&(tileRight))
  {
    playerVeloX = 0;
    stoppedX = true;
  }

  if (arduboy.pressed(LEFT_BUTTON))
	{
    stoppedX = false;
    if(faceRight)
    {
      faceRight = false;
    }
      if (playerVeloX>-5)
      playerVeloX = playerVeloX - 1;
      if (playerVeloX<-4&&playerVeloX>-10)
      playerVeloX = playerVeloX - 2;
  }

  if (arduboy.notPressed(LEFT_BUTTON))
  {
    if (playerVeloX<0)
    playerVeloX = playerVeloX + 1;
  }

  if ((!faceRight)&&(tileLeft))
  {
    playerVeloX = 0;
    stoppedX = true;
  }

  if (arduboy.notPressed(RIGHT_BUTTON)&&arduboy.notPressed(LEFT_BUTTON))
  {
    stoppedX = true;
  }

  // Jumping //////////////////////////////////////////////////////////////////////////

  if (arduboy.pressed(A_BUTTON)&&(!jumping)&&(land)&&(!aPressed))// jump start
  {
    aPressed = true;
    jumping = true;
    land = false;
    falling = false;
    firstJump = true;
    secondJump = false;
    sound.tones(jumpFx);
  }

  if (arduboy.pressed(A_BUTTON)&&(doubleSkill)&&(!aPressed)&&(firstJump)&&(!secondJump))// jump start
  {
    aPressed = true;
    jumping = true;
    land = false;
    falling = false;
    secondJump = true;
    firstJump = false;
    sound.tones(jumpFx);
  }

  if (arduboy.pressed(A_BUTTON)&&(jumping)&&(!tileAbove))
  {
    aPressed = true;
    if (playerVeloY>-28)
    {
      playerVeloY = playerVeloY - 4;
    }
    else
    {
      jumping = false;
    }
  }

  if((tileAbove)&&(!jumping))
  {
    playerVeloY = 0;
    falling = true;
  }

  if((tileAbove)&&(jumping))
  {
    playerVeloY = 0;
    falling = true;
    jumping = false;
    sound.tones(bumpFx);
  }

  if (arduboy.notPressed(A_BUTTON))
  {
    jumping = false;
    aPressed = false;
  }

  if (swordSkill)
  {
    if (arduboy.pressed(B_BUTTON)&&(playerAttackCountdown==0)&&(!bPressed))
    {
      playerAttackCountdown = 15;
      sound.tones(swingFx);
      shakeScreenCountdown = 5;
      bPressed = true;
    }

    if (arduboy.notPressed(B_BUTTON))
    bPressed = false;
  }

  if (gunSkill)
  {
    if (arduboy.pressed(B_BUTTON)&&(!bPressed))
    {
      bPressed = true;
      spawnArrows();
    }

    if (arduboy.notPressed(B_BUTTON))
    bPressed = false;
  }

  if(lookSkill)
  {
    if (arduboy.pressed(DOWN_BUTTON)&&scrollY<roomDimHeight-320)
    {
	    scrollY = scrollY + 30;
    }
    if (arduboy.pressed(UP_BUTTON)&&scrollY>320)
	  {
      scrollY = scrollY - 30;
    }
  }
}

void applyMovement()
{
  applyGravity();

  moveRoom();

  if(playerDeathCountdown==0)
  movePlayer();

  if(gunSkill)
  moveArrows();

  moveEnemies();
  moveDrones();

  if (bossExist>0)
  moveBoss();

  if (spawnBitCountdown>0)
  {
    spawnBits();
  }

  if (shakeScreenCountdown>0)
  {
    shakeScreen(); 
  }
  else
  {
    screenPushX = 0;
    screenPushY = 0;
  }

  moveBits();
}

void applyGravity()
{
  if ((!land)&&playerLocateY<roomDimHeight)
  {
    falling = true;
    if (playerVeloY<10)
    playerVeloY = playerVeloY + 2;
  }
}

void spawnArrows()
{
  for (int i = 0; arrowNumber > i; i ++)
  {
    if ((!arrowExist[i])&&(playerAttackCountdown==0))
    {
        sound.tones(swingFx);
        shakeScreenCountdown = 5;
        arrowExist[i] = true;
        playerAttackCountdown=3;

        if (faceRight)
        arrowGoRight[i] = true;
        else
        arrowGoRight[i] = false;

        arrowX[i] = playerLocateX+80;
        arrowY[i] = playerLocateY+110;
    }
  }
}

void moveArrows()
{
  for (int i = 0; arrowNumber > i; i ++)
  {
    if(arrowExist[i])
    {
      if(arrowGoRight[i])
      arrowX[i] = arrowX[i] + 32;
      else
      arrowX[i] = arrowX[i] - 32;


      if (arrowX[i] < 0)
      arrowExist[i] = false;

      if (arrowX[i] > roomDimWidth)
      arrowExist[i] = false;

      if(roomTiles[(arrowX[i])/160][(arrowY[i])/160] > 20)
      arrowExist[i] = false;
    }
  }
}

void drawArrows()
{
  for (uint8_t i = 0; arrowNumber > i; i ++)
  {
    if(arrowExist[i])
    {
      arrowScreenX[i] = arrowX[i] / 10;
      arrowScreenY[i] = arrowY[i] / 10;

      if(arrowGoRight[i])
      Sprites::drawSelfMasked((arrowScreenX[i]-3)-screenX, (arrowScreenY[i]-3)-screenY, arrowRight,0);
      else
      Sprites::drawSelfMasked((arrowScreenX[i]-3)-screenX, (arrowScreenY[i]-3)-screenY, arrowLeft,0);
    }
  }
}

void spawnBits()
{
  for(uint8_t i = 0; bitNumber > i; i++)
  {
    if(bitExist[i]==0)
    {
      bitExist[i] = 50;

      if(bitLand)
      {
        bitX[i]=playerLocateX+80;
        bitY[i]=playerLocateY+160;
      }

      if(bitHit)
      {
        bitX[i]=playerLocateX+80;
        bitY[i]=playerLocateY+80;
      }

      if(bitDie)
      {
        bitX[i]=playerLocateX+80;
        bitY[i]=playerLocateY+80;
      }

      if(bitArrow)
      {
        bitX[i]=bitPlaceX;
        bitY[i]=bitPlaceY;
      }
    }
  }
}

void moveBits()
{
  int r;
  int right;
  int left;
  int down;
  int up;

  for(int i = 0; bitNumber > i; i++)
  {
    if(bitExist[i]>0)
    {
      r = random(RAND_MAX) % 4;

      if(bitLand)
      {
        right = 15;
        left = 15;
        down = 5;
        up = 10;
      }

      if(bitHit)
      {
        right = 20;
        left = 20;
        down = 20;
        up = 20;
      }

      if(bitDie)
      {
        right = 50;
        left = 50;
        down = 50;
        up = 50;
      }

      if(bitArrow)
      {
        right = 10;
        left = 10;
        down = 10;
        up = 10;
      }

      if(r==0)
      bitX[i] = bitX[i] + right;
      if(r==1)
      bitX[i] = bitX[i] - left;
      if(r==2)
      bitY[i] = bitY[i] + down;
      if(r==3)
      bitY[i] = bitY[i] - up;

      bitExist[i]--;
    }
  }

  if (spawnBitCountdown==0){
    bitLand = false;
    bitHit = false;
    bitDie = false;
    bitArrow = false;}
}

void drawBits()
{
  for(uint8_t i = 0; bitNumber > i; i++)
  {
    if(bitExist[i]>0)
    {
      bitScreenX[i] = bitX[i]/10;
      bitScreenY[i] = bitY[i]/10;

      if(bitExist[i] > 40)
        Sprites::drawSelfMasked((bitScreenX[i]-3)-screenX, (bitScreenY[i]-3)-screenY, bit5,0);
      
      if((bitExist[i] < 41)&&(bitExist[i] > 30))
        Sprites::drawSelfMasked((bitScreenX[i]-2)-screenX, (bitScreenY[i]-2)-screenY, bit4,0);
      
      if((bitExist[i] < 31)&&(bitExist[i] > 20))
        Sprites::drawSelfMasked((bitScreenX[i]-1)-screenX, (bitScreenY[i]-1)-screenY, bit3,0);
      
      if((bitExist[i] < 21)&&(bitExist[i] > 10))
        Sprites::drawSelfMasked((bitScreenX[i])-screenX, (bitScreenY[i])-screenY, bit2,0);
      
      if(bitExist[i] < 11)
        arduboy.drawPixel((bitScreenX[i])-screenX, (bitScreenY[i])-screenY,1);
    }
  }
}

void shakeScreen()
{
  int r = random(RAND_MAX) % 4;

  //                        Coutdown > 0     Coutdown = 0
  const int8_t shakeX[] = { 7, -7, 0,  0,    1, -1, 0,   0 };
  const int8_t shakeY[] = { 0,  0, 3, -3,    0,  0, 3, -10 };

  if(playerAttackCountdown == 0) r = r + 4;

  screenPushX = screenPushX + shakeX[r];
  screenPushY = screenPushY + shakeY[r];
}


void moveRoom()
{
  scrollScreenX = scrollX/10;
  scrollScreenY = scrollY/10;
}

void movePlayer()
{
  offsetX = screenX-offsetAjustX;
  offsetY = screenY-offsetAjustY;  

  playerLocateX = playerLocateX + playerVeloX;
  
  if (playerLocateX<0)
  {
    playerLocateX=0;
    playerVeloX=0;
  }

  if (playerLocateX>roomDimWidth-160)
  {
    playerLocateX=roomDimWidth-160;
    playerVeloX=0;
  }

  if (!stoppedX)
  scrollLeaderX = playerVeloX * 27;

  if(faceRight&&scrollX<roomDimWidth-640) // X scrolling
  {
    if (playerLocateX>640)
    {
      scrollX = playerLocateX+scrollLeaderX;
    }
  }

  if(!faceRight&&scrollX>640) // X Scrolling
  {
    if (playerLocateX<roomDimWidth-640)
    {
      scrollX = playerLocateX+scrollLeaderX;
    }
  }

  ///////////////////

  playerLocateY = playerLocateY + playerVeloY;

  if (playerLocateY<0)
  {
    playerLocateY=0;
    playerVeloY=0;
  }

  if ((tileBelow)&&(!jumping)) // land on tile
  {
    playerVeloY=0;
    if (!land)
    {
      sound.tones(landFx);
      bitLand = true;
      if(spawnBitCountdown==0)
      {
        spawnBitCountdown = 15;
      }
      if(shakeScreenCountdown==0)
      {
        shakeScreenCountdown = 10;
      }
    }
    land = true;
    falling = false;
  }
  else
  {
    land = false;
    falling = true;
  }


  if (playerLocateY>roomDimHeight-160)// land on bottom of screen
  {
    playerLocateY=roomDimHeight-160;
    playerVeloY=0;
    if (!land)
    sound.tones(landFx);
    land = true;
    falling = false;
  }

  // Y scroll movement

  if (playerLocateY>320&&arduboy.notPressed(DOWN_BUTTON)&&arduboy.notPressed(UP_BUTTON))
  scrollY = playerLocateY;

  if ((!jumping)&&(!falling)&&playerLocateY<321&arduboy.notPressed(DOWN_BUTTON)&&arduboy.notPressed(UP_BUTTON))
  scrollY = playerLocateY+160;

  // Scale Cords to screen

  playerScreenX = (playerX-roomDimCentreX+playerLocateX)/10; // screen location, - distance from room corner, + position in room
  playerScreenY = (playerY-roomDimCentreY+playerLocateY)/10;
}

void moveEnemies()
{
  for (int i = 0; enemyNumber > i; i++)
  {
    if (enemyExist[i]>0) // Enemy AI
    {
      // detect right
      if (enemyGoRight[i])
      {
        if(roomTiles[(enemyX[i]+170)/160][(enemyY[i]+80)/160] > 20) // single point at right (Full)
        enemyGoRight[i] = false;
      
        if(roomTiles[(enemyX[i]+170)/160][(enemyY[i]+170)/160] < 21) // single point at right down (Empty)
        enemyGoRight[i] = false;
      
      }

      // detect left
      // if (!enemyGoRight[i])
      else {
        if(roomTiles[(enemyX[i]-10)/160][(enemyY[i]+80)/160] > 20) // single point at left (Full)
        enemyGoRight[i] = true;

        if(roomTiles[(enemyX[i]-10)/160][(enemyY[i]+170)/160] < 21) // single point at left down (Empty)
        enemyGoRight[i] = true;
      }

      if (enemyGoRight[i])
      {
        enemyX[i]=enemyX[i] + 5;
      }

      // if (!enemyGoRight[i])
      else
      {
        enemyX[i]=enemyX[i] - 5;
      }
    }
  }
}

void moveDrones()
{
  for (int i = 0; droneNumber > i; i++)
  {
  if (droneExist[i]) // Enemy AI
  {

    // detect right
    if (droneGoRight[i])
    {
      if(roomTiles[(droneX[i]+170)/160][(droneY[i]+80)/160] > 20) // single point at right (Full)
      droneGoRight[i] = false;
    }

    // detect left
    // if (!droneGoRight[i])
    else
    {
      if(roomTiles[(droneX[i]-10)/160][(droneY[i]+80)/160] > 20) // single point at left (Full)
      droneGoRight[i] = true;
    }

    int r = random(RAND_MAX) % 3;

    if ((r==1)&&(roomTiles[(droneX[i]+80)/160][(droneY[i]-10)/160] < 21))
    droneY[i] = droneY[i] - 10;

    if ((r==2)&&(roomTiles[(droneX[i]+80)/160][(droneY[i]+90)/160] < 21))
    droneY[i] = droneY[i] + 10;

    if (droneGoRight[i])
    {
      droneX[i]=droneX[i] + 5;
    }

    // if (!droneGoRight[i])
    else
    {
      droneX[i]=droneX[i] - 5;
    }
  }
  }
}


void moveBoss()
{
    // detect right
    if (bossGoRight)
    {
      if(roomTiles[(bossX+330)/160][(bossY+160)/160] > 20) // single point at right (Full)
      bossGoRight = false;
    }

    // detect left
    // if (!bossGoRight)
    else
    {
      if(roomTiles[(bossX-10)/160][(bossY+160)/160] > 20) // single point at left (Full)
      bossGoRight = true;
    }

    if (bossGoRight)
    {
      bossX = bossX + 2;
    }

    // if (!bossGoRight)
    else
    {
      bossX = bossX - 2;
    }

  int r = random(RAND_MAX) % 100; // spawn drones on boss
  if (r==0)
  {
    for (int i = 0; droneNumber > i; i++)
    {
      if (!droneExist[i])
      {
        droneExist[i] = true;
        droneX[i] = bossX;
        droneY[i] = bossY-16;
      }
    }
  }

  bossScreenX = bossX / 10;
  bossScreenY = bossY / 10;

  Rect bossCol((bossScreenX-16)-screenX,bossScreenY-screenY, 62, 33); // Player / enemy
  Rect playerCol(playerScreenX-offsetX,playerScreenY-offsetY, 16, 16);
  if (arduboy.collide(bossCol, playerCol))
  {
    startDeath();
  }


  for(uint8_t i = 0; arrowNumber > i; i++)
  {
    if (arrowExist[i])
    {
      Rect arrowCol(arrowScreenX[i]-screenX,arrowScreenY[i]-screenY, 8  , 3);
      if (arduboy.collide(bossCol, arrowCol))
      {
        sound.tones(hitFx);
        bossExist--;
        bossBack();
        arrowExist[i] = false;
        bitArrow = true;
        bitPlaceX=arrowX[i];
        bitPlaceY=arrowY[i];
        spawnBitCountdown = 50;
      }
    }
  }

  if ((playerAttackCountdown>0)&&(swordSkill))
  {
    Rect swordCol((playerScreenX-10)-offsetX,playerScreenY-offsetY, 36  , 16);
    if (arduboy.collide(bossCol, swordCol))
    {
     sound.tones(hitFx);
      bossExist--;
      bossBack();
      bitHit = true;
      spawnBitCountdown = 50;
    }
  }

  if (bossExist==0)
  playerWinCountdown = 100;
}

void bossBack()
{
  if (playerLocateX>bossX)
  {
    bossX = bossX - 32;
  }

  if (playerLocateX<bossX)
  {
    bossX = bossX + 32;
  }
}

void drawBoss()
{
  Sprites::drawSelfMasked((bossScreenX+32)-screenX,(bossScreenY+16+animSlow3)-screenY, bossFoot, 0);  
  Sprites::drawSelfMasked(bossScreenX-screenX,bossScreenY-screenY, boss, 0);
  Sprites::drawSelfMasked((bossScreenX-16)-screenX,(bossScreenY+16+animSlow3)-screenY, bossFoot, 0);
}

void calcScroll()
{
  porDestX = scrollX - 640; // 640
  if (portalX<porDestX)
  portalXvelo = (porDestX - portalX)/driftSpeedX;
  if (portalX>porDestX)
  portalXvelo = (porDestX - portalX)/driftSpeedX;
  portalX = portalX + portalXvelo;

  porDestY = scrollY - 320;
  if (portalY<porDestY)
  portalYvelo = (porDestY - portalY)/driftSpeedY;
  if (portalY>porDestY)
  portalYvelo = (porDestY - portalY)/driftSpeedY;
  portalY = portalY + portalYvelo;

  portalX = portalX + screenPushX;
  portalY = portalY + screenPushY;  // scroll down mod

  screenX = portalX/10;
  screenY = portalY/10;
}

void drawTiles()
{
  for (int y = 0; y < 24; y++)
  {
    for (int x = 0; x < 24; x ++)
    {
      if (x*16>screenX-16&&x*16<screenX+144&&y*16>screenY-16&&y*16<screenY+80) // only draw blocks in view
      {

        uint8_t tileNumber = roomTiles[x][y];

        switch (tileNumber) {

          case 21 ... 35:
            Sprites::drawOverwrite((x*16)-screenX,(y*16)-screenY, tiles, tileNumber - 21);  
            break;

          case 7:
            Sprites::drawSelfMasked((x*16)-screenX,(y*16)-screenY, acid, animSlow3 - 1);
            break;

          case 8:
            Sprites::drawExternalMask((x*16)-screenX,(y*16)-screenY, spikeFloor, spikeFloorMask,0, 0);
            break;

          case 9:
            Sprites::drawExternalMask((x*16)-screenX,(y*16)-screenY, spikeRoof, spikeRoofMask,0, 0);
            break;

          case 12:
            Sprites::drawSelfMasked(((x*16)+7)-screenX,(y*16)-screenY, plant, animVerySlow3 - 1);
            break;

          case 13:
            Sprites::drawSelfMasked((x*16)-screenX,((y*16)+8)-screenY, mushrooms, 0);
            break;

          case 14:
            Sprites::drawSelfMasked(((x*16)+3)-screenX,(y*16)-screenY, vines, animVerySlow3 - 1);
            break;

          case 15:
            Sprites::drawSelfMasked((x*16)-screenX,(y*16)-screenY, mushroom, 0);
            break;

          case 16:
            Sprites::drawSelfMasked((x*16)-screenX,(y*16)-screenY, branch, 0);
            break;

          case 17:
            Sprites::drawSelfMasked((x*16)-screenX,((y*16)+6)-screenY, sideBranch, 0);
            break;
          
        }
      }
    }
  }
}

void drawEnemies()
{
  for (int i = 0; enemyNumber > i; i++)
  {
  if (enemyExist[i]>0)
  {
    enemyScreenX[i] = enemyX[i]/10;
    enemyScreenY[i] = enemyY[i]/10;

    Sprites::drawExternalMask(enemyScreenX[i]-screenX, enemyScreenY[i]-screenY, enemy1, enemyMask1, 0,  0);
    
    if (enemyGoRight[i])
    Sprites::drawSelfMasked((enemyScreenX[i]+9)-screenX, (enemyScreenY[i]+2)-screenY, eyes,  0);
    else
    Sprites::drawSelfMasked((enemyScreenX[i]+2)-screenX, (enemyScreenY[i]+2)-screenY, eyes,  0);


    if(playerDeathCountdown==0)
    {
      Rect enemyCol(enemyScreenX[i]-screenX,enemyScreenY[i]-screenY, 16, 16); // Player / enemy
      Rect playerCol(playerScreenX-offsetX,playerScreenY-offsetY, 16, 16);
      if (arduboy.collide(enemyCol, playerCol))
      {
        startDeath();
      }

      if (playerAttackCountdown>0)
      {
        Rect swordCol((playerScreenX-10)-offsetX,playerScreenY-offsetY, 36  , 16);
        if (arduboy.collide(enemyCol, swordCol))
        {
          sound.tones(hitFx);
          enemyExist[i]--;
          enemySee = i;
          enemyBack();
          bitHit = true;
          spawnBitCountdown = 50;
        }
      }

      if (gunSkill)
      {
        for (int c = 0; arrowNumber > c; c ++)
        {
          if(arrowExist[c])
          {
            Rect arrowCol(arrowScreenX[c]-screenX,arrowScreenY[c]-screenY, 8  , 3);
            if (arduboy.collide(enemyCol, arrowCol))
            {
              sound.tones(hitFx);
              enemyExist[i]--;
              enemySee = i;
              enemyBack();
              arrowExist[c] = false;
              bitArrow = true;
              bitPlaceX=arrowX[c];
              bitPlaceY=arrowY[c];
              spawnBitCountdown = 50;
            }
          }
        }
      }
    }
  }
  }
}

void enemyBack()
{
  if(enemyX[enemySee]>playerLocateX)
  enemyX[enemySee] = enemyX[enemySee] + 48;

  if(enemyX[enemySee]<playerLocateX)
  enemyX[enemySee] = enemyX[enemySee] - 48;
}

void drawDrones()
{
  for (int i = 0; droneNumber > i; i++)
  {
  if (droneExist[i])
  {
    droneScreenX[i] = droneX[i]/10;
    droneScreenY[i] = droneY[i]/10;

    Sprites::drawSelfMasked(droneScreenX[i]-screenX, droneScreenY[i]-screenY, drone,  animSlow3 -1);

    if(playerDeathCountdown==0)
    {
      Rect droneCol(droneScreenX[i]-screenX,droneScreenY[i]-screenY, 16, 8); // Player / enemy
      Rect playerCol(playerScreenX-offsetX,playerScreenY-offsetY, 16, 16);
      if (arduboy.collide(droneCol, playerCol))
      {
        startDeath();
      }

      if (playerAttackCountdown>0)
      {
        Rect swordCol((playerScreenX-10)-offsetX,playerScreenY-offsetY, 36  , 16);
        if (arduboy.collide(droneCol, swordCol))
        {
          sound.tones(hitFx);
          droneExist[i] = false;
          bitHit = true;
          spawnBitCountdown = 50;
        }
      }

      if (gunSkill)
      {
        for (int c = 0; arrowNumber > c; c ++)
        {
          if(arrowExist[c])
          {
            Rect arrowCol(arrowScreenX[c]-screenX,arrowScreenY[c]-screenY, 8  , 3);
            if (arduboy.collide(droneCol, arrowCol))
            {
              sound.tones(hitFx);
              droneExist[i] = false;
              arrowExist[c] = false;
              bitArrow = true;
              bitPlaceX=arrowX[c];
              bitPlaceY=arrowY[c];
              spawnBitCountdown = 50;
            }
          }
        }
      }
    }
  }
  }
}

void drawSword()
{
  if (faceRight)
  {
    if (playerAttackCountdown == 0)
    Sprites::drawExternalMask((playerScreenX-9)-offsetX, playerScreenY-offsetY, swordLeft, swordLeftMask, 0,  0);
    else
    {
      if (playerAttackCountdown>2)
      Sprites::drawSelfMasked((playerScreenX+7)-offsetX, playerScreenY-offsetY, swingRight2, 0);
      else
      Sprites::drawSelfMasked((playerScreenX+16)-offsetX, playerScreenY-offsetY, sparkle, 0);
    }
  }

  if (!faceRight)
  {
    if (playerAttackCountdown == 0)
    Sprites::drawExternalMask((playerScreenX+9)-offsetX, playerScreenY-offsetY, swordRight,swordRightMask,0, 0);
    else
    {
      if (playerAttackCountdown>2)
      Sprites::drawSelfMasked((playerScreenX-10)-offsetX, playerScreenY-offsetY, swingLeft2, 0);
      else
      Sprites::drawSelfMasked((playerScreenX-10)-offsetX, playerScreenY-offsetY, sparkle, 0);
    }
  }
}

void drawBow()
{
  if (faceRight)
  {
    Sprites::drawExternalMask((playerScreenX+5)-offsetX, (playerScreenY+7)-offsetY, bowRight, bowRightMask, 0,  0);
  }

  if (!faceRight)
  {
    Sprites::drawExternalMask((playerScreenX-5)-offsetX, (playerScreenY+7)-offsetY, bowLeft,bowLeftMask,0, 0);
  }
}

void  drawSkills()
{
  Rect playerCol(playerScreenX-offsetX,playerScreenY-offsetY, 16, 16);

  if (lookIconExist)
  {
    Sprites::drawOverwrite((lookIconX+4)-screenX, (lookIconY+4+animSlow3)-screenY, look1,0);

    Rect iconCol((lookIconX+4)-screenX,(lookIconY+4)-screenY, 8, 8); // Player / enemy

    if (arduboy.collide(iconCol, playerCol))
    {
      lookIconExist = false;
      sound.tones(grabFx);
      lookSkill = true;
      displayDelay = 2000;
      textRef = 1;
    }
  }

  if (doubleIconExist)
  {
    Sprites::drawOverwrite((doubleIconX+4)-screenX, (doubleIconY+4+animSlow3)-screenY, double1,0);

    Rect iconCol((doubleIconX+4)-screenX,(doubleIconY+4)-screenY, 8, 8); // Player / enemy

    if (arduboy.collide(iconCol, playerCol))
    {
      doubleIconExist = false;
      sound.tones(grabFx);
      doubleSkill = true;
      displayDelay = 2000;
      textRef = 2;
    }  
  }

  if (swordIconExist)
  {
    Sprites::drawOverwrite((swordIconX+4)-screenX, (swordIconY+4+animSlow3)-screenY, sword1,0);

    Rect iconCol((swordIconX+4)-screenX,(swordIconY+4)-screenY, 8, 8); // Player / enemy

    if (arduboy.collide(iconCol, playerCol))
    {
      swordIconExist = false;
      sound.tones(grabFx);
      swordSkill = true;
      gunSkill = false;
      displayDelay = 2000;
      textRef = 3;
    }  
  }

  if (gunIconExist)
  {
    Sprites::drawOverwrite((gunIconX+4)-screenX, (gunIconY+4+animSlow3)-screenY, bow1,0);

    Rect iconCol((gunIconX+4)-screenX,(gunIconY+4)-screenY, 8, 8); // Player / enemy
    Rect playerCol(playerScreenX-offsetX,playerScreenY-offsetY, 16, 16);
    if (arduboy.collide(iconCol, playerCol))
    {
      gunIconExist = false;
      sound.tones(grabFx);
      gunSkill = true;
      swordSkill = false;
      displayDelay = 2000;
      textRef = 5;
    }  
  }
}

void drawPlayer()
{
  if (playerDeathCountdown==0)
  {

  int yAj = 0;
  if(!stoppedX)
  {
    if((animSlow3==2)&&(land))
    yAj = 1;
    else
    yAj = 0;
  }

  if (faceRight)
  {
    
    Sprites::drawOverwrite(playerScreenX-offsetX, (playerScreenY+yAj)-offsetY, shroomRightHead,0);

    if (land)
    {
      if(stoppedX)
      {
        Sprites::drawOverwrite((playerScreenX+3)-offsetX, (playerScreenY+11)-offsetY, shroomBodyRight, 1);
      }
      else
      {
        Sprites::drawOverwrite((playerScreenX+3)-offsetX, (playerScreenY+11)-offsetY, shroomBodyRight, animSlow3 - 1);
      }
    }

    if (!land)
    {
      Sprites::drawOverwrite((playerScreenX+4)-offsetX, (playerScreenY+11)-offsetY, shroomJump,0);      
    }
  }
  
  if (!faceRight)
  {

    Sprites::drawOverwrite(playerScreenX-offsetX, (playerScreenY+yAj)-offsetY, shroomLeftHead, 0);

    if (land)
    {
      if(stoppedX)
      {
        Sprites::drawOverwrite((playerScreenX+3)-offsetX, (playerScreenY+11)-offsetY, shroomBodyLeft, 1);
      }
      else
      {
        Sprites::drawOverwrite((playerScreenX+3)-offsetX, (playerScreenY+11)-offsetY, shroomBodyLeft, animSlow3 - 1);
      }
    }

    if (!land)
    {
      Sprites::drawOverwrite((playerScreenX+4)-offsetX, (playerScreenY+11)-offsetY, shroomJump,0);   
    }
  }
  }
}

void drawHud()
{

  if(swordSkill)
  Sprites::drawOverwrite( 10, 1, sword1,0);

  if(gunSkill)
  Sprites::drawOverwrite( 10, 1, bow1,0);

  if(doubleSkill)
  Sprites::drawOverwrite( 19, 1, double1,0);

  if(lookSkill)
  Sprites::drawOverwrite( 28, 1, look1,0);

  Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

  if (displayDelay>0)
  {
    if (textRef == 1)
    {
      tinyfont.setCursor(20,20);
      tinyfont.print(F("LOOK SKILL"));
      tinyfont.setCursor(5,30);
      tinyfont.print(F("PRESS UP AND DOWN"));
    }

    if (textRef == 2)
    {
      tinyfont.setCursor(20,20);
      tinyfont.print(F("DOUBLE JUMP SKILL"));
      tinyfont.setCursor(25,30);
      tinyfont.print(F("HIT A TWICE"));
    }

    if (textRef == 3)
    {
      tinyfont.setCursor(40,20);
      tinyfont.print(F("SWORD SKILL"));
      tinyfont.setCursor(20,30);
      tinyfont.print(F("PRESS B TO ATTACK"));
    }

    if (textRef == 5)
    {
      tinyfont.setCursor(40,20);
      tinyfont.print(F("GUN SKILL"));
      tinyfont.setCursor(15,30);
      tinyfont.print(F("PRESS B TO SHOOT"));
    }

    if (textRef == 4)
    {
      tinyfont.setCursor(65,10);
      tinyfont.print(F("ESCAPE"));
      tinyfont.setCursor(55,15);
      tinyfont.print(F("THE TESTING"));
      tinyfont.setCursor(65,20);
      tinyfont.print(F("FACILITY"));
    }
  }

  arduboy.fillRect(1,1,8,8,0);
  arduboy.drawRect(1,1,8,8,1);
  tinyfont.setCursor(3,3);
  tinyfont.print(lives);

  arduboy.fillRect(100,1,27,8,0);
  arduboy.drawRect(100,1,27,8,1);
  tinyfont.setCursor(102,3);
  tinyfont.print(timer10);

  //arduboy.setCursor(0,40);
  //arduboy.print(roomNumber);

  //arduboy.setCursor(0,50);
  //arduboy.print(enemyScreenY[0]);

  //arduboy.setCursor(70,40);
  //arduboy.print(playerLocateX);
  //arduboy.setCursor(70,50);
  //arduboy.print(playerLocateY);
}

void startDeath()
{
  sound.tones(playerDieFx);
  playerDeathCountdown = 250;
  bitDie = true;
  spawnBitCountdown = 100;
  playerScreenX = -100;
  playerScreenY = -100;
}


void die()
{

  lives--;

  if (lives==0)
  gameOver();
  roomReset();
}

void roomReset()
{
  portalX = roomDimCentreX - 640;
  portalY = roomDimCentreY - 320;

  playerLocateX = (playerTileX*160) - 80;
  playerLocateY = (playerTileY*160) - 80;

  scrollX = playerLocateX + modX; // modX ajusts x cord when entering new room
  scrollY = playerLocateY;

  if (roomTileWidth == 24)
  offsetAjustX = 128;

  if(roomTileHeight == 24)
  offsetAjustY = 160;

  if (roomTileWidth == 12)
  offsetAjustX = 32;

  if(roomTileHeight == 12)
  offsetAjustY = 64;

  if (roomTileWidth == 6)
  offsetAjustX = -16;

  if(roomTileHeight == 6)
  offsetAjustY = 16;
}

void gameOver()
{
  Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());
  arduboy.clear();
  tinyfont.setCursor(5,30);
  tinyfont.print(F("YOU HAVE BEEN DESTROYED"));
  arduboy.display();
  arduboy.delayShort(3000);
  arduboy.clear();
  arduboy.display();
  resetVariables();
}

void win()
{
  Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());
  arduboy.clear();
  tinyfont.setCursor(20,10);
  tinyfont.print(F("YOU DEFEATED THE"));
  tinyfont.setCursor(40,20);
  tinyfont.print(F("MECHANICS"));
  tinyfont.setCursor(30,40);
  tinyfont.print(F("TIME PASSED:"));
  tinyfont.setCursor(30,50);
  tinyfont.print(timer);
  arduboy.display();
  arduboy.delayShort(6000);
  arduboy.clear();
  arduboy.display();
  playerWinCountdown = 0;
  resetVariables();
}

void resetVariables()
{
  roomNumber = 1;
  roomSelect = 0;
  roomRead = 0;

  playerTileX = 4;
  playerTileY = 2;

  modX = 440;
  
  playerX = 640;
  playerY = 310;

  lives = 3;
  scrollX = 0;
  scrollY = 0;
  faceRight = true;
  timer = 0;

  lookSkill = false;
  doubleSkill = false;
  swordSkill = false;
  gunSkill = false;

  startGame = false;
}
