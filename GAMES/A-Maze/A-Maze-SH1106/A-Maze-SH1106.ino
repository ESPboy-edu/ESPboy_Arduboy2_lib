/* A-Maze
 by Alojz Jakob <http://jakobdesign.com>

 ************* A-MAZE ***********
 *     Maze game for Arduino    *
 ********************************
 
*/

//#include <SPI.h>
//#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>


//#define OLED_RESET 4
#define OLED_RESET 1
Adafruit_SH1106 display(OLED_RESET);


// 'title', 128x64px
const unsigned char title1 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x3e, 0x07, 0xe0, 0x07, 0xff, 0xc0, 
  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x03, 0x10, 0x00, 0xe0, 0xa0, 0x3c, 0x0c, 0x00, 0x70, 
  0x00, 0x00, 0x7b, 0xc0, 0x00, 0x00, 0x20, 0x02, 0x18, 0x00, 0x9d, 0x44, 0x06, 0x0a, 0x44, 0x10, 
  0x00, 0x01, 0x80, 0x60, 0x00, 0x00, 0xf8, 0x05, 0x88, 0x00, 0xba, 0x90, 0x01, 0x1d, 0x10, 0x08, 
  0x00, 0x02, 0xf4, 0x10, 0x00, 0x01, 0xac, 0x0b, 0x08, 0x00, 0x95, 0x42, 0x03, 0x2a, 0x00, 0x08, 
  0x00, 0x05, 0xaa, 0x88, 0x00, 0x01, 0x66, 0x16, 0x88, 0x00, 0xc0, 0x00, 0x02, 0x79, 0x40, 0x08, 
  0x00, 0x0b, 0xd4, 0x04, 0x00, 0x02, 0xd3, 0x2d, 0x08, 0x0f, 0xc0, 0x00, 0x04, 0x54, 0x00, 0x10, 
  0x00, 0x16, 0xaa, 0xa2, 0x00, 0x02, 0xa9, 0xda, 0x88, 0x78, 0x7f, 0xfc, 0x08, 0xa8, 0x00, 0x20, 
  0x00, 0x2f, 0x40, 0x09, 0x00, 0x02, 0x54, 0x15, 0x48, 0x80, 0x10, 0x04, 0x08, 0xd5, 0x01, 0xc0, 
  0x00, 0x2a, 0xfc, 0x20, 0x80, 0x06, 0xa8, 0x0a, 0x09, 0x28, 0x08, 0x08, 0x11, 0x28, 0x7f, 0x00, 
  0x00, 0x5d, 0x87, 0x00, 0xc0, 0x04, 0x52, 0x00, 0x8a, 0xd2, 0x04, 0x10, 0x21, 0x44, 0x40, 0x00, 
  0x00, 0x49, 0x01, 0x80, 0x40, 0x05, 0x28, 0x04, 0x0d, 0x80, 0x06, 0x20, 0x62, 0xa0, 0x40, 0x00, 
  0x00, 0xa1, 0x00, 0x80, 0x40, 0x0c, 0x00, 0x00, 0x0b, 0x3c, 0x02, 0x50, 0x42, 0x00, 0x3f, 0x80, 
  0x00, 0x89, 0x00, 0xc0, 0x40, 0x08, 0x0a, 0x01, 0x12, 0xe6, 0x01, 0xa0, 0x84, 0x08, 0x01, 0xc0, 
  0x00, 0x81, 0x00, 0x40, 0xff, 0xfc, 0x00, 0x00, 0x25, 0x83, 0x81, 0x70, 0x84, 0x20, 0x50, 0xc0, 
  0x00, 0xa1, 0x00, 0x41, 0x80, 0x06, 0x02, 0x1c, 0x69, 0x00, 0x82, 0xa1, 0x88, 0x00, 0x00, 0xc0, 
  0x00, 0x84, 0x80, 0xd1, 0x75, 0x42, 0x00, 0x36, 0x45, 0x01, 0x85, 0x41, 0x08, 0x10, 0x00, 0xc0, 
  0x01, 0x00, 0x80, 0x81, 0x6a, 0x12, 0x00, 0x6a, 0x91, 0x01, 0x0a, 0x81, 0x10, 0x00, 0x01, 0x80, 
  0x01, 0x00, 0x41, 0xa1, 0x55, 0x46, 0x18, 0x47, 0x05, 0x03, 0x11, 0x49, 0x10, 0x07, 0xff, 0x00, 
  0x01, 0x00, 0x3e, 0x00, 0x80, 0x0c, 0x1e, 0x85, 0x11, 0x82, 0x34, 0x01, 0x10, 0x04, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0xa0, 0x7f, 0xf8, 0x13, 0x06, 0x00, 0xc2, 0x20, 0x81, 0x90, 0x04, 0x00, 0x00, 
  0x01, 0x00, 0x02, 0x00, 0x20, 0x20, 0x20, 0x06, 0x00, 0x34, 0xa2, 0x00, 0x70, 0x04, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x04, 0x00, 0x18, 0x20, 0x00, 0x1f, 0x87, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x20, 0x20, 0x40, 0x04, 0x03, 0x82, 0x30, 0x00, 0x00, 0xc1, 0xf0, 0x00, 
  0x01, 0x00, 0x0c, 0x00, 0x20, 0x20, 0x40, 0x08, 0x04, 0xc0, 0x18, 0x00, 0x00, 0x44, 0x18, 0x00, 
  0x01, 0x00, 0x3f, 0x00, 0x20, 0x60, 0x80, 0x08, 0x08, 0x30, 0x0c, 0x00, 0x00, 0x40, 0x08, 0x00, 
  0x01, 0x00, 0x21, 0x80, 0x20, 0x41, 0x80, 0x10, 0x10, 0x10, 0x06, 0x00, 0x00, 0x40, 0x08, 0x00, 
  0x01, 0x80, 0x60, 0x40, 0x20, 0x41, 0x00, 0x20, 0x30, 0x08, 0x01, 0x00, 0x00, 0x40, 0x30, 0x00, 
  0x00, 0xc0, 0x40, 0x60, 0x40, 0x41, 0x00, 0x20, 0x50, 0x04, 0x00, 0x80, 0x00, 0x40, 0x60, 0x00, 
  0x00, 0x60, 0x80, 0x20, 0x80, 0x42, 0x00, 0x40, 0x90, 0x03, 0x80, 0xf8, 0x00, 0x7f, 0xc0, 0x00, 
  0x00, 0x3b, 0x80, 0x31, 0x80, 0x62, 0x00, 0x41, 0x20, 0x00, 0xc3, 0x0f, 0x80, 0x80, 0x00, 0x00, 
  0x00, 0x0e, 0x00, 0x1e, 0x00, 0x1c, 0x00, 0x42, 0x20, 0x00, 0x7e, 0x00, 0xff, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x21, 0xcc, 0x38, 0x17, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcb, 0x23, 0x2c, 0x64, 0x10, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0xa3, 0x2c, 0x64, 0x11, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc9, 0xc3, 0xec, 0x65, 0x93, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x83, 0x2c, 0x65, 0x96, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x83, 0x2f, 0x38, 0xe7, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
 // 'title2', 128x64px
