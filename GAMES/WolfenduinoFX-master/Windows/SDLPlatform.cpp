// SDL platform
#include <stdio.h>
#include "SDLPlatform.h"
#include "Engine.h"
#include "Generated/Data_Audio.h"
#include "lodepng.h"
#include "Generated/fxdata.h"

#define TONES_END 0x8000

SDLPlatform Platform;
uint8_t* diskContents = 0;
long diskContentsLength = 0;

constexpr int audioSampleRate = 48000;

const uint16_t* currentAudioPattern = nullptr;
int currentPatternBufferPos = 0;
bool isAudioEnabled = true;

bool isRecording = false;
int currentRecordingFrame = 0;

void Play(const uint16_t* pattern)
{
	currentAudioPattern = pattern;
	currentPatternBufferPos = 0;
}

void FillAudioBuffer(void* udata, uint8_t* stream, int len)
{
	int feedPos = 0;

	static int waveSamplesLeft = 0;
	static int noteSamplesLeft = 0;
	static int frequency = 0;
	static bool high = false;

	if (!isAudioEnabled)
	{
		while (feedPos < len)
		{
			stream[feedPos++] = 0;
		}
		return;
	}

	while (feedPos < len)
	{
		if (currentAudioPattern != nullptr)
		{
			if (noteSamplesLeft == 0)
			{
				frequency = currentAudioPattern[currentPatternBufferPos];
				uint16_t duration = currentAudioPattern[currentPatternBufferPos + 1];

				noteSamplesLeft = (audioSampleRate * duration) / 1024;

				waveSamplesLeft = frequency > 0 ? (audioSampleRate / frequency) / 2 : noteSamplesLeft;

				currentPatternBufferPos += 2;
				if (currentAudioPattern[currentPatternBufferPos] == TONES_END)
				{
					currentAudioPattern = nullptr;
				}
			}
		}

		if (frequency == 0)
		{
			while (feedPos < len && (!currentAudioPattern || noteSamplesLeft > 0))
			{
				stream[feedPos++] = 0;

				if (noteSamplesLeft > 0)
					noteSamplesLeft--;
			}
		}
		else
		{
			while (feedPos < len && waveSamplesLeft > 0 && noteSamplesLeft > 0)
			{
				int volume = 32;
				//stream[feedPos++] = high ? 128 + volume : 128 - volume;
				stream[feedPos++] = high ? volume : 0;
				waveSamplesLeft--;
				noteSamplesLeft--;
			}

			if (waveSamplesLeft == 0)
			{
				high = !high;
				waveSamplesLeft = (audioSampleRate / frequency) / 2;
			}
		}

	}
}

void SDLPlatform::init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer( DISPLAYWIDTH * ZOOM_SCALE, DISPLAYHEIGHT * ZOOM_SCALE, SDL_WINDOW_RESIZABLE, &m_appWindow, &m_appRenderer );
	SDL_RenderSetLogicalSize(m_appRenderer, DISPLAYWIDTH, DISPLAYHEIGHT);
	
	m_screenSurface = SDL_CreateRGBSurface(0, DISPLAYWIDTH, DISPLAYHEIGHT, 32, 
											0x000000ff,
											0x0000ff00, 
											0x00ff0000, 
											0xff000000
											);
	m_screenTexture = SDL_CreateTexture(m_appRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_screenSurface->w, m_screenSurface->h);

	SDL_AudioSpec wanted;
	wanted.freq = audioSampleRate;
	wanted.format = AUDIO_U8;
	wanted.channels = 1;
	wanted.samples = 4096;
	wanted.callback = FillAudioBuffer;

	if (SDL_OpenAudio(&wanted, NULL) < 0) {
		printf("Error: %s\n", SDL_GetError());
	}
	SDL_PauseAudio(0);

	m_isRunning = true;
}

typedef struct
{
	SDL_Scancode key;
	uint8_t mask;
} KeyMap;

#define NUM_KEY_MAPPINGS sizeof(KeyMappings) / sizeof(KeyMap)

KeyMap KeyMappings[] =
{
	{ SDL_SCANCODE_LEFT, Input_Dpad_Left },
	{ SDL_SCANCODE_RIGHT, Input_Dpad_Right },
	{ SDL_SCANCODE_UP, Input_Dpad_Up },
	{ SDL_SCANCODE_DOWN, Input_Dpad_Down },
	{ SDL_SCANCODE_X, Input_Btn_B },
	{ SDL_SCANCODE_Z, Input_Btn_A },

	{ SDL_SCANCODE_LSHIFT, Input_Btn_A },
	{ SDL_SCANCODE_LCTRL, Input_Btn_B },
};

void SDLPlatform::updateInputState()
{
	inputState = 0;

	const uint8_t* keyStates = SDL_GetKeyboardState(NULL);

	for (unsigned int n = 0; n < NUM_KEY_MAPPINGS; n++)
	{
		if (keyStates[KeyMappings[n].key])
		{
			inputState |= KeyMappings[n].mask;
		}
	}
}

