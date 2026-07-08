#include <Arduboy2.h>
#include <ArduboyTones.h>
#define I2C_IMPLEMENTATION
#define I2C_FREQUENCY 200000
#define I2C_HANDSHAKE_BUSY_CHECKS 2000
#define I2C_CHECK_CABLE_FLIPPED_CHECKS 1500
#define I2C_CHECK_CABLE_FLIPPED_DEBOUNCE_CHECKS 4
#include <ArduboyI2C.h>
#include "Game.h"
#include "Draw.h"
#include "Font.h"
#include "Menu.h"
#include "FixedMath.h"
#include "Platform.h"

Arduboy2Base arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Sprites sprites;

ARDUBOY_NO_USB

uint8_t Platform::GetInput()
{
  uint8_t result = 0;

  if(arduboy.pressed(A_BUTTON))
  {
    result |= INPUT_A;
  }
  if(arduboy.pressed(B_BUTTON))
  {
    result |= INPUT_B;
  }
  if(arduboy.pressed(UP_BUTTON))
  {
    result |= INPUT_UP;
  }
  if(arduboy.pressed(DOWN_BUTTON))
  {
    result |= INPUT_DOWN;
  }
  if(arduboy.pressed(LEFT_BUTTON))
  {
    result |= INPUT_LEFT;
  }
  if(arduboy.pressed(RIGHT_BUTTON))
  {
    result |= INPUT_RIGHT;
  }

  return result;
}

void Platform::PlaySound(const uint16_t* audioPattern)
{
	sound.tones(audioPattern);
}

void Platform::SetLED(uint8_t r, uint8_t g, uint8_t b)
{
  arduboy.digitalWriteRGB(r ? RGB_ON : RGB_OFF, g ? RGB_ON : RGB_OFF, b ? RGB_ON : RGB_OFF);
}

void Platform::PutPixel(uint8_t x, uint8_t y, uint8_t colour)
{
  arduboy.drawPixel(x, y, colour);
}

// Adpated from https://github.com/a1k0n/arduboy3d/blob/master/draw.cpp
// since the AVR has no barrel shifter, we'll do a progmem lookup
const uint8_t topmask_[] PROGMEM = {
  0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80 };
const uint8_t bottommask_[] PROGMEM = {
  0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

void Platform::DrawVLine(uint8_t x, int8_t y0_, int8_t y1_, uint8_t pattern)
{
  uint8_t *screenptr = arduboy.getBuffer() + x;

  if (y1_ < y0_ || y1_ < 0 || y0_ > 63) return;

  // clip (FIXME; clipping should be handled elsewhere)
  // cast to unsigned after clipping to simplify generated code below
  uint8_t y0 = y0_, y1 = y1_;
  if (y0_ < 0) y0 = 0;
  if (y1_ > 63) y1 = 63;

  uint8_t *page0 = screenptr + ((y0 & 0x38) << 4);
  uint8_t *page1 = screenptr + ((y1 & 0x38) << 4);
  if (page0 == page1)
  {
    uint8_t mask = pgm_read_byte(topmask_ + (y0 & 7))
      & pgm_read_byte(bottommask_ + (y1 & 7));
    *page0 &= ~mask;
    *page0 |= pattern & mask;  // fill y0..y1 in same page in one shot
  }
  else
  {
    uint8_t mask = pgm_read_byte(topmask_ + (y0 & 7));
    *page0 &= ~mask;
    *page0 |= pattern & mask;  // write top 1..8 pixels
    page0 += 128;
    while (page0 != page1)
    {
      *page0 = pattern;  // fill middle 8 pixels at a time
      page0 += 128;
    }
    mask = pgm_read_byte(bottommask_ + (y1 & 7));  // and bottom 1..8 pixels
    *page0 &= ~mask;
    *page0 |= pattern & mask;
  }
}

uint8_t* Platform::GetScreenBuffer()
{
  return arduboy.getBuffer();
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  sprites.drawPlusMask(x, y, bitmap, frame);
}

void Platform::DrawSprite(int16_t x, int16_t y, const uint8_t *bitmap,
  const uint8_t *mask, uint8_t frame, uint8_t mask_frame)
{
  sprites.drawExternalMask(x, y, bitmap, mask, frame, mask_frame);
}

void Platform::DrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
  uint8_t w = pgm_read_byte(&bitmap[0]);
  uint8_t h = pgm_read_byte(&bitmap[1]);
  arduboy.drawBitmap(x, y, bitmap + 2, w, h);
}

