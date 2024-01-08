// Sketch for testing the ArduboyTones library

#include <Arduboy2.h>
#include "src/ArduboyTonesFX.h"
#include <ArduboyFX.h>
#include "fxdata/fxdata.h"

Arduboy2 arduboy;
uint16_t buffer[8]; 
ArduboyTonesFX sound(arduboy.audio.enabled, buffer);

#define BUTTON_DELAY 200

int16_t logoPos = 128;

uint16_t inRAM[] = {
  NOTE_E4,400, NOTE_D4,200, NOTE_C4,400, NOTE_REST,200, NOTE_D4,200,
  NOTE_C4,300, NOTE_REST,100, NOTE_C4,300, NOTE_REST,100, NOTE_E4,300,
  NOTE_REST,100, NOTE_G4,300, NOTE_REST,100, NOTE_F4,300, NOTE_REST,100,
  NOTE_A4,300, NOTE_REST,100, NOTE_D5H,200, NOTE_REST,200, NOTE_D5H,200,
  NOTE_REST,1500,
  TONES_REPEAT
};

void setup() {
  arduboy.begin();
  FX::disableOLED();
  FX::begin(FX_DATA_PAGE);
}

void loop() {

  if (!arduboy.nextFrame()) return; 
  arduboy.pollButtons();

  sound.fillBufferFromFX();

  arduboy.setCursor(0,0);
  arduboy.print("tonesFromFX()\nA: Start / Stop\n\n");
  arduboy.print("tonesInRam()\nB: Start / Stop\n");

  if (arduboy.justPressed(A_BUTTON)) {
    if (sound.playing()) {
      sound.noTone();
    }
    else {
      sound.tonesFromFX(fromSD);
    }
  }

  if (arduboy.justPressed(B_BUTTON)) {
    if (sound.playing()) {
      sound.noTone();
    }
    else {
      sound.tonesInRAM(inRAM);
    }
  }

  FX::drawBitmap(logoPos, 48, fxLogo, 0, dbmNormal);

  logoPos--;
  if (logoPos == -116) logoPos = 128;

  FX::enableOLED();
  arduboy.display(CLEAR_BUFFER);
  FX::disableOLED();

}