void SDLPlatform::run()
{
	m_isRunning = true;
	
	engine.init();
	
	while(m_isRunning)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type) 
			{
				case SDL_QUIT:
				m_isRunning = false;
				break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_F5:
						{
							lodepng_encode32_file("screenshot.png", (unsigned char*)(m_screenSurface->pixels), m_screenSurface->w, m_screenSurface->h);
							break;
						}
						case SDLK_F11:
							isRecording = !isRecording;
							break;
					}
				break;
			}

		}

		updateInputState();
		isAudioEnabled = !m_isMuted;

		SDL_SetRenderDrawColor ( m_appRenderer, 206, 221, 231, 255 );
		SDL_RenderClear ( m_appRenderer );

		engine.update();
		engine.draw();

		SDL_UpdateTexture(m_screenTexture, NULL, m_screenSurface->pixels, m_screenSurface->pitch);
		SDL_RenderCopy(m_appRenderer, m_screenTexture, NULL, NULL);
		SDL_RenderPresent(m_appRenderer);

		if (isRecording)
		{
			char filename[50];
			snprintf(filename, 50, "capture/frame%03d.png", currentRecordingFrame);
			lodepng_encode32_file(filename, (unsigned char*)(m_screenSurface->pixels), m_screenSurface->w, m_screenSurface->h);
			currentRecordingFrame++;
		}

		SDL_Delay(1000 / TARGET_FRAMERATE);
	}

	SDL_Quit();
}

bool LoadDiskContents()
{
	FILE* fs;

	if (!fopen_s(&fs, "fxdata.bin", "rb"))
	{
		fseek(fs, 0, SEEK_END);
		diskContentsLength = ftell(fs);
		fseek(fs, 0, SEEK_SET);

		diskContents = new uint8_t[diskContentsLength];
		fread(diskContents, 1, diskContentsLength, fs);

		fclose(fs);
		return true;
	}
	else
	{
		printf("Error opening fxdata.bin\n");
		return false;
	}
}

int main(int, char**)
{
	if (!LoadDiskContents())
	{
		return 1;
	}

	Platform.init();

	Platform.run();

	return 0;
}

void SDLPlatform::drawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    *(Uint32 *)p = pixel;
}

uint8_t paletteColours[] =
{
#if defined(EMULATE_GAMEBUINO)
	0, 0, 0,
	206, 221, 231,
#elif defined(EMULATE_ARDUBOY)
	0, 0, 0,
	255, 255, 255,
#else
	0, 0, 0,
	255, 255, 255,
	145, 145, 170,
	182, 182, 170
#endif
};

void SDLPlatform::drawPixel(uint8_t x, uint8_t y, uint8_t colour)
{
	if (x >= DISPLAYWIDTH || y >= DISPLAYHEIGHT)
	{
		return;
	}

	Uint32 col = SDL_MapRGBA(m_screenSurface->format, paletteColours[colour * 3], paletteColours[colour * 3 + 1], paletteColours[colour * 3 + 2], 255);
	drawPixel(m_screenSurface, x, y, col);

	/*if(!colour)
	{
		Uint32 black = SDL_MapRGBA(m_screenSurface->format, 0, 0, 0, 255);
		drawPixel(m_screenSurface, x, y, black);
	}
	else
	{
		Uint32 white = SDL_MapRGBA(m_screenSurface->format, 206, 221, 231, 255);
		drawPixel(m_screenSurface, x, y, white);
	}*/
}

void clearDisplay(uint8_t colour)
{
	for(int y = 0; y < DISPLAYHEIGHT; y++)
	{
		for(int x = 0; x < DISPLAYWIDTH; x++)
		{
			drawPixel(x, y, colour);
		}
	}
}

void diskRead(uint24_t address, uint8_t* buffer, int length)
{
	for (int n = 0; n < length; n++)
	{
		if (address + n < diskContentsLength)
		{
			buffer[n] = diskContents[address + n];
		}
	}
}

void SDLPlatform::playSound(uint8_t id)
{
//	memcpy(audioPatternBuffer, diskContents + Data_audio + Data_AudioPatterns[id], Data_AudioPatterns[id + 1] - Data_AudioPatterns[id]);
	Play((const uint16_t*) (diskContents + Data_audio + Data_AudioPatterns[id]));
}

void writeSaveFile(uint8_t* buffer, int length)
{
	FILE* fs;
	
	if (!fopen_s(&fs, "save.sav", "wb"))
	{
		fwrite(buffer, 1, length, fs);
		fclose(fs);
	}
}

bool readSaveFile(uint8_t* buffer, int length)
{
	FILE* fs;

	if (!fopen_s(&fs, "save.sav", "rb"))
	{
		fread(buffer, 1, length, fs);
		fclose(fs);
		return true;
	}

	return false;
}
