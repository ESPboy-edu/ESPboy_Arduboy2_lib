#include <Arduino.h>
#include <Arduboy2.h>
//#include <EEPROM.h>

#include "common.h"

#include "game_tetris.h"
#include "game_1010.h"
#include "game_bb.h"

#define GAME_ID 112
#define OFFSET_GAME_ID (EEPROM_STORAGE_SPACE_START + 112)
#define OFFSET_GAME_DATA (OFFSET_GAME_ID + sizeof(byte))

#define FH 8
#define GAME_DATA_SZ 240
Arduboy2 arduboy;

struct game_t {
	const __FlashStringHelper *name;
	int address;
	void (*play)(Arduboy2 *sgr, uint8_t *gdat, uint8_t menu, uint8_t *gameOn, uint32_t *score, uint32_t *hiScore);
	const uint8_t *logo;
};

uint8_t game_data[GAME_DATA_SZ];

const uint8_t games_count = 3;
struct game_t games[games_count];

uint8_t choice = 0;

void bootLogo() {
	uint8_t state = 0;
	uint8_t frame = 0;
	int r = 10, x = -r, y = 6, err = 2-2*r;

	arduboy.clear();

	while(!arduboy.buttonsState()) {
		if (!arduboy.nextFrame()) {
			continue;
		}
		switch(state) {
			case 0: // clock
				arduboy.drawCircle(30, 16, 14, WHITE);
				arduboy.drawFastVLine(30, y, 10, WHITE);
				arduboy.drawFastHLine(30, y + 10, 5, WHITE);
				if(frame > 10) {
					frame = 0;
					state++;
					continue;
				}
				frame++;
			break;
			case 1: // Falling L
				arduboy.drawFastVLine(30, y, 10, BLACK);
				arduboy.drawFastHLine(30, y + 10, 5, BLACK);
				y = 6 + frame * frame;
				if(y > 43) {
					arduboy.drawCircle(30, 16, 14, BLACK);
					arduboy.drawFastVLine(30, 43, 10, WHITE);
					arduboy.drawFastHLine(30, 53, 5, WHITE);

					arduboy.setCursor(41, 8);
					arduboy.print(F("CORE1024"));
					arduboy.setCursor(41, 18);
					arduboy.print(F("Presents"));
					state++;
					y = frame = 0;
					break;
				}
				arduboy.drawFastVLine(30, y, 10, WHITE);
				arduboy.drawFastHLine(30, y + 10, 5, WHITE);
				frame++;
			break;
			case 2: // A
				if(frame < 3) {
					arduboy.drawFastVLine(50 + frame, 52 - 3 * frame, 3, WHITE);
					arduboy.drawPixel(50, 54, BLACK);
				} else if(frame < 7) {
					arduboy.drawFastVLine(50 + frame, 34 + 3 * frame, 3, WHITE);
					arduboy.drawPixel(56, 54, BLACK);
				} else {
					state++;
				}
				frame++;
			break;
			case 3: // T
				arduboy.drawLine(74-x, 53-y, 74, 53, BLACK);
				r = err;
				if (r > x) err += ++x*2+1;
				if (r <= y) err += ++y*2+1;
				arduboy.drawLine(74-x, 53-y, 74, 53, WHITE);

				if(x >= 0) {
					arduboy.drawFastHLine(72, 43, 5, WHITE);
					state++;
					frame = 0;
				}
			break;
			case 4: // E
				arduboy.drawPixel(93, 43 + (frame % 10));
				arduboy.drawPixel(93 + (frame % 5), 43);
				if(frame > 4)
				arduboy.drawPixel(93 + (frame % 5), 48);
				if(frame > 9)
				arduboy.drawPixel(93 + (frame % 5), 53);
				frame++;
				if(frame > 15) {
					state++;
					frame = 0;
				}
			break;
			default:
				if(frame > 15) {
					return;
				}
				frame++;
			break;
		} // switch
		arduboy.display();
	} // for
}

void setup() {
	games[0].name = F("Blocks Arcade");
	games[0].address = OFFSET_GAME_DATA;
	games[0].play = &gameTetris;
	games[0].logo = gameTetrisLogo;

	games[1].name = F("Blocks Puzzle");
	games[1].address = OFFSET_GAME_DATA + sizeof(game_data);
	games[1].play = &game1010;
	games[1].logo = game1010Logo;

	games[2].name = F("Bridge Builder");
	games[2].address = OFFSET_GAME_DATA + 2 * sizeof(game_data);
	games[2].play = &gameBB;
	games[2].logo = gameBBLogo;

	if (EEPROM.read(OFFSET_GAME_ID) != GAME_ID)
	{
		EEPROM.put(OFFSET_GAME_ID, (byte)GAME_ID); // game id
		memset(game_data, 0, sizeof(game_data));
		for(int i = 0;i < games_count; i++) {
			EEPROM.put(games[i].address, game_data);
		EEPROM.commit();
		}
	}

	// arduboy.begin();
	arduboy.boot();
	arduboy.flashlight();
	arduboy.systemButtons();
	arduboy.setFrameRate(30);
	bootLogo();
}

