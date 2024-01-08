#include <Arduboy2.h>      
#include "fxdataW3D.h"
#include <ArduboyFX.h>      
#include "ArduboyTonesFX.h"

#include "Engine.h"
#include "Generated/fxdata.h"
#include "ArduboyPlatform.h"
#include "Generated/Data_Audio.h"

Arduboy2Base arduboy;

uint16_t audioBuffer[32];
ArduboyTonesFX sound(arduboy.audio.enabled, audioBuffer, 32);

unsigned long lastTimingSample;

void ArduboyPlatform::playSound(uint8_t id)
{
	sound.tonesFromFX((uint24_t)((uint32_t) Data_audio + (uint32_t)(pgm_read_word(&Data_AudioPatterns[id]))));
}

void setup() {
  Serial.begin(115200);
  arduboy.boot();
  //arduboy.flashlight();
  //arduboy.systemButtons();
  //arduboy.bootLogo();
  arduboy.setFrameRate(TARGET_FRAMERATE);
  arduboy.audio.begin();
  arduboy.audio.on();

  FX::begin(FX_DATA_PAGE, FX_SAVE_PAGE);    
  engine.init();
}

void loop() {
  static int16_t tickAccum = 0;
  unsigned long timingSample = millis();
  tickAccum += (timingSample - lastTimingSample);
  lastTimingSample = timingSample;
  
  if (!arduboy.nextFrame()) return; 
 
  sound.fillBufferFromFX();
  Platform.update();

  constexpr int16_t frameDuration = 1000 / TARGET_FRAMERATE;
  while(tickAccum > frameDuration)
  {
    engine.update();
	tickAccum -= frameDuration;
  }
	
  engine.draw();

  if(engine.gameState == GameState_Playing && engine.renderer.damageIndicator < 0)
  {
    uint8_t brightness = -engine.renderer.damageIndicator * 5;
    arduboy.setRGBled(brightness, brightness, brightness);
  }
  else if(engine.gameState == GameState_Playing && engine.renderer.damageIndicator > 0)
  {
    uint8_t brightness = engine.renderer.damageIndicator * 51;
    arduboy.setRGBled(brightness, 0, 0);
  }
  else
  {
    arduboy.setRGBled(0, 0, 0);
  }  
  
  FX::display();
  //FX::display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
}
