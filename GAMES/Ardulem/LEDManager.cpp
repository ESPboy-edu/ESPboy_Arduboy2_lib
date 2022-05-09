#include "Ardulem.h"
#include "LEDManager.h"
#include "ESPboyPlaytune.h"

extern ESPboyPlaytune tune;

namespace LEDManager
{
	const int16_t SOUND_CHANNEL = 0;

  uint16_t noteToFreq(uint16_t note){
    return (pgm_read_word(midi_word_note_frequencies + note));
  }

	struct LEDStatus
	{
		unsigned char IsOn		: 1;
		unsigned char FrameCount : 7;
	};

	struct LEDBuffer
	{
		LEDStatus	Status = {0,0};
		LEDCommand	Command = {0,0,0,0,0,0};
	};

	LEDBuffer LEDBuffers[BUFFER_COUNT];
	BufferId  CurrentBufferLightUp = GAME;
}

void LEDManager::StartLEDCommand(BufferId id, LEDCommand command)
{
	LEDBuffers[id].Status = {0,0};
	LEDBuffers[id].Command = command;
}

void LEDManager::ClearLEDCommand(BufferId id)
{
	bool needToTurnOffLedAndNote = true;

	if (id == BUFFER_COUNT)
	{
		LEDBuffers[GAME].Command = {0,0,0,0,0,0};
		LEDBuffers[LEM].Command = {0,0,0,0,0,0};
	}
	else
	{
		LEDBuffers[id].Command = {0,0,0,0,0,0};
		needToTurnOffLedAndNote = (CurrentBufferLightUp == id);
	}

	if (needToTurnOffLedAndNote)
	{
		arduboy.setRGBled(0, 0, 0);
		//tune.notone();
	}
}

void LEDManager::Update()
{
	for (int16_t i = 0; i < BUFFER_COUNT; ++i)
	{
		LEDStatus&	Status = LEDBuffers[i].Status;
		LEDCommand&	Command = LEDBuffers[i].Command;

		if (Command.RepeatTime != 0)
		{
			if (Status.IsOn)
			{
				if ((Status.FrameCount == Command.OnDuration))
				{
					Status.IsOn = 0;
					Status.FrameCount = 0;
					Command.RepeatTime--;
					if (CurrentBufferLightUp == i)
					{
						arduboy.setRGBled(0, 0, 0);
						// stop also the associated note
						//if (Command.BaseNote != 0) tune.notone();
					}
				}		
			}
			else
			{
				if ((Status.FrameCount == Command.OffDuration))
				{
					Status.IsOn = 1;
					Status.FrameCount = 0;
					arduboy.setRGBled(Command.Red * MAX_LED_BRIGHTNESS, Command.Green * MAX_LED_BRIGHTNESS, Command.Blue * MAX_LED_BRIGHTNESS);
					 // play also the associated note, and increment it for the next time
					if (Command.BaseNote != 0)
					{
						tune.tone(noteToFreq(Command.BaseNote), 100);
						Command.BaseNote += Command.NoteIncrement;
					}
					// memorize which buffer took the led
					CurrentBufferLightUp = (BufferId)i;
				}
			}

			// increase the frame counter
			Status.FrameCount++;
		}
	}
}
