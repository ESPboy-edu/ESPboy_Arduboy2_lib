byte frame = 0;
int x = 140;
bool fwd = true;
int animN = 0;
int ciclo = 0;
bool pressButton = false;
unsigned long NextScreenTimer = 0;

void NextScreen(long secondi, int stato){
  constexpr unsigned long millisecondsPerSecond = 1000;
  long sec = (secondi * millisecondsPerSecond);
  if(NextScreenTimer == 0)
  {
    NextScreenTimer = millis();
  }
  else
  {
    unsigned long now = millis();
    if((now - NextScreenTimer) >= sec)
    {
      NextScreenTimer = 0;
      state = stato;
    }
  }
}

void fadeOUTLtoR(int delayT, int Time, int delayEnd, int stato, int color) {
  delay(delayT);

  for (int x = 0; x < 129; x++) {
    for (int y = 0; y < 65; y++) {
      arduboy.drawPixel(x, y, color);
    }
    arduboy.display();
    delay(Time);
  }
  delay(delayEnd);
  state = stato;
  //arduboy.clear();
}

void TitleScreen(){
  if (arduboy.justPressed(A_BUTTON)){
  pressButton = true;
  fadeOUTLtoR(0, 5, 0, 1, 1);
  }
  if (arduboy.justPressed(B_BUTTON)){
  pressButton = true;
  fadeOUTLtoR(0, 5, 0, 1, 1);
  }

  if(pressButton == false){
  Sprites::drawOverwrite(0, 0, BackgroundMenu, 0);
  if (animN == 0){
    Sprites::drawPlusMask(x, 20, Mario_mask, frame);
    if (arduboy.everyXFrames(3)){
    frame++;
    sound.tones(SubSound);
    if (frame > 1) frame = 0;
    if (fwd == true && x > 90) {
        x-=2;
    }else{
        fwd = false;
        frame = 4;
        animN = 1;
  }
  }
  } else if (animN == 1){
  Sprites::drawPlusMask(x, 20, Mario_mask, frame);
  if (arduboy.everyXFrames(3)){
  frame++;
  sound.tones(SubSound);
  ciclo++;
  delay(1000);
  if (frame > 5) frame = 4;
  if (ciclo > 4){
    ciclo = 0;
    frame = 6;
    animN = 2;
  }
  }
  } else if (animN == 2){
  Sprites::drawPlusMask(x, 20, Mario_mask, frame);
  if (arduboy.everyXFrames(3)){
  frame++;
  sound.tones(SubSound);
  ciclo++;
  delay(300);
  if (frame > 7) frame = 6;
  if (ciclo > 3){
    frame = 8;
    animN = 3;
  }
  }
  }else if (animN == 3){
  Sprites::drawPlusMask(x, 20, Mario_mask, frame);
  arduboy.display();
  sound.tones(MarioSound);
  fadeOUTLtoR(2000, 5, 0, 1, 1);
  }
  }
}
