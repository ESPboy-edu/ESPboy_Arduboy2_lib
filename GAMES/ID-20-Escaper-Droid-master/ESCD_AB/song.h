#ifndef SONG_H
#define SONG_H

// ---------------------------------------------------------------------------
//  Game songs, size-optimised (same notes, volumes, slides, tempo and cues as before)
//
//    gameOver   106 B  unchanged (nothing left to save: every track is used, the pieces that repeat are
//                      already shared; moving the toll slide into the drum track saves 0 bytes)
//    titleSong  546 B -> 351 B   (Escaper Droid, loops, ATM_CUE(2) once per beat, tempo 36 set in the song)
//    gameSong   212 B -> replaced by 4 spy songs, one per level (spyDoubleZero, spyShadowWalk, spyChase,
//                      spyVault), 8 bars each, loop 512 ticks, all at 60 % volume, no tempo in the songs
//
//  All loops are hop-free self-loops: every channel's entry track ends with ATM_GOTO(itself)
//  and sums to exactly the loop length (titleSong 512 ticks, every game song 512 ticks), so there is no
//  ATM_STOP_CHAN / ATM_GOTO_ADV any more and no silent tick at the seam.
//  Checked tick by tick against the old data: identical note and cue events and identical volume /
//  frequency of all four channels, and every loop pass is identical.
// ---------------------------------------------------------------------------

#include <ATMcmds.h>

#define Song const uint8_t PROGMEM

// in-game music plays at 60 % of the title music volume (ATMlib README: "Keep the music quieter than the effects")
// only the game songs use MV() / MS(); titleSong and gameOver stay at 100 %. Scaled at compile time, no extra bytes.
#define MUSIC_VOL_PCT 60                       // 100 = original
#define MV(v) ((v) * MUSIC_VOL_PCT / 100)
// the fade step, scaled the same way and never below 1 (a 0 would stop the fade)
#define MS(n) ((uint8_t)-((((n) * MUSIC_VOL_PCT + 50) / 100) < 1 ? 1 : (((n) * MUSIC_VOL_PCT + 50) / 100)))


Song gameOver[] = {     // total song bytes = 106
  0x06,                       // Number of tracks
  0x00, 0x00,                 // Address of track 0      0   drums
  0x06, 0x00,                 // Address of track 1      6   toll
  0x0C, 0x00,                 // Address of track 2     12   lead
  0x23, 0x00,                 // Address of track 3     35   lead_phrase
  0x2A, 0x00,                 // Address of track 4     42   chords
  0x42, 0x00,                 // Address of track 5     66   bass

  0x02,                         // CH0 entry -> track 2 (lead)
  0x04,                         // CH1 entry -> track 4 (chords)
  0x05,                         // CH2 entry -> track 5 (bass)
  0x00,                         // CH3 entry -> track 0 (drums)

  //"Track 0" drums  [6b]
  ATM_SET_TEMPO(20),
  ATM_REPEAT(4, 1),   // 5x toll
  ATM_STOP_CHAN,

  //"Track 1" toll  [6b]
  ATM_VOL(46),
  ATM_SL_VOL((uint8_t)-3),
  ATM_DELAY(64),
  ATM_RETURN,

  //"Track 2" lead  [23b]
  ATM_VOL(40),
  ATM_SL_VOL((uint8_t)-1),
  ATM_GOTO(3),   // -> lead_phrase
  ATM_ADD_TRA((uint8_t)-2),
  ATM_GOTO(3),   // -> lead_phrase
  ATM_ADD_TRA((uint8_t)-2),
  ATM_GOTO(3),   // -> lead_phrase
  ATM_ADD_TRA((uint8_t)-1),
  ATM_GOTO(3),   // -> lead_phrase
  ATM_SET_TRA(0),
  ATM_NOTE_A4,
  ATM_DELAY(64),
  ATM_STOP_CHAN,

  //"Track 3" lead_phrase  [7b]
  ATM_NOTE_A5,
  ATM_DELAY(16),
  ATM_NOTE_E5,
  ATM_DELAY(16),
  ATM_NOTE_A4,
  ATM_DELAY(32),
  ATM_RETURN,

  //"Track 4" chords  [24b]
  ATM_VOL(26),
  ATM_SL_VOL((uint8_t)-1),
  ATM_ARP(0x34, 0x20),
  ATM_NOTE_A3,
  ATM_DELAY(64),
  ATM_ARP(0x43, 0x20),
  ATM_NOTE_G3,
  ATM_DELAY(64),
  ATM_NOTE_F3,
  ATM_DELAY(64),
  ATM_NOTE_E3,
  ATM_DELAY(64),
  ATM_ARP(0x34, 0x20),
  ATM_NOTE_A3,
  ATM_DELAY(64),
  ATM_STOP_CHAN,

  //"Track 5" bass  [23b]
  ATM_VOL(48),
  ATM_SL_VOL((uint8_t)-1),
  ATM_NOTE_A2,
  ATM_DELAY(64),
  ATM_ADD_TRA((uint8_t)-2),
  ATM_NOTE_A2,
  ATM_DELAY(64),
  ATM_ADD_TRA((uint8_t)-2),
  ATM_NOTE_A2,
  ATM_DELAY(64),
  ATM_ADD_TRA((uint8_t)-1),
  ATM_NOTE_A2,
  ATM_DELAY(64),
  ATM_SET_TRA(0),
  ATM_NOTE_A2,
  ATM_DELAY(64),
  ATM_STOP_CHAN,

};

