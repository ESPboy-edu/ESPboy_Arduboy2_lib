# XYZZY: our own, a prog rock instrumental written for the game, seven
# beats to the bar all the way through. E minor, 152 to the beat. The hook
# is the riff, grouped 2+2+3: a stutter on the low E that jumps to the
# octave and drops to B, twice, then three beats that climb back up (the
# second time to the high A and down again). The riff alone for two bars,
# then the bass doubling it and the drums; syncopated lines that the tune
# and the bass play together (dotted eighths, stutters); a synth-like
# melody of long notes swooped into over E minor, C, D and B; fast runs in
# unison, chords swept up and down; and the riff again to end on a held E.
# About fifty-five seconds.
#
# The tune never goes below E5 and the bass never above E4; where they play
# together they are two or three octaves apart, so the two never meet on
# one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes. A bar is 7 beats, 84 ticks.

from kit import *

TITLE = "XYZZY"
BY = "FLUSH"
STYLE = "Prog rock"
KEY = "E minor"
BEATS = 7

TEMPO = [(0, 152)]

# the riff: two beats, two beats, three beats
RIFF_A = "e5:3 e5 e6:6 b5 d6 e5:3 e5 e6:6 b5 a5 g5:6 a5 b5:3 d6 e6:6 d6 b5 | "
RIFF_B = "e5:3 e5 e6:6 b5 d6 e5:3 e5 e6:6 b5 a5 b5:6 e6 b5:3 g6 a6:6 d6 b5 | "
END = "e5:3 e5 e6:6 b5 d6 e5:3 e5 e6:6 b5 a5 b5:3 d6 e6:30 | "

# together: dotted eighths across the beat, four beats and three
SYNC_1 = "e6:9 d6:9 b5:6 a5:9 b5:9 d6:6 e6:6 r:3 e6:3 d6:6 b5:6 a5:6 b5:6 | "
SYNC_2 = "g6:9 f#6:9 d6:6 e6:9 f#6:9 a6:6 b6:6 r:3 b6:3 f#6:6 d6:6 e6:6 b5:6 | "
SYNC_4 = ("e6:6 r:3 e6:3 b5:6 r:3 b5:3 g6:6 r:3 g6:3 d6:6 r:3 d6:3 "
          "c7:12 g6:6 a6:6 b6:12 | ")

# the synth: long notes, each swooped into from far below (a glide), over
# E minor, C, D and B
SYNTH = ("b5:3 g6:33 f#6:6 e6:6 d6:3 a6:33 | "
         "e6:3 b6:33 a6:6 g6:6 e6:12 g6:24 | "
         "d6:3 a6:33 g6:6 f#6:6 d6:12 a6:24 | "
         "b5:3 f#6:21 b6:12 a6:6 g6:6 f#6:6 d#6:6 b5:24 | ")

# the runs: chords swept up and down, six sixteenths and an eighth, twice,
# then eight and a quarter
RUN_1 = ("e5:3 g5 b5 e6 b5 g5 a5:6 f#5:3 a5 d6 f#6 d6 a5 b5:6 "
         "g5:3 b5 e6 g6 e6 b5 g5 b5 e6:12 | ")
RUN_2 = ("c6:3 c7 g6 e6 g6 c6 d6:6 d6:3 f#6 a6 c7 a6 f#6 e6:6 "
         "d#6:3 f#6 a6 b6 a6 f#6 d#6 b5 e6:12 | ")
RUN_3 = ("b5:3 e6 g6 b6 g6 e6 f#6:6 c6:3 e6 a6 c7 a6 e6 g6:6 "
         "b5:3 d6 g6 b6 g6 d6 b5 d6 e6:12 | ")
RUN_4 = ("c7:3 a6 e6 c6 e6 a6 b6:6 b6:3 f#6 d#6 b5 d#6 f#6 a6:6 "
         "g6:3 e6 b5 g5 b5 e6 g6 b6 e6:12 | ")


def fold(text):
    """the tune's line for the bass to play with it: two octaves down, or
    three where two would take the bass above E4"""
    out = []
    for tok in text.split():
        name, colon, dur = tok.partition(":")
        if name[0] in "abcdefg" and name[-1].isdigit():
            m = midi(name) - 24
            name = note(m - 12 if m > midi("e4") else m)
        out.append(name + colon + dur)
    return " ".join(out) + " "


def seven(root):
    """the bass under the synth: eighths on the root with the fifth, 2+2+3"""
    f = up(root, 7)
    return "%s:6 %s %s %s %s %s %s %s %s %s %s %s %s %s | " % (
        root, root, f, root, root, root, f, root, root, f, root, f, root, f)


# the bass doubles the riff, the lines and the runs (fold), and pulses under
# the synth
BASS_SYNC = fold(SYNC_1 + SYNC_2 + SYNC_1 + SYNC_4)
BASS_SYNTH = seven("e3") + seven("c3") + seven("d3") + seven("b2")
BASS_RUNS = fold(RUN_1 + RUN_2 + RUN_3 + RUN_4)

D_RIFF = "k:6 k s r k k s r k r s r s s | "
D_FILL = "k:6 k s r k k s r s:3 s s s s s s s s s s s | "
D_SYNC = "k:9 k:9 s:6 k:9 k:9 s:6 k:6 r:3 k:3 s:6 k:6 s:6 s:6 | "
D_SYNC4 = "k:6 r:3 k:3 s:6 r:3 s:3 k:6 r:3 k:3 s:6 r:3 s:3 k:12 s:6 s:6 s:12 | "
D_SYNTH = "k:12 r s r k s r | "
D_RUN = "k:12 r s k r s s | "
D_LAST = "k:6 s:78 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF_A + RIFF_B
          + RIFF_A + RIFF_B
          + SYNC_1 + SYNC_2 + SYNC_1 + SYNC_4
          + SYNTH
          + RUN_1 + RUN_2 + RUN_3 + RUN_4
          + RIFF_A + RIFF_B + RIFF_A + END)

BASS = ("r:84 | r:84 | "
        + fold(RIFF_A + RIFF_B)
        + BASS_SYNC
        + BASS_SYNTH
        + BASS_RUNS
        + fold(RIFF_A + RIFF_B + RIFF_A + END))

DRUMS = ("r:84 | r:84 | "
         + D_RIFF + D_FILL
         + D_SYNC * 3 + D_SYNC4
         + D_SYNTH * 3 + D_FILL
         + D_RUN * 4
         + D_RIFF + D_FILL + D_RIFF + D_LAST)
