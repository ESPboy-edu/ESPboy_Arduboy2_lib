#!/usr/bin/env python3
"""
Pocket Stage - the songs, made into what the console reads.

  python3 tools/build_data.py     writes build/$OUT/fxdata.bin (the flash chip image),
                                  build/$OUT/fxsave.bin (the 4 KB the game may write to,
                                  empty), PocketStage/fxdata.h (where everything is) and
                                  PocketStage/tables.h (perspective, pitches, gem shapes)

A song is a Python file in songs/ (songs/setlist.py says which, in order):
a title, who wrote it, a tempo map and two voices written as text, one note
per token. songs/mountain_king.py is the example. The notation:

  b3 c#4 d4:6 r:12 -:6 |    b3 = the note B in octave 3 (c4 is middle C);
                            :6 = six ticks long, 12 ticks to a beat, and the
                            length carries on to the tokens after it; r = a
                            rest; - = the note before goes on; | ends a bar
                            (checked: a bar that does not add up is an error)

Voice A is the tune, the thing the player plays; voice B is the bass. The
two must never play the same pitch at the same time: on the console the two
square waves cancel each other out (the speaker hears their difference), and
the note goes silent. The tool stops with an error when they do.

A song may also have DRUMS, written the same way with k for a kick, s for a
snare and r for nothing: the drums are on the bass's pin, a drum takes it
for a few dozen milliseconds and the bass comes back by itself after.
songs/title_riff.py is the loop the title page and the menus play: voices
and drums, no chart, the lead-in taken off and the end padded to a whole
number of bars; or, with FROM = (song, first bar, bars), those bars of a song.

Every song also gets a preview, the loop the setlist plays while the song is
picked: PREVIEW_BARS bars from the bar named in the song's PREVIEW (counting
from 1; the chorus, usually), or from bar 3 when it names none.

The chart is made from voice A: on Expert every note; on Easy a note is
kept only when it is far enough in time from the notes already kept
(LEVELS below says how far), the notes on a beat first, so that the beats
stay and the notes between them go as the song gets faster. Medium and Hard
lie at even steps between the two, so that every level adds notes to the
one below whatever the song's tempo and rhythm: each has about the same
number of times more notes than the level below it. They add the notes on
the strongest places first (the bar, the half bar, the beat, the eighths
and triplets, the sixteenths; within a place the notes that start or end a
run before those in the middle of one, spread evenly through the song),
take a whole place when that lands near the step (LEVEL_SNAP), keep their
notes no closer than the level's gap where they can, and every step is at
least LEVEL_STEP.
Lanes follow the shape of the tune: within the last eight notes, low notes
go left, high notes right. A note on the first beat of a bar that is held
for a beat or more becomes a two-lane chord (not on Easy). A long note
becomes a hold, with a tail that ends a little before the note does and
well before the next note of the chart (HOLD_GAP_MS), so that no note ever
has to be played while another is still held (as on a real guitar, where
you would have to strum again); a note whose tail would be shorter than
HOLD_MIN_MS stays a plain note. The notes in two bars of every eight (the
third and fourth) are star notes: hitting them fills the star power.

What goes on the chip, all numbers little-endian:

  0x00   "PH", 1, the number of songs, u24 where the title riff is, 9 bytes
         of nothing
  0x10   one entry of 64 bytes per song: u24 music, u24 chart x4 (easy,
         medium, hard, expert), u24 text, u32 length in ms, u16 notes x4,
         u16 id (the saved scores are filed under it; made from the name
         and the charts, so a song whose chart changes starts without a
         best: a best is for the chart it was played on), u32 perfect score x4
         (every note hit, for the stars), u16 star notes x4, u24 preview (a
         music stream that loops)
  then   the streams and the texts (title NUL by NUL), the title riff, the
         play page's stage: every camera's frames, a screen of 1024 bytes
         each (tools/scene.py says in what order; where each camera starts
         goes into fxdata.h), the flames on the pads (tools/flames.py) and
         the booed-off page's title (tools/logo.py), both as sprites: a row
         of eight at a time, every column's image byte and then its mask
         byte; the crowd and the spotlights of the title page, the menu and
         the booed-off page (tools/crowd.py): frames of some of the screen
         buffer's pages, 128 bytes a page

  music  records of 2 bytes: ms since the record before, then a command:
         bit 7 = voice B, bits 0..6 = the note (MIDI number), 0 = the voice
         stops, on voice B 0x7C = a kick and 0x7D = a snare; 0xFF = nothing
         (a wait longer than 255 ms), 0xFE = the end
  chart  records of 2 or 3 bytes: ms since the record before, flags, and if
         bit 6 of the flags is set the hold length in units of 20 ms. Flags:
         bits 0..2 = lanes (0 = no note, just a beat), bit 3 = star note,
         bit 4 = the first beat of a bar, bit 5 = a beat; 0xFF = nothing,
         0xFE = the end
"""
import bisect
import importlib.util
import math
import os
import re
import struct
import sys
import zlib

import crowd
import flames
import logo
import scene

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
SKETCH = os.path.join(ROOT, "PocketStage")
SONGS = os.path.join(ROOT, "songs")
OUT = os.path.join(ROOT, "build", os.environ.get("OUT", "game"))

