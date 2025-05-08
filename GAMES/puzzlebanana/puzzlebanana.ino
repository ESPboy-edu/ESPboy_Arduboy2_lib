/*******************************************************************************
 Puzzle Banana v1.1  by Mr.Blinky April 2025
********************************************************************************

A game entry I wrote for the Arduboy Banana Jam April 2025.

This game is inspired by the old Mobile java game Puzzle Makai Mura.

You play the role of Wonkey Monkey and you are hungry. Unfortunately someone
has picked all the bananas from the trees but lucky for you some where dropped.
In each level you can find a banana to eat. Collect the banana by walking,
climbing and moving around using special blocks. After you ate the banana,
move to the recycle bin to throw the banana peel away and get ready for another
banana in the next level.

Controls:

LEFT /RIGHT - Walk left or right / move camera left right.
UP / DOWN   - Climb up and down ladders / move camera up /down.
B           - Move the block under you in the direction of the arrow.
A press     - Turn camera mode on / off
A hold      - Restarts the level

Camera Mode:

When turned on, the game is paused and animated arrows are displayed showing
in which directions you can move the camera. Use the directional buttons to
look around the level. Press A once more to turn camera mode off.

*******************************************************************************/

//uncomment next line to enable flashy LED effects when eating/throwing banana and dying
#define ENABLE_LED_FX

#include <Arduino.h>
#include <Arduboy2.h>
#include <Sprites.h>
#include "gfx/gfx.h"
#include "levels.h"

constexpr uint8_t frameRate = 50;
constexpr uint8_t startLevel = 1 - 1;

constexpr uint8_t maxLevelWidth  = 16;
constexpr uint8_t maxLevelHeight = 16;

constexpr uint8_t msTitleScreen         = 0;
constexpr uint8_t msTitleScreenWait     = 1;
constexpr uint8_t msShowLevelScreen     = 2;
constexpr uint8_t msShowLevelScreenWait = 3;
constexpr uint8_t msPlayLevel           = 4;
constexpr uint8_t msShowEndScreen       = 5;
constexpr uint8_t msHelpScreen          = 6;
constexpr uint8_t msShowCreditsScreen   = 7;

constexpr uint8_t psNoBanana  = 0;
constexpr uint8_t psGotBanana = 1;
constexpr uint8_t psAteBanana = 2;
constexpr uint8_t psNextLevel = 3;
constexpr uint8_t psDead      = 4;
constexpr uint8_t psRestart   = 5;

struct Camera
{
  int x;
  int y;
  int xOrg;
  int yOrg;
  int xmin;
  int xmax;
  int ymin;
  int ymax;
  uint8_t mode;
  uint8_t timer;
  uint8_t shift;
};

struct Level
{
  uint8_t nr;
  uint8_t width;
  uint8_t height;
  Tiles   map[maxLevelHeight * maxLevelWidth];
};

struct Player
{
  uint8_t x;
  uint8_t y;
  int8_t  xstep;
  int8_t  ystep;
  uint8_t frame;
  uint8_t state;
};

struct Banana
{
  uint8_t x;
  uint8_t y;
  uint8_t step;
  uint8_t frame;
};

struct Block
{
  uint8_t x;
  uint8_t y;
  int8_t  xstep;
  int8_t  ystep;
  Tiles   tile;
};

struct Finish
{
  uint8_t x;
  uint8_t y;
};

Camera cam;
Player player;
Banana banana;
Block block;
Finish finish;
Level level;

Arduboy2Base arduboy;
uint8_t mainState;

////////////////////////////////////////////////////////////////////////////////

void showTitleScreen()
{
  Sprites::drawOverwrite(0, 0, titleScreen, 0);
  if (arduboy.justPressed(B_BUTTON))
  {
    level.nr = startLevel;
    player.frame = frameRate;
    mainState = msShowLevelScreen;
  }
  if (arduboy.justPressed(A_BUTTON))
  {
    mainState = msHelpScreen;
  }
}

