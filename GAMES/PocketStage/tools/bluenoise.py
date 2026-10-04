#!/usr/bin/env python3
"""A blue-noise threshold matrix for dithering pictures to the screen's two
colours, made by void-and-cluster (Ulichney 1993) and kept in
build/bluenoise.json.

Ordered dithering with it looks nearly as good as error diffusion but is
stable: a pixel's threshold never changes, so the parts of a picture that
do not change between two frames dither the same way, and nothing crawls
when the stage is animated. The matrix is 64 x 32 and tiles the screen.

The algorithm: start from a random sparse pattern and relax it until its
tightest cluster and its largest void coincide; then rank the pixels by
taking ones away from the tightest cluster (down to none), and adding ones
at the largest void (up to all). A pixel's "energy" is the sum of a
Gaussian over the ones around it, kept up to date as pixels toggle.
"""
import json
import math
import os
import random

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
W, H = 64, 32


class Field:
    def __init__(self, w, h, sigma=1.5, radius=5):
        self.w, self.h, self.n = w, h, w * h
        self.kern = [(dx, dy, math.exp(-(dx * dx + dy * dy) / (2 * sigma * sigma)))
                     for dy in range(-radius, radius + 1) for dx in range(-radius, radius + 1)]
        self.on = [0] * self.n
        self.energy = [0.0] * self.n

    def toggle(self, i, sign):
        x, y = i % self.w, i // self.w
        w, h, e = self.w, self.h, self.energy
        for dx, dy, k in self.kern:
            e[((y + dy) % h) * w + (x + dx) % w] += sign * k
        self.on[i] = 1 if sign > 0 else 0

    def tightest(self):
        """the one with the most energy"""
        e, on = self.energy, self.on
        return max((i for i in range(self.n) if on[i]), key=lambda i: e[i])

    def largest_void(self):
        """the zero with the least energy"""
        e, on = self.energy, self.on
        return min((i for i in range(self.n) if not on[i]), key=lambda i: e[i])


def make(w=W, h=H, seed=3):
    rng = random.Random(seed)
    f = Field(w, h)
    n = f.n
    m = n // 10
    start = list(range(n))
    rng.shuffle(start)
    for i in start[:m]:
        f.toggle(i, +1)
    while True:                                     # relax
        c = f.tightest()
        f.toggle(c, -1)
        v = f.largest_void()
        if v == c:
            f.toggle(c, +1)
            break
        f.toggle(v, +1)
    initial = f.on[:]
    rank = [0] * n
    for r in range(m - 1, -1, -1):                  # phase 1: take ones away
        c = f.tightest()
        f.toggle(c, -1)
        rank[c] = r
    for i in range(n):                              # back to the start
        if initial[i] and not f.on[i]:
            f.toggle(i, +1)
    for r in range(m, n):                           # phases 2 and 3: fill the voids
        v = f.largest_void()
        f.toggle(v, +1)
        rank[v] = r
    return [[(rank[y * w + x] + 0.5) / n for x in range(w)] for y in range(h)]


def matrix():
    """-> the threshold matrix (rows of floats in 0..1), made once and kept"""
    path = os.path.join(ROOT, "build", "bluenoise.json")
    try:
        with open(path) as fh:
            m = json.load(fh)
        if len(m) == H and len(m[0]) == W:
            return m
    except (OSError, ValueError):
        pass
    m = make()
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as fh:
        json.dump(m, fh)
    return m


def threshold(m, x, y):
    return m[y % H][x % W]


if __name__ == "__main__":
    import time
    t = time.time()
    m = matrix()
    print("blue noise %dx%d in %.1f s" % (W, H, time.time() - t))
    from PIL import Image
    im = Image.new("L", (W * 2, H * 2))
    px = im.load()
    for y in range(H * 2):
        for x in range(W * 2):
            px[x, y] = int(255 * threshold(m, x, y))
    im = im.resize((W * 8, H * 8), Image.NEAREST)
    im.save(os.path.join(ROOT, "build", "bluenoise.png"))
    # a ramp dithered with it, to look at
    ramp = Image.new("1", (128, 64))
    rp = ramp.load()
    for y in range(64):
        for x in range(128):
            rp[x, y] = 1 if (x / 127.0) > threshold(m, x, y) else 0
    ramp.resize((512, 256), Image.NEAREST).save(os.path.join(ROOT, "build", "bluenoise_ramp.png"))
    print("wrote build/bluenoise.png, build/bluenoise_ramp.png")
