#ifndef LED_MANAGER_H
#define LED_MANAGER_H
/*
 * This file handle the update of the LED.
 * You can send it a LED command, and it will update the LED color
 */

namespace LEDManager
{

  static const uint16_t midi_word_note_frequencies[128] PROGMEM = {
  16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31,
  33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65,
  69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131,
  139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262,
  277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523,
  554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047,
  1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093,
  2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186,
  4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372,
  8870, 9397, 9956, 10548, 11175, 11840, 12544, 13290, 14080, 14917,
  15804, 16744, 17740, 18795, 19912, 21096, 22351, 23680, 25088
};
  
	struct LEDCommand
	{
		uint16_t Red			:1;
		uint16_t Green			:1;
		uint16_t Blue			:1;
		uint16_t OnDuration		:6;
		uint16_t OffDuration	:7;
		unsigned char BaseNote;		
		signed char NoteIncrement			:4; // increment can be negative
		unsigned char RepeatTime	:4;
	};

	enum BufferId
	{
		GAME = 0,
		LEM,
		BUFFER_COUNT,
	};

	// set the LED command in the specified buffer
	extern void StartLEDCommand(BufferId id, LEDCommand command);

	// clear the specified buffer and stop the LED. If id equals BUFFER_COUNT, this will clear all the buffers.
	extern void ClearLEDCommand(BufferId id);

	// update that should be called every frame
	extern void Update();

  uint16_t noteToFreq(uint16_t note);
}

#endif
