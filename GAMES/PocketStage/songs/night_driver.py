# Night Driver: our own, an organ hard rock track written for the game. G
# minor, 168 to the beat. The hook is the organ riff: a G struck on every
# beat with a line moving above it between the beats (D, E flat, D, C,
# then B flat, C, a long D), over a bass driving eighths and a steady fast
# beat. The verse moves the same figure over C minor, E flat and D;
# a chorus that soars on long high notes over E flat, F and G minor; a
# solo of arpeggios in G harmonic minor (triplets, and runs of sixteenths
# no longer than eight); the chorus and the riff to close. About
# fifty-four seconds.
#
# The tune never goes below F#5 and the bass never above G3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "NIGHT DRIVER"
BY = "DEEP PIXEL"
STYLE = "Organ hard rock"
KEY = "G minor"
PREVIEW = 11                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 168)]

# the riff: G on the beats, the line between them
RA = "g5:6 d6 g5 eb6 g5 d6 g5 c6 | "
RB = "g5:6 bb5 g5 c6 d6:12 bb5:6 a5 | "
RC = "g5:6 bb5 g5 c6 d6:6 f6 g6:12 | "        # up to G: a turn

# the same hands over C minor, C minor, E flat and D
VERSE2 = ("g5:6 eb6 g5 d6 g5 c6 g5 eb6 | "
          "g5:6 c6 g5 d6 eb6:12 c6:6 bb5 | "
          "g5:6 eb6 g5 f6 g5 eb6 g5 bb5 | "
          "a5:6 d6 a5 eb6 f#6:12 d6:6 f#6 | ")

# E flat, F, G minor, G minor, E flat, F, D, D
CHORUS = ("g6:12 bb6:36 | "
          "a6:12 c7:24 bb6:12 | "
          "g6:48 | "
          "r:12 d6:6 f6 g6:12 a6 | "
          "bb6:12 g6:36 | "
          "a6:12 c7:24 a6:12 | "
          "f#6:24 a6 | "
          "c7:12 bb6 a6 f#6 | ")

# G minor, C minor, D, G minor, E flat, C minor, D, D: arpeggios, and
# in the fourth bar the riff's trick up high (a D struck between the notes
# of a line)
SOLO = ("g5:4 bb5 d6 g6 d6 bb5 g5:6 d6 g6:12 | "
        "c6:4 eb6 g6 c7 g6 eb6 c6:12 g5 | "
        "a5:3 d6 f#6 a6 f#6 d6 a5 f#5 g5:6 a5 bb5 c6 | "
        "g6:6 d6 a6 d6 bb6 d6 a6 d6 | "
        "g5:4 bb5 eb6 g6 eb6 bb5 g5:6 eb6 g6:12 | "
        "c6:4 eb6 g6 c7 g6 eb6 c6:12 eb6 | "
        "f#6:3 g6 a6 bb6 a6 g6 f#6 eb6 d6:6 f#6 a6 c7 | "
        "bb6:6 a6 f#6 d6 a6:24 | ")

# the chorus's first two bars and its last two, then the riff to close
CHORUS2 = ("g6:12 bb6:36 | "
           "a6:12 c7:24 bb6:12 | "
           "f#6:24 a6 | "
           "c7:12 bb6 a6 f#6 | ")
END = "g6:36 r:12 | "

# the bass drives eighths on the roots, stepping into the next chord
BG = eighths("g3")
BG_DN = "g3:6 g3 g3 g3 g3 g3 f3 d3 | "         # G, walking down: F, D
BC = eighths("c3")
BC_EB = "c3:6 c3 c3 c3 c3 c3 c3 d3 | "         # up into E flat
BEB = eighths("eb3")
BEB_D = "eb3:6 eb3 eb3 eb3 eb3 eb3 eb3 c#3 | "
BF = eighths("f3")
BD = eighths("d3")
BD_G = "d3:6 d3 d3 d3 d3 d3 e3 f#3 | "         # up into G
BD_EB = "d3:6 d3 d3 d3 d3 d3 d3 d3 | "
BASS_VERSE = BG * 3 + BG_DN + BC + BC_EB + BEB_D + BD_EB
BASS_CHORUS = BEB + BF + BG + BG + BEB + BF + BD + BD_G
BASS_SOLO = BG_DN + BC + BD + BG + BEB + BC + BD + BD_EB
BASS_CHORUS2 = BEB + BF + BD + BD_G

D_FILL = "k:6 r s r k s:3 s s s s s | "
D_FILL2 = "k:6 s:3 s s:6 s:3 s s:3 s s s s s s s | "

# the riff alone for two bars, then the band comes in
MELODY = (RA + RB
          + RA + RB + RA + RB + VERSE2       # verse
          + CHORUS
          + RA + RB + RA + RC                # the riff
          + SOLO
          + CHORUS2
          + RA + RB + RA + END)

BASS = (rest(2)
        + BASS_VERSE
        + BASS_CHORUS
        + BG * 3 + BG_DN
        + BASS_SOLO
        + BASS_CHORUS2
        + BG * 3 + "g3:36 r:12 | ")

DRUMS = (rest(2)
         + beat(ROCK, 8, D_FILL)
         + beat(BACKBEAT, 8, D_FILL)
         + beat(ROCK, 4, D_FILL)
         + beat(DRIVE, 8, D_FILL2)
         + beat(BACKBEAT, 4, D_FILL)
         + ROCK * 3 + LAST)
