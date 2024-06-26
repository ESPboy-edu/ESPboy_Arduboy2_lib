#include "common.h"

/*  Defines  */

enum STATE_T {
    STATE_INIT = 0,
    STATE_TITLE,
    STATE_SETTINGS,
    STATE_CREDIT,
    STATE_STARTED,
};

/*  Local Functions  */

static void initTitleMenu(bool isFromSettings);
static void onVsCpu(void);
static void on2Players(void);
static void onLevel(void);
static void onBack(void);
static void onResume(void);
static void onSettings(void);
static void onSettingChange(void);
static void onCredit(void);
static void handleAnyButton(void);

static void drawTitleImage(void);
static void drawRecord(void);
static void drawCredit(void);

/*  Local Variables  */

PROGMEM static const uint8_t imgTitle1[336] = { // 56x48
    0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x80, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xA4, 0x40, 0xA8, 0x42, 0x90, 0x28, 0xD0, 0xA0, 0x08, 0x41, 0x20, 0x44, 0xA1, 0x50, 0x04, 0x21,
    0x08, 0x04, 0x51, 0xA0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x40, 0x00, 0x80, 0x80, 0xC0, 0x64, 0x98,
    0xC0, 0xF2, 0xE8, 0xF0, 0xEA, 0xE4, 0xE8, 0xE2, 0xE4, 0xE8, 0xC4, 0xE8, 0xD0, 0xA8, 0x50, 0xA4, 0x32, 0xE5, 0xEA, 0xF1, 0xFC, 0xFD, 0xFE, 0xFE,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xFD, 0xF0, 0x2A, 0xD2, 0x66, 0x84, 0x4E, 0x94, 0x0C, 0x06, 0x66, 0x52,
    0xA2, 0x73, 0xA9, 0x85, 0x14, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3C,
    0x69, 0x87, 0x9F, 0x3F, 0x3F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x3F, 0x3F, 0xBF, 0x1F, 0xCF, 0x4F, 0x27, 0x13, 0x18, 0x05, 0x06,
    0x09, 0x01, 0x03, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x05, 0x02, 0x04, 0x89, 0xA9, 0x13, 0x23, 0x53, 0x27, 0x57, 0xA7, 0x57,
    0xA3, 0x53, 0xAB, 0x11, 0x89, 0x54, 0xA2, 0x55, 0x54, 0xA8, 0x51, 0xD1, 0xA2, 0x45, 0x9A, 0x14, 0x2A, 0x14, 0x8A, 0x64, 0xB3, 0x49, 0xF4, 0xA9,
    0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x08, 0x80, 0xF1,
    0x7F, 0xAB, 0x55, 0x2A, 0x94, 0x4C, 0xA0, 0x51, 0xA0, 0x64, 0xC6, 0x4D, 0x8A, 0x15, 0x2A, 0x35, 0x15, 0x8A, 0x45, 0xA2, 0x52, 0xA9, 0x54, 0xAA,
    0x55, 0xAA, 0x54, 0xA9, 0x52, 0xA5, 0x4A, 0x87, 0x14, 0x38, 0x40, 0xA0, 0x50, 0xA0, 0x40, 0xE0, 0x40, 0xC0, 0x60, 0xC0, 0x64, 0xA0, 0x64, 0xB4,
    0x70, 0xBB, 0x58, 0x2C, 0x16, 0x8B, 0xA5, 0x52, 0xA9, 0x54, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x56,
};

