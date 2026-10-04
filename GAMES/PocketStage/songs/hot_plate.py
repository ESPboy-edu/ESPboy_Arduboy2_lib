# Hot Plate: our own, a funk rock track written for the game. E, 104 to the
# beat, all sixteenths. The hook is the riff: short notes with rests where a
# funk guitar would scratch, up the E minor chord (E, G, B), down to D and
# E, then up the octave and down (E, D, B, A, B); the bass slaps E and its
# octave in the gaps. The chorus sings over A, G#m, F#m and B: a note
# twice, a step up and back, then a long one (E, E, F#, E, C#), lower each
# bar and then climbing home. A busy solo, the riff again and a last E.
# About fifty-five seconds.
#
# The tune never goes below D5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "HOT PLATE"
BY = "RED HOT CHIPPERS"
STYLE = "Funk rock"
KEY = "E"
PREVIEW = 7                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 104)]

# the riff, sixteenths with rests for the ghost notes, and its turn: G# up
# to A, the chorus's first chord
R1 = "e5:3 r g5 b5 r d5 e5 r g5 r e6 d6 r b5 a5 b5 | "
R2 = "e5:3 r g5 b5 r d5 e5 r g5 r g6 e6 r d6 b5 a5 | "
R3 = "e5:3 r g5 b5 r d5 e5 r g5 r e6 d6 r b5 g#5 a5 | "
RIFF = R1 + R2
# the second verse fills one gap with a run up into the octave
R1V = "e5:3 r g5 b5 r d5 e5 g5 a5 r e6 d6 r b5 a5 b5 | "
R2V = "e5:3 r g5 b5 r d5 e5 g5 a5 r g6 e6 r d6 b5 a5 | "

# A, G#m, F#m, B: twice the same note, a step up and back, a long one
CHORUS = ("e6:6 e6 f#6 e6 c#6:24 | "
          "b5:6 b5 c#6 b5 g#5:24 | "
          "a5:6 a5 b5 c#6 e6:24 | "
          "d#6:12 c#6:6 b5 f#6:24 | ")
CHORUS2 = ("e6:6 e6 f#6 e6 c#6:24 | "
           "b5:6 b5 c#6 d#6 g#6:24 | "
           "f#6:6 e6 c#6 a5 c#6:24 | "
           "b5:3 c#6 d#6:6 e6 f#6 a6:24 | ")

# busy: runs, a lick on a repeated E, a run up and a bend to a high B
SOLO = ("b5:3 d6 e6 d6 b5 d6 e6 g6 a6:6 g6:3 e6 d6:6 b5 | "
        "g6:3 a6 g6 e6 g6 a6 b6:6 a6:3 g6 e6 g6 e6:6 d6 | "
        "e6:3 e6 r g6 e6 e6 r a6 e6 e6 r b6 a6:3 g6 e6:6 | "
        "b5:3 d6 e6 g6 a6 b6 a6 g6 e6:3 g6 a6:6 a6:3 b6:9 | ")

END = "e5:3 r g5 b5 r d5 e5 r e6:24 | "

# the bass slaps E and its octave in the gaps of the riff
BR1 = "e3:3 r e4 r r e3 e4 r e3 r e4 r r e3 e4 d4 | "
BR2 = "e3:3 r e4 r r e3 e4 r e3 r e4 r r g3 a3 b3 | "
BR3 = "e3:3 r e4 r r e3 e4 r e3 r e4 r r e3 f#3 g#3 | "
BASS_RIFF = BR1 + BR2


def pop(lo, hi):
    """a bar of the chorus bass: a low note and a high one by turns, with a
    gap before the second and the fourth pair"""
    return "%s:6 %s r:3 %s %s:6 %s:6 %s r:3 %s %s:6 | " % (lo, hi, lo, hi, lo, hi, lo, hi)


BASS_CHORUS = pop("a2", "a3") + pop("g#3", "d#4") + pop("f#3", "c#4") + pop("b2", "b3")
B_END = "e3:3 r e4 r r e3 e4 r e3:24 | "

D_FUNK = "k:3 r r r s r r k r r k r s r r r | "     # snare on 2 and 4, the kick in between
D_FUNK_FILL = "k:3 r r r s r r k r r k r s s s s | "
D_CHORUS = "k:6 r:3 k s:6 r k k s r | "
D_END = "k:3 r r r s r r k k:6 s:18 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF + R1 + R3
          + CHORUS
          + R1V + R2V + R1V + R3
          + CHORUS2
          + SOLO
          + R1
          + END)

BASS = (rest(2)
        + BASS_RIFF + BR1 + BR3
        + BASS_CHORUS
        + BASS_RIFF + BR1 + BR3
        + BASS_CHORUS
        + BASS_RIFF * 2
        + BR1
        + B_END)

DRUMS = (rest(2)
         + beat(D_FUNK, 4, D_FUNK_FILL)
         + beat(D_CHORUS, 4, FILL)
         + beat(D_FUNK, 4, D_FUNK_FILL)
         + beat(D_CHORUS, 4, FILL)
         + beat(D_FUNK, 4, D_FUNK_FILL)
         + D_FUNK
         + D_END)