Song titleSong[] = {     // total song bytes = 351
  0x0F,                       // Number of tracks
  0x00, 0x00,                 // Address of track 0      0   drums
  0x07, 0x00,                 // Address of track 1      7   drum_half
  0x27, 0x00,                 // Address of track 2     39   drum_A
  0x37, 0x00,                 // Address of track 3     55   drum_pair
  0x44, 0x00,                 // Address of track 4     68   lead
  0xBA, 0x00,                 // Address of track 5    186   chords
  0xD1, 0x00,                 // Address of track 6    209   kick
  0xE2, 0x00,                 // Address of track 7    226   beat_D
  0xEA, 0x00,                 // Address of track 8    234   beat_G
  0xF2, 0x00,                 // Address of track 9    242   beat_A
  0xFA, 0x00,                 // Address of track 10   250   beat_Bm
  0x02, 0x01,                 // Address of track 11   258   bass
  0x23, 0x01,                 // Address of track 12   291   bass_head
  0x2B, 0x01,                 // Address of track 13   299   bass_beat
  0x32, 0x01,                 // Address of track 14   306   bass_bar

  0x04,                         // CH0 entry -> track 4 (lead)
  0x05,                         // CH1 entry -> track 5 (chords)
  0x0B,                         // CH2 entry -> track 11 (bass)
  0x00,                         // CH3 entry -> track 0 (drums)

  //"Track 0" drums  [7b]
  ATM_SET_TEMPO(36),
  ATM_REPEAT(1, 1),   // 2x drum_half
  ATM_GOTO(0),   // -> drums  (loop: this very track)

  //"Track 1" drum_half  [32b]
  ATM_REPEAT(6, 3),   // 7x drum_pair
  ATM_GOTO(2),   // -> drum_A
  ATM_VOL(22),
  ATM_SL_VOL((uint8_t)-9),
  ATM_DELAY(2),
  ATM_VOL(26),
  ATM_DELAY(2),
  ATM_VOL(30),
  ATM_DELAY(2),
  ATM_VOL(34),
  ATM_DELAY(2),
  ATM_VOL(38),
  ATM_DELAY(2),
  ATM_VOL(42),
  ATM_DELAY(2),
  ATM_VOL(46),
  ATM_DELAY(2),
  ATM_VOL(50),
  ATM_DELAY(2),
  ATM_RETURN,

  //"Track 2" drum_A  [16b]
  ATM_VOL(28),
  ATM_SL_VOL((uint8_t)-14),
  ATM_DELAY(4),
  ATM_VOL(12),
  ATM_SL_VOL((uint8_t)-6),
  ATM_DELAY(4),
  ATM_VOL(26),
  ATM_SL_VOL((uint8_t)-3),
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 3" drum_pair  [13b]
  ATM_GOTO(2),   // -> drum_A
  ATM_VOL(40),
  ATM_SL_VOL((uint8_t)-5),
  ATM_DELAY(8),
  ATM_VOL(26),
  ATM_SL_VOL((uint8_t)-3),
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 4" lead  [118b]
  ATM_VOL(38),
  ATM_SL_VOL((uint8_t)-2),
  ATM_NOTE_D5,
  ATM_DELAY(12),
  ATM_NOTE_F5_,
  ATM_DELAY(12),
  ATM_NOTE_A5,
  ATM_DELAY(8),
  ATM_NOTE_D5,
  ATM_DELAY(12),
  ATM_NOTE_F5_,
  ATM_DELAY(12),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_G5,
  ATM_DELAY(12),
  ATM_NOTE_B5,
  ATM_DELAY(12),
  ATM_NOTE_G5,
  ATM_DELAY(8),
  ATM_NOTE_B5,
  ATM_DELAY(12),
  ATM_NOTE_D6,
  ATM_DELAY(12),
  ATM_NOTE_B5,
  ATM_DELAY(8),
  ATM_NOTE_A5,
  ATM_DELAY(12),
  ATM_NOTE_C6_,
  ATM_DELAY(12),
  ATM_NOTE_A5,
  ATM_DELAY(8),
  ATM_NOTE_E5,
  ATM_DELAY(12),
  ATM_NOTE_A5,
  ATM_DELAY(12),
  ATM_NOTE_C6_,
  ATM_DELAY(8),
  ATM_NOTE_F5_,
  ATM_DELAY(12),
  ATM_NOTE_E5,
  ATM_DELAY(12),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_A4,
  ATM_DELAY(12),
  ATM_NOTE_D5,
  ATM_DELAY(12),
  ATM_NOTE_F5_,
  ATM_DELAY(8),
  ATM_NOTE_D5,
  ATM_DELAY(4),
  ATM_NOTE_E5,
  ATM_DELAY(4),
  ATM_NOTE_F5_,
  ATM_DELAY(8),
  ATM_NOTE_A5,
  ATM_DELAY(16),
  ATM_NOTE_G5,
  ATM_DELAY(8),
  ATM_NOTE_E5,
  ATM_DELAY(8),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_A4,
  ATM_DELAY(8),
  ATM_NOTE_G4,
  ATM_DELAY(4),
  ATM_NOTE_B4,
  ATM_DELAY(4),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_G5,
  ATM_DELAY(16),
  ATM_NOTE_F5_,
  ATM_DELAY(8),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_B4,
  ATM_DELAY(8),
  ATM_NOTE_G4,
  ATM_DELAY(8),
  ATM_NOTE_B4,
  ATM_DELAY(4),
  ATM_NOTE_D5,
  ATM_DELAY(4),
  ATM_NOTE_F5_,
  ATM_DELAY(8),
  ATM_NOTE_B5,
  ATM_DELAY(16),
  ATM_NOTE_A5,
  ATM_DELAY(8),
  ATM_NOTE_F5_,
  ATM_DELAY(8),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_B4,
  ATM_DELAY(8),
  ATM_NOTE_A4,
  ATM_DELAY(4),
  ATM_NOTE_C5_,
  ATM_DELAY(4),
  ATM_NOTE_E5,
  ATM_DELAY(8),
  ATM_NOTE_A5,
  ATM_DELAY(16),
  ATM_NOTE_G5,
  ATM_DELAY(8),
  ATM_NOTE_E5,
  ATM_DELAY(8),
  ATM_NOTE_C5_,
  ATM_DELAY(8),
  ATM_NOTE_A4,
  ATM_DELAY(8),
  ATM_GOTO(4),   // -> lead  (loop: this very track)

  //"Track 5" chords  [23b]
  ATM_REPEAT(3, 7),   // 4x beat_D
  ATM_REPEAT(3, 8),   // 4x beat_G
  ATM_REPEAT(3, 9),   // 4x beat_A
  ATM_REPEAT(7, 7),   // 8x beat_D
  ATM_REPEAT(3, 10),   // 4x beat_Bm
  ATM_REPEAT(3, 9),   // 4x beat_A
  ATM_REPEAT(3, 7),   // 4x beat_D
  ATM_GOTO(5),   // -> chords  (loop: this very track)

  //"Track 6" kick  [17b]
  ATM_CUE(2),                   // beat marker: fires once per beat (every 16 ticks)
  ATM_ARP_OFF,
  ATM_SL_FRQ((uint8_t)-127),
  ATM_VOL(30),
  ATM_NOTE_D3,
  ATM_DELAY(4),
  ATM_VOL(0),
  ATM_DELAY(4),
  ATM_SL_VOL((uint8_t)-3),
  ATM_VOL(24),
  ATM_RETURN,

  //"Track 7" beat_D  [8b]
  ATM_GOTO(6),   // -> kick
  ATM_ARP(0x47, 0x20),
  ATM_NOTE_D3,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 8" beat_G  [8b]
  ATM_GOTO(6),   // -> kick
  ATM_ARP(0x47, 0x20),
  ATM_NOTE_G3,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 9" beat_A  [8b]
  ATM_GOTO(6),   // -> kick
  ATM_ARP(0x47, 0x20),
  ATM_NOTE_A3,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 10" beat_Bm  [8b]
  ATM_GOTO(6),   // -> kick
  ATM_ARP(0x37, 0x20),
  ATM_NOTE_B3,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 11" bass  [33b]
  ATM_SL_VOL((uint8_t)-5),
  ATM_SET_TRA(0),
  ATM_GOTO(14),   // -> bass_bar
  ATM_SET_TRA(5),
  ATM_GOTO(14),   // -> bass_bar
  ATM_SET_TRA(7),
  ATM_GOTO(14),   // -> bass_bar
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 14),   // 2x bass_bar
  ATM_SET_TRA(9),
  ATM_GOTO(14),   // -> bass_bar
  ATM_SET_TRA(7),
  ATM_GOTO(14),   // -> bass_bar
  ATM_SET_TRA(0),
  ATM_GOTO(14),   // -> bass_bar
  ATM_GOTO(11),   // -> bass  (loop: this very track)

  //"Track 12" bass_head  [8b]
  ATM_VOL(0),
  ATM_DELAY(4),
  ATM_VOL(63),
  ATM_NOTE_D3,
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 13" bass_beat  [7b]
  ATM_GOTO(12),   // -> bass_head
  ATM_NOTE_D4,
  ATM_DELAY(4),
  ATM_NOTE_D3,
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 14" bass_bar  [10b]
  ATM_REPEAT(2, 13),   // 3x bass_beat
  ATM_GOTO(12),   // -> bass_head
  ATM_NOTE_D3,
  ATM_DELAY(4),
  ATM_NOTE_D4,
  ATM_DELAY(4),
  ATM_RETURN,

};

