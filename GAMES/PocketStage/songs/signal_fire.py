# Signal Fire: our own, an arena rock track written for the game. D major,
# 124 to the beat. The hook is a guitar through a delay, written out: two
# notes, and the same two again a dotted eighth later, so every half bar
# shimmers in a long-short lilt over a bass that pulses straight eighths.
# The delay alone for two bars, then the bass, then a heartbeat kick; the
# figure moves through D, G, B minor and A, cascades in threes as the snare
# builds, and opens into a chorus of long notes that soars to the high B;
# then it all drops back to the delay and the bass, builds again, and comes
# down on a last chorus up to the high C sharp and a held D. About
# fifty-four seconds.
#
# The tune never goes below A5 and the bass never above E4, so the two
# cannot meet on one pitch (tools/build_data.py says why they must not).
#
# See tools/build_data.py for the notation; the drums are k (kick), s
# (snare) and r, with lengths like the notes.

from kit import *

TITLE = "SIGNAL FIRE"
BY = "I2C"
STYLE = "Arena rock"
KEY = "D major"
PREVIEW = 11                # the setlist's preview starts at this bar
BEATS = 4

TEMPO = [(0, 124)]


def echo(*pairs):
    """the delay: for each pair, half a bar of the two notes as eighths and
    the same two again a dotted eighth later (long-short-long-longer); two
    pairs to a bar"""
    out = ""
    for i, (low, high) in enumerate(pairs):
        out += "%s:6 %s:3 %s:6 %s:9 " % (low, high, low, high)
        if i % 2:
            out += "| "
    return out


def cascade(*threes):
    """the delay on three eighths: they and their echoes fall in between one
    another; half a bar each, two to a bar"""
    out = ""
    for i, (a, b, c) in enumerate(threes):
        out += "%s:6 %s:3 %s:3 %s:3 %s:6 %s:3 " % (a, b, a, c, b, c)
        if i % 2:
            out += "| "
    return out


# the hook: a low A under a line that rises D, E, F sharp and falls back;
# the second time it lifts to the high A
HOOK1 = echo(("a5", "d6"), ("a5", "e6"), ("a5", "f#6"), ("a5", "e6"))
HOOK2 = echo(("a5", "d6"), ("a5", "e6"), ("a5", "f#6"), ("a5", "a6"))
# where it all drops back, the line falls instead
HOOK_DOWN = echo(("a5", "f#6"), ("a5", "e6"), ("a5", "d6"), ("a5", "e6"))

# the figure over D, G, B minor and A
VERSE = echo(("d6", "a6"), ("d6", "f#6"), ("d6", "b6"), ("d6", "g6"),
             ("d6", "f#6"), ("b5", "d6"), ("c#6", "e6"), ("c#6", "a6"))
# the build, over G and A: the echoes tumble in threes
CLIMB = cascade(("d6", "g6", "b6"), ("b5", "d6", "g6"),
                ("c#6", "e6", "a6"), ("e6", "a6", "b6"))

CHORUS_A = ("b6:24 a6:6 g6:6 a6:6 b6:6 | "
            "a6:36 f#6:6 e6:6 | ")
CHORUS_B = ("e6:12 f#6:12 a6:12 b6:12 | "
            "f#6:36 d6:6 e6:6 | ")
CHORUS_C = ("b6:24 a6:6 g6:6 a6:6 b6:6 | "
            "a6:24 b6:12 c#7:12 | ")
CHORUS_D = ("c#7:24 b6:12 a6:12 | "
            "a6:48 | ")
CHORUS = CHORUS_A + CHORUS_B + CHORUS_A + CHORUS_D
LAST_CHORUS = CHORUS_C + CHORUS_D

END = echo(("a5", "f#6"), ("a5", "e6")) + "d6:48 | "

# the bass: straight eighths on the roots, the pulse under the delay
BASS_HOOK = line(eighths, "d3 d3")
BASS_VERSE = line(eighths, "d3 g3 b2 a2")
BASS_CLIMB = line(eighths, "g3 a2")
BASS_CHORUS = line(eighths, "g3 d3 a2 b2 g3 d3 a2 d3")
BASS_LAST = line(eighths, "g3 d3 a2 d3")
BASS_END = line(eighths, "d3") + "d3:48 | "

D_HEART = "k:12 k k k | "                  # the kick alone on every beat
D_BUILD = "k:12 s k s | "
D_SNARES = "k:6 s s s s:3 s s s s s s s | "
D_BIG = "k:6 r s r k k s r | "
D_FILL = "k:6 r s r k r s:3 s s s | "
D_DROP = "k:6 r:42 | "

# the delay alone for two bars, then the bass, then the drums
MELODY = (HOOK1
          + HOOK2
          + VERSE
          + CLIMB
          + CHORUS
          + HOOK_DOWN
          + CLIMB
          + LAST_CHORUS
          + END)

BASS = ("r:48 | r:48 | " + BASS_HOOK
        + BASS_VERSE
        + BASS_CLIMB
        + BASS_CHORUS
        + BASS_HOOK
        + BASS_CLIMB
        + BASS_LAST
        + BASS_END)

DRUMS = ("r:48 | r:48 | r:48 | r:48 | "
         + D_HEART * 4
         + D_BUILD + D_SNARES
         + beat(D_BIG, 8, D_FILL)
         + D_DROP + "r:48 | "
         + D_HEART + D_SNARES
         + D_BIG * 3 + D_FILL
         + D_DROP + LAST)
