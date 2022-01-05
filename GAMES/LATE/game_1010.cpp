#include "game_1010.h"

// Game related
#define BOARD_LINES 10
#define MIN_TAIL_LENGTH 4

#define NEXT_X_OFFSET 64
#define NEXT_Y_OFFSET 40
#define NEXT_SIDE 22
#define NEXT_BUSY 128

#define SIDE_L 1
#define SIDE_R 2
#define SIDE_U 4
#define SIDE_D 8

#define FILL_EMPTY 0
#define FILL_CHECKERD 1
#define FILL_SOLID 2

static Arduboy2 *gr;

struct point_t {
	int8_t x;
	int8_t y;
};

struct data_t {
	uint8_t gameOn;
	uint32_t score;
	uint32_t hiScore;
	uint16_t screen[BOARD_LINES];
	int8_t x;
	int8_t y;
	int8_t current;
	uint8_t next[3];
};

static struct data_t *data;

static unsigned long button_wait_time = 0;;

#define NUM_TILES 19
static uint8_t tiles[NUM_TILES][5] = {
	// Test case all set
	// {
	// 	0b11111,
	// 	0b11111,
	// 	0b11111,
	// 	0b11111,
	// 	0b11111
	// },
	// SQ1x1
	{
		0b00000,
		0b00000,
		0b00100,
		0b00000,
		0b00000
	},
	// SQ2x2
	{
		0b00000,
		0b00000,
		0b01100,
		0b01100,
		0b00000
	},
	// SQ3x3
	{
		0b00000,
		0b01110,
		0b01110,
		0b01110,
		0b00000
	},
	// D2H
	{
		0b00000,
		0b00000,
		0b01100,
		0b00000,
		0b00000
	},
	// D2V
	{
		0b00000,
		0b00000,
		0b01000,
		0b01000,
		0b00000
	},
	// D3H
	{
		0b00000,
		0b00000,
		0b01110,
		0b00000,
		0b00000
	},
	// D3V
	{
		0b00000,
		0b01000,
		0b01000,
		0b01000,
		0b00000
	},
	// D4H
	{
		0b00000,
		0b00000,
		0b01111,
		0b00000,
		0b00000
	},
	// D4V
	{
		0b00000,
		0b01000,
		0b01000,
		0b01000,
		0b01000
	},
	// D5H
	{
		0b00000,
		0b00000,
		0b11111,
		0b00000,
		0b00000
	},
	// D5V
	{
		0b01000,
		0b01000,
		0b01000,
		0b01000,
		0b01000
	},
	// L2R1
	{
		0b00000,
		0b00000,
		0b01100,
		0b01000,
		0b00000
	},
	// L2R2
	{
		0b00000,
		0b00000,
		0b00110,
		0b00010,
		0b00000
	},
	// L2R3
	{
		0b00000,
		0b00000,
		0b00010,
		0b00110,
		0b00000
	},
	// L2R4
	{
		0b00000,
		0b00000,
		0b01000,
		0b01100,
		0b00000
	},
	// L3R1
	{
		0b00000,
		0b01110,
		0b01000,
		0b01000,
		0b00000
	},
	// L3R2
	{
		0b00000,
		0b01110,
		0b00010,
		0b00010,
		0b00000
	},
	// L3R3
	{
		0b00000,
		0b00010,
		0b00010,
		0b01110,
		0b00000
	},
	// L3R3
	{
		0b00000,
		0b01000,
		0b01000,
		0b01110,
		0b00000
	}
};

static uint8_t get_pixel(int8_t ox, int8_t oy, int8_t x, int8_t y, uint8_t color, uint8_t mask) {
	return bitRead(data->screen[oy + y], ox + x) == color;
} // end of get_pixel

static uint8_t set_pixel(int8_t ox, int8_t oy, int8_t x, int8_t y, uint8_t color, uint8_t mask) {
	data->score++;
	bitWrite(data->screen[oy + y], ox + x, color);
	return 0;
} // end of set_pixel


