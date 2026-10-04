# Sky Rider: our own, a heavy blues rock track written for the game. E, 112
# to the beat. The hook is the riff's twist: a figure three beats long
# (a heavy E, a quick G up to A, back to E, the jump to D and down to B)
# played four times over three bars of four, so its accent lands a beat
# earlier every time across the bar lines, then a tag that climbs the blues
# scale to a high E puts it square again. The bass doubles the riff two
# octaves down and the drums hit its accents. A middle part in half time
# with long bent notes over A, E, C and D, a fast solo of triplet licks
# over E, A and B, and the riff to close. About fifty-six seconds.
#
# The tune never goes below D5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "SKY RIDER"
BY = "LED BALLOON"
STYLE = "Heavy blues rock"
KEY = "E minor"
BEATS = 4

TEMPO = [(0, 112)]

# the figure, three beats: e5:9 g5:3 a5:6 e5:6 d6:6 b5:6 - four of them
# from bar 1, cut at the bar lines, then the tag
RIFF = ("e5:9 g5:3 a5:6 e5:6 d6:6 b5:6 e5:9 g5:3 | "
        "a5:6 e5:6 d6:6 b5:6 e5:9 g5:3 a5:6 e5:6 | "
        "d6:6 b5:6 e5:9 g5:3 a5:6 e5:6 d6:6 b5:6 | "
        "g5:6 a5:6 bb5:3 b5:3 d6:6 e6:24 | ")

# half time: long notes bent up into, over A, E, C and D
MIDDLE = ("e6:24 d6:3 e6:9 c#6:6 a5:6 | "
          "g5:3 a5:45 | "
          "b5:24 a5:3 b5:9 g5:6 e5:6 | "
          "d5:3 e5:45 | "
          "e6:24 g6:3 a6:9 g6:6 e6:6 | "
          "f#6:3 g6:9 f#6:12 d6:12 a5:12 | ")

# triplet licks in the pentatonic over E, E, A, A, B, B
SOLO = ("b5:4 d6 e6 d6 b5 a5 b5:12 d6:4 e6 g6 | "
        "a6:4 g6 e6 g6:12 e6:4 d6 b5 d6:12 | "
        "c6:3 d6:9 e6:4 g6 a6 b6:12 a6:4 g6 e6 | "
        "e6:4 g6 e6 g6 a6 g6 a6 b6 a6 b6:12 | "
        "b5:4 d6 e6 f#6 a6 b6 a6:12 f#6:12 | "
        "b6:3 a6 f#6 e6 d6 e6 f#6 a6 b6:4 a6 f#6 d#6:12 | ")

END = ("e5:9 g5:3 a5:6 e5:6 d6:6 b5:6 bb5:3 b5:9 | "
       "e6:36 r:12 | ")

# the bass: the riff two octaves down; long notes in the middle; fifths
# under the solo
BASS_RIFF = shift(RIFF, -24)
BASS_MIDDLE = ("a3:24 e3 | a3:24 g3:12 e3 | e3:24 b3 | e3:24 d3:12 c3 | "
               "c3:24 g3 | d3:24 a3:12 b3 | ")
BASS_SOLO = line(fifths, "e3 e3 a2 a2 b2 b2")
BASS_END = shift(END, -24)

# the drums on the riff's accents: the heads of the figure and its D
D_RIFF = ("k:18 k:6 s:12 k:12 | "
          "r:6 k:6 s:12 k:18 k:6 | "
          "s:12 k:18 k:6 s:12 | "
          "k:6 k s:6 s k:12 s:12 | ")
D_IN = "r:24 s:3 s s s s s s s | "            # the roll that brings the drums in
D_HALF_FILL = "k:12 r s:4 s s s s s | "
D_END = "k:18 k:6 s:12 k:12 | " + LAST

# the riff with the bass for four bars, then the drums come in
MELODY = (RIFF + RIFF
          + MIDDLE
          + SOLO
          + RIFF
          + END)

BASS = (BASS_RIFF + BASS_RIFF
        + BASS_MIDDLE
        + BASS_SOLO
        + BASS_RIFF
        + BASS_END)

DRUMS = (rest(3) + D_IN
         + D_RIFF
         + HALF * 5 + D_HALF_FILL
         + ROCK * 5 + TRIPLETS
         + D_RIFF
         + D_END)
