#pragma once

/*
  Copyright (C) 2018 Pharap (@Pharap)

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Modified by Filmote and Vampirics
*/

#include "Arduboy2Ext.h"
#include "Enums.h"
#include "../ardBitmap/ArdBitmap.h"
  
char const treasureDice_Caption_01[] PROGMEM = " Gain~a~Fire~Wand!";
char const treasureDice_Caption_02[] PROGMEM = " Gain~an~Ice~Wand!";
char const treasureDice_Caption_03[] PROGMEM = "Gain~a~Poison~Wand!";
char const treasureDice_Caption_04[] PROGMEM = "Gain~a~Healing~Wand!";
char const treasureDice_Caption_05[] PROGMEM = "  Gain~an~Armour!";
char const treasureDice_Caption_06[] PROGMEM = "Upgrade~your~Weapon!";
char const treasureDice_Caption_07[] PROGMEM = "Gain~2~Gold~Pieces!";
char const treasureDice_Caption_08[] PROGMEM = "Open~chest:\nGold~and~treasure!";
char const treasureDice_Caption_09[] PROGMEM = "Open~chest:\nYou~found~gold~only!";

char const * const treasureDice_Captions[] = {
	treasureDice_Caption_01,
	treasureDice_Caption_02,
	treasureDice_Caption_03,
	treasureDice_Caption_04,
	treasureDice_Caption_05,
	treasureDice_Caption_06,
	treasureDice_Caption_07,
	treasureDice_Caption_08,
	treasureDice_Caption_09,
}; 

class GameContext {

  public:

    GameStateType gameState;
    GameContext();
     
    void resetGame();
    void incRoom(PlayerStats & playerStats);
  
  public:

    Arduboy2Ext arduboy;
    ArdBitmap<WIDTH, HEIGHT> ardBitmap;

    GameStats gameStats;
    PlayerStats playerStats;
    GameStateType cards[7];

};