// ================ Start Screen Variables ================

int startScreenCounter = 0;
int startScreenState = 0;
int menuIndicator = 0;

bool settingsChanged = false;

// ================ Start Screen Functions ================

void ReturnToStartScreen(){
  // Automatically save game data if gameScore was high enough to make the score board
  if(gameScore > highScores[2]) SaveGameData();

  // Set gameState to 0
  SetGameState(1);
}

void PrintMainMenuElement(int menuItem, int yOffset){
  // Function to print the elements of the main menu

  // menuItem determines what to be drawn
  switch(menuItem){
    case 0:
    // Print play menu option
    arduboy.setCursor(69, 34 + yOffset);
    arduboy.print("PLAY");

    // If play is selected, underscore it with a line to show that it is selected
    if(menuItem == menuIndicator) arduboy.drawLine(69, 42 + yOffset, 91, 42 + yOffset);
    break;

    case 1:
    // Print highscore option
    arduboy.setCursor(69, 42 + yOffset);
    arduboy.print("HIGHSCORE");

    // If highscore is selected, underscore it with a line to show that it is selected
    if(menuItem == menuIndicator) arduboy.drawLine(69, 50 + yOffset, 121, 50 + yOffset);
    break;

    case 2:
    // Print options option
    arduboy.setCursor(69, 50 + yOffset);
    arduboy.print("OPTIONS");

    // If options is selected, underscore it with a line to show that it is selected
    if(menuItem == menuIndicator) arduboy.drawLine(69, 58 + yOffset, 109, 58 + yOffset);
    break;
  }
}

void PrintOptionsMenuElement(int menuItem, int yOffset){
  // Function to print the elements of the options menu

  // menuItem determines what to be drawn
  switch(menuItem){
    case 0:
    // Print sound option
    arduboy.setCursor(69, 34 + yOffset);
    arduboy.print("SOUND");

    // Print the current state of audio
    if(arduboy.audio.enabled()){
      // Audio is enabled
      arduboy.setCursor(112, 34 + yOffset);
      arduboy.print("ON");
    }
    else{
      // Audio isn't enabled
      arduboy.setCursor(106, 34 + yOffset);
      arduboy.print("OFF");
    }

    // If sound is selected, underscore it with a line to show that it is selected
    if(menuItem == menuIndicator) arduboy.drawLine(69, 42 + yOffset, 122, 42 + yOffset);
    break;

    case 1:
    // Print controls option
    arduboy.setCursor(69, 42 + yOffset);
    arduboy.print("CONTROLS");

    // If reset is selected, underscore it with a line to show that it is selected
    if(menuItem == menuIndicator) arduboy.drawLine(69, 50 + yOffset, 115, 50 + yOffset);
    break;

    case 2:
    // Print reset save option
    arduboy.setCursor(69, 50 + yOffset);
    arduboy.print("RESET");

    // If reset is selected, underscore it with a line to show that it is selected
    if(menuItem == menuIndicator) arduboy.drawLine(69, 58 + yOffset, 97, 58 + yOffset);
    break;
  }
}

void PrintHighscores(){
  // Loop through the 3 recorded highscores
  for(int i = 0; i < 3; i++){

    // Print highscore line
    arduboy.setCursor(69, 34 + i * 8);
    arduboy.print(i + 1);
    arduboy.print(".");
    arduboy.setCursor(88, 34 + i * 8);
    arduboy.print("......");
    
    // Get highscore to print
    int hs = GetHighScore(i);
    int hsOffset = GetHighScoreOffset(i);

    // Set the cursor with correct offset and print the highscore
    arduboy.setCursor(118 - hsOffset * 6, 34 + i * 8);
    arduboy.print(hs);
    //arduboy.setCursor(112, 34 + i * 8);
    //arduboy.print(hsOffset);
  }
}

void PrintControls(){
  // Function for drawing controls layout to the screen

  // Fill screen with black
  arduboy.fillScreen(BLACK);

  // Draw directional buttons
  Sprites::drawOverwrite(40, 32, sControlsDir0, 0);
  Sprites::drawOverwrite(48, 24, sControlsDir1, 0);
  Sprites::drawOverwrite(56, 32, sControlsDir2, 0);

  // Draw round buttons
  Sprites::drawOverwrite(72, 34, sControlsButton, 0);
  Sprites::drawOverwrite(80, 30, sControlsButton, 0);

  // Draw arrow heads
  Sprites::drawOverwrite(37, 29, sArrowDR, 0);
  Sprites::drawOverwrite(88, 27, sArrowDL, 0);
  Sprites::drawOverwrite(80, 42, sArrowUL, 0);

  // Draw lines
  arduboy.drawLine(36, 29, 8, 29);
  arduboy.drawLine(91, 27, 127, 27);
  arduboy.drawLine(83, 44, 127, 44);

  // Draw labels
  arduboy.setCursor(40, 8);
  arduboy.print("CONTROLS");

  arduboy.setCursor(8, 21);
  arduboy.print("MOVE");

  arduboy.setCursor(93, 19);
  arduboy.print("RELOAD");

  arduboy.setCursor(99, 36);
  arduboy.print("SHOOT");

}

void PrintResetConfirmation(){
  // Function to print game data confirmation

  // Print text
  arduboy.setCursor(69, 34);
  arduboy.print("RESET");
  arduboy.setCursor(69, 42);
  arduboy.print("GAME");
  arduboy.setCursor(100, 42);
  arduboy.print("DATA");
  arduboy.setCursor(69, 50);
  arduboy.print("YES");
  arduboy.setCursor(112, 50);
  arduboy.print("NO");

  // Underscore the selected option with a line
  if(menuIndicator == 0) arduboy.drawLine(69, 58, 85, 58);
  else arduboy.drawLine(112, 58, 122, 58);
}

