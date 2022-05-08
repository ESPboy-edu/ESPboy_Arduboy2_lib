#ifndef _FX_MANAGER_H_
#define _FX_MANAGER_H_

namespace FXManager
{
  struct FXCommand
  {
    unsigned char OnDuration  :6;
    unsigned char Red     :1;
    unsigned char Green     :1;
    unsigned char Blue      :1;
    unsigned char OffDuration :7;
    unsigned char BaseNote;
    signed char NoteIncrement      :4; // increment can be negative
    unsigned char RepeatTime  :4;
  };

	enum BufferId : unsigned char
	{
		GAME = 0,
		RICK,
		BUFFER_COUNT,
	};

	// a variable to turn on/off the sound FX
	extern bool IsSoundOn;
	
	// set the command in the specified buffer
	void StartFXCommand(BufferId id, FXCommand command);

	// clear the specified buffer and stop the LED and sound. If id equals BUFFER_COUNT, this will clear all the buffers.
	void ClearFXCommand(BufferId id);

	// update that should be called every frame
	void Update();
}

#endif