static uint8_t set_tile(int8_t x, int8_t y, uint8_t item[5], uint8_t color, uint8_t (*draw)(int8_t ox, int8_t oy, int8_t x, int8_t y, uint8_t color, uint8_t mask)) {
	for (uint8_t i = 0; i < 25; i++) {
		int8_t py = i / 5;
		int8_t px = i % 5;
		uint8_t mask = 0;
		if(py > 0 && bitRead(item[py - 1], px)) mask |= SIDE_U;
		if(py < 5 && bitRead(item[py + 1], px)) mask |= SIDE_D;
		if(px > 0 && bitRead(item[py], px - 1)) mask |= SIDE_L;
		if(px < 5 && bitRead(item[py], px + 1)) mask |= SIDE_R;
		if (bitRead(item[py], px)) {
			if (draw(x, y, px, py, color, mask)) {
				return 0;
			}
		}
	}
	return 1;
}

static void display_square(uint8_t x, uint8_t y, uint8_t w, uint8_t mask, uint8_t color, uint8_t fill) {
	uint8_t rmask = ~mask;

	// Top
	if(rmask & SIDE_U) {
		gr->drawFastHLine(x, y, w, color);
	}
	// Left
	if(rmask & SIDE_L) {
		gr->drawFastVLine(x, y, w, color);
	}
	// Bottom
	if(rmask & SIDE_D) {
		gr->drawFastHLine(x, y + w - 1, w, color);
	}
	// Right
	if(rmask & SIDE_R) {
		gr->drawFastVLine(x + w - 1, y, w, color);
	}

	// Top
	if(mask & SIDE_U) {
		gr->drawPixel(x, y, color);
		gr->drawPixel(x + w - 1, y, color);

		gr->drawPixel(x, y - 1, color);
		gr->drawPixel(x + w - 1, y - 1, color);
	}

	// Left
	if(mask & SIDE_L) {
		gr->drawPixel(x, y, color);
		gr->drawPixel(x, y + w - 1, color);

		gr->drawPixel(x - 1, y, color);
		gr->drawPixel(x - 1, y + w - 1, color);
	}

	// Bottom
	if(mask & SIDE_D) {
		gr->drawPixel(x, y + w - 1, color);
		gr->drawPixel(x + w - 1, y + w - 1, color);
		gr->drawPixel(x, y + w, color);
		gr->drawPixel(x + w - 1, y + w, color);
	}

	// Right
	if(mask & SIDE_R) {
		gr->drawPixel(x + w - 1, y, color);
		gr->drawPixel(x + w - 1, y + w - 1, color);

		gr->drawPixel(x + w, y, color);
		gr->drawPixel(x + w, y + w - 1, color);
	}

	if(!fill || w < 3) return;
	if(fill == FILL_SOLID) {
		gr->fillRect(x, y, w, w, color);
		return;
	}
	for (int fx = x; fx < x + w; fx++) {
		for (int fy = y; fy < y + w; fy++) {
			if((fx + !!(fy %2)) % 2) {
				gr->drawPixel(fx, fy, color);
			}
		}
	}
}

static void display_background(void) {
	gr->drawRect(0, 0, 63, 63, WHITE);
	gr->drawBitmap(64+29, 1, cupBmp, 8, 8, WHITE);
}

static void display_board(void) {
	if(data->hiScore < data->score) {
		data->hiScore = data->score;
	}
	gr->fillRect(65, 2, 28, 5, BLACK);
	drawNumber(gr, 65, 2, data->score, WHITE, 7);
	gr->fillRect(102, 2, 26, 5, BLACK);
	drawNumber(gr, 102, 2, data->hiScore, WHITE, 0);

	for (int x = 0; x < BOARD_LINES; x++) {
		int sx = x * 6 + 2;
		for (int y = 0; y < BOARD_LINES; y++) {
			int sy = y * 6 + 2;
			if (get_pixel(0, 0, x, y, 1, 0)) {
				display_square(sx, sy, 5, 0, WHITE, FILL_EMPTY);
			} else {
				display_square(sx, sy, 5, 0, BLACK, FILL_EMPTY);
			}
		}
	}
}

