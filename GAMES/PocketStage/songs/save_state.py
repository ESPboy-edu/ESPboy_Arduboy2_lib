# Save State: our own, a power ballad written for the game. D major, 70 to
# the beat in a 12/8 feel: every beat is three eighths of 4 ticks. The bass
# starts alone on a rippling arpeggio, then the tune plays the hook of the
# chorus over it: a long F#, then E D E. A verse that sighs down and climbs
# back (D, F#m, G, A) over the arpeggio and soft drums; a big chorus of
# long notes over G A, Bm F#m, G A, D with the band in; a soaring solo that
# turns to B, and the last chorus a step up in E, closing on one long high
# G#. About fifty-eight seconds.
#
# The tune never goes below D5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "SAVE STATE"
BY = "BON JOYPAD"
STYLE = "Power ballad"
KEY = "D major"
BEATS = 4

TEMPO = [(0, 70)]

# the hook: the first line of the chorus
HOOK = "f#6:24 e6:8 d6:4 e6:12 | "

INTRO = "r:36 a5:4 d6 e6 | f#6:24 e6:8 d6:4 c#6:4 a5 e5 | "

# the verse: a sigh down and a climb back, on the fifth of each chord
VERSE = ("a5:8 b5:4 a5:12 f#5:8 e5:4 d5:4 e5 f#5 | "
         "c#6:8 d6:4 c#6:12 a5:8 f#5:4 e5:4 f#5 a5 | "
         "d6:8 e6:4 d6:12 b5:8 a5:4 g5:4 a5 b5 | "
         "c#6:8 b5:4 a5:8 b5:4 c#6:4 d6 e6 a5 d6 e6 | ")

CHORUS_LINES = (HOOK
                + "d6:24 c#6:8 b5:4 c#6:12 | "
                + "b5:4 d6 g6:16 f#6:8 e6:4 a6:12 | ")
CHORUS = CHORUS_LINES + "f#6:24 e6:4 d6 c#6 d6:12 | "

SOLO = ("f#6:8 b5:4 d6 f#6 b6:16 a6:4 g6 f#6 | "
        "a6:12 f#6:4 e6 d6 e6:8 c#6:4 e6 f#6 a6 | "
        "d6:4 g6 b6 a6:8 g6:4 f#6 d#6 b5 d#6 f#6 a6 | ")

# the last chorus a step up, and the long note to end on
LAST = shift(CHORUS_LINES, 2) + "g#6:48 | "


def arp(a, b, c, d):
    """two beats of the arpeggio: up four notes and back down two"""
    return "%s:4 %s %s %s %s %s " % (a, b, c, d, c, b)


B_INTRO = (arp("d3", "a3", "d4", "e4") + arp("f#3", "a3", "d4", "e4") + "| "
           + arp("g3", "b3", "d4", "e4") + arp("a2", "e3", "a3", "c#4") + "| ")
B_VERSE = (arp("d3", "a3", "d4", "e4") + arp("f#3", "a3", "d4", "e4") + "| "
           + arp("f#3", "a3", "c#4", "e4") * 2 + "| "
           + arp("g3", "b3", "d4", "e4") * 2 + "| "
           + arp("a2", "e3", "a3", "b3") + arp("a2", "e3", "a3", "c#4") + "| ")
# the band: the root long, then short, and a step on to the next chord;
# the chorus starts on G with B under it, a fifth under the hook's F#
B_CHORUS_LINES = ("b2:12 b2:8 d3:4 a2:12 a2:8 e3:4 | "
                  "b2:12 b2:8 f#3:4 f#3:12 f#3:8 c#4:4 | "
                  "g3:12 g3:8 d4:4 a2:12 a2:8 c#3:4 | ")
B_CHORUS = B_CHORUS_LINES + "d3:12 d3:8 a3:4 d3:20 c#3:4 | "
B_SOLO = ("b2:12 b2:8 f#3:4 g3:12 g3:8 d4:4 | "
          "d3:12 d3:8 a3:4 a2:12 a2:8 e3:4 | "
          "g3:12 g3:8 d4:4 b2:12 b2:8 b2:4 | ")
B_LAST = shift(B_CHORUS_LINES, 2) + "e3:48 | "

D_SOFT = "k:12 r s r | "                  # the verse: the snare on 3 only
D_SOFT_FILL = "k:12 r s:4 s s s s s | "
D_BALLAD = "k:12 s k:8 k:4 s:12 | "      # the band: the snare on 2 and 4
D_FILL = "k:12 s k:8 k:4 s:4 s s | "
D_LIFT = "k:12 s s:4 s s s s s | "       # into the key change
D_END = "k:4 s:44 | "

# the arpeggio alone for a bar, then the hook on top of it
MELODY = INTRO + VERSE + CHORUS + SOLO + LAST

BASS = B_INTRO + B_VERSE + B_CHORUS + B_SOLO + B_LAST

DRUMS = (rest(2)
         + D_SOFT * 3 + D_SOFT_FILL
         + D_BALLAD * 3 + D_FILL
         + D_BALLAD * 2 + D_LIFT
         + D_BALLAD * 2 + D_FILL
         + D_END)