void loop() {
	uint32_t score, hiScore, last_score;
	uint8_t menu;
	uint8_t game_on = 0;
	uint8_t i;

	while(arduboy.buttonsState()) {
		arduboy.idle();
	}

	while (!game_on) {
		if (!arduboy.nextFrame()) {
			continue;
		}
		arduboy.pollButtons();
		arduboy.clear();

		arduboy.drawRect(0, 0, WIDTH, HEIGHT);
		arduboy.setCursor(2, 2);
		arduboy.print(F("Choose Game Mode"));
		arduboy.drawFastHLine(0, FH + 2, WIDTH);

		arduboy.setCursor(2, FH * choice + FH + FH - 2);
		arduboy.print(">");

		arduboy.drawBitmap(2, 55, aBmp, 7, 7, WHITE);
		arduboy.setCursor(12, 55);
		arduboy.print(arduboy.pressed(LEFT_BUTTON) ? F("Reset ") : F("Select"));

		arduboy.drawBitmap(66, 55, dPadBmp, 7, 7, WHITE);
		arduboy.setCursor(76, 55);
		arduboy.print(F("Move"));


		for (i = 0; i < games_count; i++) {
			arduboy.drawBitmap(9, FH * i + FH + FH - 2, games[i].logo, 7, 7, WHITE);
			arduboy.setCursor(23, FH * i + FH + FH - 2);
			arduboy.print(games[i].name);
		}
		arduboy.display();

		// Handle buttons
		if(arduboy.justPressed(UP_BUTTON)) {
				choice = (games_count + choice - 1) % games_count;
		}
		if(arduboy.justPressed(DOWN_BUTTON)) {
				choice = (games_count + choice + 1) % games_count;
		}
		if(arduboy.justPressed(A_BUTTON)) {
				game_on = 1;
		}
		arduboy.idle();
	}

	arduboy.initRandomSeed();

	// READ GAME DATA
	EEPROM.get(games[choice].address, game_data);
	if(arduboy.pressed(LEFT_BUTTON)) {
		memset(game_data, 0, sizeof(game_data));
	}

	// RUN THE GAME

	// Call game with MENU_EXIT to obtain scores
	(*games[choice].play)(&arduboy, game_data, MENU_EXIT, &game_on, &score, &hiScore);

	last_score = ~0;

	do {
		arduboy.clear();

		arduboy.drawRect(0, 0, WIDTH, HEIGHT);
		arduboy.setCursor(2, 2);
		arduboy.print(games[choice].name);
		arduboy.drawFastHLine(0, 10, WIDTH);

		arduboy.drawBitmap(60, 12, cupBmp, 8, 8, WHITE);
		drawNumber(&arduboy, 69, 13, hiScore, WHITE, 0);

		// Game over
		if(!game_on && last_score != ~0) {
			drawNumber(&arduboy, 32, 13, score, WHITE, 7);
			arduboy.setCursor(37, 20);
			arduboy.print(F("Game Over"));

			if(last_score < score) {
				arduboy.setCursor(2, 37);
				arduboy.print(F("Congratulations!"));
				arduboy.setCursor(2, 46);
				arduboy.print(F("That's a new record!"));
			} else if(score / last_score * 100 > 75) {
				arduboy.setCursor(20, 28);
				arduboy.print(F("You almost beat"));
				arduboy.setCursor(20, 37);
				arduboy.print("the record.");
				arduboy.setCursor(20, 46);
				arduboy.print("Better luck");
				arduboy.setCursor(20, 55);
				arduboy.print("next time.");
			} else {
				arduboy.setCursor(2, 28);
				arduboy.print(F("That was"));
				arduboy.setCursor(2, 37);
				arduboy.print("unfortunate.");
				arduboy.setCursor(2, 46);
				arduboy.print("Next time you will");
				arduboy.setCursor(2, 55);
				arduboy.print("do better.");
			}
			arduboy.display();
			for (;;) {
				arduboy.pollButtons();
				if(buttonPressed(&arduboy)) {
					break;
				}
				arduboy.idle();
			}
			arduboy.fillRect(2, 20, 124, 42, BLACK);
			drawNumber(&arduboy, 32, 13, score, BLACK, 7);
		}

		arduboy.drawBitmap(61, 32, games[choice].logo, 7, 7, WHITE);

		if (game_on) {
			drawNumber(&arduboy, 32, 13, score, WHITE, 7);
		} else {
			arduboy.drawFastHLine(56, 15, 3);
		}

		arduboy.drawBitmap(2, 55, aBmp, 7, 7, WHITE);
		arduboy.drawBitmap(66, 55, bBmp, 7, 7, WHITE);
		arduboy.setCursor(76, 55);
		arduboy.print(F("Exit"));

		// Handle buttons
		for (;;) {
			if (!arduboy.nextFrame()) {
				continue;
			}
			arduboy.pollButtons();

			arduboy.setCursor(12, 55);
			arduboy.print(game_on && ! arduboy.pressed(LEFT_BUTTON) ? F("Resume") : F("Start "));

			if(arduboy.justPressed(A_BUTTON)) {
				menu = game_on && ! arduboy.pressed(LEFT_BUTTON) ? MENU_RESUME : MENU_NEW; break;
			}

			if(arduboy.justPressed(B_BUTTON)) {
					menu = MENU_EXIT; break;
			}

			arduboy.display();
			arduboy.idle();
		}

		// Start the game
		last_score = hiScore;
		(*games[choice].play)(&arduboy, game_data, menu, &game_on, &score, &hiScore);

		// SAVE GAME DATA
		EEPROM.put(games[choice].address, game_data);
    EEPROM.commit();

	} while (menu != MENU_EXIT);
}
