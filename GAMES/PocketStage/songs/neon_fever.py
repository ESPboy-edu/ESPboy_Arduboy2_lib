# Neon Fever: our own, a glam metal track written for the game. A, 132 to
# the beat. The riff is the hook: a palm-muted chug on A with a stutter in
# it, then a bluesy turn (up D Eb E, then down G E D C A), the bass
# doubling it three octaves down. The riff moves to D and E like a blues
# for the verse; the chorus is a gang shout in stop time (the band hits
# with every shout) and then one long note, over D E, then F G and A; a
# flashy solo with fast triplets and a dive down the scale; the riff again
# and stabs into a last long A. About fifty-five seconds.
#
# The tune never goes below E5 and the bass never above B3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "NEON FEVER"
BY = "MOTLEY CODE"
STYLE = "Glam metal"
KEY = "A"
BEATS = 4

TEMPO = [(0, 132)]

# the riff, two bars: chug, stutter, chug, the turn up; then the fall
R1 = "a5:6 a5 r:3 a5:3 c6:6 a5:6 d6:3 eb6:3 e6:6 a5:6 | "
R2 = "a5:6 a5 r:3 a5:3 g6:6 e6:6 d6:3 c6:3 a5:12 | "
R1D = shift(R1, 5)                     # the same on D
R1E = shift(R1, 7)                     # and on E
TURN = "a5:6 a5 r:3 a5:3 c#6:6 e6:6 f#6:3 g#6:3 a6:12 | "
RIFF = R1 + R2

# the chorus: shouts in stop time, then a long note
CHORUS = ("d6:6 r d6 r d6:3 d6 e6:6 r f#6 | "
          "e6:36 r:12 | "
          "f6:6 r f6 r g6:3 g6 a6:6 r g6 | "
          "a6:36 r:12 | ")

SOLO = ("a6:12 g6:3 a6 g6:6 e6 g6 a6 c7 | "
        "a6:4 g6 e6 a6 g6 e6 a6 g6 e6 d6:6 e6 | "
        "f#6:6 a6 b6:12 a6:3 b6 a6:6 f#6 e6 | "
        "d6:4 f#6 a6 d6 f#6 a6 e6 g6 b6 e6 g6 b6 | "
        "c7:12 a6:6 f6 b6:12 g6:6 d6 | "
        "c7:3 a6 g6 e6 d6 c6 a5 g5 e5:24 | ")      # the dive

END = "a5:6 r a5 r c6:3 c#6 e6:6 r a6 | a6:36 r:12 | "

# the bass doubles the riff three octaves down
B1 = shift(R1, -36)
B2 = shift(R2, -36)
B1D = shift(R1D, -36)
B1E = shift(R1E, -36)
BTURN = "a2:6 a2 r:3 a2:3 c#3:6 e3:6 f#3:3 g#3:3 a3:12 | "
BRIFF = B1 + B2
B_CHORUS = ("d3:6 r d3 r d3:3 d3 d3:6 r d3 | "
            + eighths("e3")
            + "f3:6 r f3 r g3:3 g3 g3:6 r g3 | "
            + eighths("a2"))
B_SOLO = (line(eighths, "a2 a2 d3 d3")
          + "f3:6 f3 f3 f3 g3 g3 g3 g3 | " + eighths("e3"))
B_END = "a2:6 r a2 r a2:3 a2 a2:6 r a2 | a2:36 r:12 | "

D_SHOUT = "k:6 r s r k:3 k s:6 r k | "    # with the shouts
D_END = "k:6 r k r k:3 k s:6 r s | " + LAST

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF + R1D + R2 + R1E + TURN
          + CHORUS
          + RIFF
          + SOLO
          + CHORUS
          + RIFF + R1 + TURN
          + END)

BASS = (rest(2)
        + BRIFF + B1D + B2 + B1E + BTURN
        + B_CHORUS
        + BRIFF
        + B_SOLO
        + B_CHORUS
        + BRIFF + B1 + BTURN
        + B_END)

DRUMS = (rest(2)
         + beat(ROCK, 6, FILL)
         + D_SHOUT + DRIVE + D_SHOUT + FILL2
         + ROCK + FILL
         + beat(ROCK, 6, FILL, every=3)
         + D_SHOUT + DRIVE + D_SHOUT + FILL2
         + beat(ROCK, 4, FILL)
         + D_END)
