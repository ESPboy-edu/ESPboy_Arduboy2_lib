/**
 * @file Arduboy2Audio.cpp
 * \brief
 * The Arduboy2Audio class for speaker and sound control.
 */

#include "Arduboy2.h"
#include "Arduboy2Audio.h"

#define LOWEST_NOTE 52
#define FIRST_NOTE_IN_INT_ARRAY 52
#define HIGHEST_NOTE 82

const unsigned int PROGMEM _midi_word_note_frequencies[] = {
//262,277,294, //note 48 to 50
/*311,*/330,349,370,392,415,440,466,494,523, //note 51 to 60
554,587,622,659,698,740,784,831,880,932, //note 61 to 70
988,1047,1109,1175,1245,1319,1397,1480,1568,1661, //note 71 to 80
1760,1865,//1976,2093,2217,2349,2489,2637,2794,2960, //note 81 to 90
//3136,3322,3520,3729,3951,4186,4435,4699,4978,5274, //note 91 to 100
//5588,5920,6272,6645,7040,7459,7902,8372,8870,9397, //note 101 to 110
//9956,10548,11175,11840,12544,13290,14080,14917,15804,16744, //note 111 to 120
//17740,18795,19912,21096,22351,23680,25088 //note 121 to 127
};

unsigned int NoteToFrequency(byte note)
{
	return pgm_read_word(_midi_word_note_frequencies + note - FIRST_NOTE_IN_INT_ARRAY);
}

bool Arduboy2Audio::audio_enabled = true;

void Arduboy2Audio::on(){
  audio_enabled = true;
}

void Arduboy2Audio::off(){
  audio_enabled = false;
}

void Arduboy2Audio::toggle(){
  if (audio_enabled)
    off();
  else
    on();
}

void Arduboy2Audio::saveOnOff(){
  EEPROM.write(EEPROM_AUDIO_ON_OFF, audio_enabled);
  EEPROM.commit();
}

void Arduboy2Audio::begin(){
  if (EEPROM.read(EEPROM_AUDIO_ON_OFF) == true) on();
  else off();
}

bool Arduboy2Audio::enabled(){
  return audio_enabled;
}
