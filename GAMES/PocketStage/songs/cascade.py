# Cascade: our own, an arena rock track written for the game. B minor, 118
# to the beat. The riff is the hook and burns slowly: two quick notes and a
# long one, over Bm, G, A and F#, while the band hits with the first two
# and stops (the tune holds the long note alone), and a little turn leads
# on. A verse over a pulsing eighth bass that climbs a step every bar; a
# soaring chorus over G, D, A and Bm that starts every line on a long high
# note and ripples down; the riff again, and a climb to a long high B to
# end. About fifty-five seconds.
#
# The tune never goes below A5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "CASCADE"
BY = "FOREIGN KEY"
STYLE = "Arena rock"
KEY = "B minor"
BEATS = 4

TEMPO = [(0, 118)]

# the riff: two quick notes, a long one while the band stops, a turn
RIFF = ("b5:6 d6:6 f#6:30 e6:3 d6:3 | "
        "g6:6 f#6:6 d6:30 c#6:3 d6:3 | "
        "c#6:6 e6:6 a6:30 g6:3 f#6:3 | "
        "e6:6 c#6:6 a#5:36 | ")

# the verse: a note sung three times, a step down and back, a leap; a step
# higher every bar
VERSE = ("b5:6 b5 b5 a5 b5:12 d6:6 b5 | "
         "c#6:6 c#6 c#6 b5 c#6:12 e6:6 c#6 | "
         "d6:6 d6 d6 c#6 d6:12 g6:6 f#6 | "
         "e6:12 c#6:6 a5 e6:12 c#6:6 e6 | "
         "b5:6 b5 b5 a5 b5:12 d6:6 b5 | "
         "c#6:6 c#6 c#6 b5 c#6:12 e6:6 c#6 | "
         "d6:6 d6 d6 e6 g6:12 a6:6 b6 | "
         "a#6:12 f#6:6 c#6 a#5 c#6 e6 f#6 | ")

# the chorus: a long high note, then a ripple down
CHORUS = ("b6:24 a6:6 g6 f#6 g6 | "
          "a6:24 f#6:6 e6 d6 e6 | "
          "e6:24 c#6:6 d6 e6 f#6 | "
          "f#6:24 d6:6 e6 f#6 a6 | "
          "b6:24 a6:6 g6 f#6 g6 | "
          "a6:24 f#6:6 e6 d6 e6 | "
          "e6:12 a6 b6:6 a6 g6 e6 | "
          "f#6:24 e6:6 c#6 a#5 c#6 | ")

# the end: a climb over G and A to the leading note, and a long high B
END = ("g6:6 f#6:6 d6:12 c#6:6 e6 a6 a#6 | "
       "b6:48 | -:36 r:12 | ")


def stops(*roots):
    """the stop accents, a bar on each root: struck twice with the riff's
    first two notes, then nothing while the tune holds"""
    return "".join("%s:6 %s:6 r:36 | " % (r, r) for r in roots)


B_RIFF = stops("b2", "g3", "a2", "f#3")
B_VERSE = line(eighths, "b2 a2 g3 a2 b2 a2 g3 f#3")
B_CHORUS = (line(fifths, "g3 d3 a2 b2 g3 d3")
            + "a2:6 a2 a2 a2 g3 g3 g3 g3 | "
            + eighths("f#3"))
B_END = "g3:6 g3 g3 g3 a2 a2 f#3 f#3 | b2:48 | -:36 r:12 | "

D_STOP = "k:6 k s:36 | "                 # kick, kick, snare, then nothing
D_STOP_FILL = "k:6 k s:24 s:3 s s s | "
D_END = FILL + LAST + rest(1)

# the stops from the first bar: the band is the riff
MELODY = (RIFF + VERSE + CHORUS + RIFF + END)

BASS = (B_RIFF + B_VERSE + B_CHORUS + B_RIFF + B_END)

DRUMS = (D_STOP * 3 + D_STOP_FILL
         + beat(ROCK, 8, FILL)
         + beat(DRIVE, 8, FILL)
         + D_STOP * 3 + D_STOP_FILL
         + D_END)
