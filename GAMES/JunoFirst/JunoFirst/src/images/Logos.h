#pragma once


// ------------------------------------------------------------------------------------------------------------------------------------
//  Logos ..

const uint8_t PROGMEM gameOver[] = {
125, 28,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xf0, 0x7c, 0x1e, 0x0e, 0x06, 0x06, 0x06, 0x06, 0x26, 0x36, 0x3e, 0x1e, 0x06, 0xc0, 0xf8, 0x3e, 0x06, 0x06, 0x06, 0x06, 0xc6, 0xfe, 0xfe, 0xfe, 0x0e, 0x00, 0xf0, 0xfe, 0xfe, 0xfe, 0xfe, 0xf8, 0x80, 0x00, 0x80, 0xc0, 0xf0, 0xfc, 0xfe, 0xfe, 0xfe, 0x00, 0x00, 0xfe, 0xfe, 0xfe, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0xfc, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x02, 0x06, 0x7e, 0x7e, 0xfc, 0xe0, 0x00, 0x1e, 0x7e, 0xfe, 0xfe, 0xfc, 0xe0, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x3e, 0xfe, 0xfe, 0xc6, 0x06, 0x06, 0x06, 0x00, 0x00, 0x06, 0x3e, 0xfa, 0xe2, 0x82, 0x02, 0x02, 0x04, 0x1c, 0x78, 0xc0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe0, 0xf8, 0x3e, 0x0f, 0x07, 0x01, 0x00, 0x00, 0x04, 0x86, 0xe6, 0xfe, 0xfe, 0x7e, 0x0e, 0xc0, 0xf8, 0xfe, 0xff, 0x07, 0x06, 0x06, 0x06, 0x06, 0x06, 0xff, 0xff, 0xff, 0x03, 0x00, 0xfc, 0xff, 0xff, 0x7f, 0x07, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0x0e, 0x0e, 0x0e, 0x0e, 0x0c, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x03, 0x0f, 0x7f, 0xff, 0xff, 0xf8, 0xe0, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xfe, 0x8e, 0x0e, 0x0e, 0x0c, 0x00, 0x00, 0x03, 0x3f, 0xff, 0xfe, 0xfe, 0xe2, 0x82, 0x02, 0x03, 0x03, 0x03, 0x0f, 0x7f, 0xff, 0xfe, 0xf8, 0xe0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x50, 0x5c, 0x5f, 0x5f, 0x59, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x5e, 0x5f, 0x5f, 0x4f, 0x03, 0x18, 0x5e, 0x5f, 0x5f, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x5f, 0x5f, 0x03, 0x00, 0x5f, 0x5f, 0x5f, 0x5f, 0x00, 0x00, 0x5f, 0x5f, 0x5f, 0x07, 0x00, 0x00, 0x01, 0x5f, 0x5f, 0x5f, 0x5f, 0x00, 0x00, 0x5f, 0x5f, 0x5f, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x00, 0x00, 0x00, 0x0f, 0x5f, 0x5f, 0x5f, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x5f, 0x5f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x4f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x5f, 0x5f, 0x5f, 0x5c, 0x58, 0x58, 0x58, 0x58, 0x58, 0x00, 0x01, 0x4f, 0x5f, 0x5f, 0x5f, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x5f, 0x5f, 0x5f, 0x5f, 0x1c, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04, 0x00, 0x01, 0x05, 0x05, 0x05, 0x04, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x01, 0x00, 0x04, 0x05, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x05, 0x05, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 
};

const uint8_t PROGMEM startOfWave[] = {
44, 11,
0x01, 0x01, 0x01, 0x01, 0x01, 0x79, 0x81, 0x41, 0x81, 0x79, 0x01, 0xf1, 0x29, 0x29, 0xf1, 0x01, 0x79, 0x81, 0x61, 0x19, 0x01, 0xf9, 0xa9, 0xa9, 0x89, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
};


