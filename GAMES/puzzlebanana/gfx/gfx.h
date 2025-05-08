#include "titleScreen.h"
#include "helpScreen.h"
#include "creditsScreen.h"
#include "tileset.h"
#include "playerSprite.h"
#include "bananaSprite.h"
#include "finishSprite.h"
#include "arrowSprites.h"
#include "levelText.h"
#include "numberSprites.h"
#include "theEndText.h"

enum Tiles : uint8_t
{
  airTile = 0,
  bananaTile,
  finishTile,
  ladderTile,
  solidTile,
  deathTile,
  leftTile,
  rightTile,
  upTile,
  downTile,
  borderTopLeftTile,
  borderTopTile,
  borderTopRightTile,
  borderLeftTile,
  borderRightTile,
  borderBottomLeftTile,
  borderBottomTile,
  borderBottomRightTile,
};

constexpr uint8_t idleFrame      = 0;
constexpr uint8_t walkLeftFrame  = 2;
constexpr uint8_t walkRightFrame = 4;
constexpr uint8_t climbFrame     = 6;
constexpr uint8_t backFrame      = 8;
constexpr uint8_t deathFrame     = 9;

const uint8_t animationData[] PROGMEM =
{
  0, 0, 3, 5, 6, 8, 10, 11, 11, 12, 12, 13, 13, 13, 12, 12, 11, 11, 10, 8, 6, 5, 3, 0
};
constexpr uint8_t animationLastStep = sizeof(animationData) - 1;
constexpr uint8_t animationMidStep = sizeof(animationData) / 2;

constexpr uint8_t arrowLeftFrame  = 0;
constexpr uint8_t arrowRightFrame = 1;
constexpr uint8_t arrowUpFrame    = 2;
constexpr uint8_t arrowDownFrame  = 3;
//constexpr uint8_t noArrowsFrame   = 4; // Note frame removed see code.
