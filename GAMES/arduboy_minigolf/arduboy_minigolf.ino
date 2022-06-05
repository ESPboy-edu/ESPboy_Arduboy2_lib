#include "game.hpp"

//#include <SPI.h>
//#include <EEPROM.h>
#include <Arduboy2.h>
#include <Arduino.h>

#define FPS_SET 1
#define SHOW_FPS 0

#if SHOW_FPS
Arduboy2 a;
#else
Arduboy2Base a;
#endif
uint8_t* const buf = Arduboy2Base::sBuffer;
//uint8_t* const buf = a.sBuffer;

uint16_t time_ms()
{
    return (uint16_t)millis();
}

uint8_t poll_btns()
{
    return Arduboy2::buttonsState();
}

#define REMOVE_USB 0
//#if REMOVE_USB
//int main()
//#else
void setup()
//#endif
{
    Arduboy2Base::boot();
#if REMOVE_USB

    if(Arduboy2Core::buttonsState() & DOWN_BUTTON)
    {
        Arduboy2Core::exitToBootloader();
    }
#endif
    if(Arduboy2Core::buttonsState() & UP_BUTTON)
    {
        //Arduboy2Core::sendLCDCommand(OLED_ALL_PIXELS_ON);
        Arduboy2Core::digitalWriteRGB(RGB_ON, RGB_ON, RGB_ON);
        Arduboy2Core::digitalWriteRGB(RED_LED, RGB_ON);
        //power_timer0_disable();
        for(;;) Arduboy2Core::idle();
    }
    
    game_setup();

    //a.setFrameRate(30);
    uint16_t pt = time_ms();
#if SHOW_FPS
    uint16_t dt = 0;
    uint16_t fps = 0;
    uint16_t nf = 0;
#endif
    for(;;)
    {
        //if (WDTCSR & _BV(WDE))
        {
            // disable ints and set magic key
          //  cli();
          //  *(volatile uint8_t*)0x800 = 0x77;
          //  *(volatile uint8_t*)0x801 = 0x77;
          //  for(;;);
        }
        
        //while(!a.nextFrame())
        //    ;

        // the above call increases code size by 1 KB.
        // reimplement logic below:

#if FPS_SET
        for(;;)
        {
            uint8_t t = (uint8_t)time_ms();
            uint8_t dt = t - (uint8_t)pt;
            if(dt < 33)
            {
                if(++dt < 33) Arduboy2Base::idle();
                continue;
            }
            break;
        }
#endif

#if SHOW_FPS
        uint16_t t = time_ms();
        uint16_t tdt = t - pt;
        pt = t;
        dt += tdt;
        ++nf;
        if(dt >= 250)
        {
            fps = uint32_t(1000) * nf / dt;
            nf = 0;
            dt = 0;
        } 
#else
        pt = time_ms();
#endif
        
        game_loop();

#if SHOW_FPS
        a.setCursor(0, 0);
        uint16_t f = fps;
        a.write('0' + f / 100);
        f %= 100;
        a.write('0' + f /  10);
        f %=  10;
        a.write('0' + f);
#endif
        
        //Arduboy2Base::display(true);
        a.display(true);
    }
  
#if REMOVE_USB
    return 0;
#endif
}


void loop(){};
