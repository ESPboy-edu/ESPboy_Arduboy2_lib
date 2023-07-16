byte animBarca = 0;

void Barca(){
  if(barcaState == 0 && ingame == true){
    Sprites::drawPlusMask(10, 0, Barca_mask, 0);
  }else if(barcaState == 1 && ingame == true){
    Sprites::drawPlusMask(10, 0, Barca_mask, 1);
  }else if(barcaState == 2 && ingame == true){
      if (animBarca == 0) Sprites::drawPlusMask(10, 0, Barca_mask, 0);
      if (animBarca == 1) Sprites::drawPlusMask(10, 0, Barca_mask, 4);
      if (animBarca == 2) Sprites::drawPlusMask(10, 0, Barca_mask, 0);
      if (animBarca == 3) Sprites::drawPlusMask(10, 0, Barca_mask, 4);
      if (animBarca == 4){
        score = score+getPoint;
        subGold = false;
        animBarca = 0;
        barcaState = 0;
      }
      if (arduboy.everyXFrames(6)){
      animBarca++;
      sound.tones(SubSound);
    }
  }else if(barcaState == 3 && ingame == true){
     Sprites::drawPlusMask(10, 0, Barca_mask, 3);
  }else if(barcaState == 4 && ingame == true){
     Sprites::drawPlusMask(10, 0, Barca_mask, 7);
  }else if(barcaState == 5 && ingame == true){
      if (animBarca == 0) Sprites::drawPlusMask(10, 0, Barca_mask, 3);
      if (animBarca == 1) Sprites::drawPlusMask(10, 0, Barca_mask, 5);
      if (animBarca == 2) Sprites::drawPlusMask(10, 0, Barca_mask, 3);
      if (animBarca == 3) Sprites::drawPlusMask(10, 0, Barca_mask, 5);
      if (animBarca == 4){
        score = score+getPoint;
        subGold = false;
        animBarca = 0;
        barcaState = 3;
      }
      if (arduboy.everyXFrames(6)){
      animBarca++;
      sound.tones(SubSound);
    }
  }else if(barcaState == 6 && ingame == true){
     Sprites::drawPlusMask(10, 0, Barca_mask, 8);
  }else if(barcaState == 7 && ingame == true){
     Sprites::drawPlusMask(10, 0, Barca_mask, 9);
  }else if(barcaState == 8 && ingame == true){
      if (animBarca == 0) Sprites::drawPlusMask(10, 0, Barca_mask, 8);
      if (animBarca == 1) Sprites::drawPlusMask(10, 0, Barca_mask, 6);
      if (animBarca == 2) Sprites::drawPlusMask(10, 0, Barca_mask, 8);
      if (animBarca == 3) Sprites::drawPlusMask(10, 0, Barca_mask, 6);
      if (animBarca == 4){
        score = score+getPoint;
        subGold = false;
        animBarca = 0;
        barcaState = 9;
      }
      if (arduboy.everyXFrames(6)){
      animBarca++;
      sound.tones(SubSound);
    }
  }else if(barcaState == 9 && ingame == true){
     Sprites::drawPlusMask(10, 0, Barca_mask, 8);
  }else if(barcaState == 10){
     Sprites::drawPlusMask(10, 0, Barca_mask, 9);
  }
}
