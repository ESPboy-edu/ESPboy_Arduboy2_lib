# Hot Fingers: our own, a shred hard rock track written for the game. E,
# 136 to the beat. It opens with a tapping cadenza, the guitar alone over
# long bass notes: triplets that go high, low, middle (tap, pull off,
# hammer on) across three chord shapes, E, A and B, each bar ending on the
# tapped note left to ring, and a run up into the band. Then the riff, the
# hook: a party-rock chug on the low E with quick B-C# hammer-ons and a
# turn down from D; a shouted chorus over A, B, C#m A and B; a tapping
# solo where the tapped note walks a tune over C#m, A and F#m, with
# sixteenth runs; the chorus again, the riff, and a last tapped flourish
# down to a long E. Never more than eight sixteenths in a row. About
# fifty-three seconds.
#
# The tune never goes below E5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "HOT FINGERS"
BY = "VAN HALOGEN"
STYLE = "Shred hard rock"
KEY = "E"
PREVIEW = 5                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 136)]


def tap(top, low, mid, times=3):
    """triplets: the tapped note, the pull-off and the hammer-on"""
    return " ".join("%s:4 %s %s" % (top, low, mid) for _ in range(times)) + " "


def shape(top, low, mid):
    """a bar of tapping: three beats of it, and the tapped note left to ring"""
    return tap(top, low, mid) + "%s:12 | " % top


# the cadenza: three chord shapes, then a run up into the band
CADENZA = (shape("g#6", "b5", "e6")
           + shape("a6", "c#6", "e6")
           + shape("b6", "d#6", "f#6")
           + "b5:3 d#6 f#6 a6 b6 a6 f#6 d#6 e6:24 | ")

# the riff: the low E with hammer-ons over it, and a turn down from D
R1 = "e5:6 e5 b5:3 c#6 e5:6 e5 b5:3 c#6 d6 c#6 b5:6 | "
R2 = "e5:6 e5 b5:3 c#6 e5:6 d6:6 c#6:3 b5 a5:6 b5 | "
R3 = "e5:6 e5 b5:3 c#6 e5:6 g5:6 a5:3 b5 d6:6 e6 | "
RIFF = R1 + R2

# the chorus: shouted, a leap up each bar, and a run down into the riff
CHORUS = ("e6:6 e6 c#6 e6 a6:12 e6 | "
          "f#6:6 f#6 d#6 f#6 b6:12 f#6 | "
          "g#6:6 g#6 e6 g#6 a6:6 a6 e6 a6 | "
          "b6:18 a6:6 g#6 f#6 e6 d#6 | ")

# the solo: the tapped note walks a tune; sixteenth runs in between
SOLO = ("g#6:4 c#6 e6 g#6 c#6 e6 a6 c#6 e6 g#6:12 | "
        "e6:4 a5 c#6 e6 a5 c#6 f#6 a5 c#6 e6:12 | "
        "f#6:4 a5 c#6 f#6 a5 c#6 g#6 a5 c#6 a6:12 | "
        "b6:3 f#6 d#6 b5 d#6 f#6 a6 b6 c7:6 b6:18 | "
        "g#6:3 e6 b5 e6 g#6 b6 g#6 e6 c#6:6 e6 a6 c#6 | "
        "b5:3 d#6 f#6 b6 a6 f#6 d#6 b5 e6:6 f#6 g#6 b6 | ")

END = (tap("g#6", "b5", "e6", 2) + "b6:3 g#6 e6 b5 e6:12 | "
       + "e6:6 r e6 r e6:24 | ")

B_CADENZA = "e3:48 | a2:48 | b2:48 | b2:24 b2:6 b2 b2 b2 | "
B_RIFF = line(eighths, "e3 e3")
B_CHORUS = (line(eighths, "a2 b2")
            + "c#3:6 c#3 c#3 c#3 a2 a2 a2 a2 | "
            + eighths("b2"))
B_SOLO = line(eighths, "c#3 a2 f#3 b2 e3 b2")
B_END = eighths("e3") + "e3:6 r e3 r e3:24 | "

D_RUN = "r:24 s:3 s s s s s s s | "     # a snare roll into the band
D_END = FILL + "k:6 r k r k:24 | "

# the cadenza alone over long bass notes, then the band comes in on the riff
MELODY = (CADENZA
          + RIFF + R1 + R3
          + CHORUS
          + RIFF
          + SOLO
          + CHORUS
          + RIFF + R1 + R3
          + END)

BASS = (B_CADENZA
        + B_RIFF * 2
        + B_CHORUS
        + B_RIFF
        + B_SOLO
        + B_CHORUS
        + B_RIFF * 2
        + B_END)

DRUMS = (rest(3) + D_RUN
         + beat(DRIVE, 4, FILL)
         + beat(BACKBEAT, 4, FILL)
         + DRIVE + FILL
         + beat(DRIVE, 6, FILL, every=3)
         + beat(BACKBEAT, 4, FILL)
         + beat(DRIVE, 4, FILL)
         + D_END)
