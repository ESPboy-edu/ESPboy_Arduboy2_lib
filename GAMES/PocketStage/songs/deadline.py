# Deadline: our own, a thrash metal track written for the game. E minor, 196
# to the beat, down-picked. The hook is the riff: four eighths on the low E,
# then a stab on C falling to B; four more, and a run down from B-flat; and
# at the end of four bars a climb in half steps from E to B, like a clock
# running out. A verse where the riff and a shouted line take turns, a
# chorus of long notes, a breakdown at half time with a chunky, off-beat
# riff, a solo of short tremolo bursts and wide leaps, the riff again, and a
# held E with stabs to end. The bass doubles the riff. About fifty-four
# seconds.
#
# The tune never goes below E5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "DEADLINE"
BY = "METAL OXIDE"
STYLE = "Thrash metal"
KEY = "E minor"
PREVIEW = 15                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 196)]

# the riff, four bars: the stab, the run down, the stab, the climb
A1 = "e5:6 e5 e5 e5 c6:12 b5 | "
A2 = "e5:6 e5 e5 e5 bb5 a5 g5 f#5 | "
A4 = "e5:6 f5 f#5 g5 ab5 a5 bb5 b5 | "
RIFF = A1 + A2 + A1 + A4

# the verse: the riff and a shouted line take turns
VERSE = (A1 + "e6:6 e6 d6 b5 g5:12 r | "
         + A1 + "e6:6 e6 g6 e6 b5:12 r | "
         + "e5:6 e5 e5 e5 d6:12 c6 | " + "e6:6 e6 c6 g5 e5:12 r | "
         + A2 + "g5:6 a5 bb5 b5 d6 c#6 c6 b5 | ")
BASS_VERSE = (shift(A1, -24) + eighths("e3")
           + shift(A1, -24) + eighths("e3")
           + "e3:6 e3 e3 e3 d4:12 c4 | " + eighths("c3")
           + shift(A2, -24) + "g3:6 a3 bb3 b3 d4 c#4 c4 b3 | ")

# the chorus, long notes: E, C, D, B, twice
CHORUS = ("e6:36 d6:6 e6 | "
          "g6:36 f#6:6 e6 | "
          "f#6:24 d6 | "
          "d#6:24 b5:12 d#6 | "
          "e6:36 d6:6 e6 | "
          "g6:36 a6:6 b6 | "
          "a6:24 f#6 | "
          "b5:12 d#6 f#6 a6 | ")
BASS_CHORUS = line(eighths, "e3 c3 d3 b2 e3 c3 d3 b2")

# the breakdown at half time: chunky, off the beat
BREAK = ("e5:6 e5 r g5 r e5 bb5:12 | "
         "e5:6 e5 r g5 r e5 a5 ab5 | "
         "e5:6 e5 r g5 r e5 bb5:12 | "
         "g5:6 g5 r bb5 a5:12 ab5 | "
         "e5:6 e5 r g5 r e5 bb5:12 | "
         "e5:6 e5 r g5 bb5 a5 ab5 g5 | ")

# the solo: Em, Em, C, D, Em, B; tremolo bursts and wide leaps
SOLO = ("b6:3 b6 b6 b6 g6:6 e6 d6 b5 a5:12 | "
        "e5:6 b5 e6 b5 a6:12 g6:6 b5 | "
        "g6:3 g6 g6 g6 e6:6 c6 g5 e6 c6:12 | "
        "a6:3 a6 a6 a6 f#6:6 d6 a5 d6 f#6:12 | "
        "e6:6 g6 a6 b6 d6 e6 g5 a5 | "
        "b5:3 b5 b5 b5 d#6:6 f#6 a6 f#6 d#6 b5 | ")
BASS_SOLO = line(eighths, "e3 e3 c3 d3 e3 b2")

# a held E, and stabs
END = "e5:48 | -:24 r:24 | e5:6 r:18 e5:6 r:18 | e5:6 r:42 | "

D_BREAK = "k:6 k r k s k k r | "         # half time, kicks with the chugs
D_HELD = "k:6 s:3 s s s s s s s s s s s s s | s:3 s s s s s s s k:24 | "
D_END = "k:6 r:18 k:6 r:18 | " + LAST

# the riff alone for two bars, then the band comes in
MELODY = (A1 + A2
          + RIFF
          + VERSE
          + CHORUS
          + BREAK
          + SOLO
          + RIFF + A1 + A2
          + END)

BASS = ("r:48 | r:48 | "
        + shift(RIFF, -24)
        + BASS_VERSE
        + BASS_CHORUS
        + shift(BREAK, -24)
        + BASS_SOLO
        + shift(RIFF + A1 + A2, -24)
        + "e3:48 | -:24 r:24 | e3:6 r:18 e3:6 r:18 | e3:6 r:42 | ")

DRUMS = ("r:48 | r:48 | "
         + PUNK * 3 + FILL
         + PUNK * 7 + FILL
         + DRIVE * 7 + FILL
         + D_BREAK * 5 + FILL2
         + PUNK * 5 + FILL
         + PUNK * 5 + FILL
         + D_HELD + D_END)
