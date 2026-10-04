#pragma once
#include <stddef.h>
#include <Arduino.h>
#include "font.h"               
#include "stage.h"              

#define WINDOW_MS 90            
#define NOTE_Q 32               
#define BEAT_Q 16
#define HOLD_UNIT_MS 20

#define ST_PENDING 0
#define ST_HIT 1
#define ST_MISSED 2
#define ST_HOLDING 3

#define FLAG_STAR 0x08
#define FLAG_BAR 0x10
#define FLAG_BEAT 0x20
#define FLAG_HOLD 0x40

struct Note { uint16_t t2; uint8_t lanes, hold, state, got; };
struct Beat { uint16_t t2; uint8_t bar; };
struct Reader { uint32_t addr; uint32_t ms; uint8_t done; };

struct __attribute__((packed)) fx_addr_t {
    uint8_t b[3];
    operator uint32_t() const { return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16); }
};

struct __attribute__((packed)) Song {
  fx_addr_t music, chart[LEVELS], text;
  uint32_t length;
  uint16_t notes[LEVELS];
  uint16_t id;                                  
  uint32_t perfect[LEVELS];                     
  uint16_t star_notes[LEVELS];                  
  fx_addr_t preview;                            
};

static_assert(offsetof(Song, preview) == 56 && sizeof(Song) <= SONG_ENTRY_BYTES, "the song table's entry");

static Song song;
static Reader music_rd, chart_rd;
static uint32_t music_loop_addr;                
static uint8_t music_loop;                      
static Note notes[NOTE_Q];
static uint8_t note_head, note_count;
static Beat beats[BEAT_Q];
static uint8_t beat_head, beat_count;
static Note pending;                            
static uint8_t pending_valid;
static uint32_t score;
static uint16_t streak, best_streak, hits, misses, total_notes;
static uint8_t autoplay, song_over;

#define STAR_FULL 1000
#define STAR_READY 500
#define STAR_DRAIN_MS 16                        
static uint16_t star_meter;                     
static uint16_t star_gain;                      
static uint8_t star_on;
static uint32_t star_ms;                        

#define FLAME_OUT 0xFF
static uint8_t flame_t[3];                      
static uint8_t flame_hold;                      
static uint8_t flame_held;                      

#define ROCK_START 50
#define ROCK_RED 33                             
static const uint8_t ROCK_MISS[LEVELS] = {1, 2, 3, 4}; // PROGMEM удален для безопасности
static uint8_t rock, rock_miss, song_failed;
static uint16_t beat_last;                      

static inline uint32_t now_ms() {
  noInterrupts();
  uint32_t t = song_ms;
  interrupts();
  return t;
}

static inline uint8_t multiplier() {
  return streak >= 30 ? 4 : streak >= 20 ? 3 : streak >= 10 ? 2 : 1;
}

static void music_fill() {
  uint8_t wraps = 0;
  while (!music_rd.done && music_count < MUSIC_RING) {
    uint8_t r[2];
    fx_read(music_rd.addr, r, 2);
    music_rd.addr += 2;
    music_rd.ms += r[0];
    if (r[1] == CMD_END) {
      if (music_loop && ++wraps < 2) {           
        music_rd.addr = music_loop_addr;
        continue;
      }
      music_rd.done = 1;
      break;
    }
    if (r[1] == CMD_NOP) continue;
    music_push((uint16_t)(music_rd.ms >> 1), r[1]);
  }
}

static void loop_start(uint32_t addr) {
  audio_reset();
  music_rd.addr = music_loop_addr = addr;
  music_rd.ms = 0;
  music_rd.done = 0;
  music_loop = 1;
  music_fill();
  song_running = 1;
}

static void riff_start() {
  loop_start(FX_TITLE_MUSIC);
}

