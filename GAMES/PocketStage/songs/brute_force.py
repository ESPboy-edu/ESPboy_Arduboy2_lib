# Brute Force: our own, a neoclassical metal track written for the game. A
# harmonic minor, 152 to the beat. The riff is the hook and gallops: a tune
# on the beat (E F E C, then B C B G#) with the low A or E struck twice
# under each note in sixteenths, like a pedal in Bach, and a stop on the
# fourth beat; the bass and the kick gallop with it. Then sequences in
# sixteenths, a turn of three notes and a drop moved down a step at a time,
# a bar a step higher (Am, Dm, E, Am); an epic chorus at half time with long
# notes over F G C Am, Dm Am E; a solo of sweeps (six sixteenths up and down
# Am, Dm and E) and a run down the scale; the end of the chorus again, the
# gallop, and a climb to a long high A. Never more than eight sixteenths in
# a row. About fifty-four seconds.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "BRUTE FORCE"
BY = "RAINBOW TABLE"
STYLE = "Neoclassical metal"
KEY = "A harmonic minor"
BEATS = 4

TEMPO = [(0, 152)]

# the riff: the tune on the beat, the pedal under it twice, a stop
R1 = "e6:6 a5:3 a5 f6:6 a5:3 a5 e6:6 a5:3 a5 c6:12 | "
R2 = "b5:6 e5:3 e5 c6:6 e5:3 e5 b5:6 e5:3 e5 g#5:12 | "
R3 = "b5:6 e5:3 e5 d6:6 e5:3 e5 f6:6 g#6:3 a6 b6:12 | "
RIFF = R1 + R2

# the sequences: a turn and a drop, moved down a step; a bar a step higher
SEQ = ("e6:3 d6 e6 a5 d6 c6 d6 g#5 a5:6 c6:3 b5 c6 d6 e6:6 | "
       "f6:3 e6 f6 a5 e6 d6 e6 a5 d6:6 e6:3 f6 g#6 a6 b6:6 | "
       "g#6:3 f6 g#6 b5 f6 e6 f6 b5 e6:6 b5:3 d6 e6 f6 g#6:6 | "
       "a6:3 g#6 a6 c6 g#6 f6 g#6 b5 a5:6 e6 a5:12 | ")

# the chorus: half time, long notes
CHORUS_A = ("c6:12 f6 a6:24 | "
            "g6:24 f6:6 e6 d6:12 | "
            "e6:12 g6 c7:24 | "
            "b6:12 c7 a6:24 | ")
CHORUS_B = ("a6:36 g6:6 f6 | "
            "e6:24 d6:6 c6 e6:12 | "
            "g#6:24 b6:12 g#6 | "
            "e6:36 r:12 | ")

# the solo: sweeps, six sixteenths up and down a chord, and a landing
SOLO = ("a5:3 c6 e6 a6 e6 c6 a5:6 c6:3 e6 a6 c7 a6 e6 c6:6 | "
        "a5:3 d6 f6 a6 f6 d6 a5:6 d6:3 f6 a6 f6 d6 a5 f6:6 | "
        "g#5:3 b5 e6 g#6 e6 b5 g#5:6 b5:3 e6 g#6 b6 g#6 e6 b5:6 | "
        "a5:3 c6 e6 a6 e6 c6 e6:6 c6:3 e6 a6 c7 a6 e6 a6:6 | "
        "c7:3 b6 a6 g#6 f6 e6 d6 c6 b5:6 e6 g#6 b6 | "
        "b6:6 a6 g#6 f6 e6:12 g#6:12 | ")

END = ("e6:6 a5:3 a5 f6:6 a5:3 a5 g#6:6 a5:3 a5 b6:12 | "
       "a6:36 r:12 | ")

B_RIFF = line(gallop, "a2 e3")
B_SEQ = line(eighths, "a2 d3 e3 a2")
B_CHORUS_A = line(halves, "f3 g3 c3 a2")
B_CHORUS_B = line(halves, "d3 a2 e3 e3")
B_SOLO = (line(eighths, "a2 d3 e3 a2")
          + "f3:6 f3 f3 f3 e3 e3 e3 e3 | " + eighths("e3"))
B_END = gallop("a2") + "a2:36 r:12 | "

D_END = GALLOP + LAST

# the riff alone for two bars, then the band comes in galloping
MELODY = (RIFF
          + RIFF + R1 + R3
          + SEQ
          + CHORUS_A + CHORUS_B
          + SOLO
          + CHORUS_B
          + RIFF + RIFF
          + END)

BASS = ("r:48 | r:48 | "
        + B_RIFF * 2
        + B_SEQ
        + B_CHORUS_A + B_CHORUS_B
        + B_SOLO
        + B_CHORUS_B
        + B_RIFF * 2
        + B_END)

DRUMS = (rest(2)
         + beat(GALLOP, 4, FILL)
         + beat(DRIVE, 4, FILL)
         + beat(HALF, 8, ROLL, every=8)
         + DRIVE + DOUBLE + DRIVE + DOUBLE + DRIVE + FILL
         + beat(HALF, 4, ROLL)
         + beat(GALLOP, 4, FILL)
         + D_END)
