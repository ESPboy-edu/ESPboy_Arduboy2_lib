// PC stand-in for ArduboyTones: records which sequences were started.
#pragma once
#include <cstdint>
#include <set>
#define TONES_END 0x8000
#define TONES_REPEAT 0x8001
struct ArduboyTones {
  static std::set<const uint16_t*>& heard() { static std::set<const uint16_t*> h; return h; }
  static const uint16_t*& cur() { static const uint16_t* c = nullptr; return c; }
  static int& left() { static int l = 0; return l; }
  ArduboyTones(bool (*)()) {}
  static void tones(const uint16_t* s) {
    heard().insert(s); cur() = s;
    int ms = 0, i = 0; while (s[i] != TONES_END && s[i] != TONES_REPEAT) { ms += s[i + 1]; i += 2; }
    left() = s[i] == TONES_REPEAT ? 1 << 30 : ms / 33 + 1;     // frames at 30 fps
  }
  static void noTone() { left() = 0; }
  static bool playing() { return left() > 0; }
  static void tick() { if (left() > 0 && left() < (1 << 30)) left()--; }
};
