#ifndef ArduboyAudio_h
#define ArduboyAudio_h

#include <Arduino.h>
#include <avr/pgmspace.h>
//#include <avr/power.h>

#define AVAILABLE_TIMERS 2
#define TUNE_OP_PLAYNOTE  0x90  /* play a note: low nibble is generator #, note is next byte */
#define TUNE_OP_STOPNOTE  0x80  /* stop a note: low nibble is generator # */
#define TUNE_OP_RESTART   0xe0  /* restart the score from the beginning */
#define TUNE_OP_STOP      0xf0  /* stop playing */


class ArduboyAudio
{
public:
  void static begin();
  void static on();
  void static off();
};


class ArduboyTunes
{
public:
  // Playtune Functions

  /// Assign a timer to an output pin.
  void initChannel(byte pin, byte chan);

  /// Stop all timers.
  void closeChannels();

  void static playNote (byte chan, byte note);
  void static stopNote (byte chan);
};
#endif
