#include "common.h"


typedef struct
{
    void(*initFunc)(void);
    MODE_T(*updateFunc)(void);
    void(*drawFunc)(void);
} MODULE_FUNCS;

/*  Local Variables  */

PROGMEM static const MODULE_FUNCS moduleTable[] = {
    { initLogo,     updateLogo,     drawLogo },
    { initTitle,    updateTitle,    drawTitle },
    { initGame,     updateGame,     drawGame },
};

static MODE_T mode;

/*  For Debugging  */

#ifdef DEBUG
bool    dbgPrintEnabled = true;
char    dbgRecvChar = '\0';
uint8_t dbgCaptureMode = 0;

static void dbgCheckSerialRecv(void)
{
    int recv;
    while ((recv = Serial.read()) != -1) {
        switch (recv) {
        case 'd':
            dbgPrintEnabled = !dbgPrintEnabled;
            Serial.print("Debug output ");
            Serial.println(dbgPrintEnabled ? "ON" : "OFF");
            break;
        case 'c':
            dbgCaptureMode = 1;
            break;
        case 'C':
            dbgCaptureMode = 2;
            break;
        case 'r':
            clearRecord();
            break;
        }
        if (recv >= ' ' && recv <= '~') {
            dbgRecvChar = recv;
        }
    }
}

static void dbgScreenCapture()
{
    if (dbgCaptureMode) {
        Serial.write((const uint8_t *)arduboy.getBuffer(), WIDTH * HEIGHT / 8);
        if (dbgCaptureMode == 1) {
            dbgCaptureMode = 0;
        }
    }
}
#endif

/*---------------------------------------------------------------------------*/

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif
    arduboy.beginNoLogo();
    arduboy.setFrameRate(FPS);
    //arduboy.setTextColor(WHITE, WHITE);
    mode = MODE_LOGO;
    moduleTable[mode].initFunc();
}

void loop()
{
  delay(0);
#ifdef DEBUG
    dbgCheckSerialRecv();
#endif
    if (!(arduboy.nextFrame())) return;
    MODE_T nextMode = moduleTable[mode].updateFunc();
    moduleTable[mode].drawFunc();
#ifdef DEBUG
    dbgScreenCapture();
    dbgRecvChar = '\0';
#endif
    arduboy.display();
    if (mode != nextMode) {
        mode = nextMode;
        dprint(F("mode="));
        dprintln(mode);
        moduleTable[mode].initFunc();
    }
}
