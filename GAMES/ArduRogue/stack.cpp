#include "game.hpp"

//#if defined(ARDUINO)
//extern char* __bss_end;
//extern uint8_t __stack;
//#endif

//#if defined(ARDUINO)
//static constexpr uint8_t CANARY_VAL = 0xa5;
//#endif

void stack_canary_init()
{
//#if defined(ARDUINO)
//    uint8_t* p = (uint8_t*)&__bss_end;
//    while(p <= SP)
//        *p++ = CANARY_VAL;
//#endif
}

uint16_t unused_stack()
{
  /*
#if defined(ARDUINO)
    uint8_t* p = (uint8_t*)&__bss_end;
    uint16_t n = 0;
    while(p <= SP)
    {
        if(*p++ != CANARY_VAL) break;
        ++n;
    }
    return n;
#else
*/
    return 0;
//#endif
}
