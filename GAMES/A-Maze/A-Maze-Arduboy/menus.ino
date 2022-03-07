const unsigned char mazeBgTile [] PROGMEM = {
  // 'mazeTile, 32x32px
0x01, 0x20, 0x01, 0x20, 0x55, 0x00, 0x00, 0x00, 0x00, 0x55, 0x00, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x55, 0x00, 0x01, 0x00, 0x01, 0xa0, 
0x00, 0x00, 0x00, 0x00, 0x55, 0x08, 0x00, 0x08, 0x00, 0x55, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0xa8, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0a, 
0x20, 0x02, 0x20, 0x02, 0xa1, 0x00, 0x20, 0x00, 0x20, 0x01, 0xa2, 0x00, 0x02, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x40, 0x21, 0x00, 0x21, 0x00, 0x21, 0x00, 0x21, 0x00, 0x20, 0x00, 0x20, 0x00, 
0x00, 0x04, 0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x04, 0x80, 0x04, 0x82, 0x00, 0x00, 0x00, 0x00, 0x05, 0x80, 0x04, 0x80, 0x04, 0x80, 0x04, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

int mainMenuBgX=0;
int mainMenuBgY=0;

void mainMenu(){

  mainMenuAnimation();
  menuItemPointerAnimation();
  
  if(selectedOption==1){
    arduboy.setTextSize(2);
    arduboy.setTextColor(BLACK);
    arduboy.setCursor(11+menuPointerPos,6);
    arduboy.print(">");
    arduboy.setCursor(15+menuPointerPos,6);
    arduboy.print(">");
    arduboy.setTextColor(WHITE);
    arduboy.setCursor(13+menuPointerPos,6);
    arduboy.print(">");
    arduboy.setCursor(16+27-menuPointerPos,5);
  }else{
    arduboy.setCursor(16+27,10);
  }
  arduboy.print("walk");
  arduboy.setTextSize(1);
  
  if(selectedOption==2){
    arduboy.setTextSize(2);
    arduboy.setTextColor(BLACK);
    arduboy.setCursor(11+menuPointerPos,19);
    arduboy.print(">");
    arduboy.setCursor(15+menuPointerPos,19);
    arduboy.print(">");
    arduboy.setTextColor(WHITE);
    arduboy.setCursor(13+menuPointerPos,19);
    arduboy.print(">");
    arduboy.setCursor(16+27-menuPointerPos,18);
  }else{
    arduboy.setCursor(16+27,22);
  }
  arduboy.print("collect");
  arduboy.setTextSize(1);
  
  if(selectedOption==3){
    arduboy.setTextSize(2);
    arduboy.setTextColor(BLACK);
    arduboy.setCursor(11+menuPointerPos,30);
    arduboy.print(">");
    arduboy.setCursor(15+menuPointerPos,30);
    arduboy.print(">");
    arduboy.setTextColor(WHITE);
    arduboy.setCursor(13+menuPointerPos,30);
    arduboy.print(">");
    arduboy.setCursor(16+27-menuPointerPos,29); 
  }else{
    arduboy.setCursor(16+27,34); 
  }
  arduboy.print("escape");
  arduboy.setTextSize(1);
  
  if(selectedOption==4){
    arduboy.setTextSize(2);
    arduboy.setTextColor(BLACK);
    arduboy.setCursor(11+menuPointerPos,45);
    arduboy.print(">");
    arduboy.setCursor(15+menuPointerPos,45);
    arduboy.print(">");
    arduboy.setTextColor(WHITE);
    arduboy.setCursor(13+menuPointerPos,45);
    arduboy.print(">");
    arduboy.setCursor(16+27-menuPointerPos,44);
  }else{
    arduboy.setCursor(16+27,46);
  }
  arduboy.print("night");
  arduboy.setTextSize(1);

  /* ------- BUTTON 3 - UP ------- */
  if(arduboy.pressed(UP_BUTTON)){
    selectedOption--;
    if(selectedOption==0){
      selectedOption=4;
    }
    tunes.tone(2200,5);
    delay(100);
  }
  /* ------- BUTTON 4 - DOWN ------- */
  if(arduboy.pressed(DOWN_BUTTON)){
    selectedOption++;
    if(selectedOption==5){
      selectedOption=1;
    }
    tunes.tone(2200,5);
    delay(100);
  }

  /*if(arduboy.pressed(A_BUTTON)){//A
    //arduboy.fillCircle(20,30,5,WHITE);
    tunes.tone(2400,5);
    delay(100);
  }*/
  // toggle sound
    if (arduboy.justPressed(A_BUTTON)) {
    if (arduboy.audio.enabled()) {
      arduboy.audio.off();
      sound_enabled=!sound_enabled;
      tunes.tone(2400,5);
      delay(100);
      }
      else{
        arduboy.audio.on();
        sound_enabled=!sound_enabled;
        tunes.tone(2400,5);
        delay(100);
        }
 /* if(arduboy.justPressed(A_BUTTON)){//B
    //arduboy.fillCircle(50,30,5,WHITE);
    sound_enabled=!sound_enabled;
    tunes.tone(2400,5);
    delay(100);
  }*/
    }
  if (arduboy.pressed(B_BUTTON)){//menu/start
    arduboy.initRandomSeed();
    gameMode=selectedOption;
    generateMaze();
    if(selectedOption==2){
      generate_collectables();
    }
    delay(250);
    //button7State = digitalRead(button7Pin);
  }  
}

void gameMenu(){

  menuItemPointerAnimation();
  
  arduboy.fillRect(24,8,80,45,WHITE);
  arduboy.fillRect(25,9,78,43,BLACK);
  arduboy.setTextColor(BLACK);
  arduboy.setTextBackground(WHITE);
  arduboy.fillRect(26,10,76,11,WHITE);
  arduboy.setCursor(48,12);
  arduboy.print("PAUSED");
  arduboy.setTextColor(WHITE);
  arduboy.setTextBackground(BLACK);
  arduboy.setCursor(48,24);
  arduboy.print("resume");
  if(selectedOption==1){
    arduboy.setCursor(33+menuPointerPos,24);
    arduboy.print(">");
  }
  arduboy.setCursor(48,34);
  arduboy.print("quit");
  if(selectedOption==2){
    arduboy.setCursor(33+menuPointerPos,34);
    arduboy.print(">");
  }

  /* ------- BUTTON 3 - UP ------- */
  if(arduboy.pressed(UP_BUTTON)){
    selectedOption--;
    if(selectedOption==0){
      selectedOption=2;
    }
    tunes.tone(2200,5);
    delay(100);
  }
  /* ------- BUTTON 4 - DOWN ------- */
  if(arduboy.pressed(DOWN_BUTTON)){
    selectedOption++;
    if(selectedOption==3){
      selectedOption=1;
    }
    tunes.tone(2200,5);
    delay(100);
  }

 /* if(arduboy.pressed(A_BUTTON)){//A
    //arduboy.fillCircle(20,30,5,WHITE);
    tunes.tone(2400,5);
    delay(100);
  }
  // toggle sound
  if(arduboy.pressed(A_BUTTON)){//B
    //arduboy.fillCircle(50,30,5,WHITE);
    sound_enabled=!sound_enabled;
    tunes.tone(2400,5);
    delay(100);
  }*/
    // toggle sound
    if (arduboy.justPressed(A_BUTTON)) {
    if (arduboy.audio.enabled()) {
      arduboy.audio.off();
      sound_enabled=!sound_enabled;
      tunes.tone(2400,5);
      delay(100);
      }
      else{
        arduboy.audio.on();
        sound_enabled=!sound_enabled;
        tunes.tone(2400,5);
        delay(100);
        }
        }
  if (arduboy.pressed(B_BUTTON)){
    if(selectedOption==1){
       gamePaused=false;
       delay(100);
//       button7State = digitalRead(button7Pin);
    }
    if(selectedOption==2){
      arduboy.clear();
      Serial.println("arduboy.clear");
      //asm volatile ("  jmp 0");
    }
    tunes.tone(1600,5);
  }
}

void menuItemPointerAnimation(){
  if(menuPointerPosDir){
    menuPointerPos++;
    if(menuPointerPos>=6){
      menuPointerPos=6;
      menuPointerPosDir=!menuPointerPosDir;
    }
  }else{
    menuPointerPos--;
    if(menuPointerPos<=-1){
      menuPointerPos=0;
      menuPointerPosDir=!menuPointerPosDir;
    }
  }
}

void mainMenuAnimation(){

  //4x2 32x32 each
  drawMenuBgPattern(mainMenuBgX-32,mainMenuBgY-32);
  drawMenuBgPattern(mainMenuBgX,mainMenuBgY-32);
  
  drawMenuBgPattern(mainMenuBgX-32,mainMenuBgY);
  drawMenuBgPattern(mainMenuBgX,mainMenuBgY);
  
  drawMenuBgPattern(mainMenuBgX-32,mainMenuBgY+32);
  drawMenuBgPattern(mainMenuBgX,mainMenuBgY+32);


  
  arduboy.fillRect(32,0,64,64,BLACK);
  arduboy.drawLine(31,0,31,63,WHITE);
  for(int shdw=0;shdw<63;shdw+=2){
    arduboy.drawPixel(33,shdw,WHITE);
  }
  
    
  mainMenuBgX++;
  mainMenuBgY++;
  if(mainMenuBgX==32){
    mainMenuBgX=0;
    mainMenuBgY=0;
  }
  
  
}

void drawMenuBgPattern(int x,int y){
  //arduboy.drawPixel(x+5,y+5,WHITE);
  //arduboy.drawPixel(x+6,y+5,WHITE);
  //arduboy.drawPixel(x+6,y+6,WHITE);
  arduboy.drawBitmap(x, y, mazeBgTile , 32, 32, WHITE);
}
