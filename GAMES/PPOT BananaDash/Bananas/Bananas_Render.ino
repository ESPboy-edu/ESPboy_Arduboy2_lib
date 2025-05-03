#include <Arduboy2.h>


void playGame_Render(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a, uint8_t currentPhase) {

    Player &player = world.getPlayer();

    SpritesU::drawOverwriteFX((world.getBackground() / 2) - 128, 0, 128, 32, Images::Background, currentPlane);
    SpritesU::drawOverwriteFX((world.getBackground() / 2), 0, 128, 32, Images::Background, currentPlane);
    SpritesU::drawOverwriteFX((world.getBackground() / 2) + 128, 0, 128, 32, Images::Background, currentPlane);



    // ____________________________________________________________________________________________________________________________________________________________________________________
    //
    // Render trees ..

    for (uint8_t i = 4; i < 8; i++) {    

        uint24_t treeIdx = FX::readIndexedUInt24(Images::TreeImages, i);
        SpritesU::drawPlusMaskFX(world.getTree(i), 15, treeIdx, currentPlane);

    }


    for (uint8_t i = 0; i < 4; i++) {    

        uint24_t treeIdx = FX::readIndexedUInt24(Images::TreeImages, i);
        SpritesU::drawPlusMaskFX(world.getTree(i), 14, treeIdx, currentPlane);

    }



    // ____________________________________________________________________________________________________________________________________________________________________________________
    //
    // Render level ..
    
    int16_t xMin = (- 55 - world.getForeground()) / 16;
    int16_t xMax = xMin + 9;

    #ifdef DEBUG_RENDER
    DEBUG_PRINT(xMin);
    DEBUG_PRINT(" ");
    DEBUG_PRINTLN(xMax);
    #endif

    for (int16_t i = xMin - 1; i < xMax; i++) {    

        uint8_t levelIdx = getTile_ByIdx(i);

        switch (levelIdx) {
        
            case 0 ... 19:
                SpritesU::drawPlusMaskFX(55 + (i * 16) + world.getForeground(), 34, 16, 24, Images::LowerOnly, (levelIdx * 3) + currentPlane);
                break;

            case 20 ... 59:
                SpritesU::drawPlusMaskFX(55 + (i * 16) + world.getForeground(), 18, 16, 40, Images::Both, ((levelIdx - 20) * 3) + currentPlane);
                break;

            case 60 ... 79:
                SpritesU::drawPlusMaskFX(55 + (i * 16) + world.getForeground(), 10, 16, 48, Images::UpperOnly, ((levelIdx - 60) * 3) + currentPlane);
                break;

        }

    }


    // ____________________________________________________________________________________________________________________________________________________________________________________
    //
    // Render items  ..

    if (getReadyCount == -1) {
        
        for (uint8_t i = 2; i < Constants::Item_Count; i++) {    

            Item &item = world.getItem(i);

            int16_t x = 55 + item.getX() + world.getForeground();
            if (x < -32 || x > 128) continue;


            if (item.getItemType() == ItemType::Banana) {

                SpritesU::drawPlusMaskFX(55 + item.getX() + world.getForeground(), item.getY(), 16, 16, Images::Banana, currentPlane);

            }

            if (item.getItemType() == ItemType::Heart) {

                SpritesU::drawPlusMaskFX(58 + item.getX() + world.getForeground(), item.getY(), 9, 8, Images::Heart, currentPlane);

            }

            if (item.getItemType() == ItemType::Fire) {

                uint8_t frame = (frameCount / 4) % 6;
                SpritesU::drawPlusMaskFX(58 + item.getX() + world.getForeground(), item.getY(), 16, 16, Images::Fire, (frame * 3) + currentPlane);

            }

            if (item.getItemType() == ItemType::Fruit) {

                uint8_t frame = item.getData();
                SpritesU::drawPlusMaskFX(58 + item.getX() + world.getForeground(), item.getY(), 16, 16, Images::Fruits, (frame * 3) + currentPlane);

            }

            if (item.getItemType() >= ItemType::Bananarang_0 && item.getItemType() <= ItemType::Bananarang_3) {

                if (item.getY() > -16 && item.getY() < 64) {

                    uint8_t frame = item.getData();
                    SpritesU::drawPlusMaskFX(58 + item.getX() + world.getForeground(), item.getY(), 16, 16, Images::SpinningBananas, (frame * 3) + currentPlane);

                }

            }

        }


        // ____________________________________________________________________________________________________________________________________________________________________________________
        //
        // Render enemies ..

        for (uint8_t i = 0; i < Constants::Enemy_Count; i++) {    

            Enemy &enemy = world.getEnemy(i);

            int16_t x = 55 + enemy.getX() + world.getForeground();

            if (x < -32 || x > 128) continue;

            if (enemy.getEntityType() == EntityType::Barrel) {

                uint24_t imageIdx = getStanceImg(enemy.getStance());

                if (enemy.getCounter() == 0 || ((enemy.getCounter() / 8) % 2) == 0) { 
                    SpritesU::drawPlusMaskFX(x, enemy.getY(), 16, 16, Images::Barrel, (imageIdx * 3) + currentPlane);
                    // gEnemyRect = enemy.getRect(world.getForeground());
                }

            }

            if (enemy.getEntityType() == EntityType::Bird) {

                uint24_t imageIdx = getStanceImg(enemy.getStance());
                SpritesU::drawPlusMaskFX(x, enemy.getY(), 14, 16, Images::Bird, (imageIdx * 3) + currentPlane);

                // gEnemyRect = enemy.getRect(world.getForeground());

                // if (gEnemyRect.x != 0 && currentPlane == 2) DEBUG_BREAK

            }

            if (enemy.getEntityType() == EntityType::Spider) {

                uint24_t imageIdx = getStanceImg(enemy.getStance());
                SpritesU::drawPlusMaskFX(x, enemy.getY(), 16, 16, Images::Spider, (imageIdx * 3) + currentPlane);

                // gEnemyRect = enemy.getRect(world.getForeground());
                // if (gEnemyRect.x != 0 && currentPlane == 2) DEBUG_BREAK

            }

        }

    }


    // ____________________________________________________________________________________________________________________________________________________________________________________
    //
    // Render HUD and counters ..

    if (healthCounter > 0) {

        healthCounter = healthCounter - 1;

        if (healthFlash == false || (healthCounter / 64) % 2 == 0) {

            SpritesU::drawPlusMaskFX(1, 1, 35, 8, Images::HUD_Health, (player.getHealth() * 3) + currentPlane);
            SpritesU::drawPlusMaskFX(54, player.getY(), 16, 16, Images::Player, (player.getImageIdx() * 3) + currentPlane);

        }
        else {

            SpritesU::drawPlusMaskFX(1, 1, 35, 8, Images::HUD_Health, (12 * 3) + currentPlane);

        }

    }
    else {

        SpritesU::drawPlusMaskFX(54, player.getY(), 16, 16, Images::Player, (player.getImageIdx() * 3) + currentPlane);
        SpritesU::drawPlusMaskFX(1, 1, 35, 8, Images::HUD_Banana, (world.getBananas() * 3) + currentPlane);

    }

    if (timeCounter > 0) {

        timeCounter = timeCounter - 1;

        if ((timeCounter / 64) % 2 == 0) {

            SpritesU::drawPlusMaskFX(101, 1, 26, 8, Images::HUD_Time, ((world.getTime() / 16) * 3) + currentPlane);

        }
        else {

            SpritesU::drawPlusMaskFX(101, 1, 26, 8, Images::HUD_Time, (100 * 3) + currentPlane);

        }

    }
    else {

        SpritesU::drawPlusMaskFX(101, 1, 26, 8, Images::HUD_Time, ((world.getTime() / 16) * 3) + currentPlane);

    }


    // a.drawRect(gPlayerRect.x, gPlayerRect.y, gPlayerRect.width, gPlayerRect.height);
    // a.drawRect(gEnemyRect.x, gEnemyRect.y, gEnemyRect.width, gEnemyRect.height);


    Item &item = world.getItem(0);

    if (item.getFrame() < Constants::Puff_Max) {

        uint8_t frame = (item.getFrame() / 8) + item.getData();
        SpritesU::drawPlusMaskFX(55 + item.getX() + world.getForeground(), item.getY(), Images::Puff, (frame * 3) +  currentPlane);

    }


    Item &item1 = world.getItem(1);

    if (item1.getFrame() < Constants::Puff_Max) {

        uint8_t frame = (item1.getFrame() / 8) + item.getData();
        SpritesU::drawPlusMaskFX(55 + item1.getX() + world.getForeground(), item1.getY(), Images::Puff, (frame * 3) +  currentPlane);

    }

    if (directionCounter > 0) {

        SpritesU::drawPlusMaskFX(1, 54, 13, 8, Images::HUD_Directions, (directionCounter_Left * 3) + currentPlane);
        SpritesU::drawPlusMaskFX(114, 54, 13, 8, Images::HUD_Directions, ((8 + directionCounter_Right) * 3) + currentPlane);
        directionCounter--;

    }

}
