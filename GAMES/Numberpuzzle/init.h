#ifndef INIT_H
#define INIT_H

/*-------------------------
    initStage
  ------------------------ */
void initStage(){
  pLives = 3;
  pSpeed = 1;

  pX = pWidth - pCharSize;
  pY = pHeight - pCharSize;

  pSX = pWidth;
  pSY = pHeight / 2;
  pSSpeed = 2;

  initBox(concurrent_box_max);
}

#endif