const unsigned char title2 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xa8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x14, 0x05, 0x40, 0x05, 0x55, 0x40, 
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0xa0, 0xa0, 0x28, 0x08, 0x00, 0x20, 
  0x00, 0x00, 0x51, 0x40, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x15, 0x44, 0x04, 0x00, 0x44, 0x10, 
  0x00, 0x00, 0x80, 0x20, 0x00, 0x00, 0xa8, 0x00, 0x88, 0x00, 0xaa, 0x80, 0x00, 0x08, 0x00, 0x08, 
  0x00, 0x00, 0x54, 0x10, 0x00, 0x01, 0x04, 0x01, 0x00, 0x00, 0x15, 0x40, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xaa, 0x88, 0x00, 0x00, 0x22, 0x02, 0x88, 0x00, 0x80, 0x00, 0x02, 0x28, 0x00, 0x08, 
  0x00, 0x01, 0x54, 0x04, 0x00, 0x00, 0x51, 0x05, 0x00, 0x05, 0x40, 0x00, 0x04, 0x54, 0x00, 0x10, 
  0x00, 0x02, 0xaa, 0xa2, 0x00, 0x02, 0xa8, 0x8a, 0x88, 0x28, 0x2a, 0xa8, 0x08, 0xa8, 0x00, 0x20, 
  0x00, 0x05, 0x40, 0x01, 0x00, 0x00, 0x54, 0x15, 0x40, 0x00, 0x10, 0x04, 0x00, 0x55, 0x01, 0x40, 
  0x00, 0x2a, 0xa8, 0x20, 0x80, 0x02, 0xa8, 0x0a, 0x08, 0x28, 0x08, 0x08, 0x00, 0x28, 0x2a, 0x00, 
  0x00, 0x55, 0x05, 0x00, 0x40, 0x04, 0x50, 0x00, 0x00, 0x50, 0x04, 0x10, 0x01, 0x44, 0x40, 0x00, 
  0x00, 0x08, 0x00, 0x80, 0x00, 0x00, 0x28, 0x00, 0x08, 0x80, 0x02, 0x20, 0x22, 0xa0, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x01, 0x14, 0x00, 0x50, 0x40, 0x00, 0x15, 0x00, 
  0x00, 0x88, 0x00, 0x80, 0x00, 0x08, 0x0a, 0x00, 0x02, 0xa2, 0x00, 0xa0, 0x80, 0x08, 0x00, 0x80, 
  0x00, 0x01, 0x00, 0x40, 0x55, 0x54, 0x00, 0x00, 0x05, 0x01, 0x01, 0x50, 0x04, 0x00, 0x50, 0x40, 
  0x00, 0xa0, 0x00, 0x00, 0x80, 0x02, 0x02, 0x08, 0x28, 0x00, 0x82, 0xa0, 0x88, 0x00, 0x00, 0x80, 
  0x00, 0x04, 0x00, 0x51, 0x55, 0x40, 0x00, 0x14, 0x45, 0x01, 0x05, 0x41, 0x00, 0x10, 0x00, 0x40, 
  0x00, 0x00, 0x80, 0x80, 0x2a, 0x02, 0x00, 0x2a, 0x80, 0x00, 0x0a, 0x80, 0x00, 0x00, 0x00, 0x80, 
  0x01, 0x00, 0x41, 0x01, 0x55, 0x44, 0x10, 0x45, 0x05, 0x01, 0x11, 0x41, 0x10, 0x05, 0x55, 0x00, 
  0x00, 0x00, 0x2a, 0x00, 0x80, 0x08, 0x0a, 0x80, 0x00, 0x82, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x55, 0x50, 0x11, 0x04, 0x00, 0x40, 0x00, 0x01, 0x10, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x20, 0x20, 0x20, 0x02, 0x00, 0x20, 0xa2, 0x00, 0x20, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x15, 0x05, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x02, 0x82, 0x20, 0x00, 0x00, 0x80, 0xa0, 0x00, 
  0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x40, 0x00, 0x04, 0x40, 0x10, 0x00, 0x00, 0x44, 0x10, 0x00, 
  0x00, 0x00, 0x2a, 0x00, 0x20, 0x20, 0x80, 0x08, 0x08, 0x20, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 
  0x01, 0x00, 0x01, 0x00, 0x00, 0x41, 0x00, 0x10, 0x10, 0x10, 0x04, 0x00, 0x00, 0x40, 0x00, 0x00, 
  0x00, 0x80, 0x20, 0x00, 0x20, 0x00, 0x00, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 
  0x00, 0x40, 0x40, 0x40, 0x40, 0x41, 0x00, 0x00, 0x50, 0x04, 0x00, 0x00, 0x00, 0x40, 0x40, 0x00, 
  0x00, 0x20, 0x80, 0x20, 0x80, 0x02, 0x00, 0x00, 0x80, 0x02, 0x80, 0xa8, 0x00, 0x2a, 0x80, 0x00, 
  0x00, 0x11, 0x00, 0x11, 0x00, 0x40, 0x00, 0x41, 0x00, 0x00, 0x41, 0x05, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0a, 0x00, 0x0a, 0x00, 0x08, 0x00, 0x02, 0x20, 0x00, 0x2a, 0x00, 0xaa, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x01, 0x44, 0x10, 0x15, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8a, 0x22, 0x28, 0x20, 0x00, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x01, 0x04, 0x44, 0x11, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x82, 0xa8, 0x20, 0x82, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x04, 0x45, 0x14, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x82, 0x2a, 0x28, 0xa2, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

 // 'title3', 128x64px