void Platform::DrawSolidBitmap(int16_t x, int16_t y, const uint8_t *bitmap)
{
  uint8_t w = pgm_read_byte(&bitmap[0]);
  uint8_t h = pgm_read_byte(&bitmap[1]);
  arduboy.fillRect(x, y, w, h, BLACK);
  arduboy.drawBitmap(x, y, bitmap + 2, w, h);
}

void Platform::FillScreen(uint8_t colour)
{
  arduboy.fillScreen(colour);
}

unsigned long lastTimingSample;
unsigned long lastGoodTickTime;

bool Platform::IsAudioEnabled()
{
	return arduboy.audio.enabled();
}

static void Platform::SetAudioEnabled(bool isEnabled)
{
  if (isEnabled)
  {
    arduboy.audio.on();
  }
  else
  {
    arduboy.audio.off();
  }
}

void Platform::ExpectLoadDelay()
{
	// Resets the timer so that we don't tick multiple times after a level load
	lastTimingSample = millis();
}

#define CENTER_STR(str, csize) (WIDTH / 2 - (sizeof(str) - 1) * csize / 2)

volatile bool controllerReceived = false;
volatile uint8_t controllerInput = 0;

volatile bool controllerRequested = false;
volatile uint8_t targetInput = 0;

static void onReceive()
{
  uint8_t *buffer = I2C::getBuffer();
  controllerInput = buffer[0];
  controllerReceived = true;
}

static void onRequest()
{
  I2C::reply(targetInput);
  controllerRequested = true;
}

static void PlatformNet::Init()
{
  I2C::begin();


  I2C::checkCableFlipped(nullptr, []() {
    Game::menu.DrawHandshaking(PSTR("Please flip the cable"), 4, CENTER_STR("Please flip the cable", 4));
    arduboy.display(CLEAR_BUFFER);
  });

  I2C::Role role = I2C::handshake(nullptr, [] {
    Game::menu.DrawHandshaking(PSTR("Waiting..."), 4, CENTER_STR("Waiting...", 4));
    arduboy.display(CLEAR_BUFFER);
  });
  if (role == I2C::Role::Target)
  {
    I2C::setAddress(I2C::nullAddress);
    I2C::onReceive(onReceive);
    I2C::onRequest(onRequest);
  }
  Game::localPlayerId = role == I2C::Role::Controller ? 1 : 0;
}

static void PlatformNet::RunController(uint8_t localInput, uint8_t &remoteInput)
{
  do {
      I2C::write(I2C::targetAddress, localInput, I2C::Mode::Sync);
  } while (I2C::getError() != I2C::Error::None);
  do {
      I2C::read(I2C::targetAddress, remoteInput);
  } while (I2C::getError() != I2C::Error::None);
}

static void PlatformNet::RunTarget(uint8_t localInput, uint8_t &remoteInput)
{
  targetInput = localInput;

  I2C::setAddress(I2C::targetAddress);
  while (!controllerReceived) { }
  remoteInput = controllerInput;
  controllerReceived = false;

  while (!controllerRequested) { }
  controllerRequested = false;
  I2C::setAddress(I2C::nullAddress);
}
void setup()
{
  arduboy.boot();
  arduboy.flashlight();
  arduboy.systemButtons();
  arduboy.waitNoButtons();
  //arduboy.bootLogo();
  arduboy.setFrameRate(TARGET_FRAMERATE);

  //arduboy.audio.off();


//  SeedRandom((uint16_t) arduboy.generateRandomSeed());
  Game::Init();

  lastTimingSample = millis();
  lastGoodTickTime = millis();
}

void loop()
{
#if DEV_MODE
  if(arduboy.nextFrameDEV())
#else
  if(arduboy.nextFrame())
#endif
  {
	Game::Tick();
	Game::Draw();
  arduboy.display(false);

    //Serial.write(arduboy.getBuffer(), 128 * 64 / 8);

#if DEV_MODE
	// CPU load bar graph
	int load = arduboy.cpuLoad();
	uint8_t* screenPtr = arduboy.getBuffer();

	for(int x = 0; x < load && x < 128; x++)
	{
		screenPtr[x] = (screenPtr[x] & 0xf8) | 3;
	}
	screenPtr[100] = 0;
#endif
  }
}
