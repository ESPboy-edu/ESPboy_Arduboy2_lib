# Hard Reset: our own, a groove metal track written for the game. E, 116 to
# the beat, heavy and off the beat. The hook is the riff: the low E chugged
# in sixteenths with gaps in it (the gaps are the groove), stabs on B and D
# between, and a squeal (a pinch harmonic: a sudden high B) on the fourth
# beat; the second bar turns down from D through C, B and B-flat to G. The
# bass chugs along with it and the kick drum locks to the chugs. A verse
# where a shouted line answers the riff, a chorus of long notes cut by the
# chug, a crushing part at half time with squeals, a wild solo of octave
# jumps and dives, the riff again and a long squeal to end. About
# fifty-six seconds.
#
# The tune never goes below E5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "HARD RESET"
BY = "DIMEBYTE"
STYLE = "Groove metal"
KEY = "E"
BEATS = 4

TEMPO = [(0, 116)]

# the riff: the chug with its gaps, stabs on B and D between, the squeal on
# 4; then the turn down from D
H1 = "e5:3 e5 r b5 r e5 e5 r d6 r e5 e5 b6:6 r | "
H2 = "e5:3 e5 r b5 r e5 e5 r d6:6 c6:3 b5 bb5:6 g5 | "
H3 = "e5:3 e5 r b5 r e5 e5 r d6 r e5 e5 c7:6 b6 | "          # two squeals
BASS_H1 = "e3:3 e3 r b3 r e3 e3 r d4 r e3 e3 r:12 | "
BASS_H2 = "e3:3 e3 r b3 r e3 e3 r d4:6 c4:3 b3 bb3:6 g3 | "
D_H1 = "k:3 k r r s k k r r r k k s:12 | "    # kicks on the chugs
D_H2 = "k:3 k r r s k k r k:6 k:3 k s:6 k | "

# the verse: a shouted line answers the riff
SHOUT1 = "e5:3 e5 r b5 r b5 d6:6 r g6 e6 b5 | "
SHOUT2 = "e5:3 e5 r b5 r b5 d6:6 r g6 e6 d6:3 e6 | "
BASS_SHOUT = "e3:3 e3 r e3 r e3 e3:6 r e3 e3 e3 | "

# the chorus: long notes, cut by the chug
CHORUS = ("e6:12 g6 e5:3 e5 r b5 r e5 e5 r | "
          "a6:12 g6 e5:3 e5 r b5 d6:6 e5 | "
          "e6:12 g6 e5:3 e5 r b5 r e5 e5 r | "
          "bb6:24 a6:12 g6:6 e6 | ")
BASS_CHORUS = ("e3:24 e3:3 e3 r b3 r e3 e3 r | "
            "a2:24 e3:3 e3 r b3 d4:6 e3 | "
            "c3:24 e3:3 e3 r b3 r e3 e3 r | "
            "bb2:24 a2:12 g3:6 e3 | ")

# the crushing part at half time: the chord, its octave, and a squeal
HEAVY = ("e5:12 r:6 e5:3 e5 e6:18 c7:6 | "
         "g5:12 r:6 g5:3 g5 d6:18 b6:6 | "
         "e5:12 r:6 e5:3 e5 e6:18 c7:6 | "
         "bb5:12 r:6 bb5:3 bb5 a5:12 b6 | ")
BASS_HEAVY = ("e3:12 r:6 e3:3 e3 e3:24 | "
           "g3:12 r:6 g3:3 g3 d3:24 | "
           "e3:12 r:6 e3:3 e3 e3:24 | "
           "bb2:12 r:6 bb2:3 bb2 a2:24 | ")
D_HEAVY = "k:12 r:6 k:3 k s:18 k:6 | "

# the solo: octave jumps like a pedal kicking in, and dives
SOLO = ("b5:3 b6 b5 b6 a5 a6 a5 a6 g5:6 g6 e6:12 | "
        "c7:3 b6 bb6 a6 ab6 g6 f#6 f6 e6:24 | "
        "e5:3 e6 e5 e6 g5 g6 a5 a6 bb5:6 bb6 b5 b6 | "
        "e6:3 g6 a6 b6 a6 g6 e6 d6 e6:12 r | ")
BASS_SOLO = line(eighths, "e3 e3 e3 e3")

# the end: the chug, and the squeal left to ring
END = "e5:3 e5 r b5 r e5 e5 r b6:24 | "
BASS_END = "e3:3 e3 r b3 r e3 e3 r e3:24 | "

# the riff alone for two bars, then the band comes in
MELODY = (H1 + H2
          + H1 + H2 + H1 + H3
          + SHOUT1 + H2 + SHOUT2 + H3
          + CHORUS
          + HEAVY
          + SOLO
          + H1 + H2 + H1 + H3
          + END)

BASS = ("r:48 | r:48 | "
        + BASS_H1 + BASS_H2 + BASS_H1 + BASS_H1
        + BASS_SHOUT + BASS_H2 + BASS_SHOUT + BASS_H1
        + BASS_CHORUS
        + BASS_HEAVY
        + BASS_SOLO
        + BASS_H1 + BASS_H2 + BASS_H1 + BASS_H1
        + BASS_END)

DRUMS = ("r:48 | r:48 | "
         + D_H1 + D_H2 + D_H1 + D_H1
         + ROCK + D_H2 + ROCK + FILL
         + DRIVE * 3 + FILL
         + D_HEAVY * 3 + FILL2
         + DRIVE * 3 + FILL
         + D_H1 + D_H2 + D_H1 + D_H1
         + LAST)
