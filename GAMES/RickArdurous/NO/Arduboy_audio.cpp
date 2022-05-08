#include "CustomArduboy.h"
#include "Arduboy_audio.h"

const byte PROGMEM tune_pin_to_timer_PGM[] = { 3, 1 };
volatile byte *_tunes_timer1_pin_port;
volatile byte _tunes_timer1_pin_mask;
volatile byte *_tunes_timer3_pin_port;
volatile byte _tunes_timer3_pin_mask;
byte _tune_pins[AVAILABLE_TIMERS];

#define LOWEST_NOTE 52
#define FIRST_NOTE_IN_INT_ARRAY 52
#define HIGHEST_NOTE 82

// Table of midi note frequencies * 2
//   They are times 2 for greater accuracy, yet still fits in a word.
//   Generated from Excel by =ROUND(2*440/32*(2^((x-9)/12)),0) for 0<x<128
// The lowest notes might not work, depending on the Arduino clock frequency
// Ref: http://www.phy.mtu.edu/~suits/notefreqs.html
//const uint8_t PROGMEM _midi_byte_note_frequencies[] = {
//16,17,18,19,21,22,23,24,26,28, // note 0 to 9
//29,31,33,35,37,39,41,44,46,49, // note 10 to 19
//52,55,58,62,65,69,73,78,82,87, // note 20 to 29
//92,98,104,110,117,123,131,139,147,156, // note 30 to 39
//165,175,185,196,208,220,233,247 // note 40 to 47
//};
const unsigned int PROGMEM _midi_word_note_frequencies[] = {
//262,277,294, //note 48 to 50
/*311,*/330,349,370,392,415,440,466,494,523, //note 51 to 60
554,587,622,659,698,740,784,831,880,932, //note 61 to 70
988,1047,1109,1175,1245,1319,1397,1480,1568,1661, //note 71 to 80
1760,1865//,1976,2093,2217,2349,2489,2637,2794,2960, //note 81 to 90
//3136,3322,3520,3729,3951,4186,4435,4699,4978,5274, //note 91 to 100
//5588,5920,6272,6645,7040,7459,7902,8372,8870,9397, //note 101 to 110
//9956,10548,11175,11840,12544,13290,14080,14917,15804,16744, //note 111 to 120
//17740,18795,19912,21096,22351,23680,25088 //note 121 to 127
};

unsigned int NoteToFrequency(byte note)
{
	return pgm_read_word(_midi_word_note_frequencies + note - FIRST_NOTE_IN_INT_ARRAY);
}

/* AUDIO */

void ArduboyAudio::on()
{
  power_timer1_enable();
  power_timer3_enable();
}

void ArduboyAudio::off()
{
  power_timer1_disable();
  power_timer3_disable();
}

void ArduboyAudio::begin()
{
  on();
}

/* TUNES */

void ArduboyTunes::initChannel(byte pin, byte chan)
{
  byte timer_num;

  // we are all out of timers
  if (chan >= AVAILABLE_TIMERS)
    return;

  timer_num = pgm_read_byte(tune_pin_to_timer_PGM + chan);
  _tune_pins[chan] = pin;
  pinMode(pin, OUTPUT);
  switch (timer_num) {
    case 1: // 16 bit timer
      TCCR1A = 0;
      TCCR1B = 0;
      bitWrite(TCCR1B, WGM12, 1);
      bitWrite(TCCR1B, CS10, 1);
      _tunes_timer1_pin_port = portOutputRegister(digitalPinToPort(pin));
      _tunes_timer1_pin_mask = digitalPinToBitMask(pin);
      break;
    case 3: // 16 bit timer
      TCCR3A = 0;
      TCCR3B = 0;
      bitWrite(TCCR3B, WGM32, 1);
      bitWrite(TCCR3B, CS30, 1);
      _tunes_timer3_pin_port = portOutputRegister(digitalPinToPort(pin));
      _tunes_timer3_pin_mask = digitalPinToBitMask(pin);
      playNote(0, 60);  /* start and stop channel 0 (timer 3) on middle C so wait/delay works */
      stopNote(0);
      break;
  }
}

void ArduboyTunes::playNote(byte chan, byte note)
{
  byte timer_num;
  byte prescalar_bits;
  unsigned int frequency2; /* frequency times 2 */
  unsigned long ocr;

  // we can't plan on a channel that does not exist
  if (chan >= AVAILABLE_TIMERS)
    return;

  // we only have frequencies for a certain range of notes
  if ((note < LOWEST_NOTE) || (note > HIGHEST_NOTE))
    return;

  timer_num = pgm_read_byte(tune_pin_to_timer_PGM + chan);
  //if (note < 48)
  //  frequency2 = pgm_read_byte(_midi_byte_note_frequencies + note - LOWEST_NOTE);
  //else
    frequency2 = pgm_read_word(_midi_word_note_frequencies + note - FIRST_NOTE_IN_INT_ARRAY);

  //******  16-bit timer  *********
  // two choices for the 16 bit timers: ck/1 or ck/64
  ocr = F_CPU / frequency2 - 1;
  prescalar_bits = 0b001;
  if (ocr > 0xffff) {
    ocr = F_CPU / frequency2 / 64 - 1;
    prescalar_bits = 0b011;
  }
  // Set the OCR for the given timer, then turn on the interrupts
  switch (timer_num) {
    case 1:
      TCCR1B = (TCCR1B & 0b11111000) | prescalar_bits;
      OCR1A = ocr;
      bitWrite(TIMSK1, OCIE1A, 1);
      break;
    case 3:
      TCCR3B = (TCCR3B & 0b11111000) | prescalar_bits;
      OCR3A = ocr;
      bitWrite(TIMSK3, OCIE3A, 1);
      break;
  }
}

void ArduboyTunes::stopNote(byte chan)
{
  byte timer_num;
  timer_num = pgm_read_byte(tune_pin_to_timer_PGM + chan);
  switch (timer_num) {
    case 1:
      TIMSK1 &= ~(1 << OCIE1A);                 // disable the interrupt
      *_tunes_timer1_pin_port &= ~(_tunes_timer1_pin_mask);   // keep pin low after stop
      break;
    case 3:
      *_tunes_timer3_pin_port &= ~(_tunes_timer3_pin_mask);   // keep pin low after stop
      break;
  }
}

void ArduboyTunes::closeChannels(void)
{
  byte timer_num;
  for (uint8_t chan=0; chan < AVAILABLE_TIMERS; chan++) {
    timer_num = pgm_read_byte(tune_pin_to_timer_PGM + chan);
    switch (timer_num) {
      case 1:
        TIMSK1 &= ~(1 << OCIE1A);
        break;
      case 3:
        TIMSK3 &= ~(1 << OCIE3A);
        break;
    }
    digitalWrite(_tune_pins[chan], 0);
  }
}

// TIMER 1
ISR(TIMER1_COMPA_vect)
{
  *_tunes_timer1_pin_port ^= _tunes_timer1_pin_mask;  // toggle the pin
}

// TIMER 3
ISR(TIMER3_COMPA_vect)
{
}

