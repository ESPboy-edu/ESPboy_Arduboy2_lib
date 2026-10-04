# Skyforge: our own, a power metal track written for the game. E minor, with
# the chorus in G, 196 to the beat. The hook is a speed-picked riff: a
# burst of sixteenths that keeps coming back to the low E while the note
# above it moves (B, C, B, A), then an answer in eighths that climbs to
# the D and the high E. The riff alone for two bars, then the band with a
# burst of double kick; the riff over C and D, a verse over a galloping
# bass, a heroic chorus in G that leaps up a sixth into its long notes, a
# duelling solo (sweeps, then a lower guitar that sings, then sweeps
# higher, then two bars where the bass voice plays a third under the tune
# like a second guitar), the riff with double kick again, the chorus, and a
# triumphant end on a held B over G. About fifty-six seconds.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch, except in the two bars of twin guitars, where
# the bass voice is up in octave 6, always a third under the tune
# (tools/build_data.py says why they must not meet).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

from kit import *

TITLE = "SKYFORGE"
BY = "DRAGONFORTH"
STYLE = "Power metal"
KEY = "E minor"
BEATS = 4

TEMPO = [(0, 196)]

# the riff: the burst on the E pedal, then the answer; and over C and D
R1 = "e5:3 b5 e5 c6 e5 b5 e5 a5 g5:6 a5 b5 d6 | "
R2 = "e6:6 d6 b5 d6 e6:3 g6 e6 d6 b5:12 | "
R3 = "e5:3 c6 e5 d6 e5 c6 e5 b5 e5:6 g5 c6 e6 | "
R4 = "f#5:3 d6 f#5 e6 f#5 d6 f#5 a5 d6:12 a6:12 | "
RIFF = R1 + R2 + R3 + R4

# over E minor, C, D and B, then E minor, C, A minor and D
VERSE = ("e6:12 b5:12 e6:12 g6:6 f#6:6 | "
         "e6:24 g6:12 e6:12 | "
         "f#6:12 d6:12 f#6:12 a6:6 g6:6 | "
         "f#6:24 d#6:12 f#6:12 | "
         "e6:12 b5:12 e6:12 g6:6 a6:6 | "
         "c7:24 b6:12 g6:12 | "
         "a6:12 e6:12 c6:12 e6:12 | "
         "f#6:24 a6:24 | ")

# in G: G, D, E minor, C, G, D, C, D; the lines open with a leap of a sixth
# up into a long note
CHORUS = ("d6:12 b6:36 | "
          "a6:18 g6:6 f#6:24 | "
          "b5:12 g6:36 | "
          "e6:18 g6:6 c7:24 | "
          "d6:12 b6:36 | "
          "c7:18 b6:6 a6:24 | "
          "g6:12 a6:12 b6:12 c7:12 | ")
CHORUS1 = CHORUS + "a6:48 | "
CHORUS2 = CHORUS + "b6:24 a6:24 | "

# the duel: sweeps up the chord; a lower guitar that sings and steps up
# into its long notes; the sweeps higher; then both guitars at once
SOLO = ("e6:3 g6 b6 g6 e6:6 b5 e6:3 g6 b6 g6 e6:6 b5 | "
        "e6:3 g6 c7 g6 e6:6 c6 e6:3 g6 c7 g6 c7:12 | "
        "a5:6 d6 f#6 g6:18 f#6:6 e6 | "
        "f#6:12 d#6:6 e6 f#6 b6:18 | "
        "b5:3 e6 g6 b6 e6 g6 b6 g6 e6:6 g6 b6:12 | "
        "c6:3 e6 g6 c7 g6 e6 c6 e6 g6:6 a6 c7:12 | ")
TWIN = ("e6:6 f#6 g6 a6 b6 c7 b6 g6 | "
        "a6:6 b6 a6 f#6 d6:12 f#6:12 | ")
# the second guitar: the same line a third lower, played by the bass voice
TWIN_LOW = ("c6:6 d6 e6 f#6 g6 a6 g6 e6 | "
            "f#6:6 g6 f#6 d6 b5:12 d6:12 | ")

END = "d6:6 g6 b6 g6 d6 g6 b6 c7 | b6:48 | "

BASS_RIFF = line(eighths, "e3 e3 c3 d3")
BASS_VERSE = line(gallop, "e3 c3 d3 b2 e3 c3 a2 d3")
BASS_CHORUS = line(eighths, "g3 d3 e3 c3 g3 d3 c3 d3")
BASS_SOLO = line(eighths, "e3 c3 d3 b2 e3 c3")
BASS_END = "g3:6 g3 g3 g3 g3 g3 g3 a3 | g3:48 | "

D_FILL = "k:6 k s k k k s:3 s s s | "
# the gallop on 1 and 3 only, so the bass is heard
D_GALLOP = "k:6 k:3 k s:12 k:6 k:3 k s:12 | "
D_TWIN = "k:12 s k s | "
D_TWIN2 = "k:12 s k s:6 s | "

# the riff alone for two bars, then the band comes in
MELODY = (R1 + R2
          + RIFF
          + R1 + R2
          + VERSE
          + CHORUS1
          + SOLO + TWIN
          + RIFF
          + CHORUS2
          + END)

BASS = ("r:48 | r:48 | "
        + BASS_RIFF
        + line(eighths, "e3 e3")
        + BASS_VERSE
        + BASS_CHORUS
        + BASS_SOLO + TWIN_LOW
        + BASS_RIFF
        + BASS_CHORUS
        + BASS_END)

DRUMS = ("r:48 | r:48 | "
         + DOUBLE * 2 + GALLOP + D_FILL
         + GALLOP + D_FILL
         + beat(D_GALLOP, 8, D_FILL)
         + beat(PUNK, 8, D_FILL)
         + beat(D_GALLOP, 6, D_FILL, every=2) + D_TWIN + D_TWIN2
         + DOUBLE * 2 + GALLOP + D_FILL
         + beat(PUNK, 8, D_FILL)
         + STABS + LAST)
