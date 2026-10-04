#!/usr/bin/env python3
"""A small renderer for the stage's sprites (tools/stage.py uses it).

A figure is a set of rounded parts in the plane of the picture, each at its
own depth: capsules for limbs, ellipses for heads, outlines for torsos,
guitar bodies and drums. Every part has a rounded profile (the height of a
capsule over its axis, a bevel round an outline), and a key light from the
upper left like the stage's spotlights, a fill from the front and a rim
light from behind fall on that; glossy parts shine. What lies behind a part
gets a dark line along the part's edge. All of it is rendered at SS times
the size and brought down to the sprite's pixels in one of two styles:

- shaded: the shades become dither patterns of five steps (for figures seen
  close up, like the band of 2026-09-30 kept under the tag band-closeup);
- outline: black shapes with a white edge, the way figures look against a
  lit stage from the crowd, a part marked white filled white (a guitar, a
  cymbal); parts of one object (a person) share an id, so that only the
  edge of what is in front is drawn where one object covers another.

In both the sprite's mask is a pixel wider than the figure, so that it
stands clear of the light behind it. sprite_bytes packs a frame the way
the game reads it off the chip.
"""
import math
from math import cos, hypot, sin, sqrt

SS = 4                                  # rendered at this many times the size

# ------------------------------------------------------------------ shapes
# signed distance in the plane, negative inside, and a bounding box


def seg_d(px, py, ax, ay, bx, by):
    vx, vy = bx - ax, by - ay
    wx, wy = px - ax, py - ay
    ll = vx * vx + vy * vy
    t = 0.0 if ll == 0 else (wx * vx + wy * vy) / ll
    if t < 0:
        t = 0.0
    elif t > 1:
        t = 1.0
    dx, dy = wx - t * vx, wy - t * vy
    return sqrt(dx * dx + dy * dy)


class Capsule:
    def __init__(self, a, b, r):
        self.a, self.b, self.r = a, b, r

    def d(self, x, y):
        return seg_d(x, y, self.a[0], self.a[1], self.b[0], self.b[1]) - self.r

    def bbox(self):
        r = self.r
        return (min(self.a[0], self.b[0]) - r, min(self.a[1], self.b[1]) - r,
                max(self.a[0], self.b[0]) + r, max(self.a[1], self.b[1]) + r)


class Ellipse:
    def __init__(self, c, rx, ry, angle=0.0):
        self.c, self.rx, self.ry = c, rx, ry
        self.ca, self.sa = cos(angle), sin(angle)

    def d(self, x, y):
        px, py = x - self.c[0], y - self.c[1]
        qx = px * self.ca + py * self.sa
        qy = -px * self.sa + py * self.ca
        return (hypot(qx / self.rx, qy / self.ry) - 1.0) * min(self.rx, self.ry)

    def bbox(self):
        r = max(self.rx, self.ry)
        return (self.c[0] - r, self.c[1] - r, self.c[0] + r, self.c[1] + r)


class Poly:
    """a closed outline, its corners rounded by r"""

    def __init__(self, pts, r=0.0):
        self.pts, self.r = list(pts), r

    def d(self, x, y):
        pts = self.pts
        n = len(pts)
        inside = False
        best = 1e9
        j = n - 1
        for i in range(n):
            ax, ay = pts[i]
            bx, by = pts[j]
            if (ay > y) != (by > y) and x < (bx - ax) * (y - ay) / (by - ay) + ax:
                inside = not inside
            e = seg_d(x, y, ax, ay, bx, by)
            if e < best:
                best = e
            j = i
        return (-best if inside else best) - self.r

    def bbox(self):
        xs = [p[0] for p in self.pts]
        ys = [p[1] for p in self.pts]
        r = self.r
        return (min(xs) - r, min(ys) - r, max(xs) + r, max(ys) + r)


class Union:
    """the shapes together, joined smoothly over k"""

    def __init__(self, shapes, k=0.0):
        self.shapes, self.k = shapes, k

    def d(self, x, y):
        k = self.k
        m = 1e9
        for s in self.shapes:
            v = s.d(x, y)
            if k > 0:
                h = k - abs(m - v)
                if h > 0:
                    m = min(m, v) - h * h / (4 * k)
                    continue
            if v < m:
                m = v
        return m

    def bbox(self):
        bb = [s.bbox() for s in self.shapes]
        return (min(b[0] for b in bb), min(b[1] for b in bb), max(b[2] for b in bb), max(b[3] for b in bb))