TICKS = 12                 # ticks per beat: eighths, triplets and sixteenths all fit
LEAD_MS = 2000             # the highway runs empty this long before the first beat
GATE_MS = 25               # a note stops this much before its end, so a repeated note strikes again
SAVE_BYTES = 4096
CHIP_BYTES = 0x1000000

# the highway (the program gets these through tables.h)
HORIZON_Y = 22             # where the far end is: the edge of the drum riser
HIT_Y = 56                 # where a note is when it is due
LOOK_MS = 1500             # how far ahead the highway shows
T0 = 600                   # perspective: scale = T0 / (T0 + time to go)
PERSP_MIN = -64            # the table starts this long after the hit line...
PERSP_STEP = 8             # ...in steps of this many ms...
PERSP_N = (LOOK_MS + 48 - PERSP_MIN) // PERSP_STEP   # ...and has this many entries
NEAR_LANE = 20             # lane spacing at the hit line
NEAR_EDGE = 30             # half the width of the highway at the hit line
# gems: rows of half widths, by distance (nearest first), and the scale
# (in 1/200) a gem has to be seen at to get each size
GEM_ROWS = [[4, 6, 7, 6, 4], [3, 5, 5, 3], [2, 4, 2], [2, 2], [1]]
GEM_LIMITS = [180, 130, 90, 60]
# the rock meter's dial in the bottom left corner: its centre and radius,
# dots on the arc every 22.5 degrees, and the left third of it solid

NOTE_LO, NOTE_HI = 24, 108  # the range of the pitch table (MIDI numbers)

# the levels: the least time between two notes of the chart, in ms (a note
# on a beat may come a little closer, 0.8 of it); 0 = every note. Easy keeps
# what fits its gap; Medium and Hard keep to theirs while they can, but their
# number of notes is set by the steps between Easy and Expert.
LEVELS = [("easy", 700), ("medium", 200), ("hard", 100), ("expert", 0)]
LEVEL_SNAP = 0.10          # Medium and Hard take a whole place (every beat, every eighth) this near their step
LEVEL_STEP = 1.15          # and every level has at least this many times the notes of the one below

# holds: the tail ends this long before the next note of the chart, and a
# note is a hold only when that leaves a tail this long
HOLD_GAP_MS = 200
HOLD_MIN_MS = 250

# the setlist's previews: so many bars of the song, round and round
PREVIEW_BARS = 8
PREVIEW_FIRST = 3          # (the first bar when a song names none: after the intro's two)

CMD_NOP, CMD_END = 0xFF, 0xFE
DRUM_KICK, DRUM_SNARE = 0x7C, 0x7D    # (0x7E and 0x7F would make the commands 0xFE and 0xFF)
FLAG_STAR, FLAG_BAR, FLAG_BEAT, FLAG_HOLD = 0x08, 0x10, 0x20, 0x40
TITLE_RIFF = "title_riff"

SEMI = {"c": 0, "c#": 1, "db": 1, "d": 2, "d#": 3, "eb": 3, "e": 4, "fb": 4, "e#": 5, "f": 5,
        "f#": 6, "gb": 6, "g": 7, "g#": 8, "ab": 8, "a": 9, "a#": 10, "bb": 10, "b": 11,
        "cb": -1, "b#": 12}
TOKEN = re.compile(r"^([a-g](?:#|b)?)(-?\d)(?::(\d+))?$")


# ---------------------------------------------------------------- the notation
def parse_voice(text, beats_per_bar, name):
    """tokens -> [[tick, length, midi], ...], total ticks"""
    notes = []
    tick = 0
    length = TICKS
    bar_ticks = beats_per_bar * TICKS
    bar = 1
    for tok in text.split():
        if tok == "|":
            if tick % bar_ticks:
                sys.exit("%s: bar %d does not add up (%d ticks into it at the bar line)"
                         % (name, bar, tick % bar_ticks))
            bar += 1
            continue
        m = TOKEN.match(tok)
        dur = None
        if m:
            note, octave, dur = m.groups()
            if dur:
                length = int(dur)
            notes.append([tick, length, 12 * (int(octave) + 1) + SEMI[note]])
            tick += length
        elif tok == "r" or tok.startswith("r:"):
            if tok != "r":
                length = int(tok[2:])
            tick += length
        elif tok == "-" or tok.startswith("-:"):
            if tok != "-":
                length = int(tok[2:])
            if not notes or notes[-1][0] + notes[-1][1] != tick:
                sys.exit("%s: bar %d: '-' with no note right before it" % (name, bar))
            notes[-1][1] += length
            tick += length
        else:
            sys.exit("%s: bar %d: what is %r?" % (name, bar, tok))
    return notes, tick


