# High Score: our own, an indie rock track written for the game. B minor,
# 168 to the beat. The hook is a wiry riff in two halves: a jumpy bar of
# eighths with quick pickups from the A and a flick up to the high B, then
# two quick climbs up the chord like a coin going into an arcade machine,
# the band stopping dead behind each. The riff alone for two bars, then the
# band; a fast-talking verse, a chorus that drives over pushing eighths,
# the riff with longer stops, a stop-and-start break of climbs, the chorus
# again and the riff to end on a coin and a held B. About fifty-six
# seconds.
#
# The tune never goes below F sharp 5 and the bass never above E4, so the
# two cannot meet on one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

from kit import *

TITLE = "HIGH SCORE"
BY = "ARCADE MONKEYS"
STYLE = "Indie rock"
KEY = "B minor"
BEATS = 4

TEMPO = [(0, 168)]

# the riff: the jumpy bar, then the two coins (B, then A); the turn ends on
# a coin in G and a push up to F sharp; the stop leaves three beats of silence
RA = "b5:6 r:3 a5:3 d6:6 b5:6 r:3 a5:3 d6:3 b6:3 a6:6 f#6:6 | "
RB = "b5:3 d6:3 f#6:3 b6:3 r:12 a5:3 c#6:3 e6:3 a6:3 r:12 | "
RC = "b5:3 d6:3 f#6:3 b6:3 r:12 g5:3 b5:3 d6:3 g6:3 r:6 f#6:6 | "
RS = "b5:3 d6:3 f#6:3 b6:3 r:36 | "
RIFF = RA + RB
TURN = RA + RC

VERSE = ("f#6:6 f#6 e6 d6 e6 d6 b5 r | "
         "r:6 b5:6 d6 e6 f#6:12 d6:12 | "
         "g6:6 g6 f#6 e6 f#6 e6 d6 r | "
         "r:6 c#6:6 d6 e6 a6:24 | "
         "f#6:6 f#6 e6 d6 e6 d6 b5 r | "
         "r:6 b5:6 d6 e6 f#6:12 b6:12 | "
         "b6:6 a6 g6 f#6 g6 f#6 e6 r | "
         "r:6 c#6:6 e6 f#6 a#6:24 | ")

CHORUS1 = ("b6:18 a6:6 g6:12 f#6:12 | "
           "e6:24 r:12 c#6:6 e6:6 | "
           "f#6:18 e6:6 d6:12 b5:12 | "
           "d6:36 r:12 | ")
CHORUS2 = ("b6:18 a6:6 g6:12 b6:12 | "
           "a6:24 b6:12 a6:12 | "
           "b6:24 a#6:24 | "
           "f#6:36 r:12 | ")
CHORUS = CHORUS1 + CHORUS2

# stop and start: a climb, then nothing; then a run up the scale into the
# chorus
BREAK = ("b5:3 d6:3 f#6:3 b6:3 r:36 | "
         "a5:3 c#6:3 e6:3 a6:3 r:36 | "
         "g5:3 b5:3 d6:3 g6:3 r:12 f#5:3 a#5:3 c#6:3 f#6:3 r:12 | "
         "f#5:3 g5:3 a#5:3 b5:3 c#6:3 d6:3 e6:3 f#6:3 r:24 | ")

END = "b5:3 d6:3 f#6:3 b6:3 r:12 f#6:6 r:6 b6:12 | b6:36 r:12 | "

# the bass: octaves under the jumpy bar, a hit with each coin
BA = line(octaves, "b2")
BB = "b2:6 r:18 a2:6 r:18 | "
BC = "b2:6 r:18 g3:6 r:6 f#3:6 r:6 | "
BS = "b2:6 r:42 | "
BASS_RIFF = BA + BB
BASS_TURN = BA + BC
BASS_VERSE = (line(eighths, "b3 b3 g3 a3 b3 b3 g3")
              + "f#3:6 f#3 f#3 f#3 f#3 f#3 a#3 a#3 | ")
BASS_CHORUS1 = line(eighths, "g3 a3 b3") + "b3:6 b3 b3 b3 a3 a3 f#3 f#3 | "
BASS_CHORUS2 = (line(eighths, "g3 a3 f#3")
                + "f#3:6 f#3 f#3 f#3 f#3 f#3 a#3 a#3 | ")
BASS_CHORUS = BASS_CHORUS1 + BASS_CHORUS2
BASS_BREAK = ("b2:6 r:42 | a2:6 r:42 | g3:6 r:18 f#3:6 r:18 | "
              "b2:6 r:42 | ")
BASS_END = "b2:6 r:18 f#3:6 r:6 b2:12 | b2:36 r:12 | "

D_BEAT = "k:6 r s r k k s r | "
D_DRIVE = "k:6 k s r k k s k | "
D_FILL = "k:6 r s r k r s:3 s s s | "
D_COIN = "k:6 r:18 s:6 r:18 | "            # a hit with each coin
D_TURN = "k:6 r:18 s:6 r:6 k:3 k s:6 | "
D_STOP = "k:6 r:42 | "
D_BREAK3 = "k:6 r:18 s:6 r:18 | "
D_BREAK4 = "r:24 s:3 s s s s:6 s | "

# the riff alone for two bars, then the band comes in
MELODY = (RIFF
          + RIFF + RIFF + TURN
          + VERSE
          + CHORUS
          + RA + RS + RA + RC
          + BREAK
          + CHORUS1
          + RA + END)

BASS = ("r:48 | r:48 | "
        + BASS_RIFF + BASS_RIFF + BASS_TURN
        + BASS_VERSE
        + BASS_CHORUS
        + BA + BS + BA + BC
        + BASS_BREAK
        + BASS_CHORUS1
        + BA + BASS_END)

DRUMS = ("r:48 | r:48 | "
         + D_BEAT + D_COIN + D_BEAT + D_COIN + D_BEAT + D_TURN
         + beat(D_BEAT, 8, D_FILL)
         + beat(D_DRIVE, 8, D_FILL)
         + D_BEAT + D_STOP + D_BEAT + D_TURN
         + D_STOP + D_STOP + D_BREAK3 + D_BREAK4
         + D_DRIVE * 3 + D_FILL
         + D_BEAT + D_COIN + LAST)
