int gameOverCounter = 0;

void UpdateGameOver(){
  // Draw game over image of a skull in dust
  Sprites::drawOverwrite(32, 16, sGameOverScreenArt, 0);

  // Print "GAME OVER"
  arduboy.setCursor(38, 0);
  arduboy.print("GAME OVER");

  // Display this round's score
  arduboy.setCursor(26, 8);
  arduboy.print("SCORE ......");

  arduboy.setCursor(92 - GetGameScoreOffset() * 6, 8);
  arduboy.print(GetGameScore());

  // Count up, after 2 seconds have passed, the player can return to thee start screen
  if(gameOverCounter < 60) gameOverCounter++;
  else if(arduboy.justPressed(A_BUTTON)){
    gameOverCounter = 0;
    ReturnToStartScreen();
  } 
}