class Scaled:
    """a shape drawn k times smaller (or bigger), moved by off"""

    def __init__(self, shape, k, off=(0.0, 0.0)):
        self.shape, self.k, self.off = shape, k, off

    def d(self, x, y):
        return self.shape.d((x - self.off[0]) / self.k, (y - self.off[1]) / self.k) * self.k

    def bbox(self):
        x0, y0, x1, y1 = self.shape.bbox()
        k, (ox, oy) = self.k, self.off
        return (x0 * k + ox, y0 * k + oy, x1 * k + ox, y1 * k + oy)


# ------------------------------------------------------------------ light
def norm3(v):
    ll = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2])
    return (v[0] / ll, v[1] / ll, v[2] / ll)


KEY = norm3((-0.45, -0.75, 0.55))       # the spotlight: from the upper left, in front (y down)
RIM = norm3((0.35, -0.6, -0.72))        # the other one, behind and above to the right
HALF = norm3((KEY[0], KEY[1], KEY[2] + 1.0))
CONTRAST = 1.8                          # the shades pushed apart: lit goes white, shadow black,
MID = 0.42                              # only a narrow band of half tones dithered (the screen has two colours)


def height(d, r):
    """how far a rounded part stands out of its plane at a distance d inside its outline"""
    if d >= 0:
        return 0.0
    if d <= -r:
        return r
    return sqrt(r * r - (r + d) * (r + d))


class Part:
    __slots__ = ("shape", "z", "r", "albedo", "gloss", "style", "obj")

    def __init__(self, shape, z, r, albedo, gloss=0.0, style=None, obj=0):
        self.shape, self.z, self.r, self.albedo, self.gloss = shape, z, r, albedo, gloss
        self.style, self.obj = style, obj      # style: None, or "white" in the outline style


def shade(n, albedo, gloss):
    kd = n[0] * KEY[0] + n[1] * KEY[1] + n[2] * KEY[2]
    kr = n[0] * RIM[0] + n[1] * RIM[1] + n[2] * RIM[2]
    lum = albedo * (0.06 + 1.0 * max(0.0, kd) + 0.18 * max(0.0, n[2]))
    if gloss:
        kh = n[0] * HALF[0] + n[1] * HALF[1] + n[2] * HALF[2]
        if kh > 0:
            lum += gloss * kh ** 40
    if kr > 0:
        lum += 0.7 * kr * kr * (0.3 + 0.7 * albedo)
    return lum


def render(parts, w, h, shading=True):
    """-> (owner, lum): for every subpixel the part in front (or None) and its shade"""
    W, H = w * SS, h * SS
    zb = [[-1e9] * W for _ in range(H)]
    own = [[None] * W for _ in range(H)]
    for part in parts:
        x0, y0, x1, y1 = part.shape.bbox()
        i0, i1 = max(0, int(x0 * SS)), min(W, int(math.ceil(x1 * SS)) + 1)
        j0, j1 = max(0, int(y0 * SS)), min(H, int(math.ceil(y1 * SS)) + 1)
        d = part.shape.d
        z, r = part.z, part.r
        for j in range(j0, j1):
            y = (j + 0.5) / SS
            row_z, row_o = zb[j], own[j]
            for i in range(i0, i1):
                v = d((i + 0.5) / SS, y)
                if v >= 0:
                    continue
                zz = z + height(v, r)
                if zz > row_z[i]:
                    row_z[i] = zz
                    row_o[i] = part
    lum = [[0.0] * W for _ in range(H)]
    if not shading:
        return own, lum
    e = 0.3 / SS
    for j in range(H):
        y = (j + 0.5) / SS
        row_o, row_l = own[j], lum[j]
        for i in range(W):
            part = row_o[i]
            if part is None:
                continue
            x = (i + 0.5) / SS
            d, r = part.shape.d, part.r
            gx = (height(d(x + e, y), r) - height(d(x - e, y), r)) / (2 * e)
            gy = (height(d(x, y + e), r) - height(d(x, y - e), r)) / (2 * e)
            n = norm3((-gx, -gy, 1.0))
            row_l[i] = shade(n, part.albedo, part.gloss)
    # a dark line on what lies behind a part, along the part's edge: the
    # shadow it casts, and what keeps an arm apart from the body
    g = 2
    for j in range(H):
        row_o, row_l = own[j], lum[j]
        for i in range(W):
            part = row_o[i]
            if part is None:
                continue
            zt = part.z + 1.0
            if (i >= g and row_o[i - g] is not None and row_o[i - g].z > zt) or \
                    (i < W - g and row_o[i + g] is not None and row_o[i + g].z > zt) or \
                    (j >= g and own[j - g][i] is not None and own[j - g][i].z > zt) or \
                    (j < H - g and own[j + g][i] is not None and own[j + g][i].z > zt):
                row_l[i] *= 0.2
    return own, lum