void showHelpScreen()
{
  Sprites::drawOverwrite(0, 0, helpScreen, 0);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    mainState = msTitleScreen;
  }
}

void showLevelScreen()
{
  uint8_t displayLevel = level.nr + 1;
  Sprites::drawOverwrite(7, 20, levelText, 0);
  Sprites::drawOverwrite(7 + 82, 20, numberSprites, displayLevel / 10);
  Sprites::drawOverwrite(7 + 82 + 17, 20, numberSprites, displayLevel % 10);
  if ((player.frame--) /* && (!(arduboy.currentButtonState & (A_BUTTON | B_BUTTON))) */ ) return;

  loadLevel(level.nr);
  mainState = msPlayLevel;
}

void showEndScreen()
{
  Sprites::drawOverwrite(8, 20, theEndText, 0);
  if (player.frame)
  {
    --player.frame;
  }
  else
  {
    player.frame = frameRate * 4.25;
    mainState = msShowCreditsScreen;
  }
}

void showCreditsScreen()
{
  Sprites::drawOverwrite(0, 0, creditsScreen, 0);
  if (player.frame)
  {
    --player.frame;
  }
  else
  {
    mainState = msTitleScreen;
  }
}

void waitForButtonsReleased()
{
  if ((arduboy.currentButtonState & (A_BUTTON | B_BUTTON)) == 0) ++mainState;
}


void drawLevel()
{
   for (int8_t y = -1; y <= level.height; y++)
     for (int8_t x = -1; x <= level.width; x++)
       Sprites::drawOverwrite(
         x * tilesetWidth + cam.x,
         y * tilesetWidth + cam.y,
         tileset, getTile(x,y));
}

void drawPlayer()
{
  //draw banana or peel behind player
  if ((player.state >= psGotBanana) && (player.state <= psAteBanana) && banana.step)
  {
    Sprites::drawPlusMask(
      player.x * tilesetWidth + cam.x,
      player.y * tilesetHeight - pgm_read_byte(&animationData[banana.step]) + cam.y,
      bananaSprite, banana.frame);
      player.frame = backFrame;
      //flash RGB LED yellow
     #if defined (ENABLE_LED_FX)
      if (banana.step & 2) arduboy.setRGBled(50, 25, 0);
      else arduboy.setRGBled(0, 0, 0);
     #endif
  }
  // also draw recycle bin when throwing away banana peel
  if (player.state == psAteBanana)
  {
    Sprites::drawPlusMask(
      finish.x * tilesetWidth + cam.x,
      finish.y * tilesetHeight + cam.y,
      finishSprite, 0);
  }
  //draw player
  if (player.state != psDead)
  {
  Sprites::drawPlusMask(
    player.x * tilesetWidth + player.xstep + cam.x,
    player.y * tilesetHeight + player.ystep + cam.y,
    playerSprite, player.frame);
  }
  else
  {
    //draw dying player
    if (banana.step == animationMidStep) player.frame = deathFrame;
    Sprites::drawPlusMask(
      player.x * tilesetWidth + cam.x,
      player.y * tilesetHeight - pgm_read_byte(&animationData[banana.step]) - 1 + cam.y,
      playerSprite, player.frame);
     #if defined (ENABLE_LED_FX)
      if (banana.step & 2) arduboy.setRGBled(50, 0, 0);
      else arduboy.setRGBled(0, 0, 0);
     #endif
      
  }
}

void drawBlock()
{
  if (block.xstep || block.ystep)
  {
    Sprites::drawSelfMasked(
      block.x * tilesetWidth + block.xstep + cam.x,
      block.y * tilesetWidth + block.ystep + cam.y,
      tileset, block.tile);
  }
}

