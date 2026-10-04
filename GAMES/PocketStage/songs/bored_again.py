# Bored Again: our own, a pop punk song written for the game. E major, 176
# to the beat. The hook is the palm-muted riff of the verse: eighths in
# groups of three, three and two, each group a step up or down the chord, so
# it chugs and moves at once. It plays alone for two bars, then the band
# comes in and it carries the verse (E, C sharp minor, A, B), which ends on a
# stop and three notes up into the chorus. The chorus explodes: the drums go
# to the punk beat and the tune sings BORED A-GAIN high up, a long note each
# time. A bridge drops to half time with long notes, builds up and stops
# dead; the chorus again, the riff, and the hook of the chorus once more to
# end on a long E. About fifty-five seconds.
#
# The tune never goes below E5 and the bass never above C sharp 4, so the
# two cannot meet on one pitch.

from kit import *

TITLE = "BORED AGAIN"
BY = "SCREEN DAY"
STYLE = "Pop punk"
KEY = "E major"
PREVIEW = 13                # the setlist's preview starts at this bar
BEATS = 4
TEMPO = [(0, 176)]

# the riff, palm-muted: three, three and two eighths (E, C sharp minor; A, B)
R1 = ("e5:6 e5 e5 b5 b5 b5 a5 g#5 | "
      "e5:6 e5 e5 c#6 c#6 c#6 b5 g#5 | ")
R2 = ("e5:6 e5 e5 c#6 c#6 c#6 e6 c#6 | "
      "f#5:6 f#5 f#5 d#6 d#6 d#6 f#6:12 | ")
R1B = ("e5:6 e5 e5 b5 b5 b5 c#6 b5 | "
       "e5:6 e5 e5 e6 e6 e6 d#6 c#6 | ")
# the end of the verse: a stop, then three notes up into the chorus
R3 = ("e5:6 e5 e5 c#6 c#6 c#6 e6 c#6 | "
      "b5:6 r:18 d#6:6 e6 f#6:12 | ")

# the chorus: BORED A-GAIN (E B C#m A E B A B)
CHORUS = ("g#6:18 e6:6 f#6:24 | "
          "f#6:12 d#6:6 b5 d#6:12 f#6:12 | "
          "g#6:18 c#6:6 e6:24 | "
          "e6:12 c#6:6 a5 c#6:12 e6:12 | "
          "g#6:18 e6:6 f#6:24 | "
          "b6:18 a6:6 f#6:24 | "
          "e6:12 c#6 e6 a6 | "
          "f#6:36 r:12 | ")

# the bridge at half time: long notes, then up and a dead stop
# (C#m A E B C#m A B B)
BRIDGE = ("e6:24 d#6:12 c#6:12 | "
          "c#6:36 r:12 | "
          "b5:24 c#6:12 e6:12 | "
          "d#6:36 r:12 | "
          "e6:24 f#6:12 g#6:12 | "
          "a6:36 g#6:12 | "
          "f#6:12 d#6 b5 d#6 | "
          "f#6:6 g#6 a6 b6 r:24 | ")

# the end: the hook of the chorus, landing on E at last
END = ("g#6:18 e6:6 f#6:24 | "
       "e6:36 r:12 | ")

# the bass: roots in eighths; halves at half time
B_VERSE = (line(eighths, "e3 c#4 a3 b3 e3 c#4 a3")
           + "b3:6 r:18 b3:6 b3 b3:12 | ")
B_CHORUS = line(eighths, "e3 b3 c#4 a3 e3 b3 a3 b3")
B_BRIDGE = (line(halves, "c#4 a3 e3 b3 c#4 a3")
            + "b3:12 b3 b3 b3 | b3:6 b3 b3 b3 r:24 | ")
B_END = "e3:6 e3 e3 e3 b3 b3 b3 b3 | e3:36 r:12 | "

D_BREAK = "k:6 r:18 s:6 s s:3 s s s | "
D_BRIDGE = (HALF * 3 + TRIPLETS + HALF * 2
            + "s:12 s s s | "                      # the build
            + "s:3 s s s s s s s r:24 | ")         # and the stop

# the riff alone for two bars, then the band comes in
MELODY = (R1 + R2
          + R1 + R2 + R1B + R3                   # verse
          + CHORUS
          + BRIDGE
          + CHORUS
          + R1 + END)

BASS = (rest(2) + line(eighths, "a3 b3")
        + B_VERSE
        + B_CHORUS
        + B_BRIDGE
        + B_CHORUS
        + line(eighths, "e3 c#4") + B_END)

DRUMS = (rest(2) + DRIVE + FILL
         + beat(DRIVE, 7, FILL, 4) + D_BREAK
         + beat(PUNK, 8, FILL, 8)
         + D_BRIDGE
         + beat(PUNK, 8, FILL, 8)
         + DRIVE + FILL + PUNK + LAST)
