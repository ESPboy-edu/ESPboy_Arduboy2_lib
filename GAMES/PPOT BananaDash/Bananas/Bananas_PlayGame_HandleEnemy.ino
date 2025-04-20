#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"


void playGame_HandleEnemies(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a, GameState gameState) {

    Player &player = world.getPlayer();

    for (uint8_t i = 0; i < Constants::Enemy_Count; i++) {

        Enemy &enemy = world.getEnemy(i);
        uint16_t stance;

        if (!enemy.isEmpty()) {

            switch (enemy.getEntityType()) {

                case EntityType::Barrel:
                    {    

                        // Delay entry?

                        if (enemy.getY() == -6 && enemy.getCounter() > 0) {
                            enemy.setCounter(enemy.getCounter() - 1);
                            continue;
                        }


                        // Update position ..

                        Point offset;
                        stance = enemy.pop();
                        enemy.setStance(static_cast<Stance>(stance));

                        StanceDetails stanceDetails;

                        FX::seekData(Constants::StanceDetails + (static_cast<uint16_t>(stance) * 4));
                        FX::readObject(stanceDetails);
                        FX::readEnd();

                        #ifdef DEBUG_POP
                        DEBUG_PRINT("Pop ");
                        DEBUG_PRINT(stanceDetails.stanceY);
                        DEBUG_PRINT(", ");
                        DEBUG_PRINTLN(stanceDetails.foreground);
                        #endif

                        enemy.setY(enemy.getY() + stanceDetails.stanceY);
                        enemy.setX(enemy.getX() + stanceDetails.foreground);

                        if (enemy.getY() != -6 && (-world.getForeground() - enemy.getX() > 90 || -world.getForeground() - enemy.getX() < -90)) {

                            if (gameState == GameState::PlayGame) {
                                launchBarrel(enemy);
                            }
                            else {
                                enemy.setEntityType(EntityType::None);
                            }

                        }


                        Rect barrelRect = enemy.getRect(world.getForeground() + world.getXOffset());

                        for (uint8_t j = 0; j < Constants::Enemy_Count; j++) {
                        
                            Enemy &spider = world.getEnemy(j);

                            if (spider.getEntityType() == EntityType::Spider) {
                            
                                Rect spiderRect = spider.getRect(world.getForeground() + world.getXOffset());

                                if (gameState == GameState::PlayGame && collide(spiderRect, barrelRect)) {

                                    spider.pushSequence(Stance::Enemy_Spider_Die_00, Stance::Enemy_Spider_Die_05, true);

                                }

                            }

                        }

                    }

                    break;

                case EntityType::Bird:
                    {    

                        // Update position ..

                        Point offset;
                        stance = enemy.pop();
                        enemy.setStance(static_cast<Stance>(stance));

                        StanceDetails stanceDetails;

                        FX::seekData(Constants::StanceDetails + (static_cast<uint16_t>(stance) * 4));
                        FX::readObject(stanceDetails);
                        FX::readEnd();

                        #ifdef DEBUG_POP
                        DEBUG_PRINT("Pop ");
                        DEBUG_PRINT(stanceDetails.stanceY);
                        DEBUG_PRINT(", ");
                        DEBUG_PRINTLN(stanceDetails.foreground);
                        #endif

                        enemy.setY(enemy.getY() + stanceDetails.stanceY);
                        enemy.setX(enemy.getX() + stanceDetails.foreground);

                        if (enemy.isEmpty()) {

                            switch (stance) {

                                case Stance::Enemy_Flying_LH_19:

                                    #ifdef DEBUG_BIRD
                                    DEBUG_PRINT("Bird Left: ");
                                    DEBUG_PRINT(world.getForeground());
                                    DEBUG_PRINT(" ");
                                    DEBUG_PRINT(enemy.getX());
                                    DEBUG_PRINT(" ");
                                    DEBUG_PRINTLN(-world.getForeground() - enemy.getX());
                                    #endif

                                    if (-world.getForeground() - enemy.getX() > 100) {

                                        if (gameState == GameState::PlayGame) {

                                            enemy.setX(-world.getForeground() + 160 + (a.randomLFSR(0, 10) * 16));

                                        }
                                    
                                    }
                                    
                                    enemy.pushSequence(Stance::Enemy_Flying_LH_00, Stance::Enemy_Flying_LH_19);
                                    break;

                                case Stance::Enemy_Flying_RH_19:

                                    #ifdef DEBUG_BIRD
                                    DEBUG_PRINT("Bird Right: ");
                                    DEBUG_PRINT(world.getForeground());
                                    DEBUG_PRINT(" ");
                                    DEBUG_PRINT(enemy.getX());
                                    DEBUG_PRINT(" ");
                                    DEBUG_PRINTLN(-world.getForeground() - enemy.getX());
                                    #endif

                                    if (-world.getForeground() - enemy.getX() < -100) {

                                        if (gameState == GameState::PlayGame) {

                                            enemy.setX(-world.getForeground() - 160 - (a.randomLFSR(0, 10) * 16));

                                        }
                                    
                                    }
                                    
                                    enemy.pushSequence(Stance::Enemy_Flying_RH_00, Stance::Enemy_Flying_RH_19);
                                    break;


                            }

                        }

                    }

                    break;

                case EntityType::Spider:
                    {    

                        // Update position ..

                        Point offset;
                        stance = enemy.pop();
                        enemy.setStance(static_cast<Stance>(stance));

                        StanceDetails stanceDetails;

                        FX::seekData(Constants::StanceDetails + (static_cast<uint16_t>(stance) * 4));
                        FX::readObject(stanceDetails);
                        FX::readEnd();

                        #ifdef DEBUG_POP
                        DEBUG_PRINT("Pop ");
                        DEBUG_PRINT(stanceDetails.stanceY);
                        DEBUG_PRINT(", ");
                        DEBUG_PRINTLN(stanceDetails.foreground);
                        #endif

                        enemy.setY(enemy.getY() + stanceDetails.stanceY);
                        enemy.setX(enemy.getX() + stanceDetails.foreground);

                        if (enemy.isEmpty()) {

                            switch (stance) {

                                case Stance::Enemy_Spider_R1_15:
                                case Stance::Enemy_Spider_L1_15:
                                case Stance::Enemy_Spider_Idle_15:

                                    switch (a.randomLFSR(0, 10)) {
                                    
                                        case 0:
                                            enemy.pushSequence(Stance::Enemy_Spider_L1_00, Stance::Enemy_Spider_L1_15);
                                            break;

                                        case 1:
                                            enemy.pushSequence(Stance::Enemy_Spider_L1_07, Stance::Enemy_Spider_L1_15);
                                            break;
                                    
                                        case 2:
                                            enemy.pushSequence(Stance::Enemy_Spider_R1_00, Stance::Enemy_Spider_R1_15);
                                            break;

                                        case 3:
                                            enemy.pushSequence(Stance::Enemy_Spider_R1_07, Stance::Enemy_Spider_R1_15);
                                            break;

                                        case 4 ... 5:

                                            if (-world.getForeground() < enemy.getX()) {

                                                enemy.pushSequence(Stance::Enemy_Spider_L1_00, Stance::Enemy_Spider_L1_15);

                                            }
                                            else {

                                                enemy.pushSequence(Stance::Enemy_Spider_R1_00, Stance::Enemy_Spider_R1_15);

                                            }

                                            break;

                                        default:
                                            {
                                                uint8_t r = a.randomLFSR(2, 16);
                                                enemy.pushSequence(static_cast<Stance>(static_cast<uint16_t>(Stance::Enemy_Spider_Idle_00) + r), Stance::Enemy_Spider_Idle_15);
                                            }
                                            break;

                                    }

                                    enemy.pushSequence(Stance::Enemy_Spider_R1_00, Stance::Enemy_Spider_R1_15);
                                    break;

                            }

                        }

                    }

                    break;

                default: break;

            }

        }
        else {

            uint16_t stance = enemy.getStance();
            uint8_t tileL = getEnemyTile(enemy.getX() + 4, -1);
            uint8_t tile = getEnemyTile(enemy.getX() + 4, 0);
            uint8_t tileR = getEnemyTile(enemy.getX() + 4, 1);
            
            switch (enemy.getEntityType()) {

                case EntityType::Barrel:
                    {    

                        switch (stance) {

                            case Stance::Enemy_Fall_1L_06: 
                            case Stance::Enemy_Falling_1L_LH_07:
                            case Stance::Enemy_Falling_1L_RH_07:
                            case Stance::Enemy_Falling_2L_LH_11:
                            case Stance::Enemy_Falling_2L_RH_11:

                                if (a.randomLFSR(0, 2) == 0) { 
                                    enemy.pushSequence(Stance::Enemy_Rolling_LH_00, Stance::Enemy_Rolling_LH_01);
                                }
                                else {
                                    enemy.pushSequence(Stance::Enemy_Rolling_RH_00, Stance::Enemy_Rolling_RH_01);
                                }

                                break;

                            case Stance::Enemy_Rolling_LH_01:
                            case Stance::Enemy_Rolling_LH_03:

                                if (world.canWalkLeft(EntityType::Barrel,enemy.getX(), enemy.getY(), tileL, tile)) {

                                    if (enemy.getStance() == Stance::Enemy_Rolling_LH_01) {
                                        enemy.pushSequence(Stance::Enemy_Rolling_LH_02, Stance::Enemy_Rolling_LH_03);
                                    }
                                    else {
                                        enemy.pushSequence(Stance::Enemy_Rolling_LH_00, Stance::Enemy_Rolling_LH_01);
                                    }

                                }
                                else if (world.canFallLeft_TwoLevels(EntityType::Barrel, enemy.getY(), tileL, tile)) {
                                    
                                    enemy.pushSequence(Stance::Enemy_Falling_2L_LH_00, Stance::Enemy_Falling_2L_LH_11);

                                }                                
                                else if (world.canFallLeft(EntityType::Barrel, enemy.getY(), tileL, tile)) {
                                    
                                    enemy.pushSequence(Stance::Enemy_Falling_1L_LH_00, Stance::Enemy_Falling_1L_LH_07);

                                }                                

                                break;


                            case Stance::Enemy_Rolling_RH_01:
                            case Stance::Enemy_Rolling_RH_03:

                                if (world.canWalkRight(EntityType::Barrel, enemy.getX(), enemy.getY(), tile, tileR)) {

                                    if (enemy.getStance() == Stance::Enemy_Rolling_RH_01) {
                                        enemy.pushSequence(Stance::Enemy_Rolling_RH_02, Stance::Enemy_Rolling_RH_03);
                                    }
                                    else {
                                        enemy.pushSequence(Stance::Enemy_Rolling_RH_00, Stance::Enemy_Rolling_RH_01);
                                    }

                                }
                                else if (world.canFallRight_TwoLevels(EntityType::Barrel, enemy.getY(), tile, tileR)) {

                                    enemy.pushSequence(Stance::Enemy_Falling_2L_RH_00, Stance::Enemy_Falling_2L_RH_11);

                                }                                
                                else if (world.canFallRight(EntityType::Barrel, enemy.getY(), tile, tileR)) {

                                    enemy.pushSequence(Stance::Enemy_Falling_1L_RH_00, Stance::Enemy_Falling_1L_RH_07);

                                }                                

                                break;

                        }

                    }

                    break;

                case EntityType::Spider:

                    if (enemy.getStance() == Stance::Enemy_Spider_Die_05) {

                        enemy.setY(46);

                        if (a.randomLFSR(0, 2)) { 

                            enemy.setX(-world.getForeground() + 100);
                            enemy.setStance(Stance::Enemy_Spider_L1_00);
                            enemy.pushSequence(Stance::Enemy_Spider_L1_00, Stance::Enemy_Spider_L1_15);
                            enemy.setCounter(0);

                        }
                        else {

                            enemy.setX(-world.getForeground() - 100);
                            enemy.setStance(Stance::Enemy_Spider_R1_00);
                            enemy.pushSequence(Stance::Enemy_Spider_R1_00, Stance::Enemy_Spider_R1_15);
                            enemy.setCounter(0);
                        
                        }

                    }

                    break;


                default: break;

            }

        }

    }

}

