#include "game_bb.h"

#define HERO_LEVEL 45
#define HERO_OFFSET 4
#define HERO_HEIGHT 10
#define BONUS_OFFSET 5
#define BRIDGE_LEVEL 54
#define PLATFORM_LEVEL 55
#define PLATFORM_MIN_DISTANCE 5
#define PLATFORM_MIN_WIDTH 5
#define PLATFORM_MAX_WIDTH 15
#define SCROLL_STEP 2
#define SCROLL_MULTIPLIER 3
#define BONUS_LIMIT 50
#define BONUS_CLOSE 1
#define BONUS_FAT 2

#define platform_end(platform) (platform.x + platform.width - 1)

#define HILLS 9

const uint8_t bg_hills[][2] = {{0, 35}, {72, 20}, {96, 30}, {110, 26},
	{127, 35}, {199, 20}, {223, 30}, {237, 26}, {254, 35}};

struct platform_t {
	int8_t x;
	int8_t width;
	// TODO: There is only one bridge shown at a time. We can get rid of this
	int8_t bridge;
};

struct data_t {
	uint8_t gameOn;
	uint32_t score;
	uint32_t hiScore;
	uint8_t bonus;
	struct platform_t platformCurr;
	struct platform_t platformNext;
	int8_t bg;
	uint8_t bonus_next;
	uint8_t bonus_type;
};

static Arduboy2 *gr;

const uint8_t spriteMap[][5] PROGMEM = {
	{0b00110, 0b01111, 0b11110, 0b01111, 0b00110},
	{0b01110, 0b10001, 0b10101, 0b10001, 0b01110},
	{0b11000, 0b11000, 0b11101, 0b11000, 0b11000}
};


static struct data_t *data;

static uint8_t bridge;
static uint8_t bonus_promote;

static void platform_random(struct platform_t *platform) {
	uint8_t width = (bonus_promote & BONUS_FAT) ? 15 : random(PLATFORM_MIN_WIDTH, PLATFORM_MAX_WIDTH);
	platform->x = platform_end(data->platformNext) - platform_end(data->platformCurr) +
		((bonus_promote & BONUS_CLOSE) ? PLATFORM_MIN_DISTANCE :
		random(PLATFORM_MIN_DISTANCE, BRIDGE_LEVEL - width));

	platform->width = width;
	bonus_promote = 0;
}

static void display_platform(struct platform_t platform, uint8_t color) {
	gr->fillRect(platform.x, PLATFORM_LEVEL, platform.width, 9, color);
	gr->drawPixel(platform.x + platform.width / 2, PLATFORM_LEVEL+2, BLACK);
	if(platform.bridge && platform.x > 0) {
		gr->drawFastHLine(0,
		BRIDGE_LEVEL,
		platform.x + platform.bridge, color);
	}
}


static void display_hero(int8_t state, int8_t x, int8_t y, uint8_t color) {
	int8_t foot = y + 8;
	if(state & 4) {
		y += HERO_HEIGHT;
		foot = y;
		gr->drawPixel(x, foot, color);
		gr->drawPixel(x + 2, foot, color);
	}
	gr->drawBitmap(x, y + 2, gameBBLogo, 3, 7, color);
	if(state & 1) {
		gr->drawPixel(x + 2, foot, BLACK);
		gr->drawPixel(x + 3, foot, color);
	}
	if(state & 2) {
		gr->drawPixel(x, foot, BLACK);
		gr->drawPixel(x - 1, foot, color);
	}
}

static void display_background(void) {
	gr->clear();

	// The hills
	data->bg = data->bg % 128;
	for(int i = 0;++i < HILLS;) {
		gr->drawLine(bg_hills[i - 1][0] - data->bg, bg_hills[i - 1][1],
		bg_hills[i][0] - data->bg, bg_hills[i][1]);
	}

	// The score
	gr->drawBitmap(64+29, 1, cupBmp, 8, 8, WHITE);
	gr->fillRect(65, 2, 28, 5, BLACK);
	drawNumber(gr, 65, 2, data->score, WHITE, 7);
	gr->fillRect(102, 2, 26, 5, BLACK);
	drawNumber(gr, 102, 2, data->hiScore, WHITE, 0);

	for(int i = 0;i < ceil((float)data->bonus / BONUS_OFFSET);i++) {
		gr->drawBitmap(1 + i * (BONUS_OFFSET + 1), 1, spriteMap[0], (i + 1) * BONUS_OFFSET > data->bonus ? data->bonus % BONUS_OFFSET : BONUS_OFFSET, BONUS_OFFSET, WHITE);
	}

	if(data->bonus_next > 0) {
		gr->drawBitmap(data->bonus_next, PLATFORM_LEVEL + 2,
			spriteMap[data->bonus_type], BONUS_OFFSET, BONUS_OFFSET, WHITE);
	}

	if(bridge) {
		gr->drawFastHLine(platform_end(data->platformCurr),
			BRIDGE_LEVEL, bridge, WHITE);
	}

	display_platform(data->platformCurr, WHITE);
	display_platform(data->platformNext, WHITE);
}

