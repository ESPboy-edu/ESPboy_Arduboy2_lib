#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

enum FlashSettings : uint8_t {
	None = 0,
	FlashXP = (1 << 0),
	FlashHP = (1 << 1),
	FlashArmour = (1 << 2),
	FlashGold = (1 << 3),
	FlashFood = (1 << 4),
};

constexpr inline FlashSettings operator |(FlashSettings left, FlashSettings right) {
	return static_cast<FlashSettings>(static_cast<uint8_t>(left) | static_cast<uint8_t>(right));
}

constexpr inline FlashSettings operator &(FlashSettings left, FlashSettings right) {
	return static_cast<FlashSettings>(static_cast<uint8_t>(left) & static_cast<uint8_t>(right));
}

constexpr inline FlashSettings operator ^(FlashSettings left, FlashSettings right) {
	return static_cast<FlashSettings>(static_cast<uint8_t>(left) ^ static_cast<uint8_t>(right));
}

inline FlashSettings & operator |=(FlashSettings & left, FlashSettings right) {
	left = (left | right);
	return left;
}

inline FlashSettings & operator &=(FlashSettings & left, FlashSettings right) {
	left = (left & right);
	return left;
}

inline FlashSettings & operator ^=(FlashSettings & left, FlashSettings right) {
	left = (left ^ right);
	return left;
}

class BaseState : public GameState<GameContext, GameStateType> {

  protected:

    Font3x5 font3x5 = Font3x5();

	private:

		uint8_t leftFlame = 0;
		uint8_t rightFlame = 1;

  public:

    void renderSpinningCard(StateMachine & machine, int8_t x, int8_t y, uint8_t i, uint8_t ySpacing = 13);
    void renderLargeSpinningCard(StateMachine & machine, int8_t x, int8_t y, uint8_t i);
    void renderPlayerDead(uint8_t y = 25);
    void renderMonsterDead(StateMachine & machine);
    void renderPlayerStatistics(StateMachine & machine, bool overallFlash, FlashSettings settings);
    void renderBackground(StateMachine & machine, bool renderCorners);
    void drawItem(uint8_t position, uint8_t const *imageName);
    void renderMessageBox(StateMachine & machine, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void renderTitleBackground(StateMachine & machine, bool drawLowerLines);

};