void drawArrows(void)
{
  //bool noArrowsDrawn = true;
  if (arduboy.everyXFrames(frameRate / 3)) cam.shift ^= 2;
  if (cam.y < cam.ymax)
  {
    Sprites::drawPlusMask((WIDTH - arrowSpritesWidth) / 2 ,
      cam.shift,
      arrowSprites, arrowUpFrame);
    //noArrowsDrawn = false;
  }
  if (cam.x < cam.xmax)
  {
    Sprites::drawPlusMask(cam.shift, (HEIGHT - arrowSpritesHeight) / 2 ,
      arrowSprites, arrowLeftFrame);
    //noArrowsDrawn = false;
  }
  if (cam.x > cam.xmin)
  {
    Sprites::drawPlusMask(WIDTH - arrowSpritesWidth - cam.shift,
      (HEIGHT - arrowSpritesHeight) / 2,
      arrowSprites, arrowRightFrame);
    //noArrowsDrawn = false;
  }
  if (cam.y > cam.ymin)
  {
    Sprites::drawPlusMask((WIDTH - arrowSpritesWidth) / 2,
      HEIGHT - arrowSpritesHeight - cam.shift,
      arrowSprites, arrowDownFrame);
    //noArrowsDrawn = false;
  }
  //if (noArrowsDrawn && cam.shift)
  //{
  //  Sprites::drawPlusMask((WIDTH - arrowSpritesWidth) / 2, 
  //    (HEIGHT - arrowSpritesHeight) / 2,
  //    arrowSprites, noArrowsFrame);
  //}
  // Note Since vertical scroll is always possible (single pixel) The
  // X sprite is never shows. Code and gfx removed to not waste space.
}

////////////////////////////////////////////////////////////////////////////////

void loadLevel(uint8_t levelNr)
{
  const uint8_t * levelData = (uint8_t *)pgm_read_ptr(&levels[levelNr]);
  // get map size
  uint8_t data = pgm_read_byte(levelData++);
  level.width  = (data >> 4) + 1;
  level.height = (data & 0x0f) + 1;
  // clear map
  for (uint8_t y =  0; y < level.height; y++)
     for (uint8_t x = 0; x < level.width; x++)
         level.map[x + y * level.width] = Tiles::airTile;
  // get player
  data = pgm_read_byte(levelData++);
  player.x = data >> 4;
  player.y = data & 0x0f;
  player.xstep = 0;
  player.ystep = 0;
  player.frame = idleFrame;
  // get banana
  data = pgm_read_byte(levelData++);
  banana.x = data >> 4;
  banana.y = data & 0x0f;
  level.map[banana.x + banana.y * level.width] = Tiles::bananaTile;
  banana.step = 0;
  banana.frame = 0;
  player.state = psNoBanana;
  //get finish
  data = pgm_read_byte(levelData++);
  finish.x = data >> 4;
  finish.y = data & 0x0f;
  level.map[finish.x + finish.y * level.width] = Tiles::finishTile;
  //load tiles
  for (uint8_t t = Tiles::ladderTile; t <= Tiles::downTile; t++)
     for (uint8_t i =  pgm_read_byte(levelData++); i != 0; i--)
       {
         data = pgm_read_byte(levelData++);
         uint8_t x = data >> (uint8_t)4;
         uint8_t y = data & 0x0f;
         level.map[x + y * level.width] = (Tiles)t;
       }
  // clear movable block
  block.tile = Tiles::airTile;
  block.xstep = 0;
  block.ystep = 0;
  // reset camera
  cam.mode = 0;
  cam.timer = 0;
  cam.xmin = WIDTH - level.width * tilesetWidth - 3;
  cam.xmax = 4;
  cam.ymin = HEIGHT - 3 - level.height * tilesetHeight;
  cam.ymax = 3;
  
  if (level.width <= 10) // center levels with a width <= 10 tiles
  {
    cam.x = (WIDTH / 2) - (level.width * tilesetWidth / 2); 
  }
  // adjust camera so player is visible
  else if (player.x < 7) 
  {
    cam.x = cam.xmax;
  }
  else if (player.x > level.width - 7)
  {
    cam.x = cam.xmin;
  }
  else
  {
    cam.x = 4 - (player.x - 6) * tilesetWidth;
  }
  if (level.height <= 5) // center levels with a height <= 5 tiles
  {
    cam.y = (HEIGHT / 2) - (level.height * tilesetHeight / 2); 
  }
  else if (player.y < 3)
  {
    cam.y = cam.ymax;
  }
  else if (player.y > level.height - 3)
  {
    cam.y = cam.ymin;
  }
  else
  {
    cam.y = 3 - (player.y - 2) * tilesetHeight;
  }
  cam.xOrg = cam.x;
  cam.yOrg = cam.y;
}