static void platform_new(void) {
	struct platform_t platformFuture;
	int8_t distance;

	platform_random(&platformFuture);
	platformFuture.bridge = 0;
	data->bonus_type = 0;
	data->bonus_next = 0;

	distance = floor(platform_end(data->platformCurr) / SCROLL_STEP) * SCROLL_STEP;
	platformFuture.x += distance * SCROLL_MULTIPLIER;
	while(distance >= SCROLL_STEP) {
		if(! gr->nextFrame()) continue;
		data->bg++;
		data->platformCurr.x -= SCROLL_STEP;
		data->platformNext.x -= SCROLL_STEP;
		platformFuture.x -= SCROLL_STEP * SCROLL_MULTIPLIER;
		distance -= SCROLL_STEP;
		display_background();
		display_platform(platformFuture, WHITE);
		display_hero(0, platform_end(data->platformNext) - HERO_OFFSET, HERO_LEVEL, WHITE);
		gr->display();
	}


	data->platformNext.bridge =
		platform_end(data->platformCurr) + bridge - data->platformNext.x;

	data->platformNext.x -= platform_end(data->platformCurr);
	data->platformCurr = data->platformNext;
	data->platformNext = platformFuture;
	data->platformNext.bridge = 0;
	bridge = 0;
	distance = data->platformNext.x - platform_end(data->platformCurr);
	if(distance > BONUS_OFFSET + 3 && random(2) == 0) {
		data->bonus_next = platform_end(data->platformCurr) + 2 + random(distance - BONUS_OFFSET - 2);
		data->bonus_type = random(3);
	}
}

static void hero_fall(int8_t x, int8_t y) {
	while(y < HEIGHT) {
		if (!gr->nextFrame()) {
			continue;
		}
		display_background();
		display_hero(4 | (y % 3), x, y, WHITE);
		gr->display();
		gr->idle();
		y++;
	}
}

static void hero_score(int8_t up) {
	data->score += up;
	if(data->score > data->hiScore) {
		data->hiScore = data->score;
	}
}


static int8_t hero_walk(int8_t from, int8_t to) {
	const int8_t bridge_end = platform_end(data->platformCurr) + bridge;
	const int8_t bridge_over = bridge_end - 1 > platform_end(data->platformNext);
	int8_t hero_state = 0;
	int8_t claim_bonus = 0;
	int8_t i = from;

	if(! bridge_over) {
		to = min(to, platform_end(data->platformNext) - HERO_OFFSET);
	}
	while(i < to) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();
		if(buttonPressed(gr) && i + HERO_OFFSET + HERO_OFFSET <= data->platformNext.x) {
			hero_state = hero_state ? 0 : 4;
		}
		display_background();
		display_hero((i % 3) | hero_state, i, HERO_LEVEL, WHITE);
		gr->display();

		// Game over?
		// TODO: simplify this condition
		if((i + HERO_OFFSET >= bridge_end && (data->platformNext.x >= bridge_end || bridge_over)) ||
			(hero_state && i + HERO_OFFSET + HERO_OFFSET > data->platformNext.x)) {
			data->gameOn = 0;
			hero_fall(i + HERO_OFFSET + (hero_state ? -1 : 1),
				HERO_LEVEL - (hero_state ? 0 : HERO_HEIGHT));
			return 0;
		}

		// Pick bonus
		if(hero_state && i + HERO_OFFSET >= data->bonus_next && i <= data->bonus_next + BONUS_OFFSET) {
			bonus_promote = data->bonus_type;
			data->bonus_next = 0;
			data->bonus_type = 0;
			claim_bonus = 1;
		}
		gr->idle();
		i++;
	}
	return claim_bonus;
}

