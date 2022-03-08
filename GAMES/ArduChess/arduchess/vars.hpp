#pragma once

#include <stdint.h>
#include <Arduboy2.h>

#define CH2K_ARDUINO
static uint16_t rand_state0, rand_state1;
static uint8_t u8rand()
{
	const uint16_t temp = (rand_state0 ^ (rand_state0 << 5));
	rand_state0 = rand_state1; 
	rand_state1 = (rand_state1 ^ (rand_state1 >> 1)) ^ (temp ^ (temp >> 3));
	return (uint8_t)rand_state1;
}
#define CH2K_RAND u8rand
#include "ch2k/ch2k.hpp"

// algebraic notation move history
static constexpr uint8_t const MOVEHIST_SIZE = 6;

static constexpr uint8_t const UNDOHIST_SIZE = ch2k::game::REP_MOVES_EXTRA;
struct undohist_data
{
    uint8_t cap; // piece, not index
    uint8_t flags;
    uint8_t half_move;
};

enum
{
    SANFLAG_CAP   = (1 << 3),
    SANFLAG_FILE  = (1 << 4),
    SANFLAG_RANK  = (1 << 5),
    SANFLAG_CHECK = (1 << 6),
    SANFLAG_MATE  = (1 << 7),
};
static constexpr uint8_t const SANHIST_SIZE = UNDOHIST_SIZE + MOVEHIST_SIZE * 2;

static constexpr uint8_t const NUM_SAVE_FILES = 3;

#define SAVE_VALID_TAG 0xC5

struct save_file_grouped_data
{
  uint8_t aihappy[2];
  uint8_t ailevel[2];
  uint8_t aiblunder[2];
  undohist_data undohist[UNDOHIST_SIZE];
  uint8_t sanhist[SANHIST_SIZE];
  uint8_t undohist_num;
} dd;

struct save_file_data
{
  uint8_t valid;
  save_file_grouped_data grouped_data;
  uint8_t data[ch2k::EEPROM_GAME_DATA_SIZE];
};

static constexpr auto& aihappy      = dd.aihappy;
static constexpr auto& ailevel      = dd.ailevel;
static constexpr auto& aiblunder    = dd.aiblunder;
static constexpr auto& undohist     = dd.undohist;
static constexpr auto& sanhist      = dd.sanhist;
static constexpr auto& undohist_num = dd.undohist_num;

//static undohist_data undohist[UNDOHIST_SIZE];
//static uint8_t undohist_num;

//uint8_t sanhist[SANHIST_SIZE]; // bits 0-2 are piece type char

// whether the game was just saved (cleared on each move)
static bool game_saved;

static constexpr int const EEPROM_END = 1024;
static constexpr int const EEPROM_SIZE =
  sizeof(save_file_data) * NUM_SAVE_FILES + // save files
  2                                       + // checksum
  0;
static constexpr int const EEPROM_START = EEPROM_END - EEPROM_SIZE;

//static_assert(EEPROM_START >= 16, "EEPROM size too large");

enum {
  STATE_TITLE_START,
  STATE_TITLE,
  STATE_NEW_GAME_START,
  STATE_NEW_GAME,
  //STATE_HELP_START,
  //STATE_HELP,
  //STATE_OPTIONS_START,
  //STATE_OPTIONS,
  STATE_HUMAN_START,
  STATE_HUMAN,           // select a piece
  STATE_HUMAN_MOVE,      // select destination
  STATE_HUMAN_PROMOTION, // choose promotion type
  STATE_AI_START,
  STATE_AI,
  STATE_ANIM,
  STATE_ANIM_UNDO,
  STATE_ANIM_UNDO2,
  STATE_GAME_PAUSED_START,
  STATE_GAME_PAUSED,
  STATE_SAVE_START, // reused for loading games
  STATE_SAVE,       // reused for loading games
  STATE_SAVE_OVERWRITE,
  STATE_LOAD_START,
  STATE_LOAD,
  STATE_EXIT_CONFIRM,
  STATE_GAME_OVER,
} volatile state;

// half-screen data
static uint8_t buf[64*64/8];

// ch2k engine object
static ch2k::game g;

// whether eeprom has valid save files
static bool checksum_valid;

// multipurpose state data
static uint8_t ta, tb, tc;

// used for animations
static uint8_t nframe;

// board cache
static ch2k::piece b[8][8];
// whether board is rotated
static bool rotated;
// whose turn is it? 0 == white, 1 == black
static uint8_t turn;
static uint8_t game_status;

// cursor position
uint8_t cx, cy;

// cursor rendered position
uint8_t rcx, rcy;

// logic loop time
uint32_t tnow;
static constexpr uint8_t LOOP_TIME_MS = 4;

static constexpr uint8_t DEBOUNCE_NUM = 3;
static constexpr uint8_t REP_INIT_NUM = 80;
static constexpr uint8_t REP_NUM = 40;
static uint8_t button_debounce[8];
static uint8_t button_rep[8];
static uint8_t buttons, buttons_prev;
