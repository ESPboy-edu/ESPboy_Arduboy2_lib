#include "encourage.h"

#define MAX_ENCOURAGE 20

FlashStringHelper encouragement;

const char string_01[] PROGMEM = "Stupendous";
const char string_02[] PROGMEM = "Magnificent";
const char string_03[] PROGMEM = "Clever";
const char string_04[] PROGMEM = "Genius";
const char string_05[] PROGMEM = "Smarty Pants";
const char string_06[] PROGMEM = "Outstanding";
const char string_07[] PROGMEM = "Brilliant";
const char string_08[] PROGMEM = "Amazing";
const char string_09[] PROGMEM = "Astounding";
const char string_10[] PROGMEM = "Astonishing";
const char string_11[] PROGMEM = "Extraordinary";
const char string_12[] PROGMEM = "Remarkable";
const char string_13[] PROGMEM = "Staggering";
const char string_14[] PROGMEM = "Splendid";
const char string_15[] PROGMEM = "Spectacular";
const char string_16[] PROGMEM = "Impressive";
const char string_17[] PROGMEM = "Superb";
const char string_18[] PROGMEM = "Marvellous";
const char string_19[] PROGMEM = "Exceptional";
const char string_20[] PROGMEM = "Masterful";

const char* const encouragements[] PROGMEM = {
    string_01, string_02, string_03, string_04, string_05,
    string_06, string_07, string_08, string_09, string_10,
    string_11, string_12, string_13, string_14, string_15,
    string_16, string_17, string_18, string_19, string_20
};

void setRandomEncouragement()
{
    uint8_t r = random(0, MAX_ENCOURAGE);
    encouragement = readFlashStringPointer(&encouragements[r]);
}

FlashStringHelper getRandomEncouragment()
{
    return encouragement;
}