const unsigned char title3 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x01, 0x11, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0xa0, 0x08, 0x08, 0x00, 0x00, 
  0x00, 0x00, 0x11, 0x40, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x15, 0x44, 0x00, 0x00, 0x44, 0x00, 
  0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x88, 0x00, 0x80, 0x00, 0x2a, 0x80, 0x00, 0x08, 0x00, 0x08, 
  0x00, 0x00, 0x54, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x15, 0x40, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xaa, 0x80, 0x00, 0x00, 0x20, 0x02, 0x88, 0x00, 0x80, 0x00, 0x00, 0x28, 0x00, 0x08, 
  0x00, 0x01, 0x54, 0x04, 0x00, 0x00, 0x51, 0x05, 0x00, 0x05, 0x40, 0x00, 0x00, 0x54, 0x00, 0x00, 
  0x00, 0x02, 0xaa, 0xa0, 0x00, 0x02, 0xa8, 0x0a, 0x80, 0x28, 0x22, 0x20, 0x08, 0xa8, 0x00, 0x00, 
  0x00, 0x05, 0x40, 0x01, 0x00, 0x00, 0x54, 0x15, 0x40, 0x00, 0x10, 0x04, 0x00, 0x55, 0x01, 0x00, 
  0x00, 0x0a, 0xa8, 0x20, 0x00, 0x02, 0xa8, 0x0a, 0x08, 0x28, 0x08, 0x00, 0x00, 0x28, 0x20, 0x00, 
  0x00, 0x15, 0x05, 0x00, 0x40, 0x00, 0x50, 0x00, 0x00, 0x50, 0x04, 0x00, 0x01, 0x44, 0x40, 0x00, 
  0x00, 0x08, 0x00, 0x80, 0x00, 0x00, 0x28, 0x00, 0x00, 0x80, 0x02, 0x20, 0x22, 0xa0, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x14, 0x00, 0x10, 0x00, 0x00, 0x11, 0x00, 
  0x00, 0x08, 0x00, 0x80, 0x00, 0x00, 0x0a, 0x00, 0x02, 0xa0, 0x00, 0x20, 0x00, 0x08, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x40, 0x44, 0x00, 0x00, 0x00, 0x05, 0x01, 0x00, 0x50, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x28, 0x00, 0x02, 0xa0, 0x88, 0x00, 0x00, 0x00, 
  0x00, 0x04, 0x00, 0x51, 0x55, 0x40, 0x00, 0x10, 0x04, 0x00, 0x01, 0x40, 0x00, 0x10, 0x00, 0x40, 
  0x00, 0x00, 0x80, 0x80, 0x2a, 0x02, 0x00, 0x02, 0x00, 0x00, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x41, 0x00, 0x55, 0x40, 0x00, 0x41, 0x05, 0x01, 0x01, 0x40, 0x10, 0x04, 0x11, 0x00, 
  0x00, 0x00, 0x2a, 0x00, 0x80, 0x08, 0x00, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x00, 0x20, 0x20, 0x02, 0x00, 0x00, 0x82, 0x00, 0x20, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x82, 0x20, 0x00, 0x00, 0x80, 0x20, 0x00, 
  0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x20, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x20, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0a, 0x00, 0x08, 0x00, 0x08, 0x00, 0x02, 0x20, 0x00, 0x22, 0x00, 0x82, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x01, 0x44, 0x10, 0x15, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8a, 0x22, 0x28, 0x20, 0x00, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x01, 0x04, 0x44, 0x11, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x82, 0xa8, 0x20, 0x82, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x04, 0x45, 0x14, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x82, 0x2a, 0x28, 0xa2, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

 // 'title4', 128x64px
