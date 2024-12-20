#include "common.h"

/*  Defines  */

enum {
    STATE_MENU = 0,
    STATE_RECORD,
    STATE_CREDIT,
};

enum {
    MENU_START = 0,
    MENU_SOUND,
    MENU_RECORD,
    MENU_CREDIT,
};

enum {
    RECORD_NOT_READ = 0,
    RECORD_INITIAL,
    RECORD_STORED,
};

#define EEPROM_ADDR_BASE    832
#define EEPROM_SIGNATURE    0x034E424FUL // "OBN\x03"

/*  Typedefs  */

typedef struct
{
    const uint8_t *pImg;
    uint8_t width;
} TITLE_LETTER;

/*  Local Functions  */

static bool handleButtons(void);
static void setSound(bool enabled);
static void playSound1(void);
static void playSound2(void);
static void readRecord(void);

static void drawTitleMenu(void);
static void drawTitleRecord(void);
static void drawTitleCredit(void);

static void     eepSeek(int addr);
static uint8_t  eepRead8(void);
static uint16_t eepRead16(void);
static uint32_t eepRead32(void);
static void     eepReadBlock(uint8_t *p, uint8_t n);
static void     eepWrite8(uint8_t val);
static void     eepWrite16(uint16_t val);
static void     eepWrite32(uint32_t val);
static void     eepWriteBlock(uint8_t *p, uint8_t n);

/*  Local Variables  */

PROGMEM static const uint8_t imgTitle1[192] = { // 96x16
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xC0, 0x40,
    0xA0, 0x60, 0xA0, 0x50, 0xB0, 0x50, 0xB0, 0x50, 0xA0, 0x60, 0xA0, 0x40, 0xC0, 0x00, 0x80, 0x80,
    0x80, 0xE0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE0,
    0xF8, 0xFE, 0xFE, 0xFE, 0xFC, 0xF0, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xE0, 0xE0, 0xC0, 0xC0, 0x80,
    0x00, 0x02, 0x86, 0xFC, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x20, 0x20, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xC0, 0xB0, 0x58, 0xAC, 0x56, 0xAA, 0x55, 0xAB, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0x0A, 0xE5, 0xFA, 0xFD, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x7F, 0x07, 0x01, 0x80, 0x80, 0x42, 0x42, 0x42, 0x41, 0x21, 0x21, 0x21, 0x11, 0x11, 0x10,
    0x08, 0x08, 0x08, 0x88, 0x84, 0x84, 0x44, 0x42, 0x42, 0x22, 0x22, 0x21, 0x11, 0x10, 0x00, 0x00
};

