# Zero Day: our own, a technical thrash track written for the game. E
# phrygian, 184 to the beat. The hook is the riff: the low E with a note
# climbing over it in half steps, F, G, A-flat, grouped three, three and two
# instead of in fours, then the same grouping twice as fast, A, B-flat, B,
# up to a C and a dead stop; the second time it falls back to the F. A
# verse that twists the riff (falling pairs, trills, stabs), a sinister slow
# part over E and F, a fast solo in the Spanish-sounding scale (E, F,
# G-sharp...), and the riff moved up a fourth, then a fifth, to close on
# stabs and a long E. The bass doubles the riff. About fifty-two seconds,
# and fast.
#
# The tune never goes below E5 and the bass never above C4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "ZERO DAY"
BY = "MEGABYTE"
STYLE = "Technical thrash"
KEY = "E phrygian"
BEATS = 4

TEMPO = [(0, 184)]

# the riff: three, three and two, in eighths and then in sixteenths
Z1 = "f5:6 e5 e5 g5 e5 e5 ab5 e5 | "
Z2 = "a5:3 e5 e5 bb5 e5 e5 b5 e5 c6:6 r:18 | "       # climbs, and stops
Z3 = "c6:3 e5 e5 bb5 e5 e5 g5 e5 f5:6 r:18 | "       # falls to the F, and stops
RIFF = Z1 + Z2 + Z1 + Z3

# the verse twists it: falling pairs, trills, stabs
Y1 = "g5:6 f5 e5 bb5 a5 e5 c6 b5 | "
Y2 = "f5:3 e5 f5 e5 g5:6 r ab5:3 g5 ab5 g5 a5:6 r | "
Y3 = "e5:6 r e5 r e5 r f5:12 | "
VERSE = Z1 + Y1 + Z1 + Y2 + Z1 + Z2 + Z1 + Y3

# the slow part: E, F, E, F, G, F, E, E
MIDDLE = ("b5:12 c6:6 b5 e6:24 | "
          "f6:12 e6:6 c6 a5:24 | "
          "g5:12 a5:6 b5 e6:12 g6 | "
          "f6:36 e6:6 c6 | "
          "d6:12 b5:6 g5 d6:12 f6 | "
          "e6:12 c6:6 a5 c6:12 f6 | "
          "e6:24 f6:12 e6 | "
          "b5:12 r e5:6 r:18 | ")
BASS_MIDDLE = ("e3:48 | f3:48 | e3:48 | f3:48 | g3:48 | f3:48 | e3:48 | "
            "e3:12 r e3:6 r:18 | ")

# the solo, in E with F and G-sharp: E, F, E, F, G, F, E, E
SOLO = ("e5:3 f5 g#5 a5 b5:6 c6 b5:3 a5 g#5:6 e6 b5 | "
        "c6:3 b5 a5 g#5 a5:6 c6 f6:3 e6 c6 a5 f5:12 | "
        "e6:6 b5 g#5 e5 g#5 b5 e6 f6 | "
        "c7:6 a6 f6 c6 a5 f5 a5 c6 | "
        "b6:3 a6 g6 f6 d6:6 b5 d6:3 f6 g6 b6 d6:12 | "
        "a6:3 g#6 f6 e6 c6:6 a5 f5:12 a5:6 c6 | "
        "e5:6 e6 f5 f6 g#5 g#6 a5 a6 | "
        "b6:3 a6 g#6 f6 e6:6 r:30 | ")
BASS_SOLO = line(eighths, "e3 f3 e3 f3 g3 f3 e3") + "e3:6 e3 e3 r:30 | "

# the riff a fourth up, then a fifth up, and stabs on E
OUTRO = (shift(RIFF, 5)
         + shift(RIFF, 7)
         + "e6:6 r:18 f6:6 r:18 | e6:48 | ")
BASS_OUTRO = (shift(RIFF, 5 - 36)
           + shift(RIFF, 7 - 36)
           + "e3:6 r:18 f3:6 r:18 | e3:48 | ")

D_332 = "k:6 r s k r s k s | "               # with the riff: three, three, two
D_STOP = "k:9 k k:6 s:6 r:18 | "
D_TRILL = "k:12 s:6 r k:12 s:6 r | "
D_STABS = "k:6 r k r k r s:12 | "

# the riff alone for two bars, then the band comes in
MELODY = (Z1 + Z2
          + RIFF
          + VERSE
          + MIDDLE
          + SOLO
          + OUTRO)

BASS = ("r:48 | r:48 | "
        + shift(RIFF, -24)
        + shift(VERSE, -24)
        + BASS_MIDDLE
        + BASS_SOLO
        + BASS_OUTRO)

DRUMS = ("r:48 | r:48 | "
         + (D_332 + D_STOP) * 2
         + D_332 + D_332 + D_332 + D_TRILL + D_332 + D_STOP + D_332 + D_STABS
         + HALF * 7 + "k:12 r k:6 r:18 | "
         + PUNK * 2 + DOUBLE * 2 + PUNK * 3 + "k:6 k k r:30 | "
         + (D_332 + D_STOP) * 4
         + STABS + LAST)
