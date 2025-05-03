#pragma once

#include "../utils/Constants.h"
#include "../utils/Enums.h"
#include "Player.h"


struct World {

    public:

        
    private:
        
        GameState gameState = GameState::SplashScreen_Start;
        GameState prevGameState = GameState::SplashScreen_Start;
        Player player;
        Item items[Constants::Item_Count];
        Enemy enemies[Constants::Enemy_Count];

        int16_t background = 0;
        int16_t middleground = 0;
        int16_t foreground = 0;
        int16_t tree[8];
        uint16_t time;
        uint16_t bananas;

    public:

        GameState getGameState()                        { return this->gameState; }
        GameState getPrevGameState()                    { return this->prevGameState; }
        Player &getPlayer()                             { return this->player; }
        Item &getItem(uint8_t idx)                      { return this->items[idx]; }
        Enemy &getEnemy(uint8_t idx)                    { return this->enemies[idx]; }
        uint16_t getTime()                              { return this->time; }
        uint16_t getBananas()                           { return this->bananas; }
        int16_t getBackground()                         { return this->background; }
        int16_t getMiddleground()                       { return this->middleground; }
        int16_t getForeground()                         { return this->foreground; }

        void setGameState(GameState val)                { this->gameState = val; }
        void setPrevGameState(GameState val)            { this->prevGameState = val; }
        void setTime(uint16_t val)                      { this->time = val; }
        void setBananas(uint16_t val)                   { this->bananas = val; }
        void setBackground(int16_t val)                 { this->background = val; }
        void setMiddleground(int16_t val)               { this->middleground = val; }
        void setForeground(int16_t val)                 { this->foreground = val; }


        int16_t getTree(uint8_t idx) { return this->tree[idx]; }

        void setTree(uint8_t idx, int16_t val) { this->tree[idx] = val; }

        void incGameState() {

            this->gameState++;

        }

        void decGameState() {

            this->gameState--;

        }

        void decTime() {

            if (this->time > 0) {
                this->time--;
            }

        }
        
        void incForeground(int8_t val) {
            
            this->foreground = this->foreground + val;
            this->player.incVelocityX(val);
            
        }
        
        void incBackground(int8_t val) {

            for (uint8_t i = 4; i <= 7; i++) {
                this->incTree(i, val);
            }

            this->background += val;

            if (this->background >= 256 || this->background <= -256) {
                this->background = 0;
            }

        }

        void incTime(int16_t val) {
            
            if (this->time + val <= (99 * 16)) {
                this->time = this->time + val;
            }
            else {
                this->time = (99 * 16);
            }
            
        }

        void incTree(uint8_t idx, int8_t val) {

            this->tree[idx] = this->tree[idx] + val;

            if (idx < 4) {

                if (this->tree[idx] <= -64) {
                    this->tree[idx] = this->tree[idx] + 128 + 64;
                }
                else if (this->tree[idx] >= 128) {
                    this->tree[idx] = this->tree[idx] - 128 - 64;
                }

            }
            else {

                if (this->tree[idx] <= -43) {
                    this->tree[idx] = this->tree[idx] + 128 + 43;
                }

                else if (this->tree[idx] >= 128) {
                    this->tree[idx] = this->tree[idx] - 128 - 43;
                }
            
            }

        }

        bool incMiddleground(int8_t val) {

            for (uint8_t i = 0; i <= 3; i++) {
                this->incTree(i, val);
            }
            
            this->middleground = this->middleground + val;
            return true;

        }

        void update() { }


        bool canClimbUp(uint8_t tile) {
        
            switch (this->player.getY()) {

                case Constants::Level_15:
                case Constants::Level_10:

                    switch (tile) {
                    
                        case 10:
                        case 26:
                        case 35:
                        case 36:
                        case 37:
                        case 40:
                        case 42:
                            return true;

                        default:
                            return false;

                    }

                case Constants::Level_25:
                case Constants::Level_20:

                    switch (tile) {
                    
                        case 25:
                        case 26:
                        case 34:
                        case 36:
                        case 37:
                        case 43:
                        case 44:
                            return true;

                        default:
                            return false;

                    }

                default:

                    return false;
            
            }

        }


