# Stage Dive: our own, a heavier track written for the game. A minor, 120 to
# the beat: a riff with a slide up to the E in it over a galloping bass, a
# chorus of long notes over A minor, F, C and G, a breakdown of stabs at
# half time, and the riff to close on a high A. About fifty-five seconds.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation.

TITLE = "STAGE DIVE"
BY = "PIEZO PRIEST"
STYLE = "Heavy metal"
KEY = "A minor"
PREVIEW = 11                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 120)]

H1 = "a5:6 a5 c6 a5 d6 eb6:3 e6:9 r:6 | "
H2 = "g5:6 a5:12 g5:6 e5:12 a5:6 g5 | "
T2 = "g6:6 e6 d6:12 c6:6 b5 a5:12 | "
RIFF = H1 + H2
TURN = H1 + T2

CHORUS = ("a5:24 c6:12 e6:12 | "
          "f6:36 e6:12 | "
          "e6:12 d6 c6:24 | "
          "d6:36 r:12 | "
          "a5:24 c6:12 e6:12 | "
          "f6:24 g6:12 a6:12 | "
          "g6:12 e6 c6:24 | "
          "b5:12 d6 g5 r | ")

BREAK = ("a5:6 r a5 r r a5 r a5 | "
         "a5:6 r c6 r d6:12 eb6:6 e6 | "
         "a5:6 r a5 r r a5 r a5 | "
         "g5:6 g5 r g5 e5:12 a5:12 | ")

OUTRO = H1 + H2 + H1 + "a6:12 r a6 r | a6:36 r:12 | "


def gallop(*roots):
    """a bar of eighth-sixteenth-sixteenth on a root a beat (or one for the bar)"""
    if len(roots) == 1:
        roots = roots * 4
    return " ".join("%s:6 %s:3 %s:3" % (r, r, r) for r in roots) + " | "


BASS_RIFF = gallop("a3") + gallop("g3", "a3", "e3", "a3")
BASS_TURN = gallop("a3") + gallop("g3", "d4", "c4", "a3")
BASS_CHORUS = ("a3:6 a3 a3 e4 a3 a3 a3 e4 | f3:6 f3 f3 c4 f3 f3 f3 c4 | "
               "c4:6 c4 c4 g3 c4 c4 c4 g3 | g3:6 g3 g3 d4 g3 g3 g3 d4 | ") * 2
BASS_BREAK = ("a3:6 r a3 r r a3 r a3 | "
              "a3:6 r c4 r d4:12 eb4:6 e4 | "
              "a3:6 r a3 r r a3 r a3 | "
              "g3:6 g3 r g3 e3:12 a3:12 | ")
BASS_OUTRO = gallop("a3") + gallop("g3", "a3", "e3", "a3") + gallop("a3") + "a3:12 r a3 r | a3:36 r:12 | "

D_BEAT = "k:6 k s r k r s r | "
D_FILL = "k:6 k s r k r s:3 s s s | "
D_HALF = "k:6 r k r s:12 r:12 | "
D_ROLL = "k:6 r k r s:3 s s s s:6 s | "
D_STABS = "k:12 r k r | "
D_LAST = "k:6 s:42 | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF + TURN + RIFF + TURN
          + CHORUS
          + BREAK
          + OUTRO)

BASS = ("r:48 | r:48 | "
        + BASS_RIFF + BASS_TURN + BASS_RIFF + BASS_TURN
        + BASS_CHORUS
        + BASS_BREAK
        + BASS_OUTRO)

DRUMS = ("r:48 | r:48 | "
         + D_BEAT * 3 + D_FILL + D_BEAT * 3 + D_FILL
         + D_BEAT * 3 + D_FILL + D_BEAT * 3 + D_FILL
         + D_HALF * 3 + D_ROLL
         + D_BEAT * 3 + D_STABS + D_LAST)