PROGMEM static const uint8_t imgTitle2[224] = { // 112x16
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0,
    0x40, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0x2E, 0xEE, 0xEE, 0xEE, 0xDE, 0xDE,
    0xDC, 0xDD, 0xBC, 0xB9, 0xBA, 0x79, 0x72, 0xE5, 0xCA, 0x95, 0xAA, 0x55, 0xAA, 0x55, 0x2A, 0x10,
    0x8F, 0xBF, 0xFF, 0xD7, 0x57, 0x7F, 0xFF, 0xE0, 0xE0, 0x3F, 0x3F, 0x8F, 0x8F, 0xBF, 0x3F, 0x3F,
    0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xDF, 0x57, 0x57, 0x77, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xDF, 0x07, 0x03, 0x01, 0x00, 0x20, 0x20, 0x10, 0x10, 0x10, 0x08,
    0x08, 0x08, 0x04, 0x04, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0xC3, 0xC5, 0x89, 0x12, 0x32, 0x64, 0x04,
    0x09, 0x0E, 0x08, 0x00, 0x00, 0xE0, 0xFC, 0x7F, 0x0F, 0x3F, 0xF8, 0xFF, 0xFB, 0xF3, 0xFF, 0x7F,
    0xB3, 0xDB, 0xDD, 0xFC, 0xFE, 0x7F, 0xBF, 0xCE, 0xF5, 0xFB, 0xFD, 0xFE, 0xFE, 0x7F, 0x3F, 0x1F,
    0x0F, 0x0F, 0x07, 0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x33, 0x7B, 0x7F, 0x7F, 0x7F, 0x3F, 0x1F, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

PROGMEM static const uint8_t imgTitle3[224] = { // 56x32
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x0A, 0x0A,
    0x8A, 0x8A, 0xCA, 0xCA, 0xB3, 0x91, 0x91, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xEE, 0xEF, 0x53, 0x49,
    0xCD, 0xDE, 0xDF, 0xDF, 0xFF, 0x9F, 0xCE, 0xE6, 0xF2, 0xFA, 0xFE, 0xFE, 0x01, 0xFE, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xF0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xC0, 0x40, 0xA0, 0x60, 0xB0, 0x58, 0xA8, 0x54, 0xAA, 0xD6, 0xE2, 0xFA,
    0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x7E, 0x36, 0xB9, 0x3F, 0xA0, 0x3F, 0xBF, 0x3F,
    0x7F, 0x7F, 0x7F, 0x7F, 0x7E, 0x3D, 0xBB, 0x37, 0xB7, 0x57, 0x97, 0x47, 0xA7, 0x53, 0xA9, 0x00,
    0x00, 0x80, 0xC0, 0xC0, 0x48, 0x58, 0xD0, 0xF0, 0xE0, 0xEC, 0xEE, 0xEE, 0xDC, 0x3D, 0xFA, 0xF5,
    0xEA, 0x15, 0xAA, 0x53, 0x6B, 0x37, 0x27, 0x17, 0x1B, 0x0B, 0x0B, 0x0D, 0x06, 0x02, 0x02, 0x03,
    0x02, 0x05, 0x0E, 0x75, 0xAA, 0x55, 0xBA, 0x7D, 0x7E, 0x3D, 0x8A, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x03, 0x03, 0x39,
    0x0F, 0x03, 0x01, 0x03, 0x0F, 0x0C, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFD, 0xAA, 0x55,
    0xBE, 0x7F, 0xBE, 0x1D, 0x82, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x00
};

PROGMEM static const byte sound1[] = {
    0x90, 69, 0, 10, 0x80, 0xF0
};

PROGMEM static const byte sound2[] = {
    0x90, 74, 0, 20, 0x80, 0xF0
};

PROGMEM static const char menuText[] = "START GAME\0SOUND \0RECORD\0CREDIT";
PROGMEM static const char creditText[] = "- " APP_TITLE " -\0\0" APP_RELEASED \
        "\0PROGREMMED BY OBONO\0\0THIS PROGRAM IS\0" "RELEASED UNDER\0" "THE MIT LICENSE.";

static uint8_t  state;
static bool     toDraw;
static bool     sound;
static int8_t   menuPos;
static uint16_t lastScore = 0;
static uint8_t  recordState = RECORD_NOT_READ;
static int16_t  eepAddr;
static uint16_t hiScore[10];
static uint16_t playCount;
static uint32_t playFrames;

/*---------------------------------------------------------------------------*/
/*                              Main Functions                               */
/*---------------------------------------------------------------------------*/

void initTitle(void)
{
    arduboy.setRGBled(0, 0, 0);
    state = STATE_MENU;
    menuPos = MENU_START;
    toDraw = true;
    setSound(arduboy.audio.enabled());
    if (recordState == RECORD_NOT_READ) {
        readRecord();
    }
}

bool updateTitle(void)
{
    bool ret = false;
    if (state == STATE_MENU) {
        ret = handleButtons();
    } else {
        if (arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
            state = STATE_MENU;
            playSound2();
            toDraw = true;
        }
    }

#ifdef DEBUG
    if (dbgRecvChar == 'r') {
        eepSeek(EEPROM_ADDR_BASE);
        for (int i = 0; i < 8; i++) {
            eepWrite32(0);
        }
        recordState = RECORD_INITIAL;
        dprintln("Clean EEPROM");
    }
#endif

    randomSeed(rand() ^ micros()); // Shuffle random
    return ret;
}

void drawTitle(void)
{
    if (toDraw) {
        arduboy.clear();
        switch (state) {
        case STATE_MENU:
            drawTitleMenu();
            break;
        case STATE_RECORD:
            drawTitleRecord();
            break;
        case STATE_CREDIT:
            drawTitleCredit();
            break;
        }
        toDraw = false;
    }
}

uint8_t setLastScore(int score, uint32_t frames)
{
    lastScore = score;

    /*  Updarte best 10  */
    int r = 10;
    uint16_t h;
    while (r > 0 && (h = hiScore[r - 1]) < score) {
        if (--r < 9) hiScore[r + 1] = h;
    }
    if (r < 10) hiScore[r] = score;
    playCount++;
    playFrames += frames;

    /*  Store record to EEPROM  */
    if (recordState == RECORD_INITIAL) {
        eepSeek(EEPROM_ADDR_BASE);
        eepWrite32(EEPROM_SIGNATURE);
    }
    eepSeek(EEPROM_ADDR_BASE + 4 + r * 2);
    for (int i = r; i < 10; i++) {
        eepWrite16(hiScore[i]);
    }
    eepWrite16(playCount);
    eepWrite32(playFrames);

    uint16_t checkSum = (EEPROM_SIGNATURE & 0xFFFF) + (EEPROM_SIGNATURE >> 16) * 2;
    for (int i = 0; i < 10; i++) {
        checkSum += hiScore[i] * (i + 3);
    }
    checkSum += playCount * 13;
    checkSum += (playFrames & 0xFFFF) * 14 + (playFrames >> 16) * 15;
    eepWrite16(checkSum);

    recordState = RECORD_STORED;
    return r;
}

/*---------------------------------------------------------------------------*/
/*                             Control Functions                             */
/*---------------------------------------------------------------------------*/

static bool handleButtons()
{
    bool ret = false;
    if (arduboy.buttonDown(UP_BUTTON)) {
        if (menuPos-- == MENU_START) menuPos = MENU_CREDIT;
        playSound1();
        toDraw = true;
    }
    if (arduboy.buttonDown(DOWN_BUTTON)) {
        if (menuPos++ == MENU_CREDIT) menuPos = MENU_START;
        playSound1();
        toDraw = true;
    }
    if (arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
        switch (menuPos) {
        case MENU_START:
            ret = true;
            arduboy.audio.saveOnOff();
            break;
        case MENU_SOUND:
            setSound(!sound);
            playSound2();
            break;
        case MENU_RECORD:
            state = STATE_RECORD;
            playSound2();
            break;
        case MENU_CREDIT:
            state = STATE_CREDIT;
            playSound2();
            break;
        }
        toDraw = true;
    }
    return ret;
}

static void setSound(bool enabled)
{
    if (enabled) {
        arduboy.audio.on();
    } else {
        arduboy.audio.off();
    }
    sound = enabled;
}

static void playSound1(void)
{
    arduboy.playScore2(sound1, 255);
}

static void playSound2(void)
{
    arduboy.playScore2(sound2, 255);
}

static void readRecord(void)
{
    bool isRegular = false;
    eepSeek(EEPROM_ADDR_BASE);
    if (eepRead32() == EEPROM_SIGNATURE) {
        uint16_t checkSum = (EEPROM_SIGNATURE & 0xFFFF) + (EEPROM_SIGNATURE >> 16) * 2;
        for (int i = 0; i < 13; i++) {
            checkSum += eepRead16() * (i + 3);
        }
        isRegular = (eepRead16() == checkSum);
    }

    if (isRegular) {
        /*  Read record from EEPROM  */
        eepSeek(EEPROM_ADDR_BASE + 4);
        for (int i = 0; i < 10; i++) {
            hiScore[i] = eepRead16();
        }
        playCount = eepRead16();
        playFrames = eepRead32();
        recordState = RECORD_STORED;
    } else {
        /*  Initialize record  */
        for (int i = 0; i < 10; i++) {
            hiScore[i] = 0;
        }
        playCount = 0;
        playFrames = 0;
        recordState = RECORD_INITIAL;
    }
}

/*---------------------------------------------------------------------------*/
/*                              Draw Functions                               */
/*---------------------------------------------------------------------------*/

static void drawTitleMenu(void)
{
    /*  Title image  */
    arduboy.drawBitmap(32, 0, imgTitle1, 96, 16, WHITE);
    arduboy.drawBitmap(8, 16, imgTitle2, 112, 16, WHITE);
    arduboy.drawBitmap(0, 32, imgTitle3, 56, 32, WHITE);

    /*  Last score  */
    if (lastScore > 0) {
        arduboy.setCursor(0, 0);
        arduboy.print(lastScore);
    }

    /*  Menu items  */
    char buf[12];
    const char *p = menuText;
    for (int i = 0; i < 4; i++) {
        strcpy_P(buf, p);
        p += arduboy.printEx(68 - (i == menuPos) * 4, i * 6 + 40, buf) + 1;
        if (i == MENU_SOUND) {
            arduboy.print((sound) ? F("ON") : F("OFF"));
        }
    }
    arduboy.fillRect2(56, menuPos * 6 + 40, 5, 5, WHITE);
}

static void drawTitleRecord(void)
{
    arduboy.printEx(22, 4, F("BEST 10 SCORES"));
    arduboy.drawFastHLine2(0, 12, 128, WHITE);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 5; j++) {
            int r = i * 5 + j;
            arduboy.printEx(i * 60 + 4 - (r == 9) * 6, j * 6 + 14, F("["));
            arduboy.print(r + 1);
            arduboy.print(F("] "));
            arduboy.print(hiScore[r]);
        }
    }
    arduboy.drawFastHLine2(0, 44, 128, WHITE);
    arduboy.printEx(16, 48, F("PLAY COUNT "));
    arduboy.print(playCount);
    arduboy.printEx(16, 54, F("PLAY TIME  "));
    arduboy.print(playFrames / 3600); // minutes
    char buf[6];
    sprintf(buf, "'%02d\"", playFrames / 60 % 60); // seconds
    arduboy.print(buf);
}

