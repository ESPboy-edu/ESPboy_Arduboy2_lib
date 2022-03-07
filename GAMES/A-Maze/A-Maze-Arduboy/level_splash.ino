void displayLevelSplash(){
  int levely=8,levelnoy=8,completey=8, i, j;
  
  arduboy.setTextColor(WHITE);
  for(i=0;i<3;i++){
    for(j=0;j<50;j+=2){
      arduboy.clear();//.
      if(levely<20){
        levely+=2;
        tunes.tone(levely*110,5);
      }
      if(levely==20 and levelnoy<30){
        levelnoy+=2;
        tunes.tone(levelnoy*110,5);
      }
      if(levely==20 and levelnoy==30 and completey<40){
        completey+=2;
        tunes.tone(completey*110,5);
      }
      arduboy.setCursor(15,levely);
      arduboy.print("LEVEL");
      arduboy.setCursor(45,levelnoy);
      arduboy.print(level);
      arduboy.setCursor(55,completey);
      arduboy.print("COMPLETE!");

      displayBattery(WHITE);
      
      arduboy.display();
    }
  }
  
  delay(1000);
  
  levely=8;
  levelnoy=8;
  for(i=0;i<2;i++){
    for(j=0;j<50;j+=2){
      arduboy.clear();//.
      if(levely<20){
        levely+=2;
        tunes.tone(levely*110,5);
      }
      if(levely==20 and levelnoy<30){
        levelnoy+=2;
        tunes.tone(levelnoy*110,5);
      }
      arduboy.setCursor(15,levely);
      arduboy.print("LEVEL");
      arduboy.setCursor(45,levelnoy);
      arduboy.print(level+1);
      
      displayBattery(WHITE);
      
      arduboy.display();
    }
  }
  
  delay(1500);
  
  generateMaze();
}
