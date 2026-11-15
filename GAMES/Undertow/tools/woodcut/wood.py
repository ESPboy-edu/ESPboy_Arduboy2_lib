"""Woodcut 1-bit drawing kit (reconstructed from the style-tile mockups).

Rules of the look:
  * design on black: white is the ink
  * every mass gets a black halo so shapes read as carved blocks
  * heroes are solid white with carved lines on the shadow (right) side
  * enemies are filled with carved diagonals (hatch) and face left
"""
import math, os
from PIL import Image

ON, OFF = (238, 236, 228), (5, 5, 6)


class C:
    def __init__(s, w=128, h=64):
        s.w, s.h = w, h
        s.p = [[0] * w for _ in range(h)]
        s.a = [[0] * w for _ in range(h)]       # alpha: 1 = pixel is part of the sprite

    def px(s, x, y, v=1):
        if 0 <= x < s.w and 0 <= y < s.h:
            s.p[y][x] = v; s.a[y][x] = 1

    def get(s, x, y):
        return s.p[y][x] if 0 <= x < s.w and 0 <= y < s.h else 0

    def rect(s, x, y, w, h, v=1):
        for j in range(y, y + h):
            for i in range(x, x + w): s.px(i, j, v)

    def frame(s, x, y, w, h, v=1):
        for i in range(x, x + w): s.px(i, y, v); s.px(i, y + h - 1, v)
        for j in range(y, y + h): s.px(x, j, v); s.px(x + w - 1, j, v)

    def line(s, x0, y0, x1, y1, v=1, dash=None):
        x0, y0, x1, y1 = int(round(x0)), int(round(y0)), int(round(x1)), int(round(y1))
        dx, dy = abs(x1 - x0), -abs(y1 - y0)
        sx, sy = (1 if x0 < x1 else -1), (1 if y0 < y1 else -1)
        e, k = dx + dy, 0
        while True:
            if dash is None or dash(k): s.px(x0, y0, v)
            k += 1
            if x0 == x1 and y0 == y1: break
            e2 = 2 * e
            if e2 >= dy: e += dy; x0 += sx
            if e2 <= dx: e += dx; y0 += sy

    def blit(s, x, y, pat, mirror=False, mask=None, v=1, solid=False):
        for j, row in enumerate(pat):
            for i, ch in enumerate(row):
                ii = len(row) - 1 - i if mirror else i
                if ch == '#':
                    if mask is None or mask(x + ii, y + j): s.px(x + ii, y + j, v)
                    elif solid: s.px(x + ii, y + j, 1 - v)
                elif solid: s.px(x + ii, y + j, 1 - v)

    def text(s, x, y, t, v=1, scale=1):
        cx = x
        for ch in t.upper():
            g = FONT.get(ch, FONT['?'])
            for j, row in enumerate(g):
                for i, b in enumerate(row):
                    if b == '#':
                        for a in range(scale):
                            for bb in range(scale): s.px(cx + i * scale + a, y + j * scale + bb, v)
            cx += (len(g[0]) + 1) * scale

    def tw(s, t, scale=1):
        return sum((len(FONT.get(ch, FONT['?'])[0]) + 1) * scale for ch in t.upper()) - scale

    def image(s, transparent=False):
        """Pillow image. transparent=True keeps untouched pixels see-through (FX masked sprite)."""
        if not transparent:
            im = Image.new('RGB', (s.w, s.h))
            im.putdata([ON if s.p[y][x] else OFF for y in range(s.h) for x in range(s.w)])
            return im
        im = Image.new('RGBA', (s.w, s.h))
        im.putdata([((255, 255, 255, 255) if s.p[y][x] else (0, 0, 0, 255)) if s.a[y][x] else (0, 0, 0, 0)
                    for y in range(s.h) for x in range(s.w)])
        return im

    def paste(s, other, x, y, mirror=False):
        for j in range(other.h):
            for i in range(other.w):
                if other.a[j][i]:
                    ii = other.w - 1 - i if mirror else i
                    s.px(x + ii, y + j, other.p[j][i])


