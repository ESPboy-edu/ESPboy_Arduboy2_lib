#include "Ardulem.h"
#include "Input.h"

// variable to save the pressed state of the previous frame
unsigned char PreviousButtonState = 0;
unsigned char CurrentButtonState = 0;

// record the number of frame that the button state stay the same
uint16_t NumberOfFrameSinceLastChange = 0;

/*
 * Call this update function at the end of the loop, so that it can reset internal states.
 */
void Input::Update()
{
	// save the previous state and record the new state
	PreviousButtonState = CurrentButtonState; 
	CurrentButtonState = arduboy.buttonsState();
	
	// compare the old and current button state to check if there's any change
	if (CurrentButtonState == PreviousButtonState)
		NumberOfFrameSinceLastChange++;
	else
		NumberOfFrameSinceLastChange = 0;
}

bool Input::IsJustPressed(unsigned char buttons)
{
	return (((CurrentButtonState & buttons) == buttons) && ((PreviousButtonState & buttons) != buttons));
}

bool Input::IsDown(unsigned char buttons)
{
	return ((CurrentButtonState & buttons) == buttons);
}

bool Input::IsDownModulo(unsigned char buttons, int16_t frameCountFirstModulo, int16_t frameCountSecondModulo)
{
	if ((CurrentButtonState & buttons) == buttons)
	{
		if (NumberOfFrameSinceLastChange >= frameCountFirstModulo)
			return !(NumberOfFrameSinceLastChange % frameCountSecondModulo);
		else
			return (NumberOfFrameSinceLastChange == 0);
	}
	return false;
}

bool Input::IsJustReleased(unsigned char buttons)
{
	return (((CurrentButtonState & buttons) != buttons) && ((PreviousButtonState & buttons) == buttons));
}
