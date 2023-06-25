#include "src/utils/Arduboy2Ext.h"  
#include <ArduboyFX.h>  
#include "fxdata/Images.h"  

#include "src/utils/Constants.h"
#include "src/utils/Stack.h"
#include "src/entities/GamePlay.h"
#include "src/entities/Entities.h"
#include "src/fonts/Font3x5.h"


// Returns true if we just entered a new room.

bool testScroll(GamePlay &gamePlay, Prince &prince, Level &level) {

    bool result = false;


    // Have we scrolled to another screen ?

    if (prince.getY() - level.getYOffset() >= 56 + Constants::TileHeight) {

        if (gamePlay.level == 6 && level.getYLocation() == 6) {

            prince.clear();
            prince.push(Stance::Leave_Gate_14_End);
            level.setYLocation(12);

        }
        else {

            prince.incY(- Constants::TileHeight * 3);
            level.setYLocation(level.getYLocation() + 3);
            level.loadMap(gamePlay);
            level.setYOffset(0);
            level.setYOffsetDir(Direction::None);

            result = true;

        }

    }
    else if (static_cast<int8_t>(prince.getY() - level.getYOffset()) < 0) {

        prince.incY(Constants::TileHeight * 3);
        level.setYLocation(level.getYLocation() - 3);
        level.loadMap(gamePlay);
        level.setYOffset(Constants::TileHeight);
        level.setYOffsetDir(Direction::None);

        result = true;

    }
    else if (prince.getX() < 0) {

        prince.incX(Constants::TileWidth * Constants::ScreenWidthInTiles);
        level.setXLocation(level.getXLocation() - 10);
        level.loadMap(gamePlay);

        if (gamePlay.level == 13 && level.getXLocation() == 0 && level.getYLocation() == 0) {

            gamePlay.gameState = GameState::Title;
            cookie.setMode(TitleScreenMode::CutScene_End);



            // Has a new high score been set?  If so, save it.

            //if ((cookie.highMin * 60) + cookie.highSec < (gamePlay.timer_Min * 60) + gamePlay.timer_Sec) {
            if ((cookie.highMin < gamePlay.timer_Min) ||
               ((cookie.highMin == gamePlay.timer_Min) && (cookie.highSec < gamePlay.timer_Sec))) {
                cookie.hasSavedScore = true;
                cookie.highMin = gamePlay.timer_Min;
                cookie.highSec = gamePlay.timer_Sec;
                cookie.highSaves = gamePlay.saves;
                cookie.hasSavedLevel = false;
                saveCookie(false);

            }

            #ifndef SAVE_MEMORY_SOUND
                setSound(SoundIndex::Victory);
            #endif

        }

        result = true;

    }
    else if (prince.getX() > Constants::TileWidth * Constants::ScreenWidthInTiles) {

        prince.incX(-Constants::TileWidth * Constants::ScreenWidthInTiles);
        level.setXLocation(level.getXLocation() + 10);
        level.loadMap(gamePlay);

        if (gamePlay.level == 8 && level.getXLocation() == 40 && level.getYLocation() == 3 && mouse.counter == 0) {

            mouse.exits++;

            if (mouse.exits > 2) {
                    
                Item gate = level.getItemByIndex(ItemType::Gate, ItemType::Gate_StayOpen, 6);

                if (gate.data.gate.position == 0) { 

                    mouse.counter = arduboy.randomLFSR(170, 240);     

                }

            }

        }

        result = true;

    }


    // Calculate screen offset ..

    if (prince.getYPrevious() <= 56 && prince.getY() > 56) {
        level.setYOffsetDir(Direction::Down);
    }
    else if (prince.getYPrevious() > 56 && prince.getY() <= 56) {
        level.setYOffsetDir(Direction::Up);
    }

    return result;

}