static void drawTitleCredit(void)
{
    char buf[20];
    const char *p = creditText;
    for (int i = 0; i < 8; i++) {
        strcpy_P(buf, p);
        uint8_t len = strnlen(buf, sizeof(buf));
        p += arduboy.printEx(64 - len * 3, i * 6 + 8, buf) + 1;
    }
}


/*---------------------------------------------------------------------------*/
/*                             EEPROM Functions                              */
/*---------------------------------------------------------------------------*/

void eeprom_busy_wait(){};

uint8_t eeprom_read_byte(uint16_t eepAddr){
  uint8_t dta;
  EEPROM.get(eepAddr, dta);
  return (dta);  
};

uint16_t eeprom_read_word(uint16_t eepAddr){
  uint16_t dta;
  EEPROM.get(eepAddr, dta);
  return (dta);  
};

uint32_t eeprom_read_dword(uint16_t eepAddr){
  uint32_t dta;
  EEPROM.get(eepAddr, dta);
  return (dta); 
};

void eeprom_read_block(uint8_t *p, uint16_t  eepAddr, uint8_t n){
  uint8_t dta;
  for (uint8_t i=0; i<n; i++){
    EEPROM.get(eepAddr+i, dta);
    p[i]=dta;
    };
};

void eeprom_write_byte(uint16_t eepAddr, uint8_t val){
  EEPROM.put(eepAddr, val);
  EEPROM.commit();
};

