void TitleGame(){
  Sprites::drawOverwrite(0, 0, BackgroundGame, 0);
  Sprites::drawPlusMask(12, 17, OctopusText_mask, 0);
  Sprites::drawPlusMask(25, 38, PressAText_mask, 0);
  animFrame = 0;
  catchGold = false;
  inGold = false;
  score = 0;
  getPoint = 0;
  life = 3;
  ingame = true;
  subGold = false;
  subMove = 0;
  barcaState = 0;
  moveSubButton = true;
  dieFrame = 0;
  endGame = false;
  /*
  arduboy.setCursor(2, 2);
  arduboy.setTextSize(1);
  arduboy.setTextBackground(WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.print("HighScore: ");
  arduboy.setCursor(62, 2);
  arduboy.setTextSize(1);
  arduboy.setTextBackground(WHITE);
  arduboy.setTextColor(BLACK);
  arduboy.print(HighScore);
  */
  
  if (arduboy.justPressed(A_BUTTON)){
  state = 2;
  }
}