// Double Zero - Bond-style, E minor with minor-major-7 colour, 3-3-2 stabs, chromatic hook. Tempo 36 (135 BPM).
Song spyDoubleZero[] = {     // total song bytes = 255
  0x0F,                       // Number of tracks
  0x00, 0x00,                 // Address of track 0      0   drums
  0x0D, 0x00,                 // Address of track 1     13   half
  0x12, 0x00,                 // Address of track 2     18   beat_A
  0x20, 0x00,                 // Address of track 3     32   beat_B
  0x2B, 0x00,                 // Address of track 4     43   fill
  0x3A, 0x00,                 // Address of track 5     58   lead
  0x5C, 0x00,                 // Address of track 6     92   r1
  0x6D, 0x00,                 // Address of track 7    109   r2
  0x78, 0x00,                 // Address of track 8    120   chords
  0x94, 0x00,                 // Address of track 9    148   st_em
  0x9E, 0x00,                 // Address of track 10   158   st_b7
  0xA8, 0x00,                 // Address of track 11   168   bass
  0xC8, 0x00,                 // Address of track 12   200   bar
  0xCE, 0x00,                 // Address of track 13   206   beat
  0xD5, 0x00,                 // Address of track 14   213   beat4

  0x05,                         // CH0 entry -> track 5 (lead)
  0x08,                         // CH1 entry -> track 8 (chords)
  0x0B,                         // CH2 entry -> track 11 (bass)
  0x00,                         // CH3 entry -> track 0 (drums)

  //"Track 0" drums  [13b]
  ATM_REPEAT(13, 1),   // 14x half
  ATM_GOTO(2),   // -> beat_A
  ATM_GOTO(3),   // -> beat_B
  ATM_GOTO(2),   // -> beat_A
  ATM_GOTO(4),   // -> fill
  ATM_GOTO(0),   // -> drums

  //"Track 1" half  [5b]
  ATM_GOTO(2),   // -> beat_A
  ATM_GOTO(3),   // -> beat_B
  ATM_RETURN,

  //"Track 2" beat_A  [14b]
  ATM_VOL(MV(48)),
  ATM_SL_VOL(MS(12)),
  ATM_DELAY(4),
  ATM_VOL(MV(14)),
  ATM_SL_VOL(MS(7)),
  ATM_DELAY(4),
  ATM_VOL(MV(14)),
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 3" beat_B  [11b]
  ATM_VOL(MV(44)),
  ATM_SL_VOL(MS(5)),
  ATM_DELAY(8),
  ATM_VOL(MV(26)),
  ATM_SL_VOL(MS(3)),
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 4" fill  [15b]
  ATM_VOL(MV(30)),
  ATM_SL_VOL(MS(4)),
  ATM_DELAY(4),
  ATM_VOL(MV(38)),
  ATM_DELAY(4),
  ATM_VOL(MV(46)),
  ATM_DELAY(4),
  ATM_VOL(MV(54)),
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 5" lead  [34b]
  ATM_VOL(MV(38)),
  ATM_SL_VOL(MS(2)),
  ATM_SET_TRA(0),
  ATM_GOTO(6),   // -> r1
  ATM_GOTO(7),   // -> r2
  ATM_ADD_TRA((uint8_t)-4),
  ATM_GOTO(6),   // -> r1
  ATM_ADD_TRA((uint8_t)-1),
  ATM_GOTO(7),   // -> r2
  ATM_SET_TRA(0),
  ATM_GOTO(6),   // -> r1
  ATM_GOTO(7),   // -> r2
  ATM_SET_TRA(5),
  ATM_GOTO(6),   // -> r1
  ATM_SET_TRA((uint8_t)-5),
  ATM_GOTO(7),   // -> r2
  ATM_GOTO(5),   // -> lead

  //"Track 6" r1  [17b]
  ATM_NOTE_E5,
  ATM_DELAY(12),
  ATM_NOTE_D5_,
  ATM_DELAY(4),
  ATM_NOTE_E5,
  ATM_DELAY(8),
  ATM_NOTE_G5,
  ATM_DELAY(8),
  ATM_NOTE_F5_,
  ATM_DELAY(8),
  ATM_NOTE_E5,
  ATM_DELAY(8),
  ATM_NOTE_D5_,
  ATM_DELAY(8),
  ATM_NOTE_B4,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 7" r2  [11b]
  ATM_NOTE_E5,
  ATM_DELAY(24),
  ATM_NOTE_B4,
  ATM_DELAY(8),
  ATM_NOTE_G4,
  ATM_DELAY(8),
  ATM_NOTE_A4,
  ATM_DELAY(8),
  ATM_NOTE_B4,
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 8" chords  [28b]
  ATM_VOL(MV(30)),
  ATM_SL_VOL(MS(3)),
  ATM_REPEAT(1, 9),   // 2x st_em
  ATM_NOTE_C4,
  ATM_DELAY(24),
  ATM_NOTE_C4,
  ATM_DELAY(24),
  ATM_NOTE_C4,
  ATM_DELAY(16),
  ATM_GOTO(10),   // -> st_b7
  ATM_REPEAT(1, 9),   // 2x st_em
  ATM_NOTE_A3,
  ATM_DELAY(24),
  ATM_NOTE_A3,
  ATM_DELAY(24),
  ATM_NOTE_A3,
  ATM_DELAY(16),
  ATM_GOTO(10),   // -> st_b7
  ATM_GOTO(8),   // -> chords

  //"Track 9" st_em  [10b]
  ATM_ARP(0x38, 0x20),
  ATM_NOTE_E4,
  ATM_DELAY(24),
  ATM_NOTE_E4,
  ATM_DELAY(24),
  ATM_NOTE_E4,
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 10" st_b7  [10b]
  ATM_ARP(0x46, 0x20),
  ATM_NOTE_B3,
  ATM_DELAY(24),
  ATM_NOTE_B3,
  ATM_DELAY(24),
  ATM_NOTE_B3,
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 11" bass  [32b]
  ATM_VOL(MV(63)),
  ATM_SL_VOL(MS(6)),
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 12),   // 2x bar
  ATM_SET_TRA((uint8_t)-4),
  ATM_GOTO(12),   // -> bar
  ATM_SET_TRA((uint8_t)-5),
  ATM_GOTO(12),   // -> bar
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 12),   // 2x bar
  ATM_SET_TRA((uint8_t)-7),
  ATM_GOTO(12),   // -> bar
  ATM_SET_TRA((uint8_t)-5),
  ATM_GOTO(12),   // -> bar
  ATM_GOTO(11),   // -> bass

  //"Track 12" bar  [6b]
  ATM_REPEAT(2, 13),   // 3x beat
  ATM_GOTO(14),   // -> beat4
  ATM_RETURN,

  //"Track 13" beat  [7b]
  ATM_NOTE_E3,
  ATM_DELAY(8),
  ATM_NOTE_E3,
  ATM_DELAY(4),
  ATM_NOTE_E4,
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 14" beat4  [7b]
  ATM_NOTE_E3,
  ATM_DELAY(8),
  ATM_NOTE_E3,
  ATM_DELAY(4),
  ATM_NOTE_B3,
  ATM_DELAY(4),
  ATM_RETURN,

};

