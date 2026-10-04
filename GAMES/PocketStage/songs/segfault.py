# Segfault: our own, a nu metal track written for the game. C-sharp, 100 to
# the beat, bouncy. The hook is the riff: low C# struck in pairs of
# sixteenths with rests between, a jump to the octave, D, the tritone G and
# F#; then the pairs again and one eerie high G, slid into like a squeal and
# held over the low C#. The bass plays the riff with the tune. A verse rapped
# on the notes of C#m while the bass keeps the riff going, a chorus of long
# notes over C#m, D, C#m and G, the riff, a half-time breakdown of chugs
# answered up high, the chorus again and the squeal to finish. About
# fifty-eight seconds.
#
# The tune never goes below C#5 and the bass never above C#4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "SEGFAULT"
BY = "KERNEL"
STYLE = "Nu metal"
KEY = "C-sharp minor"
BEATS = 4

TEMPO = [(0, 100)]

# the riff: pairs of low C#, the octave, D, G, F#; then the eerie G
R1 = "c#5:3 c#5 r r c#6 r c#5 c#5 r r d5 r g5 r f#5 r | "
R2 = "c#5:3 c#5 r r c#6 r c#5 c#5 f#6:3 g6:21 | "
RIFF = R1 + R2

# rapped on the notes of C#m over the bass's riff, the last bar climbing out
V1 = "r:3 e5 e5 g#5 e5 r c#5 e5 r g#5 g#5 r b5 g#5 e5 c#5 | "
V2 = "r:3 e5 e5 g#5 e5 r c#5 e5 r a5 a5 r c#6 a5 e5 d5 | "
V4 = "r:3 e5 e5 g#5 e5 r c#5 e5 g#5:6 b5 c#6 d#6 | "
VERSE = V1 + V2 + V1 + V4

# over C#m, D, C#m and G: a long note on every downbeat, then the way on
CHORUS = ("e6:24 d#6:6 c#6 g#5 c#6 | "
          "a6:24 g#6:6 f#6 d6 f#6 | "
          "e6:24 f#6:6 e6 c#6 b5 | "
          "g6:36 d6:6 b5 | ")
CHORUS2 = ("e6:24 d#6:6 c#6 g#5 c#6 | "
           "a6:24 g#6:6 a6 b6 a6 | "
           "g#6:24 e6:6 c#6 b5 c#6 | "
           "g6:24 f#6:6 g6 d6 b5 | ")

# half time: chugs in threes answered up high by the eerie G and the octave,
# then by a high C; then pairs of chugs between the notes of C# diminished
BREAK = ("c#5:3 c#5 c#5 r g6:6 r c#5:3 c#5 c#5 r c#6:6 r | "
         "c#5:3 c#5 c#5 r g6:6 r c#5:3 c#5 c#5 r c7:12 | "
         "c#5:3 c#5 c#5 r g6:6 r c#5:3 c#5 c#5 r c#6:6 r | "
         "c#5:3 c#5 g5 g5 c#5 c#5 c#6 c#6 c#5 c#5 e6 e6 c#5 c#5 g6 g6 | ")

END = "c#5:3 c#5 r r c#6 r c#5 c#5 f#6:3 g6:21 | "

# the bass plays the riff two octaves down
B1 = "c#3:3 c#3 r r c#4 r c#3 c#3 r r d3 r g3 r f#3 r | "
B2 = "c#3:3 c#3 r r c#4 r c#3 c#3 c#3:24 | "
BASS_RIFF = B1 + B2
BASS_CHORUS = line(eighths, "c#3 d3 c#3 g3")
BASS_BREAK = ("c#3:3 c#3 c#3 r g3:6 r c#3:3 c#3 c#3 r c#4:6 r | "
              "c#3:3 c#3 c#3 r g3:6 r c#3:3 c#3 c#3 r c#3:12 | "
              "c#3:3 c#3 c#3 r g3:6 r c#3:3 c#3 c#3 r c#4:6 r | "
              "c#3:3 c#3 g3 g3 c#3 c#3 c#4 c#4 c#3 c#3 e3 e3 c#3 c#3 g3 g3 | ")

D_BOUNCE = "k:3 k r r s r k k r r k r s r k r | "     # the kick with the pairs
D_HELD = "k:3 k r r s r k k r:12 s:12 | "             # under the eerie note
D_BIG = "k:6 k s r k k s k | "
D_HALF = "k:3 k k r k:6 r s:3 k k r k:6 r | "         # the breakdown: the snare on 3
D_CHUG = "k:3 k k k s s s s k k k k s s s s | "
D_END = "k:3 k r r s r k k s:24 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF
          + VERSE
          + CHORUS
          + RIFF
          + BREAK
          + CHORUS2
          + R1
          + END)

BASS = (rest(2)
        + BASS_RIFF
        + BASS_RIFF * 2
        + BASS_CHORUS
        + BASS_RIFF
        + BASS_BREAK
        + BASS_CHORUS
        + B1
        + "c#3:3 c#3 r r c#4 r c#3 c#3 c#3:24 | ")

DRUMS = (rest(2)
         + D_BOUNCE + D_HELD
         + (D_BOUNCE + D_HELD) * 2
         + beat(D_BIG, 4, FILL)
         + D_BOUNCE + D_HELD
         + D_HALF * 3 + D_CHUG
         + beat(D_BIG, 4, FILL)
         + D_BOUNCE
         + D_END)
