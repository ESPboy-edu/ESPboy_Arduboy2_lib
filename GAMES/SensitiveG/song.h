#ifndef SONG_H
#define SONG_H

#define Song const uint8_t PROGMEM

Song music[] = {
19,      // Number of tracks
0x00, 0x00,   // Address of track 0
0x06, 0x00,   // Address of track 1
0x0d, 0x00,   // Address of track 2
0x17, 0x00,   // Address of track 3
0x5e, 0x00,   // Address of track 4
0xaf, 0x00,   // Address of track 5
0x00, 0x01,   // Address of track 6
0x03, 0x01,   // Address of track 7
0x4c, 0x01,   // Address of track 8
0xcd, 0x01,   // Address of track 9
0x4e, 0x02,   // Address of track 10
0x57, 0x02,   // Address of track 11
0x60, 0x02,   // Address of track 12
0xb1, 0x02,   // Address of track 13
0x32, 0x03,   // Address of track 14
0x4b, 0x03,   // Address of track 15
0x6e, 0x03,   // Address of track 16
0x89, 0x03,   // Address of track 17
0x9f, 0x03,   // Address of track 18
0x0f,     // Channel 0 entry track
0x10,     // Channel 1 entry track
0x11,     // Channel 2 entry track
0x12,     // Channel 3 entry track
//"Track tick"
0x40, 8,   // FX: SET VOLUME: volume = 32
0x9F + 1,   // DELAY: ticks = 1
0x40, 0,    // FX: SET VOLUME: volume = 0
0xFE,     // RETURN
//"Track snare"
0x40, 8,   // FX: SET VOLUME: volume = 32
0x41, -16,    // FX: VOLUME SLIDE ON: steps = -16
0x9F + 2,   // DELAY: ticks = 2
0x43,     // FX: VOLUME SLIDE OFF
0xFE,     // RETURN
//"Track shake"
0x49, 4 + 0,    // FX: RETRIG NOISE: point = 1 (*4) / speed = 0 (fastest)
0x40, 8,   // FX: SET VOLUME: volume = 32
0x41, -8,   // FX: VOLUME SLIDE ON: steps = -8
0x9F + 4,   // DELAY: ticks = 4
0x4A,     // FX: RETRIG: off
0x43,     // FX: VOLUME SLIDE OFF
0xFE,     // RETURN
//"Track Track 1"
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0x9F + 2,   // DELAY: ticks = 2
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0x9F + 2,   // DELAY: ticks = 2
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0x9F + 2,   // DELAY: ticks = 2
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 1,    // GOTO track 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFE,     // RETURN
//"Track 01"
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 5,   // DELAY: ticks = 5
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 11,    // NOTE ON: note = 11
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0xFE,     // RETURN
//"Track 02"
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 8,   // NOTE ON: note = 8
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 8,   // NOTE ON: note = 8
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 8,   // NOTE ON: note = 8
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 8,   // NOTE ON: note = 8
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 5,   // DELAY: ticks = 5
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 6,   // NOTE ON: note = 6
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 8,   // NOTE ON: note = 8
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 8,   // NOTE ON: note = 8
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 8,   // NOTE ON: note = 8
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 12,    // NOTE ON: note = 12
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0xFE,     // RETURN
//"Track Blank"
0x00 + 0,   // NOTE ON: note = 0
0x9F + 64,    // DELAY: ticks = 64
0xFE,     // RETURN
//"Track Track 5"
0xFC, 1,    // GOTO track 1
0x9F + 2,   // DELAY: ticks = 2
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0x9F + 2,   // DELAY: ticks = 2
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0x9F + 2,   // DELAY: ticks = 2
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 1,    // GOTO track 1
0xFC, 0,    // GOTO track 0
0x9F + 3,   // DELAY: ticks = 3
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFC, 0,    // GOTO track 0
0x9F + 1,   // DELAY: ticks = 1
0xFE,     // RETURN
//"Track 03"
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 25,    // NOTE ON: note = 25
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 25,    // NOTE ON: note = 25
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 25,    // NOTE ON: note = 25
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 25,    // NOTE ON: note = 25
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0xFE,     // RETURN
//"Track 04"
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 22,    // NOTE ON: note = 22
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 19,    // NOTE ON: note = 19
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 22,    // NOTE ON: note = 22
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 19,    // NOTE ON: note = 19
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 22,    // NOTE ON: note = 22
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 19,    // NOTE ON: note = 19
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 22,    // NOTE ON: note = 22
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 19,    // NOTE ON: note = 19
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0xFE,     // RETURN
//"Track 05"
0x00 + 25,    // NOTE ON: note = 25
0x9F + 16,    // DELAY: ticks = 16
0x00 + 0,   // NOTE ON: note = 0
0x9F + 17,    // DELAY: ticks = 17
0x00 + 32,    // NOTE ON: note = 32
0x9F + 17,    // DELAY: ticks = 17
0x00 + 0,   // NOTE ON: note = 0
0x9F + 14,    // DELAY: ticks = 14
0xFE,     // RETURN
//"Track 06"
0x00 + 30,    // NOTE ON: note = 30
0x9F + 16,    // DELAY: ticks = 16
0x00 + 0,   // NOTE ON: note = 0
0x9F + 17,    // DELAY: ticks = 17
0x00 + 23,    // NOTE ON: note = 23
0x9F + 17,    // DELAY: ticks = 17
0x00 + 0,   // NOTE ON: note = 0
0x9F + 14,    // DELAY: ticks = 14
0xFE,     // RETURN
//"Track 10"
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 13,    // NOTE ON: note = 13
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 18,    // NOTE ON: note = 18
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 18,    // NOTE ON: note = 18
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 18,    // NOTE ON: note = 18
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 18,    // NOTE ON: note = 18
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 18,    // NOTE ON: note = 18
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 18,    // NOTE ON: note = 18
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 20,    // NOTE ON: note = 20
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 20,    // NOTE ON: note = 20
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 20,    // NOTE ON: note = 20
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0x00 + 20,    // NOTE ON: note = 20
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 3,   // DELAY: ticks = 3
0xFE,     // RETURN
//"Track 11"
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 25,    // NOTE ON: note = 25
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 25,    // NOTE ON: note = 25
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 29,    // NOTE ON: note = 29
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 37,    // NOTE ON: note = 37
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 22,    // NOTE ON: note = 22
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 34,    // NOTE ON: note = 34
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 30,    // NOTE ON: note = 30
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 36,    // NOTE ON: note = 36
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 24,    // NOTE ON: note = 24
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 27,    // NOTE ON: note = 27
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 36,    // NOTE ON: note = 36
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0x00 + 32,    // NOTE ON: note = 32
0x9F + 1,   // DELAY: ticks = 1
0x00 + 0,   // NOTE ON: note = 0
0x9F + 1,   // DELAY: ticks = 1
0xFE,     // RETURN
//"Track Track 13"
0xFC, 2,    // GOTO track 2
0x9F + 2,   // DELAY: ticks = 2
0xFC, 2,    // GOTO track 2
0x9F + 2,   // DELAY: ticks = 2
0xFC, 2,    // GOTO track 2
0x9F + 8,   // DELAY: ticks = 8
0xFC, 1,    // GOTO track 1
0x9F + 6,   // DELAY: ticks = 6
0xFC, 2,    // GOTO track 2
0x9F + 2,   // DELAY: ticks = 2
0xFC, 2,    // GOTO track 2
0x9F + 2,   // DELAY: ticks = 2
0xFC, 2,    // GOTO track 2
0x9F + 8,   // DELAY: ticks = 8
0xFC, 1,    // GOTO track 1
0x9F + 6,   // DELAY: ticks = 6
0xFE,     // RETURN
//"Track channel 0"
0x9D, 18,   // SET song tempo: value = 18
0x40, 56,   // FX: SET VOLUME: volume = 48
0xFC, 6,    // GOTO track 6
0xFC, 4,    // GOTO track 4
0xFC, 5,    // GOTO track 5
0xFC, 4,    // GOTO track 4
0xFC, 5,    // GOTO track 5
0xFC, 4,    // GOTO track 4
0xFC, 5,    // GOTO track 5
0xFC, 4,    // GOTO track 4
0xFC, 5,    // GOTO track 5
0xFC, 4,    // GOTO track 4
0xFC, 5,    // GOTO track 5
0xFD, 1, 6,   // REPEAT: count = 1 + 1 / track = 6
0xFD, 3, 12,    // REPEAT: count = 3 + 1 / track = 12
0x40, 0,    // FX: SET VOLUME: volume = 0
0x9F,     // FX: STOP CURRENT CHANNEL
//"Track channel 1"
0x40, 24,   // FX: SET VOLUME: volume = 48
0xFD, 4, 6,   // REPEAT: count = 4 + 1 / track = 6
0xFC, 8,    // GOTO track 8
0xFC, 9,    // GOTO track 9
0xFC, 8,    // GOTO track 8
0xFC, 9,    // GOTO track 9
0xFC, 8,    // GOTO track 8
0xFC, 9,    // GOTO track 9
0xFC, 8,    // GOTO track 8
0xFC, 9,    // GOTO track 9
0xFD, 3, 6,   // REPEAT: count = 3 + 1 / track = 6
0x40, 0,    // FX: SET VOLUME: volume = 0
0x9F,     // FX: STOP CURRENT CHANNEL
//"Track channel 2"
0x40, 24,   // FX: SET VOLUME: volume = 48
0xFD, 8, 6,   // REPEAT: count = 8 + 1 / track = 6
0xFC, 10,   // GOTO track 10
0xFC, 11,   // GOTO track 11
0xFC, 10,   // GOTO track 10
0xFC, 11,   // GOTO track 11
0xFD, 1, 6,   // REPEAT: count = 1 + 1 / track = 6
0xFD, 1, 13,    // REPEAT: count = 1 + 1 / track = 13
0x40, 0,    // FX: SET VOLUME: volume = 0
0x9F,     // FX: STOP CURRENT CHANNEL
//"Track channel 3"
0x40, 0,    // FX: SET VOLUME: volume = 0
0xFC, 3,    // GOTO track 3
0xFD, 13, 7,    // REPEAT: count = 13 + 1 / track = 7
0xFD, 1, 14,    // REPEAT: count = 1 + 1 / track = 14
0x40, 0,    // FX: SET VOLUME: volume = 0
0x9F,     // FX: STOP CURRENT CHANNEL

};



#endif
