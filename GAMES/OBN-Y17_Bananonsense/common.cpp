#include "common.h"

#include <ESPboyPlaytune.h>
extern ESPboyPlaytune pl;

/*  Defines  */

#define EEPROM_ADDR_BASE    600
#define EEPROM_SIGNATURE    0x174E424FUL // "OBN\x17"

#define PAD_REPEAT_DELAY    (FPS / 4)
#define PAD_REPEAT_INTERVAL (FPS / 12)

enum RECORD_STATE_T : uint8_t {
    RECORD_NOT_READ = 0,
    RECORD_INITIAL,
    RECORD_STORED,
};

/*  Global Variables  */

MyArduboy2  ab;


RECORD_T    record;
uint8_t     counter;
int8_t      padX, padY, padRepeatCount;
bool        isInvalid, isRecordDirty;

/*  Local Functions  */

static uint16_t calcCheckSum();

static void     eepSeek(int addr);
static uint8_t  eepRead8(void);
static uint16_t eepRead16(void);
static uint32_t eepRead32(void);
static void     eepReadBlock(void *p, size_t n);
static void     eepWrite8(uint8_t val);
static void     eepWrite16(uint16_t val);
static void     eepWrite32(uint32_t val);
static void     eepWriteBlock(const void *p, size_t n);

/*  Local Variables  */

static RECORD_STATE_T   recordState = RECORD_NOT_READ;
static int16_t          eepAddr;
static bool             isInvalidInst;

/*---------------------------------------------------------------------------*/
/*                             Common Functions                              */
/*---------------------------------------------------------------------------*/

void readRecord(void)
{
    bool isVerified = false;
    eepSeek(EEPROM_ADDR_BASE);
    if (eepRead32() == EEPROM_SIGNATURE) {
        eepReadBlock(&record, sizeof(record));
        isVerified = (eepRead16() == calcCheckSum());
    }

    if (isVerified) {
        recordState = RECORD_STORED;
        isRecordDirty = false;
    } else {
        memset(&record, 0, sizeof(record));
        recordState = RECORD_INITIAL;
        isRecordDirty = true;
    }
    setSound(ab.isAudioEnabled()); // Load Sound ON/OFF
}

void writeRecord(void)
{
    if (!isRecordDirty) return;
    if (recordState == RECORD_INITIAL) {
        eepSeek(EEPROM_ADDR_BASE);
        eepWrite32(EEPROM_SIGNATURE);
    } else {
        eepSeek(EEPROM_ADDR_BASE + 4);
    }
    eepWriteBlock(&record, sizeof(record));
    eepWrite16(calcCheckSum());
    ab.audio.saveOnOff(); // Save Sound ON/OFF
    recordState = RECORD_STORED;
    isRecordDirty = false;
}

void clearRecord(void)
{
    eepSeek(EEPROM_ADDR_BASE);
    for (int i = 0; i < (sizeof(record) + 6) / 4; i++) {
        eepWrite32(0);
    }
    recordState = RECORD_INITIAL;
}

void handleDPad(void)
{
    padX = padY = 0;
    if (ab.buttonPressed(LEFT_BUTTON | RIGHT_BUTTON | UP_BUTTON | DOWN_BUTTON)) {
        if (++padRepeatCount >= (PAD_REPEAT_DELAY + PAD_REPEAT_INTERVAL)) {
            padRepeatCount = PAD_REPEAT_DELAY;
        }
        if (padRepeatCount == 1 || padRepeatCount == PAD_REPEAT_DELAY) {
            if (ab.buttonPressed(LEFT_BUTTON))  padX--;
            if (ab.buttonPressed(RIGHT_BUTTON)) padX++;
            if (ab.buttonPressed(UP_BUTTON))    padY--;
            if (ab.buttonPressed(DOWN_BUTTON))  padY++;
        }
    } else {
        padRepeatCount = 0;
    }
}

void drawTime(int16_t x, int16_t y, uint32_t frames)
{
    uint16_t h = frames / (FPS * 3600UL);
    uint8_t m = frames / (FPS * 60) % 60;
    uint8_t s = frames / FPS % 60;
    ab.setCursor(x, y);
    if (h == 0 && m == 0) {
        if (s < 10) ab.print('0');
        ab.print(s);
        ab.print('.');
        ab.print(frames / (FPS / 10) % 10);
    } else {
        if (h > 0) {
            ab.print(h);
            ab.print(':');
            if (m < 10) ab.print('0');
        }
        ab.print(m);
        ab.print(':');
        if (s < 10) ab.print('0');
        ab.print(s);
    }
}

/*---------------------------------------------------------------------------*/
/*                              Sound Functions                              */
/*---------------------------------------------------------------------------*/

void setSound(bool on)
{
    ab.setAudioEnabled(on);
}

void playSoundTick(void)
{
    pl.tone(440, 10);
}

void playSoundClick(void)
{
    pl.tone(587, 20);
}

/*---------------------------------------------------------------------------*/
/*                             EEPROM Functions                              */
/*---------------------------------------------------------------------------*/

static uint16_t calcCheckSum()
{
    uint16_t checkSum = (EEPROM_SIGNATURE & 0xFFFF) + (EEPROM_SIGNATURE >> 16) * 3;
    uint16_t *p = (uint16_t *) &record;
    for (int i = 0; i < sizeof(record) / 2; i++) {
        checkSum += *p++ * (i * 2 + 5);
    }
    return checkSum;
}

void eepSeek(int addr)
{
    eepAddr = max(addr, EEPROM_STORAGE_SPACE_START);
}

uint8_t eepRead8(void)
{
    uint8_t dta;
    EEPROM.get(eepAddr,dta);
    eepAddr++;
    return dta;
}

uint16_t eepRead16(void)
{
    uint16_t dta;
    EEPROM.get(eepAddr,dta);
    eepAddr += 2;
    return dta;
}

uint32_t eepRead32(void)
{
    uint32_t dta;
    EEPROM.get(eepAddr,dta);
    eepAddr += 4;
    return dta;
}

void eepReadBlock(void *p, size_t n)
{   
   uint8_t *pnt = (uint8_t *)p;
    for (uint16_t i=0; i<n; i++){
      pnt[i] = EEPROM.read(eepAddr);
      eepAddr++;
    }
}

void eepWrite8(uint8_t val)
{
    EEPROM.put(eepAddr, val);
    eepAddr++;
    EEPROM.commit();
}

void eepWrite16(uint16_t val)
{
    EEPROM.put(eepAddr, val);
    eepAddr += 2;
    EEPROM.commit();
}

void eepWrite32(uint32_t val)
{
    EEPROM.put(eepAddr, val);
    eepAddr += 4;
    EEPROM.commit();
}

void eepWriteBlock(const void *p, size_t n)
{   
    uint8_t *pnt = (uint8_t *)p;
    for (uint16_t i=0; i<n; i++){
      EEPROM.write(eepAddr, pnt[i]);
      eepAddr++;
 }
 EEPROM.commit();
}