Tiles getTile(int8_t x, int8_t y)
{
  Tiles tile;
  if(x < 0)
  {
    tile = Tiles::borderLeftTile;
    if (y < 0) tile = Tiles::borderTopLeftTile;
    else if (y >= level.height) tile = Tiles::borderBottomLeftTile;
  }
  else if (x >= level.width)
  {
    tile = Tiles::borderRightTile;
    if (y < 0) tile = Tiles::borderTopRightTile;
    else if (y >= level.height) tile = Tiles::borderBottomRightTile;
  }
  else if (y < 0)
  {
    tile = Tiles::borderTopTile;
  }
  else if (y >= level.height)
  {
    tile = Tiles::borderBottomTile;
  }
  else
  {
    tile = level.map[x + y * level.width];
  }
  return tile;
}

void setTile(int8_t x, int8_t y, Tiles tile)
{
  if ((x >= 0) && (x < level.width) && (y >= 0) && (y < level.height))
  {
    level.map[x + y * level.width] = tile;
  }
}

////////////////////////////////////////////////////////////////////////////////

void game()
{
  //return camara to play position
  if ((cam.mode == 0) && ((cam.x != cam.xOrg) || (cam.y != cam.yOrg)))
  {
    if      (cam.x < cam.xOrg) ++cam.x;
    else if (cam.x > cam.xOrg) --cam.x;
    if      (cam.y < cam.yOrg) ++cam.y;
    else if (cam.y > cam.yOrg) --cam.y;
    return;
  }
  
  // update moving block
  if (block.xstep)
  {
    if      (block.xstep < 0) block.xstep++;
    else if (block.xstep > 0) block.xstep--;
  }
  if (block.ystep)
  {
    if      (block.ystep < 0) block.ystep++;
    else if (block.ystep > 0) block.ystep--;
  }
  // restore moving block back to tile
  if ((block.tile != Tiles::airTile) && ((block.xstep | block.ystep) == 0))
  {
    level.map[block.x + block.y * level.width] = block.tile;
    block.tile = Tiles::airTile;
  }
  // update banana animation
  if (banana.step)
  {
    if (--banana.step == 0)
    {
      if ((player.state == psAteBanana) || (player.state == psDead))
      {
        banana.step = frameRate;
        player.state++;
      }
      else if (player.state == psNextLevel)
      {
        if (++level.nr == maxLevels)
        {
          player.frame = frameRate * 2;
          mainState = msShowEndScreen;
          return;
        }
        else
        {
          player.frame = frameRate;
          mainState = msShowLevelScreen;
          return;
        }
      }
      else if (player.state == psRestart)
      {
          player.frame = frameRate;
          mainState = msShowLevelScreen;
          return;
      }
    }
    if (player.state == psNextLevel) player.frame = idleFrame;
  }
  // update moving player
  if (player.xstep)
  {
    if (player.xstep >= 0) // moving left
    {
      --player.xstep;
      if ((cam.x + player.x * tilesetWidth + player.xstep < 40) && (cam.x < cam.xmax))
      //if ((level.width > 10) && (player.x < level.width - 7) && (cam.x < cam.xmax))
      {
        ++cam.x;
        cam.xOrg = cam.x;
      }
    }
    else // moving right
    {
      ++player.xstep;
      if ((cam.x + player.x * tilesetWidth + player.xstep > 76) && (cam.x > cam.xmin))
      //if ((level.width > 10) && (player.x > 6) && (cam.x > cam.xmin)) 
      {
        --cam.x;
        cam.xOrg = cam.x;
      }
    }
  }
  else if (player.ystep)
  {
    if (player.ystep >= 0) // moving up
    {
      --player.ystep;
      if ((level.height > 5) && (player.y < level.height - 3) && (cam.y < cam.ymax))
      {
        ++cam.y;
        cam.yOrg = cam.y;
      }
    }
    else // moving down
    {
      ++player.ystep;
      if ((level.height > 5) && (player.y > 2) && (cam.y > cam.ymin))
      {
        --cam.y;
        cam.yOrg = cam.y;
      }
    }
  }
  // update walking / climbing animation
  uint8_t speed = frameRate / 2;
  if (player.frame > 1) speed = 3;
  if (arduboy.everyXFrames(speed) && (player.state < psNextLevel)) player.frame ^= 1;
    
  if ((player.xstep | player.ystep | banana.step) == 0)
  {
    if (getTile(player.x, player.y) == Tiles::ladderTile) player.frame = climbFrame;
    else player.frame &= 1;
    //check banana pickup
    if ((player.state == psNoBanana) && (player.x == banana.x) && (player.y == banana.y))
    {
      player.state = psGotBanana;
      banana.step = animationLastStep;
      level.map[banana.x + banana.y * level.width] = Tiles::airTile;
    }
    else if ((player.state == psGotBanana) && (player.x == finish.x) && (player.y == finish.y))
    {
      banana.step = animationLastStep;
      banana.frame = 1;
      player.state = psAteBanana;
    }
    //check dying
    else if (getTile(player.x, player.y) == Tiles::deathTile)
    {
      if (player.state < psDead)
      {
       player.state = psDead;
       banana.step = animationLastStep;
      }
    }
    //check if falling
    else if (((getTile(player.x, player.y + 1) < Tiles::ladderTile) ||
             (getTile(player.x, player.y + 1) == Tiles::deathTile)) &&
             (getTile(player.x, player.y) != Tiles::ladderTile))
    {
      player.y++;
      player.ystep = -tilesetHeight;
    }
    // camera movement
    else if (cam.mode && (cam.timer == 0))
    {
       if (arduboy.currentButtonState & LEFT_BUTTON)
       {
         if (cam.x < cam.xmax) ++cam.x;
       }
       else if (arduboy.currentButtonState & RIGHT_BUTTON)
       {
         if (cam.x > cam.xmin) --cam.x;
       }
       else if (arduboy.currentButtonState & UP_BUTTON)
       {
         if (cam.y < cam.ymax) ++cam.y;
       }
       else if (arduboy.currentButtonState & DOWN_BUTTON)
       {
         if (cam.y > cam.ymin) --cam.y;
       }
    }
    // check block actions
    else if (arduboy.currentButtonState & B_BUTTON)
    {
      Tiles tile = getTile(player.x, player.y + 1); 
      // left
      if (tile == Tiles::leftTile)
      {
        if ((getTile(player.x - 1, player.y + 1) == Tiles::airTile))
        {
          block.tile = Tiles::leftTile;
          level.map[player.x + (player.y + 1) * level.width] = Tiles::airTile;
          block.x = player.x - 1;
          block.y = player.y + 1;
          block.xstep = tilesetWidth;
          if ((getTile(player.x, player.y) != Tiles::ladderTile) &&
              (getTile(player.x - 1, player.y) <= Tiles::ladderTile))
          {
            player.x--;
            player.xstep = tilesetWidth;
          }
        }
      }
      // right
      else if (tile == Tiles::rightTile)
      {
        if ((getTile(player.x + 1, player.y + 1) == Tiles::airTile))
        {
          block.tile = Tiles::rightTile;
          level.map[player.x + (player.y + 1) * level.width] = Tiles::airTile;
          block.x = player.x + 1;
          block.y = player.y + 1;
          block.xstep = -tilesetWidth;
          if ((getTile(player.x, player.y) != Tiles::ladderTile) &&
              (getTile(player.x + 1, player.y) <= Tiles::ladderTile))
          {
            player.x++;
            player.xstep = -tilesetWidth;
          }
        }
      }
      // up
      else if (tile == Tiles::upTile)
      {
        if ((getTile(player.x, player.y) == Tiles::airTile) &&
            (getTile(player.x, player.y - 1) <= Tiles::ladderTile))
        {
          block.tile = Tiles::upTile;
          level.map[player.x + (player.y + 1) * level.width] = Tiles::airTile;
          block.x = player.x;
          block.y = player.y;
          block.ystep = tilesetHeight;
          player.y--;
          player.ystep = tilesetWidth;
        }
      }
      // down
      else if (tile == Tiles::downTile)
      {
        if (getTile(player.x, player.y + 2) == Tiles::airTile)
        {
          block.tile = Tiles::downTile;
          level.map[player.x + (player.y + 1) * level.width] = Tiles::airTile;
          block.x = player.x;
          block.y = player.y + 2;
          block.ystep = -tilesetHeight;
          if (getTile(player.x, player.y) != Tiles::ladderTile)
          {
            player.y++;
            player.ystep = -tilesetWidth;
          }
        }
      }
    }
    // check player moves
    else if ((arduboy.currentButtonState & LEFT_BUTTON) && 
             (getTile(player.x - 1, player.y) <= Tiles::ladderTile))
    {
      player.frame = walkLeftFrame;
      if ((getTile(player.x, player.y) == Tiles::ladderTile) &&
          (getTile(player.x - 1, player.y) == Tiles::ladderTile))
      {
        player.frame = climbFrame;
      }
      player.x--;
      player.xstep = tilesetWidth;
    }
    else if ((arduboy.currentButtonState & RIGHT_BUTTON) && 
             (getTile(player.x + 1, player.y) <= Tiles::ladderTile))
    {
      player.frame = walkRightFrame;
      if ((getTile(player.x, player.y) == Tiles::ladderTile) &&
          (getTile(player.x + 1, player.y) == Tiles::ladderTile))
      {
        player.frame = climbFrame;
      }
      player.x++;
      player.xstep = -tilesetWidth;
    }
    else if ((arduboy.currentButtonState & UP_BUTTON) &&
             (getTile(player.x, player.y) == Tiles::ladderTile) && 
             (getTile(player.x, player.y - 1) <= Tiles::ladderTile))
    {
      player.y--;
      player.ystep = tilesetHeight;
      player.frame = climbFrame;
    }
    else if ((arduboy.currentButtonState & DOWN_BUTTON) && 
             (getTile(player.x, player.y + 1) <= Tiles::ladderTile))
    {
      player.y++;
      player.ystep = -tilesetHeight;
      player.frame = climbFrame;
    }
    // camera on /off control
    if (arduboy.currentButtonState & A_BUTTON)
    {
      if (arduboy.justPressed(A_BUTTON)) cam.mode ^= 1; //press A to toggle camera mode
      if(++cam.timer >= frameRate * 2/3 ) // hold down A for 2/3 second to restart level
      {
      player.frame = frameRate;
      mainState = msShowLevelScreen;
      return;
      }
    }
    else if (cam.mode)
    {
      cam.timer = 0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////


void setup()
{
  if (frameRate != 60) arduboy.setFrameRate(frameRate);
  arduboy.begin();
  //arduboy.audio.begin();
}

void loop()
{
  if (!(arduboy.nextFrame())) return;

  arduboy.pollButtons();

  switch(mainState)
  {
    case msTitleScreen:
    
      showTitleScreen();
      break;

    case msTitleScreenWait:
    case msShowLevelScreenWait:
    
      waitForButtonsReleased();
      break;

    case msShowLevelScreen:
    
      showLevelScreen();
      break;

    case msPlayLevel:
    
      game();
      drawLevel();
      drawPlayer();
      drawBlock();
      if (cam.mode && (cam.timer == 0))
      {
        drawArrows();
      }
      break;

    case msShowEndScreen:
    
      showEndScreen();
      break;

    case msShowCreditsScreen:
    
      showCreditsScreen();
      break;
      
    case msHelpScreen:
    
      showHelpScreen();
      break;
  }
  arduboy.display(CLEAR_BUFFER);
}

        
        
     
