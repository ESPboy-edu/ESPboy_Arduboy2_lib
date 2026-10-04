#!/usr/bin/env python3
"""
Pocket Stage - hear a song without the console.

  python3 tools/preview_wav.py [build] [song number or "title" ...]

Reads build/<build>/fxdata.bin (default: game) the way the console does and
writes build/<build>/<song>.wav for every song asked for (default: all, and
the title riff twice round). It does what the timers do: two pins toggling
at the notes' periods, the drums taking the bass's pin over for a moment,
and the speaker hearing the difference of the two. A real piezo is thinner
and louder up high; this is for checking the notes.
"""
import os
import struct
import sys
import wave

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
RATE = 22050
LEVEL = 9000                 # of 32767 per pin
COUNTS_PER_SAMPLE = 2e6 / RATE
DRUM_KICK, DRUM_SNARE = 0x7C, 0x7D


def period(midi):
    return round(1e6 / (440.0 * 2 ** ((midi - 69) / 12.0)))


def songs_in(blob):
    count = blob[3]
    out = []
    for i in range(count):
        e = blob[0x10 + 64 * i:0x10 + 64 * (i + 1)]
        music = int.from_bytes(e[0:3], "little")
        text = int.from_bytes(e[15:18], "little")
        title = blob[text:blob.index(b"\0", text)].decode("ascii")
        out.append((title, music))
    return out


class Pin:
    """a pin a timer toggles: a period in counts of 2 MHz, or 0 for quiet"""

    def __init__(self):
        self.period = 0
        self.level = 0
        self.acc = 0.0
        self.drum_left = 0
        self.drum_kick = False
        self.dice = 0xBEEF
        self.note = 0            # the bass note to come back to after a drum

    def play(self, midi):
        self.period = period(midi) if midi else 0
        self.level = self.level if midi else 0
        self.acc = 0.0

    def drum(self, kind):
        self.drum_kick = kind == DRUM_KICK
        self.period = 300 if self.drum_kick else 150
        self.drum_left = 44 if self.drum_kick else 220
        self.acc = 0.0

    def sample(self):
        if not self.period:
            return 0
        self.acc += COUNTS_PER_SAMPLE
        while self.acc >= self.period:
            self.acc -= self.period
            self.level ^= 1
            if self.drum_left:
                self.drum_left -= 1
                if self.drum_left:
                    if self.drum_kick:
                        self.period += self.period >> 4
                    else:
                        self.dice = (self.dice >> 1) ^ (-(self.dice & 1) & 0xB400)
                        self.period = 100 + (self.dice & 0x1FF)
                else:
                    self.play(self.note)
                    if not self.period:
                        break
        return self.level


def render(blob, music, loops=1):
    """the music stream -> 16-bit samples"""
    pins = [Pin(), Pin()]
    out = bytearray()

    def run(ms):
        for _ in range(ms * RATE // 1000):
            s = LEVEL * (pins[0].sample() - pins[1].sample())
            out.extend(struct.pack("<h", s))

    for _ in range(loops):
        pos = music
        while True:
            dt, cmd = blob[pos], blob[pos + 1]
            pos += 2
            run(dt)
            if cmd == 0xFE:
                break
            if cmd == 0xFF:
                continue
            v, midi = cmd >> 7, cmd & 0x7F
            if v == 1 and midi >= DRUM_KICK:
                pins[1].drum(midi)
            else:
                pins[v].note = midi
                if not pins[v].drum_left:
                    pins[v].play(midi)
    run(500)
    return out


def main():
    args = sys.argv[1:]
    build = args.pop(0) if args and not (args[0].isdigit() or args[0] == "title") else "game"
    folder = os.path.join(ROOT, "build", build)
    with open(os.path.join(folder, "fxdata.bin"), "rb") as f:
        blob = f.read()
    songs = songs_in(blob)
    riff = int.from_bytes(blob[4:7], "little")
    wanted = args or [str(i) for i in range(len(songs))] + ["title"]
    for a in wanted:
        if a == "title":
            title, music, loops = "TITLE RIFF", riff, 2
        else:
            title, music = songs[int(a)]
            loops = 1
        name = title.lower().replace(" ", "_").replace("-", "_") + ".wav"
        path = os.path.join(folder, name)
        samples = render(blob, music, loops)
        with wave.open(path, "wb") as w:
            w.setnchannels(1)
            w.setsampwidth(2)
            w.setframerate(RATE)
            w.writeframes(bytes(samples))
        print("%s: %.1f s -> %s" % (title, len(samples) / 2 / RATE, os.path.relpath(path, ROOT)))


if __name__ == "__main__":
    main()
