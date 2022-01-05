#include "game_tetris.h"

#define LOCK_TIME 15
#define TET_HIT_BOTTOM 1
#define TET_STOP_DROP 2
#define TET_MOVED 4

#define NEXT_TET_X 4
#define NEXT_TET_Y 0
#define BOARD_LINES 24

#define NUM_LEVELS 24

enum tet_option_t: uint8_t {
	TET_OPT_ROT_DIR = 1,
	TET_OPT_DROP_UP = 2,
	TET_OPT_DROP_DN = 4,
	TET_OPT_HIDE_GHOST = 8
};

static uint8_t level_speed[NUM_LEVELS] = {53, 49, 45, 41, 37, 33, 28, 22, 17, 11, 10, 9, 8, 7, 6, 6, 5, 5, 4, 4, 3};

static Arduboy2 *gr;

// Game state data
struct data_t {
	uint8_t gameOn;
	uint32_t score;
	uint32_t hiScore;
	uint16_t screen[BOARD_LINES];
	uint8_t level;
	uint8_t lines;
	uint8_t bag[7];
	uint8_t bag_pos;
	uint8_t tet_now_sel;
	uint8_t tet_now_rot;
	uint8_t tet_next_sel;
	uint8_t tet_next_rot;
	int8_t tx;
	uint8_t ty;
	uint8_t tet_option;
};

static struct data_t *data;

static unsigned long button_wait_time;

const uint8_t spriteMap[][7] PROGMEM = {
	// RotR
	{0b00001100, 0b00010010, 0b00100001, 0b00100001, 0b00001010, 0b00001100, 0b00001110},
	// RotL
	{0b00001110, 0b00001100, 0b00001010, 0b00100001, 0b00100001, 0b00010010, 0b00001100},
	// ->  Dn
	{0b00001000, 0b00010000, 0b00100000, 0b01111111, 0b00100000, 0b00010000, 0b00001000},
	// ->| Dn
	{0b01000100, 0b01001000, 0b01010000, 0b01111111, 0b01010000, 0b01001000, 0b01000100},
	// Ghost
	{0b00000000, 0b01111100, 0b00111010, 0b01111111, 0b00111010, 0b01111100, 0b00000000}
};

static const uint16_t tetrominoes[7][4] = {
	// L
	{17504, 3712, 50240, 11776},
	// J
	{17600, 36352, 25664, 3616},
	// T
	{3648, 19520, 19968, 17984},
	// Z
	{3168, 9792, 3168, 9792},
	// S
	{1728, 17952, 1728, 17952},
	// I
	{17476, 3840, 17476, 3840},
	// O
	{26112, 26112, 26112, 26112}
};

static uint8_t get_pixel(uint8_t x, uint8_t y) {
	return bitRead(data->screen[y], x);
} // end of get_pixel

static void set_pixel(uint8_t x, uint8_t y, uint8_t color) {
	bitWrite(data->screen[y], x, color);
} // end of set_pixel

////////////////////////////////////////////////////
static void shuffle(uint8_t a[7]) {
	uint8_t j, x, i;
	for (i = 7; i; i--) {
		j = floor(random(i));
		x = a[i - 1];
		a[i - 1] = a[j];
		a[j] = x;
	}
}

static uint8_t draw_tetromino(int8_t x, int8_t y, uint16_t item, uint8_t color) {
	for (uint8_t i = 0; i < 16; i++) {
		int8_t py = 1 + (i / 4) + y;
		int8_t px = 1 + (i % 4) + x;
		if (py < 0) continue;
		if (bitRead(item, i)) {
			if (color == 2) {
				if (get_pixel(px, py)) {
					return 0;
				}
			} else {
				set_pixel(px, py, color);
			}
		}
	}
	return 1;
}

static uint8_t fit_tetromino(int8_t x, int8_t y, uint16_t item) {
	return draw_tetromino(x, y, item, 2);
}

static void remove_lines() {
	uint8_t lines_removed = 0;
	const uint16_t lines_points[5] = {0, 40, 100, 300, 1200};
	for (uint8_t i = 2; i < 23; i++) {
		if (data->screen[i] != 0b111111111111) continue;
		lines_removed++;
		for (uint8_t j = i; j > 0; j--) {
			data->screen[j] = data->screen[j - 1];
		}
	}
	data->score += lines_points[lines_removed] * (data->level + 1);
	if (data->score > data->hiScore) {
		data->hiScore = data->score;
	}
	data->lines += lines_removed;
	data->level = data->lines / 10;
}

static void reset_board() {
	for (uint8_t i = 0; i < BOARD_LINES; i++) {
		data->screen[i] = 0b100000000001;
	}
	data->screen[BOARD_LINES - 1] = ~0;
}