static void chart_fill(uint32_t now) {
  while (!chart_rd.done) {
    if (pending_valid) {
      if (note_count >= NOTE_Q) return;
      notes[(note_head + note_count) & (NOTE_Q - 1)] = pending;
      note_count++;
      pending_valid = 0;
    }
    if (chart_rd.ms > now + LOOK_MS + 200) return;
    uint8_t r[3];
    fx_read(chart_rd.addr, r, 3);
    chart_rd.ms += r[0];
    if (r[1] == CMD_END) {
      chart_rd.done = 1;
      return;
    }
    chart_rd.addr += 2;
    if (r[1] == CMD_NOP) continue;
    uint8_t flags = r[1];
    if (flags & FLAG_HOLD) chart_rd.addr++;
    if ((flags & FLAG_BEAT) && beat_count < BEAT_Q) {
      Beat &b = beats[(beat_head + beat_count) & (BEAT_Q - 1)];
      b.t2 = chart_rd.ms >> 1;
      b.bar = (flags & FLAG_BAR) != 0;
      beat_count++;
    }
    if (flags & 7) {
      pending.t2 = chart_rd.ms >> 1;
      pending.lanes = flags & 0x0F;
      pending.hold = (flags & FLAG_HOLD) ? r[2] : 0;
      pending.state = ST_PENDING;
      pending.got = 0;
      pending_valid = 1;
    }
  }
}

static void play_start(uint8_t level) {
  audio_reset();
  music_loop = 0;
  music_rd.addr = song.music;
  music_rd.ms = 0;
  music_rd.done = 0;
  chart_rd.addr = song.chart[level];
  chart_rd.ms = 0;
  chart_rd.done = 0;
  note_head = note_count = beat_head = beat_count = 0;
  pending_valid = 0;
  score = 0;
  streak = best_streak = hits = misses = 0;
  total_notes = song.notes[level];
  song_over = 0;
  flame_t[0] = flame_t[1] = flame_t[2] = FLAME_OUT;
  flame_hold = flame_held = 0;
  star_meter = 0;
  star_on = 0;
  star_ms = 0;
  star_gain = song.star_notes[level] ? 1500 / song.star_notes[level] : 0;   
  rock = ROCK_START;
  rock_miss = ROCK_MISS[level]; // Без pgm_read
  song_failed = 0;
  beat_last = 0;
  band_reset();
  music_fill();
  chart_fill(0);
  song_running = 1;
}

static void star_start(uint32_t now) {
  star_on = 1;
  star_ms = now;
}

static void note_hit(Note &n) {
  n.state = n.hold ? ST_HOLDING : ST_HIT;
  hits++;
  streak++;
  if (streak > best_streak) best_streak = streak;
  uint8_t k = 0;
  for (uint8_t lane = 0; lane < 3; lane++) {
    uint8_t bit = 1 << lane;
    if (!(n.lanes & bit)) continue;
    k++;
    flame_t[lane] = 0;                             
    if (n.hold) {
      flame_hold |= bit;
      flame_held |= bit;
    } else {
      flame_hold &= ~bit;
    }
  }
  score += (uint16_t)50 * k * multiplier() * (star_on ? 2 : 1);
  if (n.lanes & FLAG_STAR) {
    star_meter += star_gain;
    if (star_meter > STAR_FULL) star_meter = STAR_FULL;
  }
  rock += star_on ? 2 : 1;
  if (rock > 100) rock = 100;
  band_strum = 5;
  noInterrupts();
  melody_muted = 0;
  voice_a(melody_wanted);
  interrupts();
}

static void note_miss(Note &n) {
  n.state = ST_MISSED;
  misses++;
  streak = 0;
  if (rock > rock_miss) rock -= rock_miss;
  else {
    rock = 0;
    song_failed = 1;
  }
  noInterrupts();
  melody_muted = 1;                                
  interrupts();
  noise_start(160);                                
}

static void judge_press(uint8_t lane, uint16_t t2) {
  uint8_t bit = 1 << lane;
  for (uint8_t i = 0; i < note_count; i++) {
    Note &n = notes[(note_head + i) & (NOTE_Q - 1)];
    if (n.state != ST_PENDING || !(n.lanes & bit)) continue;
    int16_t d = (int16_t)(t2 - n.t2) * 2;          
    if (d < -WINDOW_MS) return;                      
    if (d > WINDOW_MS) continue;                     
    n.got |= bit;
    if ((n.got & 7) == (n.lanes & 7)) note_hit(n);
    return;
  }
}

