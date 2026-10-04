# Overdrive: our own, a hard rock track written for the game. E minor, 144 to
# the beat: a riff on the low strings with the blue note in it, a verse on
# the riff, a chorus that climbs with long notes to hold, a solo in the
# pentatonic with a fast run at the end, and the riff again to close with
# three stabs. About fifty-five seconds.
#
# The tune never goes below E5 and the bass never above D4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

TITLE = "OVERDRIVE"
BY = "THE SQUARE WAVES"
STYLE = "Hard rock"
KEY = "E minor"
PREVIEW = 13                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 144)]

# the riff, two bars, and its two turns
R1 = ("e5:6 e5 r e5 g5:12 a5:12 | "
      "bb5:6 a5 g5:12 e5:24 | ")
R2 = ("e5:6 e5 r e5 g5:12 a5:12 | "
      "c6:6 b5 a5:12 g5:12 e5:12 | ")
R3 = ("e5:6 e5 r e5 g5:12 a5:12 | "
      "d6:6 c6 b5 a5 g5:12 f#5:12 | ")

CHORUS = ("e6:18 d6:6 b5:24 | "
          "c6:18 b5:6 a5:24 | "
          "g5:12 a5 b5 d6 | "
          "e6:36 r:12 | "
          "e6:18 d6:6 b5:24 | "
          "c6:18 b5:6 a5:24 | "
          "g5:12 a5 b5 d6 | "
          "f#6:12 e6 d6 b5 | ")

SOLO = ("e6:3 g6 e6 d6 b5:6 d6 e6:12 g6:6 e6 | "
        "d6:6 b5 a5 b5 g5:12 e5:12 | "
        "a5:3 b5:9 d6:6 e6 g6 e6 d6 b5 | "
        "a5:6 g5 e5:12 g5:6 a5 b5:12 | "
        "e6:3 g6 e6 d6 b5:6 d6 e6:12 g6:6 e6 | "
        "a6:6 g6 e6 d6 b5:12 d6:12 | "
        "e6:3 d6 b5 a5 g5 a5 b5 d6 e6:6 d6 b5 a5 | "
        "g5:6 a5 b5:12 d6:12 e6:12 | ")

OUTRO = (R1
         + "e5:6 e5 r e5 g5:12 a5:12 | "
         + "bb5:6 a5 g5 a5 bb5 b5 e6:12 | "
         + "e6:12 r:12 e6:12 r:12 | "
         + "e6:36 r:12 | ")

# the bass under the riff follows it; under the chorus it is the roots of
# E minor, C, G and D with the fifth on every fourth eighth
B1 = "e3:6 e3 e3 e3 g3 g3 a3 a3 | bb3:6 a3 g3 g3 e3 e3 e3 e3 | "
B2 = "e3:6 e3 e3 e3 g3 g3 a3 a3 | c4:6 b3 a3 a3 g3 g3 e3 e3 | "
B3 = "e3:6 e3 e3 e3 g3 g3 a3 a3 | d4:6 c4 b3 a3 g3 g3 f#3 f#3 | "
BASS_CHORUS = ("e3:6 e3 e3 b3 e3 e3 e3 b3 | c4:6 c4 c4 g3 c4 c4 c4 g3 | "
               "g3:6 g3 g3 d4 g3 g3 g3 d4 | d4:6 d4 d4 a3 d4 d4 d4 a3 | ") * 2
BASS_OUTRO = (B1
              + "e3:6 e3 e3 e3 g3 g3 a3 a3 | bb3:6 a3 g3 a3 bb3 b3 e3 e3 | "
              + "e3:12 r:12 e3:12 r:12 | "
              + "e3:36 r:12 | ")

D_BEAT = "k:6 r s r k k s r | "
D_FILL = "k:6 r s r k r s:3 s s s | "
D_BIG = "k:12 s k s | "
D_STABS = "k:12 r k r | "
D_LAST = "k:6 s:42 | "

# the riff alone for two bars, then the band comes in
MELODY = (R1 + R2                                # intro
          + R1 + R2 + R1 + R3                    # verse
          + CHORUS
          + SOLO
          + OUTRO)

BASS = ("r:48 | r:48 | " + B2
        + B1 + B2 + B1 + B3
        + BASS_CHORUS
        + B1 + B2 + B1 + B3
        + BASS_OUTRO)

DRUMS = ("r:48 | r:48 | " + D_BEAT + D_FILL
         + D_BEAT * 3 + D_FILL + D_BEAT * 3 + D_FILL
         + D_BIG * 3 + D_FILL + D_BIG * 3 + D_FILL
         + D_BEAT * 3 + D_FILL + D_BEAT * 3 + D_FILL
         + D_BEAT * 3 + D_FILL + D_STABS + D_LAST)
