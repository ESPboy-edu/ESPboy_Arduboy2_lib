#define ABG_IMPLEMENTATION
#define ABG_UPDATE_EVERY_N_DEFAULT 2

#include "common.hpp"

decltype(a) a;

void loop()
{
    a.waitForNextPlane();
    if(a.needsUpdate())
        update();
    render();
}

void setup()
{  
    Serial.begin(74880);
    a.boot();
    a.startGray();
}