static void flames_update() {
  for (uint8_t lane = 0; lane < 3; lane++) {
    uint8_t t = flame_t[lane], bit = 1 << lane;
    if (t == FLAME_OUT) continue;
    t++;
    if (flame_hold & bit) {
      if (flame_held & bit) {
        if (t >= FLAME_LOOP_B) t = FLAME_LOOP_A;
      } else if (t >= FLAME_LOOP_A && t < FLAME_LOOP_B) {
        t = FLAME_LOOP_B;
      }
      if (t >= FLAME_HOLD_N) t = FLAME_OUT;
    } else if (t >= FLAME_HIT_N) {
      t = FLAME_OUT;
    }
    flame_t[lane] = t;
  }
  flame_held = 0;                                  
}

static void play_update() {
  uint32_t now = now_ms();
  uint16_t now2 = now >> 1;
  music_fill();
  chart_fill(now);
  flames_update();                                 
  title_frame += star_on ? 2 : 1;
  band_update();
  
  for (uint8_t i = 0; i < beat_count; i++) {
    Beat &b = beats[(beat_head + i) & (BEAT_Q - 1)];
    if ((int16_t)(now2 - b.t2) < 0) break;
    if (b.t2 != beat_last) {
      beat_last = b.t2;
      band_beat ^= 1;                              
      if (b.bar) band_bar();
    }
  }
  
  if (star_on) {
    uint16_t drain = (uint16_t)((now - star_ms) / STAR_DRAIN_MS);
    if (drain) {
      star_ms += (uint32_t)drain * STAR_DRAIN_MS;
      if (drain >= star_meter) {
        star_meter = 0;
        star_on = 0;
      } else {
        star_meter -= drain;
      }
    }
  } else if (star_meter >= STAR_READY && (autoplay || arduboy.justPressed(A_BUTTON))) {
    star_start(now);
  }

  // --- ЧИСТОЕ И НАДЕЖНОЕ ЧТЕНИЕ КНОПОК БЕЗ БУФЕРА ---
  if (!autoplay) {
      uint16_t t2 = (uint16_t)(now >> 1);
      if (arduboy.justPressed(lanes_flipped ? RIGHT_BUTTON : LEFT_BUTTON)) judge_press(0, t2);
      if (arduboy.justPressed(UP_BUTTON) || arduboy.justPressed(DOWN_BUTTON)) judge_press(1, t2);
      if (arduboy.justPressed(lanes_flipped ? LEFT_BUTTON : RIGHT_BUTTON)) judge_press(2, t2);
  }

  uint8_t held = 0;
  if (arduboy.pressed(lanes_flipped ? RIGHT_BUTTON : LEFT_BUTTON)) held |= 1;
  if (arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) held |= 2;
  if (arduboy.pressed(lanes_flipped ? LEFT_BUTTON : RIGHT_BUTTON)) held |= 4;

  for (uint8_t i = 0; i < note_count; i++) {
    Note &n = notes[(note_head + i) & (NOTE_Q - 1)];
    int16_t d = (int16_t)(now2 - n.t2) * 2;         
    if (n.state == ST_PENDING) {
      if (autoplay && d >= 0) {
        n.got = n.lanes & 7;
        note_hit(n);
      } else if (d > WINDOW_MS) {
        note_miss(n);
      }
    }
    if (n.state == ST_HOLDING) {
      if (d > (int16_t)n.hold * HOLD_UNIT_MS) {
        n.state = ST_HIT;
      } else if (autoplay || (held & n.lanes & 7) == (n.lanes & 7)) {
        score += multiplier() * (star_on ? 2 : 1);
        flame_held |= n.lanes & 7;                   
      } else {
        n.state = ST_HIT;                            
      }
    }
  }
  
  while (note_count) {
    Note &n = notes[note_head];
    int16_t d = (int16_t)(now2 - n.t2) * 2;
    if (n.state == ST_HOLDING || (n.state == ST_PENDING && d <= WINDOW_MS) ||
        (n.state == ST_MISSED && d <= -PERSP_MIN)) break;
    note_head = (note_head + 1) & (NOTE_Q - 1);
    note_count--;
  }
  while (beat_count) {
    if ((int16_t)(now2 - beats[beat_head].t2) * 2 <= -PERSP_MIN) break;
    beat_head = (beat_head + 1) & (BEAT_Q - 1);
    beat_count--;
  }
  if (now > song.length + 1500) song_over = 1;
}

