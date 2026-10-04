# Candlelight: our own, an occult rock track written for the game. E minor,
# 118 to the beat: dark and catchy, a pop song dressed in black. The hook is
# an organ line that climbs E, F-sharp, G and drops to B, then answers itself
# from C and turns on the D-sharp. A verse that walks up each chord; a chorus
# to sing along, its lines long notes to hold; a bridge where the bass leaves
# the bottom for two bars to sing a sixth under the tune, like two guitars;
# the second half of the chorus and the organ line to close on a long E.
# About fifty-five seconds, and easy.
#
# The tune never goes below D#5 and the bass never above E4, except in the
# two bars where the bass sings under the tune, a sixth apart; the two never
# meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "CANDLELIGHT"
BY = "GHOSTING"
STYLE = "Occult rock"
KEY = "E minor"
PREVIEW = 9                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 118)]

# the organ line, two bars: E minor, then C and B
HOOK = ("e6:6 f#6 g6:12 f#6:6 e6 b5:12 | "
        "c6:6 d6 e6:12 d#6:6 c6 b5:12 | ")

# the verse walks up each chord and steps back down: Em, D, C, B
VERSE = ("e5:6 g5 b5:12 b5:6 a5 g5:12 | "
         "f#5:6 a5 d6:12 d6:6 c6 b5:12 | "
         "e5:6 g5 c6:12 c6:6 b5 a5:12 | "
         "d#5:6 f#5 b5:24 a5:12 | ")

# the chorus, long notes to hold: C, D, Em, B, then C, D, B, Em
CHORUS1 = ("g6:18 f#6:6 e6:24 | "
           "f#6:18 e6:6 d6:24 | "
           "e6:12 b5 c6 d6 | "
           "d#6:36 e6:6 f#6 | ")
CHORUS2 = ("g6:18 f#6:6 e6:24 | "
           "f#6:18 g6:6 a6:24 | "
           "b6:12 a6 f#6 d#6 | "
           "e6:36 r:12 | ")

# the bridge: A minor and E minor, then the twin bars, the bass a sixth under
BRIDGE = ("a5:12 b5:6 c6 e6:12 d6:6 c6 | "
          "b5:24 g5:12 e5 | ")
TWIN = ("e6:12 d6:6 c6 d6:12 e6 | "
        "f#6:12 e6:6 d#6 e6:12 f#6 | ")
TWIN_LOW = ("g5:12 f#5:6 e5 f#5:12 g5 | "
            "a5:12 g5:6 f#5 g5:12 a5 | ")

# the organ line alone for two bars, then the band comes in
MELODY = (HOOK
          + HOOK
          + VERSE
          + CHORUS1 + CHORUS2
          + BRIDGE + TWIN
          + CHORUS2
          + HOOK
          + "e6:48 | ")

BASS_HOOK = eighths("e3") + "c3:6 c3 c3 c3 b2 b2 b2 b2 | "
BASS = (rest(2)
        + BASS_HOOK
        + line(eighths, "e3 d3 c3 b2")
        + line(octaves, "c3 d3 e3 b2 c3 d3 b2 e3")
        + line(eighths, "a2 e3") + TWIN_LOW
        + line(octaves, "c3 d3 b2 e3")
        + BASS_HOOK
        + whole("e3"))

DRUMS = (rest(2)
         + ROCK + FILL
         + beat(ROCK, 4, FILL)
         + beat(ROCK, 8, FILL)
         + HALF * 2 + BACKBEAT * 2           # lighter under the twin bars
         + beat(ROCK, 4, FILL)
         + ROCK + FILL
         + LAST)