PROGMEM static const uint8_t imgTitle2[264] = { // 87x19
    0x04, 0x04, 0xFC, 0xFC, 0xFC, 0xFC, 0x0C, 0x0C, 0x0C, 0x0C, 0xFC, 0xFC, 0xF8, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0x40, 0x40,
    0xC0, 0xC0, 0x80, 0x00, 0x00, 0x40, 0xC0, 0xC0, 0xC0, 0xC0, 0x40, 0x40, 0x00, 0x00, 0x40, 0x40, 0xC0, 0xC0, 0x40, 0x00, 0x00, 0x80, 0x80, 0xC0,
    0x40, 0x40, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0x00, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x80, 0x80, 0xC0,
    0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x80, 0x86, 0xCF, 0xCF, 0xC6, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0x0C, 0x1C,
    0x7E, 0xFF, 0xF3, 0xE3, 0xC1, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xEF, 0x08, 0x08, 0x08, 0x08, 0x0F, 0x0F, 0x0F, 0x0E, 0x00, 0x00, 0x03, 0x1F,
    0x7F, 0xFE, 0xF8, 0xC0, 0x80, 0xF0, 0x1E, 0x07, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xEF, 0x08, 0x08, 0x08, 0x08, 0x0F, 0x0F, 0x0F, 0x0E, 0x00, 0x00,
    0x01, 0xFF, 0xFF, 0xFF, 0x03, 0x01, 0x01, 0x01, 0x03, 0x03, 0x00, 0x86, 0x9F, 0x1F, 0x3F, 0x3C, 0x38, 0x78, 0xF0, 0xF1, 0xE3, 0xC0, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x04, 0x04, 0x07, 0x07, 0x07, 0x07, 0x04, 0x04, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x06, 0x04, 0x04,
    0x01, 0x03, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06, 0x02, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06, 0x02, 0x03, 0x01, 0x00, 0x04, 0x06, 0x07, 0x07, 0x07, 0x04, 0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x07, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x03, 0x03, 0x00, 0x00, 0x04, 0x06, 0x07, 0x07, 0x07, 0x06, 0x04,
};

PROGMEM static const char creditText[] = "- " APP_TITLE " -\0\0" APP_RELEASED \
        "\0PROGREMMED BY OBONO\0\0THIS PROGRAM IS\0RELEASED UNDER\0THE MIT LICENSE.";

static STATE_T  state = STATE_INIT;
static bool     isSettingsChanged;

/*---------------------------------------------------------------------------*/
/*                              Main Functions                               */
/*---------------------------------------------------------------------------*/

void initTitle(void)
{ 
    arduboy.setRGBled(0, 0, 0);
    pTunes.initChannel(1);
    if (state == STATE_INIT) {
        readRecord();
    }
    initTitleMenu(false);
}

MODE_T updateTitle(void)
{
    MODE_T ret = MODE_TITLE;
    switch (state) {
    case STATE_TITLE:
    case STATE_SETTINGS:
        handleMenu();
        if (state == STATE_STARTED) {
            ret = MODE_GAME;
        }
        break;
    case STATE_CREDIT:
        handleAnyButton();
        break;
    }
    randomSeed(rand() ^ micros()); // Shuffle random
    return ret;
}

void drawTitle(void)
{
    if (isInvalid) {
        arduboy.clear();
        switch (state) {
        case STATE_TITLE:
            drawTitleImage();
            break;
        case STATE_SETTINGS:
            drawRecord();
            break;
        case STATE_CREDIT:
            drawCredit();
            break;
        }
    }
    if (state == STATE_TITLE || state == STATE_SETTINGS) {
        drawMenuItems(isInvalid);
        if (state == STATE_SETTINGS &&
                (arduboy.buttonDown(UP_BUTTON | DOWN_BUTTON) || isSettingsChanged)) {
            int8_t pos = getMenuItemPos();
            for (int i = 0; i < 4; i++) {
                bool on = (record.settings & 1 << i);
                arduboy.printEx(106 - (i == pos) * 4, i * 6 + 12, (on) ? F("ON ") : F("OFF"));
            }
            isSettingsChanged = false;
        }
    }
    isInvalid = false;
}

/*---------------------------------------------------------------------------*/
/*                             Control Functions                             */
/*---------------------------------------------------------------------------*/

static void initTitleMenu(bool isFromSettings)
{
    clearMenuItems();
    if (record.canContinue) {
        addMenuItem(F("RESUME"), onResume);
        setMenuItemPos((isFromSettings) ? 1 : 0);
    } else {
        addMenuItem(F("AS BLACK"), onVsCpu);
        addMenuItem(F("AS WHITE"), onVsCpu);
        addMenuItem(F("2 PLAYERS"), on2Players);
        setMenuItemPos((isFromSettings) ? 3 : record.gameMode);
    }
    addMenuItem(F("SETTINGS"), onSettings);
    addMenuItem(F("CREDIT"), onCredit);
    setMenuCoords(57, 28, 71, 30, false, true);
    if (isFromSettings) isInvalid = true;
    state = STATE_TITLE;
    isInvalid = true;
    dprintln(F("Menu: title"));
}

