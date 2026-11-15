#!/usr/bin/env python3
"""Sound effects + music snippets for Undertow.

One source of truth -> two outputs:
  sfx.h                PROGMEM sequences for the sketch's tone player: (pitch, length) byte pairs
  tools/audio/preview/  .wav renders (square wave, like the piezo) to audition anywhere

Byte format (half the size of ArduboyTones' freq/duration words):
  pitch   0 = rest, else 27.5 Hz * 2^(pitch/24) -- quarter-tone steps, so every equal-tempered
          note is exact (A4 = 96); sweeps land on the nearest quarter tone
  length  in 1/256 s (a note longer than 255 is split; a repeated pitch plays on unbroken)
  (0, 0) ends a sequence; (1, 0) loops it from the start

The Arduboy speaker plays ONE square-wave voice. So: short, punchy effects;
music only as brief motifs (chapter card, boss, victory, defeat, the demon reveal).
Usage:  python3 tools/audio/make_sfx.py      (from the sketch folder)
"""
import math, os, struct, wave

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
PREV = os.path.join(HERE, 'preview')
os.makedirs(PREV, exist_ok=True)

NAMES = {'C': 0, 'C#': 1, 'DB': 1, 'D': 2, 'D#': 3, 'EB': 3, 'E': 4, 'F': 5, 'F#': 6, 'GB': 6,
         'G': 7, 'G#': 8, 'AB': 8, 'A': 9, 'A#': 10, 'BB': 10, 'B': 11}


def hz(note):
    """'A4' -> 440, 'R' -> 0 (rest), ints pass through."""
    if isinstance(note, int): return note
    if note.upper() == 'R': return 0
    n, octv = note[:-1].upper(), int(note[-1])
    semis = NAMES[n] + (octv + 1) * 12
    return round(440 * 2 ** ((semis - 69) / 12))


def tune(bpm, notes, legato=0.85):
    """notes: 'D4:q A4:e ...' -> [(hz, ms)], with a short gap between notes so they articulate."""
    beat = 60000 / bpm
    L = {'w': 4, 'h': 2, 'q': 1, 'e': .5, 's': .25, 'q.': 1.5, 'h.': 3, 'e.': .75, 't': 1 / 3}
    seq_ = []
    for tok in notes.split():
        n, d = tok.split(':')
        seq_.append((hz(n), beat * L[d]))
    out = []
    for i, (f, ms) in enumerate(seq_):
        nxt = seq_[i + 1][0] if i + 1 < len(seq_) else 0
        if f == 0 or f != nxt: out.append((f, round(ms))); continue   # slurred into a new pitch: no gap (half the bytes)
        on = round(ms * legato)                                          # a repeated pitch needs its gap to sound twice
        out.append((f, on)); out.append((0, round(ms) - on))
    return out


