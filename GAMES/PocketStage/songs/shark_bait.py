# Shark Bait: our own, a surf rock song written for the game. A minor, 170
# to the beat, the tune in A harmonic minor (the G sharp, and F to G sharp
# for the exotic step). The hook: a tremolo burst on the low A (four quick
# picks of one note), up the chord to a high C and B; then a tremolo on the
# low G sharp, up through B and D to F and down to E. The first theme
# builds on that (A minor, E, F, D minor; the tremolo now low, now high)
# and ends on a fast slide down the neck; the
# second theme is long notes up high with one long tremolo, and slides down
# again. Then a melody break (the band stops, the hook alone), a drum break
# (the tune rests, the drums roll), both themes again, and a last slide
# down to a held A. The bass drives in eighths and walks up into the
# chords. About fifty-four seconds.
#
# The tune never goes below E5 and the bass never above A3, so the two
# cannot meet on one pitch.

from kit import *

TITLE = "SHARK BAIT"
BY = "SURFACE MOUNTS"
STYLE = "Surf rock"
KEY = "A minor"
BEATS = 4
TEMPO = [(0, 170)]

# the hook: a tremolo on A, up to C and B; a tremolo on G sharp, down (Am, E)
H1 = "a5:3 a5 a5 a5 e6:6 a6 c7:12 b6:12 | "
H2 = "g#5:3 g#5 g#5 g#5 b5:6 d6 f6:12 e6:12 | "
# a slide down the neck, from a low note (E)
SLIDE = "e5:6 b6:3 a6 g#6 f6 e6 d6 c6:6 b5:6 e6:12 | "

# the first theme (Am E Am E F E Dm E)
THEME_A = (H1 + H2
           + "a5:3 a5 a5 a5 e6:6 a6 c7:12 e6:12 | "
           + "b6:3 b6 b6 b6 g#6:6 f6 e6:12 g#6:12 | "
           + "c6:3 c6 c6 c6 f6:6 a6 c7:12 a6:12 | "
           + H2
           + "d6:3 d6 d6 d6 f6:6 a6 f6:12 d6:12 | "
           + SLIDE)

# the second theme: long notes up high, one long tremolo (Dm Am Dm E F Am Dm E)
B_HEAD = ("d6:12 f6:12 a6:24 | "
          "c7:12 b6:6 a6 e6:24 | "
          "f6:12 a6:12 c7:24 | "
          "b6:3 b6 b6 b6 b6 b6 b6 b6 r:6 g#6:6 e6:12 | "
          "a6:24 c7:24 | "
          "e6:24 a6:24 | ")
THEME_B = B_HEAD + "f6:12 d6:12 a6:12 f6:12 | " + SLIDE
END = B_HEAD + "f6:12 d6:12 g#6:3 g#6 g#6 g#6 e5:12 | " + "a6:3 g#6 f6 e6 d6 c6 b5 a5:27 | "

# the melody break: the hook alone; the drum break: the tune rests
M_BREAK = H1 + "g#5:3 g#5 g#5 g#5 b5:6 d6 f6:12 r:12 | "
D_REST = rest(2)

# the bass: driving eighths, walking up (or down) into the next chord
B_A = ("a3:6 a3 a3 a3 a3 a3 g3 f3 | e3:6 e3 e3 e3 e3 e3 f#3 g#3 | "
       "a3:6 a3 a3 a3 a3 a3 g3 f3 | e3:6 e3 e3 e3 e3 e3 f#3 g#3 | "
       "f3:6 f3 f3 f3 f3 f3 f3 f3 | e3:6 e3 e3 e3 e3 e3 e3 e3 | "
       "d3:6 d3 d3 d3 d3 d3 d3 d#3 | e3:6 e3 e3 e3 e3 e3 e3 e3 | ")
B_B_HEAD = ("d3:6 d3 d3 d3 d3 d3 f3 g3 | a3:6 a3 a3 a3 a3 a3 f3 e3 | "
            "d3:6 d3 d3 d3 d3 d3 d#3 d#3 | e3:6 e3 e3 e3 e3 e3 e3 e3 | "
            "f3:6 f3 f3 f3 f3 f3 g3 g#3 | a3:6 a3 a3 a3 a3 a3 f3 e3 | ")
B_B = B_B_HEAD + "d3:6 d3 d3 d3 d3 d3 d#3 d#3 | e3:6 e3 e3 e3 e3 e3 e3 e3 | "
B_END = B_B_HEAD + "d3:6 d3 d3 d3 e3 e3 e3 e3 | a3:6 r:6 a3:36 | "
B_BREAKS = "a3:6 r:42 | r:48 | " + rest(2)

D_SURF = "k:6 k s:6 k k:6 k s:6 k | "     # driving eighths, the snare on 2 and 4
D_SURF_FILL = "k:6 k s:6 k s:3 s s s s s s s | "
D_A = (D_SURF * 3 + D_SURF_FILL) * 2
D_BREAKS = ("k:6 r:42 | r:48 | "                       # the band stops for the hook
            + "k:6 s:3 s k:6 s:3 s k:6 s:3 s k:6 s:3 s | " + ROLL)   # the drums roll
D_END = D_SURF * 3 + D_SURF_FILL + D_SURF * 3 + LAST

# the hook alone for two bars, then the band comes in with the first theme
MELODY = (H1 + H2
          + THEME_A
          + THEME_B
          + M_BREAK + D_REST
          + THEME_A
          + END)

BASS = (rest(2)
        + B_A
        + B_B
        + B_BREAKS
        + B_A
        + B_END)

DRUMS = (rest(2)
         + D_A
         + D_A
         + D_BREAKS
         + D_A
         + D_END)
