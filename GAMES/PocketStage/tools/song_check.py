#!/usr/bin/env python3
"""
Pocket Stage - look a song over before it goes on the chip.

  python3 tools/song_check.py name [name ...]     (a song in songs/, without .py)
  python3 tools/song_check.py --chart name        and its Expert chart, bar by bar

Builds each song the way tools/build_data.py does and says what would stop
the build (a bar that does not add up, the tune and the bass on one pitch,
a note outside the pitch table), then what the song is: its length, its
range, the notes on each level and how dense they come, its holds and
chords, and whether the title and the band fit the setlist (the title in the
menu font left of the stars, the band in the small print left of the
length).
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(ROOT, "songs"))

import build_data as bd  # noqa: E402
import font  # noqa: E402

TITLE_PX = 75              # the setlist: a title from x 9 to the stars' box at x 84
BY_CHARS = 16              # the details: the band from x 2 to the length at x 102
LANE = "LMR"


def note_name(m):
    return ["c", "c#", "d", "eb", "e", "f", "f#", "g", "ab", "a", "bb", "b"][m % 12] + str(m // 12 - 1)


def peak(times_ms, window=2000):
    """the most notes in any window of so many ms, as notes a second"""
    best = 0
    j = 0
    for i, t in enumerate(times_ms):
        while times_ms[j] < t - window:
            j += 1
        best = max(best, i - j + 1)
    return best * 1000.0 / window


def read_chart(data):
    """a chart stream -> [(ms, lanes, hold ms, star)]"""
    out = []
    ms = 0
    i = 0
    while True:
        dt, flags = data[i], data[i + 1]
        i += 2
        ms += dt
        if flags == bd.CMD_END:
            break
        if flags == bd.CMD_NOP:
            continue
        hold = 0
        if flags & bd.FLAG_HOLD:
            hold = data[i] * 20
            i += 1
        if flags & 7:
            out.append((ms, flags & 7, hold, bool(flags & bd.FLAG_STAR)))
    return out


def check(name, show_chart=False):
    print("== %s" % name)
    try:
        mod = bd.load_song(name)
        tune, bass, drums, total, beats = bd.parse_song(mod, name)
    except SystemExit as e:
        print("   ERROR: %s" % e)
        return False
    ok = True
    times = bd.tick_times(mod.TEMPO, total, beats)
    seconds = (times[total] - bd.LEAD_MS) / 1000.0
    glyphs, _ = font.glyphs()
    title, by = mod.TITLE, mod.BY
    width = sum(glyphs[c].size[0] + 1 for c in title if c in glyphs) - 1
    bad = [c for c in title if c not in glyphs]
    print("   %s / %s / %s, %s, %s bpm, %d beats a bar"
          % (title, by, getattr(mod, "STYLE", "?"), getattr(mod, "KEY", "?"),
             "-".join(str(t) for _, t in mod.TEMPO), beats))
    if bad or width > TITLE_PX:
        print("   ERROR: the title is %d px (at most %d) %s" % (width, TITLE_PX, "and has %r" % bad if bad else ""))
        ok = False
    if len(by) > BY_CHARS or any(not (32 <= ord(c) < 127) for c in by) or by != by.upper():
        print("   ERROR: the band %r: capitals, at most %d characters" % (by, BY_CHARS))
        ok = False
    if not 48 <= seconds <= 66:
        print("   WARNING: %.1f s long (aim for 50 to 60)" % seconds)
    lo_t, hi_t = min(n[2] for n in tune), max(n[2] for n in tune)
    lo_b, hi_b = (min(n[2] for n in bass), max(n[2] for n in bass)) if bass else (0, 0)
    print("   %.1f s, %d bars; tune %s..%s (%d notes), bass %s..%s (%d), drums %d"
          % (seconds, total // (beats * bd.TICKS), note_name(lo_t), note_name(hi_t), len(tune),
             note_name(lo_b), note_name(hi_b), len(bass), len(drums)))
    if lo_t < 72:
        print("   note: the tune goes down to %s; under C5 it is thin on the speaker" % note_name(lo_t))
    lanes = bd.lanes_for(tune)
    chosen = bd.level_notes(tune, times, beats)
    for level in range(len(bd.LEVELS)):
        data, count, perfect, stars = bd.chart_stream(tune, lanes, times, total, beats, level, chosen[level])
        chart = read_chart(data)
        onsets = [c[0] for c in chart]
        gaps = [b - a for a, b in zip(onsets, onsets[1:])]
        holds = sum(1 for c in chart if c[2])
        chords = sum(1 for c in chart if bin(c[1]).count("1") > 1)
        use = [sum(1 for c in chart if c[1] & (1 << k)) for k in range(3)]
        print("   %-6s %4d notes, %.2f a second, %.1f at the most (2 s), the closest %3d ms apart; "
              "%d holds, %d chords, %d star; lanes %s"
              % (bd.LEVELS[level][0], count, count / seconds, peak(onsets), min(gaps) if gaps else 0,
                 holds, chords, stars, "/".join("%d%%" % (100 * u // max(1, sum(use))) for u in use)))
        if level == 3:
            run, longest = 1, 1
            for a, b in zip(chart, chart[1:]):
                run = run + 1 if a[1] == b[1] else 1
                longest = max(longest, run)
            fast, fastest = 0, 0
            for g in gaps:
                fast = fast + 1 if g < 150 else 0
                fastest = max(fastest, fast + 1 if fast else 0)
            print("          the same lane %d times in a row at the most; %d notes in a row under 150 ms apart"
                  % (longest, fastest))
            if show_chart:
                bar_ms = [round(times[b * beats * bd.TICKS]) for b in range(total // (beats * bd.TICKS) + 1)]
                for b in range(len(bar_ms) - 1):
                    row = []
                    for ms, lmask, hold, star in chart:
                        if bar_ms[b] <= ms < bar_ms[b + 1]:
                            row.append("".join(LANE[k] for k in range(3) if lmask & (1 << k))
                                       + ("~%d" % hold if hold else "") + ("*" if star else ""))
                    print("          bar %2d: %s" % (b + 1, " ".join(row)))
    return ok


def main():
    args = sys.argv[1:]
    show = "--chart" in args
    names = [a[:-3] if a.endswith(".py") else a for a in args if a != "--chart"]
    names = [os.path.basename(n) for n in names]
    good = all([check(n, show) for n in names])
    sys.exit(0 if good else 1)


if __name__ == "__main__":
    main()
