# Escape Key: our own, a punk rock song written for the game. A minor, 172
# to the beat. The hook is the lick that opens the song, in A harmonic
# minor: a little turn around a note in sixteenths, then the same turn a
# third higher and a third higher again, up to a top A (or, the second time,
# down the scale to the low A). It plays alone for two bars, then with the
# band, and comes back between the sections and at the end. The verse is a
# punk tune over a driving bass (A minor, F, G, E); the chorus calls
# ES-CAPE, ES-CAPE and answers with a line down to a long note; a fast solo
# in sixteenth bursts (high notes against an E pedal, broken chords); the
# chorus again and the lick once more to end on a held A. About fifty-three
# seconds.
#
# The tune never goes below E5 and the bass never above C4, so the two
# cannot meet on one pitch.

from kit import *

TITLE = "ESCAPE KEY"
BY = "THE OFFSET"
STYLE = "Punk rock"
KEY = "A minor"
BEATS = 4
TEMPO = [(0, 172)]

# the lick: a turn, a third higher, a third higher again (A minor, then E)
L1 = "a5:3 g#5 a5 b5 c6:6 a5 c6:3 b5 c6 d6 e6:6 c6 | "
LICK_UP = L1 + "e6:3 d6 e6 f6 g#6:12 a6:12 e5:12 | "
LICK_DOWN = L1 + "f6:3 e6 d6 c6 b5:12 e6:6 a5:12 r:6 | "
LICK_STOP = L1 + "e6:3 d6 e6 f6 g#6:12 a6:12 r:12 | "
LICK_END = L1 + "e6:3 d6 e6 f6 g#6:12 a6:24 | "

# the verse (Am Am F G Am Am F E)
VERSE = ("a5:6 a5 a5 c6 e6:12 d6:6 c6 | "
         "b5:6 c6 b5 a5 e5:24 | "
         "a5:6 a5 a5 c6 f6:12 e6:6 d6 | "
         "d6:6 e6 d6 b5 g5:24 | "
         "a5:6 a5 a5 c6 e6:12 d6:6 c6 | "
         "b5:6 c6 b5 a5 e6:24 | "
         "f6:12 e6:6 d6 c6:12 a5:12 | "
         "b5:6 c6 d6 e6 g#6:24 | ")

# the chorus: ES-CAPE, ES-CAPE, and a line down (Am F C G Am F E E)
CHORUS = ("e6:6 a6:18 e6:6 a6:18 | "
          "g6:6 f6 e6 f6 c6:24 | "
          "e6:6 g6:18 e6:6 g6:18 | "
          "f6:6 e6 d6 e6 b5:24 | "
          "e6:6 a6:18 e6:6 a6:18 | "
          "c7:12 a6:6 g6 f6:24 | "
          "e6:6 f6 g#6 a6 b6:12 g#6:12 | "
          "e6:36 e5:12 | ")

# the solo: bursts of sixteenths (Am G F E F E)
SOLO = ("a6:3 e6 b6 e6 c7:6 e6 b6:3 e6 a6 e6 g#6:6 e6 | "
        "b6:6 g6 d6 g6 b6:12 a6:6 g6 | "
        "a6:3 g6 f6 e6 f6:12 c7:3 b6 a6 g#6 a6:12 | "
        "g#6:6 e6 b5 e6 g#6:12 b6:12 | "
        "c7:3 b6 a6 g#6 a6:12 c7:3 b6 a6 g#6 a6:12 | "
        "e6:3 g#6 b6 g#6 e6:6 b5 d6:3 f6 g#6 f6 d6:6 b5 | ")

# the bass: the root in eighths, driving, with a step at the end of a bar
B_LICK = eighths("a3") + "e3:6 e3 e3 e3 a3:12 r:12 | "
B_VERSE = ("a3:6 a3 a3 a3 a3 a3 c4 a3 | "
           "a3:6 a3 a3 a3 g3 g3 e3 e3 | "
           "f3:6 f3 f3 f3 f3 f3 a3 f3 | "
           "g3:6 g3 g3 g3 g3 g3 b3 g3 | "
           "a3:6 a3 a3 a3 a3 a3 c4 a3 | "
           "a3:6 a3 a3 a3 g3 g3 e3 e3 | "
           "f3:6 f3 f3 f3 f3 f3 e3 d3 | "
           "e3:6 e3 e3 e3 e3 e3 g#3 b3 | ")
B_CHORUS = line(eighths, "a3 f3 c4 g3 a3 f3 e3") + "e3:6 e3 e3 e3 e3 e3 f3 g#3 | "
B_SOLO = line(eighths, "a3 g3 f3 e3 f3") + "e3:6 e3 e3 e3 e3 e3 g#3 b3 | "
B_END = eighths("a3") + "e3:6 e3 e3 e3 a3:24 | "

D_LICK = DBEAT + "k:6 s k s k:12 r:12 | "
D_END = DBEAT + "k:6 s k s k:6 s:18 | "

# the lick alone for two bars, then the band comes in
MELODY = (LICK_UP + LICK_DOWN
          + VERSE
          + CHORUS
          + LICK_STOP
          + SOLO
          + CHORUS
          + LICK_END)

BASS = (rest(2) + B_LICK
        + B_VERSE
        + B_CHORUS
        + B_LICK
        + B_SOLO
        + B_CHORUS
        + B_END)

DRUMS = (rest(2) + D_LICK
         + beat(PUNK, 8, FILL, 8)
         + beat(DRIVE, 8, FILL, 4)
         + D_LICK
         + beat(PUNK, 6, FILL2, 6)
         + beat(DRIVE, 8, FILL, 4)
         + D_END)
