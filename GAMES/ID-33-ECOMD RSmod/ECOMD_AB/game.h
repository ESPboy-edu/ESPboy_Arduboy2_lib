#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "globals.h"

void stateGameInitLevel()
{
  scorePlayer = 0;
  //set starting  weapon type
  CURRENT_WEAPON = DEFAULT_GUN;
  weapon.weaponSet(0); //set to default gun specs
  crate.randomizeCrate();
  player.xSpeed = 1;
  player.ySpeed = 3;
  player.x = 24;
  player.y = 38;

  enemy[0].enemyType = 0; //set to big enemy
  enemy[1].enemyType = 1;
  enemy[2].enemyType = 1; //set to midium size enemy;

  for (byte i = 0; i < 8; i++)
  {
    enemy[i].reset();
    enemyWalkingFrame = 0;
  }

  topHole.width = 16;
  topHole.height = 4;
  topHole.x = 56;
  topHole.y = 0;

  bottomHole.width = 16;
  bottomHole.height = 4;
  bottomHole.x = 56;
  bottomHole.y = 63;

  //platform left
  wall[0].x = 0;
  wall[0].y =  31;
  wall[0].width = 55;
  wall[0].height = 2;

  //platform right
  wall[1].x = 72;
  wall[1].y = 31;
  wall[1].width = 55;
  wall[1].height = 2;

  //platform mid top
  wall[2].x = 40;
  wall[2].y = 15;
  wall[2].width = 47;
  wall[2].height = 2;

  //platform mid bottom
  wall[3].x = 40;
  wall[3].y = 47;
  wall[3].width = 47;
  wall[3].height = 2;

  walkingFrame = 0;
  waitingFrame = 0;
  waitingTimer = 0;

  gameState = STATE_GAME_PLAYING;
}

void stateGamePlaying()
{
  checkInputs();
  gravityEffect();
  checkCrateCollisions();
  checkEnemyCollisions();
  updateEnemies();
  updateWeapon();

  drawBackDrop();
  drawPlayer();
  drawBullets();
  drawFirePit();
  drawEnemies();
  drawCrates();

  drawSurrounding();
  drawScore(98, 6, SCORE_SMALL_FONT);
  sprites.drawSelfMasked(11, 6, guns, CURRENT_WEAPON);
  if (gameState == STATE_GAME_OVER) delay(500);
};


void stateGamePause()
{
  arduboy.drawCompressed(0, 0, pause_bitmap, WHITE);
  if (arduboy.justPressed(UP_BUTTON)) gameState = STATE_GAME_PLAYING;
};


void stateGameOver()
{
  arduboy.drawCompressed(0, 0, gameover_bitmap, WHITE);
  drawScore(14, 32, SCORE_BIG_FONT);
  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) gameState = STATE_MENU_MAIN;
};

#endif
