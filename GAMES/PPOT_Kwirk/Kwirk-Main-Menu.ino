#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"

void title_Init() {

    #ifdef DEBUG

        for (uint8_t i = 0; i < 20; i++) {
            level.setLevelComplete(i);
        }

    #endif

    if (cookie.hasSavedGame) {
        level.setGameState(GameState::Title_OptResume);
    }
    else {
        level.setGameState(GameState::Title_OptPlay);
    }

}

void title_Update() {

    frameCount++;

    uint8_t justPressed = getJustPressedButtons();
    uint8_t pressed = getPressedButtons();
    uint8_t activePlayer = level.getActivePlayer();

    switch (level.getGameState()) {

        #ifndef DEBUG        

            case GameState::Title_ShowCredits:

                if (justPressed & B_BUTTON || justPressed & A_BUTTON) {

                    level.setGameState(GameState::Title_OptCredits);

                }

                break;

        #endif
        
        case GameState::Title_OptPlay:

            if (justPressed & RIGHT_BUTTON) {
                level.setGameState(GameState::Title_OptSound1);
            }

            if (justPressed & A_BUTTON) {

                #ifndef DEBUG

                    switch (level.getCompletedLevels()) {

                        case 2:
                            level.setGameState(GameState::Title_SelectSkill_3_1);
                            break;

                        case 1:
                            level.setGameState(GameState::Title_SelectSkill_2_1);
                            break;

                        default:
                            level.setGameState(GameState::Title_SelectSkill_1_1);
                            break;

                    }

                #else

                    level.setGameState(GameState::Title_SelectSkill_3_1);

                #endif

            }

            break;

        #ifndef DEBUG
            
            case GameState::Title_OptSound1:

                if (justPressed & LEFT_BUTTON) {
                    level.setGameState(GameState::Title_OptPlay);
                }

                if (justPressed & RIGHT_BUTTON) {
                    level.setGameState(GameState::Title_OptCredits);
                }

                if (justPressed & A_BUTTON) {

                    level.setGameState_Next(GameState::Title_OptSound1);
                    level.setGameState(GameState::Title_OptSound_Music);

                }

                break;
            
            case GameState::Title_OptCredits:

                if (justPressed & LEFT_BUTTON) {
                    level.setGameState(GameState::Title_OptSound1);
                }

                if (justPressed & A_BUTTON) {
                    level.setGameState(GameState::Title_ShowCredits);
                }

                break;


            case GameState::Title_OptResume:

                if (justPressed & RIGHT_BUTTON) {
                    level.setGameState(GameState::Title_OptSound2);
                }

                if (justPressed & A_BUTTON) {
                    saveSoundState();
                    FX::loadGameState(cookie);
                    level.setGameState(GameState::Play);
                    level.setMenuLeft(Constants::Menu_Right);
                    restoreSoundState();
                }

                break;
    
            case GameState::Title_OptSound2:

                if (justPressed & LEFT_BUTTON) {
                    level.setGameState(GameState::Title_OptResume);
                }

                if (justPressed & RIGHT_BUTTON) {
                    level.setGameState(GameState::Title_OptRestart);
                }

                if (justPressed & A_BUTTON) {

                    level.setGameState_Next(GameState::Title_OptSound2);
                    level.setGameState(GameState::Title_OptSound_Music);

                }

                break;

            case GameState::Title_OptRestart:

                if (justPressed & LEFT_BUTTON) {
                    level.setGameState(GameState::Title_OptSound2);
                }

                if (justPressed & A_BUTTON) {
                    level.reset();
                    cookie.hasSavedGame = false;
                    saveCookie(true);
                    setSkillLevels();
                }

                break;

           
            case GameState::Title_SelectSkill_1_1:
                
                if (justPressed & A_BUTTON) {

                    level.setSkillLevel(0);
                    level.setGameState_Skill(level.getGameState());
                    level.setGameState(GameState::Title_SelectFloor_1);

                }
                
                if (justPressed & B_BUTTON) {

                    if (cookie.hasSavedGame) {
                        level.setGameState(GameState::Title_OptResume);
                    }
                    else {
                        level.setGameState(GameState::Title_OptPlay);
                    }

                }

                break;
            
            case GameState::Title_SelectSkill_2_1 ... GameState::Title_SelectSkill_2_2:
                
                if (justPressed & A_BUTTON) {

                    level.setSkillLevel(static_cast<uint8_t>(level.getGameState()) - static_cast<uint8_t>(GameState::Title_SelectSkill_2_1));
                    level.setGameState_Skill(level.getGameState());
                    level.setGameState(GameState::Title_SelectFloor_1);

                }
                
                if (justPressed & B_BUTTON) {

                    if (cookie.hasSavedGame) {
                        level.setGameState(GameState::Title_OptResume);
                    }
                    else {
                        level.setGameState(GameState::Title_OptPlay);
                    }

                }
                
                if (justPressed & UP_BUTTON && level.getGameState() != GameState::Title_SelectSkill_2_1) {

                    level.decGameState();

                }
                
                if (justPressed & DOWN_BUTTON && level.getGameState() != GameState::Title_SelectSkill_2_2) {

                    level.incGameState();

                }

                break;
            
            case GameState::Title_OptSound_Music:
                
                if (justPressed & A_BUTTON) {

                    #ifdef SOUNDS_SYNTHU

                        level.getSoundSettings().setMusic(!level.getSoundSettings().getMusic());

                        if (level.getSoundSettings().getMusic()) {
                            playMusic();
                        }
                        else {
                            SynthU::stop();
                        }

                    #endif

                }
                
                if (justPressed & B_BUTTON) {

                    level.setGameState(level.getGameState_Next());

                }
                
                if (justPressed & DOWN_BUTTON) {

                    level.incGameState();

                }

                break;
            
            case GameState::Title_OptSound_SFX:
                
                if (justPressed & A_BUTTON) {

                    #ifdef SOUNDS_SYNTHU

                        level.getSoundSettings().setSFX(!level.getSoundSettings().getSFX());

                    #endif

                }
                
                if (justPressed & B_BUTTON) {

                    level.setGameState(level.getGameState_Next());

                }
                
                if (justPressed & UP_BUTTON) {

                    level.decGameState();

                }
                
                if (justPressed & DOWN_BUTTON) {

                    level.incGameState();

                }

                break;

            case GameState::Title_OptSound_Volume:
                
                if (justPressed & LEFT_BUTTON) {

                    #ifdef SOUNDS_SYNTHU

                        if (level.getSoundSettings().getVolume() > 0) {

                            level.getSoundSettings().setVolume(level.getSoundSettings().getVolume() - 1);

                            #ifdef SOUNDS_SYNTHU
                                SynthU::setVolume(level.getSoundSettings().getVolume());
                            #endif

                        }

                    #endif

                }
                
                if (justPressed & RIGHT_BUTTON) {

                    #ifdef SOUNDS_SYNTHU

                        if (level.getSoundSettings().getVolume() < 7) {

                            level.getSoundSettings().setVolume(level.getSoundSettings().getVolume() + 1);

                            #ifdef SOUNDS_SYNTHU
                                SynthU::setVolume(level.getSoundSettings().getVolume());
                            #endif

                        }

                    #endif

                }

                if (justPressed & B_BUTTON) {

                    level.setGameState(level.getGameState_Next());

                }
                
                if (justPressed & UP_BUTTON) {

                    level.decGameState();

                }

                break;

        #endif

        case GameState::Title_SelectSkill_3_1 ... GameState::Title_SelectSkill_3_3:
            
            if (justPressed & A_BUTTON) {

                level.setSkillLevel(static_cast<uint8_t>(level.getGameState()) - static_cast<uint8_t>(GameState::Title_SelectSkill_3_1));
                level.setGameState_Skill(level.getGameState());
                level.setGameState(GameState::Title_SelectFloor_1);

            }
            
            if (justPressed & B_BUTTON) {

                if (cookie.hasSavedGame) {
                    level.setGameState(GameState::Title_OptResume);
                }
                else {
                    level.setGameState(GameState::Title_OptPlay);
                }

            }
            
            if (justPressed & UP_BUTTON && level.getGameState() != GameState::Title_SelectSkill_3_1) {

                level.decGameState();

            }
            
            if (justPressed & DOWN_BUTTON && level.getGameState() != GameState::Title_SelectSkill_3_3) {

                level.incGameState();

            }

            break;

        case GameState::Title_SelectFloor_1 ... GameState::Title_SelectFloor_10:
            
            if (justPressed & A_BUTTON) {
                
                uint8_t levelNumber = static_cast<uint8_t>(level.getGameState()) - static_cast<uint8_t>(GameState::Title_Start) + (static_cast<uint8_t>(level.getSkillLevel()) * 10);
                level.setActivePlayer(0);
                level.setLevelNumber(levelNumber);
                level.setGameState_Floor(level.getGameState());
                level.setGameState(GameState::Play_Init);

            }
            
            if (justPressed & B_BUTTON) {

                level.setGameState(level.getGameState_Skill());

            }
            
            if (justPressed & UP_BUTTON && level.getGameState() != GameState::Title_SelectFloor_1) {

                level.decGameState();

            }
            
            if (justPressed & DOWN_BUTTON && level.getGameState() != GameState::Title_SelectFloor_10) {

                level.incGameState();

            }

            break;

        case GameState::Title_FloorsComplete ... GameState::Title_FloorsComplete2:

            if (frameCount % 24 == 0) {

                switch (level.getGameState()) {

                    case GameState::Title_FloorsComplete:
                        level.setGameState(GameState::Title_FloorsComplete2);
                        break;

                    case GameState::Title_FloorsComplete2:
                        level.setGameState(GameState::Title_FloorsComplete);
                        break;
                        
                }

            }

            if (justPressed & B_BUTTON) {

                setSkillLevels();

            }

            break;

    }
       

}

