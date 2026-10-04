#!/usr/bin/env python3
"""The flames on the pads: fire that comes out of the middle of a pad when
its note is hit, and keeps burning while a held note is held (the look Joel
picked from the samples, number 4 of build/look/v5.html).

How the fire is made. A flame is a few licks at a time. A lick is born at
the root of the flame in the pad's middle, stretches upward, thins, lets go
of the root and rises on its own, and is gone; while a note is held a new
one is born every few frames somewhere else along the root. The hit lets
three tall ones go at once. A frame is the licks' shapes worked out at four
by four points a pixel: a pixel at least 40 % covered is white, and a black
line goes round the fire (not down at the pad, so the fire comes out of
it). Each pad's fire is its own (a seed of its own).

Two timelines per pad:
  HIT    the hit's burst and nothing after it: the flame dies down
  HOLD   the burst, then a lick every few frames: frames LOOP_A..LOOP_B-1
         repeat for as long as the note is held (the licks are born to a
         schedule that repeats exactly, so the loop has no seam), and once
         it is let go, LOOP_B to the end: nothing new, it dies down

On the chip a frame is a sprite: its box (the same for every frame, X0..,
Y0.. from the pad's middle) a row of eight at a time, every column's image
byte and then its mask byte. The game finds a frame by pad, timeline and
number (fxdata.h has the numbers).

  python3 tools/flames.py      renders (or takes the frames from
                               build/flames_cache.json) and writes
                               build/flames.png, every frame at 4x
"""
import hashlib
import json
import math
import os
import random

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

LOOP_A, LOOP_P = 16, 24
LOOP_B = LOOP_A + LOOP_P
EVERY = 3                                  # a new lick every so many frames while held
SEEDS = (1, 2, 3)                          # the left, middle and right pad
SS = 4                                     # points a pixel, each way
GRID = (-12, 12, -36, 1)                   # the pixels looked at: dx and dy from the pad's middle
LINE_FROM = -3                             # the black line only from this row up (dy)


def clamp(v, lo=0.0, hi=1.0):
    return lo if v < lo else hi if v > hi else v


class Lick:
    def __init__(self, t0, x0, life, tall, wide, lean, ph, wob):
        self.t0, self.x0, self.life, self.tall, self.wide = t0, x0, life, tall, wide
        self.lean, self.ph, self.wob = lean, ph, wob

    def shape(self, f):
        """-> None, or (bottom, top, half width, attached, age)"""
        a = (f - self.t0) / float(self.life)
        if a < 0 or a >= 1:
            return None
        attached = a < 0.45
        yb = 0.0 if attached else ((a - 0.45) / 0.55) ** 1.15 * 0.85 * self.tall
        grow = min(1.0, a / 0.3) ** 0.6
        shrink = 1.0 - max(0.0, a - 0.45) / 0.55 * 0.8
        length = self.tall * grow * shrink
        w = self.wide * (1.0 - 0.55 * a)
        return yb, yb + length, w, attached, a

    def value(self, f, u, v):
        """> 0 inside the lick: 1 on its middle line, 0 at its edge"""
        sh = self.shape(f)
        if sh is None:
            return -1.0
        yb, yt, w, attached, a = sh
        if v < yb - 1.0 or v > yt or yt - yb < 0.5:
            return -1.0
        s = clamp((v - yb) / (yt - yb))
        hw = w * (1.0 - s) ** 0.85
        if not attached:
            hw *= min(1.0, math.sqrt(max(0.0, (v - yb + 1.0)) / 2.5))
        xc = self.x0 + self.lean * a * 3.0 * s + self.wob * s * math.sin(2 * math.pi * (0.9 * s - 1.6 * a) + self.ph)
        if hw <= 0.05:
            return -1.0
        return 1.0 - abs(u - xc) / hw


