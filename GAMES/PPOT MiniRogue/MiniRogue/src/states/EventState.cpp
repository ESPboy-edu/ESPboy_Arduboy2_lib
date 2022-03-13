#include "EventState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

#define FlashString FPSTR

static const uint8_t NO_OF_CARDS_IN_FLIP = 13; 
static const uint8_t CARD_DRAW_ORDER[] = { 2, 1, 0, 2, 0, 1, 0, 1, 2 };
static const uint8_t imageX[] = { 2, 28, 54 };
static const uint8_t imageY[] = { 15, 8, 15 };


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void EventState::activate(StateMachine & machine) {

  (void)machine;
  
  viewState = ViewState::SkillCheck;
  nextState = ViewState::SkillCheck;
  counter = 0;
  dice[0] = 0;
  dice[1] = 0;
  dice[2] = 0;
  skillCheck = 0;
  selection = 0;

}

// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void EventState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;

  auto justPressed = arduboy.justPressedButtons();

  switch (this->viewState) {

    case ViewState::SkillCheck:

      if (justPressed & A_BUTTON) { this->skillCheck = random(1, 7); counter = FLASH_COUNTER; }
    
			if (this->counter < sizeof(DiceDelay)) {
				
				if (arduboy.everyXFrames(pgm_read_byte(&DiceDelay[this->counter]))) {

					this->skillCheck = random(1, 7);
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

          this->counter = 0;
          this->viewState = ViewState::RollDice;

          if (this->skillCheck <= playerStats.xpTrack) {

            this->nextState = ViewState::SelectCard;

          }
          else {

            this->nextState = ViewState::UpdateStats;

          }

        }

			}
			break;

    case ViewState::RollDice:
      
			if (counter < NO_OF_CARDS_IN_FLIP - 1) {

				this->dice[1] = random(1, 7);
        this->dice[0] = this->dice[1] - 1;  if (this->dice[0] < 1) this->dice[0] = 6;
        this->dice[2] = this->dice[1] + 1;  if (this->dice[2] > 6) this->dice[2] = 1;
				counter++;

			}      
			else {
            
        this->selection = 1;
        this->viewState = this->nextState;
        this->counter = 0;

			}
			break;

    case ViewState::SelectCard:

      if (justPressed & LEFT_BUTTON && this->selection > 0)   { this->selection--; } 
      if (justPressed & RIGHT_BUTTON && this->selection < 2)  { this->selection++; } 

      if (justPressed & A_BUTTON)  { 

        this->counter = 0;
        this->viewState = ViewState::UpdateStats;

      } 
       
      break;


    case ViewState::UpdateStats:

      if (counter == 0) {

        switch (this->dice[this->selection]) {

          case 1: playerStats.incGold(2);     break;
          case 2: playerStats.incHP(2);       break;
          case 3: playerStats.incFood(1);     break;
          case 4: playerStats.incXP(2);       break;
          case 5: playerStats.incArmour(1);   break;
          case 6:                             break;

        }

      }

      if (justPressed & A_BUTTON) { this->counter = FLASH_COUNTER;}

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
      else {

        if (justPressed & A_BUTTON) { 

          if (this->dice[this->selection] < 6) {
            
            machine.changeState(gameStats.incRoom(playerStats)); 

          }
          else {
            machine.changeState(GameStateType::MonsterFromEvent);
          }

        }

      }

      break;

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void EventState::render(StateMachine & machine) {

  auto & arduboy = machine.getContext().arduboy;
  auto & ardBitmap = machine.getContext().ardBitmap;
 	auto & playerStats = machine.getContext().playerStats;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  // Render common parts ..

  for (int8_t i = -12; i < 80; i = i + 20) {
    ardBitmap.drawCompressed(i, 48, Images::Event_Background, WHITE, ALIGN_NONE, MIRROR_NONE);
  }

  BaseState::renderBackground(machine, true);

  SpritesB::drawErase(0, 56, Images::Corner_LH, 0);
  SpritesB::drawErase(85, 56, Images::Corner_RH, 0);

  switch (this->viewState) {

    case ViewState::RollDice:

      if (this->nextState == ViewState::SelectCard) {
        arduboy.fillRect(2, 15, 32, 32, BLACK);
        arduboy.fillRect(54, 15, 32, 32, BLACK);
        BaseState::renderLargeSpinningCard(machine, 2, 15, this->counter);
        BaseState::renderLargeSpinningCard(machine, 54, 15, this->counter);
      }

      arduboy.fillRect(29, 8, 30, 32, BLACK);
      BaseState::renderLargeSpinningCard(machine, 28, 8, this->counter);
      
      if (counter < NO_OF_CARDS_IN_FLIP) {

        for (uint8_t i = 0, j = 0; i < Images::Large_Spinning_Inlays[this->counter]; i++, j = j + 2) {

          if (this->nextState == ViewState::SelectCard) {
            ardBitmap.drawCompressed(6 + (this->counter * 2) + j, 15, Images::Large_Spinning_Card_Inlay, WHITE, ALIGN_NONE, MIRROR_NONE);
            ardBitmap.drawCompressed(58 + (this->counter * 2) + j, 15, Images::Large_Spinning_Card_Inlay, WHITE, ALIGN_NONE, MIRROR_NONE);
          }

          ardBitmap.drawCompressed(32 + (this->counter * 2) + j, 8, Images::Large_Spinning_Card_Inlay, WHITE, ALIGN_NONE, MIRROR_NONE);

        }

      }
      else {

        ardBitmap.drawCompressed(30, 10, Images::Event_Dice[this->dice[1] - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

      }

      break;

    case ViewState::SkillCheck:
    case ViewState::SkillCheckResult:

      font3x5.setCursor(3, 3);
      font3x5.print(F("Enough~skill?"));
      font3x5.setCursor(71, 3);
      SpritesB::drawOverwrite(56, 2, Images::Dice, this->skillCheck);

      if (viewState == ViewState::SkillCheckResult) {

        if (this->counter < FLASH_COUNTER && flash) {

          font3x5.setTextColor(BLACK);
          arduboy.fillRect(69, 3, ((this->skillCheck <= playerStats.xpTrack) ? 15 : 11), 7, WHITE);

        }

        if (this->skillCheck <= playerStats.xpTrack) {

          font3x5.print(F("Yes"));
          font3x5.setTextColor(WHITE);

        }
        else {

          font3x5.print(F("No"));
          font3x5.setTextColor(WHITE);

        }

      }

      break;

    case ViewState::SelectCard:

      for (uint8_t i = 0; i < 3; i++) {

        uint8_t renderIdx = CARD_DRAW_ORDER[(this->selection * 3) + i];

        renderLargeSpinningCard(machine, imageX[renderIdx], imageY[renderIdx], this->dice[renderIdx]);
        if (flash && this->selection == renderIdx) ardBitmap.drawCompressed(imageX[renderIdx], imageY[renderIdx], Images::Large_Spinning_Card_Highlight, BLACK, ALIGN_NONE, MIRROR_NONE);

      }

      font3x5.setCursor(4, 0);
      printEventName(this->dice[this->selection]);

      break;


    case ViewState::UpdateStats:

      renderLargeSpinningCard(machine, 28, 8, this->dice[this->selection]);
      font3x5.setCursor(4, 0);
      printEventName(this->dice[this->selection]);

      break;

  }

	// Player statistics ..

	static const FlashSettings diceHelper[] PROGMEM =
	{
		FlashSettings::None,
		FlashSettings::FlashGold,
		FlashSettings::FlashHP,
		FlashSettings::FlashFood,
		FlashSettings::FlashXP,
		FlashSettings::FlashArmour,
	};

	const FlashSettings settings = (this->dice[this->selection] <= 5) ? static_cast<FlashSettings>(pgm_read_byte(&diceHelper[this->dice[this->selection]])) : FlashSettings::None;
	const bool shouldFlash = (this->viewState == ViewState::UpdateStats && this->counter < FLASH_COUNTER);

	BaseState::renderPlayerStatistics(machine, shouldFlash, settings);

}

void EventState::printEventName(uint8_t dice) {

  font3x5.print(FlashString(eventDice_Captions[dice - 1]));

}

void EventState::renderLargeSpinningCard(StateMachine & machine, uint8_t x, uint8_t y, uint8_t dice) {

  auto & ardBitmap = machine.getContext().ardBitmap;
  auto & arduboy = machine.getContext().arduboy;

  arduboy.fillRect(x, y, 32, 32, BLACK);
  BaseState::renderLargeSpinningCard(machine, x, y, 0);
  ardBitmap.drawCompressed(x + 2, y + 2, Images::Event_Dice[dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

}