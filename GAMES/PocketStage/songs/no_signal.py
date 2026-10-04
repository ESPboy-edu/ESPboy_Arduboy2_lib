# No Signal: our own, a funk metal track written for the game. Drop D, 92 to
# the beat. The hook is the riff: short stabs on sixteenths with half of them
# left silent (the space is the groove), the low D three times and a G, then
# a glitch an octave up, D and C falling to A; the second time the G turns
# into the tritone A-flat and the glitch is F, E-flat and D up high. The bass
# plays every stab with it and the kick hits with them. A tense, quiet verse
# of scattered stabs over a few low chugs, a chromatic climb, then the chorus
# explodes: a run up into a long scream on D, C and B-flat, and stabs on A. A
# noise solo: whammy leaps of an octave, a note stuttered in sixteenths, a
# dive in octaves. The riff again and a last D. About fifty-five seconds.
#
# The tune never goes below D5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "NO SIGNAL"
BY = "RAGE QUIT"
STYLE = "Funk metal"
KEY = "D (drop D)"
BEATS = 4

TEMPO = [(0, 92)]

# the riff, two bars of sixteenths: a stab or a rest in every one
R1 = "d5:3 r d5 d5 r r g5 r d5 r d6 c6 r a5 r r | "
R2 = "d5:3 r d5 d5 r r ab5 r d5 r f6 eb6 r d6 r c6 | "
RIFF = R1 + R2

# scattered stabs, half steps and a tritone, then a climb into the chorus
V1 = "r:3 d6 eb6 r r d6 r r ab5 a5 r d6 r c6 r r | "
V2 = "r:3 d6 eb6 r r d6 r r f6 eb6 r d6 r c6 a5 r | "
CLIMB = "a5:3 r d6 r a5 r eb6 r a5 r e6 r a5 f6 f#6 g6 | "

# a run up into a scream on every chord (D, C, B-flat), then stabs on A
CHORUS = ("d6:3 f6 a6:30 g6:6 f6 | "
          "e6:3 g6 c7:30 bb6:6 a6 | "
          "d6:3 f6 bb6:30 a6:6 g6 | "
          "c#6:3 e6 a6:12 r:6 a6:3 a6 a6:6 r a6 | ")

# whammy leaps, a stutter that climbs, a dive in octaves, the stutter again
SOLO = ("a5:3 a6 a5 a6 a5 a6 r:6 c7:3 a6 r:6 a5:3 a6 g6 a6 | "
        "d6:3 d6 d6 d6 f6 f6 f6 f6 g6:6 r a6:12 | "
        "c7:3 c6 c7 c6 bb6 bb5 bb6 bb5 a6 a5 a6 a5 g6:6 f6 | "
        "d6:3 d6 d6 d6 r:6 d5:3 d6 d5 d6 d5 d6 r:6 a5 | ")

END = "d5:3 r d5 d5 r r g5 r d6:24 | "

# the bass on every stab of the riff, two octaves down
B1 = "d3:3 r d3 d3 r r g3 r d3 r d4 c4 r a3 r r | "
B2 = "d3:3 r d3 d3 r r ab3 r d3 r f3 eb3 r d4 r c4 | "
BASS_RIFF = B1 + B2
B_VERSE = "d3:3 r d3 d3 r r r r d3 r r r r r r r | "
B_CLIMB = "d3:6 d3 eb3 eb3 e3 e3 f3 f#3 | "
BASS_CHORUS = line(eighths, "d3 c3 bb2 a2")
B_END = "d3:3 r d3 d3 r r g3 r d3:24 | "

D_RIFF = "k:3 r k k s r k r k r r r s r r r | "     # the kick on the low stabs
D_FILL = "k:3 r k k s r k r s s s s s s s s | "
D_TENSE = "k:3 r k k r r r r s r r r r r r r | "    # half time, almost nothing
D_CLIMB = "k:6 s k s s:3 s s s s s s s | "
D_BLAST = "k:6 k s k k k s k | "                    # the chorus, a kick on every eighth
D_END = "k:3 r k k r r s r s:24 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF + RIFF
          + V1 + V2 + V1 + CLIMB
          + CHORUS
          + SOLO
          + RIFF
          + END)

BASS = (rest(2)
        + BASS_RIFF * 2
        + B_VERSE * 3 + B_CLIMB
        + BASS_CHORUS
        + BASS_RIFF * 2
        + BASS_RIFF
        + B_END)

DRUMS = (rest(2)
         + beat(D_RIFF, 4, D_FILL)
         + D_TENSE * 3 + D_CLIMB
         + beat(D_BLAST, 4, FILL)
         + beat(D_RIFF, 4, D_FILL)
         + D_RIFF + D_FILL
         + D_END)
