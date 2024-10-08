#pragma once

namespace Images {

    const uint8_t PROGMEM Sky[] = {
    64, 24,
    0xaa, 0x00, 0xaa, 0x00, 0xaa, 0x01, 0xab, 0x07, 0xaf, 0x1f, 0xbf, 0x7f, 0xbf, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xbf, 0x1f, 0xbf, 0x1f, 0xaf, 0x07, 0xab, 0x03, 0xab, 0x07, 0xaf, 0x1f, 0xbf, 0x7f, 0xbf, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xbf, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xbf, 0x1f, 0xbf, 0x1f, 0xaf, 0x07, 0xab, 0x01, 0xaa, 0x00, 
    0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xab, 0xfd, 0xab, 0xfd, 0xab, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 0xaa, 0xfd, 
    0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 0xfb, 0xff, 0xfe, 0xff, 
    };

    const uint8_t PROGMEM Underground_Brick[] = {
    18, 11,
    0x00, 0x10, 0x00, 0x10, 0x10, 0x90, 0x10, 0x00, 0x17, 0x00, 0x1d, 0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 
    };

    const uint8_t PROGMEM Underground_Chain[] = {
    3, 12,
    0x66, 0x88, 0x33, 
    0x06, 0x08, 0x03, 
    };

    #ifndef DEBUG
    const uint8_t PROGMEM Outside_Exit_00[] = {
    26, 36,
    0x00, 0x00, 0x00, 0x00, 0xfe, 0xfc, 0xdc, 0x2c, 0x36, 0xf6, 0x16, 0xee, 0x76, 0x7a, 0xfa, 0x7a, 0x7a, 0xf2, 0xe6, 0x0e, 0x16, 0xf6, 0x36, 0x2c, 0xdc, 0xfc, 
    0x00, 0x18, 0x18, 0x10, 0x93, 0x97, 0x16, 0xd5, 0x95, 0xd6, 0x94, 0xc3, 0x8f, 0xc6, 0x8d, 0xc7, 0x8e, 0xc3, 0x91, 0xd0, 0x90, 0xd1, 0x91, 0xd5, 0x96, 0xd7, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x62, 0x62, 0x62, 0x6a, 0x62, 0x62, 0x6a, 0x62, 0x62, 0x6a, 0xe2, 0x62, 0x2a, 0xa2, 0xa2, 0xaa, 0xa2, 0x62, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xe3, 0x63, 0x63, 0x63, 0x41, 0xe3, 0xf7, 0xff, 0x01, 0xfc, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x0f, 0x0f, 0x07, 0x08, 0x05, 0x0f, 0x05, 0x03, 0x04, 0x03, 0x0f, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };
    #else
    const uint8_t PROGMEM Outside_Exit_00[] = {
    1, 1,
    0x01
    };
    #endif

    #ifndef DEBUG
    const uint8_t PROGMEM Outside_Exit_00_Mask[] = {
    0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 
    0x3c, 0x3c, 0x3c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
    };
    #else
    const uint8_t PROGMEM Outside_Exit_00_Mask[] = {
    0x01
    };
    #endif

    #ifndef DEBUG
    const uint8_t PROGMEM Outside_Exit_01[] = {
    28, 36,
    0xfe, 0x00, 0x00, 0xa0, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x93, 0xd0, 0x90, 0xda, 0x1b, 0x9a, 0x99, 0x1a, 0x18, 0x00, 0xc0, 0x80, 0xc0, 0x80, 0xc0, 0x80, 0xc0, 0x80, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00, 
    0xea, 0x62, 0x62, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xdd, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x00, 0xc0, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 
    0x01, 0xff, 0x77, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xdd, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x00, 0xcc, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 
    0x00, 0x0f, 0x0f, 0x0f, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x0d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 
    };
    #else
    const uint8_t PROGMEM Outside_Exit_01[] = {
    1, 1,
    0x01
    };
    #endif

    #ifndef DEBUG
    const uint8_t PROGMEM Outside_Exit_01_Mask[] = {
    0xff, 0xff, 0xfe, 0x50, 0xa0, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xff, 0xff, 0xff, 0xfd, 0xfe, 0xfd, 0xfe, 0xfd, 0xfe, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
    };
    #else
    const uint8_t PROGMEM Outside_Exit_01_Mask[] = {
    0x01
    };
    #endif

    const uint8_t PROGMEM Underground_Exit_00[] = {
    13, 16,
    0xfc, 0x02, 0xa9, 0xdd, 0xfd, 0xfd, 0xfd, 0x7d, 0x81, 0x81, 0x01, 0x01, 0x01, 
    0xff, 0x00, 0xaf, 0xdf, 0xff, 0xff, 0xff, 0xf0, 0x04, 0x07, 0x80, 0x00, 0xa0, 
    };

    const uint8_t PROGMEM Underground_Exit_01[] = {
    3, 16,
    0x02, 0xfc, 0x00, 
    0x00, 0xff, 0x00, 
    };

    const uint8_t PROGMEM SignPost[] = {
    16, 16,
    0xf0, 0x08, 0x48, 0x48, 0x48, 0x4e, 0x0d, 0x4f, 0x0e, 0x48, 0x48, 0x08, 0x08, 0x98, 0xf8, 0x60, 
    0x07, 0x04, 0x05, 0x04, 0x05, 0xfd, 0x0d, 0xfd, 0xfc, 0x05, 0x04, 0x04, 0x04, 0x07, 0x03, 0x00, 

    0x00, 0xf0, 0xb0, 0xb0, 0xb0, 0xb0, 0xf2, 0xb0, 0xf0, 0xb0, 0xb0, 0xf0, 0xf0, 0x60, 0x00, 0x00, 
    0x00, 0x03, 0x02, 0x03, 0x02, 0x02, 0xf2, 0x02, 0x03, 0x02, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 
    };

    const uint8_t PROGMEM SignPost_Mask[] = {
    0xf0, 0xf8, 0xf8, 0xf8, 0xf8, 0xfe, 0xff, 0xff, 0xfe, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0x60, 
    0x07, 0x07, 0x07, 0x07, 0x07, 0xff, 0xff, 0xff, 0xff, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00, 
    };

    const uint8_t PROGMEM Platform[] = {
    12, 12,
    0x00, 0x7a, 0xb6, 0x76, 0xb6, 0x76, 0xba, 0x7a, 0xba, 0x7a, 0x36, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };

    const uint8_t PROGMEM Platform_Mask[] = {
    0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
    0x00, 0x03, 0x07, 0x0f, 0x0f, 0x07, 0x07, 0x0f, 0x0f, 0x07, 0x03, 0x00, 
    };

    const uint8_t PROGMEM Water[] = {
    24, 8,
    0xf8, 0xfc, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0xf0, 0xf8, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0xf0, 0xf8, 0xf8, 
    };
};