// Shadow Walk - stealth, D minor, short plucks and leading tones, slow tick-tock. Tempo 26 (97 BPM).
Song spyShadowWalk[] = {     // total song bytes = 171
  0x08,                       // Number of tracks
  0x00, 0x00,                 // Address of track 0      0   drums
  0x05, 0x00,                 // Address of track 1      5   pair
  0x10, 0x00,                 // Address of track 2     16   lead
  0x32, 0x00,                 // Address of track 3     50   r1
  0x3D, 0x00,                 // Address of track 4     61   r2
  0x46, 0x00,                 // Address of track 5     70   chords
  0x6B, 0x00,                 // Address of track 6    107   bass
  0x8B, 0x00,                 // Address of track 7    139   bar

  0x02,                         // CH0 entry -> track 2 (lead)
  0x05,                         // CH1 entry -> track 5 (chords)
  0x06,                         // CH2 entry -> track 6 (bass)
  0x00,                         // CH3 entry -> track 0 (drums)

  //"Track 0" drums  [5b]
  ATM_REPEAT(15, 1),   // 16x pair
  ATM_GOTO(0),   // -> drums

  //"Track 1" pair  [11b]
  ATM_VOL(MV(14)),
  ATM_SL_VOL(MS(8)),
  ATM_DELAY(16),
  ATM_VOL(MV(30)),
  ATM_SL_VOL(MS(10)),
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 2" lead  [34b]
  ATM_VOL(MV(38)),
  ATM_SL_VOL(MS(3)),
  ATM_SET_TRA(0),
  ATM_GOTO(3),   // -> r1
  ATM_GOTO(4),   // -> r2
  ATM_SET_TRA(5),
  ATM_GOTO(3),   // -> r1
  ATM_SET_TRA(7),
  ATM_GOTO(4),   // -> r2
  ATM_SET_TRA(0),
  ATM_GOTO(3),   // -> r1
  ATM_GOTO(4),   // -> r2
  ATM_SET_TRA((uint8_t)-4),
  ATM_GOTO(3),   // -> r1
  ATM_SET_TRA(7),
  ATM_GOTO(4),   // -> r2
  ATM_GOTO(2),   // -> lead

  //"Track 3" r1  [11b]
  ATM_NOTE_D5,
  ATM_DELAY(16),
  ATM_NOTE_F5,
  ATM_DELAY(8),
  ATM_NOTE_E5,
  ATM_DELAY(8),
  ATM_NOTE_C5_,
  ATM_DELAY(16),
  ATM_NOTE_D5,
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 4" r2  [9b]
  ATM_NOTE_A4,
  ATM_DELAY(24),
  ATM_NOTE_A4_,
  ATM_DELAY(8),
  ATM_NOTE_A4,
  ATM_DELAY(16),
  ATM_NOTE_G4,
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 5" chords  [37b]
  ATM_VOL(MV(26)),
  ATM_SL_VOL(MS(1)),
  ATM_ARP(0x34, 0x23),
  ATM_NOTE_D4,
  ATM_DELAY(64),
  ATM_NOTE_D4,
  ATM_DELAY(64),
  ATM_NOTE_G3,
  ATM_DELAY(64),
  ATM_ARP(0x46, 0x23),
  ATM_NOTE_A3,
  ATM_DELAY(64),
  ATM_ARP(0x34, 0x23),
  ATM_NOTE_D4,
  ATM_DELAY(64),
  ATM_NOTE_D4,
  ATM_DELAY(64),
  ATM_ARP(0x43, 0x23),
  ATM_NOTE_A3_,
  ATM_DELAY(64),
  ATM_ARP(0x46, 0x23),
  ATM_NOTE_A3,
  ATM_DELAY(64),
  ATM_GOTO(5),   // -> chords

  //"Track 6" bass  [32b]
  ATM_VOL(MV(63)),
  ATM_SL_VOL(MS(10)),
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 7),   // 2x bar
  ATM_SET_TRA(5),
  ATM_GOTO(7),   // -> bar
  ATM_SET_TRA(7),
  ATM_GOTO(7),   // -> bar
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 7),   // 2x bar
  ATM_SET_TRA((uint8_t)-4),
  ATM_GOTO(7),   // -> bar
  ATM_SET_TRA(7),
  ATM_GOTO(7),   // -> bar
  ATM_GOTO(6),   // -> bass

  //"Track 7" bar  [11b]
  ATM_NOTE_D3,
  ATM_DELAY(16),
  ATM_NOTE_D3,
  ATM_DELAY(8),
  ATM_NOTE_D3,
  ATM_DELAY(8),
  ATM_NOTE_A3,
  ATM_DELAY(16),
  ATM_NOTE_D3,
  ATM_DELAY(16),
  ATM_RETURN,

};

