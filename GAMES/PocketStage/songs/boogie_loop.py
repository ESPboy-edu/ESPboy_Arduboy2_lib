# Boogie Loop: our own, a boogie rock track written for the game. E, 172 to
# the beat, straight eighths. The bass plays the boogie line the whole way
# (up the chord to the sixth and the seventh and back, with the octave on
# top at the end of each line). The tune is a chant that stays easy: the
# hook is four quick notes (E, E, B, E) and the E an octave up held, then
# long notes to answer it. Three rounds of the twelve-bar blues, the first two in E,
# the last moved up a fourth to A, and a stop-start ending: the band hits
# on one and stops, the tune plays the hook into the gap, twice, then one
# long chord. About fifty-four seconds.
#
# The tune never goes below D5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "BOOGIE LOOP"
BY = "STATUS BYTE"
STYLE = "Boogie rock"
KEY = "E"
BEATS = 4

TEMPO = [(0, 172)]


def hook(root, fifth, octave):
    """the chant's short figure: the root twice, the fifth, the root, and
    the octave held, with a breath after it"""
    return "%s:6 %s %s %s %s:18 r:6 | " % (root, root, fifth, root, octave)


H_E = hook("e5", "b5", "e6")
H_A = hook("a5", "e6", "a6")
H_B = hook("b5", "f#6", "b6")
H_D = hook("d5", "a5", "d6")

# E E E E, A A E E, B A E B: the hook on the odd bars, long notes between
ROUND1 = (H_E + "g#6:24 e6:12 b5 | "
          + H_E + "b6:24 e6:12 g#6 | "
          + H_A + "a6:24 e6:12 c#6 | "
          + H_E + "b6:48 | "
          + H_B + "a6:24 e6:12 c#6 | "
          + H_E + "b5:12 d#6 f#6:24 | ")
ROUND2 = (H_E + "b6:24 e6:12 g#6 | "
          + H_E + "g#6:12 e6 b5:24 | "
          + H_A + "a6:12 c#6 e6:24 | "
          + H_E + "g#6:24 b6 | "
          + H_B + "a6:12 e6 a5:24 | "
          + H_E + "b5:12 d#6 f#6:24 | ")
# up a fourth: A A A A, D D A A, E D A E
ROUND3 = (H_A + "c#6:24 e6:12 a6 | "
          + H_A + "e6:24 a5 | "
          + H_D + "f#6:24 d6:12 a5 | "
          + H_A + "e6:48 | "
          + H_E + "f#6:24 d6:12 a5 | "
          + H_A + "b5:12 e6 g#6:24 | ")

# stop-start: the band on one, the hook's quick notes alone in the gap
ENDING = ("a6:6 r:18 a5:6 a5 e6 a5 | "
          "a6:6 r:18 a5:6 a5 e6 a5 | "
          "a6:48 | ")


def boogie8(r):
    """kit's boogie line with the octave on top instead of the seventh"""
    return " ".join([r + ":6"] + [up(r, s) for s in (4, 7, 9, 12, 9, 7, 4)]) + " | "


def bass_round(i, iv, v):
    """twelve bars of the boogie, the octave on top at the end of each line"""
    return (boogie(i) * 3 + boogie8(i)
            + boogie(iv) * 2 + boogie(i) + boogie8(i)
            + boogie(v) + boogie(iv) + boogie(i) + boogie8(v))


BASS = (bass_round("e3", "a2", "b2") * 2
        + bass_round("a2", "d3", "e3")
        + "a2:6 r:42 | a2:6 r:42 | a2:48 | ")

D_FILL = "k:12 s k:6 s:3 s s:6 s:3 s | "
D_STOP = "k:6 r:42 | "

# the tune and the boogie for two bars, then the drums come in
MELODY = ROUND1 + ROUND2 + ROUND3 + ENDING

DRUMS = (rest(2)
         + BACKBEAT * 9 + D_FILL
         + BACKBEAT * 11 + D_FILL
         + DRIVE * 11 + D_FILL
         + D_STOP * 2 + "k:6 s:42 | ")
