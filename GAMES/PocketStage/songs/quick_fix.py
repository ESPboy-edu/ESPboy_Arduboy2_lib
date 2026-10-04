# Quick Fix: our own, a power pop track written for the game. D, 148 to
# the beat. The riff is the hook and jangles: a note struck twice, a quick
# hammer-on up to it again, a leap up to the high A and down the chord,
# once on D and once on A; later on G and A. A short verse, a sing-along
# chorus of quarter notes over G D A D, G D E A; the riff on G; a bridge in
# B minor where the drums drop to a handclap pattern on the snare; the
# chorus again and the riff to finish on stabs up to D. About fifty-two
# seconds.
#
# The tune never goes below A5 and the bass never above A3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "QUICK FIX"
BY = "CHEAP HACK"
STYLE = "Power pop"
KEY = "D"
BEATS = 4

TEMPO = [(0, 148)]

# the riff: a note twice, a hammer-on up to it, a leap and down again
RA1 = "e6:6 e6 d6:3 e6:3 a6:12 f#6:6 d6 a5 | "
RA2 = "e6:6 e6 d6:3 e6:3 a6:12 e6:6 c#6 a5 | "
RA = RA1 + RA2
RB = ("d6:6 d6 c#6:3 d6:3 b6:12 g6:6 d6 b5 | "
      "e6:6 e6 d6:3 e6:3 a6:12 g6:6 f#6 e6 | ")

VERSE = ("r:6 a5:6 d6 d6 d6 e6 f#6:12 | "
         "r:6 b5:6 d6 d6 d6 e6 g6:12 | "
         "r:6 a5:6 d6 d6 f#6 e6 d6 b5 | "
         "a5:12 c#6:6 e6 a6:12 g6:6 f#6 | ")

# the chorus: quarter notes to sing along to, and long ones to hold
CHORUS = ("d6:12 g6 g6 f#6:6 e6 | "
          "f#6:36 r:12 | "
          "e6:12 a6 a6 g6:6 f#6 | "
          "f#6:24 d6:24 | "
          "d6:12 g6 g6 a6:6 b6 | "
          "a6:36 f#6:12 | "
          "g#6:12 b6 g#6 e6 | "
          "a6:36 r:12 | ")

# the bridge in B minor, over the handclaps
BRIDGE = ("b5:12 d6 f#6:18 e6:6 | "
          "d6:12 b5 g6:24 | "
          "b5:12 d6 f#6 a6 | "
          "g6:12 f#6 e6:6 c#6 a5 c#6 | ")

END = RA1 + "d6:6 r f#6 r a6:6 r d6:12 | "


B_RA = line(eighths, "d3 a2")
B_RB = line(eighths, "g3 a2")
B_VERSE = line(eighths, "d3 g3 d3 a2")
B_CHORUS = line(eighths, "g3 d3 a2 d3 g3 d3 e3 a2")
B_BRIDGE = line(quarters, "b2 g3 b2") + "a2:12 a2 a2:6 c#3 e3 a3 | "
B_END = eighths("d3") + "d3:6 r d3 r a2:6 r d3:12 | "

D_CLAP = "k:12 s:6 s r k s:12 | "       # the bridge: handclaps on the snare
D_CLAP_FILL = "k:12 s:6 s s:3 s s s s:6 s | "
D_END = "k:6 r k r k r k:12 | "

# the riff alone for two bars, then the band comes in
MELODY = (RA
          + RA
          + VERSE
          + CHORUS
          + RB
          + BRIDGE
          + CHORUS
          + END)

BASS = ("r:48 | r:48 | "
        + B_RA
        + B_VERSE
        + B_CHORUS
        + B_RB
        + B_BRIDGE
        + B_CHORUS
        + B_END)

DRUMS = (rest(2)
         + DRIVE + FILL
         + beat(DRIVE, 4, FILL)
         + beat(DRIVE, 8, FILL)
         + DRIVE + FILL
         + D_CLAP * 3 + D_CLAP_FILL
         + beat(DRIVE, 8, FILL)
         + DRIVE + D_END)
