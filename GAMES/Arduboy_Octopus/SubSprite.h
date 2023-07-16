byte frameSub = 0;

void Sub(){
if(ingame == true){
  if(subGold == false){
  if(subMove == 0){
  }else if(subMove == 1){
    Sprites::drawPlusMask(8, 12, Sub_mask, 0);
  }else if(subMove == 2){
    Sprites::drawPlusMask(8, 35, Sub_mask, 1);
  }else if(subMove == 3){
    Sprites::drawPlusMask(35, 45, Sub_mask, 2);
  }else if(subMove == 4){
    Sprites::drawPlusMask(60, 44, Sub_mask, 3);
  }else if(subMove == 5){
    Sprites::drawPlusMask(80, 44, Sub_mask, 5);
  }
  }else{
  if(subMove == 0){
  }else if(subMove == 1){
    Sprites::drawPlusMask(8, 12, Sub_mask, 6);
  }else if(subMove == 2){
    Sprites::drawPlusMask(7, 36, Sub_mask, 7);
  }else if(subMove == 3){
    Sprites::drawPlusMask(35, 46, Sub_mask, 8);
  }else if(subMove == 4){
    Sprites::drawPlusMask(60, 44, Sub_mask, 9);
  }else if(subMove == 5){
    Sprites::drawPlusMask(78, 44, Sub_mask, 11);
  }
  }
}
    if(ingame == false && life == 3){
      animBarca = 0;
      if (animBarca == 0) Sprites::drawPlusMask(10, 0, Barca_mask, 2);
      if (animBarca == 1) Sprites::drawPlusMask(10, 0, Barca_mask, 3);
      if (arduboy.everyXFrames(6)){
          animBarca++;
      }
      if (frameSub == 0) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 1) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 2) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 3) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 4) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 5) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 6) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 7) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 8){
        barcaState = 3;
        ingame = true;
        subMove = 0;
        inGold = false;
        subGold = false;
        --life;
        frameSub = 0;
      }
      if (arduboy.everyXFrames(6)){
      frameSub++;
      sound.tones(SubSound);
    }
    }else if(ingame == false && life == 2){
      animBarca = 0;
      if (animBarca == 0) Sprites::drawPlusMask(10, 0, Barca_mask, 7);
      if (animBarca == 1) Sprites::drawPlusMask(10, 0, Barca_mask, 8);
      if (arduboy.everyXFrames(6)){
          animBarca++;
      }
      if (frameSub == 0) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 1) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 2) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 3) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 4) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 5) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 6) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 7) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 8){
        barcaState = 6;
        ingame = true;
        subMove = 0;
        inGold = false;
        subGold = false;
        --life;
        frameSub = 0;
      }
      if (arduboy.everyXFrames(6)){
      frameSub++;
      sound.tones(SubSound);
    }
    }else if(ingame == false && life == 1){
      moveSubButton = false;
      Sprites::drawPlusMask(10, 0, Barca_mask, 9);
      if (frameSub == 0) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 1) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 2) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 3) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 4) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 5) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 6) Sprites::drawPlusMask(48, 19, Subdie_mask, 0);
      if (frameSub == 7) Sprites::drawPlusMask(48, 19, Subdie_mask, 1);
      if (frameSub == 8){
        barcaState = 10;
        ingame = false;
        subMove = 0;
        inGold = false;
        subGold = false;
        --life;
        frameSub = 0;
      }
      if (arduboy.everyXFrames(6)){
      frameSub++;
      sound.tones(SubSound);
    }
    }
}

void animSubGold(){
  if(subGold == false){
      if (animFrame == 0) Sprites::drawPlusMask(80, 44, Sub_mask, 4);
      if (animFrame == 1) Sprites::drawPlusMask(80, 44, Sub_mask, 5);
      if (animFrame == 2) Sprites::drawPlusMask(78, 44, Sub_mask, 12);
      if (animFrame == 3){
         ++score;
         ++getPoint;
         sound.tones(GoldSound);
         subGold = true;
         subMove = 5;
         animFrame = 0;
         catchGold = false;
      }
  }else if(subGold == true){
      if (animFrame == 0) Sprites::drawPlusMask(79, 44, Sub_mask, 10);
      if (animFrame == 1) Sprites::drawPlusMask(78, 44, Sub_mask, 11);
      if (animFrame == 2) Sprites::drawPlusMask(78, 44, Sub_mask, 12);
      if (animFrame == 3){
         ++score;
         ++getPoint;
         sound.tones(GoldSound);
         subGold = true;
         subMove = 5;
         animFrame = 0;
         catchGold = false;
      }
  }
      if (arduboy.everyXFrames(6)){
      animFrame++;
      }
}
