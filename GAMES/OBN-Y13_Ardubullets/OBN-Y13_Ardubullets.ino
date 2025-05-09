#include "common.h"

/*  Typedefs  */

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
        case 'r':
            clearRecord();
            break;
        }
        if (recv >= ' ' && recv <= '~') {
            dbgRecvChar = recv;
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
    //arduboy.setTextColors(WHITE, WHITE);
    mode = MODE_LOGO;
    moduleTable[mode].initFunc();
}

void loop()
{
#ifdef DEBUG
    dbgCheckSerialRecv();
#endif
    if (!(arduboy.nextFrame())) return;
    MODE_T nextMode = moduleTable[mode].updateFunc();
    moduleTable[mode].drawFunc();
#ifdef DEBUG
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