// Chase Sequence - G minor, chugging square, syncopated saw bass, 16th riff. Tempo 40 (150 BPM).
Song spyChase[] = {     // total song bytes = 231
  0x0D,                       // Number of tracks
  0x00, 0x00,                 // Address of track 0      0   drums
  0x0D, 0x00,                 // Address of track 1     13   half
  0x12, 0x00,                 // Address of track 2     18   beat_A
  0x23, 0x00,                 // Address of track 3     35   beat_B
  0x31, 0x00,                 // Address of track 4     49   fill
  0x40, 0x00,                 // Address of track 5     64   lead
  0x60, 0x00,                 // Address of track 6     96   riff
  0x71, 0x00,                 // Address of track 7    113   chug
  0x95, 0x00,                 // Address of track 8    149   chug1
  0x98, 0x00,                 // Address of track 9    152   bass
  0xB8, 0x00,                 // Address of track 10   184   bar
  0xBE, 0x00,                 // Address of track 11   190   beat
  0xC3, 0x00,                 // Address of track 12   195   beat4

  0x05,                         // CH0 entry -> track 5 (lead)
  0x07,                         // CH1 entry -> track 7 (chug)
  0x09,                         // CH2 entry -> track 9 (bass)
  0x00,                         // CH3 entry -> track 0 (drums)

  //"Track 0" drums  [13b]
  ATM_REPEAT(13, 1),   // 14x half
  ATM_GOTO(2),   // -> beat_A
  ATM_GOTO(3),   // -> beat_B
  ATM_GOTO(2),   // -> beat_A
  ATM_GOTO(4),   // -> fill
  ATM_GOTO(0),   // -> drums

  //"Track 1" half  [5b]
  ATM_GOTO(2),   // -> beat_A
  ATM_GOTO(3),   // -> beat_B
  ATM_RETURN,

  //"Track 2" beat_A  [17b]
  ATM_VOL(MV(48)),
  ATM_SL_VOL(MS(12)),
  ATM_DELAY(4),
  ATM_VOL(MV(14)),
  ATM_SL_VOL(MS(7)),
  ATM_DELAY(4),
  ATM_VOL(MV(14)),
  ATM_DELAY(4),
  ATM_VOL(MV(14)),
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 3" beat_B  [14b]
  ATM_VOL(MV(44)),
  ATM_SL_VOL(MS(5)),
  ATM_DELAY(8),
  ATM_VOL(MV(14)),
  ATM_SL_VOL(MS(7)),
  ATM_DELAY(4),
  ATM_VOL(MV(14)),
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 4" fill  [15b]
  ATM_VOL(MV(30)),
  ATM_SL_VOL(MS(4)),
  ATM_DELAY(4),
  ATM_VOL(MV(38)),
  ATM_DELAY(4),
  ATM_VOL(MV(46)),
  ATM_DELAY(4),
  ATM_VOL(MV(54)),
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 5" lead  [32b]
  ATM_VOL(MV(38)),
  ATM_SL_VOL(MS(2)),
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 6),   // 2x riff
  ATM_SET_TRA(5),
  ATM_GOTO(6),   // -> riff
  ATM_SET_TRA(7),
  ATM_GOTO(6),   // -> riff
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 6),   // 2x riff
  ATM_SET_TRA(5),
  ATM_GOTO(6),   // -> riff
  ATM_SET_TRA(7),
  ATM_GOTO(6),   // -> riff
  ATM_GOTO(5),   // -> lead

  //"Track 6" riff  [17b]
  ATM_NOTE_G4,
  ATM_DELAY(8),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_G5,
  ATM_DELAY(8),
  ATM_NOTE_F5,
  ATM_DELAY(8),
  ATM_NOTE_D5,
  ATM_DELAY(12),
  ATM_NOTE_C5,
  ATM_DELAY(4),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_G4,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 7" chug  [36b]
  ATM_VOL(MV(26)),
  ATM_SL_VOL(MS(3)),
  ATM_SET_TRA(0),
  ATM_REPEAT(15, 8),   // 16x chug1
  ATM_SET_TRA(5),
  ATM_REPEAT(7, 8),   // 8x chug1
  ATM_SET_TRA(7),
  ATM_REPEAT(7, 8),   // 8x chug1
  ATM_SET_TRA(0),
  ATM_REPEAT(15, 8),   // 16x chug1
  ATM_SET_TRA(5),
  ATM_REPEAT(7, 8),   // 8x chug1
  ATM_SET_TRA(7),
  ATM_REPEAT(7, 8),   // 8x chug1
  ATM_GOTO(7),   // -> chug

  //"Track 8" chug1  [3b]
  ATM_NOTE_D4,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 9" bass  [32b]
  ATM_VOL(MV(63)),
  ATM_SL_VOL(MS(4)),
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 10),   // 2x bar
  ATM_SET_TRA(5),
  ATM_GOTO(10),   // -> bar
  ATM_SET_TRA(7),
  ATM_GOTO(10),   // -> bar
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 10),   // 2x bar
  ATM_SET_TRA(5),
  ATM_GOTO(10),   // -> bar
  ATM_SET_TRA(7),
  ATM_GOTO(10),   // -> bar
  ATM_GOTO(9),   // -> bass

  //"Track 10" bar  [6b]
  ATM_REPEAT(2, 11),   // 3x beat
  ATM_GOTO(12),   // -> beat4
  ATM_RETURN,

  //"Track 11" beat  [5b]
  ATM_NOTE_G2,
  ATM_DELAY(12),
  ATM_NOTE_G2,
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 12" beat4  [5b]
  ATM_NOTE_G2,
  ATM_DELAY(8),
  ATM_NOTE_D3,
  ATM_DELAY(8),
  ATM_RETURN,

};