void processRunJump(Prince &prince, Level &level, bool testEnemy) {

    RunningJumpResult jumpResult = RunningJumpResult::None;

    #ifndef SAVE_MEMORY_ENEMY

        if (testEnemy && enemy.getHealth() > 0) {

            int16_t xDelta = prince.getPosition().x - enemy.getPosition().x;

            if (xDelta > -30 && xDelta < 30) {

                return;

            }

        }

    #endif

    if (gamePlay.level == 6 && level.getXLocation() == 0) {

        jumpResult = level.canRunningJump(prince, Action::RunJump_Level6Exit);

    }
    else {

        jumpResult = level.canRunningJump(prince, Action::RunJump_Normal);

    }


    uint24_t pos = RunningJumpStances + static_cast<uint24_t>(static_cast<uint16_t>(jumpResult) * 16);
    FX::seekData(pos);

    {
        uint16_t s1 = FX::readPendingUInt16();
        uint16_t s2 = FX::readPendingUInt16();
        uint16_t s3 = FX::readPendingUInt16();

        if (s1 != Stance::None) {
            prince.pushSequence(s1, s2, s3);
        }

    }

    {
        uint16_t s1 = FX::readPendingUInt16();
        uint16_t s2 = FX::readPendingUInt16();
        uint16_t s3 = FX::readPendingUInt16();

        if (s1 != Stance::None) {
            prince.pushSequence(s1, s2, s3);
        }

    }

    {
        uint16_t collision = FX::readPendingUInt16();
        uint16_t counter = FX::readPendingUInt16();

        if (collision == 1) {
            prince.setIgnoreWallCollisions(true);
        }
        if (counter > 0) {
            prince.setHangingCounter(static_cast<uint8_t>(counter));
        }

    }

    FX::readEnd();


    // switch (jumpResult) {

    //     case RunningJumpResult::None:
    //         prince.pushSequence(Stance::Stopping_1_Start, Stance::Stopping_5_End, Stance::Upright);
    //         break;

    //     case RunningJumpResult::Jump4_GrabLedge_Pos2:
    //         prince.pushSequence(Stance::Running_Jump_4_GL_2_1_Start, Stance::Running_Jump_4_GL_2_16_End, Stance::Jump_Up_A_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         prince.setHangingCounter(200);
    //         break;

    //     case RunningJumpResult::Jump4_GrabLedge_Pos6:
    //         prince.pushSequence(Stance::Running_Jump_4_GL_6_1_Start, Stance::Running_Jump_4_GL_6_16_End, Stance::Jump_Up_A_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         prince.setHangingCounter(200);
    //         break;

    //     case RunningJumpResult::Jump4_GrabLedge_Pos10:
    //         prince.pushSequence(Stance::Running_Jump_4_GL_10_1_Start, Stance::Running_Jump_4_GL_10_16_End, Stance::Jump_Up_A_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         prince.setHangingCounter(200);
    //         break;

    //     case RunningJumpResult::Jump4_DropLevel:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Running_Jump_4_DropLvl_1_Start, Stance::Running_Jump_4_DropLvl_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump4_DropLevel_Pos10:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Running_Jump_4_DropLvl_10_1_Start, Stance::Running_Jump_4_DropLvl_10_13);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump3_DropLevel:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Running_Jump_3_DropLvl_1_Start, Stance::Running_Jump_3_DropLvl_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump3_DropLevel_Pos10:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Running_Jump_3_DropLvl_10_1_Start, Stance::Running_Jump_3_DropLvl_10_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump3_Pos2:
    //         prince.pushSequence(Stance::Run_Repeat_5_Mid, Stance::Run_Repeat_8_End);
    //         prince.pushSequence(Stance::Running_Jump_3_2_1_Start, Stance::Running_Jump_3_2_8_End, Stance::Run_Repeat_4);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump3_Pos6:
    //         prince.pushSequence(Stance::Run_Repeat_5_Mid, Stance::Run_Repeat_8_End);
    //         prince.pushSequence(Stance::Running_Jump_3_6_1_Start, Stance::Running_Jump_3_6_8_End, Stance::Run_Repeat_4);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump3_Pos10:
    //         prince.pushSequence(Stance::Run_Repeat_5_Mid, Stance::Run_Repeat_8_End);
    //         prince.pushSequence(Stance::Running_Jump_3_10_1_Start, Stance::Running_Jump_3_10_8_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Normal_Pos2:
    //         prince.pushSequence(Stance::Running_Jump_Short_2_1_Start, Stance::Running_Jump_Short_2_7_End, Stance::Run_Repeat_4);
    //         break;

    //     case RunningJumpResult::Normal_Pos6:
    //         prince.pushSequence(Stance::Running_Jump_Short_6_1_Start, Stance::Running_Jump_Short_6_7_End, Stance::Run_Repeat_4);
    //         break;

    //     case RunningJumpResult::Jump2_Pos2:
    //         prince.pushSequence(Stance::Running_Jump_2_2_1_Start, Stance::Running_Jump_2_2_7_End, Stance::Run_Repeat_4);
    //         break;

    //     case RunningJumpResult::Jump2_Pos6:
    //         prince.pushSequence(Stance::Running_Jump_2_6_1_Start, Stance::Running_Jump_2_6_7_End, Stance::Run_Repeat_4);
    //         break;

    //     case RunningJumpResult::Jump2_Pos10:
    //         prince.pushSequence(Stance::Running_Jump_2_10_1_Start, Stance::Running_Jump_2_10_7_End, Stance::Run_Repeat_4);
    //         break;

    //     case RunningJumpResult::Jump2_DropLevel:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Running_Jump_2_DropLvl_1_Start, Stance::Running_Jump_2_DropLvl_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump2_DropLevel_Pos10:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Running_Jump_2_DropLvl_10_1_Start, Stance::Running_Jump_2_DropLvl_10_14_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case RunningJumpResult::Jump1_Pos2:
    //         prince.pushSequence(Stance::Running_Jump_1_2_1_Start, Stance::Running_Jump_1_2_8_End, Stance::Upright);
    //         break;

    //     case RunningJumpResult::Jump1_Pos6:
    //         prince.pushSequence(Stance::Running_Jump_1_6_1_Start, Stance::Running_Jump_1_6_9_End, Stance::Upright);
    //         break;

    //     case RunningJumpResult::Jump1_Pos10:
    //         prince.pushSequence(Stance::Running_Jump_1_10_1_Start, Stance::Running_Jump_1_10_10_End, Stance::Upright);
    //         break;

    //     default: break;

    // }



}

