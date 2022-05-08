#include "RickArdurous.h"
#include "Input.h"

// variable to save the pressed state of the previous frame
unsigned char PreviousButtonState = 0;
unsigned char CurrentButtonState = 0;

/*
 * Call this update function at the end of the loop, so that it can reset internal states.
 */
void Input::Update()
{
	// save the previous state and record the new state
	PreviousButtonState = CurrentButtonState; 
	CurrentButtonState = arduboy.buttonsState();
}

bool Input::IsJustPressed(unsigned char buttons)
{
	return (((CurrentButtonState & buttons) == buttons) && ((PreviousButtonState & buttons) != buttons));
}

bool Input::IsDown(unsigned char buttons)
{
	return ((CurrentButtonState & buttons) == buttons);
}

bool Input::IsJustReleased(unsigned char buttons)
{
	return (((CurrentButtonState & buttons) != buttons) && ((PreviousButtonState & buttons) == buttons));
}
