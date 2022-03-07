int collectables[10][2]={
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0}
  };

int numberCollectables=0;

int collectablePhase=0;

void collector(){
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
        }else if(numberCollectables>0){
          aj_tone(sound,1000,50);
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


    if(check_collectable(posx,posy)){
      get_collectable(posx,posy);
      --numberCollectables;
      aj_tone(sound,2800,5);
    }
    if(numberCollectables>0){
      display.drawPixel(124,60,BLACK);
      display.drawPixel(124,59,BLACK);
      display.drawPixel(125,60,BLACK);
      display.drawPixel(125,59,BLACK);
      //display.drawLine(122,58,122,62,WHITE);
      //display.drawLine(127,58,127,62,WHITE);
    }else{
      Maze[31]=0b1100000000000101;
    }

    
  
  }


  
  display.setCursor(0,0);

  if(posx==MAZEHEIGHT+1 and numberCollectables==0){
    posx=0;
    posy=2;
    displayLevelSplash();
    generate_collectables();
    level++;
    illuminatedRow=0;
    blinkPlayer=1;
    wallPhase=1;
  }else{
    drawMaze();
    if(numberCollectables>0 and wallPhase>1){
      display.drawRect(122,57,6,6,WHITE);
      display.drawRect(123,58,4,4,BLACK);
    }
    draw_collectables();
    if(gamePaused){
      gameMenu();
    }
    
  }

}


void generate_collectables(){
  int i=0,x,y,seeded=11;
  randomSeed(analogRead(A0)+readVcc());
  do{
    x=random(1,MAZEHEIGHT);
    y=random(1,MAZEWIDTH);
    if(!readPixel(x,y)){
      collectables[i][0]=x;
      collectables[i][1]=y;
      i++;
      seeded--;
    }
  }while(seeded>0);
  numberCollectables=10;
  Maze[31]=0b1100000000000111;
}

bool check_collectable(int x,int y){
  for(int i=0;i<10;i++){
    if(collectables[i][0]==x and collectables[i][1]==y){
      return true;
    }
  }
  return false;
}

bool get_collectable(int x,int y){
  for(int i=0;i<10;i++){
    if(collectables[i][0]==x and collectables[i][1]==y){
      collectables[i][0]=0;
      collectables[i][1]=0;
      return true;
    }
  }
  return false;
}

void draw_collectables(){
  for(int i=0;i<10;i++){
    int x=collectables[i][0];
    int y=collectables[i][1];
    
    if(x>0 and y>0){
      if(collectablePhase==0){
        display.drawPixel(x*4+1,y*4+1+2,WHITE);
        display.drawPixel(x*4+1,y*4+2+2,WHITE);
        display.drawPixel(x*4+2,y*4+1+2,WHITE);
      }
      if(collectablePhase==1){
        display.drawPixel(x*4+1,y*4+1+2,WHITE);
        display.drawPixel(x*4+1,y*4+2+2,WHITE);
        display.drawPixel(x*4+2,y*4+2+2,WHITE); 
      }
      if(collectablePhase==2){
        display.drawPixel(x*4+1,y*4+1+2,WHITE);
        display.drawPixel(x*4+2,y*4+1+2,WHITE);
        display.drawPixel(x*4+2,y*4+2+2,WHITE); 
      }
      if(collectablePhase==3){
        display.drawPixel(x*4+1,y*4+2+2,WHITE);
        display.drawPixel(x*4+2,y*4+1+2,WHITE);
        display.drawPixel(x*4+2,y*4+2+2,WHITE); 
      }
      if(collectablePhase==4){
        display.drawPixel(x*4+1,y*4+1+2,WHITE);
        display.drawPixel(x*4+1,y*4+2+2,WHITE);
        display.drawPixel(x*4+2,y*4+1+2,WHITE);
        display.drawPixel(x*4+2,y*4+2+2,WHITE); 
      }
    }
  }
  collectablePhase++;
  if(collectablePhase==5){collectablePhase=0;}
}
