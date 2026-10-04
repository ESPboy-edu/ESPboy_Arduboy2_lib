# Undefined: our own, an alt metal track written for the game. E minor, 104
# to the beat. The hook is a piano line, single notes in octaves 5 and 6:
# each bar flicks from a root up to its octave, falls through the chord and
# steps on to the next root, over Em, Am, G and B. It plays alone, then the
# bass and drums come in under its second half. The verses are a heavy riff,
# low E chugged in pairs and answered by G, A and B, with the bass on it. The
# hook comes back with the band as the way into the chorus, and the chorus
# lifts on C: E, G and a long high B, then D, Em; everything drops out but
# the hook (now with broken chords), and the chorus comes back to end on a
# long E. About fifty-five seconds.
#
# The tune never goes below E5 and the bass never above B3, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "UNDEFINED"
BY = "LINKER PARK"
STYLE = "Alt metal"
KEY = "E minor"
PREVIEW = 13                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 104)]

# the piano hook: Em, Am, G, B (the second half walks on in eighths)
H1 = "e5:9 e6:3 r:6 d6:6 b5:12 g5:6 a5:6 | "
H2 = "a5:9 a6:3 r:6 g6:6 e6:12 c6:6 d6:6 | "
H3 = "g5:9 g6:3 r:6 f#6:6 d6:6 b5 c6 d6 | "
H4 = "b5:9 b6:3 r:6 a6:6 f#6:6 d#6 e6 f#6 | "
HOOK = H1 + H2 + H3 + H4
# alone in the bridge, the long notes broken into the chord
BRIDGE = ("e5:9 e6:3 r:6 d6:6 b5:6 g5:3 b5 g5:6 a5 | "
          "a5:9 a6:3 r:6 g6:6 e6:6 c6:3 e6 c6:6 d6 | "
          "g5:9 g6:3 r:6 f#6:6 d6:6 b5:3 d6 b5:6 c6 | "
          "b5:9 b6:3 r:6 a6:6 f#6:6 d#6:3 f#6 a6:6 b6 | ")

# the verse riff: low E in pairs, answered higher each time; the last bar
# climbs over the low E
V1 = "e5:3 e5 r g5 e5 e5 r a5 e5 e5 r b5 a5 r g5 e5 | "
V2 = "e5:3 e5 r g5 e5 e5 r a5 e5 e5 r d6 c6 r b5 a5 | "
V3 = "e5:3 e5 r g5 e5 e5 r a5 e5 b5 r e5 c6 e5 d6 e5 | "
VERSE = V1 + V2 + V1 + V2 + V1 + V3

# C, D, Em: up the chord to a long high B
CHORUS = ("e6:6 g6 b6:36 | "
          "a6:12 f#6:6 g6 a6:12 d6:12 | "
          "e6:6 g6 b6:24 a6:6 g6 | "
          "f#6:12 e6:24 b5:6 d6 | ")
# the last time C, D, B and a long E
OUTRO = ("e6:6 g6 b6:36 | "
         "a6:12 f#6:6 g6 a6:12 d6:12 | "
         "d#6:6 f#6 b6:24 a6:12 | "
         "g6:6 f#6 e6:36 | ")

# the bass plays the verse riff two octaves down; elsewhere the roots
B1 = "e3:3 e3 r g3 e3 e3 r a3 e3 e3 r b3 a3 r g3 e3 | "
B2 = "e3:3 e3 r g3 e3 e3 r a3 e3 e3 r d3 c3 r b2 a2 | "
B3 = "e3:3 e3 r g3 e3 e3 r a3 e3 b3 r e3 c3 e3 d3 e3 | "
BASS_VERSE = B1 + B2 + B1 + B2 + B1 + B3

D_BUILD = "k:24 s:24 | "
D_BUILD_FILL = "k:12 s s:3 s s s s:6 s | "
D_RIFF = "k:3 k r r s k r r k k r r s r k r | "      # the kick with the pairs
D_LIFT = "k:6 k s k k k s k | "

# the hook alone for two bars, then bass and drums under its second half
MELODY = (HOOK
          + VERSE
          + H3 + H4                              # the hook again, with the band
          + CHORUS
          + BRIDGE
          + OUTRO)

BASS = (rest(2) + "g3:48 | b2:48 | "
        + BASS_VERSE
        + line(eighths, "g3 b2")
        + line(eighths, "c3 d3 e3 e3")
        + rest(4)
        + line(eighths, "c3 d3 b2") + "e3:48 | ")

DRUMS = (rest(2) + D_BUILD + D_BUILD_FILL
         + D_RIFF * 5 + FILL
         + D_LIFT + FILL
         + beat(ROCK, 4, FILL)
         + rest(4)
         + ROCK * 3 + LAST)
