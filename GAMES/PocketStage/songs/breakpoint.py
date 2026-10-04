# Breakpoint: our own, a metalcore track written for the game. C sharp minor,
# 148 to the beat. The hook is the verse riff: every bar starts with two
# beats of syncopated chugs on the low C sharp (two quick ones, a gap, one,
# a late one), the bass and the kick with them, and answers up high: a
# note, a flick down and back, and down the chord, over C sharp minor, A, E
# and B, the top notes G sharp, A, B, A (the last bar chugs on the D a step
# above, the dark note of the key). The riff alone for two bars, then the
# band; a melodic chorus of long notes, a breakdown at half time (chugs
# with gaps, the tritone G and the octave stabbed in), the riff again, and
# a melodic outro that comes to rest on a held C sharp. About fifty-five
# seconds.
#
# The tune never goes below C sharp 5 and the bass never above C sharp 4,
# so the two cannot meet on one pitch (tools/build_data.py says why they
# must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

from kit import *

TITLE = "BREAKPOINT"
BY = "KILLSWITCH"
STYLE = "Metalcore"
KEY = "C sharp minor"
PREVIEW = 13                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 148)]

# the chugs: two quick ones, a gap, one, a late one (two beats)
CHUG = "c#5:3 c#5 r:6 c#5:6 r:3 c#5:3 "
CHUG_D = "c#5:3 c#5 r:6 d5:6 r:3 d5:3 "

# the answers: a high note, down and back up in a flick, then down the chord;
# the top notes G sharp, A, B, A
RA = CHUG + "g#6:6 c#6:3 g#6:3 e6:6 c#6:6 | "
RB = CHUG + "a6:6 c#6:3 a6:3 e6:6 c#6:6 | "
RC = CHUG + "b6:6 e6:3 b6:3 g#6:6 e6:6 | "
RD = CHUG_D + "a6:6 b5:3 a6:3 f#6:6 b5:6 | "
RIFF = RA + RB + RC + RD

# over A, E, B and C sharp minor, twice
CHORUS = ("c#6:6 e6:6 a6:36 | "
          "g#6:12 b6:12 g#6:24 | "
          "f#6:12 d#6:12 f#6:12 b6:12 | "
          "g#6:36 e6:6 f#6:6 | "
          "e6:6 a6:6 b6:36 | "
          "g#6:12 e6:12 b6:24 | "
          "a6:12 g#6:12 f#6:12 d#6:12 | "
          "e6:24 c#6:24 | ")

# half time: chugs with gaps; the tritone (G) and the octave stabbed in
BREAK = ("c#5:6 r:12 c#5:6 r:6 c#5:6 r:6 g5:6 | "
         "c#5:6 r:12 c#5:6 r:6 c#5:6 c#6:12 | "
         "c#5:6 r:12 c#5:6 r:6 c#5:3 c#5:3 r:6 g5:6 | "
         "c#5:3 c#5:3 r:6 c#5:3 c#5:3 r:6 d5:6 g5:6 c#6:12 | ")

OUTRO = ("e6:6 c#6 e6 a6 g#6:12 e6:12 | "
         "b5:6 e6 g#6 b6 a6:12 g#6:12 | "
         "f#6:6 d#6 f#6 b6 a6:12 f#6:12 | "
         "g#6:6 e6 c#6 e6 g#6:24 | "
         "a6:24 g#6:12 e6:12 | "
         "c#6:48 | ")

# the bass chugs with the tune, then holds the chord in eighths
BCHUG = "c#3:3 c#3 r:6 c#3:6 r:3 c#3:3 "
BA = BCHUG + "c#3:6 c#3 c#3 c#3 | "
BB = BCHUG + "a2:6 a2 a2 a2 | "
BC = BCHUG + "e3:6 e3 e3 e3 | "
BD = "c#3:3 c#3 r:6 d3:6 r:3 d3:3 b2:6 b2 b2 b2 | "
BASS_RIFF = BA + BB + BC + BD
BASS_CHORUS = line(eighths, "a2 e3 b2 c#3 a2 e3 b2 c#3")
BASS_BREAK = ("c#3:6 r:12 c#3:6 r:6 c#3:6 r:6 g3:6 | "
              "c#3:6 r:12 c#3:6 r:6 c#3:6 c#4:12 | "
              "c#3:6 r:12 c#3:6 r:6 c#3:3 c#3:3 r:6 g3:6 | "
              "c#3:3 c#3:3 r:6 c#3:3 c#3:3 r:6 d3:6 g3:6 c#4:12 | ")
BASS_OUTRO = line(eighths, "a2 e3 b2 c#3") + line(halves, "a2") + "c#3:48 | "

# the kick with the chugs, the snare on 2 and 4
D_RIFF = "k:3 k r:6 s:6 r:3 k:3 k:6 k s k | "
D_FILL = "k:3 k r:6 s:6 r:3 k:3 s:3 s s s s:6 s | "
D_DRIVE = "k:6 k s r k k s r | "
D_CFILL = "k:6 k s r k r s:3 s s s | "
D_HALF = "k:18 k:6 s:6 k:12 k:6 | "             # half time: the snare on 3
D_HALF2 = "k:18 k:6 s:6 k:6 k:12 | "
D_HALF3 = "k:18 k:6 s:6 k:3 k:3 r:6 k:6 | "
D_BUILD = "k:3 k r:6 k:3 k r:6 s:3 s s s s:6 s | "
D_OUT = "k:12 s k s | "

# the riff alone for two bars, then the band comes in
MELODY = (RA + RB
          + RC + RD
          + RIFF + RIFF
          + CHORUS
          + BREAK
          + RIFF
          + OUTRO)

BASS = ("r:48 | r:48 | " + BC + BD
        + BASS_RIFF * 2
        + BASS_CHORUS
        + BASS_BREAK
        + BASS_RIFF
        + BASS_OUTRO)

DRUMS = ("r:48 | r:48 | "
         + D_RIFF + D_FILL
         + beat(D_RIFF, 8, D_FILL)
         + beat(D_DRIVE, 8, D_CFILL)
         + D_HALF + D_HALF2 + D_HALF3 + D_BUILD
         + D_RIFF * 3 + D_FILL
         + D_OUT * 5 + LAST)