static void persp(int16_t tau, uint8_t &y, uint8_t &s) {
  uint16_t i = (uint16_t)(tau - PERSP_MIN) / PERSP_STEP;
  y = pgm_read_byte(&PERSP_Y[i]);
  s = pgm_read_byte(&PERSP_S[i]);
}

static inline int16_t lane_x(uint8_t lane, uint8_t s) {
  return 64 + ((int16_t)s * NEAR_LANE * ((int8_t)lane - 1)) / 200;
}

static inline uint8_t edge_dx(uint8_t s) {
  return ((uint16_t)s * NEAR_EDGE) / 200;
}

static inline uint8_t divider_dx(uint8_t s) {
  return ((uint16_t)s * NEAR_LANE) / 400;
}

static void gem(int16_t x, uint8_t y, uint8_t s, bool hollow, bool star = false) {
  uint8_t k = GEM_SIZES - 1;
  for (uint8_t i = 0; i < GEM_SIZES - 1; i++) {
    if (s >= pgm_read_byte(&GEM_LIMIT[i])) {
      k = i;
      break;
    }
  }
  uint8_t h = pgm_read_byte(&GEM_H[k]);
  int16_t top = y - h / 2;
  const uint8_t *rows = &GEM_HW[k * GEM_ROWS];
  uint8_t wide = pgm_read_byte(&rows[h / 2]);
  rect_clear(x - wide - 1, top - 1, 2 * wide + 3, h + 2);
  for (uint8_t j = 0; j < h; j++) {
    uint8_t hw = pgm_read_byte(&rows[j]);
    arduboy.drawFastHLine(x - hw, top + j, 2 * hw + 1, WHITE);
  }
  if ((hollow || star) && h >= 3) {
    for (uint8_t j = 1; j < h - 1; j++) {
      uint8_t hw = pgm_read_byte(&rows[j]) - 1;
      arduboy.drawFastHLine(x - hw + 1, top + j, 2 * hw - 1, BLACK);
    }
    if (star && !hollow) arduboy.drawPixel(x, y, WHITE);
  }
}

static const uint8_t HEART[] PROGMEM = {7, 7, 0x0E, 0x1F, 0x3F, 0x7E, 0x3F, 0x1F, 0x0E};
#define ROCK_BAR_TOP 19
#define ROCK_BAR_H 38

static void rock_draw() {
  rect_clear(0, 9, 8, 48);
  Sprites::drawSelfMasked(0, 10, HEART, 0);
  arduboy.drawRect(0, ROCK_BAR_TOP, 5, ROCK_BAR_H, WHITE);
  uint8_t red = (uint8_t)((uint16_t)ROCK_RED * (ROCK_BAR_H - 2) / 100);
  for (uint8_t y = ROCK_BAR_TOP + ROCK_BAR_H - 2 - red; y < ROCK_BAR_TOP + ROCK_BAR_H - 1; y += 2) {
    arduboy.drawPixel(0, y, BLACK);
    arduboy.drawPixel(4, y, BLACK);
  }
  if (rock < ROCK_RED && (title_frame & 8)) return;
  uint8_t fill = (uint8_t)((uint16_t)rock * (ROCK_BAR_H - 2) / 100);
  if (fill) arduboy.fillRect(1, ROCK_BAR_TOP + ROCK_BAR_H - 1 - fill, 3, fill, WHITE);
}

#define prof_mark(i, t)

static void tail_draw(int16_t xa, uint8_t ya, int16_t xb, uint8_t yb) {
  int16_t den = ya - yb;
  for (uint8_t y = yb; y <= ya; y++) {
    int16_t x = xa;
    if (den) {
      int16_t q = 2 * (xb - xa) * (int16_t)(ya - y) + den, d = 2 * den;
      x += q >= 0 ? q / d : -((d - 1 - q) / d);   
    }
    uint8_t bit = 1 << (y & 7);
    uint8_t *p = arduboy.sBuffer + (uint16_t)(y >> 3) * 128;
    if (x >= 0 && x < 128) p[x] |= bit;
    if (x >= -1 && x < 127) p[x + 1] |= bit;
  }
}

