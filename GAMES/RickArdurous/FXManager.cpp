/*
 * This file handle the update of the LED and Sound FX.
 * You can send it a command, and it will update the LED color and play some tone
 */

#include "RickArdurous.h"
#include "FXManager.h"

namespace FXManager
{

  static bool setedLED = false;
	static constexpr int SOUND_CHANNEL = 1;
	static constexpr int MAX_LED_BRIGHTNESS = 50;
  
  struct FXStatus
  {
    unsigned char IsOn    : 1;
    unsigned char FrameCount : 7;
  };

	struct FXBuffer
	{
		FXStatus	Status = {0,0};
		FXCommand	Command = {0,0,0,0,0,0,0,0};
	};

	FXBuffer FXBuffers[BUFFER_COUNT];
	BufferId CurrentBufferLightUp = GAME;
	
	bool IsSoundOn = true;
}

void FXManager::StartFXCommand(BufferId id, FXCommand command_)
{
	FXBuffers[id].Status = {0,0};
	FXBuffers[id].Command = command_;
}

void FXManager::ClearFXCommand(BufferId id)
{
	bool needToTurnOffLedAndNote = true;

	if (id == BUFFER_COUNT)
	{
		FXBuffers[GAME].Command = {0,0,0,0,0,0,0,0};
		FXBuffers[RICK].Command = {0,0,0,0,0,0,0,0};
	}
	else
	{
		FXBuffers[id].Command = {0,0,0,0,0,0,0,0};
		needToTurnOffLedAndNote = (CurrentBufferLightUp == id);
	}

	if (needToTurnOffLedAndNote)
	{
		if (setedLED){ 
		  arduboy.setRGBled(0, 0, 0);
		  setedLED=false;}
		noTone(D3);
		//arduboy.tunes.stopNote(SOUND_CHANNEL);
	}
}

#define LED_BRIGHTNESS 50

void FXManager::Update(){   
  
	for (unsigned char i = 0; i < BUFFER_COUNT; ++i)
	{
		FXStatus	&Status = FXBuffers[i].Status;
		FXCommand	&Command = FXBuffers[i].Command;
		if (Command.RepeatTime != 0){
			if (Status.IsOn){
				if (Status.FrameCount == Command.OnDuration){
					Status.IsOn = 0;
					Status.FrameCount = 0;
					Command.RepeatTime--;
					if (CurrentBufferLightUp == i)
					{
						if(setedLED){
						  arduboy.setRGBled(0, 0, 0);
						  setedLED=false;}
						// stop also the associated note
						if (Command.BaseNote != 0)
							noTone(D3);
					}
					if (Command.OffDuration != 0)
					{
						Status.FrameCount = 1;
						continue;
					}
				}
			}
			if (!Status.IsOn && (Command.RepeatTime != 0))
			{ 
				if (Status.FrameCount == Command.OffDuration)
				{
					Status.IsOn = 1;
					Status.FrameCount = 0;
          if(Command.Red || Command.Green || Command.Blue){
            setedLED=true;
					  arduboy.setRGBled(Command.Red * LED_BRIGHTNESS, Command.Green * LED_BRIGHTNESS, Command.Blue * LED_BRIGHTNESS);}
					// memorize which buffer took the led
					CurrentBufferLightUp = (BufferId)i;
					// play also the associated note, and increment it for the next time
					if (IsSoundOn && (Command.BaseNote != 0))
					{
						tone(D3,NoteToFrequency(Command.BaseNote),100);
						Command.BaseNote += Command.NoteIncrement;
					}
				}
			}
			// increase the frame counter
			Status.FrameCount++;
		}
	}
}