const unsigned char title4 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x01, 0x11, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x00, 0x08, 0x08, 0x00, 0x00, 
  0x00, 0x00, 0x11, 0x40, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 
  0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x08, 0x00, 0x08, 
  0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x80, 0x00, 0x00, 0x20, 0x00, 0x08, 
  0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x28, 0x22, 0x20, 0x08, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 
  0x00, 0x00, 0x28, 0x00, 0x00, 0x02, 0x00, 0x00, 0x08, 0x20, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 
  0x00, 0x10, 0x05, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x40, 0x04, 0x00, 0x01, 0x04, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x20, 0x22, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x11, 0x00, 
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x40, 0x44, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x00, 0x02, 0x00, 0x88, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x40, 
  0x00, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x05, 0x01, 0x01, 0x00, 0x10, 0x04, 0x11, 0x00, 
  0x00, 0x00, 0x22, 0x00, 0x80, 0x08, 0x00, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x00, 0x20, 0x20, 0x02, 0x00, 0x00, 0x82, 0x00, 0x20, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x82, 0x20, 0x00, 0x00, 0x80, 0x20, 0x00, 
  0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x80, 0x20, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x20, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x20, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0a, 0x00, 0x08, 0x00, 0x08, 0x00, 0x02, 0x20, 0x00, 0x22, 0x00, 0x82, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x10, 0x11, 0x40, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x22, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x44, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x82, 0x22, 0x20, 0x22, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



