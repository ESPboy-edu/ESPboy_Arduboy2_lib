#ifndef SFX_H
#define SFX_H

// 0: music + SFX   1: music only   2: SFX only   3: mute
byte soundMode = 0;

#define SFX_DOOR              0
#define SFX_PICKUP            1
#define SFX_MENU              2
#define SFX_SHOOT             3
#define SFX_LEVELUP           4
#define SFX_BEAMMEUPSCOTTY    5
#define SFX_YOUHURTME         6
#define SFX_SWITCH            7
#define SFX_KILL              8
#define SFX_PIRAMIDE          9
#define SFX_DOOROPEN          10
#define SFX_KICK              11

const uint8_t sfxDoorClosed[] PROGMEM = {
  ATM_VOL(63),
  //ATM_SL_VOL(-8),
  ATM_NOTE_C2,
  ATM_DELAY(16),
  ATM_STOP_CHAN,
};

const uint8_t pickUp[] PROGMEM = {
  ATM_VOL(63),
  ATM_SL_VOL(-8),
  ATM_NOTE_F6,
  ATM_DELAY(5),
  ATM_NOTE_G6,
  ATM_DELAY(11),
  ATM_STOP_CHAN,
};

const uint8_t menuClick[] PROGMEM = {
  ATM_VOL(63),
  ATM_SL_VOL(-8),
  ATM_NOTE_G3,
  ATM_DELAY(7),
  ATM_STOP_CHAN,
};

const uint8_t shootBullet[] PROGMEM = {
  ATM_VOL(63),
  ATM_SL_VOL(-8),
  ATM_GLIS(1),
  ATM_CUT(0),
  ATM_NOTE_C3,
  ATM_DELAY(8),
  ATM_STOP_CHAN,
};


const uint8_t levelUp[] PROGMEM = {
  ATM_VOL(16),
  ATM_SET_TEMPO(50),
  ATM_GLIS(1),
  ATM_ARP(0B01010011,0B00100001),
  ATM_SL_VOL(1),
  ATM_NOTE_C4,
  ATM_DELAY(64),
  ATM_SET_TEMPO(36),
  ATM_STOP_CHAN,
};

const uint8_t beamMeUpScotty[] PROGMEM = {
  ATM_VOL(32),
  ATM_GLIS(0B10000001),
  ATM_ARP(0B01010011,0B00100001),
  ATM_SL_VOL(1),
  ATM_VIB(32,0B10000111),
  ATM_NOTE_C7,
  ATM_DELAY(48),
  ATM_STOP_CHAN,
};

const uint8_t youHurtMe[] PROGMEM = {
  ATM_VOL(63),
  ATM_VIB(8,0B10000011),
  ATM_ARP(0B01010011,0B00100001),
  ATM_NOTE_G3,
  ATM_DELAY(16),
  ATM_GLIS(0B10000001),
  ATM_SL_VOL(-2),
  ATM_DELAY(16),
  ATM_STOP_CHAN,
};

const uint8_t useSwitch[] PROGMEM = {
  ATM_VOL(63),
  ATM_NOTE_F2,
  ATM_ARP(0B11001100,0B00000100),
  ATM_DELAY(14),
  ATM_SL_VOL(-8),
  ATM_DELAY(4),
  ATM_STOP_CHAN,
};

const uint8_t killTheEnemy[] PROGMEM = {
  ATM_VOL(63),
  ATM_VIB(24,0B10000001),
  ATM_NOTE_C4,
  ATM_DELAY(12),
  ATM_SL_VOL(-16),
  ATM_DELAY(6),
  ATM_STOP_CHAN,
};

const uint8_t clearThePiramide[] PROGMEM = {
  ATM_VOL(63),
  ATM_NOTE_E2,
  ATM_DELAY(8),
  ATM_SL_VOL(-16),
  ATM_GLIS(0B00000001),
  ATM_DELAY(4),
  ATM_STOP_CHAN,
};

const uint8_t openTheDoor[] PROGMEM = {   // rising "slide open" blip on the square channel
  ATM_VOL(63),
  ATM_SL_VOL(-4),
  ATM_NOTE_C3,
  ATM_DELAY(2),
  ATM_NOTE_G3,
  ATM_DELAY(2),
  ATM_NOTE_C4,
  ATM_DELAY(2),
  ATM_NOTE_G4,
  ATM_DELAY(8),
  ATM_STOP_CHAN,
};

const uint8_t kickBlock[] PROGMEM = {   // short dull thud: pitch drops while it fades
  ATM_VOL(63),
  ATM_SL_VOL(-8),
  ATM_GLIS(0B10000001),
  ATM_NOTE_G3,
  ATM_DELAY(6),
  ATM_STOP_CHAN,
};

const unsigned char * const PROGMEM soundFX[] =
{
  sfxDoorClosed, pickUp, menuClick, shootBullet, levelUp, beamMeUpScotty, youHurtMe, useSwitch, killTheEnemy, clearThePiramide, openTheDoor, kickBlock
};

// an EMPTY song: it keeps the ATMlib playroutine running before the title song starts, so that the
// ARDUBOY intro tune (arduboy_intro.h, a sound effect) behaves. All four channels play the same silent loop.
const uint8_t silentSong[] PROGMEM = {
  0x01,                         // Number of tracks
  0x00, 0x00,                   // Address of track 0
  0x00, 0x00, 0x00, 0x00,       // entry track of channels 0..3
  ATM_VOL(0),
  ATM_DELAY(64),
  ATM_GOTO(0)
};

#endif
