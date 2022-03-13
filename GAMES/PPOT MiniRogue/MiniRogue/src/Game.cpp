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

#include "Game.h"

#include "states/States.h"
#include "utils/Utils.h"

/*
#ifdef SOUND
long t;
uint8_t hpISR = 46;

ISR(TIMER3_COMPA_vect)
{
    t++;
    OCR4A = ((t*(t>>8|t>>9)&hpISR&t>>8))^(t&t>>13|t>>6); // by xpansive
}
#endif
*/

void Game::setup(void) {

	// #ifdef SOUND

	// 	// set up Timer 3
	// 	TCCR3A = 0; // normal operation
	// 	TCCR3B = _BV(WGM32) | _BV(CS30); // CTC, no pre-scaling
	// 	OCR3A = 1999; // compare A register value (8000Hz)
	// 	TIMSK3 = _BV(OCIE3A); // interrupt on Compare A Match		// // set up Timer 3
	// 	// TCCR3A = 0; // normal operation
	// 	// TCCR3B = _BV(WGM32) | _BV(CS30); // CTC, no pre-scaling
	// 	// OCR3A = 1999; // compare A register value (8000Hz)
	// 	// TIMSK3 = _BV(OCIE3A); // interrupt on Compare A Match

	// 	//set up Timer 4
	// 	TCCR4A = _BV(COM4A0) | _BV(PWM4A); // Fast-PWM 8-bit
	// 	TCCR4B = _BV(CS40); // 62500Hz
	// 	OCR4C = 0xFF; // Resolution to 8-bit (TOP=0xFF)

	// #endif

	auto & arduboy = this->context.arduboy;

	arduboy.boot();
	arduboy.flashlight();
	arduboy.systemButtons();
	#ifdef USE_LEDS
	// 3 parameter version must first be called at least once, in order to initialize the hardware
	arduboy.setRGBled(0, 0, 0);
	#endif
	arduboy.audio.begin();
	arduboy.initRandomSeed();
	arduboy.setFrameRate(60);

	this->currentState = GameStateType::SplashScreen;
	this->splashScreenState.activate(*this);
}

void Game::loop(void) {

	auto &arduboy = this->context.arduboy;
	if (!arduboy.nextFrame()) return;

	arduboy.pollButtons();
	arduboy.clear();

	switch (currentState) {

		case GameStateType::Resting:
		
			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->restingState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->restingState.update(*this);
			this->restingState.render(*this);
			break;

		case GameStateType::Treasure:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->treasureState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->treasureState.update(*this);
			this->treasureState.render(*this);
			break;

		case GameStateType::Event:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->eventState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->eventState.update(*this);
			this->eventState.render(*this);
			break;

		case GameStateType::Merchant:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->merchantState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->merchantState.update(*this);
			this->merchantState.render(*this);
			break;

		case GameStateType::Trap:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->trapState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->trapState.update(*this);
			this->trapState.render(*this);
			break;

		case GameStateType::Monster:
		case GameStateType::BossMonster:
		case GameStateType::MonsterFromEvent:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->fightMonstersState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->fightMonstersState.update(*this);
			this->fightMonstersState.render(*this);
			break;

		case GameStateType::ShowCards:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->showCardsState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->showCardsState.update(*this);
			this->showCardsState.render(*this);
			break;

		case GameStateType::SplashScreen:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->splashScreenState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->splashScreenState.update(*this);
			this->splashScreenState.render(*this);
			break;

		case GameStateType::TitleScreen: 

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->titleScreenState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->titleScreenState.update(*this);
			this->titleScreenState.render(*this);
			break;

		case GameStateType::Winner:
		case GameStateType::PlayerDead:

			if (currentState != savedCurrentState) {
				this->context.gameState = this->currentState;
				this->gameOverState.activate(*this);
				this->savedCurrentState = this->currentState;
			}
			this->gameOverState.update(*this);
			this->gameOverState.render(*this);
			break;

		default: break;	

	}

	arduboy.display();
	
}