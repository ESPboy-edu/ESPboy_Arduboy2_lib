// by "Aska-Games" Johan Bengtsson
// 12/01-2026
// Zombie Slayer

// ================ Game Inclusions ================

#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "ZombieSlayerSprites.h"
#include "ZombieSlayerSounds.h"

// ================ Game Instances ================

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

// ================ Game Variables ================

int gameState = 0;
int gameScore = 0;
int gameScoreOffset = 0;

int highScores[3] = {0, 0, 0};
int highScoreOffsets[3] = {0, 0, 0};

// ================ Game Functions ================

void SetGameState(int newState){
  // Set new game state
  gameState = newState;
}

void ResetGameScore(){
  // Reset game score
  gameScore = 0;
}

void SetHighScore(int index, int score){
  // Set highscore at index to a specific value, and determine a graphical offset for it
  highScores[index] = score;
  highScoreOffsets[index] = DetermineScoreOffset(score);
}

int DetermineScoreOffset(int score){
  // Offsets are used when graphically aligning scores to the right
  int scoreOffset = 0;

  if(score > 9999) scoreOffset = 4;
  else if(score > 999) scoreOffset = 3;
  else if(score > 99) scoreOffset = 2;
  else if(score > 9) scoreOffset = 1;

  return scoreOffset;
}

void CompareToHighScores(int newScore){

  // Loop through high score array once
  for(int i = 0; i < 3; i++){
    if(newScore > highScores[i]){
      int oldScore = highScores[i];

      // Compare highscores again with existing number to shunt array down
      CompareToHighScores(oldScore);

      // Record new score
      highScores[i] = newScore;

      // Determine offset for new score
      highScoreOffsets[i] = DetermineScoreOffset(newScore);

      // A high score has been recorded, return out of the function
      return;
    }
  }
}

void CalculateGameScore(bool playerWon, int wave, int zSlain, int pTime, int dTaken){
  // Calculate game score
  gameScore = wave * 400 + zSlain * 10 - pTime - dTaken * 20;

  // Apply a score multiplier if the player managed to extract
  if(playerWon) gameScore *= 1.2;

  // Set minimum and maximum values for game score
  if(gameScore < 0) gameScore = 0;
  else if(gameScore > 999999) gameScore = 999999;

  // Determine game scoree offset
  gameScoreOffset = DetermineScoreOffset(gameScore);

  // Compare new score to existing highscores
  CompareToHighScores(gameScore);
}

int GetGameScore(){
  // Get current game score
  return gameScore;
}

int GetHighScore(int i){
  // Get highscore at the given index
  return highScores[i];
}

int GetGameScoreOffset(){
  // Get current game score graphical offset
  return gameScoreOffset;
}

int GetHighScoreOffset(int i){
  // Get graphical offset for highscore at given index
  return highScoreOffsets[i];
}

void Victory(){
  // Transition to victory state
  SetGameState(3);
  SetVictoryState(9);
  StopAudio();
}

void GameOver(){
  // Transition to game over state
  SetGameState(4);
  PlayPrimaryAudio(aGameOverMusic);
}

// ================ setup and loop ================

void setup() {
  // Setup code here
  arduboy.begin();

  // Initialize a random seed and set frame rate to 30
  arduboy.initRandomSeed();
  arduboy.setFrameRate(30);

  // Audio setup
  TurnOnAudio();

  // Check and load game data
  CheckGameData();
  LoadGameData();

  // Clear the screen
  arduboy.clear();
}

void loop() {
  // Gameplay code here

  // Only run game code on correct frame
  if(!arduboy.nextFrame()) return;

  // Clear the screen and poll arduboy buttons
  arduboy.clear();
  arduboy.pollButtons();

  // Switch to handle different game states
  switch(gameState){
    case 0:
    // Intro
    UpdateIntroScreen();
    break;

    case 1:
    // Start screen
    UpdateStartScreen();
    break;

    case 2:
    // Gameplay
    UpdateGameplay();
    break;

    case 3:
    // Player win
    UpdateVictory();
    break;

    case 4:
    // Game over
    UpdateGameOver();
    break;
  }

  // Display drawn image to arduboy's screen
  arduboy.display();
}