#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"

uint8_t getJustPressedButtons() {

    a.pollButtons();

    return a.justPressedButtons();

}

uint8_t getPressedButtons() {

    return a.pressedButtons();

}

void saveCookie(bool enableLEDs) {

    #ifdef USE_LED
    if (enableLEDs) {
        #ifndef MICROCADE
            a.setRGBled(RED_LED, 32);
        #else
            a.setRGBledGreenOff();
            a.setRGBledBlueOff();
        #endif
    }
    #endif

    FX::saveGameState(cookie);

    #ifdef USE_LED
    if (enableLEDs) {
        #ifndef MICROCADE
            a.setRGBled(RED_LED, 0);
            a.setRGBled(GREEN_LED, 32);
        #else
            a.setRGBledRedOff();
            a.setRGBledGreenOn();
        #endif
    }
    #endif

}

void deactivateLEDs() {

    #ifdef USE_LED
        #ifndef MICROCADE
            a.setRGBled(0, 0, 0);
        #else
            a.setRGBledRedOff();
            a.setRGBledGreenOff();
            a.setRGBledBlueOff();
        #endif
    #endif

}


void renderEnemies(uint8_t currentPlane, bool pickupTreasures) {

    for (Enemy &enemy : enemies) {

        if (enemy.isActive()) {

            switch (enemy.getEnemyType()) {

                case EnemyType::Plane:
                case EnemyType::Plane_Decelerate:
                case EnemyType::Plane_SetHeight:
                case EnemyType::Plane_Accelerate:

                    switch (enemy.getDirection()) {

                        case Direction::Right:

                            SpritesU::drawPlusMaskFX((enemy.getX() - camera.getX()).getInteger(), enemy.getY().getInteger(), Images::Enemy_00, (12 + enemy.getImageIdx()) + currentPlane);
                            break;

                        default:

                            SpritesU::drawPlusMaskFX((enemy.getX() - camera.getX()).getInteger(), enemy.getY().getInteger(), Images::Enemy_00, enemy.getImageIdx() + currentPlane);
                            break;

                    }

                    break;

                case EnemyType::Plane_Pickup:
                
                    switch (enemy.getDirection()) {

                        case Direction::Right:

                            SpritesU::drawPlusMaskFX((enemy.getX() - camera.getX()).getInteger(), enemy.getY().getInteger(), Images::Enemy_00_Pickup, ((9 + enemy.getPickupImageIdx()) * 3) + currentPlane);
                            break;

                        default:

                            SpritesU::drawPlusMaskFX((enemy.getX() - camera.getX()).getInteger(), enemy.getY().getInteger(), Images::Enemy_00_Pickup, (enemy.getPickupImageIdx() * 3) + currentPlane);
                            break;

                    }

                    break;


                case EnemyType::Mine:
        
                    SpritesU::drawPlusMaskFX((enemy.getX() - camera.getX()).getInteger(), enemy.getY().getInteger(), Images::Enemy_01, enemy.getImageIdx() + currentPlane);
                    break;

                case EnemyType::Heart:
        
                    SpritesU::drawPlusMaskFX((enemy.getX() - camera.getX()).getInteger(), enemy.getY().getInteger(), Images::Enemy_02, (((frameCount / 12) % 4) * 3) + currentPlane);
                    break;

                case EnemyType::Zap:
        
                    SpritesU::drawPlusMaskFX((enemy.getX() - camera.getX()).getInteger(), enemy.getY().getInteger(), Images::Enemy_03, (enemy.getPickupImageIdx() * 3) + currentPlane);
                    break;
                    
            }

        }

        if (currentPlane == 2) {
    
            updateEnemy(enemy);

            if (pickupTreasures && enemyPickup == nullptr && enemy.getEnemyType() == EnemyType::Plane && frameCount % 4 == 0) {

                testForTreasures(player, enemy);

            }

        }

    }

}


void launchTreasure(Treasure &treasure) {

    treasure.setActive(true);
    treasure.setX(static_cast<SQ15x16>(random(-Constants::WorldWidth.getInteger() + 100, Constants::WorldWidth.getInteger() - 100)));
    treasure.setY(51);

}


void relaunchEnemy(Enemy &enemy) {

    if (random(0, 2) == 0) {
        enemy.setDirection(Direction::Left);
        enemy.setX(player.getX() + Constants::WorldWidth.getInteger() - 10);
    }
    else {
        enemy.setDirection(Direction::Right);
        enemy.setX(player.getX() - Constants::WorldWidth.getInteger() + 10);
    }

    if (enemy.getEnemyType() >= EnemyType::Plane_Start) {

        enemy.setEnemyType(EnemyType::Plane);
    }

    enemy.setActive(true);
    enemy.setImageIdx(0);
    enemy.setY(random(0, 42));
    enemy.setSpeed(static_cast<SQ15x16>(static_cast<SQ15x16>(random(12, 24)) / 8));

}


void launchEnemy(Enemy &enemy) {

    enemy.setActive(true);

    if (random(0, 2) == 0) {
        enemy.setX(static_cast<SQ15x16>(random(-Constants::WorldWidth.getInteger() + 100, -50)));
    }
    else {
        enemy.setX(static_cast<SQ15x16>(50, Constants::WorldWidth.getInteger() - 100));
    }

    enemy.setY(random(0, 42));
    enemy.setSpeed(static_cast<SQ15x16>(static_cast<SQ15x16>(random(12, 24)) / 8));
    
    if (random(0, 2) == 0) {
        enemy.setDirection(Direction::Left);
    }
    else {
        enemy.setDirection(Direction::Right);
    }

}