const int8_t button1Pin = 9; //LEFT
const int8_t button2Pin = 8; //RIGHT
const int8_t button3Pin = 7; //UP
const int8_t button4Pin = 6; //DOWN
const int8_t button5Pin = 5; //A
const int8_t button6Pin = 4; //B
const int8_t button7Pin = 2; //MENU

const int8_t sound = 3; 
bool sound_enabled=true;
const int8_t ledPin = 10; 

int8_t button1State = 0;
int8_t button2State = 0;
int8_t button3State = 0;
int8_t button4State = 0;
int8_t button5State = 0;
int8_t button6State = 0;
int8_t button7State = 0;


#define ACTIVATED LOW

#define DELAYMULTIPLIER  0




int8_t posx=0, posy=2; // Where you are in the Maze

int8_t illuminatedRow=0;
int8_t blinkPlayer=1;
int8_t wallPhase=1;

int8_t level=1;

int8_t gameMode=0;
int8_t selectedOption=1;
int8_t menuPointerPos=0;
bool menuPointerPosDir=false,gamePaused=false,escapeGameOver=false;

void setup() {
  
  //Serial.begin(9600);
  
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);
  pinMode(button4Pin, INPUT);
  pinMode(button5Pin, INPUT);
  pinMode(button6Pin, INPUT);
  pinMode(button7Pin, INPUT);

  digitalWrite(button1Pin, HIGH);
  digitalWrite(button2Pin, HIGH);
  digitalWrite(button3Pin, HIGH);
  digitalWrite(button4Pin, HIGH);
  digitalWrite(button5Pin, HIGH);
  digitalWrite(button6Pin, HIGH);
  digitalWrite(button7Pin, HIGH);
  // or just 
  // pinMode(button1Pin, INPUT_PULLUP)
  // etc
  
  pinMode(sound, OUTPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(13,OUTPUT);

  randomSeed(analogRead(A0)+readVcc());
  
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  //display.begin(SH1106_SWITCHCAPVCC, 0x78);
  display.clearDisplay();
  display.display();
  
  delay(random(2,2000));
  
  display.clearDisplay();
  
  // splash
  display.setTextColor(WHITE);
  //display.println(F("jakobdesign presents")); 
  //display.print(F(" generating maze...")); 
  display.drawBitmap(0, 10, title4 , 128, 64, WHITE);
  display.display();
  digitalWrite(ledPin, HIGH);
  aj_tone(sound,2200,5);
  digitalWrite(ledPin, LOW);
  delay(100);
  display.clearDisplay();
  display.drawBitmap(0, 5, title3 , 128, 64, WHITE);
  display.display();
  digitalWrite(ledPin, HIGH);
  aj_tone(sound,2200,5);
  digitalWrite(ledPin, LOW);
  delay(100);
  display.clearDisplay();
  display.drawBitmap(0, 2, title2 , 128, 64, WHITE);
  display.display();
  digitalWrite(ledPin, HIGH);
  aj_tone(sound,2200,5);
  digitalWrite(ledPin, LOW);
  delay(100);
  display.clearDisplay();
  display.drawBitmap(0, 0, title1 , 128, 64, WHITE);
  display.display();
  digitalWrite(ledPin, HIGH);
  aj_tone(sound,2200,5);
  digitalWrite(ledPin, LOW);
  delay(3000);
  display.clearDisplay();
  display.drawBitmap(0, 2, title2 , 128, 64, WHITE);
  display.display();
  digitalWrite(ledPin, HIGH);
  aj_tone(sound,2200,5);
  digitalWrite(ledPin, LOW);
  delay(100);
  display.clearDisplay();
  display.drawBitmap(0, 5, title3 , 128, 64, WHITE);
  display.display();
  digitalWrite(ledPin, HIGH);
  aj_tone(sound,2200,5);
  digitalWrite(ledPin, LOW);
  delay(100);
  display.clearDisplay();
  display.drawBitmap(0, 10, title4 , 128, 64, WHITE);
  display.display();
  digitalWrite(ledPin, HIGH);
  aj_tone(sound,2200,5);
  digitalWrite(ledPin, LOW);
  delay(100);
  display.clearDisplay();  
}