void processStandingJump(Prince &prince, Level &level) {

    StandingJumpResult standingJumpResult = level.canStandingJump(prince);



    uint24_t pos = StandingJumpStances + static_cast<uint24_t>(static_cast<uint16_t>(standingJumpResult) * 16);
    FX::seekData(pos);

    {
        uint16_t s1 = FX::readPendingUInt16();
        uint16_t s2 = FX::readPendingUInt16();
        uint16_t s3 = FX::readPendingUInt16();

        if (s1 != Stance::None) {
            prince.pushSequence(s1, s2, s3);
        }

    }

    {
        uint16_t s1 = FX::readPendingUInt16();
        uint16_t s2 = FX::readPendingUInt16();
        uint16_t s3 = FX::readPendingUInt16();

        if (s1 != Stance::None) {
            prince.pushSequence(s1, s2, s3);
        }

    }

    {
        uint16_t collision = FX::readPendingUInt16();
        uint16_t counter = FX::readPendingUInt16();

        if (collision == 1) {
            prince.setIgnoreWallCollisions(true);
        }
        if (counter > 0) {
            prince.setHangingCounter(static_cast<uint8_t>(counter));
        }

    }

    FX::readEnd();

    // switch (standingJumpResult) {

    //     case StandingJumpResult::DropLevel_36:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Standing_Jump_DL_36_1_Start, Stance::Standing_Jump_DL_36_16_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case StandingJumpResult::DropLevel_40:
    //         prince.pushSequence(Stance::Crouch_Stand_1_Start, Stance::Crouch_Stand_12_End, Stance::Upright);
    //         prince.pushSequence(Stance::Standing_Jump_DL_40_1_Start, Stance::Standing_Jump_DL_40_16_End);
    //         prince.setIgnoreWallCollisions(true);
    //         break;

    //     case StandingJumpResult::Normal_20:
    //         prince.pushSequence(Stance::Standing_Jump_20_1_Start, Stance::Standing_Jump_20_16_End, Stance::Upright);
    //         break;

    //     case StandingJumpResult::Normal_24:
    //         prince.pushSequence(Stance::Standing_Jump_24_1_Start, Stance::Standing_Jump_24_16_End, Stance::Upright);
    //         break;

    //     case StandingJumpResult::Normal_28:
    //         prince.pushSequence(Stance::Standing_Jump_28_1_Start, Stance::Standing_Jump_28_16_End, Stance::Upright);
    //         break;

    //     case StandingJumpResult::Normal_32:
    //         prince.pushSequence(Stance::Standing_Jump_32_1_Start, Stance::Standing_Jump_32_16_End, Stance::Upright);
    //         break;

    //     case StandingJumpResult::Normal_36:
    //         prince.pushSequence(Stance::Standing_Jump_36_1_Start, Stance::Standing_Jump_36_18_End, Stance::Upright);
    //         break;

    //     case StandingJumpResult::GrabLedge_28:
    //         prince.setHangingCounter(200);
    //         prince.pushSequence(Stance::Standing_Jump_GL_28_1_Start, Stance::Standing_Jump_GL_28_18_End, Stance::Jump_Up_A_14_End);
    //         break;

    //     case StandingJumpResult::GrabLedge_32:
    //         prince.setHangingCounter(200);
    //         prince.pushSequence(Stance::Standing_Jump_GL_32_1_Start, Stance::Standing_Jump_GL_32_18_End, Stance::Jump_Up_A_14_End);
    //         break;

    //     case StandingJumpResult::GrabLedge_36:
    //         prince.setHangingCounter(200);
    //         prince.pushSequence(Stance::Standing_Jump_GL_36_1_Start, Stance::Standing_Jump_GL_36_18_End, Stance::Jump_Up_A_14_End);
    //         break;

    //     case StandingJumpResult::GrabLedge_40:
    //         prince.setHangingCounter(200);
    //         prince.pushSequence(Stance::Standing_Jump_GL_40_1_Start, Stance::Standing_Jump_GL_40_18_End, Stance::Jump_Up_A_14_End);
    //         break;
        
    //     default: // StandingJumpResult::None
    //         break;

    // }

}


