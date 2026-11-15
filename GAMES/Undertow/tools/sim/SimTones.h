// PC stand-in for the sketch's tone player: records which sequences were started and
// how many frames each would sound for (byte pairs: pitch, length in 1/256 s; (1,0) loops).
#pragma once
#include <cstdint>
#include <set>
inline std::set<const uint8_t*>& sndHeard() { static std::set<const uint8_t*> h; return h; }
inline int& sndFrames() { static int l = 0; return l; }
struct SndOn { operator bool() const { return sndFrames() > 0; } };
static SndOn sndOn;
inline void sndPlay(const uint8_t* s) {
  sndHeard().insert(s);
  int t = 0, i = 0; while (s[i + 1]) { t += s[i + 1]; i += 2; }
  sndFrames() = s[i] ? 1 << 30 : t * 30 / 256 + 1;               // frames at 30 fps
}
inline void sndStop() { sndFrames() = 0; }
inline void sndTick() { if (sndFrames() > 0 && sndFrames() < (1 << 30)) sndFrames()--; }
