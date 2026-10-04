# Go Go Go: our own, a three-chord punk song written for the game. A major,
# 180 to the beat, A, D and E and nothing else. The hook is the riff of the
# intro, two bars with a bluesy slip from C to C sharp in it, alone and then
# with the band. The verse sings a shouted line: a shout on beat one, then a
# handful of quick words. The chorus shouts GO GO GO three times up the
# chord, with a long note to hold after each. A short solo bashes the verse
# an octave higher; then the chorus again, the riff, and a stabbed ending.
# About fifty-six seconds.
#
# The bass hammers the root in eighths and the drums play the punk beat (a
# snare on every off-beat) from start to end.
#
# The tune never goes below E5 and the bass never above A3, so the two
# cannot meet on one pitch.

from kit import *

TITLE = "GO GO GO"
BY = "THE ROMONES"
STYLE = "Punk"
KEY = "A major"
BEATS = 4
TEMPO = [(0, 180)]

# the riff, two bars: A, then D and E
RIFF = ("a5:6 a5 c6 c#6 e6:12 a5:12 | "
        "d6:6 d6 c#6 a5 b5:24 | ")

# the verse: a shout on beat one, then the words (A A D E A A D E)
V1 = ("c#6:12 r:6 a5:6 a5 b5 c#6:12 | "
      "b5:6 a5 f#5:12 e5:12 r:12 | ")
V2 = ("d6:12 r:6 a5:6 a5 b5 d6:12 | "
      "c#6:6 b5 g#5:12 e5:12 r:12 | ")
V4 = ("d6:12 r:6 f#5:6 a5 b5 d6:12 | "
      "b5:6 c#6 d6:12 e6:24 | ")
VERSE = V1 + V2 + V1 + V4

# the chorus: GO GO GO up the chord, then a line down to a long note
# (D E A A D E A A)
CHORUS = ("d6:12 f#6 a6 r | "
          "g#6:12 f#6:6 e6 b5:24 | "
          "c#6:12 e6 a6 r | "
          "a6:6 b6 a6 e6 c#6:24 | "
          "d6:12 f#6 a6 r | "
          "b6:12 a6:6 g#6 e6:24 | "
          "c#6:12 e6 a6 c#7 | "
          "a6:36 r:12 | ")

# the solo: the verse an octave up, bashed (A A D E A A D E)
SOLO = ("c#7:6 c#7 r e6 a6 b6 c#7:12 | "
        "b6:6 a6 f#6:12 e6:24 | "
        "a6:12 r:6 f#6:6 d6 a6 b6:12 | "
        "g#6:6 b6 g#6 e6 b5:24 | "
        "c#7:6 c#7 r e6 a6 b6 c#7:12 | "
        "e6:6 a6 c#7 a6 e6:24 | "
        "d6:6 f#6 a6:12 f#6:6 a6 b6:12 | "
        "b6:6 g#6 e6 b5 g#5:12 e5:12 | ")

# the end: GO GO GO as stabs, the last one held
END = ("c#6:6 r:18 e6:6 r:18 | "
       "a6:6 r a6 r a6:24 | ")

# the bass: roots in eighths, a walk down from A to D into the chorus
B_RIFF = eighths("a3") + "d3:6 d3 d3 d3 e3 e3 e3 e3 | "
B_VERSE = line(eighths, "a3 a3 d3 e3 a3 a3 d3 e3")
B_CHORUS = (line(eighths, "d3 e3 a3") + "a3:6 a3 a3 a3 a3 g3 f#3 e3 | "
            + line(eighths, "d3 e3 a3 a3"))
B_END = "a3:6 r:18 a3:6 r:18 | a3:6 r a3 r a3:24 | "

D_FILL = "k:6 s k s s:3 s s s s s s s | "
D_END = "k:6 r:18 k:6 r:18 | k:6 r k r k:6 s:18 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF
          + VERSE
          + CHORUS
          + RIFF
          + SOLO
          + CHORUS
          + RIFF
          + END)

BASS = (rest(2) + B_RIFF
        + B_VERSE
        + B_CHORUS
        + B_RIFF
        + B_VERSE
        + B_CHORUS
        + B_RIFF
        + B_END)

DRUMS = (rest(2) + PUNK + D_FILL
         + beat(PUNK, 8, D_FILL, 8)
         + beat(PUNK, 8, D_FILL, 8)
         + PUNK + D_FILL
         + beat(PUNK, 8, D_FILL, 8)
         + beat(PUNK, 8, D_FILL, 8)
         + PUNK + D_FILL
         + D_END)