def parse_drums(text, beats_per_bar, name):
    """k, s and r tokens -> [[tick, kind], ...], total ticks"""
    hits = []
    tick = 0
    length = TICKS
    bar_ticks = beats_per_bar * TICKS
    bar = 1
    for tok in text.split():
        if tok == "|":
            if tick % bar_ticks:
                sys.exit("%s: bar %d does not add up (%d ticks into it at the bar line)"
                         % (name, bar, tick % bar_ticks))
            bar += 1
            continue
        kind, _, dur = tok.partition(":")
        if dur:
            length = int(dur)
        if kind == "k":
            hits.append([tick, DRUM_KICK])
        elif kind == "s":
            hits.append([tick, DRUM_SNARE])
        elif kind != "r":
            sys.exit("%s: bar %d: what is %r?" % (name, bar, tok))
        tick += length
    return hits, tick


def parse_song(mod, name):
    """a song module -> tune, bass, drums, ticks, beats per bar; checked"""
    beats = getattr(mod, "BEATS", 4)
    tune, ticks_a = parse_voice(mod.MELODY, beats, name + " melody")
    bass, ticks_b = parse_voice(mod.BASS, beats, name + " bass")
    drums, ticks_d = parse_drums(getattr(mod, "DRUMS", ""), beats, name + " drums")
    if ticks_a != ticks_b or (drums and ticks_d != ticks_a):
        sys.exit("%s: the tune is %d ticks, the bass %d, the drums %d" % (name, ticks_a, ticks_b, ticks_d))
    check_unison(tune, bass, name, beats)
    lo = min(n[2] for n in tune + bass)
    hi = max(n[2] for n in tune + bass)
    if lo < NOTE_LO or hi > NOTE_HI:
        sys.exit("%s: notes %d..%d are outside the pitch table %d..%d" % (name, lo, hi, NOTE_LO, NOTE_HI))
    return tune, bass, drums, ticks_a, beats


def tempo_at(tempo, bar):
    """beats per minute at a position in bars; a straight line between the
    entries of the tempo map, level outside them"""
    if bar <= tempo[0][0]:
        return tempo[0][1]
    for (b0, t0), (b1, t1) in zip(tempo, tempo[1:]):
        if b0 <= bar <= b1:
            return t0 + (t1 - t0) * (bar - b0) / (b1 - b0)
    return tempo[-1][1]


def tick_times(tempo, total_ticks, beats_per_bar):
    """the time in ms of every tick, 0 to total_ticks, the lead-in included"""
    times = [float(LEAD_MS)]
    t = float(LEAD_MS)
    for k in range(total_ticks):
        bpm = tempo_at(tempo, (k + 0.5) / TICKS / beats_per_bar)
        t += 60000.0 / bpm / TICKS
        times.append(t)
    return times


# ---------------------------------------------------------------- the streams
def with_waits(out, dt):
    while dt > 255:
        out += bytes([255, CMD_NOP])
        dt -= 255
    return dt


