# Slapback: our own, a rockabilly song written for the game. E major, 176 to
# the beat, swung (eighths long-short, 8 + 4 ticks). Three times round a
# twelve-bar blues (E, A, B seven) after a four-bar intro. The hook is the
# lead lick: a slide from G into G sharp, down the chord, a slide from D
# sharp into E; then rocking between two strings like a double stop (E
# against C sharp, E against B) and the slide into G sharp again. The
# head sings short phrases and the lick answers them; the second time
# round is a solo of rockabilly licks; the third starts in stop-time (the
# band hits beat one and stops, the tune fills the gaps), the band comes back
# in, and the tune lands on C sharp over the E: the sixth chord rockabilly
# ends on. The slap bass walks in quarters all the way. About fifty-five
# seconds.
#
# The tune never goes below A5 and the bass never above D4, so the two
# cannot meet on one pitch.

from kit import *

TITLE = "SLAPBACK"
BY = "STRAY BITS"
STYLE = "Rockabilly"
KEY = "E major"
BEATS = 4
TEMPO = [(0, 176)]

# the lick: slides into G sharp and E, then rocking between two strings
H1 = "g6:4 g#6:8 e6:8 b5:4 d#6:4 e6:8 b5:12 | "
H2 = "e6:4 c#6 e6 c#6 e6 b5 g6:8 g#6:4 e6:12 | "
HOOK = H1 + H2

# the head: a phrase, the lick answers (E E E E A A E E B7 A7 E B7)
CALL1 = "e6:8 g#6:4 b6:12 g#6:8 e6:4 c#6:12 | b5:12 e6:36 | "
CALL2 = "c#6:8 e6:4 a6:12 e6:8 c#6:4 f#6:12 | e6:12 a6:36 | "
ANSWER2 = "b6:4 e6 b6 e6 b6 e6 g#6:24 | r:24 c#6:8 d6:4 d#6:12 | "
CALL3 = "f#6:8 a6:4 b6:12 a6:8 f#6:4 d#6:12 | e6:8 g6:4 a6:12 c#6:24 | "
TURN = "e6:8 g#6:4 b6:8 g#6:4 e6:12 r:12 | b5:12 d#6 f#6 a6 | "
HEAD = CALL1 + HOOK + CALL2 + ANSWER2 + CALL3 + TURN

# the solo: rockabilly licks round the twelve bars
SOLO = ("e6:4 g6 g#6 e6:8 b5:4 c#7:8 b6:4 g#6:12 | "
        "g6:4 g#6:8 e6:4 g#6:8 b6:24 | "
        "b6:4 e6 b6 e6 b6 e6 c#7:24 | "
        "b6:8 g#6:4 a#6:8 g6:4 a6:8 f#6:4 d6:12 | "
        "a6:4 e6 c#6 a6:12 e6:24 | "
        "c6:4 c#6:8 e6:8 a6:4 g6:8 e6:4 c#6:12 | "
        "e6:8 g#6:4 b6:12 c#7:8 b6:4 g#6:12 | "
        "g6:4 g#6:8 g6:4 g#6:8 e6:24 | "
        "d#6:4 f#6 a6 b6:24 a6:12 | "
        "a6:4 g6 e6 c#6:24 a5:12 | "
        "e6:4 g#6 b6 c#7:36 | "
        "b6:8 a6:4 f#6:8 d#6:4 b5:24 | ")

# stop-time: the band hits beat one, the tune fills the bar; then the head
# again, and the end on C sharp over E
STOP = ("e6:12 r:12 g#6:8 g#6:4 b6:12 | "
        "e6:12 r:12 g#6:8 g#6:4 c#6:12 | "
        "e6:12 r:12 b6:4 e6 b6 e6 b6 e6 | "
        "e6:12 r:12 g6:4 g#6:8 d6:12 | ")
END = "e6:8 g#6:4 b6:8 g#6:4 e6:8 d#6:4 d6:8 b5:4 | c#6:48 | "


# the slap bass walks: up the chord (root, third, fifth, sixth), then down
# (seventh, sixth, fifth, third)
def up4(r):
    return walk(r)


def down4(r):
    return " ".join([up(r, 10) + ":12", up(r, 9), up(r, 7), up(r, 4)]) + " | "


B7 = "b2:12 d#3 f#3 a3 | "              # up the B seventh chord
B_INTRO = up4("e3") + down4("e3")
B_12 = (up4("e3") + down4("e3") + up4("e3") + down4("e3")
        + up4("a2") + down4("a2") + up4("e3") + down4("e3")
        + B7 + up4("a2") + up4("e3") + B7)
B_STOP = ("e3:8 r:40 | " * 4
          + up4("a2") + down4("a2") + up4("e3") + down4("e3")
          + B7 + up4("a2") + "e3:12 g#3 b3 d4 | " + "e3:48 | ")

D_12 = beat(SHUFFLE, 12, TRIPLETS, 4)
D_STOP = ("k:12 r:36 | " * 4
          + SHUFFLE * 3 + TRIPLETS + SHUFFLE * 3 + LAST)

# the lick alone for two bars, then the band comes in
MELODY = (HOOK
          + HOOK
          + HEAD
          + SOLO
          + STOP + CALL2 + ANSWER2 + CALL3 + END)

BASS = (rest(2) + B_INTRO
        + B_12
        + B_12
        + B_STOP)

DRUMS = (rest(2) + SHUFFLE + TRIPLETS
         + D_12
         + D_12
         + D_STOP)
