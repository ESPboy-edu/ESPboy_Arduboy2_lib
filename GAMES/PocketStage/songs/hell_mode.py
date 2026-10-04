# Hell Mode: our own, an extreme thrash track written for the game, built on
# the tritone, E against B-flat. 216 to the beat. The hook is the riff: a
# burst of four fast E's and a stab on the B-flat, four more and a stab on
# the A, then the E's again, a B, and a long B-flat. The riff with the band,
# two bars of blast beat under a trill on the tritone, a riff of tritone
# leaps that creeps up in half steps, a crushing part at half time, a wild
# solo of runs and leaps, a last crushing riff and stabs to end. The bass
# doubles the riffs. About fifty-three seconds, and very fast.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "HELL MODE"
BY = "RELAYER"
STYLE = "Extreme thrash"
KEY = "E"
BEATS = 4

TEMPO = [(0, 216)]

# the riff: bursts of E, stabs on the tritone
H1 = "e5:3 e5 e5 e5 bb5:12 e5:3 e5 e5 e5 a5:12 | "
H2 = "e5:3 e5 e5 e5 b5:6 e5 bb5:24 | "
H3 = "g5:6 f#5 f5 e5 bb5:6 r:18 | "       # down in half steps, a stab, a stop
RIFF = H1 + H2 + H1 + H3

# the blast: the tritone as a trill, E and B-flat, then F and B
BLAST_TUNE = ("e5:3 bb5 e5 bb5 e5 bb5 e5 bb5 a5:24 | "
              "f5:3 b5 f5 b5 f5 b5 f5 b5 e6:24 | ")

# tritone leaps that creep up in half steps
C1 = "e5:6 bb5 e5 bb5 f5:12 b5 | "
C2 = "f#5:6 c6 f#5 c6 g5:3 g5 g5 g5 c#6:12 | "
C3 = "e5:6 bb5 e5 bb5 f#5:12 c6 | "
C4 = "bb5:3 bb5 bb5 bb5 a5:6 ab5 g5:12 e5 | "
CREEP = C1 + C2 + C3 + C4

# the crushing part at half time
CRUSH = ("e5:24 bb5 | "
         "a5:12 g5 e5:24 | "
         "e5:24 bb5 | "
         "b5:12 c6 bb5:24 | "
         "e5:24 bb5 | "
         "a5:12 g5 e5:24 | "
         "f5:12 e5 bb5 a5 | "
         "e5:6 r e5 r e5:24 | ")

# the solo: runs, leaps, the tritone everywhere
SOLO = ("e5:3 f5 f#5 g5 ab5 a5 bb5 b5 e6:12 bb5 | "
        "a6:6 e6 bb5 e5 b6:12 r | "
        "c7:3 bb6 g6 e6 d6 bb5 a5 g5 e5:12 e6 | "
        "bb5:6 e6 bb6 e6 bb5:12 e5 | "
        "e6:3 e6 e6 e6 f6 f6 f6 f6 bb6:24 | "
        "a6:6 f6 d6 bb5 g5:12 c#6 | "
        "c7:6 bb6 g6 e6 c#6:12 bb5 | "
        "e5:6 e6 f5 f6 f#5 f#6 g5 g6 | ")
BASS_SOLO = line(eighths, "e3 e3 e3 bb2 e3 g3 c#3 e3")

# the last crushing riff, and stabs
F1 = "e5:6 e5 r e5 bb5:12 a5 | "
F2 = "e5:6 e5 r e5 g5:12 bb5 | "
F3 = "e5:6 e5 r e5 bb5:12 b5 | "
F4 = "c6:6 b5 bb5 a5 e5:24 | "
FINAL = F1 + F2 + F3 + F4
END = "e5:6 r:18 bb5:6 r:18 | e5:6 r:42 | "

D_CRUSH = "k:6 k r k s:12 k:6 r | "        # half time: the snare on 3
D_STOP = "k:6 s k s k:6 r:18 | "          # with the stab and the stop

# the riff alone for two bars, then the band comes in
MELODY = (H1 + H2
          + RIFF + RIFF
          + BLAST_TUNE
          + CREEP + CREEP
          + CRUSH
          + SOLO
          + FINAL + FINAL
          + H1 + H2
          + END)

BASS = ("r:48 | r:48 | "
        + shift(RIFF + RIFF, -24)
        + "e3:24 a2 | f3:24 e3 | "
        + shift(CREEP + CREEP, -24)
        + shift(CRUSH, -24)
        + BASS_SOLO
        + shift(FINAL + FINAL + H1 + H2, -24)
        + "e3:6 r:18 bb2:6 r:18 | e3:6 r:42 | ")

DRUMS = ("r:48 | r:48 | "
         + (PUNK * 3 + D_STOP) * 2
         + BLAST * 2
         + PUNK * 7 + FILL2
         + D_CRUSH * 7 + FILL
         + PUNK * 2 + DOUBLE * 2 + PUNK * 3 + FILL2
         + PUNK * 7 + FILL2
         + PUNK + FILL2
         + STABS + LAST)
