//
// Piracy for ArduBoy
// (C)2022 Bert vt Veer
//

#define _SOUND_

#include <Arduboy2.h>
//#include <avr/wdt.h>
#include "Font4x6/Font4x6.h"

Arduboy2 _ardu;
Font4x6 _font;


#define IMP_HAS_MATH true
#define IMP_screenW WIDTH
#define IMP_screenH HEIGHT
#define IMP_fgcolor WHITE
#define IMP_bgcolor BLACK

#define IMP_frameCount _ardu.frameCount
#define IMP_waitFrame() while(!_ardu.nextFrame())
#define IMP_readbyte(addr) pgm_read_byte((addr))
#define IMP_strcpy(des,src) strcpy_P((des),(PGM_P)pgm_read_dword(&((src))))
#define IMP_strlen(str) strlen_P((str)
#define IMP_storeRead(addr) EEPROM.read((addr)+EEPROM_STORAGE_SPACE_START)
#define IMP_storeWrite(addr,val) EEPROM.write((addr)+EEPROM_STORAGE_SPACE_START,(val))
#define IMP_reboot() { /*wdt_disable(); wdt_enable(WDTO_15MS); while (1) {}*/ ESP.reset(); }

#define IMP_keyup UP_BUTTON
#define IMP_keydown DOWN_BUTTON
#define IMP_keyleft LEFT_BUTTON
#define IMP_keyright RIGHT_BUTTON
#define IMP_keyA A_BUTTON
#define IMP_keyB B_BUTTON
#define IMP_buttonStates() _ardu.buttonsState()

#define IMP_getBuffer() _ardu.getBuffer()
#define IMP_clearScreen() _ardu.fillScreen(BLACK)
#define IMP_swapBuffers() _ardu.display()
#define IMP_invertScreen(st) _ardu.invert((st))

void __IMP_fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t col, bool fill) {
    _ardu.fillRect(x, y, w, h, col); 
    if (!fill) _ardu.fillRect(x+1, y+1, w-2, h-2, 1-col);   
}
#define IMP_fillRect(x,y,w,h,col) __IMP_fillRect((x),(y),(w),(h),(col), true)
#define IMP_drawRect(x,y,w,h,col) __IMP_fillRect((x),(y),(w),(h),(col), false)
#define IMP_drawPixel(x,y,col) _ardu.fillRect((x),(y),1,1,(col))
#define IMP_drawImage(x,y,bmp,fr) Sprites::drawExternalMask((x),(y),(bmp),image::generic_mask,(fr),(0))
#define IMP_drawImageMasked(x,y,bmp,mask,fr) Sprites::drawExternalMask((x),(y),(bmp),(mask),(fr),(fr))

#define IMP_drawTextXY(x,y,txt) { _font.setCursor((x),(y)); _font.print((txt)); }
#define IMP_drawText(txt) _font.print((txt)) 


#ifdef _SOUND_

#include <Arduboy2Beep.h>
BeepPin1 _beep1;
BeepPin2 _beep2;

#define IMP_playSound(ch,f) { \
    if((ch)==0) _beep1.tone(_beep1.freq((f))); \
    else _beep2.tone(_beep2.freq((f))); \
}
#define IMP_stopSound(ch) { if((ch)==0) _beep1.noTone(); else _beep2.noTone(); }
#define IMP_noSound() { _beep1.noTone(); _beep2.noTone(); }
#define IMP_soundEnabled() _ardu.audio.enabled()
#define IMP_toggleSound() _ardu.audio.toggle()
#define IMP_saveSoundState() _ardu.audio.saveOnOff()
#else
#define IMP_playSound(notes)
#define IMP_soundEnabled() false
#define IMP_toggleSound()
#define IMP_saveSoundState()

#endif // _SOUND_



#include "game.h"
Game _game;


void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("STARTGAME");
    _ardu.begin();
    //_ardu.flashlight();
    //_ardu.systemButtons();
    _ardu.audio.begin();
    _ardu.setFrameRate(60);
    
#ifdef _SOUND_
    _beep1.begin();
    _beep2.begin();
#endif   
}


void loop() {
    if (_ardu.nextFrame()) {
        _game.nextFrame();
    }
    delay(0);
}