void initFlash(Prince &prince, Level &level, FlashType flashType) {

    Flash &flash = level.getFlash();
    flash.frame = 7;
    flash.type = flashType;
    flash.x = level.coordToTileIndexX(prince.getX()) + level.getXLocation();
    flash.y = level.coordToTileIndexY(prince.getY()) + level.getYLocation(); 

}


void initFlash(Enemy &enemy, Level &level, FlashType flashType) {

    Flash &flash = level.getFlash();
    flash.frame = 5;
    flash.type = flashType;
    flash.x = level.coordToTileIndexX(enemy.getX());
    flash.y = level.coordToTileIndexY(enemy.getY()); 

}


uint8_t activateSpikes(Prince &prince, Level &level) {

    int8_t tileXIdx = level.coordToTileIndexX(prince.getPosition().x);
    int8_t tileYIdx = level.coordToTileIndexY(prince.getPosition().y);
    uint8_t itemIdx = level.getItem(ItemType::Spikes, tileXIdx, tileYIdx);


    // Withdraw spikes after a few second..

    if (itemIdx != Constants::NoItemFound) {

        Item &spikes = level.getItem(itemIdx);
        activateSpikes_Helper(spikes);

    }

    uint8_t itemIdx2 = level.getItem(ItemType::Spikes, tileXIdx - 1, tileYIdx);

    if (itemIdx2 != Constants::NoItemFound) {

        Item &spikes = level.getItem(itemIdx2);
        activateSpikes_Helper(spikes);

    }

    itemIdx2 = level.getItem(ItemType::Spikes, tileXIdx + 1, tileYIdx);

    if (itemIdx2 != Constants::NoItemFound) {

        Item &spikes = level.getItem(itemIdx2);
        activateSpikes_Helper(spikes);

    }

    return itemIdx;
    
}


void activateSpikes_Helper(Item &spikes) {

    if (spikes.data.spikes.closingDelay == 0) {
        spikes.data.spikes.closingDelay = Constants::SpikeClosingDelay;
    }

}


