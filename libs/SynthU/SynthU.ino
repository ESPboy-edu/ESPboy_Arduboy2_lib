#include <Arduboy2.h>

#define SYNTHU_IMPLEMENTATION
#include "common.hpp"
#include "SynthU.hpp"


Arduboy2 arduboy;

void setup() {
  arduboy.begin();
  SynthU::setup();
  // set the framerate of the game at 60 fps
  arduboy.setFrameRate(60);
  // let's make sure the sound was not muted in a previous sketch
  arduboy.audio.on();
  // Begin playback of song.
  SynthU::play((uint32_t)song);

}


void loop(){
  SynthU::update();
  delay(1000/60);
}
