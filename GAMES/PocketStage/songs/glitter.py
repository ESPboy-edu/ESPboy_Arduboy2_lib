# Glitter: our own, a glam rock song written for the game. A major, 128 to
# the beat, on the stomp: kick kick snare, kick kick snare, the snares like
# handclaps. The hook is the riff: two notes on the kicks, a gap where the
# clap goes, then a strut up to the flat seventh and back (and down over D
# to A), with the bass playing a boogie line under it. The verse sings in
# short-short-long phrases over the boogie; the chorus is a chant, GLIT-TER
# in two long notes, answered each time by a bar of the riff. A break of
# handclaps and the riff alone, then the chorus once more to end on a long
# A. Simple and catchy. About fifty-six seconds.
#
# The tune never goes below G5 and the bass never above D4, so the two
# cannot meet on one pitch.

from kit import *

TITLE = "GLITTER"
BY = "SWEET 16"
STYLE = "Glam rock"
KEY = "A major"
BEATS = 4
TEMPO = [(0, 128)]

# the riff: on the kicks, a gap for the clap, a strut (A; D to A)
R1 = "a5:6 a5 r:6 e6:6 g6:6 a6 r:6 e6:6 | "
R2 = "f#6:6 f#6 r:6 d6:6 e6:6 c#6 a5:12 | "
R1_G = "g5:6 g5 r:6 d6:6 f6:6 g6 r:6 d6:6 | "       # the same over G
RIFF = R1 + R2

# the verse (A A D D A A E E)
VERSE = ("a5:6 c#6 e6:12 e6:6 f#6 e6:12 | "
         "c#6:6 e6 a6:12 g6:6 e6 c#6:12 | "
         "d6:6 f#6 a6:12 a6:6 b6 a6:12 | "
         "f#6:6 a6 c7:24 a6:12 | "
         "a5:6 c#6 e6:12 e6:6 f#6 e6:12 | "
         "c#6:6 e6 a6:12 g6:6 e6 c#6:12 | "
         "b5:6 e6 g#6:12 e6:6 f#6 g#6:12 | "
         "b6:6 a6 g#6 e6 d6:6 e6 g#6:12 | ")

# the chorus: GLIT-TER, and the riff answers (A A D D-A A G D-E A)
CHANT = ("a6:24 e6:24 | " + R1
         + "a6:24 f#6:24 | " + R2
         + "a6:24 e6:24 | " + R1_G
         + "f#6:24 g#6:24 | ")
CHORUS = CHANT + "a6:24 e6:12 c#6:12 | "
CHORUS_END = CHANT + "a6:48 | "

# the bass: the boogie, root, third, fifth, sixth, seventh and back
B_RIFF = boogie("a2") + "d3:6 f#3 a3 b3 a2 c#3 e3 c#3 | "
B_VERSE = line(boogie, "a2 a2 d3 d3 a2 a2 e3 e3")
B_CHANT = (boogie("a2") + boogie("a2") + boogie("d3") + "d3:6 f#3 a3 b3 a2 c#3 e3 c#3 | "
           + boogie("a2") + "g3:6 g3 b3 g3 d4 d4 b3 g3 | "
           + "d3:6 f#3 a3 f#3 e3 g#3 b3 g#3 | ")
B_CHORUS = B_CHANT + boogie("a2")
B_END = B_CHANT + "a3:6 r:42 | "

D_FILL = "k:6 k s:12 k:6 k s:3 s s s | "
D_CLAPS = "k:6 k s:12 k:6 k s:6 s | "      # the stomp with a double clap
D_BREAK = "r:12 s r s | r:12 s r s:6 s | "  # handclaps alone
D_CHORUS = (STOMP + D_CLAPS) * 3 + STOMP + D_FILL

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF
          + VERSE
          + CHORUS
          + RIFF
          + CHORUS_END)

BASS = (rest(2) + B_RIFF
        + B_VERSE
        + B_CHORUS
        + rest(2)
        + B_END)

DRUMS = (rest(2) + STOMP + D_FILL
         + beat(STOMP, 8, D_FILL, 8)
         + D_CHORUS
         + D_BREAK
         + (STOMP + D_CLAPS) * 3 + STOMP + LAST)