static void next_tetromino() {
	data->bag_pos = (data->bag_pos + 1) % 7;
	if (!data->bag_pos) {
		shuffle(data->bag);
	}

	data->tet_now_sel = data->tet_next_sel;
	data->tet_now_rot = data->tet_next_rot;
	data->tet_next_sel = data->bag[data->bag_pos];
	data->tet_next_rot = random(4);
}

uint8_t find_tet_bottom(void) {
	uint8_t g = data->ty;
	while(g < 20 && fit_tetromino(data->tx, g + 1, tetrominoes[data->tet_now_sel][data->tet_now_rot])) g++;
	return g;
}

static void display_bubble(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) {
	gr->drawRoundRect(x, y, w, h, r + 2);
	gr->fillRoundRect(x + 2, y + 2, w - 4, h - 4, r);
}

static void display_text_bubble(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const __FlashStringHelper *label) {
	display_bubble(x, y, w, h, 1);
	gr->setCursor(x + 5, y + 3);
	gr->print(label);
}

static void display_background() {
	gr->clear();

	//Score rounded rect -(SCORE)-
	display_bubble(62, 0, 47, 13, 1);
	gr->drawFastHLine(43, 7, 19);
	gr->drawFastHLine(109, 7, 19);
	gr->setCursor(71, 3);
	gr->print(F("SCORE"));

	// The score square
	gr->drawFastHLine(43, 15, 85);
	gr->fillRect(44, 17, 84, 9);
	gr->drawFastHLine(43, 27, 85);

	display_text_bubble(66, 34, 62, 13, F("LEVEL"));
	display_text_bubble(66, 50, 62, 13, F("LINES"));

	// Next rounded rect
	display_bubble(46, 40, 17, 17, 0);

	// Vertical lines arround the board
	gr->drawFastVLine(0, 0, 63);
	gr->drawFastVLine(1, 0, 63);
	gr->drawFastVLine(41, 0, 63);
	gr->drawFastVLine(42, 0, 63);

	// Bottom wall
	gr->drawPixel(36, 60);
	gr->drawPixel(37, 62);

	for (uint8_t x = 1; x <= 10; x++) {
		uint8_t x3 = x * 3 + 2;
		// Bottom wall
		gr->drawPixel(x3 + 0, 60);
		gr->drawPixel(x3 + 1, 60);
		gr->drawPixel(x3 + 3, 60);
		gr->drawPixel(x3 + 0, 62);
		gr->drawPixel(x3 + 2, 62);
		gr->drawPixel(x3 + 3, 62);
	}

	for (uint8_t i = 0; i < 16; i++) {
		uint8_t v = i * 4;

		// Left wall
		gr->drawPixel(2, v);
		gr->drawPixel(3, v);
		gr->drawPixel(5, v);

		// Rigth wall
		gr->drawPixel(37, v);
		gr->drawPixel(38, v);
		gr->drawPixel(40, v);

		v += 2;
		// Left wall
		gr->drawPixel(2, v);
		gr->drawPixel(4, v);
		gr->drawPixel(5, v);

		// Rigth wall
		gr->drawPixel(37, v);
		gr->drawPixel(39, v);
		gr->drawPixel(40, v);
	}
}

static void display_board() {
	for (uint8_t x = 1; x <= 10; x++) {
		uint8_t x3 = x * 3 + 4;
		for (uint8_t y = 0; y < 20; y++) {
			uint8_t y3 = y * 3;
			gr->drawFastHLine(x3, y3 + 2, 2, BLACK);
			gr->drawFastVLine(x3 - 1, y3, 2, BLACK);
			if (get_pixel(x, y + BOARD_LINES - 21)) {
				gr->drawFastHLine(x3, y3, 2, WHITE);
				gr->drawFastHLine(x3, y3 + 1, 2, WHITE);
			} else {
				gr->drawFastHLine(x3, y3, 2, BLACK);
				gr->drawFastHLine(x3, y3 + 1, 2, BLACK);
			}
		}
	}
}

