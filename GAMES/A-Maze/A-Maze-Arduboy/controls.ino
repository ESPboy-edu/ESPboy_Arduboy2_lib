//GAME CONTROL
 void controls(){
  bool wall;
     
    /* ------- BUTTON 1 - LEFT ------- */
    if(arduboy.pressed(LEFT_BUTTON)){
      //generateMaze();
      if(posx-1>=0){
        wall=readPixel(posx-1,posy);
        if(!wall){
          --posx;
          tunes.tone(2200,5);
        }else{
          tunes.tone(1000,50);  
        }
      }else{
        tunes.tone(1000,50);
      }
      delay(130);
    }
    /* ------- BUTTON 2 - RIGHT ------- */
    if(arduboy.pressed(RIGHT_BUTTON)){
      if(posx+1<=MAZEHEIGHT+1){
        wall=readPixel(posx+1,posy);
        if(!wall){
          ++posx;
          tunes.tone(2200,5);
        }else{
          tunes.tone(1000,50);  
        }
      }else{
        tunes.tone(1000,50);
      }
      delay(130);
    }
    /* ------- BUTTON 3 - UP ------- */
    if(arduboy.pressed(UP_BUTTON)){
      if(posy-1>=2){
        wall=readPixel(posx,posy-1);
        if(!wall){
          --posy;
          tunes.tone(2200,5);
        }else{
          tunes.tone(1000,50);  
        }
      }else{
        tunes.tone(1000,50);
      }
      delay(130);
    }
    /* ------- BUTTON 4 - DOWN ------- */
    if(arduboy.pressed(DOWN_BUTTON)){
      if(posy+1<=MAZEWIDTH){
        wall=readPixel(posx,posy+1);
        if(!wall){
          ++posy;
          tunes.tone(2200,5);
        }else{
          tunes.tone(1000,50);  
        }
      }else{
        tunes.tone(1000,50);
      }
      delay(130);
    }
  /*
    if(arduboy.pressed(A_BUTTON)){//A
      //arduboy.fillCircle(20,30,5,WHITE);
      tunes.tone(2400,5);
      delay(60);
    }
    if(arduboy.pressed(A_BUTTON | B_BUTTON)){//B
      //arduboy.fillCircle(50,30,5,WHITE);
      tunes.tone(2400,5);
      delay(60);
    }*/
    if (arduboy.pressed(B_BUTTON)){//MENU
      //arduboy.fillCircle(80,30,5,WHITE);
      gamePaused=true;
      tunes.tone(2400,5);
      delay(500);
      //button7State = digitalRead(button7Pin);
    }
  
  }
  
