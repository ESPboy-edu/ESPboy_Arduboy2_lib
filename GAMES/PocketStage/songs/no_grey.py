# No Grey: our own, a garage blues stomp written for the game. A minor, 116
# to the beat. The hook is one big riff: an octave drop on its first two
# notes (no... grey) and a fall back down through the blue note, the bass
# doubling it two octaves under like a guitar through an octave pedal. The
# riff alone for two bars, then the stomp (a kick on every beat, the snare
# cracking with it on 2 and 4); a sung verse over a sparse bass, a chorus
# of long notes over D, A and E, a raw slide solo (a grace note into every
# long note), the chorus again and the riff to close on the octave drop.
# About fifty-eight seconds.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

from kit import *

TITLE = "NO GREY"
BY = "WHITE SPRITES"
STYLE = "Garage blues stomp"
KEY = "A minor"
BEATS = 4

TEMPO = [(0, 116)]

# the riff, two bars: the octave drop, then the fall through the blue note;
# the turn climbs back to E instead
R1 = "a6:12 a5:12 r:6 a5:6 c6:6 d6:6 | "
R2 = "g6:6 eb6:6 d6:6 c6:6 a5:12 e5:12 | "
T2 = "g6:6 eb6:6 d6:6 c6:6 d6:12 e6:12 | "
RIFF = R1 + R2
TURN = R1 + T2

VERSE = ("r:6 e5:6 a5:6 c6:6 d6:12 c6:12 | "
         "a5:6 g5:6 e5:36 | "
         "r:6 e5:6 a5:6 c6:6 e6:12 d6:12 | "
         "c6:12 d6:6 c6:6 a5:24 | ")

CHORUS = ("a6:18 g6:6 a6:24 | "
          "e6:36 r:12 | "
          "a6:18 g6:6 a6:12 c7:12 | "
          "b6:24 g6:12 e6:12 | ")

# the slide: every long note is reached from a step under it
SOLO = ("d6:3 e6:21 g6:3 a6:9 e6:12 | "
        "d6:6 c6:6 a5:12 g5:3 a5:21 | "
        "g6:3 a6:21 g6:6 e6:6 d6:3 e6:9 | "
        "c6:6 a5:6 c6:6 d6:6 eb6:3 e6:21 | ")

END = "a6:12 a5:36 | "

# the bass doubles the riff two octaves down (the octave drop stays on A)
B1 = "a3:24 r:6 a3:6 c4:6 d4:6 | "
B2 = "g3:6 eb4:6 d4:6 c4:6 a3:12 e3:12 | "
BT2 = "g3:6 eb4:6 d4:6 c4:6 d4:12 e4:12 | "
BASS_RIFF = B1 + B2
BASS_TURN = B1 + BT2
BASS_VERSE = ("a3:12 r a3 r | a3:12 r a3 r | "
              "d3:12 r d3 r | a3:12 r e3 g3 | ")
BASS_CHORUS = line(eighths, "d3 a3 d3 e3")
BASS_END = "a3:12 a2:36 | "

# the stomp: a kick on every beat, and on 2 and 4 the snare one tick behind
# it, so the two crack together
D_STOMP = "k:12 k:1 s:11 k:12 k:1 s:11 | "
D_PUSH = "k:12 k:1 s:11 k:6 k k:1 s:11 | "  # with a kick pushed in
D_FILL = "k:12 k:1 s:11 k:12 s:6 s | "
D_ROLL = "k:12 s s:6 s s:4 s s | "
D_SPARSE = "k:12 r k k:1 s:11 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + TURN
          + VERSE
          + RIFF
          + CHORUS
          + TURN
          + SOLO
          + CHORUS
          + RIFF + R1 + END)

BASS = ("r:48 | r:48 | " + BASS_TURN
        + BASS_VERSE
        + BASS_RIFF
        + BASS_CHORUS
        + BASS_TURN
        + BASS_RIFF * 2
        + BASS_CHORUS
        + BASS_RIFF + B1 + BASS_END)

DRUMS = ("r:48 | r:48 | " + D_STOMP + D_FILL
         + D_SPARSE * 3 + D_FILL
         + D_STOMP + D_FILL
         + STOMP * 3 + D_ROLL
         + D_STOMP + D_FILL
         + D_PUSH * 3 + D_FILL
         + STOMP * 3 + D_ROLL
         + D_STOMP * 2 + STABS + LAST)
