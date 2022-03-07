unsigned int timeLeft=630;



void escaper(){
    /* ------- BUTTON PRESS ACTIONS ------- */
    
  bool wall;
  
  if(!gamePaused and !escapeGameOver){
    
    //GAME CONTROL
    
    /* ------- BUTTON 1 - LEFT ------- */
    if(button1State==ACTIVATED){
      //generateMaze();
      if(posx-1>=0){
        wall=readPixel(posx-1,posy);
        if(!wall){
          --posx;
          aj_tone(sound,2200,5);
        }else{
          aj_tone(sound,1000,50);  
        }
      }else{
        aj_tone(sound,1000,50);
      }
      delay(30);
    }
    /* ------- BUTTON 2 - RIGHT ------- */
    if(button2State==ACTIVATED){
      if(posx+1<=MAZEHEIGHT+1){
        wall=readPixel(posx+1,posy);
        if(!wall){
          ++posx;
          aj_tone(sound,2200,5);
        }else{
          aj_tone(sound,1000,50);  
        }
      }else{
        aj_tone(sound,1000,50);
      }
      delay(30);
    }
    /* ------- BUTTON 3 - UP ------- */
    if(button3State==ACTIVATED){
      if(posy-1>=2){
        wall=readPixel(posx,posy-1);
        if(!wall){
          --posy;
          aj_tone(sound,2200,5);
        }else{
          aj_tone(sound,1000,50);  
        }
      }else{
        aj_tone(sound,1000,50);
      }
      delay(30);
    }
    /* ------- BUTTON 4 - DOWN ------- */
    if(button4State==ACTIVATED){
      if(posy+1<=MAZEWIDTH){
        wall=readPixel(posx,posy+1);
        if(!wall){
          ++posy;
          aj_tone(sound,2200,5);
        }else{
          aj_tone(sound,1000,50);  
        }
      }else{
        aj_tone(sound,1000,50);
      }
      delay(30);
    }
  
    if(button5State==ACTIVATED){//A
      //display.fillCircle(20,30,5,WHITE);
      aj_tone(sound,2400,5);
      delay(60);
    }
    if(button6State==ACTIVATED){//B
      //display.fillCircle(50,30,5,WHITE);
      aj_tone(sound,2400,5);
      delay(60);
    }
    if(button7State==ACTIVATED){//MENU
      //display.fillCircle(80,30,5,WHITE);
      gamePaused=true;
      aj_tone(sound,2400,5);
      delay(200);
      button7State = digitalRead(button7Pin);
    }
  
  }

  


  
  display.setCursor(0,0);

  if(!gamePaused and wallPhase>1){
    if(timeLeft>0){
      timeLeft--;
      uint8_t timeLine=round(timeLeft/15);
      display.drawLine(125,52,125,52-timeLine,WHITE);
      display.drawLine(126,52,126,52-timeLine,WHITE);
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
  
  display.fillRect(24,8,80,45,WHITE);
  display.fillRect(25,9,78,43,BLACK);
  display.setTextColor(BLACK);
  display.fillRect(26,10,76,11,WHITE);
  display.setCursor(37,12);
  display.print("GAME OVER");
  display.setTextColor(WHITE);
  display.setCursor(48,24);
  display.print("retry");
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
  if(button7State==ACTIVATED){
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
      button7State = digitalRead(button7Pin);
    }
    if(selectedOption==2){
       asm volatile ("  jmp 0");
    }
    aj_tone(sound,2200,5);
  }
}
