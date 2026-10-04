# Fatal Error: our own, a hard rock track written for the game. A minor,
# 140 to the beat. The hook is a ringing arpeggio riff in eighths, picked
# in groups of three, three and two: root, fifth and octave, then third,
# fifth and octave, then fifth and third, so it walks the three lanes left
# to right and back in every bar, over A minor, G, F and E (the top note
# stepping down each bar). The riff alone for two bars, the bass in on the
# F, then the band; the riff over D minor, F, G and E as it builds, a
# chorus of long notes (FA-TAL ER-ROR), a melodic solo that climbs to the
# high C, and the riff to end on a held A. About fifty-five seconds.
#
# The tune never goes below E5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

from kit import *

TITLE = "FATAL ERROR"
BY = "BLUE SCREEN CULT"
STYLE = "Hard rock"
KEY = "A minor"
BEATS = 4

TEMPO = [(0, 140)]


def ring(root, third, fifth, octave):
    """a bar of the picking pattern: root fifth octave, third fifth octave,
    fifth third"""
    return "%s:6 %s %s %s %s %s %s %s | " % (root, fifth, octave,
                                             third, fifth, octave, fifth, third)


RIFF = (ring("a5", "c6", "e6", "a6")
        + ring("g5", "b5", "d6", "g6")
        + ring("f5", "a5", "c6", "f6")
        + ring("e5", "g#5", "b5", "e6"))
# building: D minor (from its A), F, G and E
RIFF_B = (ring("a5", "d6", "f6", "a6")
          + ring("f5", "a5", "c6", "f6")
          + ring("g5", "b5", "d6", "g6")
          + ring("e5", "g#5", "b5", "e6"))

# fa-TAL er-ROR: the second word falls
CHORUS = ("a6:12 c7:36 | "
          "b6:12 d6:36 | "
          "e6:12 g6:12 c7:24 | "
          "a6:24 e6:24 | "
          "a6:12 c7:36 | "
          "b6:12 d6:24 g6:12 | "
          "b6:24 g#6:24 | "
          "e6:36 r:12 | ")

SOLO = ("e6:6 a6:6 b6:6 c7:6 b6:12 a6:6 e6:6 | "
        "d6:6 g6:6 a6:6 b6:6 a6:12 g6:6 d6:6 | "
        "c6:6 f6:6 a6:6 c7:6 a6:12 f6:6 a6:6 | "
        "g#6:12 e6:6 b6:6 g#6:6 d6:6 e6:12 | "
        "e6:3 a6 c7 a6 e6:6 a6 c7:12 b6:6 a6:6 | "
        "d6:3 g6 b6 g6 d6:6 g6 b6:12 a6:6 g6:6 | "
        "c6:3 f6 a6 f6 c6:6 f6 a6:12 c7:6 a6:6 | "
        "b5:3 e6 g#6 b6 g#6:6 e6 b6:24 | ")

END = (ring("a5", "c6", "e6", "a6")
       + ring("g5", "b5", "d6", "g6")
       + ring("f5", "a5", "c6", "f6")
       + "e5:6 b5:6 e6:6 g#6:6 a6:24 | ")

# the bass: roots under the riff, in on the F; eighths with the band
BASS_INTRO = "r:48 | r:48 | f3:48 | e3:48 | "
BASS_RIFF = line(eighths, "a3 g3 f3 e3")
BASS_B = line(eighths, "d3 f3 g3 e3")
BASS_CHORUS = line(fifths, "f3 g3 c3 a2 f3 g3") + line(eighths, "e3 e3")
BASS_SOLO = line(fifths, "a2 g3 f3 e3 a2 g3 f3 e3")
BASS_END = line(eighths, "a3 g3 f3") + "e3:6 e3 e3 e3 a2:24 | "

D_BEAT = "k:6 r s r k k s r | "
D_FILL = "k:6 r s r k r s:3 s s s | "
D_BIG = "k:12 s k:6 k s:12 | "
D_DRIVE = "k:6 k s r k k s k | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF
          + RIFF_B
          + CHORUS
          + SOLO
          + END)

BASS = (BASS_INTRO
        + BASS_RIFF
        + BASS_B
        + BASS_CHORUS
        + BASS_SOLO
        + BASS_END)

DRUMS = ("r:48 | r:48 | r:48 | " + FILL2
         + beat(D_BEAT, 4, D_FILL)
         + beat(D_BEAT, 4, D_FILL)
         + beat(D_BIG, 8, D_FILL)
         + beat(D_DRIVE, 8, D_FILL)
         + D_BEAT * 3 + LAST)
