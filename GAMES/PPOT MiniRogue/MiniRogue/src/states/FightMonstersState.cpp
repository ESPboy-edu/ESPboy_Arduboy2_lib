#include "FightMonstersState.h"

#include "../images/Images.h"
#include "../fonts/Font3x5.h"
#include "../utils/Utils.h"

#define FlashString FPSTR

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void FightMonstersState::activate(StateMachine & machine) {

	auto & gameStats = machine.getContext().gameStats;

	viewState = ViewState::RollDice;
	nextState = ViewState::RollDice;
	lastState = ViewState::RollDice;

	selectedElement = SelectedElement::None;

	for (uint8_t i = 0; i < 4; i++) {

		dice[i] = 0;
		dice_Sixes[i] = 0;
		dice_Retain[i] = false;

	}

	diceMonster = 0;
	counter = 0;
	ice = 0;
	poison = false;

	setDiceSelection(machine, false);

	switch (machine.getContext().gameState) {

		case GameStateType::Monster:
			this->monsterStats.hp = (gameStats.level + 1) + random(1, 7); 
			this->monsterStats.dmg = gameStats.getMonsterDMG();
			break;

		case GameStateType::BossMonster:
			this->monsterStats.hp = 10 + (gameStats.getAreaId() * 5);
			this->monsterStats.dmg = gameStats.getBossMonsterDMG();
			break;

		case GameStateType::MonsterFromEvent:
			this->monsterStats.hp = (gameStats.level + 1) * 2;
			this->monsterStats.dmg = 5;
			break;

		default: break;

	}

	viewState = ViewState::RollDice;
	nextState = ViewState::RollDice;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void FightMonstersState::update(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

	switch (this->viewState) {

		case ViewState::HighlightMonsterStats:
		case ViewState::HighlightPlayerStats:

			if (justPressed & A_BUTTON) { this->counter = FLASH_COUNTER - 1; }
			
			if (this->counter < FLASH_COUNTER) {

				this->counter++;

				if (counter == FLASH_COUNTER) {

					counter = 0;
					this->viewState = nextState;
					arduboy.resetFrameCount();

					#ifdef USE_LEDS
					arduboy.setRGBled(RED_LED, 0);
					#endif

				}

			}
			break;

		case ViewState::RollDice:

			this->lastState = ViewState::RollDice;

			if (justPressed & A_BUTTON) { 

				counter = sizeof(DiceDelay); 

				for (uint8_t i = 0; i < playerStats.xpTrack; i++) {
					if (!this->dice_Retain[i]) this->dice[i] = random(1, 7);
				}

			}

			if (counter < sizeof(DiceDelay)) {
				
				if (arduboy.everyXFrames(pgm_read_byte(&DiceDelay[counter]))) {

					for (uint8_t i = 0; i < playerStats.xpTrack; i++) {
						if (!this->dice_Retain[i]) this->dice[i] = random(1, 7);
					}

					counter++;
					arduboy.resetFrameCount();

				}

			}
			else {


				// Highlight first available dice option ..

				this->selectedElement = SelectedElement::ItemPoison;
				this->selectedElement = nextDiceSelection(this->selectedElement);

				setDiceSelection(machine, true); 
				this->viewState = ViewState::DiceSelection;

			}
			break;

		case ViewState::DiceSelection:

			if (justPressed & LEFT_BUTTON) 		{ this->selectedElement = prevDiceSelection(this->selectedElement); }
			if (justPressed & RIGHT_BUTTON)		{ this->selectedElement = nextDiceSelection(this->selectedElement); }
			
			if (justPressed & A_BUTTON) {

				switch (this->selectedElement) {

					case SelectedElement::Dice1 ... SelectedElement::Dice4:

						counter = 0;
						this->dice_Retain[static_cast<uint8_t>(this->selectedElement) - 4] = false;
						this->dice_Sixes[static_cast<uint8_t>(this->selectedElement) - 4]++;
						this->viewState = ViewState::RollDice;
						break;

					case SelectedElement::Action:
						{
							uint8_t dmg = getMonsterDMG(machine);

							this->monsterStats.hp = clamp(this->monsterStats.hp - dmg, 0, 30);	
							setDiceSelection(machine, false);

							if (this->monsterStats.hp == 0) { 

								monsterIsDead(machine);

							}
							else {
								
								counter = 0;
								this->lastState = ViewState::DiceSelection;
								this->selectedElement = SelectedElement::Action;

								if (dmg != 0) {

									this->viewState = ViewState::HighlightMonsterStats;

									if (playerStats.itemCount() == 0) {

										if (this->ice == 0) {
											this->nextState = ViewState::Defend;
										}
										else {
											this->nextState = ViewState::RollDice;
										}
										
									}
									else {

										this->nextState = ViewState::WandSelection;

									}

								}
								else {

									if (playerStats.itemCount() == 0) {

										if (this->ice == 0) {
											this->viewState = ViewState::Defend;
										}
										else {
											this->viewState = ViewState::RollDice;
										}

									}
									else {

										this->viewState = ViewState::WandSelection;

									}

								}
								
							}

						}
						break;

					default: break;

				}
			
			}

			break;

		case ViewState::WandSelection:

			this->lastState = ViewState::WandSelection;
	
			if (justPressed & UP_BUTTON || justPressed & LEFT_BUTTON) 			{ this->selectedElement = prevWandSelection(machine, this->selectedElement); }
			if (justPressed & DOWN_BUTTON || justPressed & RIGHT_BUTTON) 		{ this->selectedElement = nextWandSelection(machine, this->selectedElement); }
			
			if (justPressed & A_BUTTON) {

				switch (this->selectedElement) {

					case SelectedElement::ItemFire:

						playerStats.items[static_cast<uint8_t>(Wand::Fire)]--;
						this->monsterStats.hp = clamp(this->monsterStats.hp - 10, 0, 30);	
						setDiceSelection(machine, false);

						if (this->monsterStats.hp == 0) { 

							monsterIsDead(machine);

						}
						else {

							this->lastState = ViewState::WandSelection;
							this->viewState = ViewState::ItemFireUsed;

						}
						break;

					case SelectedElement::ItemIce:
						this->ice = 1;
						this->viewState = ViewState::ItemIceUsed;
						playerStats.items[static_cast<uint8_t>(Wand::Ice)]--;
						break;

					case SelectedElement::ItemPoison:
						this->poison = true;
						this->viewState = ViewState::ItemPoisonUsed;
						playerStats.items[static_cast<uint8_t>(Wand::Poison)]--;
						break;

					case SelectedElement::ItemHealing:
						this->viewState = ViewState::ItemHealingUsed;
						playerStats.items[static_cast<uint8_t>(Wand::Healing)]--;
						playerStats.incHP(8);
						break;

					default: 
						if (this->ice == 0) {
							this->viewState = ViewState::Defend;
						}
						else {
							this->viewState = ViewState::RollDice;
						}
						break;

				}
			
			}

			break;

		case ViewState::Defend:

			playerStats.decHP(clamp(this->monsterStats.dmg - playerStats.armour, 0, 50));

			if (playerStats.hp == 0) {

				this->viewState = ViewState::HighlightPlayerStats;
				this->nextState = ViewState::PlayerDead;

			}
			else {

				setDiceSelection(machine, false);
				this->viewState = ViewState::HighlightPlayerStats;
				this->nextState = ViewState::RollDice;
			}
			
			break;

		case ViewState::MonsterDead:

			if (justPressed & A_BUTTON) { 
				gameStats.monsterDefeated = true;
        machine.changeState(gameStats.incRoom(playerStats)); 
			}
			
			if (this->counter < FLASH_COUNTER) {

				this->counter++;

				if (counter == FLASH_COUNTER) {

					this->lastState = this->viewState;
					this->viewState = ViewState::MonsterDead_Wait;

				}

			}

			break;

		case ViewState::MonsterDead_Wait:

			if (justPressed & A_BUTTON) {

				gameStats.monsterDefeated = true;
        machine.changeState(gameStats.incRoom(playerStats));

				#ifdef USE_LEDS
				arduboy.setRGBled(RED_LED, 0);
				#endif

			}

			break;

		case ViewState::PlayerDead:

			if (justPressed & A_BUTTON) {
				machine.changeState(GameStateType::PlayerDead);

				#ifdef USE_LEDS
				arduboy.setRGBled(RED_LED, 0);
				#endif

			}

			break;

		case ViewState::ItemIceUsed:

			if (justPressed & A_BUTTON) {
				this->viewState = ViewState::RollDice;
			}

			break;
			
		case ViewState::ItemFireUsed:
		case ViewState::ItemPoisonUsed:
		case ViewState::ItemHealingUsed:

			if (justPressed & A_BUTTON) {
				this->viewState = ViewState::Defend;
			}

			break;

	}

}


void FightMonstersState::monsterIsDead(StateMachine & machine ) {
	
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;

	switch (machine.getContext().gameState) {

		case GameStateType::Monster:
			playerStats.incXP(gameStats.getMonsterReward());
			break;

		case GameStateType::BossMonster:
		 	{
				playerStats.incXP(gameStats.getAreaId() + 2);
				playerStats.incGold(gameStats.getAreaId() < 2 ? 2 : 3);
				playerStats.bossesKilled++;

        this->diceMonster = random(1, 7);
        if (playerStats.itemCount() >= 2 && this->diceMonster < 5) this->diceMonster = 7;

				switch (this->diceMonster) {

					case 1 ... 4:   playerStats.items[this->diceMonster - 1]++; break;
					case 5:         playerStats.incArmour(1); break;
					case 6:         playerStats.incXP(2); break;
					case 7:         playerStats.incGold(2); break;

					break;

				}

			}

		  break;

		case GameStateType::MonsterFromEvent:
			playerStats.incXP(2);
			break;

		default: break;

	}

	this->viewState = ViewState::MonsterDead;
	
}


void FightMonstersState::drawMonsterHead(StateMachine & machine, uint8_t const *imageHead, uint8_t const *maskHead) {
	
	auto & ardBitmap = machine.getContext().ardBitmap;

	uint8_t head_inc = (this->monsterPosition == 1 || this->monsterPosition == 2 ? 1 : 0);

	if (maskHead != nullptr) {
		ardBitmap.drawCompressed(62, 24 + head_inc, maskHead, BLACK, ALIGN_NONE, MIRROR_NONE);
		ardBitmap.drawCompressed(62 + 7, 24 + head_inc, maskHead, BLACK, ALIGN_NONE, MIRROR_HORIZONTAL);
	}

	ardBitmap.drawCompressed(52, 0 + head_inc, imageHead, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(52 + 17, 0 + head_inc, imageHead, WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void FightMonstersState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;
	auto & ardBitmap = machine.getContext().ardBitmap;
	bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);

	uint8_t hand_inc = (this->monsterPosition == 2 || this->monsterPosition == 3 ? 1 : 0);


	// Update monster animation ..

	this->monsterPositionInc++; 
	if (this->monsterPositionInc > 3) {

		this->monsterPositionInc = 0;

		this->monsterPosition++; 
		if (this->monsterPosition > 4) this->monsterPosition = 0;

	}


	// Draw background ..

  BaseState::renderBackground(machine, true);
	ardBitmap.drawCompressed(0, 0, Images::Monster_Stats, WHITE, ALIGN_NONE, MIRROR_NONE);

	ardBitmap.drawCompressed(42 + 45, 27, Images::Monster_LowerBody_Mask, BLACK, ALIGN_NONE, MIRROR_HORIZONTAL);
	ardBitmap.drawCompressed(42, 19, Images::Monster_LowerBody, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(42 + 27, 19, Images::Monster_LowerBody, WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);
	ardBitmap.drawCompressed(33, 1 + hand_inc, Images::Monster_Sword, WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);

	{

		switch (machine.getContext().gameState) {

			case GameStateType::BossMonster:

				drawMonsterHead(machine, Images::BossMonster_Head, Images::BossMonster_Head_Mask);
				ardBitmap.drawCompressed(94, 1 + hand_inc, Images::Monster_Sword_Mask, BLACK, ALIGN_NONE, MIRROR_NONE);
				ardBitmap.drawCompressed(94, 1 + hand_inc, Images::Monster_Sword, WHITE, ALIGN_NONE, MIRROR_NONE);				
				
				break;

			default:

				drawMonsterHead(machine, Images::Monster_Head, nullptr);
				ardBitmap.drawCompressed(73, 18 + hand_inc, Images::Monster_Shield_Mask, BLACK, ALIGN_NONE, MIRROR_NONE);
				ardBitmap.drawCompressed(73 + 13, 18 + hand_inc, Images::Monster_Shield_Mask, BLACK, ALIGN_NONE, MIRROR_HORIZONTAL);
				ardBitmap.drawCompressed(73, 18 + hand_inc, Images::Monster_Shield, WHITE, ALIGN_NONE, MIRROR_NONE);

				break;

		}

	}


	// Monster statistics ..
	{

		if (this->viewState == ViewState::HighlightMonsterStats) {

			if (flash) {
				font3x5.setTextColor(BLACK);
				arduboy.fillRect(20, 2, (this->monsterStats.hp < 10 ? 5 : 10), 7, WHITE);
				#ifdef USE_LEDS
				//arduboy.setRGBled(0, 32, 0);
				#endif
			}
			else {

				#ifdef USE_LEDS
				//arduboy.setRGBled(0, 0, 0);
				#endif
			}

		}

		font3x5.setCursor(21, 2);
		font3x5.print(this->monsterStats.hp);
		font3x5.setTextColor(WHITE);
		font3x5.setCursor(21, 10);
		font3x5.print(this->monsterStats.dmg); 

	}


	// Inventory ..

	if (viewState == ViewState::WandSelection || lastState == ViewState::WandSelection) {

		ardBitmap.drawCompressed(0, 19, Images::Monster_Items, WHITE, ALIGN_NONE, MIRROR_NONE);

		for (uint8_t i = 0; i < 4; i++) {
			font3x5.setCursor(21, 23 + (i*10));
			font3x5.print(playerStats.items[i]);
		}
	
	}


	// Dice ..

	if (viewState == ViewState::DiceSelection || viewState == ViewState::RollDice || lastState == ViewState::DiceSelection) {

		uint8_t captionIndex = gameStats.getAreaId() + (machine.getContext().gameState == GameStateType::BossMonster ? 5 : 0);
		font3x5.setCursor(3, 22 + (monster_captions_offsets[captionIndex] * 4));
		font3x5.print(FlashString(monster_captions[captionIndex]));

		for (uint8_t i = 0; i < playerStats.xpTrack; i++) {
			SpritesB::drawOverwrite(3 + (i * 10), 52, Images::Dice, this->dice[i]);
		}
	}


	// Marker ..
	
	if ((this->selectedElement == SelectedElement::Action && flash) || (this->selectedElement != SelectedElement::Action)) {

		uint8_t x = 0;
		uint8_t y = 0;

		if (this->viewState == ViewState::DiceSelection) 			{	x = 3 + (playerStats.xpTrack * 10);	y = 53;	}
		if (this->viewState == ViewState::WandSelection) 			{ x = 26; y = 53;	}

		if (x > 0) SpritesB::drawOverwrite(x, y, Images::Marker, 0);

	}


	// Highlighted item ..
	{

		if (this->viewState == ViewState::DiceSelection || this->viewState == ViewState::WandSelection) {

		  if (!flash) {
			
				uint8_t x = 0;
				uint8_t y = 0;

				switch (this->selectedElement) {

					case SelectedElement::ItemFire ... SelectedElement::ItemHealing:
						x = 3;
						y = 22 + (static_cast<uint8_t>(this->selectedElement) * 10);
						break;

					case SelectedElement::Dice1 ... SelectedElement::Dice4:
						x = 3 + ((static_cast<uint8_t>(this->selectedElement) - 4) * 10);
						y = 52;
						break;

					default: break;
		
				}

				if (x > 0) SpritesB::drawPlusMask(x, y, Images::Dice_Highlight_SelfMasked, 0);

			}

		}

	}


	// Player statistics ..

	const FlashSettings settings = ((this->viewState == ViewState::HighlightPlayerStats) ? FlashSettings::FlashHP : FlashSettings::None);

	#ifdef USE_LEDS
	if (this->viewState == ViewState::HighlightPlayerStats) {
		arduboy.setRGBled(RED_LED, (flash ? 32 : 0));
	}
	#endif
	BaseState::renderPlayerStatistics(machine, true, settings);


	// Messages ..

	switch (this->viewState) {

		case ViewState::HighlightPlayerStats:
	
			if (this->nextState == ViewState::PlayerDead) {
				BaseState::renderPlayerDead();
			}

			break;
			
		case ViewState::MonsterDead:
		case ViewState::MonsterDead_Wait:

			if (this->viewState != ViewState::MonsterDead_Wait) {

				FlashSettings settings = FlashSettings::FlashXP;

				if(machine.getContext().gameState == GameStateType::BossMonster) {

					settings |= FlashSettings::FlashGold;
					if(this->diceMonster == 5) settings |= FlashSettings::FlashArmour;
					
				}

				BaseState::renderPlayerStatistics(machine, true, settings);

			}

			if (machine.getContext().gameState == GameStateType::BossMonster) {

    	 	BaseState::renderMessageBox(machine, 20, 23, 88, 26);
			 	font3x5.setCursor(26, 28);
			 	font3x5.print(F("You~killed~the~Boss.\n"));
			 	font3x5.print(FlashString(bossDice_captions[this->diceMonster - 1]));

			}
			else {
			
				BaseState::renderMonsterDead(machine);

			}
			break;

		case ViewState::PlayerDead:

			BaseState::renderPlayerDead();
			break;

		case ViewState::ItemIceUsed:
		case ViewState::ItemFireUsed:
		case ViewState::ItemPoisonUsed:
		case ViewState::ItemHealingUsed:

   	 	BaseState::renderMessageBox(machine, 15, 17, 100, 32);
			font3x5.setCursor(20, 21);
			font3x5.print(FlashString(itemUsed_captions[ static_cast<uint8_t>(this->viewState) - static_cast<uint8_t>(ViewState::ItemIceUsed) ]));
			break;

		default: break;

	}

}



// ----------------------------------------------------------------------------
//  Get the previous dice selection.  Dices need to be 6 to select ..
//
SelectedElement FightMonstersState::prevDiceSelection(SelectedElement element) {

	if (element > SelectedElement::Dice4 && this->dice[3] == 6) 							{ return SelectedElement::Dice4; }
	if (element > SelectedElement::Dice3 && this->dice[2] == 6) 							{ return SelectedElement::Dice3; }
	if (element > SelectedElement::Dice2 && this->dice[1] == 6) 							{ return SelectedElement::Dice2; }
	if (element > SelectedElement::Dice1 && this->dice[0] == 6) 							{ return SelectedElement::Dice1; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the next dice or action selection.  Dices need to be 6 to select ..
//
SelectedElement FightMonstersState::nextDiceSelection(SelectedElement element) {

	if (element < SelectedElement::Dice1 && this->dice[0] == 6) 							{ return SelectedElement::Dice1; }
	if (element < SelectedElement::Dice2 && this->dice[1] == 6) 							{ return SelectedElement::Dice2; }
	if (element < SelectedElement::Dice3 && this->dice[2] == 6) 							{ return SelectedElement::Dice3; }
	if (element < SelectedElement::Dice4 && this->dice[3] == 6) 							{ return SelectedElement::Dice4; }
	if (element < SelectedElement::Action) 																		{ return SelectedElement::Action; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the previous wand selection ..
//
SelectedElement FightMonstersState::prevWandSelection(StateMachine & machine, SelectedElement element) {

	auto & playerStats = machine.getContext().playerStats;

	if (element > SelectedElement::ItemHealing && playerStats.items[static_cast<uint8_t>(Wand::Healing)] > 0) 	{ return SelectedElement::ItemHealing; }
	if (element > SelectedElement::ItemFire && playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) 				{ return SelectedElement::ItemFire; }
	if (element > SelectedElement::ItemIce && playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) 					{ return SelectedElement::ItemIce; }
	if (element > SelectedElement::ItemPoison && playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) 		{ return SelectedElement::ItemPoison; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the next wand or action selection ..
//
SelectedElement FightMonstersState::nextWandSelection(StateMachine & machine, SelectedElement element) {
	
	auto & playerStats = machine.getContext().playerStats;

	if (element < SelectedElement::ItemHealing && playerStats.items[static_cast<uint8_t>(Wand::Healing)] > 0) 	{ return SelectedElement::ItemHealing; }
	if (element < SelectedElement::ItemFire && playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) 				{ return SelectedElement::ItemFire; }
	if (element < SelectedElement::ItemIce && playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) 					{ return SelectedElement::ItemIce; }
	if (element < SelectedElement::ItemPoison && playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) 		{ return SelectedElement::ItemPoison; }
	if (element < SelectedElement::Action) 																																			{ return SelectedElement::Action; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the damage inflicted on the monster this roll of the dice ..
//
uint8_t FightMonstersState::getMonsterDMG(StateMachine & machine) {

	uint8_t damage = 0;

	for (uint8_t i = 0; i < machine.getContext().playerStats.xpTrack; i++) {
		if (this->dice[i] != 1) {
			damage = damage + this->dice[i] + (this->dice_Sixes[i] * 6);
		}
	}

	return damage + (poison ? 5 : 0);

}


// ----------------------------------------------------------------------------
//  Clear dice selection array ready to roll again ..
//
void  FightMonstersState::setDiceSelection(StateMachine & machine, bool value) {

	for (uint8_t i = 0; i < machine.getContext().playerStats.xpTrack; i++) {

		this->dice_Retain[i] = value;

		if (!value) {
			this->dice_Sixes[i] = 0;
			if (this->ice > 0) this->ice--;
		}

	}

}