void launchBarrel(Enemy &enemy) {

    bool contTile = true;
    uint8_t worldRepeat = abs(world.getForeground()) / (Constants::Tile_Count * 16);
    if (worldRepeat == Constants::World_Repeat) worldRepeat = worldRepeat - 1;

    while (contTile) {

        uint16_t r = a.randomLFSR(0, Constants::Tile_Count);
        uint8_t idx = Level::Level[r];

        switch (idx) {
        
            case 20 ... 23:
            case 25 ... 34:
            case 36 ... 38:
            case 40 ... 41:

                if (!doesBananaOrFiteOrHeartExistHere((r * 16) + (worldRepeat * Constants::Tile_Count * 16), 15)) {
                
                    enemy.setEntityType(EntityType::Barrel);
                    enemy.setY(-6);
                    enemy.setX((r * 16) + (worldRepeat * Constants::Tile_Count * 16));
                    enemy.setCounter(48);

                    enemy.setStance(Stance::Enemy_Fall_1L_00);
                    enemy.pushSequence(Stance::Enemy_Fall_1L_00, Stance::Enemy_Fall_1L_06, true);

                    contTile = false;

                }

                break;

        }

    }

}

void launchBanana(Item &bananaToLaunch) {

    bool contTile = true;
    uint8_t worldRepeat = abs(world.getForeground()) / (Constants::Tile_Count * 16);
    if (worldRepeat == Constants::World_Repeat) worldRepeat = worldRepeat - 1;

    while (contTile) {

        uint16_t r = a.randomLFSR(0, Constants::Tile_Count);
        uint16_t h = a.randomLFSR(0, 3);
        
        if (abs(world.getForeground() + (r * 16) + (worldRepeat * Constants::Tile_Count * 16)) < 100) continue;

        uint8_t idx = Level::Level[r];
        uint8_t flags = FX::readIndexedUInt8(Constants::BananaLaunch, idx);
        
        switch (h) {
        
            case 0:

                if ((flags & 1) && !doesBananaOrFiteOrHeartExistHere((r * 16) + (worldRepeat * Constants::Tile_Count * 16), 47)) {

                    bananaToLaunch.setItemType(ItemType::Banana);
                    bananaToLaunch.setY(47);
                    bananaToLaunch.setX((r * 16) + (worldRepeat * Constants::Tile_Count * 16));
                    contTile = false;
                
                }

                break;
        
            case 1:

                if ((flags & 2) && !doesBananaOrFiteOrHeartExistHere((r * 16) + (worldRepeat * Constants::Tile_Count * 16), 31)) {

                    bananaToLaunch.setItemType(ItemType::Banana);
                    bananaToLaunch.setY(31);
                    bananaToLaunch.setX((r * 16) + (worldRepeat * Constants::Tile_Count * 16));
                    contTile = false;
                
                }

                break;
        
            case 2:

                if ((flags & 4) && !doesBananaOrFiteOrHeartExistHere((r * 16) + (worldRepeat * Constants::Tile_Count * 16), 15)) {

                    bananaToLaunch.setItemType(ItemType::Banana);
                    bananaToLaunch.setY(15);
                    bananaToLaunch.setX((r * 16) + (worldRepeat * Constants::Tile_Count * 16));
                    contTile = false;
                
                }

                break;

        }

    }

}