// The Vault - suspense, B minor, heartbeat bass, bells with vibrato, F#7 tension and a diminished bar. Tempo 24 (90 BPM).
Song spyVault[] = {     // total song bytes = 180
  0x07,                       // Number of tracks
  0x00, 0x00,                 // Address of track 0      0   drums
  0x05, 0x00,                 // Address of track 1      5   dbar
  0x1D, 0x00,                 // Address of track 2     29   lead
  0x45, 0x00,                 // Address of track 3     69   r1
  0x4C, 0x00,                 // Address of track 4     76   chords
  0x77, 0x00,                 // Address of track 5    119   bass
  0x9B, 0x00,                 // Address of track 6    155   beat

  0x02,                         // CH0 entry -> track 2 (lead)
  0x04,                         // CH1 entry -> track 4 (chords)
  0x05,                         // CH2 entry -> track 5 (bass)
  0x00,                         // CH3 entry -> track 0 (drums)

  //"Track 0" drums  [5b]
  ATM_REPEAT(7, 1),   // 8x dbar
  ATM_GOTO(0),   // -> drums

  //"Track 1" dbar  [24b]
  ATM_VOL(MV(36)),
  ATM_SL_VOL(MS(9)),
  ATM_DELAY(16),
  ATM_VOL(MV(14)),
  ATM_SL_VOL(MS(8)),
  ATM_DELAY(8),
  ATM_VOL(MV(14)),
  ATM_DELAY(8),
  ATM_VOL(MV(36)),
  ATM_SL_VOL(MS(9)),
  ATM_DELAY(16),
  ATM_VOL(MV(14)),
  ATM_SL_VOL(MS(8)),
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 2" lead  [40b]
  ATM_VOL(MV(38)),
  ATM_SL_VOL(MS(2)),
  ATM_VIB(2, 3),
  ATM_SET_TRA(0),
  ATM_GOTO(3),   // -> r1
  ATM_DELAY(64),
  ATM_SET_TRA((uint8_t)-4),
  ATM_GOTO(3),   // -> r1
  ATM_SET_TRA((uint8_t)-5),
  ATM_GOTO(3),   // -> r1
  ATM_SET_TRA(0),
  ATM_GOTO(3),   // -> r1
  ATM_NOTE_F5,
  ATM_DELAY(16),
  ATM_NOTE_D5,
  ATM_DELAY(16),
  ATM_NOTE_B4,
  ATM_DELAY(32),
  ATM_SET_TRA(5),
  ATM_GOTO(3),   // -> r1
  ATM_SET_TRA((uint8_t)-5),
  ATM_GOTO(3),   // -> r1
  ATM_GOTO(2),   // -> lead

  //"Track 3" r1  [7b]
  ATM_NOTE_F5_,
  ATM_DELAY(32),
  ATM_NOTE_B5,
  ATM_DELAY(16),
  ATM_NOTE_A5,
  ATM_DELAY(16),
  ATM_RETURN,

  //"Track 4" chords  [43b]
  ATM_VOL(MV(26)),
  ATM_SL_VOL(MS(1)),
  ATM_ARP(0x34, 0x23),
  ATM_NOTE_B3,
  ATM_DELAY(64),
  ATM_NOTE_B3,
  ATM_DELAY(64),
  ATM_ARP(0x43, 0x23),
  ATM_NOTE_G3,
  ATM_DELAY(64),
  ATM_ARP(0x46, 0x23),
  ATM_NOTE_F3_,
  ATM_DELAY(64),
  ATM_ARP(0x34, 0x23),
  ATM_NOTE_B3,
  ATM_DELAY(64),
  ATM_ARP(0x33, 0x23),
  ATM_NOTE_B3,
  ATM_DELAY(64),
  ATM_ARP(0x34, 0x23),
  ATM_NOTE_E4,
  ATM_DELAY(64),
  ATM_ARP(0x46, 0x23),
  ATM_NOTE_F3_,
  ATM_DELAY(64),
  ATM_GOTO(4),   // -> chords

  //"Track 5" bass  [36b]
  ATM_VOL(MV(63)),
  ATM_SL_VOL(MS(10)),
  ATM_SET_TRA(0),
  ATM_REPEAT(7, 6),   // 8x beat
  ATM_SET_TRA((uint8_t)-4),
  ATM_REPEAT(3, 6),   // 4x beat
  ATM_SET_TRA((uint8_t)-5),
  ATM_REPEAT(3, 6),   // 4x beat
  ATM_SET_TRA(0),
  ATM_REPEAT(7, 6),   // 8x beat
  ATM_SET_TRA(5),
  ATM_REPEAT(3, 6),   // 4x beat
  ATM_SET_TRA((uint8_t)-5),
  ATM_REPEAT(3, 6),   // 4x beat
  ATM_GOTO(5),   // -> bass

  //"Track 6" beat  [6b]
  ATM_NOTE_B2,
  ATM_DELAY(4),
  ATM_NOTE_B2,
  ATM_DELAY(4),
  ATM_DELAY(8),
  ATM_RETURN,

};

