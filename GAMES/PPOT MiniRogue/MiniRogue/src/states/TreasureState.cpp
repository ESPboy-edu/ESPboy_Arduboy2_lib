#include "TreasureState.h"

#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

#define FlashString FPSTR

constexpr const static uint8_t NO_OF_CARDS_IN_FLIP = 7; 


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void TreasureState::activate(StateMachine & machine) { 

  (void)machine;

  viewState = ViewState::InitialRoll;
  counter = 0;
  dice = 0;
  foundTreasure = false;
  
}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void TreasureState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

  switch (this->viewState) {

    case ViewState::InitialRoll:
            
			if (this->counter < sizeof(DiceDelay)) {

				if (arduboy.everyXFrames(pgm_read_byte(&DiceDelay[this->counter]))) {

					this->dice = random(1, 7);

          if (justPressed & A_BUTTON) { 
            
            counter = sizeof(DiceDelay); 

          }
          else {

            this->counter++;
            arduboy.resetFrameCount();
          
          }

				}

			}
			else {

        if (this->dice >= 5) {
          
          if (justPressed & A_BUTTON) {

            this->counter = NO_OF_CARDS_IN_FLIP;
            this->viewState = ViewState::RollDice;
            this->dice = random(1, 7);

          }

        }
        else {

          playerStats.incGold(gameStats.monsterDefeated ? 2 : 1);
          this->counter = 0;
          this->viewState = ViewState::UpdateStats;   

        }

			}
			break;

    case ViewState::RollDice:

			if (counter > 0) {

				this->dice = random(1, 7);
				counter--;

			} 
			else {
          
        this->foundTreasure = true;
        if (playerStats.itemCount() >= 2 && this->dice < 5) this->dice = 7;

        switch (this->dice) {

          case 1 ... 4:   playerStats.items[this->dice - 1]++; break;
          case 5:         playerStats.incArmour(1); break;
          case 6:         playerStats.incXP(2); break;
          case 7:         playerStats.incGold(2); break;

        }
          
        this->counter = 0;
        this->viewState = ViewState::UpdateStats;
        playerStats.incGold(gameStats.monsterDefeated ? 2 : 1);

			}
			break;


    case ViewState::UpdateStats:

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }

      if (justPressed & A_BUTTON) {

        machine.changeState(gameStats.incRoom(playerStats)); 

      }

      break;

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void TreasureState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
  auto & ardBitmap = machine.getContext().ardBitmap;
  
  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  // Render common parts ..

  BaseState::renderBackground(machine, true);
  ardBitmap.drawCompressed(0, 0, Images::Chest_Background, WHITE, ALIGN_NONE, MIRROR_NONE);

  switch (this->viewState) {

    case ViewState::InitialRoll:

      if (this->counter < sizeof(DiceDelay)) {
       
        ardBitmap.drawCompressed(14, 16, Images::Chest_Closed, WHITE, ALIGN_NONE, MIRROR_NONE);
        SpritesB::drawSelfMasked(48, 1, Images::Dice, this->dice);
      
        font3x5.setCursor(3, 2);
        font3x5.print(F("Open~chest:"));

      }
      else {

        renderChestResults(machine);

      }

      break;

    case ViewState::RollDice:

      renderSelectTreasure(machine);
      break;

    case ViewState::UpdateStats:

      if (this->foundTreasure) {

        renderSelectTreasure(machine);

      }
      else {

        renderChestResults(machine);

      }

      break;

  }


	// Player statistics ..

	FlashSettings settings = FlashSettings::FlashGold;

	if (this->dice == 6)        		                    settings |= FlashSettings::FlashXP;
	else if (this->dice == 5)    		                    settings |= FlashSettings::FlashArmour;
	else if (this->dice == 4 && this->foundTreasure)		settings |= FlashSettings::FlashHP;

	const bool shouldFlash = (this->viewState == ViewState::UpdateStats && this->counter < FLASH_COUNTER);

	BaseState::renderPlayerStatistics(machine, shouldFlash, settings);

  if (this->viewState == ViewState::UpdateStats && this->foundTreasure && this->counter < FLASH_COUNTER && flash) {

    font3x5.setCursor(8, 0);
    printCaption(this->dice - 1); 

  }

}

void TreasureState::printCaption(uint8_t index) {

  font3x5.print(FlashString(treasureDice_Captions[index]));

}


void TreasureState::renderSelectTreasure(StateMachine & machine) {
	
  auto & ardBitmap = machine.getContext().ardBitmap;

  ardBitmap.drawCompressed(14, 8, Images::Chest_Open, WHITE, ALIGN_NONE, MIRROR_NONE);

  if (this->viewState == ViewState::RollDice && this->counter > 0) {

    BaseState::renderSpinningCard(machine, 34, 13, this->counter - 1, 5);

  }
  else {

    BaseState::renderSpinningCard(machine, 34, 13, 0, 5);
    ardBitmap.drawCompressed(35, 15, Images::Chest_Dice[this->dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

  }

}


void TreasureState::renderChestResults(StateMachine & machine) { 

  auto & ardBitmap = machine.getContext().ardBitmap;

  font3x5.setHeight(9);
  font3x5.setCursor(3, 2);
  SpritesB::drawOverwrite(48, 1, Images::Dice, this->dice);
  ardBitmap.drawCompressed(14, 16, Images::Chest_Closed, WHITE, ALIGN_NONE, MIRROR_NONE);
  
  if (this->dice >= 5) {

    printCaption(7);

  }
  else {

    printCaption(8);

  }

  font3x5.setHeight(8);

}