def sweep(f0, f1, ms, steps=None):
    steps = steps or max(3, ms // 10)
    return [(round(f0 * (f1 / f0) ** (k / (steps - 1))), max(1, ms // steps)) for k in range(steps)]


def seq(*parts):
    out = []
    for p in parts: out += p
    return out


# ------------------------------------------------------------------ effects
SFX = {
    'cursor':   [(1760, 6)],
    'menu':     [(1175, 10)],
    'confirm':  [(880, 28), (1319, 40)],
    'cancel':   [(659, 28), (440, 40)],
    'march':    [(147, 22), (0, 40), (165, 22)],
    'step':     [(131, 9)],                                             # one footfall per tile walked                       # a unit takes its new square
    'phase_p':  tune(180, 'D4:e A4:e D5:q'),                            # player phase: open fifth fanfare
    'phase_e':  tune(120, 'A3:e F3:e E3:q'),                            # enemy phase: falling, minor
    'swing':    sweep(1400, 300, 80),
    'arrow':    sweep(2600, 1500, 110),
    'spell':    [(f, 14) for f in (988, 1480, 1175, 1760, 1397, 2093, 1661, 2489)],   # a spell: a rising shimmer
    'hit':      [(2200, 6), (110, 32), (82, 48)],                       # click of contact, then the thud
    'crit':     seq(tune(400, 'C5:s G5:s C6:s'), [(2200, 6), (98, 40), (73, 70)]),
    'miss':     [(3136, 12), (0, 12), (2637, 30)],                      # glancing clink
    'fall':     seq([(2200, 6), (110, 32)], [(f, 30) for f in (220, 196, 165, 147, 123, 110, 98, 82)]),
    'blip_lo':  [(196, 9)],                                             # dialogue voice: Brannock
    'blip_mid': [(330, 8)],                                             # Idra, Hale, Wren
    'blip_osk': [(110, 11)],                                            # Osk
    'blip_dmn': [(70, 7), (74, 7)],                                     # demons: a grinding buzz
    'exp':      [(1568, 5)],                                            # EXP bar tick
    'lvlup':    tune(260, 'D5:s F#5:s A5:s D6:e'),                      # level up!
    'ding':     [(2093, 18), (2637, 34)],                               # a stat went up
    'seal':     seq(sweep(523, 2093, 220, 12), [(0, 30)], [(1568, 40), (2093, 120)]),   # a demon is sealed
    'summon':   seq([(f, 18) for f in (62, 66) * 8], sweep(80, 40, 200)),            # a demon arrives
    'alarm':    seq(*[[(1397, 14), (1175, 30), (0, 46)] for _ in range(5)], [(1397, 14), (1175, 120)]),   # the gate bell: the garrison wakes
}

# ------------------------------------------------------------------ music
# D dorian throughout: modal, a little old, not the usual heroic major.
MUSIC = {
    # chapter card: "The Drowned Ford"
    'm_chapter': tune(112, 'D4:q A4:e A4:e G4:q A4:q  C5:q B4:e A4:e G4:h  F4:q G4:e A4:e D5:q C5:q  A4:q. G4:e A4:h '
                           'D5:q C5:e A4:e G4:q F4:q  E4:q F4:e G4:e A4:h  G4:q F4:e E4:e D4:q C4:q  D4:w'),
    # Osk: a heavy, falling motif with a tritone; loops while he speaks
    'm_boss':    tune(84, 'A3:q. A3:e C4:q B3:q  A3:q D#4:q E4:h  F4:q E4:e D#4:e E4:q A3:q  R:h'),
    # victory: mixolydian lift, bright and short
    'm_win':     tune(150, 'D4:e F#4:e A4:e D5:q C5:e D5:e E5:e F#5:h.'),
    # defeat: slow dorian lament
    'm_lose':    tune(60, 'A4:q G4:q F4:q E4:q D4:h. R:q'),
    # the reveal: chromatic crawl + tritone, for when the demons show themselves
    'm_demon':   tune(70, 'C4:e C#4:e C4:e F#4:q. R:e  B3:e C4:e B3:e F3:q. R:e  C4:t C#4:t D4:t D#4:t E4:t F4:t F#4:h R:q'),
}

LOOPED = {'m_boss'}


def pitch(f):
    return 0 if f == 0 else max(1, min(255, round(24 * math.log2(f / 27.5))))


def pitch_hz(p):
    return 0 if p == 0 else 27.5 * 2 ** (p / 24)


def encode(s):
    """[(hz, ms/1024)] -> [(pitch, 1/256 s)]: what the device actually plays"""
    out = []
    for f, d in s:
        n = max(1, round(d / 4))
        while n:
            k = min(n, 255); out.append((pitch(f), k)); n -= k
    return out


def write_header():
    L = ["// Generated by tools/audio/make_sfx.py -- edit the generator, not this file.",
         "#pragma once", "#include <avr/pgmspace.h>", ""]
    total = 0
    for name, s in list(SFX.items()) + list(MUSIC.items()):
        vals = []
        for p, d in encode(s):
            vals += [str(p), str(d)]
        vals += ['1', '0'] if name in LOOPED else ['0', '0']
        total += len(vals)
        L.append(f"const uint8_t S_{name.upper()}[] PROGMEM = {{ {', '.join(vals)} }};")
    with open(os.path.join(ROOT, 'sfx.h'), 'w') as f:
        f.write("\n".join(L) + "\n")
    return total


def render(name, s, rate=22050, loops=1):
    """Square wave at 50% duty -- close to what the piezo does."""
    frames = bytearray()
    phase = 0.0
    for _ in range(loops):
        for p, d in encode(s):
            f = pitch_hz(p)
            n = int(rate * d / 256)
            for _ in range(n):
                if f:
                    phase = (phase + f / rate) % 1.0
                    v = 0.28 if phase < 0.5 else -0.28
                else:
                    v = 0.0
                frames += struct.pack('<h', int(v * 32767))
    frames += bytes(int(rate * 0.12) * 2)
    with wave.open(os.path.join(PREV, name + '.wav'), 'wb') as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(rate)
        w.writeframes(bytes(frames))


def main():
    total = write_header()
    for name, s in SFX.items(): render(name, s)
    for name, s in MUSIC.items(): render(name, s, loops=2 if name in LOOPED else 1)
    secs = {k: round(sum(d for _, d in v) / 1024, 1) for k, v in MUSIC.items()}
    print(f"sfx.h: {total} bytes of sequences;  music lengths (s): {secs}")


if __name__ == '__main__':
    main()
