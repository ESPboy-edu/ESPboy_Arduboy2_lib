// ================ Intro Constants ================

const int introTime = 90;

// ================ Intro Variables ================

int introCounter = 0;

// ================ Intro Functions ================

void UpdateIntroScreen(){
  // Function for the intro screen
  introCounter++;

  // White background for the intro screen
  arduboy.fillScreen();

  // Draw Aska logo
  Sprites::drawOverwrite(24, 16, sAskaSkalle, 0);
  Sprites::drawOverwrite(72, 16, sAskaLogga, 0);

  // Continue to the start screen when enough time has passed
  if(introCounter > introTime) SetGameState(1);
}