static void remove_lines(void) {
	uint8_t x, y;
	uint8_t lines = 0;
	uint8_t vlines[5];
	uint8_t vline = 0;
	uint8_t vcount;
	for (x = 0; x < BOARD_LINES; x++) {
		vcount = 0;
		for (y = 0; y < BOARD_LINES; y++) {
			if (get_pixel(0, 0, x, y, WHITE, 0)) {
				vcount++;
			}
		}
		if(vcount == BOARD_LINES) {
			vlines[vline] = x;
			vline++;
			lines++;
		}
	}
	for (y = 0; y < BOARD_LINES; y++) {
		if (data->screen[y] == 0b1111111111) {
			data->screen[y] = 0;
			data->score += 10;
			lines++;
		}
	}
	for (x = 0; x < vline; x++) {
		for (y = 0; y < BOARD_LINES; y++) {
			set_pixel(0, 0, vlines[x], y, BLACK, 0);
		}
	}

	// Combo reward
	if(lines > 1) {
		data->score += 10 * (lines - 1);
	}
	display_board();
}

uint8_t display_next_square(int8_t ox, int8_t oy, int8_t x, int8_t y, uint8_t color, uint8_t mask) {
	display_square(ox + 4 * x, oy + 4 * y, 3, 0, color, FILL_SOLID);
	return 0;
}

uint8_t check_current_square(int8_t ox, int8_t oy, int8_t x, int8_t y, uint8_t color, uint8_t mask) {
	uint8_t rx = ox + 6 * x + 2;
	uint8_t ry = oy + 6 * y + 2;
	return rx > 60 || ry > 60 || rx < 0 || ry < 0;
}

uint8_t display_current_square(int8_t ox, int8_t oy, int8_t x, int8_t y, uint8_t color, uint8_t mask) {
	uint8_t rx = ox + 6 * x + 2;
	uint8_t ry = oy + 6 * y + 2;

	if(rx > 60 || ry > 60) return 0;
	display_square(rx, ry, 5, mask, color, get_pixel(ox / 6, oy / 6, x, y, WHITE, 0) ? FILL_CHECKERD : FILL_SOLID);
	return 0;
}

static void display_next(void) {
	gr->fillRect(NEXT_X_OFFSET, NEXT_Y_OFFSET, 3 * NEXT_SIDE, NEXT_SIDE + 1, BLACK);
	for (int i = 0; i < 3; i++) {
		if(data->next[i] >= 0 && data->next[i] < NUM_TILES) {
			set_tile(NEXT_SIDE * i + NEXT_X_OFFSET, NEXT_Y_OFFSET + 2, tiles[data->next[i]], 1, display_next_square);
		}
	}
}

static void next_random(void) {
	data->next[0] = random(NUM_TILES);
	data->next[1] = random(NUM_TILES);
	data->next[2] = random(NUM_TILES);
	data->current = -1;
}

static uint8_t pick_tile(void) {
	int8_t direction;
	uint8_t cursor = 1;
	while(data->next[cursor] & NEXT_BUSY) cursor = (cursor + 1) % 3;

	for (;;) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();

		if (gr->justPressed(B_BUTTON)) {
			return 1;
		}

		if (gr->justPressed(A_BUTTON)) {
			data->current = cursor;
			data->next[cursor] |= NEXT_BUSY;
			break;
		}

		direction = gr->justPressed(RIGHT_BUTTON) - gr->justPressed(LEFT_BUTTON);

		do {
			cursor = (cursor + 3 + direction) % 3;
		} while(data->next[cursor] & NEXT_BUSY);
		display_next();

		gr->drawFastHLine(NEXT_SIDE * cursor + NEXT_X_OFFSET, NEXT_Y_OFFSET, NEXT_SIDE - 3, WHITE);
		gr->drawFastHLine(NEXT_SIDE * cursor + NEXT_X_OFFSET, NEXT_Y_OFFSET + NEXT_SIDE, NEXT_SIDE - 3, WHITE);

		gr->display();
		gr->idle();
	}
	return 0;
}

