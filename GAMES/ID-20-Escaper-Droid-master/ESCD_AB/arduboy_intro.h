#ifndef ARDUBOY_INTRO_H
#define ARDUBOY_INTRO_H

// ARDUBOY intro, the small version: the 7 letters slide in from the right one after the other, each letter lands on a note.
// No cues and no letter state: the position of every letter is a function of ONE frame counter, and the sound effect is
// started once. The timing is exact: at setFrameRate(60) a frame is 16 ms and an ATMlib tick is 40 ms (tempo 25),
// so 5 frames = 2 ticks. A letter every 15 frames = 240 ms = 6 ticks keeps the notes and the letters locked together.
//
// Needs (declared before this file):  Arduboy2Base arduboy;  Sprites sprites;  ATMsynth ATM;   and setFrameRate(60).
// The octave 2 of ATMlib must be on (default). Use it like this:
//
//     byte introFrame = 0;
//     loop():  if (introFrame < INTRO_FRAMES) arduboyIntro(introFrame++);  else { ... the game / the logo ... }
//
// Do not start a song before INTRO_FRAMES: the last note is still sounding on channel 1.

// Defaults for setFrameRate(60). For another frame rate define these BEFORE including this file (see the table below).
#ifndef INTRO_STEP
#define INTRO_STEP        15      // frames between two letters (15 x 16 ms = 240 ms = 6 ticks)
#define INTRO_TICKS       6       // ATM ticks between two notes (a tick is 40 ms at the default tempo 25)
#define INTRO_SLIDE       20      // frames a letter needs to slide in, it lands on its note
#define INTRO_SPEED       6       // pixels per frame while sliding (SLIDE x SPEED must be at least 120)
#define INTRO_TAIL        20      // frames the finished word stays (about the ring of the last note)
#endif
#define INTRO_FRAMES      (6 * INTRO_STEP + INTRO_SLIDE + INTRO_TAIL)   // frames until the intro is over (60 fps: 130)
//   fps  frame   STEP/TICKS    SLIDE x SPEED    TAIL
//    60   16 ms   15 / 6        20 x 6           20      exact: 5 frames = 2 ticks
//    45   22 ms   11 / 6        15 x 8           15      242 ms against 240 ms
//    30   33 ms    7 / 6        10 x 12          10      231 ms against 240 ms (or 6 / 5: 198 ms against 200 ms)

const unsigned char PROGMEM arduboyLetters[] =
{
  // width, height,
  8, 8,
  // FRAME 00 - 06 : A R D U B O Y
  0x38, 0x7d, 0x7d, 0x75, 0x75, 0x7f, 0x7e, 0x00, 0x7f, 0x7f, 0x7f, 0x0d, 0x0d, 0x7f, 0x76, 0x00, 0x7f, 0x7f, 0x7f, 0x79, 0x79, 0x7f, 0x3e, 0x00, 0x3f, 0x7f, 0x7f, 0x78, 0x78, 0x7f, 0x3f, 0x00, 0x7f, 0x7f, 0x7f, 0x75, 0x75, 0x7f, 0x3a, 0x00, 0x3e, 0x7f, 0x7f, 0x79, 0x79, 0x7f, 0x3e, 0x00, 0x4f, 0x5f, 0x5f, 0x58, 0x78, 0x7f, 0x3f, 0x00,
};

// one note per letter (E C G D A F B), INTRO_TICKS apart; the last note rings 2 ticks longer. Starts when the first letter lands.
ATM_SFX_TRACK(sfxIntro, ATM_VOL(48), ATM_SL_VOL((uint8_t)-4),
  ATM_NOTE_E2, ATM_DELAY(INTRO_TICKS), ATM_NOTE_C2, ATM_DELAY(INTRO_TICKS), ATM_NOTE_G2, ATM_DELAY(INTRO_TICKS), ATM_NOTE_D2, ATM_DELAY(INTRO_TICKS),
  ATM_NOTE_A2, ATM_DELAY(INTRO_TICKS), ATM_NOTE_F2, ATM_DELAY(INTRO_TICKS), ATM_NOTE_B2, ATM_DELAY(INTRO_TICKS + 2));

// draws frame f (0 .. INTRO_FRAMES - 1) of the intro
void arduboyIntro(byte f)
{
  if (f == INTRO_SLIDE && arduboy.audio.enabled()) ATM.playSfx(sfxIntro, 1);
  for (byte i = 0; i < 7; i++)
  {
    if (f < INTRO_STEP * i) break;                              // this letter has not started yet
    byte d = f - INTRO_STEP * i;                                // frames since it started
    byte x = 36 + 8 * i;                                        // its place in the row
    if (d < INTRO_SLIDE + 2) x += (d < INTRO_SLIDE) ? INTRO_SPEED * (INTRO_SLIDE - d) - 2 : d - (INTRO_SLIDE + 2);   // slides in 2 px too far, then bounces back   // still sliding in from the right
    sprites.drawSelfMasked(x, 28, arduboyLetters, i);
  }
}

#endif