// In-game songs: level 1 plays song 0, level 2 song 1 ... level 5 starts over with song 0.
// The tempo is not in the songs, the sketch sets it per song (playGameMusic in game.h).
const unsigned char * const PROGMEM gameSongs[] =
{
  spyDoubleZero, spyShadowWalk, spyChase, spyVault
};

const uint8_t PROGMEM gameSongTempo[] = { 36, 26, 40, 24 };

// Finishing tune (played once the game is completed), looped until A/B. Full volume like the title song.
// Tempo is set by the sketch (stateGameNextLevel): ATM.setTempo(34).
// Happy Disco - finishing tune, 512 ticks/loop, tempo 34
Song finishDisco[] = {     // total song bytes = 144
  0x08,                       // Number of tracks
  0x00, 0x00,                 // Address of track 0      0   lead
  0x0B, 0x00,                 // Address of track 1     11   riff
  0x1A, 0x00,                 // Address of track 2     26   cycL
  0x2B, 0x00,                 // Address of track 3     43   harm
  0x36, 0x00,                 // Address of track 4     54   bass
  0x3F, 0x00,                 // Address of track 5     63   beat
  0x4B, 0x00,                 // Address of track 6     75   cycB
  0x60, 0x00,                 // Address of track 7     96   drum

  0x00,                         // CH0 entry -> track 0 (lead)
  0x03,                         // CH1 entry -> track 3 (harm)
  0x04,                         // CH2 entry -> track 4 (bass)
  0x07,                         // CH3 entry -> track 7 (drum)

  //"Track 0" lead  [11b]
  ATM_SET_TRA(0),
  ATM_VOL(MV(38)),
  ATM_SL_VOL(MS(2)),
  ATM_REPEAT(1, 2),   // 2x cycL
  ATM_GOTO(0),   // -> lead

  //"Track 1" riff  [15b]
  ATM_NOTE_G4,
  ATM_DELAY(8),
  ATM_NOTE_C5,
  ATM_DELAY(8),
  ATM_NOTE_E5,
  ATM_DELAY(8),
  ATM_NOTE_G5,
  ATM_DELAY(8),
  ATM_NOTE_E5,
  ATM_DELAY(16),
  ATM_NOTE_D5,
  ATM_DELAY(8),
  ATM_NOTE_C5,
  ATM_DELAY(8),
  ATM_RETURN,

  //"Track 2" cycL  [17b]
  ATM_GOTO(1),   // -> riff
  ATM_ADD_TRA(5),
  ATM_GOTO(1),   // -> riff
  ATM_ADD_TRA(2),
  ATM_GOTO(1),   // -> riff
  ATM_ADD_TRA((uint8_t)-7),
  ATM_GOTO(1),   // -> riff
  ATM_ADD_TRA(7),
  ATM_RETURN,

  //"Track 3" harm  [11b]
  ATM_SET_TRA((uint8_t)-12),
  ATM_VOL(MV(20)),
  ATM_SL_VOL(MS(1)),
  ATM_REPEAT(1, 2),   // 2x cycL
  ATM_GOTO(3),   // -> harm

  //"Track 4" bass  [9b]
  ATM_SL_VOL(MS(5)),
  ATM_SET_TRA(0),
  ATM_REPEAT(1, 6),   // 2x cycB
  ATM_GOTO(4),   // -> bass

  //"Track 5" beat  [12b]
  ATM_VOL(MV(0)),
  ATM_DELAY(4),
  ATM_VOL(MV(63)),
  ATM_NOTE_C3,
  ATM_DELAY(4),
  ATM_NOTE_C4,
  ATM_DELAY(4),
  ATM_NOTE_C3,
  ATM_DELAY(4),
  ATM_RETURN,

  //"Track 6" cycB  [21b]
  ATM_REPEAT(3, 5),   // 4x beat
  ATM_ADD_TRA(5),
  ATM_REPEAT(3, 5),   // 4x beat
  ATM_ADD_TRA(2),
  ATM_REPEAT(3, 5),   // 4x beat
  ATM_ADD_TRA((uint8_t)-7),
  ATM_REPEAT(3, 5),   // 4x beat
  ATM_ADD_TRA(7),
  ATM_RETURN,

  //"Track 7" drum  [27b]
  ATM_VOL(MV(28)),
  ATM_SL_VOL(MS(14)),
  ATM_DELAY(4),
  ATM_VOL(MV(12)),
  ATM_SL_VOL(MS(6)),
  ATM_DELAY(4),
  ATM_VOL(MV(26)),
  ATM_SL_VOL(MS(3)),
  ATM_DELAY(8),
  ATM_VOL(MV(40)),
  ATM_SL_VOL(MS(5)),
  ATM_DELAY(8),
  ATM_VOL(MV(26)),
  ATM_SL_VOL(MS(3)),
  ATM_DELAY(8),
  ATM_GOTO(7),   // -> drum

};

#endif
