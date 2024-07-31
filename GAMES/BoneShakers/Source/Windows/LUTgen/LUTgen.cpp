#include <stdio.h>
#include "Defines.h"
#include "FixedMath.h"

int16_t gen_viewFwdTable[DISPLAY_HEIGHT];
int16_t gen_viewXTable[DISPLAY_WIDTH];
int16_t gen_sinTable[FIXED_ANGLE_MAX];

void GenerateLUT()
{
	for (int n = 0; n < FIXED_ANGLE_MAX; n++)
	{
		gen_sinTable[n] = FLOAT_TO_FIXED(sin(FIXED_ANGLE_TO_RADIANS(n)));
	}

	for (int y = DISPLAY_HEIGHT / 3 + 1; y < DISPLAY_HEIGHT; y++)
	{
		int16_t viewY = FLOAT_TO_FIXED(((float)y - (DISPLAY_HEIGHT / 5)) / (4 * DISPLAY_HEIGHT / 5));
		gen_viewFwdTable[y] = (FIXED_ONE * CAMERA_HEIGHT / viewY);
	}

	for (int x = 0; x < DISPLAY_WIDTH; x++)
	{
		gen_viewXTable[x] = FLOAT_TO_FIXED(((float)x - (DISPLAY_WIDTH / 2)) / (DISPLAY_WIDTH / 1.5));
	}
}

int main(int argc, char* argv[])
{
	GenerateLUT();

	FILE* fs;
	fopen_s(&fs, "LUT.h", "w");

	fprintf(fs, "const int16_t viewFwdTable[DISPLAY_HEIGHT] PROGMEM = {\n\t");
	for (int n = 0; n < DISPLAY_HEIGHT; n++)
	{
		fprintf(fs, "%d", gen_viewFwdTable[n]);
		if (n != DISPLAY_HEIGHT - 1)
		{
			fprintf(fs, ",");
		}
	}
	fprintf(fs, "\n};\n\n");

	fprintf(fs, "const int16_t viewXTable[DISPLAY_WIDTH] PROGMEM = {\n\t");
	for (int n = 0; n < DISPLAY_WIDTH; n++)
	{
		fprintf(fs, "%d", gen_viewXTable[n]);
		if (n != DISPLAY_WIDTH - 1)
		{
			fprintf(fs, ",");
		}
	}
	fprintf(fs, "\n};\n\n");

	fprintf(fs, "const int16_t sinTable[FIXED_ANGLE_MAX] PROGMEM = {\n\t");
	for (int n = 0; n < FIXED_ANGLE_MAX; n++)
	{
		fprintf(fs, "%d", gen_sinTable[n]);
		if (n != FIXED_ANGLE_MAX - 1)
		{
			fprintf(fs, ",");
		}
	}
	fprintf(fs, "\n};\n\n");

	fclose(fs);

	return 0;
}