void title(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {
    
    if (a.needsUpdate()) title_Update();

    uint8_t currentPlane = a.currentPlane();
    uint8_t idx = static_cast<uint8_t>(level.getGameState()) - static_cast<uint8_t>(GameState::Title_Start);

    SpritesU::drawOverwriteFX(0, 0, Images::Title, (3 * idx) + currentPlane);

    switch (level.getGameState()) {
        
        case GameState::Title_OptPlay ... GameState::Title_OptRestart:
        case GameState::Title_SelectSkill_3_1 ... GameState::Title_SelectSkill_2_2:
        case GameState::Title_FloorsComplete ... GameState::Title_FloorsComplete2:
            break;

        case GameState::Title_SelectFloor_1 ... GameState::Title_SelectFloor_10:
            {
                uint8_t levelTickStart = Constants::Level_Tick_Start[idx];

                for (uint8_t i = 0; i < 4; i++) {

                    if (level.getLevelComplete((static_cast<uint8_t>(level.getSkillLevel()) * 10) + levelTickStart + i)) {

                        SpritesU::drawPlusMaskFX(94, 30 + (i * 7), Images::Tick, currentPlane);

                    }

                }

            }

            break;


        case GameState::Title_OptSound_Music ... GameState::Title_OptSound_Volume:
            {

                if (level.getSoundSettings().getMusic())    SpritesU::drawPlusMaskFX(25, 40, Images::Sound_Checkbox, currentPlane);
                if (level.getSoundSettings().getSFX())      SpritesU::drawPlusMaskFX(25, 49, Images::Sound_Checkbox, currentPlane);

                uint8_t volume = (level.getSoundSettings().getMusic() || level.getSoundSettings().getSFX()) ? level.getSoundSettings().getVolume() : 0;

                if (level.getSoundSettings().getMusic() || level.getSoundSettings().getSFX()) {
                    SpritesU::drawOverwriteFX(84, 39, Images::Sound_Volume, (level.getSoundSettings().getVolume() * 3) + currentPlane);
                }
                else {
                    SpritesU::drawOverwriteFX(84, 39, Images::Sound_Volume, currentPlane);
                }

            }
            break;            

    }

}


void setSkillLevels() {

    #ifndef DEBUG
        
        switch (level.getCompletedLevels()) {

            case 2:

                switch (level.getGameState_Skill()) {

                    case GameState::Title_SelectSkill_1_1:
                    case GameState::Title_SelectSkill_2_1:
                    case GameState::Title_SelectSkill_3_1:
                        level.setGameState(GameState::Title_SelectSkill_3_1);
                        break;

                    case GameState::Title_SelectSkill_2_2:
                        level.setGameState(GameState::Title_SelectSkill_3_2);
                        break;

                    default:
                        break;
                        
                }

                break;

            case 1:

                switch (level.getGameState_Skill()) {

                    case GameState::Title_SelectSkill_1_1:
                    case GameState::Title_SelectSkill_2_1:
                        level.setGameState(GameState::Title_SelectSkill_2_1);
                        break;

                    default:
                        break;
                        
                }

                break;

            case 0:

                level.setGameState(GameState::Title_SelectSkill_1_1);
                break;

        }            

    #else

        level.setGameState(GameState::Title_SelectSkill_3_1);

    #endif    
        
}
