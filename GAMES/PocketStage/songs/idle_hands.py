# Idle Hands: our own, a new wave rock track written for the game. E minor,
# 136 to the beat, the bass pumping octaves in eighths all the way. The
# riff is the hook and sneers: stabs with rests between them, an octave
# leap from the low E to the high one, then G, and an answer falling to B.
# A chanted verse that climbs a step a bar (Em, C, D); a chorus that rises
# a step every bar over C, D, Em and B; a short solo with an echo: the
# bass voice plays every note again a dotted eighth later, an octave down,
# like a delay; the chorus twice more and the riff to end on three stabs.
# About fifty-three seconds.
#
# The tune never goes below E5 and the bass never above E4 (but in the
# solo, where the echo stays an octave under the tune), so the two cannot
# meet on one pitch (tools/build_data.py says why they must not).

from kit import *

TITLE = "IDLE HANDS"
BY = "BILLY IDLE"
STYLE = "New wave rock"
KEY = "E minor"
BEATS = 4

TEMPO = [(0, 136)]

# the riff: stabs and rests, the octave leap, and the answer down to B
R1 = "e5:6 r e6 r d6 e6 r g6 | "
R2 = "r:6 g6 e6 r d6 r b5:12 | "
RIFF = R1 + R2

# the verse: a note twice, a leap and a fall, a step higher every bar
VERSE = ("b5:6 b5 r e6 r d6 b5 a5 | "
         "b5:6 b5 r e6 r g6 e6:12 | "
         "c6:6 c6 r e6 r g6 e6 c6 | "
         "d6:6 d6 r f#6 r a6 f#6 e6 | ")

# the chorus rises a step a bar; the second time it comes back down
RISE = ("e6:6 r e6 g6 e6:24 | "
        "f#6:6 r f#6 a6 f#6:24 | "
        "g6:6 r g6 b6 g6:24 | ")
CHORUS = RISE + "a6:6 r b6 c7 b6:24 | "
CHORUS2 = RISE + "b6:6 r a6 g6 e6:24 | "

# the solo: long notes, so that the echo can be heard
SOLO = ("b6:12 g6:6 e6 g6:12 a6 | "
        "c7:12 b6:6 g6 e6:24 | "
        "a6:12 f#6:6 a6 b6:12 a6 | "
        "f#6:12 a6:6 b6 c7:12 b6 | ")

END = R1 + "e6:6 r e6 r e6:24 | "


def echo(text, delay, semis, beats=4):
    """the line again, delay ticks later and moved by semis, cut off where
    the line ends: a delay pedal for the bass voice to play"""
    bar = TICKS * beats
    notes = []
    length = TICKS
    for tok in text.split():
        if tok == "|":
            continue
        name, _, dur = tok.partition(":")
        if dur:
            length = int(dur)
        if name == "-":
            notes[-1][1] += length
        else:
            notes.append([name if name == "r" else up(name, semis), length])
    total = sum(t for _, t in notes)
    out = []
    tick = 0
    for name, ticks in [["r", delay]] + notes:
        first = True
        while ticks and tick < total:
            part = min(ticks, bar - tick % bar, total - tick)
            out.append("%s:%d" % (name if first or name == "r" else "-", part))
            first = False
            ticks -= part
            tick += part
            if tick % bar == 0:
                out.append("|")
    return " ".join(out) + " "


B_RIFF = line(octaves, "e3 e3")
B_VERSE = line(octaves, "e3 e3 c3 d3")
B_CHORUS = line(octaves, "c3 d3 e3 b2")
B_CHORUS2 = line(octaves, "c3 d3 e3 e3")
B_SOLO = echo(SOLO, 9, -12)              # the echo, an octave under the tune
B_END = octaves("e3") + "e3:6 r e3 r e3:24 | "

D_END = DRIVE + "k:6 r k r k:24 | "

# the riff over the pumping bass for two bars, then the drums come in
MELODY = (RIFF
          + RIFF
          + VERSE
          + CHORUS
          + RIFF
          + SOLO
          + CHORUS + CHORUS2
          + RIFF
          + END)

BASS = (B_RIFF
        + B_RIFF
        + B_VERSE
        + B_CHORUS
        + B_RIFF
        + B_SOLO
        + B_CHORUS + B_CHORUS2
        + B_RIFF
        + B_END)

DRUMS = (rest(2)
         + DRIVE + FILL
         + beat(DRIVE, 4, FILL)
         + beat(DRIVE, 4, FILL)
         + DRIVE + FILL2
         + beat(DRIVE, 4, FILL)
         + beat(DRIVE, 8, FILL)
         + DRIVE + FILL
         + D_END)
