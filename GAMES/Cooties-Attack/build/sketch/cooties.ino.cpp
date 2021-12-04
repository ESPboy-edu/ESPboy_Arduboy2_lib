#include <Arduino.h>
#line 1 "c:\\Users\\diego\\OneDrive\\Escritorio\\arduboy\\Github\\Cooties-Attack\\cooties\\cooties.ino"
#include "general.h"


#line 4 "c:\\Users\\diego\\OneDrive\\Escritorio\\arduboy\\Github\\Cooties-Attack\\cooties\\cooties.ino"
void setup();
#line 31 "c:\\Users\\diego\\OneDrive\\Escritorio\\arduboy\\Github\\Cooties-Attack\\cooties\\cooties.ino"
void loop();
#line 4 "c:\\Users\\diego\\OneDrive\\Escritorio\\arduboy\\Github\\Cooties-Attack\\cooties\\cooties.ino"
void setup()
{

    arduboy.begin();
    //beep.begin();
    arduboy.setFrameRate(60);
    arduboy.clear();
    arduboy.display();
    

    initBullets();
    initEnemies();

    hero.x=64;
    hero.y=32;
    hero.width=8;
    hero.height=8;

    hitCount=0;

    level = 1;

    State::Title;
    
    
}

void loop()
{   //  beep.tone(beep.freq(1000),120);
    if (!(arduboy.nextFrame())) return;
    //beep.timer();
    arduboy.clear();
    input();
    

    switch (state) {

      case State::Title:
        //Title screen
          anim();
          sprite.drawOverwrite(3,1,title_sp, title_frame);
          sprite.drawOverwrite(14,46,push_sp,push_frame);

          

          

        break;

      case State::Game:
        //Gameplay screen
          //input();

          drawCooties();
          moveEnemy();
          
          
          moveBullets();
          drawBullets();

          setLife();

          if (waitCount != 0) {
            --waitCount;
          }

          if (hero.iframe !=0){
            --hero.iframe;
          }

          //draw
          //hero
          sprite.drawPlusMask(hero.x,hero.y,hero_sp, hero.frame);

          //life
          arduboy.fillRect(0,0,128,10,BLACK);
          arduboy.fillRect(0,10,128,1,WHITE);
          sprite.drawPlusMask(arduboy.width()-28,0,hearts_sp, hearts_frame);
          arduboy.fillRect(arduboy.width()-28,0,hide_w,8,INVERT);

          
          //test
          //sprite.drawPlusMask(0,0,title_sp, title_frame);
          //sprite.drawOverwrite(0,0,push_sp,push_frame);
          //arduboy.drawRect(targetX, targetY, targetWidth+5, targetHeight+5);

          

          checkCollisions();
          //enemyHealth();

          //just test

          //points
          font3x5.setCursor(0, 0);
          font3x5.print("Points: ");
          font3x5.setCursor(3*10, 0);
          font3x5.print(hitCount);

          //level
          font3x5.setCursor((arduboy.width()/2)-6, 0);
          font3x5.print("Level: ");
          font3x5.setCursor(((arduboy.width()/2)-6)+8*3, 0);
          font3x5.print(level);
        break;

      case State::GameOver:
        //Game over screen
        sprite.drawOverwrite(4,15,over, 0);
        sprite.drawOverwrite(14,35,push_sp,push_frame);

        arduboy.setCursor(48,52);
        arduboy.print("Score: ");
        arduboy.setCursor(48+5*7,52);
        arduboy.print(hitCount*10);

        break;
    }

    

    arduboy.display();
}
