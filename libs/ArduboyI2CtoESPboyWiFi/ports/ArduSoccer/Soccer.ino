#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <ArduboyI2C.h>
#include "Engine.h"
#include "ArduboyPlatform.h"
//#include "Generated/Data_Audio.h"

Arduboy2Base arduboy;
ArduboyTones sound(arduboy.audio.enabled);

unsigned long lastTimingSample;

void ArduboyPlatform::playSound(const uint16_t* pattern)
{
	sound.tones(pattern);
}

void setup() {
	arduboy.boot();
	I2C::begin();

	//arduboy.systemButtons();
	//arduboy.bootLogo();
	arduboy.setFrameRate(TARGET_FRAMERATE);
	// arduboy.audio.begin();
	arduboy.waitNoButtons();

	engine.init();
}

void loop() {
	//static int16_t tickAccum = 0;
	//unsigned long timingSample = millis();
	//tickAccum += (timingSample - lastTimingSample);
	//lastTimingSample = timingSample;

	if (!arduboy.nextFrame()) return;

	Platform.update();

	/*  constexpr int16_t frameDuration = 1000 / TARGET_FRAMERATE;
	while(tickAccum > frameDuration)
	{
	engine.update();
	tickAccum -= frameDuration;
	}*/
	engine.update();

	engine.draw();

	arduboy.display(true);
}