static void onVsCpu(void)
{
    record.gameMode = (getMenuItemPos() == 0) ? GAME_MODE_BLACK : GAME_MODE_WHITE;
    playSoundClick();

    int maxLevel = 3;
    if (record.isAdvancedLevel) {
        maxLevel = (record.vsCpuWinLose == WIN_LOSE_VALUE_MAX) ? 5 : 4;
    }
    clearMenuItems();
    addMenuItem(F("LEVEL 1"), onLevel);
    addMenuItem(F("LEVEL 2"), onLevel);
    addMenuItem(F("LEVEL 3"), onLevel);
    if (maxLevel >= 4) addMenuItem(F("LEVEL 4"), onLevel);
    if (maxLevel >= 5) addMenuItem(F("LEVEL 5"), onLevel);
    addMenuItem(F("BACK"), onBack);
    setMenuCoords(57, 28, 71, 36, false, false);
    setMenuItemPos(record.cpuLevel - 1);
    dprintln(F("Menu: level"));
}

static void on2Players(void)
{
    record.gameMode = GAME_MODE_2PLAYERS;
    state = STATE_STARTED;
    dprintln(F("Start 2 players game"));
}
static void onLevel(void)
{
    record.cpuLevel = getMenuItemPos() + 1;
    state = STATE_STARTED;
    dprint(F("Start VS CPU: level="));
    dprint(record.cpuLevel);
    dprintln((record.gameMode == GAME_MODE_BLACK) ? F(" Black") : F(" White"));
}

static void onBack(void)
{
    playSoundClick();
    initTitleMenu(state == STATE_SETTINGS);
}

static void onResume(void)
{
    state = STATE_STARTED;
    dprintln(F("Resume last game"));
}

static void onSettings(void)
{
    playSoundClick();
    clearMenuItems();
    addMenuItem(F("THINKING LED"), onSettingChange);
    addMenuItem(F("STONES COUNTER"), onSettingChange);
    addMenuItem(F("SHOW PLACEABLE"), onSettingChange);
    addMenuItem(F("SHOW FIXED"), onSettingChange);
    addMenuItem(F("EXIT"), onBack);
    setMenuCoords(4, 12, 120, 30, false, false);
    setMenuItemPos(0);
    state = STATE_SETTINGS;
    isInvalid = true;
    isSettingsChanged = true;
    dprintln(F("Menu: settings"));
}

static void onSettingChange(void)
{
    playSoundTick();
    record.settings ^= 1 << getMenuItemPos();
    isSettingsChanged = true;
    dprint(F("Setting changed: "));
    dprintln(getMenuItemPos());
}

static void onCredit(void)
{
    playSoundClick();
    state = STATE_CREDIT;
    isInvalid = true;
    dprintln(F("Show credit"));
}

static void handleAnyButton(void)
{
    if (arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
        playSoundClick();
        state = STATE_TITLE;
        isInvalid = true;
    }
}

/*---------------------------------------------------------------------------*/
/*                              Draw Functions                               */
/*---------------------------------------------------------------------------*/

static void drawTitleImage(void)
{
    arduboy.drawBitmap(0, 16, imgTitle1, 56, 48, WHITE);
    arduboy.drawBitmap(20, 0, imgTitle2, 87, 19, WHITE);
}

static void drawRecord(void)
{
    arduboy.printEx(34, 4, F("[SETTINGS]"));
    arduboy.drawFastHLine2(0, 44, 128, WHITE);
    arduboy.printEx(10, 48, F("PLAY COUNT "));
    arduboy.print(record.playCount);
    arduboy.printEx(10, 54, F("PLAY TIME"));
    drawTime(76, 54, record.playFrames);
}

static void drawCredit(void)
{
    const char *p = creditText;
    for (int i = 0; i < 8; i++) {
        uint8_t len = strnlen_P(p, 20);
        arduboy.printEx(64 - len * 3, i * 6 + 8, (const __FlashStringHelper *) p);
        p += len + 1;
    }
}
