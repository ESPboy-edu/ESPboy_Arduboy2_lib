#include "CustomArduboy.h"
#include "Arduboy_audio.h"

const byte PROGMEM tune_pin_to_timer_PGM[] = { 3, 1 };
volatile byte *_tunes_timer1_pin_port;
volatile byte _tunes_timer1_pin_mask;
volatile byte *_tunes_timer3_pin_port;
volatile byte _tunes_timer3_pin_mask;
byte _tune_pins[AVAILABLE_TIMERS];
volatile boolean tune_playing; // is the score still playing?
volatile unsigned wait_timer_frequency2;       /* its current frequency */
volatile boolean wait_timer_playing = false;   /* is it currently playing a note? */
volatile unsigned long wait_toggle_count;      /* countdown score waits */

// pointers to your musical score and your position in said score
volatile const byte *score_start = 0;
volatile const byte *score_cursor = 0;

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
  tune_playing = false;
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
      wait_timer_frequency2 = frequency2;  // for "tune_delay" function
      wait_timer_playing = true;
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
      wait_timer_playing = false;
      *_tunes_timer3_pin_port &= ~(_tunes_timer3_pin_mask);   // keep pin low after stop
      break;
  }
}

void ArduboyTunes::playScore(const byte *score)
{
  score_start = score;
  score_cursor = score_start;
  step();  /* execute initial commands */
  tune_playing = true;  /* release the interrupt routine */
}

void ArduboyTunes::stopScore (void)
{
  for (uint8_t i = 0; i < AVAILABLE_TIMERS; i++)
    stopNote(i);
  tune_playing = false;
}

bool ArduboyTunes::playing()
{
  return tune_playing;
}

/* Do score commands until a "wait" is found, or the score is stopped.
This is called initially from tune_playcore, but then is called
from the interrupt routine when waits expire.
*/
/* if CMD < 0x80, then the other 7 bits and the next byte are a 15-bit big-endian number of msec to wait */
void ArduboyTunes::step()
{
  byte command, opcode, chan;
  unsigned duration;

  while (1) {
    command = pgm_read_byte(score_cursor++);
    opcode = command & 0xf0;
    chan = command & 0x0f;
    if (opcode == TUNE_OP_STOPNOTE) { /* stop note */
      stopNote(chan);
    }
    else if (opcode == TUNE_OP_PLAYNOTE) { /* play note */
      playNote(chan, pgm_read_byte(score_cursor++));
    }
    else if (opcode == TUNE_OP_RESTART) { /* restart score */
      score_cursor = score_start;
    }
    else if (opcode == TUNE_OP_STOP) { /* stop score */
      tune_playing = false;
      break;
    }
    else if (opcode < 0x80) { /* wait count in msec. */
      duration = ((unsigned)command << 8) | (pgm_read_byte(score_cursor++));
      wait_toggle_count = ((unsigned long) wait_timer_frequency2 * duration + 500) / 1000;
      if (wait_toggle_count == 0) wait_toggle_count = 1;
      break;
    }
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
  tune_playing = false;
}

void ArduboyTunes::soundOutput()
{
  if (wait_timer_playing) { // toggle the pin if we're sounding a note
    *_tunes_timer3_pin_port ^= _tunes_timer3_pin_mask;
  }
  if (tune_playing && wait_toggle_count && --wait_toggle_count == 0) {
    // end of a score wait, so execute more score commands
    ArduboyTunes::step();  // execute commands
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
  // Timer 3 is the one assigned first, so we keep it running always
  // and use it to time score waits, whether or not it is playing a note.
  ArduboyTunes::soundOutput();
}