static void play_draw() {
  uint32_t now = now_ms();
  
  scene_draw(star_on, star_on && (uint16_t)(now - ((uint32_t)beat_last << 1)) < 180, rock >= ROCK_START);
  
  uint8_t yn, sn, yf, sf, y, s;
  persp(PERSP_MIN, yn, sn);
  persp(LOOK_MS, yf, sf);
  highway_draw(yf, yn, edge_dx(sf), edge_dx(sn), divider_dx(sf), divider_dx(sn), star_on);
  
  for (uint8_t i = 0; i < beat_count; i++) {
    Beat &b = beats[(beat_head + i) & (BEAT_Q - 1)];
    int32_t tau = (int32_t)((uint32_t)b.t2 << 1) - (int32_t)now;
    if (tau < PERSP_MIN || tau > LOOK_MS) continue;
    persp((int16_t)tau, y, s);
    uint8_t e = edge_dx(s);
    row_dots(64 - e, 64 + e, y, b.bar ? 1 : 2);
  }
  
  uint8_t held = 0;
  if (arduboy.pressed(lanes_flipped ? RIGHT_BUTTON : LEFT_BUTTON)) held |= 1;
  if (arduboy.pressed(UP_BUTTON) || arduboy.pressed(DOWN_BUTTON)) held |= 2;
  if (arduboy.pressed(lanes_flipped ? LEFT_BUTTON : RIGHT_BUTTON)) held |= 4;

  persp(0, y, s);
  for (uint8_t lane = 0; lane < 3; lane++) {
    int16_t x = lane_x(lane, s);
    gem(x, y, s, !(held & (1 << lane)));
    uint8_t t = flame_t[lane];
    if (t == FLAME_OUT) continue;
    if (flame_hold & (1 << lane)) t += FLAME_HIT_N;           
    fx_sprite(FX_FLAMES + (uint32_t)(lane * (FLAME_HIT_N + FLAME_HOLD_N) + t) * FLAME_BYTES,
              x + FLAME_X0, y + FLAME_Y0, FLAME_W, FLAME_PAGES);
  }
  
  for (uint8_t i = note_count; i-- > 0;) {
    Note &n = notes[(note_head + i) & (NOTE_Q - 1)];
    if (n.state == ST_HIT) continue;
    int32_t tau32 = (int32_t)((uint32_t)n.t2 << 1) - (int32_t)now;
    if (tau32 > LOOK_MS) continue;
    if (tau32 < PERSP_MIN) {
      if (n.state != ST_HOLDING) continue;       
      tau32 = PERSP_MIN;                         
    }
    int16_t tau = (int16_t)tau32;
    persp(tau, y, s);
    for (uint8_t lane = 0; lane < 3; lane++) {
      if (!(n.lanes & (1 << lane))) continue;
      int16_t x = lane_x(lane, s);
      if (n.hold) {
        int16_t ta = n.state == ST_HOLDING && tau < 0 ? 0 : tau;
        int32_t tb32 = (int32_t)tau + (int32_t)n.hold * HOLD_UNIT_MS;
        int16_t tb = tb32 > LOOK_MS ? LOOK_MS : (int16_t)tb32;
        if (tb > ta) {
          uint8_t ya, sa, yb, sb;
          persp(ta, ya, sa);
          persp(tb, yb, sb);
          tail_draw(lane_x(lane, sa), ya, lane_x(lane, sb), yb);
        }
      }
      if (n.state != ST_HOLDING) gem(x, y, s, n.state == ST_MISSED, (n.lanes & FLAG_STAR) != 0);
    }
  }
  
  if (song.length) arduboy.drawFastHLine(0, 63, (uint8_t)(now * 128 / song.length), WHITE);
  
  char text[12];
  ultoa(score, text, 10);
  hud_text(0, text);
  text[0] = 'x';
  text[1] = '0' + multiplier() * (star_on ? 2 : 1);
  text[2] = ' ';
  utoa(streak, text + 3, 10);
  hud_text(128 - 6 * strlen(text), text);
  rock_draw();
  
  bool lit = star_on ? (uint8_t)(now >> 7) & 1 : star_meter >= STAR_READY;
  rect_clear(120, 9, 8, 48);
  Sprites::drawSelfMasked(121, 10, lit ? STAR : STAR_HOLLOW, 0);
  arduboy.drawRect(123, 19, 5, 38, WHITE);
  uint8_t fill = (uint8_t)((uint32_t)star_meter * 36 / STAR_FULL);
  if (fill) arduboy.fillRect(124, 56 - fill, 3, fill, WHITE);
}