void pushJumpUp_Drop(Prince &prince) {

    prince.pushSequence(Stance::Jump_Up_Drop_A_1_Start, Stance::Jump_Up_Drop_A_5_End, Stance::Upright);
    prince.pushSequence(Stance::Jump_Up_A_1_Start, Stance::Jump_Up_A_14_End);

}


bool leaveLevel(Prince &prince, Level &level) {

    int8_t tileXIdx = level.coordToTileIndexX(prince.getPosition().x);
    int8_t tileYIdx = level.coordToTileIndexY(prince.getPosition().y);

    Item &exitGate = level.getItem(Constants::Item_ExitDoor);


    // Are we close to the exist gate?  If so, exit scene ..

    if (tileXIdx == exitGate.data.location.x - 1 && tileYIdx == exitGate.data.location.y && exitGate.data.exitDoor.direction == Direction::Up) {

        switch (prince.getDirection()) {

            case Direction::Left:

                prince.pushSequence(Stance::Standing_Turn_1_Start, Stance::Standing_Turn_5_End, Stance::Upright_Turn);
                break;

            case Direction::Right:

                prince.pushSequence(Stance::Leave_Gate_01_Start, Stance::Leave_Gate_14_End);
                    
                switch (level.distToEdgeOfTile(prince.getDirection(), (level.getXLocation() * Constants::TileWidth) + prince.getX())) {

                    case 2:
                        prince.pushSequence(Stance::Small_Step_1_Start, Stance::Small_Step_6_End);
                        break;

                    case 6:
                        prince.pushSequence(Stance::Single_Step_2, Stance::Single_Step_8_End);
                        break;

                    case 10:
                        prince.pushSequence(Stance::Small_Step_1_Start, Stance::Small_Step_6_End);
                        prince.pushSequence(Stance::Single_Step_2, Stance::Single_Step_8_End);
                        break;

                    default: break;

                }

                return true;

            default: break;

        }

        return true;

    }
    else if (tileXIdx == exitGate.data.location.x && tileYIdx == exitGate.data.location.y && exitGate.data.exitDoor.direction == Direction::Up) {

        prince.pushSequence(Stance::Standing_Turn_1_Start, Stance::Standing_Turn_5_End, Stance::Upright_Turn);

        switch (prince.getDirection()) {

            case Direction::Left:
                    
                switch (level.distToEdgeOfTile(prince.getDirection(), (level.getXLocation() * Constants::TileWidth) + prince.getX())) {

                    case 6:
                        prince.pushSequence(Stance::Small_Step_1_Start, Stance::Small_Step_6_End);
                        break;

                    case 10:
                        prince.pushSequence(Stance::Single_Step_1_Start, Stance::Single_Step_8_End);
                        break;

                    default: break;

                }

                break;

            case Direction::Right:

                prince.pushSequence(Stance::Leave_Gate_01_Start, Stance::Leave_Gate_14_End);
                    
                switch (level.distToEdgeOfTile(prince.getDirection(), (level.getXLocation() * Constants::TileWidth) + prince.getX())) {

                    case 2:
                        prince.pushSequence(Stance::Single_Step_8_End, Stance::Single_Step_1_Start);
                        break;

                    case 6:
                        prince.pushSequence(Stance::Small_Step_6_End, Stance::Small_Step_1_Start);
                        break;

                    default: break;

                }

                break;

            default: break;
            
        }

        return true;

    }

    return false;

}


void pushDead(Prince &entity, Level &level, GamePlay &gamePlay, bool clear, DeathType deathType) {

    #ifndef SAVE_MEMORY_SOUND
        setSound(SoundIndex::Dead);
    #endif

    if (clear) prince.clear();

    switch (deathType) {

        case DeathType::Falling:
            entity.pushSequence(Stance::Falling_Dead_1_Start, Stance::Falling_Dead_3_End);
            break;

        default: // case DeathType::Blade, Spikes, SwordFight
            entity.pushSequence(Stance::Falling_Dead_Blade_1_Start, Stance::Falling_Dead_Blade_2_End);
            break;
            
    }

    entity.setHealth(0);

    if (gamePlay.isGameOver()) {

        showSign(entity, level, SignType::GameOver, 20);

    }
    else {

        showSign(entity, level, SignType::PressA, 20);

    }

}