static void display_now_next() {
	// Find ghost piece y position
	uint8_t g = find_tet_bottom();

	// Next rect
	gr->fillRect(48, 42, 13, 13);

	for (uint8_t i = 0; i < 16; i++) {
		uint8_t tw;
		uint8_t bx = i % 4;
		uint8_t by = i / 4;
		uint8_t dx = 3 * bx + 7;
		uint8_t dy = 3 * by;
		uint8_t dx3tx = dx + 3 * data->tx;

		// Ghost
		if(!(data->tet_option & TET_OPT_HIDE_GHOST) && g > data->ty && bitRead(tetrominoes[data->tet_now_sel][data->tet_now_rot], i)) {
			gr->drawPixel(dx3tx, dy + 3 * g - 6);
		}

		// Now
		if(bitRead(tetrominoes[data->tet_now_sel][data->tet_now_rot], i)) {
			tw = 2;
			if(bitRead(tetrominoes[data->tet_now_sel][data->tet_now_rot] >> by * 4, bx + 1)) {
				tw = 3;
			}
			gr->drawFastHLine(dx3tx, dy + 3 * data->ty - 5, tw);
			gr->drawFastHLine(dx3tx, dy + 3 * data->ty - 6, tw);
			if(bitRead(tetrominoes[data->tet_now_sel][data->tet_now_rot] >> (by + 1) * 4, bx)) {
				gr->drawFastHLine(dx3tx, dy + 3 * data->ty - 4, 2);
			}
		}

		// Next
		if(bitRead(tetrominoes[data->tet_next_sel][data->tet_next_rot], i)) {
			tw = 2;
			if(bitRead(tetrominoes[data->tet_next_sel][data->tet_next_rot] >> by * 4, bx + 1)) {
				tw = 3;
			}
			gr->drawFastHLine(dx + 42, dy + 43, tw, BLACK);
			gr->drawFastHLine(dx + 42, dy + 44, tw, BLACK);
			if(bitRead(tetrominoes[data->tet_next_sel][data->tet_next_rot] >> (by + 1) * 4, bx)) {
				gr->drawFastHLine(dx + 42, dy + 45, 2, BLACK);
			}
		}
	}
}

static void print_pad(uint8_t v) {
	if(v < 100) gr->print(" ");
	if(v < 10) gr->print(" ");
	gr->print(v);
}

static void display_stats() {
	gr->setCursor(71, 18);
	gr->print(data->score);

	gr->setCursor(106, 37);
	print_pad(data->level);

	gr->setCursor(106, 53);
	print_pad(data->lines);
}

static void game_settings() {
	while(!(gr->justPressed(LEFT_BUTTON) || gr->justPressed(RIGHT_BUTTON))) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();
		gr->clear();
		gr->drawBitmap(28, 28, dPadBmp, 7, 7, WHITE);
		gr->drawBitmap(78, 20, aBmp, 7, 7, WHITE);
		gr->drawBitmap(78, 36, bBmp, 7, 7, WHITE);

		gr->drawBitmap(28, 12, spriteMap[(data->tet_option & TET_OPT_DROP_UP) ? data->tet_option & TET_OPT_ROT_DIR : 3], 7, 7, WHITE);

		gr->drawBitmap(28, 44, spriteMap[2 + !!(data->tet_option & TET_OPT_DROP_DN)], 7, 7, WHITE);

		gr->drawBitmap(92, 20, spriteMap[data->tet_option & TET_OPT_ROT_DIR], 7, 7, WHITE);

		if(!(data->tet_option & TET_OPT_HIDE_GHOST)) {
			gr->drawBitmap(92, 36, spriteMap[4], 7, 7, WHITE);
		}

		if(gr->justPressed(A_BUTTON)) {
			data->tet_option ^= TET_OPT_ROT_DIR;
			continue;
		}
		if (gr->justPressed(B_BUTTON)) {
			data->tet_option ^= TET_OPT_HIDE_GHOST;
			continue;
		}
		if(gr->justPressed(UP_BUTTON)) {
			data->tet_option ^= TET_OPT_DROP_UP;
			continue;
		}
		if(gr->justPressed(DOWN_BUTTON)) {
			data->tet_option ^= TET_OPT_DROP_DN;
			continue;
		}
		gr->display();
		gr->idle();
	}
	while(!gr->nextFrame()) gr->idle();
}

