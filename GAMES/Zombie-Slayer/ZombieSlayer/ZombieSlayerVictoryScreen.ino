int victoryCounter = 0;
int victoryState = 0;

void SetVictoryState(int newState){
  // Set new victory state and reset counter
  victoryCounter = 0;
  victoryState = newState;
}

void DrawOrbit(int stationPosX){
  // Draw orbit image
  Sprites::drawOverwrite(0, 32, sEarthLeft, 0);
  Sprites::drawOverwrite(64, 32, sEarthRight, 0);

  Sprites::drawOverwrite(stationPosX, 8, sSpaceStation, 0);
}

void DrawSpaceLaser(int x1, int y1, int x2, int y2){
  // Draw laser line in white with a black contour
  arduboy.drawLine(x1, y1, x2, y2, WHITE);

  arduboy.drawLine(x1 - 1, y1, x2 - 1, y2, BLACK);
  arduboy.drawLine(x1 + 1, y1, x2 + 1, y2, BLACK);
}

void UpdateVictory(){
  // Function to update victory state

  // Tick up counter
  victoryCounter++;

  switch(victoryState){
    case 9:
    // Start by showing earth from space
    DrawOrbit(79);

    if(victoryCounter == 60) SetVictoryState(10);
    break;

    case 10:
    // Move space station slightly to the right
    DrawOrbit(80);

    // When enough time has passed, seet of an explosion effect on the earth's surface and proceed to next step
    if(victoryCounter == 30) {
      SetOffExplosion(67, 58);
      SetVictoryState(11);
    }
    break;

    case 11:
    // Begin drawing a laser beam from the earth's surface going towards the space station
    DrawOrbit(80);
    DrawSpaceLaser(67, 58, 67 + victoryCounter, 58 - victoryCounter * 2);
    DrawExplosions();
    
    // When the beam reaches the station, proceed to next step
    if(victoryCounter == 16) SetVictoryState(12);
    break;

    case 12:
    // Beam now shortens towards the station
    DrawOrbit(80);
    DrawSpaceLaser(67 + victoryCounter, 58 - victoryCounter * 2, 84, 24);

    // When the beam's end reaches the station, set off an explosion effect and proceed to next step
    if(victoryCounter == 16){
      SetOffExplosion(84, 23);
      SetVictoryState(13);
    }
    break;

    case 13:
    // Les the explosion effect play out
    DrawOrbit(80);
    DrawExplosions();

    if(victoryCounter == 30) SetVictoryState(14);
    break;

    case 14:
    // Move the station slightly to the right
    DrawOrbit(81);

    // When enough time has passed, play victory music and proceed to the last step
    if(victoryCounter == 60) {
      PlayPrimaryAudio(aVictoryMusic);
      SetVictoryState(15);
    }
    break;

    case 15:
    // Draw "VICTORY"
    arduboy.setCursor(44, 0);
    arduboy.print("VICTORY");

    // Display game score
    arduboy.setCursor(26, 8);
    arduboy.print("SCORE ......");

    arduboy.setCursor(92 - GetGameScoreOffset() * 6, 8);
    arduboy.print(GetGameScore());

    // Draw the trooper emerging from a teleporter on the station
    Sprites::drawOverwrite(0, 16, sTeleporter01, 0);
    Sprites::drawOverwrite(32, 16, sTeleporter02, 0);
    Sprites::drawOverwrite(96, 16, sTeleporter03, 0);

    // When enough time has passed, the player can return to the start screen
    if(arduboy.justPressed(A_BUTTON) && victoryCounter > 30){
      victoryCounter = 0;
      ReturnToStartScreen();
    } else if (victoryCounter > 120 ) victoryCounter = 60;
    break;
  }
}