void pushDead(Enemy &entity, bool clear) {

    #ifndef SAVE_MEMORY_SOUND
        setSound(SoundIndex::Triumph);
    #endif

    if (clear) entity.clear();
    entity.pushSequence(Stance::Falling_Dead_1_Start, Stance::Falling_Dead_3_End);
    entity.setHealth(0);
    entity.setMoveCount(64);

}


void showSign(Prince &prince, Level &level, SignType signType, uint8_t counter) {

    Sign &sign = level.getSign();

    switch (signType) {

        case SignType::GameOver:
            sign.type = SignType::GameOver;
            sign.x = 39;
            break;

        default:
            sign.type = SignType::PressA;
            sign.x = 24;
            break;

    }

    sign.counter = counter;

    switch (prince.getY()) {

        case 25:
            sign.y = 48;
            break;

        case 56:
        case 87:
            sign.y = 1;
            break;

    }

}

void playGrab() {

    #ifndef SAVE_MEMORY_SOUND

        switch (gamePlay.getGrab()) {

            case 0:
                setSound(SoundIndex::Grab1);
                break;

            case 1:
                setSound(SoundIndex::Grab2);
                break;

            case 2:
                setSound(SoundIndex::Grab3);
                break;

            default:
                setSound(SoundIndex::Grab4);
                break;

        }

    #endif
    
}

#ifndef SAVE_MEMORY_ENEMY

    void isEnemyVisible(Prince &prince, bool swapEnemies, bool &isVisible, bool &sameLevelAsPrince, bool justEnteredRoom) { // Should we swap active enemies (no if stack is not empty)

        bool enemyIsVisible = false;
        bool onSameLevelAsPrince = false;

        uint8_t currentEnemy = enemy.getActiveEnemy();

        for (uint8_t i = 0; i < enemy.getEnemyCount(); i++) {

            enemy.setActiveEnemy(i);

            if (enemy.getStatus() == Status::Active) {

                uint8_t tileXIdx = level.coordToTileIndexX(enemy.getPosition().x);
                uint8_t tileYIdx = level.coordToTileIndexY(enemy.getPosition().y);

                if (tileXIdx >= level.getXLocation() && tileXIdx < level.getXLocation() + Constants::ScreenWidthInTiles && tileYIdx >= level.getYLocation() && tileYIdx < level.getYLocation() + Constants::ScreenHeightInTiles) {

                    enemyIsVisible = true;

                    int8_t princeTileYIdx = level.coordToTileIndexY(prince.getPosition().y);

                    if (princeTileYIdx == tileYIdx) {
                        
                        onSameLevelAsPrince = true;

                    }

                }

                if (enemy.getHealth() == 0 && enemy.getMoveCount() > 0) {

                    enemy.decMoveCount();

                }

            }

            if (enemyIsVisible) break;
            
        }


        if (!enemyIsVisible || !swapEnemies) {
            
            enemy.setActiveEnemy(currentEnemy);

        }

        if (justEnteredRoom && enemyIsVisible) {

            BaseEntity &base = enemy.getActiveBase();
            int16_t xDelta = prince.getPosition().x - base.getPosition().x;
            
            if (base.getHealth() > 0) {

                enemy.getPosition().x = enemy.getX();
                enemy.clear();
                enemy.setStance(Stance::Upright);

                if (xDelta < 0) {

                    enemy.setX((level.getXLocation() * Constants::TileWidth) + base.getX_LeftEntry());
                    enemy.setDirection(Direction::Left);

                }
                else {

                    enemy.setX((level.getXLocation() * Constants::TileWidth) + base.getX_RightEntry());
                    enemy.setDirection(Direction::Right);

                }

            }

        }

        isVisible = enemyIsVisible;
        sameLevelAsPrince = isVisible ? onSameLevelAsPrince : false;

    }

#endif


void fixPosition() {

    switch (level.distToEdgeOfTile(prince.getDirection(), (level.getXLocation() * Constants::TileWidth) + prince.getX())) {

        case 0:
            prince.incX(2);
            break;

        case 1:
        case 5:
        case 9:
            prince.incX(1);
            break;

        case 3:
        case 7:
        case 11:
            prince.incX(-1);
            break;

        case 2:
        case 6:
        case 10:
            break;

        case 4:
        case 8:
            prince.incX(2);
            break;

    }

    prince.updateLocation(level.getXLocation(), level.getYLocation());

}


