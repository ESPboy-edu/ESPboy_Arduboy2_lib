# Seven Suns: our own, a grunge track in seven written for the game. D with
# the low D chugging under everything (a drop-D feel), 104 to the beat, seven
# beats to the bar grouped 2+2+3. The hook is the riff: two chugs on D before
# each note, G, then C, then a bend into A and a slinky turn down to F; the
# bass plays it with the tune and the drums follow the groups. A sung verse
# over the riff, then a chorus in major that floats across the bar lines
# (long notes that start late and run into the next bar), a solo in an
# eastern scale over a drone on D, the riff again and a last long D. About
# fifty-six seconds.
#
# The tune never goes below D5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "SEVEN SUNS"
BY = "SOUNDCARDEN"
STYLE = "Grunge in 7/4"
KEY = "D"
BEATS = 7

TEMPO = [(0, 104)]

# the riff, one bar of 2+2+3 beats, a variation and the turn
R1 = "d5:6 d5 g5:12 d5:6 d5 c6:12 d5:6 d5 ab5:3 a5 c6 a5 g5:6 f5 | "
R2 = "d5:6 d5 g5:12 d5:6 d5 c6:6 d6 d5:6 d5 ab5:3 a5 c6 a5 g5:6 f5 | "
R3 = "d5:6 d5 g5:12 d5:6 d5 c6:12 d5:3 f5 g5 ab5 a5:6 c6 d6:12 | "

# sung over the riff in the bass
VERSE = ("r:6 a5:6 a5 a5 c6:6 a5 g5 a5 c6:6 d6 c6 a5 g5 f5 | "
         "r:6 a5:6 c6 d6 f6:6 d6 c6 d6 f6:6 g6 f6 d6 c6 d6 | ")

# major now, and floating: the long notes start late and run over the bar line
CHORUS = ("a5:6 c6 d6:12 e6:6 f#6:36 e6:18 | "  # D, C
          "-:18 d6:6 b5 a5 b5 d6 e6:36 | "      # G, A
          "-:12 f6:6 e6 d6:12 f6 g6:36 | ")      # B-flat, C

# an eastern scale on D (D Eb F# G A Bb C) over the drone: notes rolling in
# threes across the beat, a bend, a tremolo on a fifth, a run up to a high C
SOLO = ("d6:3 f#6 a6 d6 f#6 a6 d6 f#6 a6:3 bb6:9 a6:6 g6 "
        "eb6:3 g6 bb6 eb6 g6 bb6 a6:6 g6 f#6 | "
        "c7:3 bb6 a6 g6 a6:6 eb6:3 d6 d6:3 a6 d6 a6 d6 a6 eb6:6 "
        "eb6:3 f#6 g6 a6 bb6 a6 g6 f#6 g6:6 c7 | ")

END = "d5:6 d5 g5:12 d5:6 d5 c6:12 d6:36 | "

# the bass plays the riff with the tune, two octaves down
B1 = "d3:6 d3 g3:12 d3:6 d3 c4:12 d3:6 d3 a3:12 g3:6 f3 | "
B2 = "d3:6 d3 g3:12 d3:6 d3 c4:6 d4 d3:6 d3 a3:12 g3:6 f3 | "
B3 = "d3:6 d3 g3:12 d3:6 d3 c4:12 d3:6 f3 a3 c4 d4:12 | "


def four_three(a, b):
    """a bar of eighths, four beats on one root and three on the next"""
    return " ".join([a + ":6"] + [a] * 7 + [b] * 6) + " | "


BASS_CHORUS = four_three("d3", "c3") + four_three("g3", "a3") + four_three("bb2", "c3")
BASS_SOLO = eighths("d3", 7) * 2
BASS_END = "d3:6 d3 g3:12 d3:6 d3 c4:12 d3:36 | "

# the drums follow the groups: kick kick snare, kick kick snare, kick kick . kick snare
D_RIFF = "k:6 k s:12 k:6 k s:12 k:6 k r k s:12 | "
D_FILL = "k:6 k s:12 k:6 k s:12 s:3 s s s s s s s s s s s | "
D_FLOAT = "k:24 s k:12 s:24 | "                  # wide open under the chorus
D_SOLO = "k:6 k s:12 k:6 k s:12 k:6 k s k s:12 | "
D_END = "k:6 k s:12 k:6 k s:12 k:6 s:30 | "

# the riff alone for a bar, then the band comes in
MELODY = (R1
          + R1 + R2 + R3
          + VERSE
          + CHORUS
          + SOLO
          + R1 + R3
          + END)

BASS = (rest(1, 7)
        + B1 + B2 + B3
        + B1 + B1
        + BASS_CHORUS
        + BASS_SOLO
        + B1 + B3
        + BASS_END)

DRUMS = (rest(1, 7)
         + D_RIFF * 2 + D_FILL
         + D_RIFF + D_FILL
         + D_FLOAT * 2 + D_FILL
         + D_SOLO + D_FILL
         + D_RIFF + D_FILL
         + D_END)