F = """
A .#. #.# ### #.# #.#
B ##. #.# ##. #.# ##.
C .## #.. #.. #.. .##
D ##. #.# #.# #.# ##.
E ### #.. ##. #.. ###
F ### #.. ##. #.. #..
G .## #.. #.# #.# .##
H #.# #.# ### #.# #.#
I ### .#. .#. .#. ###
J ..# ..# ..# #.# .#.
K #.# #.# ##. #.# #.#
L #.. #.. #.. #.. ###
M #.# ### ### #.# #.#
N ##. #.# #.# #.# #.#
O .#. #.# #.# #.# .#.
P ##. #.# ##. #.. #..
Q .#. #.# #.# ##. .##
R ##. #.# ##. #.# #.#
S .## #.. .#. ..# ##.
T ### .#. .#. .#. .#.
U #.# #.# #.# #.# ###
V #.# #.# #.# #.# .#.
W #.# #.# ### ### #.#
X #.# #.# .#. #.# #.#
Y #.# #.# .#. .#. .#.
Z ### ..# .#. #.. ###
0 ### #.# #.# #.# ###
1 .#. ##. .#. .#. ###
2 ##. ..# .#. #.. ###
3 ##. ..# .#. ..# ##.
4 #.# #.# ### ..# ..#
5 ### #.. ##. ..# ##.
6 .## #.. ### #.# ###
7 ### ..# .#. .#. .#.
8 ### #.# ### #.# ###
9 ### #.# ### ..# ##.
. . . . . #
: . # . # .
- ... ... ### ... ...
+ ... .#. ### .#. ...
/ ..# ..# .#. #.. #..
? ##. ..# .#. ... .#.
! # # # . #
' # # . . .
, .. .. .. .# #.
> #.. .#. ..# .#. #..
< ..# .#. #.. .#. ..#
"""
FONT = {' ': ['..', '..', '..', '..', '..']}
for ln in F.strip().split('\n'):
    k, *rows = ln.split(' ')
    FONT[k] = rows


# ---------------- masks & fills ----------------
def ellipse(cx, cy, rx, ry):
    return {(x, y) for y in range(int(cy - ry) - 1, int(cy + ry) + 2) for x in range(int(cx - rx) - 1, int(cx + rx) + 2)
            if ((x - cx) / rx) ** 2 + ((y - cy) / ry) ** 2 <= 1.0}


def poly(pts):
    pts = [(round(a), round(b)) for a, b in pts]
    xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
    out = set()
    for y in range(min(ys), max(ys) + 1):
        for x in range(min(xs), max(xs) + 1):
            inside, j = False, len(pts) - 1
            for i in range(len(pts)):
                xi, yi = pts[i]; xj, yj = pts[j]
                if (yi > y + 0.5) != (yj > y + 0.5) and x + 0.5 < (xj - xi) * (y + 0.5 - yi) / (yj - yi) + xi:
                    inside = not inside
                j = i
            if inside: out.add((x, y))
    return out


def rectm(x, y, w, h): return {(i, j) for i in range(x, x + w) for j in range(y, y + h)}


def thick(x0, y0, x1, y1, r):
    """A limb: every pixel within r of the segment."""
    out = set()
    for y in range(int(min(y0, y1) - r) - 1, int(max(y0, y1) + r) + 2):
        for x in range(int(min(x0, x1) - r) - 1, int(max(x0, x1) + r) + 2):
            dx, dy = x1 - x0, y1 - y0
            L = dx * dx + dy * dy
            t = 0 if L == 0 else max(0, min(1, ((x - x0) * dx + (y - y0) * dy) / L))
            if math.hypot(x - (x0 + t * dx), y - (y0 + t * dy)) <= r: out.add((x, y))
    return out


N4 = [(1, 0), (-1, 0), (0, 1), (0, -1)]
N8 = N4 + [(1, 1), (1, -1), (-1, 1), (-1, -1)]


def edge(m):
    return {(x, y) for (x, y) in m if any((x + dx, y + dy) not in m for dx, dy in N4)}