uint8_t getImageIndexFromStance(uint16_t stance) {

    #ifdef MOVEMENT_DATA_FROM_FX

        FX::seekData(Constants::StanceToImageXRefFX + stance);
        uint8_t image = FX::readPendingUInt8();
        FX::readEnd();

        return image;

    #else

        return static_cast<uint8_t>(pgm_read_byte(&Constants::StanceToImageXRef[stance]));

    #endif
    
}


void getStance_Offsets(Direction direction, Point &offset, int16_t stance) {

    #ifdef MOVEMENT_DATA_FROM_FX

        FX::seekData(Constants::Stance_XYOffsetsFX + ((stance - 1) * 2));
        offset.x = static_cast<int8_t>(FX::readPendingUInt8()) * (direction == Direction::Left ? -1 : 1) * (stance < 0 ? -1 : 1);;
        offset.y = static_cast<int8_t>(FX::readPendingUInt8()) * (stance < 0 ? -1 : 1);
        FX::readEnd();

    #else

        uint16_t idx = (stance - 1) * 2;
        offset.x = static_cast<int8_t>(pgm_read_byte(&Constants::Stance_XYOffsets[idx])) * (direction == Direction::Left ? -1 : 1) * (stance < 0 ? -1 : 1);
        offset.y = static_cast<int8_t>(pgm_read_byte(&Constants::Stance_XYOffsets[idx + 1])) * (stance < 0 ? -1 : 1);

    #endif
        
}

void processRunningTurn() {

    if (level.canMoveForward(prince, Action::RunningTurn, prince.getDirection(), 0)) {

        prince.pushSequence(Stance::Running_Turn_1_Start, Stance::Running_Turn_13_End, Stance::Run_Repeat_8_End_Turn);

    }
    else {

        prince.pushSequence(Stance::Stopping_1_Start, Stance::Stopping_5_End, Stance::Upright);

    }

}

void saveCookie(bool enableLEDs) {

    #ifdef USE_LED
    if (enableLEDs) {
        #ifndef MICROCADE
        arduboy.setRGBled(RED_LED, 32);
        #else
        arduboy.setRGBledGreenOff();
        arduboy.setRGBledBlueOff();
        #endif
    }
    #endif

    #ifdef SAVE_TO_FX

        FX::saveGameState(cookie);

    #else

        EEPROM_Utils::saveCookie(cookie);

    #endif

    #ifdef USE_LED
    if (enableLEDs) {
        #ifndef MICROCADE
        arduboy.setRGBled(RED_LED, 0);
        arduboy.setRGBled(GREEN_LED, 32);
        #else
        arduboy.setRGBledRedOff();
        arduboy.setRGBledGreenOn();
        #endif
    }
    #endif

}


#ifndef SAVE_MEMORY_SOUND
    
    void setSound(SoundIndex index) {

        bool playSound = true;

        switch (prince.getStance()) {

            case Stance::Falling_Dead_1_Start ... Stance::Falling_Dead_3_End:
            case Stance::Falling_Dead_Blade_1_Start ... Stance::Falling_Dead_Blade_2_End:

                switch (index) {

                    case SoundIndex::ChopChop:
                    case SoundIndex::GateGoingDown:
                    case SoundIndex::GateGoingUp:
                        playSound = false;

                    default:
                        break;

                }

            default:
                break;

        }

        if (playSound) {
            sound.tonesFromFX(FX::readIndexedUInt24(Sounds::Table,(uint8_t)index));
        }
        
    }
    
#endif


void handleBlades(int8_t tileXIdx, int8_t tileYIdx) {

    uint8_t itemIdx = level.getItem(ItemType::Blade, tileXIdx, tileYIdx);

    if (itemIdx != Constants::NoItemFound) {

        Item &item = level.getItem(itemIdx);

        if (abs(item.data.blade.position) <= 5) {

            pushDead(prince, level, gamePlay, true, DeathType::Blade);

        }

    }

}
