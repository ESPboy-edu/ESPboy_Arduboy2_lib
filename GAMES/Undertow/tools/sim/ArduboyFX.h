// PC stand-in for ArduboyFX: reads fxdata-data.bin and draws from it.
#pragma once
#include "Arduboy2.h"
#include <vector>
#define __uint24 uint32_t
extern Arduboy2Base ab;
constexpr uint8_t dbmNormal = 0, dbmMasked = 16;
struct FX {
  static std::vector<uint8_t>& data() { static std::vector<uint8_t> d; return d; }
  static void begin(uint16_t) {
    const char* p = getenv("FXDATA"); if (!p) p = "../../fxdata/fxdata-data.bin";
    FILE* f = fopen(p, "rb"); if (!f) { fprintf(stderr, "no FX data at %s\n", p); exit(2); }
    auto& d = data(); d.clear(); int c; while ((c = fgetc(f)) != EOF) d.push_back(c); fclose(f);
  }
  static void drawBitmap(int16_t x, int16_t y, uint32_t addr, uint8_t frame, uint8_t mode) {
    auto& d = data();
    int w = d.at(addr) << 8 | d.at(addr + 1), h = d.at(addr + 2) << 8 | d.at(addr + 3);
    int pages = (h + 7) / 8; bool masked = mode & dbmMasked;
    size_t p = addr + 4 + (size_t)frame * w * pages * (masked ? 2 : 1);
    for (int pg = 0; pg < pages; pg++)
      for (int i = 0; i < w; i++) {
        uint8_t b = d.at(p++), m = masked ? d.at(p++) : 0xFF;
        for (int j = 0; j < 8; j++) {
          int yy = pg * 8 + j; if (yy >= h) break;
          if (m & (1 << j)) ab.drawPixel(x + i, y + yy, (b >> j) & 1);
        }
      }
  }
  static uint8_t readIndexedUInt8(uint32_t a, uint8_t i) { return data().at(a + i); }
  static uint16_t readIndexedUInt16(uint32_t a, uint8_t i) { a += i * 2; return data().at(a) << 8 | data().at(a + 1); }
  static uint32_t readIndexedUInt24(uint32_t a, uint8_t i) { a += i * 3; return data().at(a) << 16 | data().at(a + 1) << 8 | data().at(a + 2); }
  static void readDataBytes(uint32_t a, uint8_t* buf, size_t n) { for (size_t k = 0; k < n; k++) buf[k] = data().at(a + k); }
  static void display(bool clear) { memcpy(ab.shown, ab.px, sizeof ab.px); if (clear) ab.clear(); }
  // the game's 4 KB save block of FX flash: erase sets 0xFF, page programs can only clear bits
  static std::vector<uint8_t>& save() { static std::vector<uint8_t> v(4096, 0xFF); return v; }
  static constexpr uint16_t programSavePage = 0;
  static uint32_t& at() { static uint32_t a; return a; }
  static uint8_t& cmd() { static uint8_t c; return c; }
  static void begin(uint16_t d, uint16_t) { begin(d); }
  static void eraseSaveBlock(uint16_t) { std::fill(save().begin(), save().end(), 0xFF); }
  static void waitWhileBusy() {}
  static void writeEnable() {}
  static void seekCommand(uint8_t c, uint32_t a) { cmd() = c; at() = a; }
  static uint8_t writeByte(uint8_t b) {                  // a page program wraps within its 256-byte page
    auto& v = save(); uint32_t a = at(); v.at(a) &= b; at() = (a & ~0xFFu) | ((a + 1) & 0xFF); return 0;
  }
  static void disable() {}
  static void seekSave(uint32_t a) { at() = a; }
  static uint8_t readPendingUInt8() { return save().at(at()++); }
  static void readEnd() {}
};
constexpr uint8_t SFC_WRITE = 0x02;