void UpdateStartScreen(){
  // Function to update the start screen

  // Draw art to the left and title to the top right
  Sprites::drawOverwrite(0, 0, sStartScreenArt, 0);
  Sprites::drawOverwrite(68, 0, sStartScreenTitle, 0);

  // Start screen audio loop plays if nothing is playing
  PlaySecondaryAudio(aMainMusicLoop);

  // startScreenState determines what happens
  switch(startScreenState){
    case 0:
    // Press start to continue

    // Tick up time counter
    startScreenCounter++;

    // Flashing start icon
    if(startScreenCounter > 30){
      arduboy.setCursor(76, 40);
      arduboy.print("PRESS A");
    }

    // Reset counter if it grows to big
    if(startScreenCounter > 60) startScreenCounter = 0;

    // Go to main menu if A is pressed
    if(arduboy.justPressed(A_BUTTON)) startScreenState = 1;
    break;

    case 1:
    // Main menu

    // Loop through and print elements of the main menu
    for(int i = 0; i < 3; i++){
      if(i > menuIndicator)PrintMainMenuElement(i, 4);
      else  PrintMainMenuElement(i, 0);
    }

    // What happens when the player presses A is determined by what is currently selected
    if(arduboy.justPressed(A_BUTTON)){
      switch(menuIndicator){
        case 0:
        // Play selected
        startScreenState = 10;
        startScreenCounter = 0;
        break;

        case 1:
        // High score selected
        startScreenState = 2;
        break;

        case 2:
        // Options selected
        menuIndicator = 0;
        startScreenState = 3;
        settingsChanged = false;
        break;
      }
    }
    else if(arduboy.justPressed(UP_BUTTON) && menuIndicator > 0) menuIndicator--;
    else if(arduboy.justPressed(DOWN_BUTTON) && menuIndicator < 2) menuIndicator++;
    break;

    case 2:
    // Highscores

    // Print highscores
    PrintHighscores();
    
    // Go back to main menu if the B button is pressed
    if(arduboy.justPressed(B_BUTTON)) startScreenState = 1;
    break;

    case 3:
    // Options

    // Loop through the options menu elements
    for(int i = 0; i < 3; i++){
      if(i > menuIndicator) PrintOptionsMenuElement(i, 4);
      else  PrintOptionsMenuElement(i, 0);
    }

    // What happens when the A button is pressed is determined by the selected element
    if(arduboy.justPressed(A_BUTTON)){
      switch(menuIndicator){
        case 0:
        // Toggle audio
        arduboy.audio.toggle();

        // Settings have been changed
        settingsChanged = true;

        // Start playing main music loop
        if(arduboy.audio.enabled()) PlayPrimaryAudio(aMainMusicLoop);
        break;

        case 1:
        // Show controls
        startScreenState = 5;
        break;

        case 2:
        // Reset game data
        menuIndicator = 1;
        startScreenState = 4;
        break;
      }
    } 
    else if(arduboy.justPressed(B_BUTTON)){
      // Save settings if they have been changed
      if(settingsChanged) SaveGameData();

      // Go back to main menu if the B button is pressed
      menuIndicator = 2;
      startScreenState = 1;
    }
    else if(arduboy.justPressed(UP_BUTTON) && menuIndicator > 0) menuIndicator--;
    else if(arduboy.justPressed(DOWN_BUTTON) && menuIndicator < 2) menuIndicator++;
    break;

    case 4:
    // Reset game data confirmation
    PrintResetConfirmation();

    // Player input
    if(arduboy.justPressed(A_BUTTON)){
      if(menuIndicator == 0){
        // The player has confirmed that they want to reset the game's data
        for(int i = 0; i < 3; i++) SetHighScore(i, 0);
        ResetGameData();
      }

      // Whether data was reset or not, return to options menu
      menuIndicator = 2;
      startScreenState = 3;
    }
    else if(arduboy.justPressed(RIGHT_BUTTON) && menuIndicator == 0) menuIndicator = 1;
    else if(arduboy.justPressed(LEFT_BUTTON) && menuIndicator == 1) menuIndicator = 0;
    else if(arduboy.justPressed(B_BUTTON)){
      // Return to options menu if B button is pressed
      menuIndicator = 2;
      startScreenState = 3;
    }
    break;

    case 5:
    // Show game controls
    PrintControls();

    // Player input
    if(arduboy.justPressed(B_BUTTON)){
      // Return to options menu if B button is pressed
      menuIndicator = 1;
      startScreenState = 3;
    }
    break;

    case 10:
    // Transition to black and move to gameplay
    for(int i = 0; i < 3; i++){
      if(i > menuIndicator)PrintMainMenuElement(i, 4);
      else  PrintMainMenuElement(i, 0);
    }

    // Tick up counter by 2
    startScreenCounter += 2;

    // Draw a growing black rectangle over the menu screen
    arduboy.fillRect(64 - startScreenCounter, 32 - startScreenCounter / 2, startScreenCounter * 2, startScreenCounter, BLACK);

    // Go to gameplay when enough time has passed
    if(startScreenCounter > 70){
      startScreenCounter = 0;
      startScreenState = 0;
      
      StopAudio();
      PrepareGameplay();
      SetGameState(2);
    }
    break;
  }
}