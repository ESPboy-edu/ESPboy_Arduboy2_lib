#pragma once

#include <stdint.h>

// Converts a cat key into a unsigned long
#define CatKeyLength (sizeof(unsigned long) * 2)

struct CatKey
{
	uint8_t key[CatKeyLength + 1];
};

class RandKey
{
private:
	RandKey(){};

public:
	static CatKey toCatKey(unsigned long value)
	{
		CatKey catkey;
		for (uint8_t i = 0; i < CatKeyLength; i++)
		{
			unsigned long mask = (static_cast<unsigned long>(0xf) << (i * 4));
			catkey.key[i] = static_cast<uint8_t>(((value & mask) >> (i * 4)));
		}
		return catkey;
	}

	static unsigned long fromCatKey(CatKey catkey)
	{
		unsigned long key;
		for (uint8_t i = 0; i < CatKeyLength; i++)
		{
			key += static_cast<unsigned long>(catkey.key[i]) << (i * 4);
		}
		return key;
	}
};