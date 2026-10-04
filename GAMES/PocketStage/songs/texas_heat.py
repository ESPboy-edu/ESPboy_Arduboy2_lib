# Texas Heat: our own, a Texas blues shuffle written for the game. E, 120 to
# the beat, the eighths swung (8 + 4 ticks). The hook is the opening lick,
# played alone: a triplet from E up through G to G sharp, the minor third
# bent to the major, twice, then a rake of repeated B's and a bend up into
# E. Then two rounds of the twelve-bar blues with the quick change (E A E
# E, A A E E, B A E B): the first with the hook as the head over a bass
# walking the shuffle; the second starts in stop time (the band hits on
# beat one only and the lead fills the gaps), then hot licks to the end:
# rakes, triplet runs, bends as grace notes. About fifty-six seconds.
#
# The tune never goes below E5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "TEXAS HEAT"
BY = "RAM VAUGHAN"
STYLE = "Texas blues shuffle"
KEY = "E"
BEATS = 4

TEMPO = [(0, 120)]

# the hook: E-G-G sharp twice, then the rake and the bend
HOOK = "e6:4 g6 g#6 b6:8 g#6:4 e6:4 g6 g#6 e6:8 d6:4 | "
RAKE = "b5:4 b5 b5 d6:3 e6:9 b5:8 g5:4 g#5:12 | "

# the first round: the head
HEAD = (HOOK
        + "a5:4 c6 c#6 e6:8 c#6:4 g6:8 e6:4 c#6:8 a5:4 | "        # A
        + RAKE
        + "e5:8 g#5:4 b5:8 c#6:4 d6:8 c#6:4 b5:8 g#5:4 | "         # with the bass
        + "a5:4 c6 c#6 e6:8 c#6:4 a5:4 c6 c#6 a5:8 g5:4 | "        # the hook on A
        + "e6:4 e6 e6 g6:3 a6:9 e6:8 c#6:4 a5:12 | "
        + HOOK
        + "g6:4 e6 d6 e6 d6 b5 d6 e6 g6 e6:8 d6:4 | "
        + "a6:3 b6:9 a6:8 f#6:4 d#6:8 f#6:4 a6:8 b6:4 | "          # B
        + "c7:4 b6 a6 g6:8 e6:4 c#6:8 e6:4 g6:8 e6:4 | "           # A
        + "e6:4 e6 e6 d6:3 e6:9 g6:4 e6 d6 b5:12 | "
        + "f#6:4 a6 b6 a6:8 f#6:4 d#6:8 f#6:4 b5:12 | ")           # B

# stop time: the band on beat one, the lead in the gaps
STOP = ("e6:12 r:4 g6 g#6 b6:8 g#6:4 e6:4 d6 b5 | "
        "b6:4 b6 b6 a6 g6 e6 a6:3 b6:9 g6:8 e6:4 | "
        "e6:12 d6:4 b5 d6 e6:4 d6 b5 a5:8 g5:4 | "
        "e5:4 g5 g#5 b5 d6 e6 g6:3 g#6:9 b6:12 | ")

# the rest of the second round: hot licks
LICKS = ("a5:4 c#6 e6 g6:3 a6:9 e6:8 g6:4 e6:8 c#6:4 | "            # A
         "e6:4 g6 a6 c7:8 a6:4 g6:8 e6:4 c#6:8 a5:4 | "
         "b6:4 b6 b6 g#6 e6 b5 d6:3 e6:9 g6:8 e6:4 | "                 # E
         + HOOK
         + "d#6:4 f#6 a6 b6:8 a6:4 f#6:8 d#6:4 f#6:8 a6:4 | "       # B
         "b6:3 c7:9 a6:8 g6:4 e6:8 c#6:4 a5:12 | "                  # A
         "d6:8 e6:4 c#6:8 e6:4 c6:8 e6:4 b5:8 e6:4 | "              # E: the turnaround
         "b5:4 d6 d#6 f#6:8 a6:4 b6:24 | ")                         # B

END = HOOK + "b5:4 b5 b5 d6:3 e6:33 | "

# the bass walks the shuffle: up the chord and back, with turns
BE = boogie("e3", swing=True)
BE2 = "e3:8 g3:4 g#3:8 b3:4 c#4:8 d4:4 c#4:8 b3:4 | "
BA = boogie("a2", swing=True)
BA2 = "a2:8 c#3:4 e3:8 f#3:4 g3:8 e3:4 c#3:8 b2:4 | "
BB = boogie("b2", swing=True)
BB_TURN = "b2:8 d#3:4 f#3:8 a3:4 b3:8 a3:4 f#3:8 d#3:4 | "
FIRST4 = BE + BA + BE + BE                     # the fourth doubles the tune
REST8 = BA + BA2 + BE + BE2 + BB + BA + BE + BB_TURN
BASS_STOP = "e3:8 r:40 | " * 3 + "e3:8 r:16 e3:8 f#3:4 g3:8 g#3:4 | "

D_FILL = "k:8 r:4 s:8 k:4 s:4 s s s s s | "
D_STOP = "k:8 r:40 | " * 3 + "k:8 r:16 s:4 s s s s s | "

# the hook alone for two bars, then the band comes in
MELODY = (HOOK + RAKE
          + HEAD
          + STOP + LICKS
          + END)

BASS = (rest(2)
        + FIRST4 + REST8
        + BASS_STOP + REST8
        + BE + "r:12 e3:36 | ")

DRUMS = (rest(2)
         + SHUFFLE * 11 + D_FILL
         + D_STOP
         + SHUFFLE * 7 + D_FILL
         + SHUFFLE + "k:12 s:36 | ")
