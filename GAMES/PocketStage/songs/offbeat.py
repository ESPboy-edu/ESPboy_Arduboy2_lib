# Offbeat: our own, a ska punk song written for the game. C major, 168 to
# the beat. The hook is a horn line off the beat: a stab on the "and" of
# one, a breath, then four quick notes, twice (over C and G). The verses are
# ska: the tune plays the upstroke, short stabs on every "and" that walk up
# and down the chords (C, A minor, D minor, G), while the bass walks in
# quarters under a plain backbeat. The chorus turns into fast punk (the punk
# beat, the bass in eighths) and shouts OFF-BEAT, OFF-BEAT, each one a long
# note to hold, then runs up to a high note. Back to ska with the hook, the
# chorus again, and a punk blast to end on three stabs. About fifty-six
# seconds.
#
# The tune never goes below B5 and the bass never above C4, so the two
# cannot meet on one pitch.

from kit import *

TITLE = "OFFBEAT"
BY = "OPCODE IVY"
STYLE = "Ska punk"
KEY = "C major"
BEATS = 4
TEMPO = [(0, 168)]


def skank(*notes):
    """a bar of short stabs on the off-beats, a note of the chord each"""
    return " ".join("r:6 %s:3 r:3" % n for n in notes) + " | "


# the hook: a stab off the beat, a breath, four quick notes (C, G)
HOOK = ("r:6 c7:6 r:12 g6:6 a6 g6 e6 | "
        "r:6 b6:6 r:12 g6:6 a6 g6 d6 | ")

# the verse in ska: stabs up and down the chords (C Am Dm G C Am Dm G);
# the fourth bar ends on a little horn line, the last runs up into the chorus
V1 = skank("c6", "e6", "g6", "e6")
V2 = skank("c6", "e6", "a6", "e6")
V3 = skank("d6", "f6", "a6", "f6")
V4 = "r:6 b5:3 r:3 r:6 d6:3 r:3 g6:6 a6 b6 g6 | "
V5 = skank("e6", "g6", "c7", "g6")
V6 = skank("e6", "a6", "c7", "a6")
V7 = skank("d6", "f6", "a6", "c7")
V8 = "r:6 b6:3 r:3 r:6 g6:3 r:3 d6:6 e6 f6 g6 | "
VERSE = V1 + V2 + V3 + V4 + V5 + V6 + V7 + V8
VERSE2 = HOOK + V1 + V2 + V3 + V4 + V7 + V8

# the chorus, fast punk: OFF-BEAT, OFF-BEAT, never on the one
# (F G C Am F G C C)
CHORUS = ("a6:18 g6:6 f6:18 e6:6 | "
          "d6:6 e6 f6 g6 b6:24 | "
          "g6:18 f6:6 e6:18 d6:6 | "
          "c6:6 d6 e6 g6 a6:24 | "
          "a6:18 g6:6 f6:18 e6:6 | "
          "d6:6 e6 f6 g6 b6:24 | "
          "c7:18 b6:6 a6:18 g6:6 | "
          "e6:6 f6 g6 b6 c7:24 | ")

# the punk blast: broken chords in eighths, then three stabs
BLAST_END = ("e6:6 g6 c7 g6 e6 g6 c7 g6 | "
             "f6:6 a6 c7 a6 g6 b6 d6 g6 | "
             "c7:6 r c7 r c7:24 | ")

# the bass: walking quarters in the ska, eighths in the punk
B_HOOK = "c3:12 e3 g3 e3 | g3:12 f3 e3 d3 | "
B_V = ["c3:12 e3 g3 g#3 | ",
       "a3:12 g3 e3 eb3 | ",
       "d3:12 f3 a3 f#3 | ",
       "g3:12 f3 e3 d3 | ",
       "c3:12 e3 g3 g#3 | ",
       "a3:12 g3 e3 eb3 | ",
       "d3:12 f3 a3 f#3 | ",
       "g3:6 g3 g3 g3 g3 g3 g3 g3 | "]
B_VERSE = "".join(B_V)
B_VERSE2 = B_HOOK + "".join(B_V[0:4]) + B_V[6] + B_V[7]
B_CHORUS = line(eighths, "f3 g3 c4 a3 f3 g3 c4 c4")
B_END = line(eighths, "c4") + "f3:6 f3 f3 f3 g3 g3 g3 g3 | " + "c4:6 r c4 r c4:24 | "

D_SKA_FILL = "k:12 s k s:3 s s s | "
D_TO_PUNK = "k:12 s s:6 s s:3 s s s | "
D_END = BLAST + PUNK + "k:6 r k r k:6 s:18 | "

# the hook alone for two bars, then the band comes in
MELODY = (HOOK
          + HOOK
          + VERSE
          + CHORUS
          + VERSE2
          + CHORUS
          + BLAST_END)

BASS = (rest(2) + B_HOOK
        + B_VERSE
        + B_CHORUS
        + B_VERSE2
        + B_CHORUS
        + B_END)

DRUMS = (rest(2) + BACKBEAT + D_SKA_FILL
         + BACKBEAT * 3 + D_SKA_FILL + BACKBEAT * 3 + D_TO_PUNK
         + beat(PUNK, 8, FILL, 8)
         + BACKBEAT * 3 + D_SKA_FILL + BACKBEAT * 3 + D_TO_PUNK
         + beat(PUNK, 8, FILL, 8)
         + D_END)