void eeprom_write_word(uint16_t eepAddr, uint16_t val){
  EEPROM.put(eepAddr, val);
  EEPROM.commit();
  };

void eeprom_write_dword(uint16_t eepAddr, uint32_t val){
  EEPROM.put(eepAddr, val);
  EEPROM.commit();
  };

void eeprom_write_block(uint8_t *p, uint16_t eepAddr, uint8_t n){
  for (uint8_t i=0; i<n; i++){
    EEPROM.put(eepAddr+i, p[i]);
  }
  EEPROM.commit();
};



void eepSeek(int addr)
{
    //eepAddr = max(addr, EEPROM_STORAGE_SPACE_START);
    eepAddr = addr;
}

uint8_t eepRead8(void)
{
    eeprom_busy_wait();
    return eeprom_read_byte(eepAddr++);
}

uint16_t eepRead16(void)
{
    eeprom_busy_wait();
    uint16_t ret = eeprom_read_word(eepAddr);
    eepAddr += 2;
    return ret;
}

uint32_t eepRead32(void)
{
    eeprom_busy_wait();
    uint32_t ret = eeprom_read_dword(eepAddr);
    eepAddr += 4;
    return ret;
}

void eepReadBlock(uint8_t *p, uint8_t n)
{
    eeprom_busy_wait();
    eeprom_read_block(p,eepAddr, n);
    eepAddr += n;
}

void eepWrite8(uint8_t val)
{
    eeprom_busy_wait();
    eeprom_write_byte(eepAddr, val);
    eepAddr++;
}

void eepWrite16(uint16_t val)
{
    eeprom_busy_wait();
    eeprom_write_word(eepAddr, val);
    eepAddr += 2;
}

void eepWrite32(uint32_t val)
{
    eeprom_busy_wait();
    eeprom_write_dword(eepAddr, val);
    eepAddr += 4;
}

void eepWriteBlock(uint8_t *p, uint8_t n)
{
    eeprom_busy_wait();
    eeprom_write_block(p, eepAddr, n);
    eepAddr += n;
}