void loop() {
  
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  button4State = digitalRead(button4Pin);
  button5State = digitalRead(button5Pin);
  button6State = digitalRead(button6Pin);
  button7State = digitalRead(button7Pin);

  display.clearDisplay();

  if(gameMode==0){

    //display.drawBitmap(0, -20, title1 , 128, 64, WHITE);
    mainMenu();
    displayBattery(WHITE);
    
  }
  if(gameMode==1){
    walker();
    displayBattery(WHITE);
    if(!gamePaused){
      selectedOption=1;
    }
  }
  if(gameMode==2){
    collector();
    displayBattery(WHITE);
    if(!gamePaused){
      selectedOption=1;
    }
  }
  if(gameMode==3){
    escaper();
    displayBattery(WHITE);
    if(!gamePaused and !escapeGameOver){
      selectedOption=1;
    }
  }
  if(gameMode==4){
    dark();
    displayBattery(WHITE);
    if(!gamePaused){
      selectedOption=1;
    }
  }

  displayIndicators(WHITE);
  
  display.display();
  delay(10*DELAYMULTIPLIER);
  
}

void displayIndicators(uint8_t font){
  //speaker
  display.drawLine(103,1,103,6,font);
  display.drawLine(102,1,102,6,font);
  display.drawLine(101,2,101,5,font);
  display.drawLine(100,3,100,4,font);
  display.drawLine(99,3,99,4,font);
  if(sound_enabled){
    display.drawPixel(105,2,font);
    display.drawPixel(106,3,font);
    display.drawPixel(106,4,font);
    display.drawPixel(105,5,font);
  }
  //...
}


void displayBattery(uint8_t font){

  display.setTextColor(font);
  display.setCursor(82,0);
  //display.print("vcc: ");
  int batt=readVcc(); 
  //display.print(batt);

  display.drawLine(114,1,125,1,font);
  display.drawLine(114,6,125,6,font);
  display.drawLine(114,1,114,6,font);
  display.drawLine(125,1,125,6,font);
  display.drawLine(126,3,126,4,font);

  if(batt>4300){
    display.drawLine(109,2,109,1,font);
    display.drawLine(111,2,111,1,font);
    display.drawLine(108,3,112,3,font);
    display.drawLine(108,4,112,4,font);
    display.drawPixel(110,5,font);
    display.drawPixel(111,6,font);
    display.drawPixel(112,6,font);
    display.drawPixel(113,6,font);
  }
  if(batt>2900){
    display.drawLine(116,3,116,4,font);
    display.drawLine(117,3,117,4,font);
  }
  if(batt>3000){
    display.drawLine(119,3,119,4,font);
    display.drawLine(120,3,120,4,font);
  }
  if(batt>3100){
    display.drawLine(122,3,122,4,font);
    display.drawLine(123,3,123,4,font);
  }
    
}

int readVcc() {
  int result; // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result;
  // Back-calculate AVcc in mV
  return result;
}

void aj_tone(uint8_t pin,int freq,int duration){
  if(sound_enabled){
    tone(pin,freq,duration);  
  }
}