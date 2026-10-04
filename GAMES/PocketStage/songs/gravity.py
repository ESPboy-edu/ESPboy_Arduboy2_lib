# Gravity: our own, a slow psychedelic rock track written for the game. B
# minor, 64 to the beat. A singing lead in call and response: each call
# leaps up and bends into a long note (the hook: F sharp, B, then C sharp
# bent up to D and held), each answer comes down and bends back onto B.
# Over B minor, G, E minor and F sharp, then again with the calls reaching
# higher; then a slow climb over F sharp that lands on a long high B, held
# over G, and a last answer down to B. The bass plays long notes and slow
# walking lines, the drums a slow beat with the snare on three. About
# fifty-six seconds.
#
# The tune never goes below F#5 and the bass never above B3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "GRAVITY"
BY = "PINK FLOAT"
STYLE = "Psychedelic rock"
KEY = "B minor"
BEATS = 4

TEMPO = [(0, 64)]

# B minor, B minor, G, G, E minor, F sharp: three calls and their answers
PART1 = ("f#5:6 b5 c#6:3 d6:21 e6:6 f#6 | "
         "e6:3 d6 b5:6 a5 f#5 a5:3 b5:21 | "
         "g5:6 b5 d6:3 e6:21 f#6:6 g6 | "
         "f#6:3 e6 d6:6 b5 g5 a5:3 b5:21 | "
         "b5:6 e6 f#6:3 g6:21 a6:6 g6 | "
         "f#6:3 e6 c#6:6 a#5 f#5 a#5:3 b5 c#6:18 | ")

# B minor, B minor, G, A: the calls reach higher
PART2 = ("f#5:6 b5 c#6:3 d6:9 e6:6 f#6 a6:12 | "
         "a6:3 f#6 e6:6 d6 b5 c#6:3 d6:9 b5:12 | "
         "d6:6 g6 f#6 d6 e6:3 f#6:15 e6:3 d6 | "
         "e6:3 f#6 e6:6 c#6 a5 c#6:3 d6:9 e6:12 | ")

# F sharp, F sharp: the climb; B minor: the long high B; G; B minor
CLIMB = ("f#5:6 a#5 c#6 e6 f#6:12 e6:6 f#6 | "
         "c#6:3 d6 e6:6 f#6 a6 a#6:24 | "
         "b6:48 | "
         "-:24 a6:6 g6 e6:3 f#6 d6:6 | "
         "f#5:6 b5 c#6:3 d6:9 b5:24 | ")          # the hook once more, home to B

# the bass: long notes, walking slowly into the next chord
BASS = ("b2:48 | b2:24 d3:12 f#3 | "
        "g3:48 | g3:24 a3:12 f#3 | "
        "e3:24 d3:12 e3 | f#3:24 e3:12 c#3 | "
        "b2:48 | b2:24 d3:12 f#3 | "
        "g3:24 d3:12 e3 | a3:36 g3:12 | "
        "f#3:24 a#3:12 c#3 | f#3:24 c#3:12 a#2 | "
        "b2:48 | g3:24 e3:12 f#3 | b2:48 | ")

D_SLOW = "k:12 r:6 k:6 s:24 | "
D_FILL = "k:12 r:6 k:6 s:12 s:3 s s s | "

# the lead and the bass alone for two bars, then the drums come in
MELODY = PART1 + PART2 + CLIMB

DRUMS = ("r:48 | r:36 s:3 s s s | "
         + D_SLOW * 3 + D_FILL
         + D_SLOW * 3 + D_FILL
         + D_SLOW + D_FILL
         + D_SLOW * 2 + LAST)
