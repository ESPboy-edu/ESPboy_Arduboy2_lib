# Street Dog: our own, a classic rock track written for the game. G major
# with an open-G feel, 124 to the beat. The hook is the riff: it comes in
# just after the beat and hops from B up to C and back (G to a suspended C
# over G) while the bass holds G in eighths, walking into every change.
# A chorus over C and G that climbs on its last line to a long high B, a
# solo like a slide guitar (every long note slid into from below), the riff
# again and a slide up to a long G to end. About fifty-four seconds.
#
# The tune never goes below G5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "STREET DOG"
BY = "ROLLING TONES"
STYLE = "Classic rock"
KEY = "G major"
PREVIEW = 9                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 124)]

# the riff: in after the beat, the hop from B to C and up to D, four endings
R1 = "r:6 d6 b5 c6 r d6 g5:12 | "
R2 = "r:6 d6 b5 c6 e6 d6 b5:12 | "
R2B = "r:6 d6 b5 c6 e6 g6 e6:12 | "
R3 = "r:6 d6 b5 c6 d6 f6 g6:12 | "          # up to G, into the chorus on C

# C, G, C, G, C, D, then a long high B over G
CHORUS = ("e6:18 d6:6 c6:18 g5:6 | "
          "b5:18 c6:6 d6:24 | "
          "e6:18 d6:6 c6:18 e6:6 | "
          "g6:18 f6:6 d6:24 | "
          "e6:18 d6:6 c6:18 e6:6 | "
          "a5:12 d6 f#6 a6 | "
          "b6:48 | "
          "-:18 a6:6 g6 e6 d6 b5 | ")

# the slide: every long note reached from a step below
SOLO = ("f#6:3 g6:21 r:6 d6:6 f6:3 g6:9 | "
        "bb6:3 b6:9 a6:6 g6 e6 d6 g5:12 | "
        "d#6:3 e6:21 r:6 c6:6 d#6:3 e6:9 | "
        "f6:3 g6:9 e6:6 c6 a5 bb5:3 c6:9 a5:6 | "
        "c#6:3 d6:9 f#6:6 d6 g#6:3 a6:21 | "
        "a6:6 f#6 d6 a5 f6:3 g6:21 | ")

END = "f#6:3 g6:33 r:12 | "

# the bass: G in eighths, walking up (or down) into the next chord
BG = eighths("g3")
BG_UP = "g3:6 g3 g3 g3 g3 g3 e3 f#3 | "       # into G again
BG_C = "g3:6 g3 g3 g3 g3 g3 a3 b3 | "         # into C
BC_G = "c4:6 c4 c4 c4 c4 c4 c4 a3 | "         # C, stepping down into G
BC_D = "c4:6 c4 c4 c4 c4 c4 c4 c#4 | "        # C, up into D
BD_G = "d4:6 d4 d4 d4 d4 d4 a3 f#3 | "        # D, down into G
BD_G2 = "d4:6 d4 d4 d4 d4 d4 b3 a3 | "       # the same under the solo's last G
BASS_RIFF = BG + BG_UP
BASS_CHORUS = BC_G + BG_C + BC_G + BG_C + BC_D + BD_G + BG + BG_UP
BASS_SOLO = BG + BG_C + eighths("c4") + BC_D + eighths("d4") + BD_G2

D_INTRO = "k:12 s k s | "
D_FILL = "k:6 r s r k s:3 s s:6 s:3 s | "
D_CHORUS = "k:12 s k:6 k s:12 | "

# the riff with the drums for two bars, then the bass comes in
MELODY = (R1 + R2
          + R1 + R2 + R1 + R2B + R1 + R3                # verse
          + CHORUS
          + SOLO
          + R1 + R2 + R1 + R2B + R1                     # the riff again
          + END)

BASS = (rest(2)
        + BASS_RIFF + BG + BG_UP + BG + BG_C
        + BASS_CHORUS
        + BASS_SOLO
        + BASS_RIFF + BG + BG_UP + BG
        + "g3:36 r:12 | ")

DRUMS = (D_INTRO * 2
         + ROCK * 5 + D_FILL
         + beat(D_CHORUS, 8, D_FILL)
         + ROCK * 5 + D_FILL
         + ROCK * 4 + D_FILL
         + LAST)
