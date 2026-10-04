# Pipe Dream: our own, a flute rock track written for the game. E dorian,
# 132 to the beat. The tune is a flute: a folky melody full of little
# ornaments (a slide onto a note, a shake, a cut into a long note) over a
# heavy riff in the bass that leans on the dorian C sharp. The hook is the
# flute's first phrase, alone for two bars: down to B, a slide onto the G,
# and up through C sharp to the high A. Then the riff and the drums; a
# verse and a higher part with shakes into the long notes, a jig in
# triplets (a 6/8 feel), a frantic solo, the hook and the verse again, and
# the riff played by the flute and the bass together, a leap up, and a
# trilled E to end. About fifty-five seconds.
#
# The tune never goes below D5 and the bass never above C sharp 4, so the
# two cannot meet on one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

from kit import *

TITLE = "PIPE DREAM"
BY = "JETHRO NULL"
STYLE = "Flute rock"
KEY = "E dorian"
BEATS = 4

TEMPO = [(0, 132)]

# the hook: over E, then A (the dorian major IV)
HOOK = ("e6:12 b5:6 e6:6 f#6:3 a6:3 g6:6 f#6:6 e6:6 | "
        "c#6:6 e6:6 f#6:6 a6:6 g6:3 f#6:3 e6:18 | ")

VERSE = ("b6:12 a6:6 g6:6 f#6:6 e6:6 b5:12 | "
         "c#6:6 d6:6 e6:6 f#6:6 e6:24 | "
         "b6:12 a6:6 g6:6 a6:3 b6:3 a6:6 g6:6 e6:6 | "
         "f#6:6 a6:6 e6:6 c#6:6 d6:3 e6:21 | ")

# higher, over G, D, A and E: shakes (a fast third) into the long notes
HIGH = ("d6:6 g6:6 b6:24 a6:6 g6:6 | "
        "f#6:3 a6:3 f#6:3 a6:3 f#6:3 a6:21 f#6:6 d6:6 | "
        "e6:6 a6:6 b6:12 a6:6 g6:6 f#6:6 e6:6 | "
        "e6:3 g6:3 e6:3 g6:3 f#6:12 e6:24 | ")

# the jig: triplets, long-short and three-even, over E, D, E and B minor
JIG = ("e6:4 b5 e6 g6:8 e6:4 f#6:12 e6:8 d6:4 | "
       "d6:4 a5 d6 f#6:8 d6:4 e6:12 d6:8 c#6:4 | "
       "b5:4 e6 g6 b6:8 a6:4 g6:12 f#6:8 g6:4 | "
       "a6:8 e6:4 b6:8 f#6:4 g6:12 b5:12 | ")

SOLO = ("b5:3 e6 g6 b6 a6:12 e6:3 g6 a6 g6 e6:12 | "
        "a6:4 e6 b6 a6 d6 g6 c#6:12 e6:6 a6:6 | "
        "b6:3 g6 b6 g6 b6:12 g6:4 a6 b6 e6:12 | "
        "a6:3 e6 b6 g6 e6 b5 d6 e6 g6:4 a6 b6 e6:12 | ")

# the riff, the flute two octaves over the bass; then a leap up and the last
# trill
UNISON = ("e5:6 e5 d5 e5 g5 e5 c#6 b5 | "
          "a5:6 a5 g5 a5 b5:12 e6:12 | ")
END = ("e5:6 e5 d5 e5 g5 e5 b6 a6 | "
       "e6:3 f#6:3 e6:3 f#6:3 e6:36 | ")

# the heavy riff in the bass: E with its neighbour D, up to the C sharp; then A
RIFF = "e3:6 e3 d3 e3 g3 e3 c#4 b3 | a3:6 a3 g3 a3 b3:12 e3:12 | "
BASS_HIGH = line(eighths, "g3 d3 a2 e3")
BASS_JIG = line(shuffle, "e3 d3 e3 b2")
BASS_END = "e3:6 e3 d3 e3 g3 e3 b3 a3 | e3:6 r:42 | "    # with the flute's leap

D_BEAT = "k:6 k s r k k s r | "
D_FILL = "k:6 k s r k r s:3 s s s | "
D_BACK = "k:12 s k:6 k s:12 | "
D_JIG = SHUFFLE
D_HEAVY = "k:6 k s r k k s s | "         # under the riff the flute and the bass play together

# the flute alone for two bars, then the band comes in
MELODY = (HOOK
          + HOOK
          + VERSE
          + HIGH
          + JIG
          + SOLO
          + HOOK
          + VERSE
          + UNISON
          + END)

BASS = ("r:48 | r:48 | " + RIFF
        + RIFF * 2
        + BASS_HIGH
        + BASS_JIG
        + RIFF * 2
        + RIFF
        + RIFF * 2
        + RIFF
        + BASS_END)

DRUMS = ("r:48 | r:48 | " + D_BEAT + D_FILL
         + beat(D_BEAT, 4, D_FILL)
         + D_BACK * 3 + D_FILL
         + D_JIG * 3 + TRIPLETS
         + beat(D_BEAT, 4, D_FILL)
         + D_BEAT + D_FILL
         + beat(D_BEAT, 4, D_FILL)
         + D_HEAVY * 2
         + D_HEAVY + LAST)