const uint8_t PROGMEM waveCompleted[] = {
76, 20,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x79, 0x81, 0x41, 0x81, 0x79, 0x01, 0xf1, 0x29, 0x29, 0xf1, 0x01, 0x79, 0x81, 0x61, 0x19, 0x01, 0xf9, 0xa9, 0xa9, 0x89, 0x01, 0x01, 0x01, 0x71, 0x89, 0x89, 0x51, 0x01, 0x71, 0x89, 0x89, 0x71, 0x01, 0xf9, 0x11, 0x21, 0x11, 0xf9, 0x01, 0xf9, 0x49, 0x49, 0x31, 0x01, 0xf9, 0x81, 0x81, 0x81, 0x01, 0xf9, 0xa9, 0xa9, 0x89, 0x01, 0x09, 0xf9, 0x09, 0x09, 0x01, 0xf9, 0xa9, 0xa9, 0x89, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
0x00, 0x00, 0xf8, 0x28, 0x28, 0x08, 0x00, 0x78, 0x80, 0x80, 0x78, 0x00, 0xf8, 0xa8, 0xa8, 0x88, 0x00, 0xf8, 0x80, 0x80, 0x80, 0x00, 0x00, 0xf8, 0xa8, 0xa8, 0x50, 0x00, 0x70, 0x88, 0x88, 0x70, 0x00, 0xf8, 0x10, 0x20, 0xf8, 0x00, 0x78, 0x80, 0x80, 0x78, 0x00, 0x90, 0xa8, 0xa8, 0x48, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
};

const uint8_t PROGMEM getReady[] = {
51, 11,
0x01, 0x01, 0x71, 0x89, 0xa9, 0xe9, 0x01, 0xf9, 0xa9, 0xa9, 0x89, 0x01, 0x09, 0x09, 0xf9, 0x09, 0x09, 0x01, 0x01, 0x01, 0x01, 0xf9, 0x29, 0x29, 0xd1, 0x01, 0xf9, 0xa9, 0xa9, 0x89, 0x01, 0xf1, 0x29, 0x29, 0xf1, 0x01, 0xf9, 0x89, 0x89, 0x71, 0x01, 0x99, 0xa1, 0xa1, 0x79, 0x01, 0x01, 0x01, 0xb9, 0x01, 0x01, 
0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
};

const uint8_t PROGMEM x2Inverted[] = {
14, 7,
0x7f, 0x5d, 0x6b, 0x77, 0x6b, 0x5d, 0x7f, 0x7f, 0x5b, 0x4d, 0x55, 0x5b, 0x7f, 0x00,
};

const uint8_t PROGMEM pause[] = {
33, 16,
0x01, 0x01, 0xf9, 0x29, 0x29, 0x11, 0x01, 0xf1, 0x29, 0x29, 0xf1, 0x01, 0x79, 0x81, 0x81, 0x79, 0x01, 0x91, 0xa9, 0xa9, 0x49, 0x01, 0xf9, 0xa9, 0xa9, 0x89, 0x01, 0xf9, 0x89, 0x89, 0x71, 0x01, 0x01, 
0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 
};


// ------------------------------------------------------------------------------------------------------------------------------------
//  HUD ..

const uint8_t PROGMEM fuelGauge[] = {
7, 31,
0x00, 0xa2, 0x12, 0x96, 0x12, 0xa6, 0x00, 
0x00, 0xff, 0x00, 0xaa, 0x00, 0xff, 0x00, 
0x00, 0xff, 0x00, 0xaa, 0x00, 0xff, 0x00, 
0x00, 0x1f, 0x20, 0x2a, 0x20, 0x1f, 0x00, 
};

const uint8_t PROGMEM shieldGauge[] = {
7, 25,
0x00, 0x8c, 0x5e, 0x56, 0x52, 0x9e, 0x00, 
0x00, 0xfe, 0x00, 0xaa, 0x00, 0xfe, 0x00, 
0x00, 0x7f, 0x80, 0xaa, 0x80, 0x7f, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const uint8_t PROGMEM x2_score_table[] = {
11, 5,
0x11, 0x0a, 0x04, 0x0a, 0x11, 0x00, 0x00, 0x12, 0x19, 0x15, 0x12, 
};

const uint8_t PROGMEM x2_flashing[] = {
11, 8,
0x42, 0x81, 0xa9, 0x91, 0xa9, 0x81, 0xa5, 0xb5, 0xad, 0x81, 0x42, 
};