class Fire:
    """one pad's fire on one timeline ('hit' or 'hold')"""

    def __init__(self, seed=1, kind="hit", burst_tall=(14.0, 20.0, 13.0), steady_tall=(11.0, 17.0),
                 root_w=4.6, root_h=2.6):
        rng = random.Random(seed * 977 + (0 if kind == "hit" else 1))
        self.root_w, self.root_h = root_w, root_h
        self.licks = []
        xs = [-3.2, 0.2, 3.1]
        rng.shuffle(xs)
        for x, tall in zip(xs, burst_tall):                       # the burst: three tall ones at once
            self.licks.append(Lick(0, x + rng.uniform(-0.4, 0.4), rng.randint(13, 16), tall * rng.uniform(0.9, 1.1),
                                   rng.uniform(2.3, 2.8), -x * rng.uniform(0.15, 0.35), rng.uniform(0, 6.28),
                                   rng.uniform(0.8, 1.4)))
        self.licks.append(Lick(2, rng.uniform(-1.5, 1.5), 12, burst_tall[1] * 0.7, 2.2, rng.uniform(-0.4, 0.4),
                               rng.uniform(0, 6.28), 1.0))
        if kind == "hold":
            sched = random.Random(seed * 131 + 7)
            cycle = []
            for k in range(LOOP_P // EVERY):                        # one turn of the schedule, repeated
                cycle.append((sched.choice((-3.0, -1.6, 0.0, 1.6, 3.0)) + sched.uniform(-0.5, 0.5),
                              sched.randint(12, 15), sched.uniform(*steady_tall), sched.uniform(2.0, 2.6),
                              sched.uniform(-0.35, 0.35), sched.uniform(0, 6.28), sched.uniform(0.7, 1.3)))
            t = 4
            while t < LOOP_B:
                c = cycle[((t - 4) // EVERY) % len(cycle)]
                self.licks.append(Lick(t, *c))
                t += EVERY
        self.n = max(l.t0 + l.life for l in self.licks)
        # the root shrinks over the last 8 frames before this; on the hold's
        # timeline not before it is let go (LOOP_B), so the loop has no seam
        self.root_end = 14 if kind == "hit" else LOOP_B + 8

    def root(self, f):
        """the root of the flame in the pad's middle: bigger in the burst,
        shrinking as the flame dies"""
        if f < 2:
            k = 1.15
        elif f < self.root_end - 8:
            k = 1.0
        else:
            k = clamp((self.root_end - f) / 8.0)
        return self.root_w * k, self.root_h * k

    def value(self, f, u, v):
        best = -1.0
        rw, rh = self.root(f)
        if rw > 0.4:
            best = 1.0 - (u / rw) ** 2 - (max(0.0, v) / rh) ** 2 - (max(0.0, -v) / 1.5) ** 2
        for l in self.licks:
            if l.t0 <= f < l.t0 + l.life:
                best = max(best, l.value(f, u, v))
        return best

    def frame(self, f):
        """-> (lit, line): sets of (dx, dy) from the pad's middle, dy down"""
        x0, x1, y0, y1 = GRID
        lit = set()
        for dy in range(y0, y1 + 1):
            for dx in range(x0, x1 + 1):
                n = 0
                for j in range(SS):
                    for i in range(SS):
                        if self.value(f, dx + (i + 0.5) / SS - 0.5, -dy + (j + 0.5) / SS - 0.5) > 0:
                            n += 1
                if n >= 0.4 * SS * SS:
                    lit.add((dx, dy))
        line = set()
        for (x, y) in lit:
            for ddx in (-1, 0, 1):
                for ddy in (-1, 0, 1):
                    q = (x + ddx, y + ddy)
                    if q not in lit and q[1] <= LINE_FROM:
                        line.add(q)
        return lit, line


def render():
    """-> [(hit frames, hold frames)] for the three pads, a frame (lit, line)"""
    out = []
    for seed in SEEDS:
        timelines = []
        for kind in ("hit", "hold"):
            fire = Fire(seed, kind)
            timelines.append([fire.frame(f) for f in range(fire.n)])
        out.append(tuple(timelines))
    return out


def cached():
    """the frames, rendered when this file changed"""
    key = hashlib.sha1(open(__file__, "rb").read()).hexdigest()
    path = os.path.join(ROOT, "build", "flames_cache.json")
    try:
        with open(path) as f:
            c = json.load(f)
        if c.get("key") == key:
            return [tuple([[(set(map(tuple, lit)), set(map(tuple, line))) for lit, line in tl] for tl in pad])
                    for pad in c["pads"]]
    except (OSError, ValueError, KeyError):
        pass
    pads = render()
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump({"key": key, "pads": [[[[sorted(lit), sorted(line)] for lit, line in tl] for tl in pad]
                                        for pad in pads]}, f)
    return pads


def layout(pads):
    """-> the sprite box (x0, y0, w, pages), frames on the hit's timeline,
    frames on the hold's: the box holds every frame, the counts are the
    longest of the three pads' (a shorter one ends in empty frames)"""
    xs, ys = [], []
    for pad in pads:
        for tl in pad:
            for lit, line in tl:
                for x, y in lit | line:
                    xs.append(x)
                    ys.append(y)
    x0, x1, y0, y1 = min(xs), max(xs), min(ys), max(ys)
    pages = (y1 - y0 + 8) // 8
    n_hit = max(len(pad[0]) for pad in pads)
    n_hold = max(len(pad[1]) for pad in pads)
    return (x0, y0, x1 - x0 + 1, pages), n_hit, n_hold


def sprite_bytes(lit, line, box):
    """a frame as the game draws it: a row of eight at a time, every column's
    image byte, then its mask byte (the mask: the fire and its black line)"""
    x0, y0, w, pages = box
    out = bytearray()
    for p in range(pages):
        for c in range(w):
            img = msk = 0
            for i in range(8):
                q = (x0 + c, y0 + p * 8 + i)
                if q in lit:
                    img |= 1 << i
                    msk |= 1 << i
                elif q in line:
                    msk |= 1 << i
            out += bytes([img, msk])
    return bytes(out)


def chip_data(pads):
    """-> (bytes, box, n_hit, n_hold): every pad's hit frames then its hold
    frames, the pads one after the other"""
    box, n_hit, n_hold = layout(pads)
    empty = (set(), set())
    data = bytearray()
    for hit, hold in pads:
        for tl, n in ((hit, n_hit), (hold, n_hold)):
            for k in range(n):
                lit, line = tl[k] if k < len(tl) else empty
                data += sprite_bytes(lit, line, box)
    return bytes(data), box, n_hit, n_hold


def preview(pads, scale=4):
    """build/flames.png: each pad a row of its hit's frames, then a row of its hold's"""
    from PIL import Image
    box, n_hit, n_hold = layout(pads)
    x0, y0, w, pages = box
    h = pages * 8
    cols = max(n_hit, n_hold)
    im = Image.new("RGB", ((w + 2) * cols * scale, (h + 2) * 6 * scale), (60, 60, 70))
    for r, (pad, tl) in enumerate((p, t) for p in range(3) for t in range(2)):
        for k, (lit, line) in enumerate(pads[pad][tl]):
            t = Image.new("RGB", (w, h), (0, 0, 0))
            for x, y in line:
                t.putpixel((x - x0, y - y0), (90, 0, 0))
            for x, y in lit:
                t.putpixel((x - x0, y - y0), (255, 255, 255))
            im.paste(t.resize((w * scale, h * scale), Image.NEAREST), (k * (w + 2) * scale, r * (h + 2) * scale))
    path = os.path.join(ROOT, "build", "flames.png")
    im.save(path)
    print("wrote", os.path.relpath(path, ROOT))


if __name__ == "__main__":
    import time
    t = time.time()
    pads = cached()
    data, box, n_hit, n_hold = chip_data(pads)
    print("flames: box %d x %d at (%d, %d) from the pad's middle, %d pages; %d hit frames and %d hold frames a pad; "
          "%d bytes; %.1f s" % (box[2], box[3] * 8, box[0], box[1], box[3], n_hit, n_hold, len(data), time.time() - t))
    preview(pads)
