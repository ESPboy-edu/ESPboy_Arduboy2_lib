#include "game.hpp"

//#include <SPI.h>
//#include <EEPROM.h>
#include <Arduboy2.h>
#include <Arduino.h>
#include "lib/ESPboyInit.h"

extern ESPboyInit myESPboy;

/*
static constexpr uint8_t LOOP_TIME_MS = 4;
static constexpr uint8_t DEBOUNCE_NUM = 3;
static constexpr uint8_t REP_INIT_NUM = 80;
static constexpr uint8_t REP_NUM = 40;

static uint8_t button_debounce[8];
static uint8_t button_rep[8];
static uint8_t buttons, buttons_prev;
static uint8_t just_pressed;
*/

uint8_t read_persistent(uint16_t addr)
{
    return EEPROM.read((int)addr);
}

void update_persistent(uint16_t addr, uint8_t data)
{
    EEPROM.write((int)addr, data);
}

void flush_persistent() {
  EEPROM.commit();
  }

void seed()
{
    rand_seed = Arduboy2::generateRandomSeed();
}

/*
static void poll_buttons_debounce(uint8_t debounce_num)
{
    uint8_t b = Arduboy2Core::buttonsState();
    
    uint8_t chg = 0;
    uint8_t diff = b ^ buttons;
    
    for(uint8_t n = 0, m = 1; m != 0; m <<= 1, ++n)
    {
        if(diff & m)
        {
            if(++button_debounce[n] >= debounce_num)
            {
                button_debounce[n] = 0;
                chg |= m;
                button_rep[n] = REP_INIT_NUM;
            }
        }
        else
            button_debounce[n] = 0;
    }
    
    buttons_prev = buttons;
    buttons ^= chg;
    
    static constexpr uint8_t const DIR_BUTTONS =
        UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON;
    for(uint8_t n = 0, m = 1; m != 0; m <<= 1, ++n)
    {
        if((buttons & m) && --button_rep[n] == 0)
        {
            button_rep[n] = REP_NUM;
            buttons_prev &= (~m | (~DIR_BUTTONS));
        }
    }
    
    just_pressed = buttons & ~buttons_prev;
}

static uint8_t get_just_pressed()
{
    uint8_t t = just_pressed & (just_pressed - 1);
    uint8_t r = t ^ just_pressed;
    just_pressed = t;
    return r;
}
*/

void wait()
{
    Arduboy2::delayShort(100);
}

uint8_t wait_btn()
{/*
    for(;;)
    {
        if (WDTCSR & _BV(WDE))
        {
          // disable ints and set magic key
          cli();
          *(volatile uint8_t*)0x800 = 0x77;
          *(volatile uint8_t*)0x801 = 0x77;
          for(;;);
        }

        if(just_pressed)
            return get_just_pressed();
        
        uint16_t t = (uint16_t)millis();
        
        while((uint16_t)millis() - t < LOOP_TIME_MS)
            Arduboy2Core::idle();
    
        poll_buttons_debounce(DEBOUNCE_NUM);
    }*/
    uint8_t keys=0;
    while (!(keys=myESPboy.getKeys())) delay(10);
    if (keys&PAD_ACT || keys&PAD_ESC){
      while(myESPboy.getKeys()) delay(10);
      delay(250);}
    else delay(100);
    return (keys);
}

static void paint_half(uint8_t const* b, bool clr, uint8_t shift)
{
/*
    uint16_t count;
    // this code is adapted from the Arduboy2 library
    // which is licensed under BSD-3
    // the only modification is to adjust the loop count
    // because ArduChess buffers 64x64 half-screens to save RAM
    asm volatile (
        "   ldi   %A[count], %[len_lsb]               \n\t" //for (len = WIDTH * HEIGHT / 8)
        "   ldi   %B[count], %[len_msb]               \n\t"
        "1: ld    __tmp_reg__, %a[ptr]      ;2        \n\t" //tmp = *(image)
        "   out   %[spdr], __tmp_reg__      ;1        \n\t" //SPDR = tmp
        "   cpse  %[clear], __zero_reg__    ;1/2      \n\t" //if (clear) tmp = 0;
        "   mov   __tmp_reg__, __zero_reg__ ;1        \n\t"
        "2: sbiw  %A[count], 1              ;2        \n\t" //len --
        "   sbrc  %A[count], 0              ;1/2      \n\t" //loop twice for cheap delay
        "   rjmp  2b                        ;2        \n\t"
        "   st    %a[ptr]+, __tmp_reg__     ;2        \n\t" //*(image++) = tmp
        "   brne  1b                        ;1/2 :18  \n\t" //len > 0
        "   in    __tmp_reg__, %[spsr]                \n\t" //read SPSR to clear SPIF
        : [ptr]     "+&e" (b),
          [count]   "=&w" (count)
        : [spdr]    "I"   (_SFR_IO_ADDR(SPDR)),
          [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
          [len_msb] "M"   (64 * (64 / 8 * 2) >> 8),   // 8: pixels per byte
          [len_lsb] "M"   (64 * (64 / 8 * 2) & 0xFF), // 2: for delay loop multiplier
          [clear]   "r"   (clear)
    );
*/
  static uint16_t oBuffer[WIDTH/2*16] __attribute__ ((aligned));;
  static uint16_t currentDataByte, currentDataAddr;
  static uint16_t xPos, yPos, kPos, kkPos, addr;
  static uint8_t vertOffset;
  
  myESPboy.tft.setAddrWindow(shift, 20, WIDTH/2, HEIGHT);
 
  for(kPos = 0; kPos<4; kPos++){
    kkPos = kPos<<1;
    for (xPos = 0; xPos < WIDTH/2; xPos++) {
            currentDataAddr = xPos + kkPos * WIDTH/2;
            currentDataByte = b[currentDataAddr] + (b[currentDataAddr+WIDTH/2]<<8);
      for (yPos = 0; yPos < 16; yPos++) {    
        addr =  yPos*WIDTH/2+xPos;
            if (currentDataByte & 0x01) oBuffer[addr] = TFT_YELLOW;
            else oBuffer[addr] = TFT_BLACK;
      currentDataByte = currentDataByte >> 1;
    }
    }
    myESPboy.tft.pushColors(oBuffer, WIDTH/2*16);
  }

  if(clr) memset((void *)b, 0, WIDTH/2*HEIGHT/8);

}

void paint_offset(uint8_t x, bool clear)
{
//    Arduboy2Core::LCDCommandMode();
//    Arduboy2Core::SPItransfer(0x21);
//    Arduboy2Core::SPItransfer(x);
//    Arduboy2Core::SPItransfer(x + 63);
//    Arduboy2Core::LCDDataMode();
    paint_half(buf.data(), clear, x);
}

//#define REMOVE_USB 0
//#if REMOVE_USB
//int main()
//#else
void setup()
//#endif
{
    Arduboy2Base::boot();
/*
#if REMOVE_USB

    if(Arduboy2Core::buttonsState() & DOWN_BUTTON)
    {
        Arduboy2Core::exitToBootloader();
    }
#endif
    if(Arduboy2Core::buttonsState() & UP_BUTTON)
    {
        Arduboy2Core::sendLCDCommand(OLED_ALL_PIXELS_ON);
        Arduboy2Core::digitalWriteRGB(RGB_ON, RGB_ON, RGB_ON);
        Arduboy2Core::digitalWriteRGB(RED_LED, RGB_ON);
        power_timer0_disable();
        for(;;) Arduboy2Core::idle();
    }
*/  
//    run();
//#if REMOVE_USB
//    return 0;
//#endif
}

void loop(){
  run();
}
