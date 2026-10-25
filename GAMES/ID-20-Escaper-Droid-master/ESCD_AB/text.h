#ifndef TEXT_H
#define TEXT_H

#include "font.h"

// TEXT CHARACTERISTICS FOR THE TICKER
// NOT ALL COMBINATIONS ARE POSSIBLE.    
// HERE ARE THE DEFINES YOU CAN PICK FROM
#define TEXT_NOT_SHOWN             0
#define TEXT_STAND_STILL           1       
#define TEXT_SCROLL_LEFT           3       
#define TEXT_SCROLL_RIGHT          5            
#define TEXT_BLINK                 33      
#define TEXT_BLINK_SCROLL_LEFT     35      
#define TEXT_BLINK_SCROLL_RIGHT    37                                                 

//         byte setTicker = 0b00000000;
//                            ||||||||       
//                            ||||||||   
//                            ||||||||
//                            |||||||└->0 1 SHOW TEXT    (0 = false / 1 = true)
//                            ||||||└-->1 2 SCROLL LEFT  (0 = false / 1 = true)
//                            |||||└--->2 4 SCROLL RIGHT (0 = false / 1 = true)
//                            ||||└---->3 8 NOT USED     (0 = false / 1 = true)
//                            |||└----->4 16 NOT USED    (0 = false / 1 = true)
//                            ||└------>5 32 BLINK       (0 = false / 1 = true)
//                            |└------->6 64 NOT USED    (0 = false / 1 = true)
//                            └-------->7 128 USED FOR THE BLICKING EFFECT



unsigned char charBox[120]; 
//unsigned char textBox[30];

PROGMEM const char textMessage[] = // each message max 30 characters including spaces
//"MAXIMUM 30 CHARS IS TILL HERE.\0"
//"---------------|--------------\Ø"
//"//////////////////////////////\0"
  "    CONF SDFX   INFO PLAY\0"         //00
  " BUTTON SCHEME    N<>S  E<>W\0"      //01
  "    MSFX MUSC   SDFX MUTE\0"         //02
  "MADE BY JO3RI T-ARG.GITHUB.IO \0"    //03
  " CONTINUE GAME    NEW   CONT\0"      //04
  "   LEVEL         SCORE:\0"           //05
  "GAME OVER  :<  SCORE:\0"             //06
  "DROID ESCAPED :>  SCORE:\0"          //07
  "ACCESS DENIED: USE WHITE CARD\0"     //08
  "ACCESS DENIED: USE BLACK CARD\0"     //09
  "ALERT ALERT DROID ESCAPING\0"        //10
  "GET THAT DROID NOW\0"                //11
  "DO NOT LET THAT DROID ESCAPE\0"      //12
  "IT STOLE THE DEADSTAR PLANS\0"       //13
  "THIS IS A NO SMOKING FACILITY\0"     //14
  "AREA 51 IS IN LOCKDOWN\0"            //15
  "DROID Q3E3 PLEASE REPORT NOW\0"      //16
  "THE EXIT IS THAT WAY ====>\0";       //17



void loadAndFillMessage(uint8_t indexMessage)
{
  memset(charBox, 0, sizeof(charBox));

  // Find the start of the requested message
  const char* msg = textMessage;
  while (indexMessage > 0)
  {
    while (pgm_read_byte(msg) != 0) msg++;
    msg++;               // skip the '\0'
    indexMessage--;
  }

  // Directly convert characters → font data into charBox
  uint8_t pos = 0;       // position in charBox

  while (pos + 2 < sizeof(charBox))
  {
    char c = pgm_read_byte(msg++);
    if (c == 0) break;   // end of string
    if (c == '.') c = ';';   // '.' glyph lives in the unused ';' font slot (no extra flash)
    else if (c == '-') c = '@';   // '-' glyph lives in the unused '@' font slot

    uint8_t idx = c - FONT_OFFSET;
    if (idx != 240)              // space → leave zeros = frame 0 blank
    {
      byte fr = idx * 3 + 1;     // skip header; frame 0 is blank
      charBox[pos    ] = fr;
      charBox[pos + 1] = fr + 1;
      charBox[pos + 2] = fr + 2;
    }

    pos += 4;            // advance by one glyph (4 bytes) adding a spacing between characters
  }
}

void addNumber(unsigned long number, byte charIndex, byte amountLeadingZeros)
{
  if (amountLeadingZeros > 6) amountLeadingZeros = 6;

  // Count how many digits the number really has
  unsigned long temp = number;
  byte actualDigits = (number == 0) ? 1 : 0;
  while (temp)
  {
    actualDigits++;
    temp /= 10;
  }

  // Total digits to write = max(actualDigits, amountLeadingZeros)
  byte totalDigits = (actualDigits > amountLeadingZeros) ? actualDigits : amountLeadingZeros;

  // Start writing from the rightmost character
  int writePos = (charIndex + totalDigits - 1) * 4;

  for (byte i = 0; i < totalDigits; i++)
  {
    uint8_t digit = number % 10;
    number /= 10;

    if (writePos + 2 < sizeof(charBox))
    {
      byte fr = digit * 3 + 1;
      charBox[writePos    ] = fr;
      charBox[writePos + 1] = fr + 1;
      charBox[writePos + 2] = fr + 2;
    }

    writePos -= 4;   // previous character
  }
}

#endif