def music_stream(voices, times, drums=(), shift=0, end_ms=None):
    """the voices and the drums -> the records; shift takes the lead-in off
    (the title riff has none), end_ms puts the end record there"""
    events = []                                    # (ms, order, command)
    for v, notes in enumerate(voices):
        for tick, length, midi in notes:
            on, off = round(times[tick]) - shift, round(times[tick + length]) - shift
            gate = min(GATE_MS, (off - on) // 4)
            events.append((on, 1, (v << 7) | midi))
            events.append((off - gate, 0, (v << 7)))
    for tick, kind in drums:
        events.append((round(times[tick]) - shift, 2, 0x80 | kind))
    events.sort()
    out = bytearray()
    last = 0
    for ms, _, cmd in events:
        dt = with_waits(out, ms - last)
        out += bytes([dt, cmd])
        last = ms
    dt = with_waits(out, (end_ms if end_ms is not None else last) - last)
    out += bytes([dt, CMD_END])
    return out


def section_stream(tune, bass, drums, times, beats_per_bar, first, bars):
    """bars first .. first+bars-1 (counting from 1) as a music stream that
    starts at 0 and ends where the loop comes round; notes that run past the
    end are cut there -> (stream, length in ms)"""
    total = len(times) - 1
    bt = beats_per_bar * TICKS
    t0 = (first - 1) * bt
    t1 = min(total, t0 + bars * bt)
    if t0 >= t1:
        sys.exit("a section from bar %d is past the end of the song" % first)

    def cut(notes):
        return [[t, min(ln, t1 - t), m] for t, ln, m in notes if t0 <= t < t1]

    shift = round(times[t0])
    end = round(times[t1]) - shift
    return music_stream([cut(tune), cut(bass)], times, [d for d in drums if t0 <= d[0] < t1],
                        shift=shift, end_ms=end), end


def lanes_for(notes):
    """a lane for every note of the tune, following its shape"""
    lanes = []
    for i, (tick, length, midi) in enumerate(notes):
        recent = [n[2] for n in notes[max(0, i - 7):i + 1]]
        lo, hi = min(recent), max(recent)
        if hi - lo <= 2:
            prev = lanes[-1] if lanes else 1
            before = notes[i - 1][2] if i else midi
            lane = prev if midi == before else (min(2, prev + 1) if midi > before else max(0, prev - 1))
        else:
            lane = min(2, (midi - lo) * 3 // (hi - lo + 1))
        lanes.append(lane)
    return lanes


def pick_notes(tune, times, min_gap):
    """the notes far enough apart for the level: those on a beat first, then
    the others where there is room left"""
    if not min_gap:
        return list(tune)
    kept = []
    kept_ms = []
    for on_beat in (True, False):
        for n in tune:
            if (n[0] % TICKS == 0) != on_beat:
                continue
            ms = round(times[n[0]])
            need = min_gap * 0.8 if on_beat else min_gap
            if all(abs(ms - k) >= need for k in kept_ms):
                kept.append(n)
                kept_ms.append(ms)
    kept.sort(key=lambda n: n[0])
    return kept


def place(tick, beats_per_bar):
    """how strong a note's place in the bar is: 0 the bar, 1 the half bar,
    2 a beat, 3 an eighth or a triplet, 4 finer"""
    bar = beats_per_bar * TICKS
    if tick % bar == 0:
        return 0
    if beats_per_bar % 2 == 0 and tick % (bar // 2) == 0:
        return 1
    if tick % TICKS == 0:
        return 2
    if tick % (TICKS // 2) == 0 or tick % (TICKS // 3) == 0:
        return 3
    return 4


def spread(n):
    """0..n-1 in an order whose first k are spread evenly, for any k: by the
    index's bits read backwards (0, 1/2, 1/4, 3/4, 1/8 ...)"""
    def key(i):
        x, d = 0.0, 0.5
        while i:
            if i & 1:
                x += d
            i >>= 1
            d /= 2
        return x
    return sorted(range(n), key=lambda i: (key(i), i))


def note_order(tune, beats_per_bar):
    """the order in which Medium and Hard take the tune's notes (indices):
    the strongest places first; within a place the notes that start or end a
    run (the note before or after further off than the place's own step)
    before those in the middle of one; within those, spread through the song"""
    groups = {}
    for i, (tick, length, midi) in enumerate(tune):
        p = place(tick, beats_per_bar)
        if p <= 2:
            step = TICKS
        elif p == 3:
            step = TICKS // 2 if tick % (TICKS // 2) == 0 else TICKS // 3
        else:
            step = TICKS // 4
        before = tick - tune[i - 1][0] if i else step + 1
        after = tune[i + 1][0] - tick if i + 1 < len(tune) else step + 1
        middle = p >= 2 and before <= step and after <= step
        groups.setdefault((p, middle), []).append(i)
    order = []
    for key in sorted(groups):
        g = groups[key]
        order += [g[j] for j in spread(len(g))]
    return order


def level_notes(tune, times, beats_per_bar):
    """the notes of every level, in time order, each level holding the one
    below: Easy by its gap, Expert all of them, Medium and Hard at even steps
    between the two (see the top of this file)"""
    n = len(tune)
    easy = {id(t) for t in pick_notes(tune, times, LEVELS[0][1])}
    kept = [i for i, t in enumerate(tune) if id(t) in easy]
    levels = [kept]
    order = note_order(tune, beats_per_bar)
    for level in range(1, len(LEVELS) - 1):
        left = len(LEVELS) - level                 # the steps from the level below up to Expert
        below = len(kept)
        target = below * (n / below) ** (1.0 / left)
        lo = math.ceil(below * LEVEL_STEP)
        hi = math.floor(n / LEVEL_STEP ** (left - 1))
        gap = LEVELS[level][1]

        def take(notes, goal, gaps):
            """the level below and then notes in this order, as long as they
            keep the gap (each gap in turn), until there are goal notes"""
            have = set(kept)
            at = sorted(times[tune[i][0]] for i in kept)
            for g in gaps:
                for i in notes:
                    if goal is not None and len(have) >= goal:
                        return have
                    if i in have:
                        continue
                    ms = times[tune[i][0]]
                    need = g * (0.8 if tune[i][0] % TICKS == 0 else 1)
                    j = bisect.bisect_left(at, ms)
                    if (j == 0 or ms - at[j - 1] >= need) and (j == len(at) or at[j] - ms >= need):
                        have.add(i)
                        bisect.insort(at, ms)
            return have

        # what every place up to the bar, the half bar, the beat ... would give
        ends = [len(take([i for i in order if place(tune[i][0], beats_per_bar) <= p], None, (gap,)))
                for p in range(5)]
        near = [c for c in ends if lo <= c <= hi and abs(c - target) <= LEVEL_SNAP * target]
        goal = min(near, key=lambda c: abs(c - target)) if near else max(lo, min(hi, round(target)))
        kept = sorted(take(order, goal, (gap, 0)))
        levels.append(kept)
    levels.append(list(range(n)))
    return [[tune[i] for i in lv] for lv in levels]


def check_unison(tune, bass, name, beats_per_bar):
    for t in tune:
        for b in bass:
            if t[2] == b[2] and t[0] < b[0] + b[1] and b[0] < t[0] + t[1]:
                tick = max(t[0], b[0])
                sys.exit("%s: the tune and the bass both play MIDI note %d in bar %d (beat %d): on the "
                         "console the two cancel each other out; move one of them an octave"
                         % (name, t[2], tick // (beats_per_bar * TICKS) + 1, tick % (beats_per_bar * TICKS) // TICKS + 1))


def chart_stream(tune, lanes, times, total_ticks, beats_per_bar, level, chosen):
    """level 0 easy, 1 medium, 2 hard, 3 expert, with that level's notes (from
    level_notes) -> (bytes, number of notes, perfect score, star notes)"""
    lane_of = {id(n): l for n, l in zip(tune, lanes)}
    records = {}                                   # ms -> [lanes, flags, hold]
    bar_ticks = beats_per_bar * TICKS
    for i, n in enumerate(chosen):
        tick, length, midi = n
        ms = round(times[tick])
        lane = lane_of[id(n)]
        mask = 1 << lane
        if level >= 1 and tick % bar_ticks == 0 and length >= TICKS:
            mask |= 1 << (lane + 1 if lane < 2 else lane - 1)
        if level == 3 and length >= 2 * TICKS and tick % TICKS == 0:
            mask |= 1 << (lane + 1 if lane < 2 else lane - 1)
        # a hold's tail: to just before the note's end, but clear of the next
        # note of this chart (never a note to play while another is held)
        tail = round(times[tick + length]) - ms - 60
        if i + 1 < len(chosen):
            tail = min(tail, round(times[chosen[i + 1][0]]) - ms - HOLD_GAP_MS)
        hold = min(255, tail // 20) if tail >= HOLD_MIN_MS else 0
        r = records.setdefault(ms, [0, 0, 0])
        r[0] |= mask
        r[2] = max(r[2], hold)
        if (tick // bar_ticks) % 8 in (2, 3):
            r[1] |= FLAG_STAR
    for beat in range(total_ticks // TICKS + 1):
        ms = round(times[beat * TICKS])
        r = records.setdefault(ms, [0, 0, 0])
        r[1] |= FLAG_BEAT | (FLAG_BAR if beat % beats_per_bar == 0 else 0)
    out = bytearray()
    last = 0
    count = 0
    stars = 0
    streak = 0
    perfect = 0                                    # the score for hitting everything (play.h's rules)
    for ms in sorted(records):
        mask, flags, hold = records[ms]
        dt = with_waits(out, ms - last)
        flags |= mask | (FLAG_HOLD if hold and mask else 0)
        assert flags < CMD_END, "a chart record must not look like the end or nothing"
        out += bytes([dt, flags])
        if hold and mask:
            out += bytes([hold])
        if mask:
            count += 1
            if flags & FLAG_STAR:
                stars += 1
            streak += 1
            mult = 4 if streak >= 30 else 3 if streak >= 20 else 2 if streak >= 10 else 1
            perfect += 50 * bin(mask).count("1") * mult
            if hold:
                perfect += mult * ((hold * 20 * 60 + 999) // 1000)      # one multiplier a frame, 60 a second
        last = ms
    out += bytes([0, CMD_END])
    return out, count, perfect, stars


# ---------------------------------------------------------------- the songs
def load_song(name):
    path = os.path.join(SONGS, name + ".py")
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def build_song(name):
    mod = load_song(name)
    tune, bass, drums, total, beats = parse_song(mod, name)
    times = tick_times(mod.TEMPO, total, beats)
    music = music_stream([tune, bass], times, drums)
    lanes = lanes_for(tune)
    chosen = level_notes(tune, times, beats)
    charts, counts, perfects, stars = [], [], [], []
    for level in range(len(LEVELS)):
        c, n, p, s = chart_stream(tune, lanes, times, total, beats, level, chosen[level])
        charts.append(c)
        counts.append(n)
        perfects.append(p)
        stars.append(s)
    if not all(a < b for a, b in zip(counts, counts[1:])):
        sys.exit("%s: a level has no more notes than the one below it (%s)" % (name, "/".join(map(str, counts))))
    length = round(times[total])
    seconds = (length - LEAD_MS) / 1000.0
    preview_from = getattr(mod, "PREVIEW", PREVIEW_FIRST)
    preview, preview_ms = section_stream(tune, bass, drums, times, beats, preview_from, PREVIEW_BARS)
    text = (mod.TITLE[:21] + "\0" + mod.BY[:21] + "\0").encode("ascii")
    song_id = zlib.crc32(name.encode("ascii") + b"".join(charts)) & 0xFFFF
    print("%-16s %5.1f s, %d bars, tune %d notes, bass %d, drums %d; charts %s; music %d bytes, charts %d"
          % (name, seconds, total // (beats * TICKS), len(tune), len(bass), len(drums),
             " ".join("%s %d (%.1f/s, %d, %d star)" % (LEVELS[i][0], counts[i], counts[i] / seconds, perfects[i], stars[i])
                      for i in range(len(LEVELS))),
             len(music), sum(len(c) for c in charts)))
    print("%16s preview from bar %d, %.1f s, %d bytes" % ("", preview_from, preview_ms / 1000.0, len(preview)))
    return dict(music=music, charts=charts, counts=counts, perfects=perfects, stars=stars, length=length, text=text,
                id=song_id, preview=preview)


def build_riff():
    """the title page's loop: a music stream without the lead-in, ending
    exactly where the loop comes round -> (stream, a beat in ms: the logo
    nods and the crowd jumps to it)"""
    mod = load_song(TITLE_RIFF)
    if hasattr(mod, "FROM"):
        name, first, bars = mod.FROM
        song = load_song(name)
        tune, bass, drums, total, beats = parse_song(song, name)
        times = tick_times(song.TEMPO, total, beats)
        music, loop_ms = section_stream(tune, bass, drums, times, beats, first, bars)
        beat_ms = round(60000.0 / tempo_at(song.TEMPO, first - 1))
        print("%-16s %5.1f s loop, bars %d-%d of %s, a beat %d ms; music %d bytes"
              % (TITLE_RIFF, loop_ms / 1000.0, first, first + bars - 1, name, beat_ms, len(music)))
        return music, beat_ms
    tune, bass, drums, total, beats = parse_song(mod, TITLE_RIFF)
    times = tick_times(mod.TEMPO, total, beats)
    loop_ms = round(times[total]) - LEAD_MS
    music = music_stream([tune, bass], times, drums, shift=LEAD_MS, end_ms=loop_ms)
    beat_ms = round(60000.0 / tempo_at(mod.TEMPO, 0))
    print("%-16s %5.1f s loop, %d bars, a beat %d ms; music %d bytes"
          % (TITLE_RIFF, loop_ms / 1000.0, total // (beats * TICKS), beat_ms, len(music)))
    return music, beat_ms


# ---------------------------------------------------------------- the tables
def note_periods():
    """timer counts for every note: 16 MHz / 8, toggled at every match"""
    out = []
    for n in range(NOTE_LO, NOTE_HI + 1):
        f = 440.0 * 2 ** ((n - 69) / 12.0)
        out.append(round(1e6 / f) - 1)
    return out


def perspective():
    ys, ss = [], []
    for i in range(PERSP_N):
        tau = PERSP_MIN + i * PERSP_STEP
        s = 200.0 * T0 / (T0 + tau)
        y = HORIZON_Y + (s - 40) * (HIT_Y - HORIZON_Y) / 160.0
        ys.append(max(0, min(63, round(y))))
        ss.append(max(1, min(255, round(s))))
    return ys, ss


def c_array(kind, name, values, per_line=16):
    lines = []
    for i in range(0, len(values), per_line):
        lines.append("  " + ", ".join(str(v) for v in values[i:i + per_line]) + ",")
    return "static const %s %s[%d] PROGMEM = {\n%s\n};\n" % (kind, name, len(values), "\n".join(lines))


def write_tables():
    ys, ss = perspective()
    rows = max(len(r) for r in GEM_ROWS)
    gem_h = [len(r) for r in GEM_ROWS]
    gem_hw = []
    for r in GEM_ROWS:
        gem_hw += r + [0] * (rows - len(r))
    with open(os.path.join(SKETCH, "tables.h"), "w") as f:
        f.write("// Generated by tools/build_data.py - do not edit by hand.\n#pragma once\n#include <avr/pgmspace.h>\n\n")
        f.write("#define HORIZON_Y %d\n#define HIT_Y %d\n#define LOOK_MS %d\n" % (HORIZON_Y, HIT_Y, LOOK_MS))
        f.write("#define PERSP_MIN %d\n#define PERSP_STEP %d\n#define PERSP_N %d\n" % (PERSP_MIN, PERSP_STEP, PERSP_N))
        f.write("#define NEAR_LANE %d\n#define NEAR_EDGE %d\n" % (NEAR_LANE, NEAR_EDGE))
        f.write("#define NOTE_LO %d\n#define NOTE_HI %d\n" % (NOTE_LO, NOTE_HI))
        f.write("#define GEM_SIZES %d\n#define GEM_ROWS %d\n\n" % (len(GEM_ROWS), rows))
        f.write("// the highway: for the time a note still has to go (PERSP_MIN, then\n"
                "// every PERSP_STEP ms): its row, and its scale in 1/200 (200 at the hit line)\n")
        f.write(c_array("uint8_t", "PERSP_Y", ys))
        f.write(c_array("uint8_t", "PERSP_S", ss))
        f.write("\n// timer counts for the notes NOTE_LO..NOTE_HI\n")
        f.write(c_array("uint16_t", "NOTE_PERIOD", note_periods(), 12))
        f.write("\n// gems by size: rows, and the half width of every row; the scale each size needs\n")
        f.write(c_array("uint8_t", "GEM_H", gem_h))
        f.write(c_array("uint8_t", "GEM_HW", gem_hw, rows))
        f.write(c_array("uint8_t", "GEM_LIMIT", GEM_LIMITS))


# ---------------------------------------------------------------- sprites
BOOED_SIZE = 22            # the booed-off page's title, Impact at this size


def sprite_bytes(lit, mask):
    """a picture and its mask (Pillow '1' images) as the game draws a sprite
    off the chip: a row of eight at a time, every column's image byte and
    then its mask byte"""
    w, h = lit.size
    lp, mp = lit.load(), mask.load()
    out = bytearray()
    for page in range((h + 7) // 8):
        for x in range(w):
            img = msk = 0
            for i in range(8):
                y = page * 8 + i
                if y < h and lp[x, y]:
                    img |= 1 << i
                if y < h and mp[x, y]:
                    msk |= 1 << i
            out += bytes([img, msk])
    return bytes(out)


# ---------------------------------------------------------------- the chip image
def main():
    sys.path.insert(0, SONGS)
    setlist = load_song("setlist").SETLIST
    songs = [build_song(name) for name in setlist]
    if len({s["id"] for s in songs}) < len(songs):
        sys.exit("two songs have the same id: the saved scores would mix; rename one")
    table_at = 0x10
    entry = 64
    blob = bytearray(b"PH\x01" + bytes([len(songs)]) + b"\0" * 12)
    blob += b"\0" * (entry * len(songs))
    entries = []
    for s in songs:
        offs = {}
        parts = ([("music", s["music"])] + [("c%d" % i, c) for i, c in enumerate(s["charts"])]
                 + [("text", s["text"]), ("preview", s["preview"])])
        for key, data in parts:
            offs[key] = len(blob)
            blob += data
        entries.append(offs)
    for i, (s, offs) in enumerate(zip(songs, entries)):
        e = struct.pack("<I", offs["music"])[:3]
        for k in range(len(LEVELS)):
            e += struct.pack("<I", offs["c%d" % k])[:3]
        e += struct.pack("<I", offs["text"])[:3]
        e += struct.pack("<I", s["length"])
        e += struct.pack("<%dH" % len(LEVELS), *s["counts"])
        e += struct.pack("<H", s["id"])
        e += struct.pack("<%dI" % len(LEVELS), *s["perfects"])
        e += struct.pack("<%dH" % len(LEVELS), *s["stars"])
        e += struct.pack("<I", offs["preview"])[:3]
        assert len(e) <= entry
        e += b"\0" * (entry - len(e))
        blob[table_at + i * entry:table_at + (i + 1) * entry] = e
    riff_at = len(blob)
    riff, riff_beat = build_riff()
    blob += riff
    blob[4:7] = struct.pack("<I", riff_at)[:3]
    cameras = []
    for name, crowds, data in scene.scenes():
        cameras.append((name, len(blob), crowds))
        blob += data
    flame_data, flame_box, flame_hit, flame_hold = flames.chip_data(flames.cached())
    flames_at = len(blob)
    blob += flame_data
    booed_lit, booed_mask = logo.title("BOOED OFF", BOOED_SIZE)
    booed_at = len(blob)
    blob += sprite_bytes(booed_lit, booed_mask)
    people = crowd.cached()
    layers_at = {}
    for name in ("title_light", "title_crowd", "boo_light", "boo_crowd"):
        layers_at[name] = len(blob)
        for frame in people[name]:
            blob += frame
    # the booed-off page's light as it opens, frame by frame: the game works
    # it out (both on, the left one out, the last picture with both out)
    lo, ro = crowd.BOO_LEFT_OFF, crowd.BOO_RIGHT_OFF
    assert people["boo_order"] == [f >> 3 if f < lo else (f >> 3) + 1 for f in range(ro)]
    assert len(people["boo_light"]) == ((ro - 1) >> 3) + 3
    while len(blob) % 256:
        blob += b"\xff"
    page = (CHIP_BYTES - SAVE_BYTES - len(blob)) >> 8
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "fxdata.bin"), "wb") as f:
        f.write(blob)
    with open(os.path.join(OUT, "fxsave.bin"), "wb") as f:
        f.write(b"\xff" * SAVE_BYTES)
    with open(os.path.join(SKETCH, "fxdata.h"), "w") as f:
        f.write("// Generated by tools/build_data.py - do not edit by hand.\n#pragma once\n\n"
                "// development places: the data sits at the end of the 16 MB chip, in front\n"
                "// of the 4 KB the game may write to (the way the ArduboyFX tools have it)\n"
                "#define FX_DATA_PAGE 0x%04X\n#define FX_SAVE_PAGE 0x%04X\n#define FX_DATA_BYTES %dUL\n\n"
                % (page, (CHIP_BYTES - SAVE_BYTES) >> 8, len(blob)))
        f.write("#define SONG_COUNT %d\n#define FX_SONG_TABLE 0x%02X\n#define SONG_ENTRY_BYTES %d\n#define LEVELS %d\n"
                % (len(songs), table_at, entry, len(LEVELS)))
        f.write("#define FX_TITLE_MUSIC 0x%06XUL\n#define TITLE_BEAT_MS %d      // a beat of the menus' song: the logo nods to it\n"
                % (riff_at, riff_beat))
        # the cameras: their frames on the chip, a screen each (tools/scene.py)
        f.write("\n// the cameras of the play page (tools/scene.py): where each camera's frames\n"
                "// start on the chip, a screen of 1024 bytes each: for every position of the\n"
                "// lights (CAM_LIGHTS), every pose of the band (BAND_STATES: the two beats, the\n"
                "// two with a strum, a jump) and every pose of the crowd (CAM_CROWDS: fists low\n"
                "// and up, or 1 where the crowd is not in sight), in that order\n")
        f.write("#define CAM_COUNT %d\n#define CAM_LIGHTS %d\n#define BAND_STATES %d\n"
                % (len(cameras), scene.LIGHTS, scene.BAND_STATES))
        for i, (name, a, crowds) in enumerate(cameras):
            f.write("#define CAM_%s %d\n" % (name, i))
        f.write("static const uint32_t CAM_FRAMES[%d] PROGMEM = {%s};\n"
                % (len(cameras), ", ".join("0x%06XUL" % a for _, a, _ in cameras)))
        f.write("static const uint8_t CAM_CROWDS[%d] PROGMEM = {%s};\n"
                % (len(cameras), ", ".join(str(c) for _, _, c in cameras)))
        f.write("// the order the cameras cut in, every few bars\n")
        f.write("static const uint8_t SCENE_CUTS[%d] PROGMEM = {%s};\n"
                % (len(scene.CUTS), ", ".join("CAM_" + c for c in scene.CUTS)))
        # the flames and the booed-off page's title: sprites
        f.write("\n// the flames on the pads (tools/flames.py): sprites FLAME_W columns wide and\n"
                "// FLAME_PAGES rows of eight tall, their box at FLAME_X0, FLAME_Y0 from the\n"
                "// pad's middle; for each pad its hit's frames, then its hold's, of which\n"
                "// FLAME_LOOP_A..FLAME_LOOP_B-1 go round while the note is held\n")
        x0, y0, w, pages = flame_box
        f.write("#define FX_FLAMES 0x%06XUL\n#define FLAME_X0 %d\n#define FLAME_Y0 %d\n#define FLAME_W %d\n"
                "#define FLAME_PAGES %d\n#define FLAME_BYTES %d\n#define FLAME_HIT_N %d\n#define FLAME_HOLD_N %d\n"
                "#define FLAME_LOOP_A %d\n#define FLAME_LOOP_B %d\n"
                % (flames_at, x0, y0, w, pages, w * pages * 2, flame_hit, flame_hold, flames.LOOP_A, flames.LOOP_B))
        f.write("// the booed-off page's title (tools/logo.py), a sprite\n")
        f.write("#define FX_BOOED 0x%06XUL\n#define BOOED_W %d\n#define BOOED_H %d\n#define BOOED_PAGES %d\n"
                % (booed_at, booed_lit.size[0], booed_lit.size[1], (booed_lit.size[1] + 7) // 8))
        # the crowd and the spotlights
        f.write("\n// the crowd and the spotlights (tools/crowd.py), frames of the screen buffer's\n"
                "// pages: the title page's and the menu's light (the spotlights at %d places of their\n"
                "// swing) over pages 0..TITLE_LIGHT_PAGES-1 and their crowd (%d steps of its jumping)\n"
                "// over pages CROWD_PAGE on, from row CROWD_TOP down (the highest row the people\n"
                "// reach; above it what is under the crowd stays); the booed-off page's light (as\n"
                "// the page opens, then BOO_LIGHT_N-1 with the spotlights out) and its crowd booing\n"
                "// (%d frames); where the cups leave the throwers' hands: x, y, towards, the frame\n"
                "// of the throw\n"
                % (crowd.SWING, crowd.STEPS, crowd.BOO_FRAMES))
        lp, cp = crowd.LIGHT_ROWS // 8, crowd.TITLE_SPLIT // 8
        f.write("#define FX_TITLE_LIGHT 0x%06XUL\n#define TITLE_LIGHT_PAGES %d\n"
                "#define FX_TITLE_CROWD 0x%06XUL\n#define TITLE_CROWD_PAGES %d\n"
                "#define FX_BOO_LIGHT 0x%06XUL\n#define BOO_LIGHT_PAGES %d\n#define BOO_LIGHT_N %d\n"
                "#define BOO_LEFT_OFF %d\n#define BOO_RIGHT_OFF %d\n"
                "#define FX_BOO_CROWD 0x%06XUL\n#define BOO_CROWD_PAGES %d\n"
                "#define CROWD_PAGE %d\n#define CROWD_TOP %d\n"
                % (layers_at["title_light"], lp, layers_at["title_crowd"], 8 - cp,
                   layers_at["boo_light"], lp, len(people["boo_light"]), crowd.BOO_LEFT_OFF, crowd.BOO_RIGHT_OFF,
                   layers_at["boo_crowd"], 8 - cp, cp, people["crowd_top"]))
        cups = crowd.cups()
        f.write("static const int8_t BOO_CUPS[%d] PROGMEM = {%s};\n"
                % (4 * len(cups), ", ".join("%d, %d, %d, %d" % c for c in cups)))
    write_tables()
    print("flames: %d bytes (%d frames a pad, %dx%d); booed-off title %dx%d; crowd and spotlights %d bytes"
          % (len(flame_data), flame_hit + flame_hold, flame_box[2], flame_box[3] * 8,
             booed_lit.size[0], booed_lit.size[1],
             sum(len(b) for k in ("title_light", "title_crowd", "boo_light", "boo_crowd") for b in people[k])))
    print("fxdata.bin: %d bytes, data page 0x%04X, %d song%s" % (len(blob), page, len(songs), "s" if len(songs) != 1 else ""))


if __name__ == "__main__":
    main()