static void rise_bridge(int8_t bridge_x) {
	bridge = 0;
	while(! buttonReleased(gr)) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();
		bridge += bridge < BRIDGE_LEVEL;
		gr->drawFastVLine(bridge_x,
			BRIDGE_LEVEL - bridge + 1, bridge, WHITE);
		gr->display();
		gr->idle();
	}
	gr->drawFastVLine(bridge_x,
		BRIDGE_LEVEL - bridge + 1, bridge, BLACK);
}

static void place_bridge(int8_t bridge_x) {
	uint8_t bridge_short = (data->platformNext.x >= platform_end(data->platformCurr) + bridge);
	for(float d = 0;d < PI/(bridge_short ? 1 : 2);) {
		int8_t x, y;
		if (!gr->nextFrame()) {
			continue;
		}
		y = BRIDGE_LEVEL + sin(d + PI + PI / 2) * bridge;
		x = bridge_x + cos(d + PI + PI / 2) * bridge;
		gr->drawLine(bridge_x, BRIDGE_LEVEL, x, y, WHITE);
		gr->display();
		gr->drawLine(bridge_x, BRIDGE_LEVEL, x, y, BLACK);
		d += PI / 15;
		gr->idle();
	}
	bridge = bridge_short ? 0 : bridge;
}

static void bonus_up(int8_t claim_bonus) {
	if(! claim_bonus) return;
	int8_t y = HERO_LEVEL - 2;
	int8_t hero_pos = platform_end(data->platformNext) - HERO_OFFSET;
	while(! buttonPressed(gr) && y > 0) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();

		display_background();
		gr->drawFastHLine(hero_pos - 4, y + 2, 3);
		gr->drawFastVLine(hero_pos - 3, y + 1, 3);
		drawNumber(gr, hero_pos, y, claim_bonus, WHITE, 0);
		display_hero(0, hero_pos, HERO_LEVEL, WHITE);
		y -= (HERO_LEVEL - y) / 2;
		gr->display();
		gr->idle();
	}
}

static void build_bridge(void) {
	int8_t claim_bonus;
	int8_t bridge_x = platform_end(data->platformCurr);

	rise_bridge(bridge_x);
	place_bridge(bridge_x);

	// Walk over the bridge
	claim_bonus = hero_walk(bridge_x - HERO_OFFSET,
		max(platform_end(data->platformNext) - HERO_OFFSET,
		bridge_x + bridge));

	if(!claim_bonus) {
		data->bonus_next = 0;
		data->bonus_type = 0;
	}

	if(data->gameOn) {
		claim_bonus += bridge_x + bridge - 1 == data->platformNext.x + data->platformNext.width / 2;
		hero_score(claim_bonus + 1);
		bonus_up(claim_bonus);
		claim_bonus -= !!bonus_promote;
		if(claim_bonus) {
			data->bonus = min(data->bonus + claim_bonus, BONUS_LIMIT);
		}
	}
}

static void game_new(void) {
	data->gameOn = 1;
	data->score = 0;
	data->bonus = 0;
	data->bonus_next = 0;
	data->bonus_type = 0;
	data->platformCurr.x = -19;
	data->platformCurr.width = 36;
	data->platformCurr.bridge = 0;
	data->platformNext.x = 32;
	data->platformNext.width = 0;
	data->platformNext.bridge = 0;
	platform_random(&data->platformNext);
}

static void game_on(void) {
	bridge = 0;
	while(data->gameOn && ! gr->justPressed(B_BUTTON)) {
		if (!gr->nextFrame()) {
			continue;
		}
		gr->pollButtons();
		display_background();

		display_hero(0, platform_end(data->platformCurr) - HERO_OFFSET,
			HERO_LEVEL, WHITE);
		if(buttonPressed(gr) && ! gr->justPressed(B_BUTTON)) {
			build_bridge();
			if(data->gameOn) {
				// Place new platform
				platform_new();
			} else if(data->bonus >= BONUS_OFFSET) {
				data->bonus -= BONUS_OFFSET;
				data->gameOn = 1;
				bridge = 0;
			}
		}
		gr->display();
		gr->idle();
	}
}


void gameBB(Arduboy2 *sgr, uint8_t *gdat, uint8_t menu, uint8_t *gameOn, uint32_t *score, uint32_t *hiScore) {
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
