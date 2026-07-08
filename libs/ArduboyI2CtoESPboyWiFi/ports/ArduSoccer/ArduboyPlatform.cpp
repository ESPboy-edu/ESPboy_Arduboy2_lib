#define I2C_IMPLEMENTATION
#define I2C_PLATFORM I2C_PLATFORM_FX_C
#include <ArduboyI2C.h>
#include "ArduboyPlatform.h"
#include "Engine.h"

#define CENTER_STR(str, csize) (WIDTH / 2 - (sizeof(str) - 1) * csize / 2)

//#include "Generated/Data_Audio.h"

ArduboyPlatform Platform;

volatile bool controllerReceived = false;
volatile bool controllerRequested = false;

void onReceive()
{
	uint8_t *buffer = I2C::getBuffer();
	Platform.lastInputState[REMOTE_PLAYER] = Platform.inputState[REMOTE_PLAYER];
	Platform.inputState[REMOTE_PLAYER] = buffer[0];
	controllerReceived = true;
}

void onRequest()
{
	I2C::reply(Platform.inputState[LOCAL_PLAYER]);
	controllerRequested = true;
}

void ArduboyPlatform::updateInput()
{
	lastInputState[LOCAL_PLAYER] = inputState[LOCAL_PLAYER];
	inputState[LOCAL_PLAYER] = 0;

	if(arduboy.pressed(A_BUTTON))
	{
		inputState[LOCAL_PLAYER] |= Input_Btn_A;
	}
	if(arduboy.pressed(B_BUTTON))
	{
		inputState[LOCAL_PLAYER] |= Input_Btn_B;
	}
	if(arduboy.pressed(UP_BUTTON))
	{
		inputState[LOCAL_PLAYER] |= Input_Dpad_Up;
	}
	if(arduboy.pressed(DOWN_BUTTON))
	{
		inputState[LOCAL_PLAYER] |= Input_Dpad_Down;
	}
	if(arduboy.pressed(LEFT_BUTTON))
	{
		inputState[LOCAL_PLAYER] |= Input_Dpad_Left;
	}
	if(arduboy.pressed(RIGHT_BUTTON))
	{
		inputState[LOCAL_PLAYER] |= Input_Dpad_Right;
	}
}

void ArduboyPlatform::update()
{
    if(arduboy.audio.enabled() != !m_isMuted)
    {
		if(m_isMuted)
		{
			arduboy.audio.off();
		}
		else
		{
			arduboy.audio.on();
		}
    }
	updateInput();
	if (multiplayerConnected)
	{
		if (role == I2C::Role::Controller)
		{
			do {
				I2C::write(I2C::targetAddress, inputState[LOCAL_PLAYER], I2C::Mode::Sync);
			} while (I2C::getError() != I2C::Error::None);

			lastInputState[REMOTE_PLAYER] = inputState[REMOTE_PLAYER];
			do {
				I2C::read(I2C::targetAddress, inputState[REMOTE_PLAYER]);
			} while (I2C::getError() != I2C::Error::None);
		}
		else
		{
			I2C::setAddress(I2C::targetAddress);
			while (!controllerReceived) { }
			controllerReceived = false;
			while (!controllerRequested) { }
			controllerRequested = false;
			I2C::setAddress(I2C::nullAddress);
		}
	}
}

void ArduboyPlatform::disconnectMultiplayer()
{
	multiplayerConnected = false;
}

bool ArduboyPlatform::connectMultiplayer()
{
	arduboy.display(CLEAR_BUFFER);

	I2C::checkCableFlipped([]() {
		engine.renderer.drawText(smallFont, PSTR("PLEASE FLIP THE CABLE"), CENTER_STR("PLEASE FLIP THE CABLE", 6), 30, 1);
		arduboy.display(CLEAR_BUFFER);
	});

	engine.renderer.drawText(smallFont, PSTR("WAITING FOR PLAYERS"), CENTER_STR("WAITING FOR PLAYERS", 6), 30, 1);
	arduboy.display(CLEAR_BUFFER);

	role = I2C::handshake();
	if (role == I2C::Role::Target)
	{
		I2C::setAddress(I2C::nullAddress);
		I2C::onReceive(onReceive);
		I2C::onRequest(onRequest);
	}

	multiplayerConnected = true;

	return role == I2C::Role::Controller;
}
