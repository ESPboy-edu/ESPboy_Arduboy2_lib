#include "TrapState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

#define FlashString FPSTR

constexpr const static uint8_t NO_OF_CARDS_IN_FLIP = 13; 


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void TrapState::activate(StateMachine & machine) {

	(void)machine;
  
  viewState = ViewState::SkillCheck;
  counter = 0;
  dice = 0;
  
}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void TrapState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

  switch (this->viewState) {

    case ViewState::SkillCheck:

      if (justPressed & A_BUTTON) { this->dice = random(1, 7); counter = sizeof(DiceDelay); }

   		if (this->counter < sizeof(DiceDelay)) {
				
				if (arduboy.everyXFrames(pgm_read_byte(&DiceDelay[this->counter]))) {

					this->dice = random(1, 7);
					this->counter++;
					arduboy.resetFrameCount();

				}

			}
			else {    

        this->counter = 0;
        arduboy.resetFrameCount();
        this->viewState = ViewState::SkillCheckResult;

			}
			break;

    case ViewState::SkillCheckResult:

      if (justPressed & A_BUTTON) { counter = FLASH_COUNTER; }

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
			else {

        if (justPressed & A_BUTTON) {

          if (this->dice <= playerStats.xpTrack) {

            machine.changeState(gameStats.incRoom(playerStats)); 

          }
          else {

            this->counter = 0;
            this->viewState = ViewState::RollDice;

          }

        }

			}
			break;

    case ViewState::RollDice:
      
			if (counter < NO_OF_CARDS_IN_FLIP - 1) {

        this->dice = random(1, 7);
        counter++;

			}
			else {

        switch (this->dice) {

          case 1: playerStats.decFood(1);       break;
          case 2: playerStats.decGold(1);       break;
          case 3: playerStats.decArmour(1);     break;
          case 4: playerStats.decHP(1);         break;
          case 5: if (playerStats.xp > 1)       playerStats.xp--;       break;

          case 6: 
            
            playerStats.decHP(2); 
            
            if (playerStats.hp > 0) {
              gameStats.dropArea();
            }

          break;

        }


        // Have we died?

        if (playerStats.hp > 0) {

          arduboy.resetFrameCount();
          this->counter = 0;
          this->viewState = ViewState::UpdateStats;

        }
        else {

          viewState = ViewState::PlayerDead;

        }

			}
			break;

    case ViewState::UpdateStats:

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
      else {

        if (justPressed & A_BUTTON) {

          machine.changeState(gameStats.incRoom(playerStats)); 

        }

      }

      break;

		case ViewState::PlayerDead:

      if (justPressed & A_BUTTON) {
				machine.changeState(GameStateType::PlayerDead);
			}

      break;

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void TrapState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
  auto & ardBitmap = machine.getContext().ardBitmap;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  // Render common parts ..

  BaseState::renderBackground(machine, true);
  ardBitmap.drawCompressed(0, 40, Images::Trap_LHS, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(79, 40, Images::Trap_RHS, WHITE, ALIGN_NONE, MIRROR_NONE);

  for (uint8_t i = 9; i < 70; i = i + 14) {
    ardBitmap.drawCompressed(i, 40, Images::Trap_Single, WHITE, ALIGN_NONE, MIRROR_NONE);
  }

  switch (this->viewState) {

    case ViewState::SkillCheck:
    case ViewState::SkillCheckResult:

      font3x5.setCursor(3, 3);
      font3x5.print(F("Evade~trap?"));
      font3x5.setCursor(66, 3);
      SpritesB::drawOverwrite(51, 2, Images::Dice, this->dice);

      if (viewState == ViewState::SkillCheckResult) {

        if (this->counter < FLASH_COUNTER && flash) font3x5.setTextColor(BLACK);

        if (this->dice <= playerStats.xpTrack) {

          if (this->counter < FLASH_COUNTER && flash) arduboy.fillRect(64, 3, 15, 7, WHITE);
          font3x5.print(F("Yes"));

        }
        else {

          if (this->counter < FLASH_COUNTER && flash) arduboy.fillRect(64, 3, 11, 7, WHITE);
          font3x5.print(F("No"));

        }

        font3x5.setTextColor(WHITE);

      }

      break;

    case ViewState::RollDice:

      BaseState::renderLargeSpinningCard(machine, 28, 8, this->counter);

      if (counter < NO_OF_CARDS_IN_FLIP) {

        for (uint8_t i = 0, j = 0; i < Images::Large_Spinning_Inlays[this->counter]; i++, j = j + 2) {
          ardBitmap.drawCompressed(32 + (this->counter * 2) + j, 8, Images::Large_Spinning_Card_Inlay, WHITE, ALIGN_NONE, MIRROR_NONE);
        }

      }
      else {

        ardBitmap.drawCompressed(30, 10, Images::Trap_Dice[this->dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

      }

      break;

    case ViewState::UpdateStats:

      BaseState::renderLargeSpinningCard(machine, 28, 8, 0);
      ardBitmap.drawCompressed(30, 10, Images::Trap_Dice[this->dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      font3x5.setCursor(4, 0);
      printTrapName();

      break;

    case ViewState::PlayerDead:

      font3x5.setCursor(4, 0);
      printTrapName();
      renderLargeSpinningCard(machine, 28, 8, 0);
      ardBitmap.drawCompressed(30, 10, Images::Trap_Dice[dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      renderPlayerDead(35);
      break;

  }

  static const FlashSettings diceHelper[] PROGMEM = {
    FlashSettings::None,
    FlashSettings::FlashFood,
    FlashSettings::FlashGold,
    FlashSettings::FlashArmour,
    FlashSettings::FlashHP,
    FlashSettings::FlashXP,
    FlashSettings::FlashHP,
  };

	// Player statistics ..

	const FlashSettings settings = static_cast<FlashSettings>(pgm_read_byte(&diceHelper[this->dice]));
	const bool shouldFlash = (this->viewState == ViewState::UpdateStats && this->counter < FLASH_COUNTER);

	BaseState::renderPlayerStatistics(machine, shouldFlash, settings);

}

char const trapDice_Caption_01[] PROGMEM = "Mildew       ~-1~Food";
char const trapDice_Caption_02[] PROGMEM = "Tripwire     ~-1~Gold";
char const trapDice_Caption_03[] PROGMEM = "Rusted~Armour   -1~AR";
char const trapDice_Caption_04[] PROGMEM = "Spring~Blade    -1~HP";
char const trapDice_Caption_05[] PROGMEM = "Moving~Walls    -1~XP";
char const trapDice_Caption_06[] PROGMEM = "Pit     ~-2~HP,~+Fall";

char const * const trapDice_Captions[] = {
	trapDice_Caption_01,
	trapDice_Caption_02,
	trapDice_Caption_03,
	trapDice_Caption_04,
	trapDice_Caption_05,
	trapDice_Caption_06,
};

void TrapState::printTrapName() {

  font3x5.print(FlashString(trapDice_Captions[this->dice - 1]));

}
