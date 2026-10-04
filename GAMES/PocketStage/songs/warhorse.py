# Warhorse: our own, a heavy metal track written for the game. E minor, 164
# to the beat, and everything gallops: the tune, the bass and the kick drum.
# The hook is the riff: a gallop on E that climbs G, A, B, then gallops on
# C and G (the second time on D and A, up to a long E). A verse that tells
# its story over the galloping bass; a chorus of long notes, each line a
# step higher than the last; two guitars in harmony (the bass leaves the
# bottom for four bars and plays a third under the tune); a fast solo; the
# riff to close on a high E. About fifty-three seconds.
#
# The tune never goes below E5 and the bass never above E4, except in the
# four harmony bars, where the bass stays a third under the tune; the two
# never meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "WARHORSE"
BY = "TIN MAIDEN"
STYLE = "Heavy metal gallop"
KEY = "E minor"
PREVIEW = 13                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 164)]


def gal(*roots):
    """a bar of gallops, an eighth and two sixteenths on a root a beat (or
    one root for the whole bar)"""
    if len(roots) == 1:
        roots = roots * 4
    return " ".join("%s:6 %s:3 %s:3" % (r, r, r) for r in roots) + " | "


# the riff, two bars, and its turn: E, then C and D
R1 = ("e5:6 e5:3 e5 e5:6 e5:3 e5 g5:6 a5 b5:12 | "
      "c6:6 c6:3 c6 g5:6 g5:3 g5 d6:6 c6 b5:12 | ")
R2 = ("e5:6 e5:3 e5 e5:6 e5:3 e5 g5:6 a5 b5:12 | "
      "d6:6 d6:3 d6 a5:6 a5:3 a5 e6:24 | ")
BASS_R1 = gal("e3") + gal("c3", "c3", "d3", "d3")
BASS_R2 = gal("e3") + gal("d3", "d3", "e3", "e3")

# the verse: Em, Em, C, D, Em, B
VERSE = ("e6:6 e6 d6 b5 a5:12 b5 | "
         "g5:6 g5 a5 b5 e6:12 d6 | "
         "c6:6 c6 b5 a5 g5:12 c6 | "
         "d6:6 d6 e6 d6 a5:24 | "
         "e6:6 e6 d6 b5 a5:12 b5 | "
         "f#5:6 a5 b5 d#6 f#6:12 d#6 | ")
BASS_VERSE = line(gallop, "e3 e3 c3 d3 e3 b2")

# the chorus: each line a step higher, held; the riff answers in bar 4
CHORUS = ("e6:6 e6 e6:12 g6:24 | "
          "f#6:6 f#6 f#6:12 a6:24 | "
          "b6:24 a6:12 g6 | "
          "e6:6 e6:3 e6 g6:6 g6:3 g6 b6:6 a6 g6:12 | "
          "e6:6 e6 e6:12 g6:24 | "
          "f#6:6 f#6 f#6:12 a6:24 | "
          "b6:24 a6:12 f#6 | "
          "d#6:36 r:12 | ")
BASS_CHORUS = line(gallop, "c3 d3 e3 e3 c3 d3 b2 b2")

# two guitars: the tune, and the bass a third under it
TWIN = ("g5:6 a5 b5 g5 e6:12 d6:6 b5 | "
        "c6:6 b5 a5 b5 c6:12 e6 | "
        "d6:6 c6 b5 c6 d6:12 f#6 | "
        "d#6:6 e6 f#6 e6 d#6:12 b5 | ")
TWIN_LOW = ("e5:6 f#5 g5 e5 c6:12 b5:6 g5 | "
            "a5:6 g5 f#5 g5 a5:12 c6 | "
            "b5:6 a5 g5 a5 b5:12 d6 | "
            "b5:6 c6 d#6 c6 b5:12 g5 | ")

# the solo: Em, C, D, Em, C, B
SOLO = ("e6:3 f#6 g6 a6 b6:6 a6:3 g6 e6:6 b5 e6:12 | "
        "g6:6 g6:3 g6 e6:6 e6:3 e6 c6:6 c6:3 c6 g5:12 | "
        "a5:3 b5 c6 d6 e6 f#6 g6 a6 f#6:12 d6 | "
        "e6:6 b6 g6 e6 b6 g6 e6:12 | "
        "c7:12 b6:6 g6 e6 c6 g6:12 | "
        "a6:3 g6 f#6 e6 d#6:6 f#6 b5:24 | ")
BASS_SOLO = line(gallop, "e3 c3 d3 e3 c3 b2")

# the riff to close, and the last gallop up to the high E
OUTRO = (R1 + R2
         + "e5:6 e5:3 e5 g5:6 g5:3 g5 b5:6 b5:3 b5 d6:6 d#6 | "
         + "e6:36 r:12 | ")
BASS_OUTRO = BASS_R1 + BASS_R2 + gal("e3", "g3", "b3", "b3") + "e3:36 r:12 | "

D_FILL = "k:6 k:3 k s:6 k:3 k s:3 s s s s s s s | "
D_LIGHT = "k:6 k:3 k s:12 k:6 k:3 k s:12 | "    # under the two guitars

# the riff alone for two bars, then the band comes in
MELODY = (R1
          + R1 + R2
          + VERSE
          + CHORUS
          + TWIN
          + SOLO
          + OUTRO)

BASS = (rest(2)
        + BASS_R1 + BASS_R2
        + BASS_VERSE
        + BASS_CHORUS
        + TWIN_LOW
        + BASS_SOLO
        + BASS_OUTRO)

DRUMS = (rest(2)
         + GALLOP * 3 + D_FILL
         + GALLOP * 5 + D_FILL
         + GALLOP * 7 + D_FILL
         + D_LIGHT * 4
         + GALLOP * 5 + D_FILL
         + GALLOP * 3 + D_FILL
         + GALLOP + LAST)
