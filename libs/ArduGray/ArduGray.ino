#define ABG_IMPLEMENTATION
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
