#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"

bool updatePlayer(uint8_t frameCount) {

    PlayerUpdate playerUpdate = player.update(frameCount);

    if (playerUpdate == PlayerUpdate::Dead) {

        gameOver = true;
        gameState = GameState::Play_EndOfGame;
        launchParticles((player.getX() - camera.getX() + 6).getInteger(), player.getY().getInteger() + 6);
        playSFX(MusicSFX::SFX_Death, true);

        if (cookie.score > cookie.highScore) {

            cookie.highScore = cookie.score;
            saveCookie(true);

        }

    }
return true;

}


void updateCamera(Player &player) {

    SQ15x16 tcamx = player.getX() - CAMERA_OFFSET_X;

    tcamx += player.getVelocityX() * (CAMERA_OFFSET_X_VEL_DELTA / PLAYER_MAX_MOVE_VEL_X);

    if (player.getDirectionX() == Direction::Right)
        tcamx += CAMERA_OFFSET_X_DELTA;
    else
        tcamx -= CAMERA_OFFSET_X_DELTA;

    SQ15x16 d2camx =
        (tcamx - camera.getX()) * CAMERA_UPDATE_X_ALPHA -
        camera.getVelocityX() * CAMERA_UPDATE_X_BETA;

    camera.incX(camera.getVelocityX() * CAMERA_UPDATE_X_DT);
    camera.incVelocityX(d2camx * CAMERA_UPDATE_X_DT);

}


void updatePlayerBullet(int16_t player_x, Bullet &bullet) {

    if (bullet.isActive()) {

        bullet.update(player_x);
        Rect bulletRect = bullet.getRect();

        for (Enemy &enemy : enemies) {

            if (enemy.isActive() && enemy.getImageIdx() == 0) {

                Rect enemyRect = enemy.getRect();

                if (Arduboy2::collide(bulletRect, enemyRect)) {

                    bullet.setActive(false);
                    enemy.setImageIdx(1);

                    launchParticles((enemy.getX() - camera.getX() + 12).getInteger(), enemy.getY().getInteger() + 6);
                    playSFX(MusicSFX::SFX_Explosion, true);

                    switch (enemy.getEnemyType()) {

                        case EnemyType::Mine:
                            cookie.score = cookie.score + 10;
                            break;

                        case EnemyType::Plane_Start ... EnemyType::Plane_End:
                            cookie.score = cookie.score + 25;
                            break;

                        case EnemyType::Heart:
                            break;

                        case EnemyType::Zap:
                            player.decHealth(Constants::Health_Zap);
                            world.setZapFlash(11 * 3);
                            break;

                    }

                }

            }

        }

    }

}

void updateEnemyBullet(int16_t player_x, Bullet &bullet) {

    if (bullet.isActive()) {

        bullet.update(player_x);

        Rect bulletRect = bullet.getRect();
        Rect playerRect = player.getRect();

        if (Arduboy2::collide(bulletRect, playerRect)) {

            bullet.setActive(false);
            launchParticles((player.getX() - camera.getX() + 12).getInteger(), (player.getY() + 6).getInteger());
            player.decHealth(Constants::Health_Bullet);
            playSFX(MusicSFX::SFX_Explosion, true);

        }

    }
    
}

