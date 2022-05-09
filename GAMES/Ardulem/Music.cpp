#include "Ardulem.h"
#include "Music.h"

namespace Music
{
// Playtune bytestream for file "e.mid" created by MIDITONES V1.14 on Fri Oct 20 17:11:59 2017
// command line: miditones.exe -r -t1 e 
const unsigned char PROGMEM score [] = {
// French Horn
0,18, 0x90,66, 1,103, 0x80, 0,21, 0x90,66, 0,245, 0x80, 0,18, 0x90,66, 0,113, 0x80, 0,21, 0x90,66, 1,8, 
0x90,64, 0,135, 0x90,62, 1,144, 0x90,66, 1,122, 0x80, 0,21, 0x90,66, 0,245, 0x80, 0,18, 0x90,66, 0,113, 
0x80, 0,21, 0x90,66, 0,131, 0x90,67, 0,133, 0x90,66, 0,135, 0x90,64, 1,144, 0x90,66, 1,122, 0x80, 0,21, 
0x90,66, 0,245, 0x80, 0,18, 0x90,66, 0,113, 0x80, 0,21, 0x90,66, 1,8, 0x90,67, 0,135, 0x90,69, 1,144, 
0x90,61, 6,64, 0x90,59, 1,122, 0x80, 0,21, 0x90,59, 0,245, 0x80, 0,18, 0x90,59, 0,113, 0x80, 0,21, 0x90,59, 
1,8, 0x90,61, 0,135, 0x90,62, 1,144, 0x90,61, 1,122, 0x80, 0,21, 0x90,61, 0,245, 0x80, 0,18, 0x90,61, 
0,113, 0x80, 0,21, 0x90,61, 1,8, 0x90,62, 0,135, 0x90,64, 1,144, 0x90,69, 1,144, 0x90,66, 1,144, 0x90,62, 
1,144, 0x90,57, 1,144, 0x90,64, 6,64, 0x80, 1,144, 0x90,57, 2,88, 0x90,64, 2,88, 0x90,62, 2,88, 0x90,57, 
2,88, 0x90,62, 1,144, 0x80, 1,144, 0x90,66, 2,88, 0x80, 0,101, 0x90,74, 1,144, 0x80, 0,98, 0x90,74, 1,144, 
0x90,73, 1,144, 0x90,71, 1,144, 0x90,73, 1,144, 0x90,74, 0,200, 0x80, 1,144, 0x90,74, 2,88, 0x90,76, 1,144, 
0x80, 1,144, 0x90,74, 1,123, 0x80, 0,20, 0x90,74, 1,144, 0x90,73, 1,144, 0x90,74, 6,64, 0x80, 6,64, 0xe0};
// This score contains 294 bytes, and 1 tone generator is used.
}

void Music::SwitchMusicStatus()
{
	unsigned char currentValue = EEPROM.read(EEPROM_AUDIO_ON_OFF);
	EEPROM.write(EEPROM_AUDIO_ON_OFF, !currentValue);
  EEPROM.commit();
}

bool Music::IsMusicEnabled()
{
	return EEPROM.read(EEPROM_AUDIO_ON_OFF);
}