BAYER = ((0, 2), (3, 1))                # five shades: black, a dot in four, the checkerboard, three, white


def dilate(cov, w, h):
    out = [[0] * w for _ in range(h)]
    for y in range(h):
        for x in range(w):
            if cov[y][x] or (x and cov[y][x - 1]) or (x < w - 1 and cov[y][x + 1]) \
                    or (y and cov[y - 1][x]) or (y < h - 1 and cov[y + 1][x]):
                out[y][x] = 1
    return out


def to_bits(own, lum, w, h, style="shaded", cover=0.5):
    """-> (image, mask) as rows of 0/1; cover: how much of a pixel a figure
    must fill to count (less for small figures, whose limbs are thin)"""
    cov = [[0] * w for _ in range(h)]
    img = [[0] * w for _ in range(h)]
    front = [[None] * w for _ in range(h)]          # the part most seen in the pixel
    for y in range(h):
        for x in range(w):
            n, s = 0, 0.0
            seen = {}
            for jj in range(SS):
                ro, rl = own[y * SS + jj], lum[y * SS + jj]
                for ii in range(SS):
                    p = ro[x * SS + ii]
                    if p is not None:
                        n += 1
                        s += rl[x * SS + ii]
                        seen[p] = seen.get(p, 0) + 1
            if n >= cover * SS * SS:
                cov[y][x] = 1
                front[y][x] = max(seen, key=lambda p: (seen[p], p.z))
                if style == "shaded":
                    v = (s / n - MID) * CONTRAST + 0.5
                    img[y][x] = 1 if v > (BAYER[y & 1][x & 1] + 0.5) / 4.0 else 0
    if style == "outline":                          # black shapes with a white edge
        for y in range(h):
            for x in range(w):
                if not cov[y][x]:
                    continue
                p = front[y][x]
                if p.style == "white":
                    img[y][x] = 1
                    continue
                for nx, ny in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)):
                    if nx < 0 or ny < 0 or nx >= w or ny >= h:
                        continue                    # the sprite's border is not an edge
                    if not cov[ny][nx]:
                        img[y][x] = 1               # the edge of everything
                        break
                    q = front[ny][nx]
                    if q.obj != p.obj and q.z < p.z:
                        img[y][x] = 1               # the edge of what is in front
                        break
    elif style == "solid":                          # white shapes, a black line where one lies over another
        for y in range(h):
            for x in range(w):
                if not cov[y][x]:
                    continue
                img[y][x] = 1
                p = front[y][x]
                for nx, ny in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)):
                    if 0 <= nx < w and 0 <= ny < h and cov[ny][nx]:
                        q = front[ny][nx]
                        if q.obj != p.obj and q.z > p.z:
                            img[y][x] = 0           # next to the edge of what is in front
                            break
    return img, dilate(cov, w, h)


class Flip:
    """a shape mirrored left to right in a box w wide"""

    def __init__(self, shape, w):
        self.shape, self.w = shape, w

    def d(self, x, y):
        return self.shape.d(self.w - x, y)

    def bbox(self):
        x0, y0, x1, y1 = self.shape.bbox()
        return (self.w - x1, y0, self.w - x0, y1)


# ------------------------------------------------------------------ sprites
def sprite_bytes(img, msk, w, h):
    """-> the frame's bytes: for each row of eight pixels (a page, the top row
    in bit 0) the image a column at a time, then the mask the same way, so
    that the game reads a page's image and mask off the chip in one go"""
    out = bytearray()
    for page in range(h // 8):
        for rows in (img, msk):
            for x in range(w):
                v = 0
                for i in range(8):
                    if rows[page * 8 + i][x]:
                        v |= 1 << i
                out.append(v)
    return bytes(out)


def text_frame(rows, w, h):
    """-> (image, mask) from art drawn as text: '#' white, '.' black, ' ' see-through"""
    assert len(rows) <= h and all(len(r) <= w for r in rows), "text art bigger than %dx%d" % (w, h)
    img = [[1 if x < len(r) and r[x] == "#" else 0 for x in range(w)] for r in rows]
    msk = [[1 if x < len(r) and r[x] != " " else 0 for x in range(w)] for r in rows]
    while len(img) < h:
        img.append([0] * w)
        msk.append([0] * w)
    return img, msk
