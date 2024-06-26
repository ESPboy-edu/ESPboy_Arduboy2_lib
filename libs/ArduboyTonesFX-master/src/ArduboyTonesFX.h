/**
 * @file ArduboyTones.h
 * \brief An Arduino library for playing tones and tone sequences, 
 * intended for the Arduboy game system.
 */

/*****************************************************************************
  ArduboyTones

An Arduino library to play tones and tone sequences.

Specifically written for use by the Arduboy miniature game system
https://www.arduboy.com/
but could work with other Arduino AVR boards that have 16 bit timer 3
available, by changing the port and bit definintions for the pin(s)
if necessary.

Copyright (c) 2017 Scott Allen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*****************************************************************************/

#ifndef ARDUBOY_TONES_H
#define ARDUBOY_TONES_H

#include <Arduino.h>
#include <ArduboyFX.h>

// ************************************************************
// ***** Values to use as function parameters in sketches *****
// ************************************************************

/** \brief
 * Frequency value for sequence termination. (No duration follows)
 */
#define TONES_END 0x8000

/** \brief
 * Frequency value for sequence repeat. (No duration follows)
 */
#define TONES_REPEAT 0x8001


#define TONES_MODE_NORMAL 0
#define TONES_MODE_FX     1

// ************************************************************



// ***** CONTROL THE TIMER CLOCK PRESCALER ****
// Uncommenting this will switch the timer clock to use no prescaler,
// instead of a divide by 8 prescaler, if the frequency is high enough to
// allow it. This will result in higher frequencies being more accurate at
// the expense of requiring more code. If left commented out, a divide by 8
// prescaler will be used for all frequencies.
//#define TONES_ADJUST_PRESCALER
// ********************************************

// This must match the maximum number of tones that can be specified in
// the tone() function.
#define MAX_TONES 3


// Dummy frequency used to for silent tones (rests).
#define SILENT_FREQ 250


/** \brief
 * The ArduboyTones class for generating tones by specifying
 * frequency/duration pairs.
 */
class ArduboyTonesFX
{
 public:

  /** \brief
   * The ArduboyTones class constructor.
   *
   * \param outEn A function which returns a boolean value of `true` if sound
   * should be played or `false` if sound should be muted. This function will
   * be called from the timer interrupt service routine, at the start of each
   * tone, so it should be as fast as possible.
   */
  ArduboyTonesFX(bool (*outEn)());

  /** \brief
   * The ArduboyTones class constructor for use when using FX data.
   *
   * \param outEn A function which returns a boolean value of `true` if sound
   * should be played or `false` if sound should be muted. This function will
   * be called from the timer interrupt service routine, at the start of each
   * tone, so it should be as fast as possible.
   * \param tonesBufferFX pointer to a predefined buffer of type `uint16_t[]`.
   * \param tonesBufferLen buffer length.  Note this is the number of array 
   * elements rather than the number of bytes it occupies,
   */
  ArduboyTonesFX(bool (*outEn)(), uint16_t *tonesBufferFX, uint8_t tonesBufferLen);

  /** \brief
   * The ArduboyTones class constructor for use when using FX data.  Overload
   * of above.
   *
   * \param outEn A function which returns a boolean value of `true` if sound
   * should be played or `false` if sound should be muted. This function will
   * be called from the timer interrupt service routine, at the start of each
   * tone, so it should be as fast as possible.
   * \param tonesBufferFX pointer to a predefined buffer of type `uint16_t[]`.
   */
  template<size_t size>
  ArduboyTonesFX(boolean (*enabled)(), uint16_t (&buffer)[size]) :
    ArduboyTonesFX(enabled, buffer, size)
  {
    static_assert(size < 256, "Buffer too large");
  }


  static void checkTones();


  /** \brief
   * Play a single tone.
   *
   * \param freq The frequency of the tone, in hertz.
   * \param dur The duration to play the tone for, in 1024ths of a
   * second (very close to milliseconds). A duration of 0, or if not provided,
   * means play forever, or until `noTone()` is called or a new tone or
   * sequence is started.
   */
  static void tone(uint16_t freq, uint16_t dur = 0);

