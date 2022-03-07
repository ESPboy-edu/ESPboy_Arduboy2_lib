void walker(){
    /* ------- BUTTON PRESS ACTIONS ------- */
  bool wall;
  
  if(!gamePaused){
    
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

  if(posx==MAZEHEIGHT+1){
    posx=0;
    posy=2;
    displayLevelSplash();
    level++;
    illuminatedRow=0;
    blinkPlayer=1;
    wallPhase=1;
  }else{
    drawMaze();
    if(gamePaused){
      gameMenu();
    }
    
  }

}
