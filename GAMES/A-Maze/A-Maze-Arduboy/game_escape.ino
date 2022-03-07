unsigned int timeLeft=630;



void escaper(){
    /* ------- BUTTON PRESS ACTIONS ------- */
    
  bool wall;
  
  if(!gamePaused and !escapeGameOver){
    //GAME CONTROL    
    controls();
  
  }
  
  arduboy.setCursor(0,0);

  if(!gamePaused and wallPhase>1){
    if(timeLeft>0){
      timeLeft--;
      uint8_t timeLine=round(timeLeft/15);
      arduboy.drawLine(125,52,125,52-timeLine,WHITE);
      arduboy.drawLine(126,52,126,52-timeLine,WHITE);
    }else{
      escapeGameOver=true;
    }
  }

  if(posx==MAZEHEIGHT+1){
    posx=0;
    posy=2;
    displayLevelSplash();
    level++;
    illuminatedRow=0;
    blinkPlayer=1;
    wallPhase=1;
    timeLeft=630;
  }else{
    drawMaze();
    if(gamePaused){
      gameMenu();
    }
    if(escapeGameOver){
      escapeGameOverMenu();
    }
    
  }

}

void escapeGameOverMenu(){

  menuItemPointerAnimation();
  
  arduboy.fillRect(24,8,80,45,WHITE);
  arduboy.fillRect(25,9,78,43,BLACK);
  arduboy.setTextColor(BLACK);
  arduboy.fillRect(26,10,76,11,WHITE);
  arduboy.setTextBackground(WHITE);
  arduboy.setCursor(48,12);
  arduboy.setCursor(37,12);
  arduboy.print("GAME OVER");
  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
  arduboy.setCursor(48,24);
  arduboy.print("retry");
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
  if (arduboy.pressed(B_BUTTON)){
    if(selectedOption==1){
      gamePaused=false;
      escapeGameOver=false;
      posx=0;
      posy=2;
      level=1;
      illuminatedRow=0;
      blinkPlayer=1;
      wallPhase=1;
      timeLeft=840;
      generateMaze();
      delay(100);
      //button7State = digitalRead(button7Pin);
    }
    if(selectedOption==2){
       Serial.println("selectedOption==2");
       //asm volatile ("  jmp 0");
    }
    tunes.tone(2200,5);
  }
}
