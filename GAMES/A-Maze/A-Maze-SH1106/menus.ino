const unsigned char mazeBgTile [] PROGMEM = {
  // 'mazeTile, 32x32px
  0xa8, 0x42, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x08, 0x42, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x08, 0x42, 0x00, 0x20, 0x50, 0x00, 0x0a, 0x81, 0x08, 0x40, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 
  0x08, 0x40, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x08, 0x40, 0x00, 0x20, 0x05, 0x15, 0x55, 0x41, 
  0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 
  0x08, 0x40, 0x0a, 0xa0, 0x50, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xaa, 0xa2, 0x0a, 0xaa, 0x00, 0x00, 0x10, 0x00, 0x08, 0x22, 0x00, 0x00, 
  0x00, 0x00, 0x10, 0x00, 0x08, 0x22, 0x00, 0x00, 0x50, 0x14, 0x15, 0x40, 0x00, 0x00, 0x00, 0x20, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x0a, 0xa0
};

int mainMenuBgX=0;
int mainMenuBgY=0;

void mainMenu(){

  mainMenuAnimation();
  menuItemPointerAnimation();
  
  if(selectedOption==1){
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(11+menuPointerPos,6);
    display.print(">");
    display.setCursor(15+menuPointerPos,6);
    display.print(">");
    display.setTextColor(WHITE);
    display.setCursor(13+menuPointerPos,6);
    display.print(">");
    display.setCursor(16+27-menuPointerPos,5);
  }else{
    display.setCursor(16+27,10);
  }
  display.print("walk");
  display.setTextSize(1);
  
  if(selectedOption==2){
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(11+menuPointerPos,19);
    display.print(">");
    display.setCursor(15+menuPointerPos,19);
    display.print(">");
    display.setTextColor(WHITE);
    display.setCursor(13+menuPointerPos,19);
    display.print(">");
    display.setCursor(16+27-menuPointerPos,18);
  }else{
    display.setCursor(16+27,22);
  }
  display.print("collect");
  display.setTextSize(1);
  
  if(selectedOption==3){
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(11+menuPointerPos,30);
    display.print(">");
    display.setCursor(15+menuPointerPos,30);
    display.print(">");
    display.setTextColor(WHITE);
    display.setCursor(13+menuPointerPos,30);
    display.print(">");
    display.setCursor(16+27-menuPointerPos,29); 
  }else{
    display.setCursor(16+27,34); 
  }
  display.print("escape");
  display.setTextSize(1);
  
  if(selectedOption==4){
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(11+menuPointerPos,45);
    display.print(">");
    display.setCursor(15+menuPointerPos,45);
    display.print(">");
    display.setTextColor(WHITE);
    display.setCursor(13+menuPointerPos,45);
    display.print(">");
    display.setCursor(16+27-menuPointerPos,44);
  }else{
    display.setCursor(16+27,46);
  }
  display.print("night");
  display.setTextSize(1);

  /* ------- BUTTON 3 - UP ------- */
  if(button3State==ACTIVATED){
    selectedOption--;
    if(selectedOption==0){
      selectedOption=4;
    }
    aj_tone(sound,2200,5);
    delay(100);
  }
  /* ------- BUTTON 4 - DOWN ------- */
  if(button4State==ACTIVATED){
    selectedOption++;
    if(selectedOption==5){
      selectedOption=1;
    }
    aj_tone(sound,2200,5);
    delay(100);
  }

  if(button5State==ACTIVATED){//A
    //display.fillCircle(20,30,5,WHITE);
    aj_tone(sound,2400,5);
    delay(100);
  }
  // toggle sound
  if(button6State==ACTIVATED){//B
    //display.fillCircle(50,30,5,WHITE);
    sound_enabled=!sound_enabled;
    aj_tone(sound,2400,5);
    delay(100);
  }
  
  if(button7State==ACTIVATED){//menu/start
    gameMode=selectedOption;
    generateMaze();
    if(selectedOption==2){
      generate_collectables();
    }
    delay(250);
    button7State = digitalRead(button7Pin);
  }  
}

void gameMenu(){

  menuItemPointerAnimation();
  
  display.fillRect(24,8,80,45,WHITE);
  display.fillRect(25,9,78,43,BLACK);
  display.setTextColor(BLACK);
  display.fillRect(26,10,76,11,WHITE);
  display.setCursor(48,12);
  display.print("PAUSED");
  display.setTextColor(WHITE);
  display.setCursor(48,24);
  display.print("resume");
  if(selectedOption==1){
    display.setCursor(33+menuPointerPos,24);
    display.print(">");
  }
  display.setCursor(48,34);
  display.print("quit");
  if(selectedOption==2){
    display.setCursor(33+menuPointerPos,34);
    display.print(">");
  }

  /* ------- BUTTON 3 - UP ------- */
  if(button3State==ACTIVATED){
    selectedOption--;
    if(selectedOption==0){
      selectedOption=2;
    }
    aj_tone(sound,2200,5);
    delay(100);
  }
  /* ------- BUTTON 4 - DOWN ------- */
  if(button4State==ACTIVATED){
    selectedOption++;
    if(selectedOption==3){
      selectedOption=1;
    }
    aj_tone(sound,2200,5);
    delay(100);
  }

  if(button5State==ACTIVATED){//A
    //display.fillCircle(20,30,5,WHITE);
    aj_tone(sound,2400,5);
    delay(100);
  }
  // toggle sound
  if(button6State==ACTIVATED){//B
    //display.fillCircle(50,30,5,WHITE);
    sound_enabled=!sound_enabled;
    aj_tone(sound,2400,5);
    delay(100);
  }
  
  if(button7State==ACTIVATED){
    if(selectedOption==1){
       gamePaused=false;
       delay(100);
       button7State = digitalRead(button7Pin);
    }
    if(selectedOption==2){
       asm volatile ("  jmp 0");
    }
    aj_tone(sound,1600,5);
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


  
  display.fillRect(32,0,64,64,BLACK);
  display.drawLine(31,0,31,63,WHITE);
  for(int shdw=0;shdw<63;shdw+=2){
    display.drawPixel(33,shdw,WHITE);
  }
  
    
  mainMenuBgX++;
  mainMenuBgY++;
  if(mainMenuBgX==32){
    mainMenuBgX=0;
    mainMenuBgY=0;
  }
  
  
}

void drawMenuBgPattern(int x,int y){
  //display.drawPixel(x+5,y+5,WHITE);
  //display.drawPixel(x+6,y+5,WHITE);
  //display.drawPixel(x+6,y+6,WHITE);
  display.drawBitmap(x, y, mazeBgTile , 32, 32, WHITE);
}
