#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

struct MonsterStats {
  uint8_t hp;
  uint8_t dmg;
};

char const itemUsed_caption_01[] PROGMEM = " You~used~an~ice~spell.\n  ~Monster~is~frozen\n     for~one~turn.";
char const itemUsed_caption_02[] PROGMEM = " You~used~a~fire~spell.\n   ~Monster~loses~10\n      HP~points.";
char const itemUsed_caption_03[] PROGMEM = "~You~used~a~poison~spell.\n  ~Deal~2~extra~damage\n       per~turn.";
char const itemUsed_caption_04[] PROGMEM = "You~used~a~healing~spell.\n   ~You~have~gained~8\n    ~extra~HP~points.";

char const * const itemUsed_captions[] = {
	itemUsed_caption_01,
	itemUsed_caption_02,
	itemUsed_caption_03,
	itemUsed_caption_04,
};

char const bossDice_caption_01[] PROGMEM = " Gain~a~Fire~Wand!";
char const bossDice_caption_02[] PROGMEM = " Gain~an~Ice~Wand!";
char const bossDice_caption_03[] PROGMEM = "Gain~a~Poison~Wand!";
char const bossDice_caption_04[] PROGMEM = "Gain~a~Healing~Wand!";
char const bossDice_caption_05[] PROGMEM = "  Gain~an~Armour!";
char const bossDice_caption_06[] PROGMEM = "Upgrade~your~Weapon!";
char const bossDice_caption_07[] PROGMEM = "Gain 2 Gold Pieces!";

char const * const bossDice_captions[] = {
	bossDice_caption_01,
	bossDice_caption_02,
	bossDice_caption_03,
	bossDice_caption_04,
	bossDice_caption_05,
	bossDice_caption_06,
	bossDice_caption_07,
}; 


char const monster_caption_01[] PROGMEM = "Undead\nSoldier";
char const monster_caption_02[] PROGMEM = "Skeleton";
char const monster_caption_03[] PROGMEM = "Undead\nKnight";
char const monster_caption_04[] PROGMEM = "Serpent\nKnight";
char const monster_caption_05[] PROGMEM = "Ogs\nSanctum\nGuard";
char const monster_caption_06[] PROGMEM = "Undead\nGiant";
char const monster_caption_07[] PROGMEM = "Skeleton\nLord";
char const monster_caption_08[] PROGMEM = "Undead\nLord";
char const monster_caption_09[] PROGMEM = "Serpent\nDemon";
char const monster_caption_10[] PROGMEM = "Ogs\nRemains";

char const * const monster_captions[] = {
	monster_caption_01,
	monster_caption_02,
	monster_caption_03,
	monster_caption_04,
	monster_caption_05,
	monster_caption_06,
	monster_caption_07,
	monster_caption_08,
	monster_caption_09,
	monster_caption_10,
}; 

const uint8_t monster_captions_offsets[] = { 1, 2, 1, 1, 0, 1, 1, 1, 1, 1 };

enum class ViewState : uint8_t {
  HighlightMonsterStats,
  HighlightPlayerStats,
  RollDice,
  DiceSelection,
  WandSelection,
  Defend,
  MonsterDead,
  MonsterDead_Wait,
  PlayerDead,
  ItemIceUsed,
  ItemFireUsed,
  ItemPoisonUsed,
  ItemHealingUsed
};

enum class SelectedElement : int8_t {
  None = -1,
  ItemFire,
  ItemIce,
  ItemPoison,
  ItemHealing,
  Dice1,
  Dice2,
  Dice3,
  Dice4,
  Action,
};

class FightMonstersState : public BaseState {

  private:

    ViewState viewState = ViewState::RollDice;
    ViewState nextState = ViewState::RollDice;
    ViewState lastState = ViewState::RollDice;

    MonsterStats monsterStats;

    SelectedElement selectedElement = SelectedElement::None;

    uint8_t dice[4];
    uint8_t dice_Sixes[4];
		uint8_t diceMonster = 0;
    uint8_t counter = 0;
    uint8_t ice = 0;
    uint8_t monsterPositionInc = 0;
    uint8_t monsterPosition = 0;

    bool dice_Retain[4];
    bool poison = false;


  private:

    SelectedElement prevDiceSelection(SelectedElement index);
    SelectedElement nextDiceSelection(SelectedElement index);
    SelectedElement prevWandSelection(StateMachine & machine, SelectedElement index);
    SelectedElement nextWandSelection(StateMachine & machine, SelectedElement index);

    uint8_t getMonsterDMG(StateMachine & machine);
    void setDiceSelection(StateMachine & machine, bool value);
    void monsterIsDead(StateMachine & machine);
    void drawMonsterHead(StateMachine & machine, uint8_t const *imageHead, uint8_t const *maskHead);

  public:	

  	void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;
    
};