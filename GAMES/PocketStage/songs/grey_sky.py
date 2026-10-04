# Grey Sky: our own, a grunge track written for the game. F-sharp minor, 116
# to the beat, quiet then loud. The hook is the quiet figure: F# and C#, a
# long A, up again and down to F#, alone for two bars and then over a slow
# bass that moves under it (F#m, D, E and G, the G a half step above home).
# The chorus is loud: the title as two long notes, C# up to F#, over F#, A, G
# and E, then F#, A, G and C#, the bass driving in eighths. A noisy solo
# plays the chorus with bends, trills and a dive; two quiet bars; the chorus
# again and a last long F#. About fifty-six seconds.
#
# The tune never goes below E5 and the bass never above A3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "GREY SKY"
BY = "NIRVANAND"
STYLE = "Grunge"
KEY = "F-sharp minor"
BEATS = 4

TEMPO = [(0, 116)]

# the quiet figure, two bars, the same notes over every chord of the verse
Q1 = "f#5:6 c#6:6 r:6 a5:18 b5:6 c#6:6 | "
Q2 = "e6:6 c#6:6 r:6 a5:18 f#5:12 | "
# its second bar on the way up into the chorus
UP = "e6:6 f#6:6 r:6 g6:12 f#6:6 e6:6 d6:6 | "

# the title, C# up to F#, and its answers
CHORUS = ("c#6:12 f#6:36 | "         # F#
          "e6:18 c#6:6 a5:24 | "      # A
          "b5:12 d6:36 | "            # G
          "e6:18 b5:6 e6:24 | ")      # E
CHORUS2 = ("c#6:12 f#6:36 | "        # F#
           "a6:18 g#6:6 e6:24 | "     # A
           "g6:18 f#6:6 d6:24 | "     # G
           "e6:12 f#6:12 g#6:24 | ")  # C#

# the chorus again, noisy: bends, a stutter, a trill and a dive
SOLO = ("b5:3 c#6:9 e6:3 f#6:21 e6:6 f#6:6 | "
        "e6:6 e6 g#6:3 a6:9 e6:6 c#6:6 a5:12 | "
        "b5:6 d6 b5 d6 f#6:3 g6:9 d6:12 | "
        "e6:3 f#6 e6 f#6 e6:12 b5:6 g5 e5:12 | ")

END = "f#6:48 | "


def drive(root, to=None):
    """a bar of eighths on the root, the last one on the way to the next"""
    return " ".join([root + ":6"] + [root] * 6 + [to or root]) + " | "


# the quiet bass: slow, a root and a step down to the fifth below
B_QUIET = "f#3:36 c#3:12 | d3:36 a2:12 | e3:36 b2:12 | g3:36 d3:12 | "
B_UP = "f#3:36 c#3:12 | " + drive("g3")
B_CHORUS = drive("f#3", "g#3") + drive("a3") + drive("g3", "f#3") + drive("e3")
B_CHORUS2 = drive("f#3", "g#3") + drive("a3") + drive("g3", "d3") + drive("c#3", "e3")

D_QUIET = "k:12 r:6 k:6 s:24 | "               # half time, sparse
D_UP = "k:6 s k s s:3 s s s s s s s | "         # the build into the chorus

# the figure alone for two bars, then the band comes in, quietly
MELODY = (Q1 + Q2
          + Q1 + Q2 + Q1 + Q2 + Q1 + UP          # verse
          + CHORUS + CHORUS2
          + SOLO
          + Q1 + UP                              # quiet again
          + CHORUS2
          + END)

BASS = (rest(2)
        + B_QUIET + B_UP
        + B_CHORUS + B_CHORUS2
        + B_CHORUS
        + B_UP
        + B_CHORUS2
        + "f#3:48 | ")

DRUMS = (rest(2)
         + D_QUIET * 5 + D_UP
         + beat(ROCK, 8, FILL)
         + beat(DRIVE, 4, FILL)
         + D_QUIET + D_UP
         + beat(ROCK, 4, FILL)
         + LAST)
