void dark(){
    /* ------- BUTTON PRESS ACTIONS ------- */
  bool wall;
  
  if(!gamePaused){
    //GAME CONTROL
    controls();
    }

  arduboy.setCursor(0,0);

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
