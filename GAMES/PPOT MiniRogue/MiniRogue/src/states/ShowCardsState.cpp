#include "ShowCardsState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../fonts/Font3x5.h"

#define FlashString FPSTR

constexpr const static int8_t STARVED_TO_DEATH = -1; 
constexpr const static uint8_t NO_OF_CARDS_IN_FLIP = 7; 
constexpr const static uint8_t DEAL_DELAY = 5; 
constexpr const static uint8_t CARD_SHOW_NONE = -1;
constexpr const static uint8_t CARD_SHOW_ALL = 127;
constexpr const static uint8_t CARD_NONE_SELECTED = 255;

const int8_t  cardPositionX[] PROGMEM = { -1, 19, 19, 39, 59, 59, 79 };
const uint8_t cardPositionY[] PROGMEM = { 23, 8, 36, 23, 8, 36, 23 };
const uint8_t cardIndexToRoom[] PROGMEM = { 1, 2, 2, 3, 4, 4, 5 };


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void ShowCardsState::activate(StateMachine & machine) {
	
	auto & arduboy = machine.getContext().arduboy;
  auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;

	viewState = ViewState::DealCards;
	
	counter = 0;           
	displayCard = 0;           
	numberOfCardsToDisplay = 6;

	this->numberOfCardsToDisplay = (gameStats.isLastLevelInArea() ? 7 : 6);
  arduboy.resetFrameCount();

	if (gameStats.room <= 0) {

		this->viewState = ViewState::DealCards;
		this->displayCard = -3;
		this->counter = NO_OF_CARDS_IN_FLIP;


		// Shuffle cards ..

		machine.getContext().cards[6] = GameStateType::BossMonster;
		
		for (uint8_t i = 0; i < 10; i++) { 
			
			uint8_t x = random(0, 6);
			uint8_t y = random(0, 6);
			swap(machine.getContext().cards[x], machine.getContext().cards[y]);

		}

		if (playerStats.food == STARVED_TO_DEATH) {

  		this->displayCard = CARD_SHOW_ALL;
			this->viewState = ViewState::PlayerDead;
      playerStats.food = 0;
  		gameStats.room = 0;

		}
		else {

			gameStats.room = 1;
			
		}

		//machine.getContext().cards[0] = GameStateType::Event; //sjh

	}
	else {

		this->displayCard = CARD_SHOW_ALL;
  	this->counter = NO_OF_CARDS_IN_FLIP;
		this->viewState = ViewState::PlayCard;
		
	}
  
}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void ShowCardsState::update(StateMachine & machine) {
	
	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

	switch (this->viewState) {

		case ViewState::DealCards:

			if (arduboy.everyXFrames(DEAL_DELAY)) {

				if (this->displayCard < numberOfCardsToDisplay) {
					this->displayCard++;
				}
				else {
					this->displayCard = CARD_SHOW_ALL;
        	this->viewState = ViewState::PlayCard;
					this->counter = NO_OF_CARDS_IN_FLIP;
				}

			}
			break;

		case ViewState::PlayCard:

			if ((justPressed & UP_BUTTON) && (gameStats.selectedCard == 2 || gameStats.selectedCard == 5)) 		{ gameStats.selectedCard--;}
			if ((justPressed & DOWN_BUTTON) && (gameStats.selectedCard == 1 || gameStats.selectedCard == 4)) 	{ gameStats.selectedCard ++; }

			if (justPressed & A_BUTTON) { 
		
				machine.changeState(machine.getContext().cards[gameStats.selectedCard]); 
				
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
void ShowCardsState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
	auto & ardBitmap = machine.getContext().ardBitmap;
	
	uint8_t room = gameStats.room;


	// Display area names ..

	font3x5.setTextColor(WHITE);
	font3x5.setCursor(0, 0);

	font3x5.print("L");
	font3x5.print(gameStats.getAreaId() + 1);
	font3x5.print("~A");
	font3x5.print(gameStats.level + 1);
	font3x5.print(FlashString(area_Captions[gameStats.getAreaId()]));


	// Draw background ..

	ardBitmap.drawCompressed(105, 0, Images::Health, WHITE, ALIGN_NONE, MIRROR_NONE);	
	for (int8_t i = -2; i < 93; i = i + 10) {
		ardBitmap.drawCompressed(i, 18, Images::Card_Border_Top, WHITE, ALIGN_NONE, MIRROR_NONE);
		ardBitmap.drawCompressed(i, 50, Images::Card_Border_Bottom, WHITE, ALIGN_NONE, MIRROR_NONE);
	}


	// Player statistics ..

	const FlashSettings settings = ((viewState == ViewState::PlayerDead) ? FlashSettings::FlashFood : FlashSettings::None);

	BaseState::renderPlayerStatistics(machine, true, settings);

  const bool flash = arduboy.getFrameCountHalf(12);

	for (uint8_t i = 0; i < 7; i++) {

		if (this->displayCard >= i) {

			int8_t x =  pgm_read_byte(&cardPositionX[i]);
			uint8_t y = pgm_read_byte(&cardPositionY[i]);
			uint8_t r = pgm_read_byte(&cardIndexToRoom[i]);


      // Draw cards as they are being dealt ..

			if (this->counter == NO_OF_CARDS_IN_FLIP || (room != r && this->counter > 0) || this->counter == 0) {

        BaseState::renderSpinningCard(machine, x, y, 6);

			}


      // Draw card highlight ..
      
			if (this->displayCard == CARD_SHOW_ALL && (gameStats.selectedCard == i) && flash && room != 0) {

        ardBitmap.drawCompressed(x, y, Images::Card_Outline_Highlight, BLACK, ALIGN_NONE, MIRROR_NONE);

			}

      if (room != 0) {

        if ((room > r) || (room == r && this->counter == 0) || (i == 6 && this->numberOfCardsToDisplay == 6)) {

          arduboy.fillRect(x + 3, y + 4, 14, 21, BLACK);
          ardBitmap.drawCompressed(x + 3, y + 6, Images::Card_Faces[(i == 6 && this->numberOfCardsToDisplay == 6 ? 7 : static_cast<uint8_t>(machine.getContext().cards[i]) - 1)], WHITE, ALIGN_NONE, MIRROR_NONE);

        }

        if (room == r && this->counter > 0 && this->displayCard == CARD_SHOW_ALL) {

					BaseState::renderSpinningCard(machine, x, y, this->counter - 1);

        }

      }

		}

	}

	if (this->counter > 0 && this->displayCard == CARD_SHOW_ALL) this->counter --;


	// Are we dead?

  if (viewState == ViewState::PlayerDead) {

    BaseState::renderPlayerDead();

	}

}

