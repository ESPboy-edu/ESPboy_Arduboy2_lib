# Duckwalk: our own, a fifties rock and roll track written for the game. B
# flat, 168 to the beat, the eighths swung (8 + 4 ticks). The hook is the
# intro lick, played alone: F and D struck by turns, swung and then in
# triplets (a double stop, implied), a bend up into the high B flat, then
# E flat bent up to F and the blue note bent up to D. Then three rounds of the twelve-bar
# blues over a bass walking the boogie: the "vocal" line with a guitar
# answer after every call, a solo of repeated-note licks, the vocal line
# again, and to end a turnaround that slides down from A flat to F over an
# F struck between (while the bass walks down from B flat to G), F7, and
# the last B flat. About fifty-six seconds.
#
# The tune never goes below F5 and the bass never above E flat 4, so the
# two cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "DUCKWALK"
BY = "CHIP BERRY"
STYLE = "Rock and roll"
KEY = "B flat"
PREVIEW = 1                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 168)]

INTRO = ("f6:8 d6:4 f6:8 d6:4 f6:4 d6 f6 ab6:4 bb6:8 | "
         "eb6:4 f6:8 d6:8 bb5:4 db6:4 d6:8 bb5:12 | ")

# the vocal line: a call, then the guitar's answer
CALL1 = "d6:12 f6:8 d6:4 g6:12 f6 | "
CALL2 = "d6:12 f6:8 d6:4 bb6:12 g6 | "
ANS1 = "r:12 db6:4 d6:8 f6:8 d6:4 f6:12 | "
ANS2 = "r:12 bb6:4 bb6 bb6 ab6:8 f6:4 d6:12 | "
VOCAL = (CALL1 + ANS1 + CALL2 + ANS2                       # B flat
         + "g6:12 bb6:8 g6:4 c7:12 bb6 | "                 # E flat
         + "r:12 gb6:4 g6:8 eb6:8 c6:4 eb6:12 | "
         + CALL1 + "r:12 db6:4 d6:8 bb5:8 g5:4 bb5:12 | "  # B flat
         + "c6:12 eb6:8 c6:4 f6:12 eb6 | "                 # F
         + "bb5:12 eb6:8 bb5:4 g6:12 f6 | ")               # E flat
TAG = ("d6:24 r:12 f6:8 d6:4 | "                           # B flat
       "f5:8 a5:4 c6:8 eb6:4 f6:8 eb6:4 c6:8 a5:4 | ")     # F

# the solo: repeated notes, struck by turns or all on one
SOLO = ("f6:4 d6 f6 d6 f6 d6 bb6:24 | "
        "r:12 f6:8 f6:4 f6:8 f6:4 db6:4 d6:8 | "
        "bb6:8 bb6:4 g6:8 bb6:4 ab6:4 bb6:8 g6:12 | "
        "f6:8 d6:4 bb5:24 r:12 | "
        "eb6:4 g6 eb6 g6 eb6 g6 bb6:24 | "                 # E flat
        "r:12 bb6:8 bb6:4 bb6:8 c7:4 g6:12 | "
        "d6:4 bb6 d6 bb6 d6 bb6 f6:24 | "                  # B flat
        "r:12 ab6:8 f6:4 db6:4 d6:8 bb5:12 | "
        "c7:8 c7:4 c7:8 a6:4 f6:12 a6 | "                  # F
        "bb6:8 bb6:4 bb6:8 bb6:4 g6:12 eb6 | "             # E flat
        "d6:4 f6 bb6 f6 d6 bb5 d6:24 | "                   # B flat
        "f5:8 a5:4 c6:8 eb6:4 f6:8 a6:4 c7:12 | ")         # F

# the turnaround: the line slides down over an F struck between its notes
TURN = ("ab6:8 f6:4 g6:8 f6:4 gb6:8 f6:4 f6:8 d6:4 | "     # B flat
        "c6:8 a5:4 c6:8 eb6:4 f6:8 a6:4 c7:12 | "          # F
        "d6:4 f6 g6 bb6:24 r:12 | ")                       # the last chord

# the bass walks the boogie, swung
BB = boogie("bb2", swing=True)
BB_W = "bb2:12 d3 f3 g3 | "                       # quarters, walking
BEB = boogie("eb3", swing=True)
BEB_W = "eb3:12 g3 bb3 c4 | "
BF = boogie("f3", swing=True)
BF_TURN = "f3:8 a3:4 c4:8 eb4:4 c4:8 a3:4 f3:8 a3:4 | "
# walking under the calls, the boogie under the answers
TEN = BB_W + BB + BB_W + BB + BEB_W + BEB + BB_W + BB + BF + BEB_W
ROUND = TEN + BB_W + BF_TURN
BASS_END = (TEN
            + "bb3:12 a3 ab3 g3 | "               # down with the turnaround
            + BF_TURN
            + "bb2:4 d3 f3 bb2:24 r:12 | ")

D_FILL = "k:12 s:12 s:4 s s s s s | "

# the lick alone for two bars, then the band comes in
MELODY = (INTRO
          + VOCAL + TAG
          + SOLO
          + VOCAL + TURN)

BASS = (rest(2)
        + ROUND
        + ROUND
        + BASS_END)

DRUMS = (rest(2)
         + SHUFFLE * 11 + D_FILL
         + BACKBEAT * 11 + D_FILL
         + SHUFFLE * 10 + D_FILL + "k:12 s s:4 s s k:12 | "
         + "k:4 r:8 s:24 r:12 | ")
