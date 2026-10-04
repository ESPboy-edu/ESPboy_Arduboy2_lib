# Low Power: our own, a reggae rock song written for the game. A minor, 152
# to the beat; the verses in a reggae half-time feel, the chorus straight
# rock. The hook is the figure of the verse: three quick notes up the chord
# into a high note left to ring, a bar of space, and a little answer falling
# back (A minor, D minor, F, G). Under it the bass plays a reggae line full
# of rests and the drums a one-drop, nothing but the snare on beat three.
# The chorus is plain rock with long notes: LOW POW-ER, each time a step
# lower, as if the battery runs down, then back up. A second verse that
# sings a little more, the chorus again, and the figure to end on a long A.
# Easy to play. About fifty-seven seconds.
#
# The tune never goes below A5 and the bass never above C4, so the two
# cannot meet on one pitch.

from kit import *

TITLE = "LOW POWER"
BY = "THE POLLING"
STYLE = "Reggae rock"
KEY = "A minor"
BEATS = 4
TEMPO = [(0, 152)]

# the figure: up the chord, let it ring, space, the answer
FIG_AM = "a5:6 e6:6 a6:6 c7:30 | r:24 b6:6 a6:6 e6:12 | "
FIG_DM = "a5:6 d6:6 f6:6 a6:30 | r:24 g6:6 f6:6 d6:12 | "
FIG_FG = "a5:6 c6 f6 a6:30 | b5:6 d6 g6 b6:30 | "
VERSE = FIG_AM + FIG_DM + FIG_AM + FIG_FG

# the second verse sings a little more over the same chords
VERSE2 = ("a5:6 e6:6 a6:6 c7:30 | r:12 b6:6 a6:6 e6:6 g6:6 a6:12 | "
          "a5:6 d6:6 f6:6 a6:30 | r:12 g6:6 f6:6 d6:6 e6:6 f6:12 | "
          "e6:12 g6:6 a6:6 c7:12 a6:12 | g6:12 e6:6 d6:6 c6:24 | "
          + FIG_FG)

# the chorus: LOW POW-ER, a step lower each time, then back up
# (F G Am Am F G E E)
CHORUS = ("a6:24 g6:12 f6:12 | "
          "g6:24 f6:12 d6:12 | "
          "e6:24 d6:12 c6:12 | "
          "a5:36 r:12 | "
          "a6:24 g6:12 f6:12 | "
          "g6:24 a6:12 b6:12 | "
          "b6:24 c7:12 b6:12 | "
          "g#6:36 r:12 | ")

END = "a5:6 e6:6 a6:6 c7:30 | a6:48 | "

# the bass: a reggae line with rests, two bars on each chord; rock eighths
# in the chorus
B_AM = "a3:12 r:6 a3:6 c4:12 e3:6 g3:6 | a3:6 r:18 r:6 e3:6 g3:6 e3:6 | "
B_DM = "d3:12 r:6 d3:6 f3:12 a3:6 c4:6 | d3:6 r:18 r:6 a3:6 g3:6 e3:6 | "
B_FG = "f3:12 r:6 f3:6 a3:12 c4:6 a3:6 | g3:12 r:6 g3:6 b3:12 a3:6 g3:6 | "
B_VERSE = B_AM + B_DM + B_AM + B_FG
B_CHORUS = line(eighths, "f3 g3 a3 a3 f3 g3 e3 e3")
B_END = "a3:12 r:6 a3:6 c4:12 e3:6 g3:6 | a3:48 | "

D_DROP = "r:24 s:24 | "                  # the one-drop: the snare on three
D_DROP_FILL = "r:24 s:12 s:6 s | "
D_VERSE = (D_DROP * 3 + D_DROP_FILL) * 2

# the figure alone for two bars, then the band comes in
MELODY = (FIG_AM
          + VERSE
          + CHORUS
          + VERSE2
          + CHORUS
          + END)

BASS = (rest(2)
        + B_VERSE
        + B_CHORUS
        + B_VERSE
        + B_CHORUS
        + B_END)

DRUMS = (rest(2)
         + D_VERSE
         + beat(ROCK, 8, FILL, 8)
         + D_VERSE
         + beat(ROCK, 8, FILL, 8)
         + D_DROP + LAST)