void launchHeart(Item &bananaToLaunch) {

    bool contTile = true;
    uint8_t worldRepeat = abs(world.getForeground()) / (Constants::Tile_Count * 16);
    if (worldRepeat == Constants::World_Repeat) worldRepeat = worldRepeat - 1;

    while (contTile) {

        uint16_t r = a.randomLFSR(0, Constants::Tile_Count);
        uint16_t h = a.randomLFSR(0, 3);

        uint8_t idx = Level::Level[r];
        uint8_t flags = FX::readIndexedUInt8(Constants::BananaLaunch, idx);
        
        if (abs(world.getForeground() + (r * 16) + (worldRepeat * Constants::Tile_Count * 16)) < 80) continue;

        switch (h) {
        
            case 0:

                if ((flags & 1) && !doesBananaOrFiteOrHeartExistHere((r * 16) + (worldRepeat * Constants::Tile_Count * 16), 47)) {

                    bananaToLaunch.setItemType(ItemType::Heart);
                    bananaToLaunch.setY(50);
                    bananaToLaunch.setX((r * 16) + (worldRepeat * Constants::Tile_Count * 16));
                    bananaToLaunch.setCounter(0);
                    contTile = false;
                
                }

                break;
        
            case 1:

                if ((flags & 2) && !doesBananaOrFiteOrHeartExistHere((r * 16) + (worldRepeat * Constants::Tile_Count * 16), 31)) {

                    bananaToLaunch.setItemType(ItemType::Heart);
                    bananaToLaunch.setY(34);
                    bananaToLaunch.setX((r * 16) + (worldRepeat * Constants::Tile_Count * 16));
                    bananaToLaunch.setCounter(0);
                    contTile = false;
                
                }

                break;
        
            case 2:

                if ((flags & 4) && !doesBananaOrFiteOrHeartExistHere((r * 16) + (worldRepeat * Constants::Tile_Count * 16), 15)) {

                    bananaToLaunch.setItemType(ItemType::Heart);
                    bananaToLaunch.setY(18);
                    bananaToLaunch.setX((r * 16) + (worldRepeat * Constants::Tile_Count * 16));
                    bananaToLaunch.setCounter(0);
                    contTile = false;
                
                }

                break;

        }

    }

}

bool doesBananaOrFiteOrHeartExistHere(int16_t x, uint8_t y) {

    // Make sure there is not an existing banana in the same location ..

    for (uint8_t i = 1; i < Constants::Item_Count; i++) {
    
        Item item = world.getItem(i);

        if (item.getItemType() == ItemType::Banana && 
            abs(item.getY() - y) < 8 && 
            item.getX() == x) {
            return true;
        }

        if (item.getItemType() == ItemType::Fire && 
            abs(item.getY() - y) < 8 && 
            item.getX() == x) {
            return true;
        }

        if (item.getItemType() == ItemType::Heart && 
            abs(item.getY() - y) < 8 && 
            item.getX() == x) {
            return true;
        }

    }

    return false;

}