static uint8_t next_fit(void) {
	int8_t t, x, y, ox, oy;
	uint8_t pass = 0;
	for(t = 0;t < 3;t++) {
		// Skip already placed tile
		if(data->next[t] & NEXT_BUSY) {
			continue;
		}

		// Locate offset Left
		for(x = 0;x > -5;x--) {
			if( ! set_tile(x * 6, 0, tiles[data->next[t]], WHITE, check_current_square)) {
				ox = x + 1;
				break;
			}
		}

		// Locate offset top
		for(y = 0;y > -5;y--) {
			if( ! set_tile(0, y * 6, tiles[data->next[t]], WHITE, check_current_square)) {
				oy = y + 1;
				break;
			}
		}
		for(x = 0;x < BOARD_LINES;x++) {
			for(y = 0;y < BOARD_LINES;y++) {
				if( ! set_tile((x + ox) * 6, (y + oy) * 6, tiles[data->next[t]], WHITE, check_current_square)) {
					continue;
				}
				if(set_tile(x + ox, y + oy, tiles[data->next[t]], WHITE, get_pixel)) {
					x = y = BOARD_LINES;
					pass++;
				}
			}
		}
	}
	return pass != 0;
}

static void place_tile(void) {
	int8_t dx, dy;
	if(data->current < 0 || data->current > 2) {
		return;
	}

	data->x = 18;
	data->y = 18;

	display_next();
	display_board();
	set_tile(data->x, data->y, tiles[data->next[data->current] & ~NEXT_BUSY], WHITE, display_current_square);
	gr->display();

	for (;;) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();
		set_tile(data->x, data->y, tiles[data->next[data->current] & ~NEXT_BUSY], BLACK, display_current_square);
		// display_board();

		if (gr->justPressed(B_BUTTON)) {
			data->next[data->current] &= ~NEXT_BUSY;
			data->current = -1;
			return;
		}

		if (gr->justPressed(A_BUTTON)) {
			if( ! set_tile(data->x / 6, data->y / 6, tiles[data->next[data->current] & ~NEXT_BUSY], WHITE, get_pixel)) {
				continue;
			}
			set_tile(data->x / 6, data->y / 6, tiles[data->next[data->current] & ~NEXT_BUSY], WHITE, set_pixel);
			data->current = -1;
			display_board();
			return;
		}

		if((gr->pressed(LEFT_BUTTON) ||
			gr->pressed(RIGHT_BUTTON) ||
			gr->pressed(UP_BUTTON) ||
			gr->pressed(DOWN_BUTTON)
			) && (millis() - button_wait_time > 250)) {
			if(gr->justPressed(LEFT_BUTTON) ||
			gr->justPressed(RIGHT_BUTTON) ||
			gr->justPressed(UP_BUTTON) ||
			gr->justPressed(DOWN_BUTTON)) {
				button_wait_time = millis();
			}
			dx = (gr->pressed(RIGHT_BUTTON) - gr->pressed(LEFT_BUTTON)) * 6;
			dy = (gr->pressed(DOWN_BUTTON) - gr->pressed(UP_BUTTON)) * 6;
			if(set_tile(data->x + dx, data->y + dy, tiles[data->next[data->current] & ~NEXT_BUSY], WHITE, check_current_square)) {
				data->x += dx;
				data->y += dy;
				display_board();
				set_tile(data->x, data->y, tiles[data->next[data->current] & ~NEXT_BUSY], WHITE, display_current_square);
				gr->display();
			}
		}

		gr->idle();
	}
}

static void game_new(void) {
	data->gameOn = 1;
	data->score = 0;
	memset(data->screen, 0, sizeof data->screen);
	next_random();
}

static void game_on(void) {
	gr->clear();
	display_background();
	display_board();
	gr->display();
	for (;;) {

		if(data->next[0] & NEXT_BUSY && data->next[1] & NEXT_BUSY && data->next[2] & NEXT_BUSY) {
			next_random();
		}

		// Game over?
		if( ! next_fit()) {
			data->gameOn = 0;
			return;
		}

		if(pick_tile()) {
			return;
		}

		place_tile();
		remove_lines();
	}
}

void game1010(Arduboy2 *sgr, uint8_t *gdat, uint8_t menu, uint8_t *gameOn, uint32_t *score, uint32_t *hiScore) {
	gr = sgr;
	data = (struct data_t *)gdat;

	if (menu == MENU_NEW) {
		game_new();
	}

	if (menu != MENU_EXIT) {
		game_on();
	}

	*gameOn = data->gameOn;
	*score = data->score;
	*hiScore = data->hiScore;
}