        bool canClimbDown(uint8_t tile) {
        
            switch (this->player.getY()) {

                case Constants::Level_20:
                case Constants::Level_15:

                    switch (tile) {
                    
                        case 10:
                        case 26:
                        case 35:
                        case 36:
                        case 37:
                        case 40:
                        case 42:
                            return true;

                        default:
                            return false;

                    }

                case Constants::Level_25:
                case Constants::Level_30:

                    switch (tile) {
                    
                        case 25:
                        case 26:
                        case 34:
                        case 36:
                        case 37:
                        case 43:
                        case 44:
                            return true;

                        default:
                            return false;

                    }

                default:

                    return false;
            
            }
            
        }


        bool canWalkLeft(EntityType entityType, int16_t x, uint8_t y, uint8_t tileL, uint8_t tile) {
            
            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CWL "); 
                DEBUG_PRINT(x);
                DEBUG_PRINT(", abs(x%16)=");
                DEBUG_PRINT(abs(x % 16));  
                DEBUG_PRINT(", ");
                DEBUG_PRINT(tileL); 
                DEBUG_PRINT(" "); 
                DEBUG_PRINTLN(tile); 
            }
            #endif

            switch (y) {

                case Constants::Level_10:

                    switch (tile) {
                    
                        case 11:                // Crate
                        case 12:                // Barrel
                        case 45:                // Crate
                        case 46:                // Barrel
                        case 47:                // Crate
                        case 48:                // Barrel
                        case 49:                // Crate
                        case 50:                // Barrel

                            return (entityType != EntityType::Player);

                        case 255:

                            switch (entityType) {
                            
                                case EntityType::Player:
                                    return false;
                            
                                case EntityType::Barrel:
                                    return true;

                                default:
                                    return true;

                            }

                        default:

                            break;

                    }

                    switch (tileL) {
                    
                        case 255:

                            switch (entityType) {
                            
                                case EntityType::Player:
                                    return false;
                            
                                case EntityType::Barrel:
                                    return true;

                                default:
                                    return true;

                            }

                        default:

                            return true;

                    }

                    break;

                case Constants::Level_20:

                    switch (tile) {
                    
                        case 0:
                            return (abs(x % 16) < 12 ? true : false);
                    
                        case 1:

                            if (entityType == EntityType::Player) { return true; }
                            break;

                        case 26:                // On 2 level Vine
                        case 36:                // On 2 level Ladder
                        case 60:
                        case 61:
                        case 62:
                        case 63:
                        case 64:
                        case 65:
                        case 66:
                        case 67:
                            return false;
                    
                        case 5:       
                        case 6:       
                        case 7:       
                        case 8:       
                        case 9:       
                        case 10:       
                        case 20:       
                        case 21:       
                        case 22:
                        case 24:
                        case 25:
                        case 27:
                        case 28:
                        case 29:
                        case 30:
                        case 31:
                        case 32:
                        case 33:
                        case 34:
                        case 35:       
                        case 37:       
                        case 40:       
                        case 42:       
                        case 43:       
                        case 44:       
                            return true;

                        case 3:
                        case 38:
                        case 51:

                            if (entityType == EntityType::Player) { return false; }
                            if (entityType == EntityType::Barrel) { return (abs(x % 16) == 8 ? false : true); }
                            break;

                        case 254:
                        case 255:

                            if (entityType == EntityType::Player) { return false; }
                            if (entityType == EntityType::Barrel) { return true; }
                            break;

                        default:
                            break;

                    }

                    switch (tileL) {
                    
                        case 4:
                        case 11:
                        case 12:
                        case 23:
                        case 60 ... 79:
                        case 255:

                            return false;

                        case 3:

                            if (entityType == EntityType::Player) { return (abs(x % 16) == 8 ? false : true); }
                            return false;

                        default:

                            return true;

                    }

                case Constants::Level_30:

                    switch (tile) {

                        case 20:
                        case 60:

                            return (abs(x % 16) < 12 ? true : false);

                        case 64:            // Crate
                            return (entityType != EntityType::Player);

                        case 21:
                        case 23:
                        case 25:
                        case 26:
                        case 27:
                        case 28:
                        case 29:
                        case 30:
                        case 34:
                        case 36:
                        case 37:
                        case 38:
                        case 40:
                        case 41:
                        case 42:
                        case 43:
                        case 44:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:
                        case 51:
                        case 61:
                        case 66:
                        case 67:

                            return true;

                        case 24:

                            //if (entityType == EntityType::Player) {

                                return (abs(x % 16) == 8 ? false : true);

                            //}

                            break;

                        case 33:

                            if (entityType == EntityType::Player) { return abs(x % 16) <= 8; }
                            if (entityType == EntityType::Barrel) { return true; }

                            break;

                    }

                    switch (tileL) {
                    
                        case 0 ... 19:
                        case 24:
                        case 35:
                        case 63:
                        case 64:
                        case 65:
                        case 254:
                        case 255:

                            return false;

                        default:

                            return true;

                    }

                default:

                    return false;
            
            }
            
        }

        bool canFallLeft(EntityType entityType, uint8_t y, uint8_t tileL, uint8_t tile) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFL ");
                DEBUG_PRINT(tileL);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tile);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    return false;
            
                case Constants::Level_20:

                    switch (tile) {
                        
                        case 0:
                        case 3:
                        case 38:
                        case 51:
                        case 60:
                        case 61:
                        case 62:
                        case 63:
                        case 64:
                        case 65:
                        case 66:
                        case 67:
                        case 254:
                        case 255:
                            return true;

                        default:
                            break;

                    }

                    switch (tileL) {

                        case 10:
                        case 37:
                        case 254:
                        case 255:

                            return true;

                        case 3:

                            if (entityType == EntityType::Player) { return true; }
                            return false;

                        default:

                            return false;

                    }

                case Constants::Level_30:

                    switch (tile) {
                    
                        case 24:

                            return true;

                        default:

                            break;

                    }

                    switch (tileL) {
                    
                        case 1:
                        case 10: 
                        case 33:
                        case 35:

                            return true;

                        default:

                            return false;

                    }

                default:

                    return false;
            
            }
            
        }


        bool canFallLeft_TwoLevels(EntityType entityType, uint8_t y, uint8_t tileL, uint8_t tile) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFL2 ");
                DEBUG_PRINT(tileL);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tile);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    return false;
            
                case Constants::Level_20:

                    return false;

                case Constants::Level_30:

                    switch (tile) {
                    
                        case 60:
                        case 65:
                            return true; 

                        default:

                            return false;

                    }

                default:

                    return false;
            
            }
            
        }


        bool canJumpLeft(EntityType entityType, uint8_t y, uint8_t tileL2, uint8_t tileL, uint8_t tile) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CJL ");
                DEBUG_PRINT(tileL2);
                DEBUG_PRINT(" ");
                DEBUG_PRINT(tileL);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tile);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    switch (tile) {

                        case 10:
                        case 11:
                        case 12:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileL) {

                        case 11:
                        case 12:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileL2) {

                        case 10:
                        case 11:
                        case 12:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:

                            return false; 

                        default:

                        return true;

                    }
        
                case Constants::Level_20:

                    return true;

                case Constants::Level_30:

                    switch (tile) {

                        case 64:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileL) {

                        case 64:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileL2) {

                        case 64:

                            return false; 

                        default:

                            break;

                    }

                    return true;

                default:

                    return false;
            
            }
            
        }

        bool canFallLeft_Down_1Level(EntityType entityType, int16_t x, uint8_t y, uint8_t tileL, uint8_t tile) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFLD 1L ");
                DEBUG_PRINT(x);
                DEBUG_PRINT(", abs(x%16)=");
                DEBUG_PRINT(abs(x % 16));  
                DEBUG_PRINT(", ");
                DEBUG_PRINT(tileL);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tile);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    return false;

                case Constants::Level_20:

                    switch (tile) {

                        case 3:
                        case 38:
                        case 51:

                            return abs(x % 16) == 0 || abs(x % 16) == 12;

                        case 4:
                        case 23:
                        case 26:
                        case 41:
                        case 60:
                        case 61:
                        case 62:
                        case 63:
                        case 64:
                        case 65:
                        case 66:
                        case 67:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            break;

                    }

                    switch (tileL) {

                        case 4:
                        case 23:
                        case 26:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            return false;

                    }

                case Constants::Level_30:

                    switch (tile) {

                        // case 10:

                        //     return x % 16 < 12;

                        case 24:

                            return x % 16 == 0;

                        case 0:
                        case 1:
                        case 2:
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 35:
                        case 39:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            break;

                    }

                    switch (tileL) {

                        case 35:
                        case 39:
                        case 254:
                        case 255:

                            return false; 

                        default:

                            return false;

                    }

                default:

                    return false;
            
            }
            
        }

        bool canFallLeft_Down_2Levels(EntityType entityType, uint8_t y, uint8_t tileL, uint8_t tile) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFLD 2L ");
                DEBUG_PRINT(tileL);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tile);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:
                case Constants::Level_20:

                    return false;

                case Constants::Level_30:

                    switch (tile) {

                        case 3:
                        case 4:
                        case 63:
                        case 65:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            break;

                    }

                    switch (tileL) {

                        case 3:
                        case 4:
                        case 63:
                        case 65:
                        case 254:
                        case 255:

                            return false; 

                        default:

                            return false;

                    }

                default:

                    return false;
            
            }
            
        }

        bool canWalkRight(EntityType entityType, int16_t x, uint8_t y, uint8_t tile, uint8_t tileR) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CWR "); 
                DEBUG_PRINT(x);
                DEBUG_PRINT(", abs(x%16)=");
                DEBUG_PRINT(abs(x % 16));  
                DEBUG_PRINT(", ");
                DEBUG_PRINT(tile); 
                DEBUG_PRINT(" "); 
                DEBUG_PRINTLN(tileR); 
            }
            #endif

            switch (y) {

                case Constants::Level_10:

                    switch (tile) {
                    
                        case 11:        // Crate
                        case 12:        // Barrel
                        case 45:        // Crate
                        case 46:        // Barrel
                        case 47:        // Crate
                        case 48:        // Barrel
                        case 49:        // Crate
                        case 50:        // Barrel
                        case 255:
                    
                            return (entityType != EntityType::Player);

                        default:

                            break;

                    }

                    switch (tileR) {

                        case 255:
                    
                            return false;

                        default:

                            return true;

                    }

                case Constants::Level_20:

                    switch (tile) {
                        
                        case 4:
                        case 23:
                        case 26:                // On 2 level Vine
                        case 36:                // On 2 level Ladder
                        case 60:         
                        case 61:         
                        case 62:         
                        case 63:         
                        case 64:         
                        case 65:         
                        case 66:         
                        case 67:         
                        case 254:             
                        case 255:             

                            return false;

                        case 2:

                            if (entityType == EntityType::Player) { return true; }
                            if (entityType == EntityType::Barrel) { return (abs(x % 16) != 8 ? true : false); }
                            break;

                        case 1:
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 27:
                        case 28:
                        case 29:
                        case 30:
                        case 31:
                        case 32:
                        case 33:
                        case 34:
                        case 35:
                        case 39:
                        case 40:
                        case 42:
                        case 43:
                        case 44:

                            return true;

                        default:

                            break;

                    }

                    switch (tileR) {
                    
                        case 3:
                        case 4:
                        case 11:
                        case 12:
                        case 60 ... 79:
                        case 255:

                            return false;

                        default:

                            return true;

                    }

                case Constants::Level_30:

                    switch (tile) {

                        case 64:            // Crate

                            return (entityType != EntityType::Player);

                        case 0 ... 19:
                        case 63:
                        case 254:
                        case 255:
                            return false;

                        case 21:
                        case 23:
                        case 26:
                        case 32:
                        case 33:
                        case 34:
                        case 36:
                        case 37:
                        case 38:
                        case 40:
                        case 41:
                        case 42:
                        case 43:
                        case 44:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:
                        case 51:
                        case 61:
                        case 66:
                        case 67:

                            return true;

                        case 22:

                            if (entityType == EntityType::Player) { return true; }
                            if (entityType == EntityType::Barrel) { return (abs(x % 16) != 8 ? true : false); }
                            break;

                        case 39:

                            if (entityType == EntityType::Player) { return (abs(x % 16) <= 8 ? true : false); }
                            if (entityType == EntityType::Barrel) { return false; }
                            break;

                        default:

                            break;

                    }

                    switch (tileR) {
                    
                        case 0 ... 19:
                        case 34:
                        case 35:
                        case 63:
                        case 64:
                        case 65:

                            return false;

                        default:

                            return true;

                    }

                default:

                    return false;
            
            }
            
        }

        bool canFallRight(EntityType entityType, uint8_t y, uint8_t tile, uint8_t tileR) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFR ");
                DEBUG_PRINT(tile);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tileR);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    return false;
            
                case Constants::Level_20:

                    switch (tile) {
                    
                        case 3:
                        case 23:
                        case 60:
                        case 61:
                        case 62:
                        case 63:
                        case 64:
                        case 65:
                        case 66:
                        case 67:
                        case 254:
                        case 255:
                            return true;

                        case 2:

                            if (entityType == EntityType::Barrel) { return true; }
                            break;

                        case 4:

                            return true;

                        default:

                            break;

                    }

                    switch (tileR) {
                    
                        case 10:
                        case 23:
                        case 37:
                            return true;

                        default:
                            return false;

                    }

                case Constants::Level_30:

                    switch (tile) {
                    
                        case 0 ... 19:
                        case 39:

                            return true;

                        case 22:

                            if (entityType == EntityType::Player) { return false; }
                            if (entityType == EntityType::Barrel) { return true; }
                            break;

                        default:

                            break;

                    }

                    switch (tileR) {
                    
                        case 33:
                            return true;

                        default:
                            return false;

                    }

                default:
                    return false;
            
            }
            
        }

        bool canFallRight_TwoLevels(EntityType entityType, uint8_t y, uint8_t tile, uint8_t tileR) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFR2 ");
                DEBUG_PRINT(tile);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tileR);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    return false;
            
                case Constants::Level_20:

                    return false;

                case Constants::Level_30:

                    switch (tile) {

                        case 63:
                            //DEBUG_PRINTLN("63 true");
                            return true; 

                        default:

                           return false;

                    }

                default:

                    return false;
            
            }
            
        }


        bool canJumpRight(EntityType entityType, uint8_t y, uint8_t tile, uint8_t tileR, uint8_t tileR2) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CJR ");
                DEBUG_PRINT(tile);
                DEBUG_PRINT(" ");
                DEBUG_PRINT(tileR);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tileR2);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    switch (tile) {

                        case 10:
                        case 11:
                        case 12:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileR) {

                        case 11:
                        case 12:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileR2) {

                        case 10:
                        case 11:
                        case 12:
                        case 45:
                        case 46:
                        case 47:
                        case 48:
                        case 49:
                        case 50:

                            return false; 

                        default:

                           return true;

                    }
            
                case Constants::Level_20:

                    return true;

                case Constants::Level_30:

                    switch (tile) {

                        case 64:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileR) {

                        case 64:

                            return false; 

                        default:

                            break;

                    }

                    switch (tileR2) {

                        case 64:

                            return false; 

                        default:

                            return true;

                    } 

                default:

                    return false;
            
            }
            
        }

        bool canFallRight_Down_2Levels(EntityType entityType, uint8_t y, uint8_t tile, uint8_t tileR) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFRD 2L ");
                DEBUG_PRINT(tile);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tileR);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:
                case Constants::Level_20:

                    return false;

                case Constants::Level_30:

                    switch (tile) {

                        case 3:
                        case 4:
                        case 63:
                        case 65:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            break;

                    }

                    switch (tileR) {

                        case 3:
                        // case 63:
                        case 65:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            return false;

                    }

                default:

                    return false;
            
            }
            
        }


        bool canFallRight_Down_1Level(EntityType entityType, int16_t x, uint8_t y, uint8_t tile, uint8_t tileR) {

            #ifdef DEBUG_FALLS
            if (entityType == EntityType::Player) {
                DEBUG_PRINT("CFRD 1L ");
                DEBUG_PRINT(x);
                DEBUG_PRINT(", abs(x%16)=");
                DEBUG_PRINT(abs(x % 16));  
                DEBUG_PRINT(", ");
                DEBUG_PRINT(tile);
                DEBUG_PRINT(" ");
                DEBUG_PRINTLN(tileR);
            }
            #endif

            switch (y) {
            
                case Constants::Level_10:

                    return false;

                case Constants::Level_20:

                    switch (tile) {

                        case 3:
                        case 4:
                        case 23:
                        case 41:
                        case 60:
                        case 61:
                        case 62:
                        case 63:
                        case 64:
                        case 65:
                        case 66:
                        case 67:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            break;

                    }

                    switch (tileR) {

                        case 3:
                        case 4:
                        case 23:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            return false;

                    }

                case Constants::Level_30:

                    switch (tile) {

                        case 0:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 24:
                        case 39:
                        case 63:
                        case 65:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            break;

                    }

                    switch (tileR) {

                        case 3:
                        case 4:
                        // case 63:
                        case 65:
                        case 254:
                        case 255:

                            return true; 

                        default:

                            return false;

                    }

                default:

                    return false;
            
            }
            
        }

        bool canGoIdle(uint8_t tile) {

            switch (this->player.getY()) {
            
                case Constants::Level_10:

                    return true;
            
                case Constants::Level_15:

                    return false;
            
                case Constants::Level_20:

                    switch (tile) {
                    
                        case 26:
                        case 36:
                            return false;

                        default:
                            return true;

                    }
            
                case Constants::Level_25:

                    return false;
            
                case Constants::Level_30:

                    return true;

            }

            return false;
        
        }

};