  /** \brief
   * Play two tones in sequence.
   *
   * \param freq1,freq2 The frequency of the tone in hertz.
   * \param dur1,dur2 The duration to play the tone for, in 1024ths of a
   * second (very close to milliseconds).
   */
  static void tone(uint16_t freq1, uint16_t dur1,
                   uint16_t freq2, uint16_t dur2);

  /** \brief
   * Play three tones in sequence.
   *
   * \param freq1,freq2,freq3 The frequency of the tone, in hertz.
   * \param dur1,dur2,dur3 The duration to play the tone for, in 1024ths of a
   * second (very close to milliseconds).
   */
  static void tone(uint16_t freq1, uint16_t dur1,
                   uint16_t freq2, uint16_t dur2,
                   uint16_t freq3, uint16_t dur3);

  /** \brief
   * Play a tone sequence from frequency/duration pairs in a PROGMEM array.
   *
   * \param tones A pointer to an array of frequency/duration pairs.
   * The array must be placed in code space using `PROGMEM`.
   *
   * \details 
   * \parblock
   * See the `tone()` function for details on the frequency and duration values.
   * A frequency of 0 for any tone means silence (a musical rest).
   *
   * The last element of the array must be `TONES_END` or `TONES_REPEAT`.
   *
   * Example:
   *
   * \code
   * const uint16_t sound1[] PROGMEM = {
   *   220,1000, 0,250, 440,500, 880,2000,
   *   TONES_END
   * };
   * \endcode
   *
   * \endparblock
   */
  static void tones(const uint16_t *tones);

  /** \brief
   * Play a tone sequence from frequency/duration pairs in an array in RAM.
   *
   * \param tones A pointer to an array of frequency/duration pairs.
   * The array must be located in RAM.
   *
   * \see tones()
   *
   * \details 
   * \parblock
   * See the `tone()` function for details on the frequency and duration values.
   * A frequency of 0 for any tone means silence (a musical rest).
   *
   * The last element of the array must be `TONES_END` or `TONES_REPEAT`.
   *
   * Example:
   *
   * \code
   * uint16_t sound2[] = {
   *   220,1000, 0,250, 440,500, 880,2000,
   *   TONES_END
   * };
   * \endcode
   *
   * \endparblock
   *
   * \note Using `tones()`, with the data in PROGMEM, is normally a better
   * choice. The only reason to use tonesInRAM() would be if dynamically
   * altering the contents of the array is required.
   */
  static void tonesInRAM(uint16_t *tones);

  /** \brief
   * Play a tone sequence from the FX chip.
   *
   * \details
   *  Play a tone sequence from the FX chip.
   */
  static void tonesFromFX(uint24_t tones);

  /** \brief
   * Fill the FX buffer with data.
   *
   * \details
   *  Fill the FX buffer with data.  This should be placed at the start of the
   *  main game loop, typically after 'if (!arduboy.nextFrame()) return;'.
   */
  static void fillBufferFromFX();

  /** \brief
   * Stop playing the tone or sequence.
   *
   * \details
   * If a tone or sequence is playing, it will stop. If nothing
   * is playing, this function will do nothing.
   */
  static void noTone();

  /** \brief
   * Set the volume to always normal, always high, or tone controlled.
   *
   * \param mode
   * \parblock
   * One of the following values should be used:
   *
   * - `VOLUME_IN_TONE` The volume of each tone will be specified in the tone
   *    itself.
   * - `VOLUME_ALWAYS_NORMAL` All tones will play at the normal volume level.
   * - `VOLUME_ALWAYS_HIGH` All tones will play at the high volume level.
   *
   * \endparblock
   */
  static void volumeMode(uint8_t mode);

  /** \brief
   * Check if a tone or tone sequence is playing.
   *
   * \return boolean `true` if playing (even if sound is muted).
   */
  static bool playing();

private:
  // Get the next value in the sequence
  static uint16_t getNext();
  static void nextTone();
};

#include "ArduboyTonesPitches.h"

#endif
