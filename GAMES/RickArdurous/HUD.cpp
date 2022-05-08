/*
 * This file implement display of the HUD in game
*/

#include "RickArdurous.h"
#include "SpriteData.h"
#include "HUD.h"
#include "Rick.h"
#include "MapDataConstVariables.h"

namespace HUD
{
	static constexpr int LIFE_POS = 3;
	static constexpr int BULLET_POS = 32;
	static constexpr int DYNAMITE_POS = 64;
	static constexpr int STATUETTE_POS = 95;
	static constexpr int GAP_BETWEEN_LOGO_AND_NUMBER = 3;
	
	unsigned char drawNumber(unsigned char posX, unsigned char num);
}

void HUD::Update()
{
	// clear the top area where HUD will be drawn
	arduboy.fillRect(0,0, WIDTH, 6, BLACK);
	
	// draw each sprites from the HUD
	// draw the life count
	arduboy.drawBitmapExtended(LIFE_POS, 0, SpriteData::HUDHeart, SpriteData::HUD_SPRITE_WIDTH, SpriteData::HUD_SPRITE_HEIGHT, WHITE, false);
	arduboy.drawBitmapExtended(LIFE_POS + SpriteData::HUD_SPRITE_WIDTH + GAP_BETWEEN_LOGO_AND_NUMBER, 0, SpriteData::Numbers[Rick::LifeCount], SpriteData::NUMBERS_SPRITE_WIDTH, SpriteData::NUMBERS_SPRITE_HEIGHT, WHITE, false);
	
	// draw the bullet count
	arduboy.drawBitmapExtended(BULLET_POS, 0, SpriteData::HUDBullet, SpriteData::HUD_BULLET_SPRITE_WIDTH, SpriteData::HUD_SPRITE_HEIGHT, WHITE, false);
	arduboy.drawBitmapExtended(BULLET_POS + SpriteData::HUD_BULLET_SPRITE_WIDTH + GAP_BETWEEN_LOGO_AND_NUMBER, 0, SpriteData::Numbers[Rick::BulletCount], SpriteData::NUMBERS_SPRITE_WIDTH, SpriteData::NUMBERS_SPRITE_HEIGHT, WHITE, false);

	// draw the dynamite count
	arduboy.drawBitmapExtended(DYNAMITE_POS, 0, SpriteData::HUDDynamite, SpriteData::HUD_SPRITE_WIDTH, SpriteData::HUD_SPRITE_HEIGHT, WHITE, false);
	arduboy.drawBitmapExtended(DYNAMITE_POS + SpriteData::HUD_SPRITE_WIDTH + GAP_BETWEEN_LOGO_AND_NUMBER, 0, SpriteData::Numbers[Rick::DynamiteCount], SpriteData::NUMBERS_SPRITE_WIDTH, SpriteData::NUMBERS_SPRITE_HEIGHT, WHITE, false);
	
	// draw the statuette count
	arduboy.drawBitmapExtended(STATUETTE_POS, 0, SpriteData::HUDStatuette, SpriteData::HUD_SPRITE_WIDTH, SpriteData::HUD_SPRITE_HEIGHT, WHITE, false);
	unsigned char posX = drawNumber(STATUETTE_POS + SpriteData::HUD_SPRITE_WIDTH + GAP_BETWEEN_LOGO_AND_NUMBER, Rick::StatuetteCount);
	arduboy.drawBitmapExtended(posX + 1, 0, SpriteData::HUDSlash, SpriteData::HUD_SLASH_SPRITE_WIDTH, SpriteData::NUMBERS_SPRITE_HEIGHT, WHITE, false);
	drawNumber(posX + SpriteData::HUD_SLASH_SPRITE_WIDTH + 3, MapManager::MAX_STATUETTE_COUNT);
}

unsigned char HUD::drawNumber(unsigned char posX, unsigned char num)
{
	// count how many character we will need to draw the number, and init the x position
	unsigned char digitCount = /*(num >= 100) ? 3 : */ ((num >= 10) ? 2 : 1);
	unsigned char lastPos = posX + (digitCount * (SpriteData::NUMBERS_SPRITE_WIDTH + 1));
	unsigned char x = lastPos;
	// draw all the needed chars
	for (unsigned char i = 0; i < digitCount; ++i)
	{
		// shift the position
		x -= SpriteData::NUMBERS_SPRITE_WIDTH + 1;
		// draw the modulo
		arduboy.drawBitmapExtended(x, 0, SpriteData::Numbers[num % 10], SpriteData::NUMBERS_SPRITE_WIDTH, SpriteData::NUMBERS_SPRITE_HEIGHT, WHITE, false);
		// divide the num to draw the next number
		num = num / 10;
	}
	return lastPos;
}