void updateEnemy(Enemy &enemy) {

    if (enemy.isActive()) {

        EnemyUpdate enemyUpdateStatus = enemy.update();

        if (enemyUpdateStatus != EnemyUpdate::Inactive) {
                
            SQ15x16 diffX = enemy.getX() - player.getX();


            // If the enemy has moved beyond the 'HUD' then wrap them ..

            if (diffX < -Constants::WorldWidth) {
                enemy.incX(2 * Constants::WorldWidth);
            }

            if (diffX > Constants::WorldWidth) {
                enemy.incX(-2 * Constants::WorldWidth);
            }

            if (gameState == GameState::Play) {

                Rect enemyRect = enemy.getRect();
                Rect playerRect = player.getRect();

                if (Arduboy2::collide(playerRect, enemyRect)) {

                    switch (enemy.getEnemyType()) {

                        case EnemyType::Mine:
                        case EnemyType::Plane_Start ... EnemyType::Plane_End:

                            if (enemy.getImageIdx() == 0) {

                                player.decHealth(Constants::Health_Plane);
                                enemy.setImageIdx(1);
                                launchParticles((enemy.getX() - camera.getX() + 12).getInteger(), enemy.getY().getInteger() + 6);
       
                                playSFX(MusicSFX::SFX_Explosion, true);

                            }

                            break;

                        case EnemyType::Zap:

                            if (enemy.getImageIdx() == 0) {

                                player.decHealth(Constants::Health_Plane);
                                enemy.setImageIdx(1);
                                launchParticles((enemy.getX() - camera.getX() + 12).getInteger(), enemy.getY().getInteger() + 6);
                                world.setZapFlash(11 * 3);
       
                                playSFX(MusicSFX::SFX_Explosion, true);

                            }

                            break;

                        case EnemyType::Heart:

                            if (enemy.getImageIdx() == 0) {

                                player.incHealth(random(Constants::HealthMax / 2, Constants::HealthMax + 1));
                                enemy.setImageIdx(11);
       
                                playSFX(MusicSFX::SFX_XPGain, true);

                            }
                        
                            break;

                    }

                }
            
            }


            // Test for other returns ..

            if (enemyUpdateStatus == EnemyUpdate::Treasure_StartPickup) {

                enemyPickup = &enemy;
                enemyPickupCounter = Constants::EnemyPickup;
                playSFX(MusicSFX::SFX_DoubleBeep, true);

            }
            else if (enemyUpdateStatus == EnemyUpdate::Treasure_PickedUp) {

                enemyPickup = nullptr;
                
                player.decTreasureCount();
                player.setTreasureBlink(128);

                if (player.getTreasureCount() == 0) {

                    player.setDeathSeq(true);

                }

                playSFX(MusicSFX::SFX_Three_Dec, true);

            }

        }
        else if (enemyUpdateStatus == EnemyUpdate::Inactive) {
                
            if (&enemy == enemyPickup) {
                enemyPickup = nullptr;
                enemyPickupCounter = 0;
            }

            relaunchEnemy(enemy);

        }

    }

}

void updateTreasures() {

    for (Treasure &treasure : treasures) {

        if (treasure.isActive()) {

            SQ15x16 diffX = treasure.getX() - player.getX();

            // If the treasures has moved beyond the 'HUD' then wrap them ..

            if (diffX < -Constants::WorldWidth) {
                treasure.incX(2 * Constants::WorldWidth);
            }

            if (diffX > Constants::WorldWidth) {
                treasure.incX(-2 * Constants::WorldWidth);
            }

        }

    }

}


void testForTreasures(Player &player, Enemy &enemy) {


    // Is the enemy near the player?

    if (player.getX() - enemy.getX() < -300 || player.getX() - enemy.getX() > 300) return;


    // Is the enemy near a treasure?

    if (enemy.isActive() && enemy.getImageIdx() == 0) {
        
        bool pickup = false;

        for (uint8_t i = 0; i < Constants::TreasureCount; i++) {

            Treasure *treasure = &treasures[i];

            if (treasure->isActive()) {
                    
                switch (enemy.getDirection()) {

                    case Direction::Right:
                        {

                            if (treasure->getX() < enemy.getX()) continue;

                            SQ15x16 xDiff = treasure->getX() - enemy.getX();
                            if (xDiff < static_cast<SQ15x16>(-60.0f) || xDiff > static_cast<SQ15x16>(60.0f)) continue;

                            SQ15x16 xRatio = xDiff / enemy.getSpeed();

                            if (xRatio >= static_cast<SQ15x16>(18.0f) && xRatio <= static_cast<SQ15x16>(22.0f)) {

                                enemy.setEnemyType(EnemyType::Plane_Decelerate);
                                enemy.setTreasure(treasure);
                                enemyPickup = &enemy;
                                enemyPickupCounter = Constants::EnemyPickup;
                                
                                playSFX(MusicSFX::SFX_DoubleBeep, true);

                            }

                        }

                        break;

                    case Direction::Left:
                        {
                            if (treasure->getX() + 10 > enemy.getX()) continue;

                            SQ15x16 xDiff = treasure->getX() - enemy.getX();
                            if (xDiff < static_cast<SQ15x16>(-50.0f) || xDiff > static_cast<SQ15x16>(70.0f)) continue;

                            SQ15x16 xRatio = xDiff / enemy.getSpeed();

                            if (xRatio >= static_cast<SQ15x16>(-22.0f) && xRatio <= static_cast<SQ15x16>(-18.0f)) {

                                enemy.setEnemyType(EnemyType::Plane_Decelerate);
                                enemy.setTreasure(treasure);
                                enemyPickup = &enemy;
                                enemyPickupCounter = Constants::EnemyPickup;

                                playSFX(MusicSFX::SFX_DoubleBeep, true);

                            }

                        }

                        break;

                }

            }

        }

    }

}