static void game_on() {
	uint16_t lockAfterXFrames = gr->frameCount + LOCK_TIME;
	uint8_t tetState = 0;
	display_background();
	display_stats();

	for (;;) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();

		if (gr->justPressed(B_BUTTON)) {
			return;
		}

		tetState &= ~TET_MOVED;

		// Vertical Movement
		if((!(data->tet_option & TET_OPT_DROP_UP) && gr->justPressed(UP_BUTTON)) ||
			(!!(data->tet_option & TET_OPT_DROP_DN) && gr->justPressed(DOWN_BUTTON))) {
			data->ty = find_tet_bottom();
			tetState |= TET_HIT_BOTTOM;
		} else if((gr->pressed(DOWN_BUTTON) && !(tetState & TET_STOP_DROP)) || gr->justPressed(DOWN_BUTTON)) {
			tetState &= ~TET_STOP_DROP;
			int8_t dy = data->ty + gr->pressed(DOWN_BUTTON);
			if(fit_tetromino(data->tx, dy, tetrominoes[data->tet_now_sel][data->tet_now_rot])) {
				data->ty = dy;
			} else {
				tetState |= TET_HIT_BOTTOM;
			}
		}

		// Horizontal Movement
		if((gr->pressed(LEFT_BUTTON) || gr->pressed(RIGHT_BUTTON)) && (millis() - button_wait_time > 250)) {
			if(gr->justPressed(LEFT_BUTTON) || gr->justPressed(RIGHT_BUTTON)) {
				button_wait_time = millis();
			}
			int8_t dx = data->tx + gr->pressed(RIGHT_BUTTON) - gr->pressed(LEFT_BUTTON);
			if (fit_tetromino(dx, data->ty, tetrominoes[data->tet_now_sel][data->tet_now_rot])) {
				tetState &= ~TET_HIT_BOTTOM;
				data->tx = dx;
				lockAfterXFrames = gr->frameCount + LOCK_TIME;
			}
		}

		// Rotation
		if(gr->justPressed(A_BUTTON) || (!!(data->tet_option & TET_OPT_DROP_UP) && gr->justPressed(UP_BUTTON))) {
			int8_t dr = (data->tet_now_rot + 5 - 2 * (!!(data->tet_option & TET_OPT_ROT_DIR))) % 4;
			tetState |= TET_MOVED;
			if ( ! fit_tetromino(data->tx, data->ty, tetrominoes[data->tet_now_sel][dr])) {
				if(fit_tetromino(data->tx + 1, data->ty, tetrominoes[data->tet_now_sel][dr])) {
					data->tx = data->tx + 1;
				} else if(fit_tetromino(data->tx - 1, data->ty, tetrominoes[data->tet_now_sel][dr])) {
					data->tx = data->tx - 1;
				} else if(fit_tetromino(data->tx, data->ty - 1, tetrominoes[data->tet_now_sel][dr])) {
					data->ty = data->ty - 1;
				} else {
					dr = data->tet_now_rot;
				}
			}
			if(dr != data->tet_now_rot) {
				tetState &= ~TET_HIT_BOTTOM;
				data->tet_now_rot = dr;
				lockAfterXFrames = gr->frameCount + LOCK_TIME;
			}
		}

		if (! (tetState & TET_HIT_BOTTOM) && gr->everyXFrames((level_speed[data->level < NUM_LEVELS ? data->level : NUM_LEVELS - 1]) / 2)) {
			if (fit_tetromino(data->tx, data->ty + 1, tetrominoes[data->tet_now_sel][data->tet_now_rot])) {
					data->ty++;
			} else {
					tetState |= TET_HIT_BOTTOM;
					lockAfterXFrames = gr->frameCount + LOCK_TIME;
			}
		}

		if ((tetState & TET_HIT_BOTTOM) && gr->frameCount >= lockAfterXFrames) {
			draw_tetromino(data->tx, data->ty, tetrominoes[data->tet_now_sel][data->tet_now_rot], 1);
			remove_lines();
			display_stats();

			// Game over?
			if (data->ty <= 1) {
				data->gameOn = 0;
				break;
			}
			data->tx = 3;
			data->ty = 0;
			tetState |= TET_STOP_DROP;
			tetState &= ~TET_HIT_BOTTOM;
			next_tetromino();
			lockAfterXFrames = gr->frameCount + LOCK_TIME;
		}

		display_board();
		display_now_next();

		gr->display();
		gr->idle();
	} // for(;;)

	// END screen
	// reset_board();
	// draw_tetromino(3, 3, 0b0010011000101111, 1);
	// draw_tetromino(3, 7, 0b1001100100001111, 1);
	// draw_tetromino(3, 11, 0b0000100111011011, 1);
	// draw_tetromino(3, 15, 0b1010101010100111, 1);
	// draw_tetromino(3, 19, 0b0000000000000111, 1);
	// display_board();
	// gr->display();
	// while(! gr->justPressed(A_BUTTON)) {
	//   if (gr->nextFrame()) {
	//     gr->pollButtons();
	//   }
	//   gr->idle();
	// }
}

static void game_new(void) {
	data->score = 0;
	data->lines = 0;
	data->level = 0;
	data->tx = 3;
	data->ty = 0;

	data->tet_next_sel = random(7);
	data->tet_next_rot = random(4);

	data->bag_pos = 6;
	for (int i = 0; i < 7; i++) {
		data->bag[i] = i;
	}
	reset_board();
	next_tetromino();
	data->gameOn = 1;
}

void gameTetris(Arduboy2 *sgr, uint8_t *gdat, uint8_t menu, uint8_t *gameOn, uint32_t *score, uint32_t *hiScore) {
	gr = sgr;
	data = (struct data_t *)gdat;

	if (menu == MENU_NEW) {
		game_new();
	}

	if (menu != MENU_EXIT) {
		if(gr->pressed(RIGHT_BUTTON)) {
			game_settings();
		}

		gr->setTextBackground(WHITE);
		gr->setTextColor(BLACK);

		game_on();

		gr->setTextBackground(BLACK);
		gr->setTextColor(WHITE);
	}

	*gameOn = data->gameOn;
	*score = data->score;
	*hiScore = data->hiScore;
}

