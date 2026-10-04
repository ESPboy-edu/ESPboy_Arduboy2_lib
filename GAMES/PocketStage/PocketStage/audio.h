#pragma once
#include <Arduino.h>
#include <Arduboy2.h>
#include <sigma_delta.h>

extern Arduboy2 arduboy;

#define CMD_NOP 0xFF
#define CMD_END 0xFE
#define MUSIC_RING 32

struct MusicEv { uint16_t t2; uint8_t cmd; };

static volatile uint32_t song_ms;
static volatile uint8_t song_running;
static MusicEv music_ring[MUSIC_RING];
static volatile uint8_t music_head, music_count;

static volatile uint8_t melody_muted;
static volatile uint8_t melody_wanted;
static uint8_t lanes_flipped = 0;
static uint8_t sound_on;

static inline uint16_t note_period(uint8_t midi) {
  if (midi < NOTE_LO) midi = NOTE_LO;
  if (midi > NOTE_HI) midi = NOTE_HI;
  return pgm_read_word(&NOTE_PERIOD[midi - NOTE_LO]);
}

// --- Программный эмулятор аппаратных таймеров AVR ---
#define SAMPLE_RATE 22050
#define SOUND_PIN 0 // Пин D3 на Wemos / ESPboy

static volatile uint16_t ocr3a = 0;
static uint16_t tcnt3 = 0;
static uint8_t pin3 = 0;
static volatile bool en3 = false;
static volatile uint16_t noise_left = 0;
static uint16_t noise_dice = 0xACE1;

static volatile uint16_t ocr1a = 0;
static uint16_t tcnt1 = 0;
static uint8_t pin1 = 0;
static volatile bool en1 = false;
static volatile uint8_t drum_left = 0;
static volatile bool drum_kick = false;
static volatile uint8_t bass_note = 0;
static uint16_t drum_dice = 0xBEEF;

static inline void voice_a(uint8_t midi) {
  noInterrupts();
  if (!midi) {
    en3 = false;
    pin3 = 0;
  } else {
    ocr3a = note_period(midi);
    tcnt3 = 0;
    en3 = true;
  }
  interrupts();
}

static void noise_start(uint16_t toggles) {
  noInterrupts();
  ocr3a = 300;
  tcnt3 = 0;
  en3 = true;
  noise_left = toggles;
  interrupts();
}

#define DRUM_KICK 0x7C
#define DRUM_SNARE 0x7D

static inline void bass_pin(uint8_t midi) {
  if (!midi) {
    en1 = false;
    pin1 = 0;
    return;
  }
  ocr1a = note_period(midi);
  tcnt1 = 0;
  en1 = true;
}

static inline void voice_b(uint8_t midi) {
  noInterrupts();
  if (midi >= DRUM_KICK) {
    drum_kick = (midi == DRUM_KICK);
    ocr1a = drum_kick ? 300 : 150;
    tcnt1 = 0;
    drum_left = drum_kick ? 44 : 220;
    en1 = true;
  } else {
    bass_note = midi;
    if (!drum_left) bass_pin(midi);
  }
  interrupts();
}

// Прерывание звука (отрабатывает 22050 раз в секунду)
void ICACHE_RAM_ATTR audio_isr() {
  for (uint8_t i = 0; i < 91; i++) {
    if (en3) {
      if (++tcnt3 >= ocr3a) {
        tcnt3 = 0;
        pin3 ^= 1;
        if (noise_left) {
          noise_dice = (noise_dice >> 1) ^ (-(noise_dice & 1) & 0xB400);
          ocr3a = 200 + (noise_dice & 0x3FF);
          if (--noise_left == 0) en3 = false;
        }
      }
    }
    if (en1) {
      if (++tcnt1 >= ocr1a) {
        tcnt1 = 0;
        pin1 ^= 1;
        if (drum_left) {
          if (drum_kick) ocr1a += (ocr1a >> 4);
          else {
            drum_dice = (drum_dice >> 1) ^ (-(drum_dice & 1) & 0xB400);
            ocr1a = 100 + (drum_dice & 0x1FF);
          }
          if (--drum_left == 0) bass_pin(bass_note);
        }
      }
    }
  }

  // Микширование в Sigma-Delta
  if (sound_on) {
    uint8_t out = 0;
    if (en3 && pin3) out += 64;
    if (en1 && pin1) out += 64;
    sigmaDeltaWrite(0, out);
  } else {
    sigmaDeltaWrite(0, 0);
  }

  // Эмуляция Таймера 4 (Игровой таймер 1000 Гц)
  static uint8_t ms_divider = 0;
  if (++ms_divider >= 22) {
    ms_divider = 0;
    if (song_running) {
      song_ms++;
      uint16_t t2 = (uint16_t)(song_ms >> 1);
      while (music_count && (int16_t)(music_ring[music_head].t2 - t2) <= 0) {
        uint8_t cmd = music_ring[music_head].cmd;
        music_head = (music_head + 1) & (MUSIC_RING - 1);
        music_count--;
        if (cmd & 0x80) voice_b(cmd & 0x7F);
        else {
          melody_wanted = cmd;
          if (!melody_muted) voice_a(cmd);
        }
      }
    }
  }
}

static void audio_init() {
  sound_on = Arduboy2Audio::enabled();
  noInterrupts();
  sigmaDeltaSetup(0, SAMPLE_RATE);
  sigmaDeltaAttachPin(SOUND_PIN);
  sigmaDeltaEnable();
  timer1_attachInterrupt(audio_isr);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
  timer1_write(80 * 1000000 / SAMPLE_RATE);
  interrupts();
}

static void audio_set_sound(uint8_t on) {
  sound_on = on;
  if (on) Arduboy2Audio::on(); 
  else Arduboy2Audio::off();
  Arduboy2Audio::saveOnOff();
}

static void audio_reset() {
  noInterrupts();
  song_running = 0;
  song_ms = 0;
  music_head = music_count = 0;
  melody_muted = 0;
  melody_wanted = 0;
  drum_left = 0;
  en1 = false;
  en3 = false;
  pin1 = 0;
  pin3 = 0;
  interrupts();
}

static void audio_stop() {
  noInterrupts();
  song_running = 0;
  drum_left = 0;
  en1 = false;
  en3 = false;
  pin1 = 0;
  pin3 = 0;
  interrupts();
}

static void audio_pause() {
  noInterrupts();
  song_running = 0;
  drum_left = 0;
  en1 = false;
  en3 = false;
  pin1 = 0;
  pin3 = 0;
  interrupts();
}

static void audio_resume() {
  noInterrupts();
  if (!melody_muted) voice_a(melody_wanted);
  bass_pin(bass_note);
  song_running = 1;
  interrupts();
}

static void music_push(uint16_t t2, uint8_t cmd) {
  noInterrupts();
  MusicEv &e = music_ring[(music_head + music_count) & (MUSIC_RING - 1)];
  e.t2 = t2;
  e.cmd = cmd;
  music_count++;
  interrupts();
}
