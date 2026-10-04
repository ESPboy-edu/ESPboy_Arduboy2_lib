# No Brakes: our own, a speed rock and roll track written for the game. A,
# 210 to the beat: rock and roll played as fast as metal. The hook is the
# riff: A, A, A, then C sliding up to C-sharp and a long E, answered by D, C,
# A, G and home on A. The bass drives eighths all the way (it is half the
# song: a boogie line under the verse, runs on its own in the stop-time
# bars). A verse shouted on repeated notes, a gang chorus of long notes, the
# riff, a short wild solo, stop-time bars where the bass answers the band,
# the chorus again, the riff, and two stabs to end it dead. About fifty-five
# seconds.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "NO BRAKES"
BY = "MOTHERBOARD"
STYLE = "Speed rock and roll"
KEY = "A"
BEATS = 4

TEMPO = [(0, 210)]

# the riff, two bars, and its turn
R1 = ("a5:6 a5 r a5 c6 c#6 e6:12 | "
      "d6:6 c6 a5 g5 a5:12 r | ")
R2 = ("a5:6 a5 r a5 c6 c#6 e6:12 | "
      "g6:6 e6 d6 c6 a5:12 r | ")
BASS_R1 = "a2:6 a2 a3 a2 a2 a2 a3 a2 | d3:6 d3 d4 d3 a2 a2 c3 c#3 | "
BASS_R2 = "a2:6 a2 a3 a2 a2 a2 a3 a2 | g3:6 g3 d4 g3 a2 a2 c3 c#3 | "

# the verse, shouted on repeated notes: A, A, D, A, E, D, A, E
VERSE = ("e6:6 e6 e6 c#6 e6:12 r | "
         "e6:6 e6 g6 e6 c#6:12 a5 | "
         "f#6:6 f#6 f#6 d6 f#6:12 r | "
         "e6:6 e6 c#6 a5 c#6:12 r | "
         "b5:6 b5 b5 g#5 b5:12 d6 | "
         "a6:6 a6 f#6 d6 f#6:12 r | "
         "e6:6 e6 e6 c#6 a5:12 c6 | "
         "b5:6 d6 e6 g#6 b6:12 r | ")
BASS_VERSE = line(boogie, "a2 a2 d3 a2 e3 d3 a2 e3")

# the gang chorus, long notes: A, D, A, E, A, D, E, A
CHORUS = ("e6:24 c#6 | "
          "f#6:6 f#6 f#6 f#6 e6:12 d6 | "
          "e6:24 c#6 | "
          "g#6:6 g#6 g#6 g#6 e6:12 b5 | "
          "e6:24 c#6 | "
          "f#6:24 a6 | "
          "g#6:24 b6 | "
          "a6:24 e6:12 r | ")
BASS_CHORUS = line(octaves, "a2 d3 a2 e3 a2 d3 e3 a2")

# the solo, short and wild
SOLO = ("c6:3 c#6:9 e6:6 e6 e6 g6:3 a6:9 e6:6 | "
        "a6:3 a6 a6 a6 g6:6 e6 r d6 c6 a5 | "
        "e6:6 a6 e6 c6 a5 c6 e6 a6 | "
        "c7:12 a6:6 e6 c#6 a5 e5:12 | ")
BASS_SOLO = line(eighths, "a2 a2 d3 a2")

# stop time: the band hits, the bass answers
STOP = ("e6:6 e6 e6 e6 r:24 | "
        "g6:6 g6 g6 e6 r:24 | "
        "e6:6 e6 e6 e6 r:24 | "
        "g6:6 a6 g6 e6 d6 c6 a5 g5 | ")
BASS_STOP = ("a2:24 a2:6 c3 c#3 e3 | "
          "a2:24 g3:6 e3 d3 c#3 | "
          "a2:24 a2:6 c3 c#3 e3 | "
          "a2:6 a2 a2 a2 a2 a2 e3 e3 | ")
D_STOP = "k:6 s k s r:24 | "

# two stabs, and nothing
END = "a5:6 r:18 a5:6 r:18 | a6:6 r:42 | "
BASS_END = "a2:6 r:18 a2:6 r:18 | a2:6 r:42 | "

# the riff with the bass for two bars, then the drums come in
MELODY = (R1
          + R1 + R2
          + VERSE
          + CHORUS
          + R1 + R2
          + SOLO
          + STOP
          + CHORUS
          + R1 + R2
          + END)

BASS = (BASS_R1
        + BASS_R1 + BASS_R2
        + BASS_VERSE
        + BASS_CHORUS
        + BASS_R1 + BASS_R2
        + BASS_SOLO
        + BASS_STOP
        + BASS_CHORUS
        + BASS_R1 + BASS_R2
        + BASS_END)

DRUMS = (rest(2)
         + PUNK * 3 + FILL2
         + beat(PUNK, 8, FILL2)
         + beat(DRIVE, 8, FILL)
         + PUNK * 3 + FILL2
         + PUNK * 3 + ROLL
         + D_STOP * 3 + FILL2
         + beat(DRIVE, 8, FILL)
         + PUNK * 3 + FILL2
         + STABS + "k:6 r:42 | ")
