# Hot Wired: our own, a hard rock track written for the game. A major, 128
# to the beat, straight eighths. The hook is the riff: a bar that bounces
# between the root and the chord tones of A with rests between the hits,
# then the same bounce on D landing on E; the bass pounds the roots in
# eighths under a plain backbeat. A chorus chanted on long notes ("hot ...
# wired") that climbs a step on every chord, a short solo up in octave 6
# with repeated-note licks, the riff again and stabs to end. About
# fifty-six seconds.
#
# The tune never goes below E5 and the bass never above A3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "HOT WIRED"
BY = "DC/DC"
STYLE = "Hard rock"
KEY = "A major"
BEATS = 4

TEMPO = [(0, 128)]

# the riff: the bounce on A, then on D landing on E
RA = "a5:6 r e5 a5 r c#6 a5:12 | "
RB = "d6:6 r a5 d6 r f#6 e6:12 | "
RE = "e6:6 r b5 e6 r g#6 b6:12 | "          # the bounce on E
RT = "e6:6 r e6 r e6 d6 c#6 b5 | "           # down into the chorus

# the chant climbs a step on every chord: A, D, E, then home
CHORUS = ("e6:12 r:6 c#6:18 r:6 a5:6 | "
          "f#6:12 r:6 d6:18 r:6 a5:6 | "
          "g#6:12 r:6 e6:18 r:6 b5:6 | "
          "e6:6 r c#6 e6 a6:24 | "
          "e6:12 r:6 c#6:18 r:6 a5:6 | "
          "f#6:12 r:6 d6:18 r:6 a5:6 | "
          "g#6:12 r:6 e6:6 g#6 b6:18 | "
          "a6:36 r:12 | ")

# over the riff's chords: repeated notes in triplets, bends as grace notes
SOLO = ("a6:4 a6 a6 e6:12 a6:4 a6 a6 e6:6 c#6 | "
        "d6:6 f#6 a6:12 g6:3 a6:9 f#6:6 e6 | "
        "e6:4 e6 e6 c#6:12 e6:4 e6 e6 a6:12 | "
        "b6:6 a6 f#6 e6 d6 e6 g#6:12 | "
        "c6:3 c#6:9 e6:6 a6 c6:3 c#6:9 e6:6 a6 | "
        "b6:6 b6 a6 a6 g#6 g#6 e6:12 | ")

STABS = ("e6:6 r:18 e6:6 r:18 | "
         "a6:6 r a6 r a6:24 | ")

# the bass: the roots in eighths
B_A = eighths("a3")
B_DE = "d3:6 d3 d3 d3 d3 d3 e3 e3 | "
B_E = eighths("e3")
B_UP = "e3:6 e3 e3 e3 e3 e3 f#3 g#3 | "      # walking up into the chorus
BASS_CHORUS = (line(eighths, "a3 d3 e3 a3 a3 d3 e3") + "a3:6 a3 a3 a3 a3 a3 g#3 e3 | ")
BASS_SOLO = B_A + B_DE + B_A + B_DE + B_A + B_E
BASS_STABS = "e3:6 r:18 e3:6 r:18 | a2:6 r a2 r a2:24 | "

D_FILL = "k:12 s k:6 k s:3 s s s | "
D_FILL_CH = "k:6 k s r k s:3 s s:6 s:3 s | "
D_STABS = "k:6 r:18 k:6 r:18 | k:6 r k r s:24 | "

# the riff alone for two bars, then the band comes in
MELODY = (RA + RB
          + RA + RB + RA + RE + RA + RB + RA + RT      # verse
          + CHORUS
          + SOLO
          + RA + RB + RA + RE                          # the riff again
          + STABS)

BASS = (rest(2)
        + B_A + B_DE + B_A + B_E + B_A + B_DE + B_A + B_UP
        + BASS_CHORUS
        + BASS_SOLO
        + B_A + B_DE + B_A + B_E
        + BASS_STABS)

DRUMS = (rest(2)
         + beat(BACKBEAT, 8, D_FILL)
         + beat(DRIVE, 8, D_FILL_CH)
         + beat(BACKBEAT, 6, D_FILL, every=6)
         + beat(BACKBEAT, 4, D_FILL)
         + D_STABS)
