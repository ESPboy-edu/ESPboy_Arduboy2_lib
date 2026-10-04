# Dune Buggy: our own, a stoner rock track written for the game. D, slow and
# heavy at 92 to the beat, every pair of eighths swung long-short. The hook
# is the riff: D twice, up to A and C, a bend up into C, down A and G; then a
# long bend from F up to F#, home over C to D and a swung A back into it.
# The bass plays it with the tune, bends and all. A chorus of long bent notes
# over G, C and D, the riff, a jam (the bass walks a boogie, the tune bends
# and answers itself up the neck), the riff and a last bend into a long low
# D. About fifty-five seconds, and easy to play.
#
# The tune never goes below C5 and the bass never above C4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "DUNE BUGGY"
BY = "SLEEP MODE"
STYLE = "Stoner rock"
KEY = "D"
BEATS = 4

TEMPO = [(0, 92)]

# the riff, two bars, swung (8 + 4 ticks to a beat)
R1 = "d5:8 d5:4 a5:8 c6:4 b5:3 c6:9 a5:8 g5:4 | "
R2 = "f5:3 f#5:21 d5:8 c5:4 d5:8 a5:4 | "
RIFF = R1 + R2

# G, C, D: long notes, each bent into
CHORUS = ("a5:3 b5:21 a5:8 g5:4 d5:12 | "
          "b5:3 c6:21 b5:8 a5:4 g5:12 | "
          "e5:3 f#5:21 a5:8 d6:4 c6:12 | "
          "c6:3 d6:45 | ")

# the jam: bends and answers, a little higher each time, then back down
JAM = ("a5:3 c6:21 a5:8 g5:4 a5:12 | "
       "d6:12 c6:8 a5:4 g5:8 f5:4 d5:12 | "
       "a5:3 c6:21 d6:8 c6:4 a5:12 | "
       "f6:3 g6:21 f6:8 d6:4 c6:12 | "
       "d6:8 d6:4 d6:8 d6:4 f6:3 f#6:21 | "
       "c6:8 a5:4 g5:8 f5:4 d5:24 | ")

END = "c5:3 d5:45 | "

# the bass plays the riff with the tune, two octaves down, bends and all
B1 = "d3:8 d3:4 a3:8 c4:4 b3:3 c4:9 a3:8 g3:4 | "
B2 = "f3:3 f#3:21 d3:8 c3:4 d3:8 a3:4 | "
BASS_RIFF = B1 + B2
BASS_CHORUS = line(shuffle, "g3 c3 d3 d3")
BASS_JAM = "".join(boogie("d3", swing=True) for _ in range(5)) + "d3:8 d3:4 d3:8 d3:4 d3:24 | "

D_FILL = "k:8 r:4 s:8 k:4 s:4 s s s:8 s:4 | "     # the shuffle with a triplet roll at the end

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF * 2
          + CHORUS
          + RIFF
          + JAM
          + RIFF
          + END)

BASS = (rest(2)
        + BASS_RIFF * 2
        + BASS_CHORUS
        + BASS_RIFF
        + BASS_JAM
        + BASS_RIFF
        + "d3:48 | ")

DRUMS = (rest(2)
         + beat(SHUFFLE, 4, D_FILL)
         + beat(SHUFFLE, 4, D_FILL)
         + SHUFFLE + D_FILL
         + beat(SHUFFLE, 6, D_FILL, 3)
         + SHUFFLE + D_FILL
         + LAST)
