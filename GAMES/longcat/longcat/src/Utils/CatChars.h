#pragma once

#include <stdint.h>
#include <Arduboy2.h>
#include "../Images/Bitmaps.h"

class CatChars{
private:
	CatChars() {};
public:
	static void drawChar(uint8_t x, uint8_t y,bool invert,uint8_t character){
		if(invert)
			character += 64;
		Sprites::drawOverwrite(x, y, CatCharSet, character);
	}
	
	//Null terminated uint8_t string
	static void print(uint8_t x, uint8_t y,bool invert,uint8_t* string){
		uint8_t index = 0;
		while(string[index] != 0){
			drawChar(x + (index*8),y,invert,string[index]);
			index++;
		}
	}
	
	static void print(uint8_t x, uint8_t y,bool invert,const uint8_t* string){
		uint8_t index = 0;
		while(string[index] != 0){
			drawChar(x + (index*8),y,invert,string[index]);
			index++;
		}
	}
	
	//Null terminated char string
	static void print(uint8_t x, uint8_t y,bool invert,char* string){
		uint8_t index = 0;
		while(string[index] != 0){
			drawChar(x + (index*8),y,invert,fromChar(string[index]));
			index++;
		}
	}
	
	static void print(uint8_t x, uint8_t y,bool invert,const char* string){
		uint8_t index = 0;
		while(string[index] != 0){
			drawChar(x + (index*8),y,invert,fromChar(string[index]));
			index++;
		}
	}
	
	static uint8_t length(const char * string){
		uint8_t index = 0;
		while(string[index] != 0){
			index++;
		}
		return index;
	}
	
	static uint8_t length(char * string){
		uint8_t index = 0;
		while(string[index] != 0){
			index++;
		}
		return index;
	}
	
	static uint8_t length(const uint8_t * string){
		uint8_t index = 0;
		while(string[index] != 0){
			index++;
		}
		return index;
	}
	
	static uint8_t length(uint8_t * string){
		uint8_t index = 0;
		while(string[index] != 0){
			index++;
		}
		return index;
	}
	
	static bool inRange(uint8_t value, uint8_t min, uint8_t max){
		return ((value >= min)&&(value <= max));
	}
	
	static uint8_t fromChar(char character){
		uint8_t value = static_cast<uint8_t>(character);
		if(inRange(value,65,90)){
			value -= 65;
			value += 12;
			return value;
		}
		if(inRange(value,97,122)){
			value -= 97;
			value += 12;
			return value;
		}
		if(inRange(value,48,57)){
			value -= 48;
			value += 2;
			return value;
		}
		if(value == 32){
			value = 63;
			return value;
		}
		
		if(character == ':')
			return 38;
		if(character == ';')
			return 42;
		if(character == '.')
			return 40;
		if(character == '_')
			return 41;
		if(character == '-')
			return 39;
		if(character == '<')
			return 1;
		
		return value;
	}
	
	static uint8_t lenBCD(uint8_t value)
	{
		uint8_t index = 0;
		while (value > 0)
		{
			value /= 10;
			index++;
		}
		return index;
	}

	static void printBCD(uint8_t x, uint8_t y, bool invert,uint8_t value)
	{
		uint8_t bcdlength = lenBCD(value);
		uint8_t index = 0;

		if (value == 0)
		{
			CatChars::drawChar(x, y, invert, 2);
			return;
		}

		while (value > 0)
		{
			uint8_t newUnit = value % 10;
			CatChars::drawChar((x + ((bcdlength - 1) * 8)) - (index * 8), y, invert, newUnit + 2);
			value /= 10;
			index++;
		}
	}

};
