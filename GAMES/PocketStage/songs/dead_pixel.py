# Dead Pixel: our own, a doom metal track written for the game. E minor, 72
# to the beat, slow and heavy. The hook is the riff: a struck E, E and G, and
# a long B-flat (the tritone) that crawls back down to a long E, with the
# bass on it two octaves down. The riff alone, then the drums at half time; a
# verse that sings over the riff and creeps down in half steps; a middle part
# where the riff runs at twice the pace in eighths; the slow riff again and a
# long E to end. About fifty-three seconds, and easy: long notes to hold.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "DEAD PIXEL"
BY = "BLACK SABBYTE"
STYLE = "Doom metal"
KEY = "E minor"
BEATS = 4

TEMPO = [(0, 72)]

# the riff, two bars, and its turn
R1 = ("e5:12 e5:6 g5:6 bb5:24 | "
      "f5:6 e5 f5 g5 e5:24 | ")
R2 = ("e5:12 e5:6 g5:6 bb5:24 | "
      "c6:6 b5 bb5 a5 e5:24 | ")

# the verse sings over the riff; at its end it creeps down with the bass
VERSE = ("g5:12 a5:6 b5 d6:12 c6:6 bb5 | "
         "a5:6 g5 a5 b5 g5:12 e5:6 g5 | "
         "g5:12 a5:6 b5 f6:12 e6:6 d6 | "
         "e6:6 d#6 d6 c#6 b5:24 | ")

# the middle part: the riff at twice the pace, in eighths
M1 = "e5:6 e5 g5 e5 a5 a5 bb5 a5 | "
M2 = "g5:6 g5 bb5 g5 c6 bb5 a5 f5 | "
M3 = "g5:6 g5 bb5 g5 b5 c6 d6 e6 | "
MIDDLE = M1 + M2 + M1 + M3

# slow again: the riff, the B-flat once more, and the E rings
OUTRO = R1 + "e5:12 e5:6 g5:6 bb5:24 | e5:48 | "

# half time: kicks with the riff, the snare on 3
D_A = "k:12 k:6 k:6 s:24 | "
D_B = "k:6 k k k s:24 | "
D_FILL = "k:6 k k k s:6 s s:3 s s s | "

# the riff (with the bass on it) alone for two bars, then the drums come in
MELODY = (R1 + R2
          + VERSE
          + MIDDLE
          + OUTRO)

# the bass is the riff two octaves down, under the verse too
BASS = (shift(R1 + R2, -24)
        + shift(R1 + R2, -24)
        + shift(MIDDLE, -24)
        + shift(OUTRO, -24))

DRUMS = (rest(2)
         + D_A + D_B
         + D_A + D_B + D_A + D_FILL
         + DRIVE * 3 + FILL                  # twice the pace
         + D_A + D_B + D_A + LAST)
