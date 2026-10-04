# Fix Me Later: our own, an AOR shuffle written for the game. C-sharp minor
# for the riff and the verse, E major for the chorus, 104 to the beat with
# a triplet feel. The hook is a piano-like riff in triplet eighths (4
# ticks) that bounces low, high, middle across the chord tones of C#m, B, A
# and B and stops on a held G#. The verse sings over C#m, F#m, A and G#7
# with a piano answer in triplets; the chorus is smooth and long over E B,
# G#m C#m, F#m B, E, each line a long note and a swung pickup into the
# next; a short solo walks down A, G#m, F#m, G#; the chorus again and the
# riff to end on a long C#. About fifty-five seconds.
#
# The tune never goes below G#5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "FIX ME LATER"
BY = "TODO"
STYLE = "AOR shuffle"
KEY = "C# minor / E major"
PREVIEW = 9                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 104)]

# the riff: low, high, middle on every beat, then a held G#
RIFF = ("c#6:4 g#6 e6 c#6 g#6 e6 b5 f#6 d#6 b5 f#6 d#6 | "
        "a5:4 e6 c#6 a5 e6 c#6 b5 f#6 d#6 g#6:12 | ")

# the verse: a sung line, and the piano answering in triplets
VERSE = ("e6:8 e6:4 d#6:8 c#6:4 g#5:4 c#6 e6 g#6:12 | "
         "c#6:8 c#6:4 b5:8 a5:4 a5:8 b5:4 c#6:12 | "
         "e6:8 e6:4 c#6:8 b5:4 a5:4 c#6 e6 a6:12 | "
         "c6:8 d#6:4 f#6:8 g#6:4 f#6:8 d#6:4 b5:8 e6:4 | ")

# the chorus: a long note, a swung step down and a pickup into the next line
CHORUS = ("g#6:24 f#6:8 e6:4 b5:8 e6:4 | "
          "b6:24 a6:8 g#6:4 e6:8 f#6:4 | "
          "a6:24 g#6:8 f#6:4 g#6:8 f#6:4 | "
          "e6:36 r:12 | ")

SOLO = ("c#6:8 e6:4 a6:12 f#6:8 e6:4 c#6:8 e6:4 | "
        "b6:12 a6:4 g#6 f#6 g#6:12 d#6:8 b5:4 | "
        "a6:8 f#6:4 c#6:8 f#6:4 a6:4 b6 a6 g#6 f#6 e6 | "
        "d#6:4 f#6 g#6 c7:24 b6:4 a6 f#6 | ")

END = ("c#6:4 g#6 e6 c#6 g#6 e6 b5 f#6 d#6 b5 f#6 d#6 | "
       "a5:4 e6 c#6 b5 f#6 d#6 c#6:24 | ")


def shuf(r, top):
    """two beats of the shuffle: the root long and short, then the top"""
    return "%s:8 %s:4 %s:8 %s:4 " % (r, r, top, r)


B_RIFF = (shuf("c#3", "c#4") + shuf("b2", "b3") + "| "
          + shuf("a2", "a3") + shuf("b2", "b3") + "| ")
B_VERSE = ((shuf("c#3", "c#4") * 2) + "| "
           + (shuf("f#3", "c#4") * 2) + "| "
           + (shuf("a2", "a3") * 2) + "| "
           + (shuf("g#3", "d#4") * 2) + "| ")
B_CHORUS = (shuf("e3", "e4") + shuf("b2", "b3") + "| "
            + shuf("g#3", "d#4") + shuf("c#3", "c#4") + "| "
            + shuf("f#3", "c#4") + shuf("b2", "b3") + "| "
            + (shuf("e3", "e4") * 2) + "| ")
B_SOLO = ((shuf("a2", "a3") * 2) + "| "
          + (shuf("g#3", "d#4") * 2) + "| "
          + (shuf("f#3", "c#4") * 2) + "| "
          + (shuf("g#3", "d#4") * 2) + "| ")
B_END = (shuf("c#3", "c#4") + shuf("b2", "b3") + "| "
         + "a2:8 a2:4 b2:8 b2:4 c#3:24 | ")

D_FILL = "k:8 r:4 s:8 k:4 s:4 s s s:4 s s | "
D_END = "k:8 r:4 s:8 k:4 k:12 r:12 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF
          + VERSE
          + CHORUS
          + RIFF
          + SOLO
          + CHORUS
          + END)

BASS = (rest(2)
        + B_RIFF
        + B_VERSE
        + B_CHORUS
        + B_RIFF
        + B_SOLO
        + B_CHORUS
        + B_END)

DRUMS = (rest(2)
         + SHUFFLE + D_FILL
         + SHUFFLE * 3 + TRIPLETS
         + SHUFFLE * 3 + D_FILL
         + SHUFFLE + D_FILL
         + SHUFFLE * 3 + TRIPLETS
         + SHUFFLE * 3 + D_FILL
         + SHUFFLE + D_END)
