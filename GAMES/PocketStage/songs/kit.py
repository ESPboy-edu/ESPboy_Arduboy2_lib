# Pocket Stage - the band's toolbox for writing songs.
#
# A song is text in the notation of tools/build_data.py. Most songs share
# their kind of bass line and their drum beats; these build them a bar at a
# time, so a song spells out only what is its own. Every function returns
# whole bars, each ending in "| ".
#
#   from kit import *
#
#   BASS = line(eighths, "e3 e3 c3 d3")      # a bar of eighths on each root
#   DRUMS = beat(ROCK, 8, FILL)              # eight bars of the rock beat, a fill every fourth
#
# Lengths are ticks, 12 to a beat: 6 an eighth, 3 a sixteenth, 4 a triplet
# eighth. The drums are k (kick), s (snare) and r (nothing); a drum takes the
# bass's pin for a few dozen milliseconds.

import re

TICKS = 12
_SEMI = {"c": 0, "c#": 1, "db": 1, "d": 2, "d#": 3, "eb": 3, "e": 4, "fb": 4, "e#": 5, "f": 5,
         "f#": 6, "gb": 6, "g": 7, "g#": 8, "ab": 8, "a": 9, "a#": 10, "bb": 10, "b": 11,
         "cb": -1, "b#": 12}
_NAMES = ["c", "c#", "d", "eb", "e", "f", "f#", "g", "ab", "a", "bb", "b"]
_NOTE = re.compile(r"^([a-g](?:#|b)?)(-?\d)$")


# ---------------------------------------------------------------- notes
def midi(n):
    """'e3' -> 52"""
    m = _NOTE.match(n)
    return 12 * (int(m.group(2)) + 1) + _SEMI[m.group(1)]


def note(m):
    """52 -> 'e3'"""
    return "%s%d" % (_NAMES[m % 12], m // 12 - 1)


def up(n, semis):
    """a note so many semitones higher (lower if negative)"""
    return note(midi(n) + semis)


def shift(text, semis):
    """a voice's text with every note moved by so many semitones (12 is an
    octave); rests, '-' and bar lines stay as they are"""
    out = []
    for tok in text.split():
        name, colon, dur = tok.partition(":")
        if _NOTE.match(name):
            name = up(name, semis)
        out.append(name + colon + dur)
    return " ".join(out) + " "


def rest(bars=1, beats=4):
    return ("r:%d | " % (TICKS * beats)) * bars


# ---------------------------------------------------------------- the bass
# One bar on a root, in a pattern. line() strings them together; a root of
# "r" there is a bar of rest.
def eighths(r, beats=4):
    return " ".join([r + ":6"] + [r] * (2 * beats - 1)) + " | "


def quarters(r, beats=4):
    return " ".join([r + ":12"] + [r] * (beats - 1)) + " | "


def halves(r, beats=4):
    return " ".join([r + ":24"] + [r] * (beats // 2 - 1)) + " | "


def whole(r, beats=4):
    return "%s:%d | " % (r, TICKS * beats)


def octaves(r, beats=4):
    """eighths, the root and its octave by turns"""
    o = up(r, 12)
    return " ".join([r + ":6", o] + [r, o] * (beats - 1)) + " | "


def fifths(r, beats=4):
    """eighths on the root with the fifth on every fourth one"""
    f = up(r, 7)
    return " ".join([r + ":6"] + ([r, r, f, r] * beats)[:2 * beats - 1]) + " | "


def gallop(r, beats=4):
    """an eighth and two sixteenths a beat"""
    return " ".join("%s:6 %s:3 %s:3" % (r, r, r) for _ in range(beats)) + " | "


def sixteenths(r, beats=4):
    return " ".join([r + ":3"] + [r] * (4 * beats - 1)) + " | "


def shuffle(r, beats=4):
    """the root on the beat and on the last triplet of it: long-short"""
    return " ".join("%s:8 %s:4" % (r, r) for _ in range(beats)) + " | "


def boogie(r, beats=4, swing=False):
    """the boogie line in eighths: root, third, fifth, sixth, seventh and back
    down (4/4)"""
    steps = [0, 4, 7, 9, 10, 9, 7, 4]
    if swing:
        return " ".join("%s:8 %s:4" % (up(r, steps[i]), up(r, steps[i + 1])) for i in range(0, 8, 2)) + " | "
    return " ".join([up(r, steps[0]) + ":6"] + [up(r, s) for s in steps[1:]]) + " | "


def walk(r, beats=4, minor=False):
    """quarters up the chord: root, third, fifth, sixth (minor: root, minor
    third, fourth, fifth)"""
    steps = [0, 3, 5, 7] if minor else [0, 4, 7, 9]
    return " ".join([up(r, steps[0]) + ":12"] + [up(r, s) for s in steps[1:beats]]) + " | "


def line(pattern, roots, beats=4):
    """a bar of the pattern on every root ('r' = a bar of rest)"""
    return "".join(rest(1, beats) if x == "r" else pattern(x, beats) for x in roots.split())


# ---------------------------------------------------------------- the drums
# One bar each, 4/4.
ROCK = "k:6 r s r k k s r | "            # kick on 1, 3 and the and of 3; snare on 2 and 4
BACKBEAT = "k:12 s k s | "               # kick on 1 and 3, snare on 2 and 4
DRIVE = "k:6 k s r k k s r | "           # the rock beat pushed harder
PUNK = "k:6 s k s k s k s | "            # a snare on every off-beat: fast punk
HALF = "k:12 r s r | "                   # half time: the snare on 3
STOMP = "k:6 k s:12 k:6 k s:12 | "       # kick kick snare
GALLOP = "k:6 k:3 k s:6 k:3 k k:6 k:3 k s:6 k:3 k | "     # metal: the kick gallops
DOUBLE = "k:3 k k k s k k k k k k k s k k k | "            # double kick sixteenths, snare on 2 and 4
DBEAT = "k:6 s k:3 k s:6 k:6 s k:3 k s:6 | "               # hardcore
BLAST = "k:3 s k s k s k s k s k s k s k s | "             # short bursts only: the bass is gone meanwhile
SHUFFLE = "k:8 r:4 s:8 k:4 k:8 r:4 s:8 r:4 | "             # triplet feel
FILL = "k:6 r s r k r s:3 s s s | "      # the rock beat with a run of sixteenths at the end
FILL2 = "k:6 r s r s:3 s s s s s s s | "  # half a bar of it
ROLL = "s:3 s s s s s s s s s s s s s s s | "             # a whole bar of sixteenths
TRIPLETS = "k:12 s s:4 s s s:4 s s | "   # a triplet fill
STABS = "k:12 r k r | "                  # hits on 1 and 3
LAST = "k:6 s:42 | "                     # the last bar: a kick and the snare left to ring


def beat(pattern, bars, fill=None, every=4):
    """so many bars of a pattern; with a fill, every every-th bar is the fill"""
    return "".join(fill if fill and (i + 1) % every == 0 else pattern for i in range(bars))
