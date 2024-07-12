#define ABG_IMPLEMENTATION
#define ABG_UPDATE_EVERY_N_DEFAULT 1

#include "common.hpp"

decltype(a) a;


void setup()
{  
    Serial.begin(74880);
    a.boot();
    a.startGray();
    a.setFrameRate(240);
}


void loop()
{
    if(!a.nextFrame()) return;
    a.waitForNextPlane();
    if(a.needsUpdate())
        update();
    render();
}