def halo(c, m, r=1):
    for (x, y) in m:
        for dx in range(-r, r + 1):
            for dy in range(-r, r + 1):
                if (x + dx, y + dy) not in m: c.px(x + dx, y + dy, 0)


def fill(c, m, rule=lambda x, y: 1):
    for (x, y) in m: c.px(x, y, 1 if rule(x, y) else 0)


def hatch(x, y): return (x - y) % 3 != 0          # enemy fill: carved diagonals


def carve(cx, shade_from):
    """white mass, carved diagonal lines on the shadow side (x >= shade_from)."""
    def r(x, y):
        if x >= shade_from + 4: return (x - y) % 2 != 0
        if x >= shade_from: return (x - y) % 3 != 0
        return True
    return r


# ---------------- UI pieces ----------------
def plank(c, x, y, w, h=7, text=None, align='c', notch=True):
    c.rect(x, y, w, h, 1)
    if notch:
        mid = y + h // 2
        c.px(x, mid, 0); c.px(x + w - 1, mid, 0)
        c.px(x, y, 0); c.px(x + w - 1, y, 0); c.px(x, y + h - 1, 0); c.px(x + w - 1, y + h - 1, 0)
    if text is not None:
        ty = y + (h - 5) // 2
        if align == 'c': tx = x + (w - c.tw(text)) // 2
        elif align == 'l': tx = x + 3
        else: tx = x + w - 3 - c.tw(text)
        c.text(tx, ty, text, 0)


def sunburst(c, cx, cy, r0, r1, step=10, a0=0, a1=360, box=None, keep_out=set()):
    for a in range(a0, a1, step):
        t = math.radians(a)
        for r in range(r0, r1):
            x, y = round(cx + r * math.cos(t)), round(cy + r * math.sin(t))
            if box and not (box[0] <= x < box[2] and box[1] <= y < box[3]): continue
            if (x, y) in keep_out: continue
            if r > r0 + 14 and (r // 2) % 3 == 0: continue
            c.px(x, y)


def border(c, x=0, y=0, w=128, h=64):
    c.frame(x, y, w, h); c.frame(x + 2, y + 2, w - 4, h - 4)
    for i in range(x + 6, x + w - 4, 6): c.px(i, y + 2, 0); c.px(i, y + h - 3, 0)
    for j in range(y + 6, y + h - 4, 6): c.px(x + 2, j, 0); c.px(x + w - 3, j, 0)


def shield_mask(x0, y0, w, h):
    m = set()
    for j in range(h):
        t = j / (h - 1)
        hw = w / 2 if t < 0.5 else (w / 2) * math.sqrt(max(0, 1 - ((t - 0.5) / 0.5) ** 2))
        for i in range(w):
            if abs(i + 0.5 - w / 2) <= hw: m.add((x0 + i, y0 + j))
    return m


# ---------------- 8x8 map sprites (face right) ----------------
SPR = {
 'lance': ["......#.", "..##..#.", "..##..#.", ".####.#.", "#.##.##.", "..##..#.", ".#..#.#.", ".#..#..."],
 'sword': ["........", "..##....", "..##...#", ".####.#.", "#.##.#..", "..###...", ".#..#...", ".#..#..."],
 'bow':   ["....#...", "..##.#..", "..##..#.", ".#####..", "..##..#.", "..##.#..", ".#..#...", ".#..#..."],
 'axe':   ["......#.", "..##.###", "..##.###", ".####.#.", "#.##..#.", "..##..#.", ".#..#...", ".#..#..."],
 'knight':["..##....", ".####...", ".#.##..#", "######.#", "######.#", "######.#", ".##.##..", ".##.##.."],
 'horse': ["...##..#", "...##.#.", "..####..", "##.##...", "#######.", ".######.", ".#.#.#..", ".#.#.#.."],
 'boss':  ["..#.#...", "..###...", ".#.#.#..", "#######.", "######.#", "#####.##", ".##.##.#", ".##.##.."],
}
