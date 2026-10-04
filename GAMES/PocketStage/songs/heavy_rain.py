# Heavy Rain: our own, a sludgy grunge track written for the game. E minor,
# slow at 84 to the beat, and dark. The hook is the riff: a slow bend up into
# E, then G, the tritone B-flat and back down; then a sour bend from G to G#
# and the half step F falling onto E. The chorus is long notes climbing from
# G to C while the bass slides down against them a half step at a time, E to
# B, so the chords go darker under the tune. A short crying solo of bends,
# the chorus again higher, the riff, and a last bend into a long E. About
# fifty-four seconds.
#
# The tune never goes below D5 and the bass never above B3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "HEAVY RAIN"
BY = "ALICE IN CHIPS"
STYLE = "Sludgy grunge"
KEY = "E minor"
BEATS = 4

TEMPO = [(0, 84)]

# the riff, two bars, and its turn up to B for the chorus
R1 = "d5:3 e5:15 e5:6 g5:6 bb5:6 a5:6 g5:6 | "
R2 = "e5:6 e5 g5:3 g#5:9 a5:6 f5:6 e5:12 | "
R3 = "e5:6 e5 g5:3 g#5:9 a5:6 bb5:6 b5:12 | "
RIFF = R1 + R2

# long notes climbing while the bass goes down
CHORUS = ("g5:36 f#5:6 g5:6 | "
          "a5:24 b5:24 | "
          "c6:36 b5:6 c6:6 | "
          "b5:24 a5:12 g5:12 | ")
CHORUS2 = ("b5:36 a5:6 b5:6 | "
           "c6:24 b5:24 | "
           "e6:36 d6:6 e6:6 | "
           "c6:24 d#6:12 b5:12 | ")

# crying bends over the riff
SOLO = ("d6:3 e6:21 g6:6 e6:6 d6:3 e6:9 | "
        "g6:3 a6:9 g6:6 e6:6 e6:3 f6:3 e6:6 d#6:12 | ")

END = "d5:3 e5:45 | "

# the bass doubles the riff two octaves down; under the chorus it slides
# from E down to B a half step at a time, then A and up to B
B1 = "e3:24 g3:6 bb3:6 a3:6 g3:6 | "
B2 = "e3:24 a3:6 f3:6 e3:12 | "
B3 = "e3:24 a3:6 bb3:6 b3:12 | "
BASS_RIFF = B1 + B2
BASS_CHORUS = "e3:24 d#3:24 | d3:24 c#3:24 | c3:24 b2:24 | a2:24 b2:24 | "

D_HEAVY = "k:12 s k:6 k s:12 | "        # a kick that drags behind the snare

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF + R1 + R3
          + CHORUS
          + SOLO
          + CHORUS2
          + RIFF
          + END)

BASS = (rest(2)
        + BASS_RIFF + B1 + B3
        + BASS_CHORUS
        + B1 + B3
        + BASS_CHORUS
        + BASS_RIFF
        + "e3:48 | ")

DRUMS = (rest(2)
         + beat(D_HEAVY, 4, TRIPLETS)
         + beat(BACKBEAT, 4, TRIPLETS)
         + D_HEAVY + FILL
         + beat(BACKBEAT, 4, TRIPLETS)
         + D_HEAVY + STABS
         + LAST)
