#!/usr/bin/env python3
"""The play page's stage: every camera a photograph of the concert in the
screen's two colours (the look Joel chose, v4 with blue noise), animated:
one frame of 128 x 64 for every position of the lights, every pose of the
band and every pose of the crowd. The frames live on the flash chip and the
game copies the one it wants straight into the screen buffer, under the
highway and the hud it draws itself.

How a frame is made. The light first, in grey at four times the size: the
haze, spots and beams from lamps above the frame, glows, the fog lit at the
front of the stage. Then the stage and the people as black shapes cut out
of it, built to a real body's proportions (100 units from the top of the
head to the soles: seven and a half heads, slim arms and legs of the right
length, boots seen from the front), the drummer behind a kit of real drum
sizes, the amps and mic stands to the players' scale. A shape gets a white
rim only where it stands against dark, a backlit player a one-pixel glare
round him, and seen close the guitar and the arms a thin line of light
where they cross the body. The picture is brought down to the screen and
dithered with blue noise (tools/bluenoise.py): a pixel's threshold never
changes, so between two frames only what moves flips.

The animation, the same for every camera:
  light   LIGHTS positions of the swinging lights; the game steps them with
          time, twice as fast with star power on
  band    BAND_STATES poses: 0 and 1 the two beats (the weight on one foot,
          then the other; the bassist's head up, then down; the drummer's
          sticks up, then down), 2 and 3 the same with the guitarist's strum
          (after a hit note), 4 a jump (star power, on the beat); the light
          pulses a little on the second beat
  crowd   for a camera that shows the crowd: fists low, then up
A camera's frames, one after the other: for each light, for each band
pose, for each crowd pose; a frame's place is
((light * BAND_STATES + band) * crowds + crowd) * 1024.

  python3 tools/scene.py       renders (or takes the frames from build/scene_cache.json)
                               and writes build/scene_<camera>.png, every frame at 2x
  python3 tools/scene.py -f    renders afresh
"""
import hashlib
import json
import math
import os
import random
import sys
from math import cos, sin, tan

from PIL import Image, ImageDraw, ImageMath

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import bluenoise  # noqa: E402

W, H = 128, 64
S = 4                                   # shapes are drawn at four times the size
CW, CH = W * S, H * S
LIGHTS = 4                              # positions of the swinging lights (a power of two)
BAND_STATES = 5
SWAY = (0.0, 1.0, 0.0, -1.0)            # the lights' swing over the LIGHTS positions...
SWING = (1.0, 0.0, -1.0, 0.0)           # ...and the same a quarter of a swing later


def clamp(v, lo=0.0, hi=1.0):
    return lo if v < lo else hi if v > hi else v


def lam(fn, **kw):
    return ImageMath.lambda_eval(fn, **kw)


# ------------------------------------------------------------------ canvas
class Canvas:
    """the light, in grey at four times the size (Pillow float images), and
    what is fixed at the screen's size: black shapes, white rims and glints
    (None where the dither decides)"""

    def __init__(self):
        self.v = Image.new("F", (CW, CH), 0.0)
        self.fix = [None] * (W * H)

    def add(self, m, gain):
        """light: v += m * gain (m an 'L' mask or an 'F' field)"""
        if m.mode == "F":
            self.v = lam(lambda a: a["v"] + a["m"] * gain, v=self.v, m=m)
        else:
            k = gain / 255.0
            self.v = lam(lambda a: a["v"] + a["m"] * k, v=self.v, m=m.convert("F"))

    def paint(self, m, level):
        """something in front: v = level where m covers"""
        self.v = lam(lambda a: a["v"] + (a["m"] * (1 / 255.0)) * (level - a["v"]), v=self.v, m=m.convert("F"))

    def paint_field(self, m, f):
        self.v = lam(lambda a: a["v"] + (a["m"] * (1 / 255.0)) * (a["f"] - a["v"]), v=self.v, m=m.convert("F"), f=f)

    def grey(self):
        return list(self.v.resize((W, H), Image.BOX).get_flattened_data())

    @staticmethod
    def cover(m):
        return [p / 255.0 for p in m.resize((W, H), Image.BOX).get_flattened_data()]

    def bg(self, g, i):
        f = self.fix[i]
        return g[i] if f is None else f

    def black(self, m, rims=(), level=0.0, thresh=0.5, dark=0.42):
        """a black shape (4x mask m): fixed black where it covers `thresh` of a
        pixel; rims: (dx, dy, width), the light's direction; a pixel on the
        edge towards the light turns white where what lies beyond it is dark
        (a rim pixel on its own is a speck, not an edge, and is dropped)"""
        g = self.grey()
        self.paint(m, level)
        cov = self.cover(m)
        inside = [cv >= thresh for cv in cov]
        fix = self.fix
        rim = set()
        for dx, dy, width in rims:
            ll = math.hypot(dx, dy)
            sx, sy = int(round(dx / ll)), int(round(dy / ll))
            for i, ins in enumerate(inside):
                if not ins:
                    continue
                x, y = i % W, i // W
                for w in range(1, width + 1):
                    qx, qy = x + sx * w, y + sy * w
                    if not (0 <= qx < W and 0 <= qy < H):
                        break
                    q = qy * W + qx
                    if inside[q]:
                        continue
                    if self.bg(g, q) < dark:
                        rim.add(i)
                    break
        keep = set()
        for i in rim:
            x, y = i % W, i // W
            for ddy in (-1, 0, 1):
                for ddx in (-1, 0, 1):
                    if (ddx or ddy) and 0 <= x + ddx < W and (y + ddy) * W + (x + ddx) in rim:
                        keep.add(i)
        for i, ins in enumerate(inside):
            if ins:
                fix[i] = 1 if i in keep else 0
        return inside

    def white(self, m, thresh=0.4, level=1.0):
        """something lit white in front: painted, and fixed white where it covers"""
        self.paint(m, level)
        for i, cv in enumerate(self.cover(m)):
            if cv >= thresh:
                self.fix[i] = 1

    def soft(self, m, thresh=0.5):
        """back to the dither's choice where m covers"""
        for i, cv in enumerate(self.cover(m)):
            if cv >= thresh:
                self.fix[i] = None

    def dot(self, x, y, v):
        x, y = int(round(x)), int(round(y))
        if 0 <= x < W and 0 <= y < H:
            self.fix[y * W + x] = v

    def pixline(self, a, b, v=1):
        """a crisp line one pixel wide straight on the screen (Bresenham)"""
        x0, y0 = int(round(a[0])), int(round(a[1]))
        x1, y1 = int(round(b[0])), int(round(b[1]))
        dx, dy = abs(x1 - x0), -abs(y1 - y0)
        sx, sy = (1 if x0 < x1 else -1), (1 if y0 < y1 else -1)
        err = dx + dy
        while True:
            self.dot(x0, y0, v)
            if x0 == x1 and y0 == y1:
                break
            e2 = 2 * err
            if e2 >= dy:
                err += dy
                x0 += sx
            if e2 <= dx:
                err += dx
                y0 += sy


class D:
    """draws into a 4x mask in screen units"""

    def __init__(self, m=None):
        self.m = m or Image.new("L", (CW, CH), 0)
        self.d = ImageDraw.Draw(self.m)

    def ellipse(self, cx, cy, rx, ry, v=255):
        self.d.ellipse((cx * S - rx * S, cy * S - ry * S, cx * S + rx * S, cy * S + ry * S), fill=v)

    def poly(self, pts, v=255):
        self.d.polygon([(x * S, y * S) for x, y in pts], fill=v)

    def rect(self, x0, y0, x1, y1, v=255):
        self.d.rectangle((x0 * S, y0 * S, x1 * S - 1, y1 * S - 1), fill=v)

    def line(self, a, b, w=1.0, v=255):
        self.d.line([(a[0] * S, a[1] * S), (b[0] * S, b[1] * S)], fill=v, width=max(1, int(round(w * S))))
        r = w * S / 2.0
        for x, y in (a, b):
            self.d.ellipse((x * S - r, y * S - r, x * S + r, y * S + r), fill=v)


class Shape(D):
    """a black shape with lit edges, drawn as a mask and then put on the canvas"""

    def paint(self, c, rims=(), level=0.0, thresh=0.5, dark=0.42):
        return c.black(self.m, rims, level, thresh, dark)


def ell_pts(cx, cy, rx, ry, ang=0.0, n=28):
    ca, sa = cos(ang), sin(ang)
    out = []
    for i in range(n):
        t = 2 * math.pi * i / n
        ex, ey = rx * cos(t), ry * sin(t)
        out.append((cx + ex * ca - ey * sa, cy + ex * sa + ey * ca))
    return out


# ------------------------------------------------------------------ light
def field(fn, bbox=None, ss=2):
    """a light field: fn(x, y) in screen units, sampled ss x ss a pixel,
    returned as a 4x float image"""
    x0, y0, x1, y1 = (0, 0, W, H) if bbox is None else bbox
    x0, y0 = max(0, int(math.floor(x0))), max(0, int(math.floor(y0)))
    x1, y1 = min(W, int(math.ceil(x1))), min(H, int(math.ceil(y1)))
    data = [0.0] * (W * H)
    offs = [((i + 0.5) / ss, (j + 0.5) / ss) for j in range(ss) for i in range(ss)]
    n = float(len(offs))
    for y in range(y0, y1):
        for x in range(x0, x1):
            s = 0.0
            for ox, oy in offs:
                s += fn(x + ox, y + oy)
            data[y * W + x] = s / n
    im = Image.new("F", (W, H))
    im.putdata(data)
    return im.resize((CW, CH), Image.BILINEAR)


def beam(c, lx, ly, ang, spread=0.1, power=1.0, length=40.0, w0=1.0, floor=None, profile=1.0):
    """a cone of light in the haze from (lx, ly), ang from straight down
    (positive to the right); it fades along its length and stops at the floor"""
    ux, uy = sin(ang), cos(ang)
    ts = tan(spread)

    def f(px, py):
        dx, dy = px - lx, py - ly
        t = dx * ux + dy * uy
        if t <= 0:
            return 0.0
        s = dx * uy - dy * ux
        w = w0 + t * ts
        a = 1.0 - (s * s) / (w * w)
        if a <= 0:
            return 0.0
        fade = 1.0
        if floor is not None and py > floor - 1.0:
            fade = clamp((floor + 1.0 - py) / 2.0)
        return power * (a ** profile) * fade / (1.0 + t / length)
    c.add(field(f), 1.0)


def spot(c, x, top, floor, w_floor, power=1.3, w_top=1.5, profile=0.8, length=70.0, lean=0.0):
    """a spot from a lamp above the frame down to the floor, w_floor wide at the
    floor; bright in its middle, falling off to its edges and along its length"""
    ang = math.atan2(lean, floor - top)
    spread = math.atan(max(0.01, (w_floor / 2.0 - w_top) / (floor - top)))
    beam(c, x, top, ang, spread=spread, power=power, length=length, w0=w_top, floor=floor, profile=profile)


def glow(c, x, y, r, gain, falloff=2.0, sx=1.0, sy=1.0):
    """a soft light round a point: gain at the middle, half of it at r"""
    def f(px, py):
        d = math.hypot((px - x) / sx, (py - y) / sy) / r
        return gain / (1.0 + d ** falloff)
    c.add(field(f, (x - r * 8 * sx, y - r * 8 * sy, x + r * 8 * sx, y + r * 8 * sy)), 1.0)


def haze(c, top, bottom, y0, y1):
    def f(px, py):
        t = clamp((py - y0) / float(y1 - y0))
        return top + (bottom - top) * t
    c.add(field(f, ss=1), 1.0)


def pool(c, x, y, rx, ry, gain):
    """light on the floor where a beam lands"""
    glow(c, x, y, 1.0, gain, 4.0, sx=rx, sy=ry)


def lamp(c, x, y, power=1.0, rays=4.0, halo_r=3.0):
    """a lamp seen from the front: the burnt-out middle, a halo, and a lens
    star (four short rays), fixed white where it is brightest"""
    glow(c, x, y, halo_r * 0.5, 0.9 * power, 2.0)
    m = D()
    m.ellipse(x, y, 1.1, 0.9)
    c.white(m.m, 0.35)
    for ang in (0.0, math.pi / 2, math.pi, 3 * math.pi / 2):
        for k in range(1, int(rays * power) + 1):
            if k <= rays * power * 0.6:
                c.dot(x + cos(ang) * (k + 0.6), y + sin(ang) * (k + 0.6), 1)


def starburst(c, x, y, r, power=2.0, rays=8, ray_len=20.0, ray_w=0.55, turn=0.2, halo_k=2.4):
    """a lamp aimed at the camera: a burnt-out disc, a wide halo and the lens's
    star of thin rays, every other one shorter"""
    angs = [turn + i * 2 * math.pi / rays for i in range(rays)]

    def f(px, py):
        dx, dy = px - x, py - y
        d = math.hypot(dx, dy)
        v = power / (1.0 + (d / (r * halo_k)) ** 2) * 0.55
        if d <= r:
            v += power
        elif d < r + 1.0:
            v += power * (r + 1.0 - d)
        for i, a in enumerate(angs):
            ux, uy = cos(a), sin(a)
            t = dx * ux + dy * uy
            if t <= 0:
                continue
            s = abs(dx * uy - dy * ux)
            ln = ray_len * (1.0 if i % 2 == 0 else 0.55)
            if t > ln or s > ray_w * 3:
                continue
            v += power * 0.9 * math.exp(-(s / ray_w) ** 2) * (1.0 - t / ln)
        return v
    reach = ray_len + 2
    c.add(field(f, (x - reach, y - reach, x + reach, y + reach)), 1.0)


def smooth(e0, e1, v):
    t = clamp((v - e0) / (e1 - e0)) if e1 != e0 else (1.0 if v >= e0 else 0.0)
    return t * t * (3 - 2 * t)


def fog_band(c, y, sigma, gain=1.3, x0=0, x1=W):
    """the fog at the front of the stage, lit from the floor"""
    def f(px, py):
        d = (py - y) / sigma
        return gain * math.exp(-d * d)
    c.add(field(f, (x0, y - sigma * 3, x1, y + sigma * 3)), 1.0)


# ------------------------------------------------------------------ the set
def boxlit(c, x0, y0, x1, y1, level, seams=(), outline=True):
    """a box lit from the front (an amp): a grey face the dither fills, black
    seams across it, a black edge round it"""
    m = D()
    m.rect(x0, y0, x1, y1)
    c.paint(m.m, level)
    c.soft(m.m, 0.5)
    for y in seams:
        c.pixline((x0, y), (x1 - 1, y), 0)
    if outline:
        c.pixline((x0, y0), (x0, y1 - 1), 0)
        c.pixline((x1 - 1, y0), (x1 - 1, y1 - 1), 0)
        c.pixline((x0, y1 - 1), (x1 - 1, y1 - 1), 0)


def amp_wall(c, x0, top, floor, w, level=0.28):
    """a full stack to the players' scale: a head and two cabinets, lit from the
    front, black seams, the head's top lit"""
    head = (floor - top) * 0.14
    boxlit(c, x0 + 0.5, top, x0 + w - 0.5, top + head, level * 0.7)
    cab = (floor - top - head) / 2.0
    boxlit(c, x0, top + head, x0 + w, floor, level, seams=(top + head + cab,))
    c.pixline((x0 + 1, top), (x0 + w - 2, top), 1)


def riser(c, x0, x1, top, bottom):
    s = Shape()
    s.rect(x0, top, x1, bottom)
    s.paint(c)
    c.pixline((x0, top), (x1 - 1, top), 1)             # the lit front edge


def mic_stand(c, x, floor, height, boom=3.0):
    top = (x, floor - height)
    c.pixline((x, floor), top, 0)
    c.pixline((x - 2, floor), (x + 2, floor), 0)
    c.pixline(top, (top[0] + boom, top[1] - 1), 0)


# ------------------------------------------------------------------ people
class Rig:
    """a figure's units on the screen: its (0, 0) (the top of the head) at
    screen (x, y), k pixels a unit; flip mirrors it; low > 0 stretches what
    is lower down (seen from below)"""

    def __init__(self, x, y, k, flip=False, low=0.0):
        self.x, self.y, self.k, self.flip, self.low = x, y, k, flip, low

    def __call__(self, u, v):
        v = v * (1.0 + self.low * v / 100.0)
        return (self.x + (-u if self.flip else u) * self.k, self.y + v * self.k)

    def ang(self, a):
        return math.pi - a if self.flip else a


def taper(s, a, b, wa, wb):
    """a limb from a to b (screen), wa wide at a and wb at b, rounded ends"""
    dx, dy = b[0] - a[0], b[1] - a[1]
    ll = math.hypot(dx, dy) or 1e-6
    nx, ny = -dy / ll, dx / ll
    s.poly([(a[0] + nx * wa / 2, a[1] + ny * wa / 2), (b[0] + nx * wb / 2, b[1] + ny * wb / 2),
            (b[0] - nx * wb / 2, b[1] - ny * wb / 2), (a[0] - nx * wa / 2, a[1] - ny * wa / 2)])
    s.ellipse(a[0], a[1], wa / 2, wa / 2)
    s.ellipse(b[0], b[1], wb / 2, wb / 2)


LP_BODY = [(0, -2.4), (-3, -5.2), (-8, -6.8), (-13, -6.4), (-16.6, -3.4), (-17.2, 1.0), (-15.4, 5.4), (-11, 7.6),
           (-6.5, 7.2), (-3.4, 5.6), (-1.6, 6.4), (1.2, 5.6), (0.3, 3.4), (0, 2.4)]
BASS_BODY = [(0, -2.6), (-1.2, -3.8), (0.6, -6.2), (3.0, -7.4), (1.0, -8.6), (-4, -8.8), (-9, -7.8), (-14, -5.6),
             (-17.8, -1.4), (-17.8, 2.8), (-15, 6.6), (-10.5, 8.0), (-6, 7.6), (-3, 5.8), (-1.6, 6.4), (0.8, 5.4),
             (-0.4, 3.6), (0, 2.6)]


def guitar(s, R, centre, ang, bass=False):
    """a guitar or a bass into s: its body's middle at centre (units), the neck
    at ang (screen sense, from the player's right hand side up to the left
    hand); a Les Paul's body 25 units long, a bass's 28, necks to scale"""
    body, gs, neck, head = (BASS_BODY, 1.55, 34.0, 12.0) if bass else (LP_BODY, 1.36, 24.0, 9.0)
    a = R.ang(ang)
    ca, sa = cos(a), sin(a)
    cx, cy = R(*centre)
    k = R.k
    ux, uy = cx + 8.0 * gs * k * ca, cy + 8.0 * gs * k * sa         # the neck's joint (u = 0)

    def at(u, v):
        u, v = u * gs * k, v * gs * k * (-1 if R.flip else 1)
        return (ux + u * ca - v * sa, uy + u * sa + v * ca)

    def along(d, w=0.0):
        d, w = d * k, w * k * (-1 if R.flip else 1)
        return (ux + d * ca - w * sa, uy + d * sa + w * ca)
    s.poly([at(u, v) for u, v in body])
    taper(s, along(-1.0), along(neck), 3.0 * k, 2.4 * k)
    s.poly([along(neck - 0.5, -1.6), along(neck + head, -2.6), along(neck + head + 0.5, 2.0), along(neck - 0.5, 1.6)])


# poses, in units (x to the viewer's right as the player faces us): the
# right hand strums (viewer's left), the left hand frets; knees bent a
# little and out, the guitar's body at the hips sticking out on the
# strumming side, the neck up. The bassist is left-handed: the same poses,
# mirrored.
STANCE = dict(head=(0.6, 6.8), tilt=0.18, face=1,
              sh=((-11.0, 21.0), (11.0, 21.0)), el=((-18.5, 35.0), (19.5, 36.0)), hd=((-4.0, 49.5), (11.0, 36.3)),
              hip=((-5.0, 48.0), (5.0, 48.0)), kn=((-12.0, 71.0), (12.5, 70.5)), an=((-15.5, 95.0), (17.0, 95.0)),
              guitar=((-7.0, 52.0), -0.72))
STANCE_B = dict(STANCE, head=(1.6, 7.0), tilt=0.30,                      # the weight on the other foot
                hip=((-4.4, 48.0), (5.6, 48.0)), kn=((-11.0, 71.5), (13.8, 70.0)),
                el=((-18.0, 35.5), (19.5, 36.2)), hd=((-3.6, 50.0), (10.7, 35.8)), guitar=((-6.6, 52.3), -0.76))
JUMP = dict(STANCE, head=(0.0, 6.4), tilt=0.25,                          # off the floor, the feet tucked
            el=((-18.5, 33.0), (17.5, 35.0)), hd=((-4.0, 47.5), (8.8, 33.1)),
            kn=((-12.5, 65.0), (13.0, 64.0)), an=((-13.0, 86.0), (14.0, 85.5)), guitar=((-7.0, 51.0), -0.85))
JUMP_LIFT = 7.0                                                          # units the jump lifts him
NOD = dict(STANCE, head=(1.8, 9.6), tilt=-0.35, face=0)                  # the bassist's head down on the beat
HEADBANG = dict(head=(2.0, 12.5), tilt=-0.5, face=0,
                sh=((-11.0, 22.5), (11.0, 22.5)), el=((-17.5, 36.5), (18.5, 34.0)), hd=((-4.2, 48.3), (10.2, 29.4)),
                hip=((-5.0, 48.0), (5.0, 48.0)), kn=((-12.5, 71.0), (13.0, 70.5)), an=((-16.5, 95.0), (18.0, 95.0)),
                guitar=((-7.0, 52.0), -0.92))
HEADBANG_UP = dict(HEADBANG, head=(1.2, 8.4), tilt=0.08, sh=((-11.0, 21.5), (11.0, 21.5)))
HEADBANG_DEEP = dict(HEADBANG, head=(3.0, 15.0), tilt=-0.8, sh=((-11.0, 23.5), (11.0, 23.5)))


def strummed(pose):
    """the same pose with the picking hand down through the strings"""
    (el, el2), (hd, hd2) = pose["el"], pose["hd"]
    return dict(pose, el=((el[0] + 0.8, el[1] + 2.5), el2), hd=((hd[0] + 1.6, hd[1] + 5.0), hd2))


def hair_shape(s, R, hc, style, tilt):
    """the hair round a head whose middle is hc (units)"""
    if style == "long":                                         # straight, swept back off the face, past the shoulders
        pts = [(-5.4, -4.6), (-2.6, -7.2), (2.0, -7.4), (4.6, -5.6), (5.0, -3.0), (2.6, -3.4), (1.0, -1.0),
               (-1.0, 4.0), (-2.4, 9.0), (-4.6, 15.5), (-7.4, 21.0), (-9.0, 16.0), (-7.8, 8.0), (-6.6, 1.0)]
    elif style == "mane":                                       # a full mane, down to the chest on both sides
        pts = [(-5.8, -4.4), (-2.8, -7.6), (2.4, -7.6), (5.8, -4.4), (6.8, 1.0), (7.6, 8.0), (8.6, 16.0), (8.0, 20.5),
               (5.0, 19.0), (4.0, 10.0), (-4.0, 10.0), (-5.0, 19.0), (-8.0, 20.5), (-8.6, 16.0), (-7.6, 8.0),
               (-6.8, 1.0)]
    elif style == "bang":                                       # long hair thrown forward over the face
        pts = [(-5.2, -5.6), (-1.0, -7.6), (4.0, -6.6), (7.0, -2.0), (8.6, 6.0), (9.4, 15.0), (8.0, 21.0), (5.0, 19.0),
               (3.0, 22.0), (0.0, 18.0), (-2.6, 21.0), (-4.4, 14.0), (-6.4, 6.0), (-6.4, -1.0)]
    elif style == "short":
        pts = [(-5.4, -3.0), (-4.6, -6.4), (-1.0, -7.8), (3.0, -7.6), (5.6, -5.0), (5.8, -1.0), (3.6, -3.6),
               (-3.6, -3.6)]
    else:
        return
    ca, sa = cos(tilt), sin(tilt)
    s.poly([R(hc[0] + u * ca - v * sa, hc[1] + u * sa + v * ca) for u, v in pts])


def halo(c, inside, g0, lo=0.3):
    """the glare round a backlit silhouette: every pixel just outside it whose
    light is at least lo turns white, so its edge reads against dithered light"""
    fix = c.fix
    ring = set()
    for i, ins in enumerate(inside):
        if not ins:
            continue
        x, y = i % W, i // W
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                xx, yy = x + dx, y + dy
                if (dx or dy) and 0 <= xx < W and 0 <= yy < H:
                    q = yy * W + xx
                    if not inside[q] and fix[q] is None and g0[q] >= lo:
                        ring.add(q)
    for q in ring:
        fix[q] = 1


ALL_ROUND = ((0, -1, 1), (0, 1, 1), (-1, 0, 1), (1, 0, 1))


def player(c, R, pose=STANCE, hair="long", bass=False, rims=(), thresh=0.5, glare=0.3):
    """a guitarist or a bassist standing: black against the light, the edges
    towards the light white where they stand against dark, a glare round him
    where he stands in the light. Seen close, the guitar and the arms are
    drawn over the body with a thin line of light round them where they
    cross it, the way the backlight catches their edges"""
    p = pose
    k = R.k
    g0 = c.grey() if glare else None
    sep = k >= 0.45
    flipped = tuple(((-dx if R.flip else dx), dy, w) for dx, dy, w in rims)
    s = Shape()
    hc = p["head"]
    hair_shape(s, R, hc, hair, p["tilt"])
    s.poly([R(*q) for q in ell_pts(hc[0], hc[1], 4.9, 6.6, p["tilt"])])
    if p.get("face"):                                           # turned to the neck: a nose and a chin in profile
        f, ct, st = p["face"], cos(p["tilt"]), sin(p["tilt"])
        prof = [(3.6, -1.5), (6.4, 1.6), (5.0, 2.6), (5.2, 4.4), (4.4, 5.6), (1.0, 6.6)]
        s.poly([R(hc[0] + f * u * ct - v * st, hc[1] + f * u * st + v * ct) for u, v in prof])
    taper(s, R(hc[0] * 0.5, hc[1] + 4.5), R(0.0, 19.0), 5.2 * k, 5.6 * k)
    (sl, sr) = p["sh"]
    s.poly([R(*q) for q in ((sl[0] - 0.4, sl[1] - 2.6), (-4.0, sl[1] - 3.8), (4.0, sr[1] - 3.8),
                            (sr[0] + 0.4, sr[1] - 2.6), (sr[0] - 0.2, sr[1] + 4.5), (8.4, 37.0), (9.6, 45.0),
                            (5.5, 50.5), (0.0, 51.5), (-5.5, 50.5), (-9.6, 45.0), (-8.4, 37.0),
                            (sl[0] + 0.2, sl[1] + 4.5))])
    for sh in (sl, sr):                                         # the deltoids
        s.ellipse(*R(*sh), 3.4 * k, 3.4 * k)
    for hip, kn, an in zip(p["hip"], p["kn"], p["an"]):
        taper(s, R(*hip), R(*kn), 10.0 * k, 7.0 * k)
        taper(s, R(*kn), R(*an), 7.0 * k, 4.8 * k)
        out = 1.4 if an[0] > 0 else -1.4                       # a boot seen from the front, turned out a little
        s.poly([R(an[0] - 3.4, an[1] - 1.0), R(an[0] + 3.4, an[1] - 1.0), R(an[0] + 4.0 + out, an[1] + 5.0),
                R(an[0] - 4.0 + out, an[1] + 5.0)])
    centre, ang = p["guitar"]
    if not sep:
        guitar(s, R, centre, ang, bass)
        for sh, el, hd in zip(p["sh"], p["el"], p["hd"]):
            taper(s, R(*sh), R(*el), 6.2 * k, 5.0 * k)
            taper(s, R(*el), R(*hd), 5.0 * k, 3.8 * k)
            s.ellipse(*R(*hd), 2.7 * k, 3.0 * k)
        inside = s.paint(c, flipped, thresh=thresh)
    else:
        inside = s.paint(c, flipped, thresh=thresh)
        g = Shape()                                             # the guitar over the body
        guitar(g, R, centre, ang, bass)
        ins = g.paint(c, ALL_ROUND, thresh=thresh)
        inside = [a or b for a, b in zip(inside, ins)]
        for sh, el, hd in zip(p["sh"], p["el"], p["hd"]):     # the arms over the guitar
            a = Shape()
            taper(a, R(*sh), R(*el), 6.2 * k, 5.0 * k)
            taper(a, R(*el), R(*hd), 5.0 * k, 3.8 * k)
            a.ellipse(*R(*hd), 2.7 * k, 3.0 * k)
            ins = a.paint(c, ALL_ROUND, thresh=thresh)
            inside = [a_ or b for a_, b in zip(inside, ins)]
    if glare:
        halo(c, inside, g0, glare)


# ------------------------------------------------------------------ the drummer and his kit
DRUM_ARMS = {                           # shoulder, elbow, hand, the stick's tip (units)
    "up": (((-11.0, 21.0), (-21.0, 21.0), (-23.0, 7.0), (-31.0, -12.0)),
           ((11.0, 21.0), (21.0, 21.0), (23.0, 7.0), (31.0, -12.0))),
    "hit": (((-11.0, 21.0), (-19.0, 34.0), (-15.0, 41.0), (-8.0, 45.0)),
            ((11.0, 21.0), (21.0, 22.0), (23.0, 9.0), (30.0, -8.0))),
    "high": (((-11.0, 21.0), (-19.0, 12.0), (-18.0, -2.0), (-24.0, -20.0)),
             ((11.0, 21.0), (19.0, 12.0), (18.0, -2.0), (24.0, -20.0))),
}


def drum_arms(beat, jump):
    return "high" if jump else "hit" if beat else "up"


def drummer_body(s, R, arms):
    """the drummer from the waist up into s: head, short hair, shoulders, arms and sticks"""
    k = R.k
    hair_shape(s, R, (0.0, 6.8), "short", 0.0)
    s.poly([R(*q) for q in ell_pts(0.0, 6.8, 4.9, 6.6, 0.0)])
    taper(s, R(0.0, 11.0), R(0.0, 19.0), 5.2 * k, 5.6 * k)
    for side in (-1, 1):
        s.ellipse(*R(side * 11.0, 21.0), 3.4 * k, 3.4 * k)
    for sh, el, hd, tip in DRUM_ARMS[arms]:
        taper(s, R(*sh), R(*el), 6.2 * k, 5.0 * k)
        taper(s, R(*el), R(*hd), 5.0 * k, 3.8 * k)
        s.ellipse(*R(*hd), 2.6 * k, 2.8 * k)
        taper(s, R(*hd), R(*tip), max(1.3 * k, 0.9), max(1.0 * k, 0.8))      # the stick


def kit_shape(c, cx, floor, k, arms="hit", rims=((0, -1, 1),), bass_head=True, stand_px=False, near=1.0):
    """the drummer on his throne and his kit, one black shape: the floor of the
    riser at screen row floor, the kit's middle at cx, k pixels a unit; real
    sizes (a 22 inch bass drum is 31 units, an 18 inch crash 25); near > 1
    for a close camera: what stands in front of the drummer comes out bigger
    and further apart, the way a wide lens sees it close up.
    -> the drums' heads on the screen: (x, y, rx, ry)"""
    R = Rig(cx, floor - 78.0 * k, k)
    s = Shape()
    drummer_body(s, R, arms)
    s.poly([R(*q) for q in ((-11.4, 18.4), (-4.0, 17.2), (4.0, 17.2), (11.4, 18.4), (10.8, 25.0), (9.0, 37.0),
                            (10.0, 48.0), (-10.0, 48.0), (-9.0, 37.0), (-10.8, 25.0))])
    # the kit: y in units from the top of the drummer's head, the floor at 78
    stands, heads = [], []
    mid = (1.0 + near) / 2.0

    def drum(x, top, rx, depth, f, legs=False):
        x, rx, depth = x * f, rx * f, depth * f
        ry = rx * 0.28
        s.ellipse(*R(x, top), rx * k, ry * k)
        s.rect(*(R(x - rx, top) + R(x + rx, top + depth)))
        s.ellipse(*R(x, top + depth), rx * k, ry * k)
        heads.append(R(x, top) + (rx * k, ry * k))
        if legs:
            stands.append(((x - rx * 0.8, top + depth), (x - rx * 1.0, 78.0)))
            stands.append(((x + rx * 0.8, top + depth), (x + rx * 1.0, 78.0)))

    def cymbal(x, y, rx, tilt, f, foot=None):
        x, rx = x * f, rx * f
        s.poly([R(*q) for q in ell_pts(x, y, rx, max(rx * 0.1, 0.9 / k), tilt)])
        s.poly([R(*q) for q in ell_pts(x, y - 0.5, rx * 0.18, max(rx * 0.1, 0.9 / k), tilt)])
        if foot is not None:
            stands.append(((x, y), (foot * f, 78.0)))
    cymbal(-23.0, 2.0, 13.0, -0.16, mid, -20.0)                  # crash
    cymbal(31.0, 16.0, 15.0, 0.12, mid, 28.0)                    # ride
    cymbal(-35.0, 26.5, 10.0, 0.0, near, -35.0)                  # hi-hat
    cymbal(-35.0, 25.0, 10.0, 0.0, near)
    drum(-8.5, 34.0, 7.5, 10.0, mid)                             # rack toms
    drum(8.5, 34.0, 8.5, 11.0, mid)
    drum(-21.0, 42.0, 10.0, 7.0, near)                           # snare
    drum(23.0, 45.0, 11.5, 22.0, near, legs=True)                # floor tom
    stands.append(((-21.0 * near, 49.0), (-25.0 * near, 78.0)))
    stands.append(((-21.0 * near, 49.0), (-17.0 * near, 78.0)))
    s.ellipse(*R(0.0, 62.5), 15.5 * k * near, 15.5 * k * near)  # bass drum
    if stand_px:
        s.paint(c, rims)
        for a, b in stands:
            c.pixline(R(*a), R(*b), 0)
    else:
        for a, b in stands:
            taper(s, R(*a), R(*b), max(1.1 * k, 1.1), max(1.1 * k, 1.1))
        s.paint(c, rims)
    if bass_head:                                                # the bass drum's front head, lit, its logo ring
        m = D()
        m.ellipse(*R(0.0, 62.5), 13.0 * k * near, 13.0 * k * near)
        c.white(m.m, 0.5)
        ring = D()
        ring.ellipse(*R(0.0, 62.5), 7.0 * k * near, 7.0 * k * near)
        ring.ellipse(*R(0.0, 62.5), 4.0 * k * near, 4.0 * k * near, 0)
        c.black(ring.m, (), thresh=0.45)
    return heads


# ------------------------------------------------------------------ the crowd
def person(s, x, y, r, rng, arm=False, phone=False, horns=False, up=3.0, side=1):
    """a member of the crowd into a Shape: head at (x, y) radius r, the
    shoulders under it; maybe an arm up (up head-radii over the head) with a
    fist, the horns or a phone. -> where the phone's screen is, or None"""
    s.ellipse(x, y, r * 0.86, r)
    s.ellipse(x, y + r * 1.8, r * 2.1, r * 0.95)
    s.rect(x - r * 2.1, y + r * 1.8, x + r * 2.1, y + r * 7)
    if not arm:
        return None
    sx, sy = x + side * r * 1.5, y + r * 1.4
    fx = sx + side * r * (0.4 + rng.uniform(-0.5, 0.5))
    fy = y - r * up
    s.line((sx, sy), (fx, fy), max(0.9, r * 0.7))
    if phone:
        s.rect(fx - r * 0.45, fy - r * 1.0, fx + r * 0.45, fy + r * 0.3)
        return (fx, fy - r * 0.35, r * 0.3, r * 0.55)
    s.ellipse(fx, fy, r * 0.62, r * 0.66)
    if horns:
        for hdx in (-0.4, 0.42):
            s.line((fx + hdx * r, fy - r * 0.3), (fx + hdx * r * 1.45, fy - r * 1.6), max(0.7, r * 0.3))
    return None


def fan(c, people, lift=1.0):
    """a crowd placed by hand, back to front: (x, y of the head, radius, arm),
    arm None or (side, height in head radii, kind: fist/horns/phone); lift
    scales how high the arms are (the fists pump)"""
    rng = random.Random(1)
    screens = []
    for x, y, r, arm in people:
        s = Shape()
        if arm is None:
            person(s, x, y, r, rng)
        else:
            side, h, kind = arm
            sc = person(s, x, y, r, rng, arm=True, phone=(kind == "phone"), horns=(kind == "horns"),
                        up=1.2 + (h - 1.2) * lift, side=side)
            if sc:
                screens.append(sc)
        s.paint(c)
    for fx, fy, rx, ry in screens:
        m = D()
        m.ellipse(fx, fy, rx + 0.2, ry + 0.2)
        c.white(m.m, 0.3)


def wide_crowd():
    """the front of the crowd in the two corners, heads bigger than the
    players' (they are nearer), arms up against the fog"""
    left = [(9.0, 47.0, 2.4, None), (14.0, 46.4, 2.6, (1, 3.3, "fist")), (19.4, 47.4, 2.5, None),
            (24.4, 46.6, 2.7, (-1, 3.6, "horns")), (29.6, 47.6, 2.5, None), (34.6, 46.8, 2.6, (1, 2.9, "phone")),
            (39.6, 47.8, 2.5, None)]
    right = [(88.4, 47.8, 2.5, None), (93.4, 46.8, 2.6, (1, 3.4, "horns")), (98.4, 47.6, 2.5, None),
             (103.6, 46.6, 2.7, (-1, 2.9, "phone")), (108.6, 47.4, 2.5, None), (113.8, 46.4, 2.6, (-1, 3.3, "fist")),
             (119.0, 47.0, 2.4, None)]
    near = [(15.0, 58.0, 4.4, (1, 3.6, "horns")), (31.0, 60.0, 4.6, None),
            (97.0, 60.0, 4.6, None), (113.0, 58.0, 4.4, (-1, 3.6, "horns"))]
    return left + right + near


def close_crowd(side):
    """the front row along the barrier in one bottom corner, against the fog"""
    people = [(84.0, 51.0, 3.4, None), (91.0, 50.0, 3.6, (1, 3.0, "horns")), (98.0, 51.2, 3.4, None),
              (105.0, 50.2, 3.6, (-1, 2.8, "phone")), (112.0, 51.0, 3.4, None), (119.0, 50.0, 3.6, (-1, 3.1, "fist")),
              (125.0, 51.4, 3.4, None), (102.0, 61.0, 5.0, None), (118.0, 60.0, 4.8, (-1, 3.2, "horns"))]
    if side < 0:
        people = [(128.0 - x, y, r, None if a is None else (-a[0], a[1], a[2])) for x, y, r, a in people]
    return people


def far_crowd():
    """the crowd out there, seen from the stage: two rows of small heads, arms up"""
    rng = random.Random(4)
    people = []
    for y, r, dx, arms in ((28.2, 1.2, 2.6, 0.25), (29.6, 1.4, 3.0, 0.3)):
        x = rng.uniform(0, dx)
        while x < 128:
            arm = None
            if rng.random() < arms:
                arm = (rng.choice((-1, 1)), rng.uniform(2.8, 4.0), rng.choice(("fist", "horns", "horns", "phone")))
            people.append((x, y + rng.uniform(-0.3, 0.3), r * rng.uniform(0.9, 1.1), arm))
            x += dx * rng.uniform(0.8, 1.2)
    return people


# ------------------------------------------------------------------ the cameras
def moves(band):
    """a band pose -> (beat, strum, jump)"""
    return (1 if band in (1, 3, 4) else 0), band in (2, 3), band == 4


def guitarist_pose(beat, strum, jump):
    if jump:
        return JUMP, JUMP_LIFT
    p = STANCE_B if beat else STANCE
    return (strummed(p) if strum else p), 0.0


def bassist_pose(beat, strum, jump, close=False):
    """-> (pose, hair): headbanging close, nodding on the beat from further off"""
    if close:
        p, hair = (HEADBANG_DEEP, "bang") if jump else (HEADBANG_UP, "mane") if beat else (HEADBANG, "bang")
    else:
        p, hair = (HEADBANG, "bang") if jump else (NOD, "mane") if beat else (STANCE, "mane")
    return (strummed(p) if strum else p), hair


def wide_set(light, band, crowd, v):
    """the stage from the crowd: players 33 px tall (feet on the stage at row
    37), the kit on its riser further back, the stacks behind the players,
    mic stands, the crowd in the corners; v: the mock-up's V of beams out of
    the middle (the wide shot with star power on)"""
    c = Canvas()
    beat, strum, jump = moves(band)
    pulse = 1.0 + 0.08 * beat
    sw, sg = SWAY[light], SWING[light]

    def air(px, py):
        t = clamp((py - 2.0) / 34.0)
        side = clamp(abs(px - 64.0) / 64.0)
        return (0.03 - 0.03 * t) * (1.0 - 0.6 * side * side)
    c.add(field(air, ss=1), 1.0)
    for x, d in ((34, 1), (94, -1)):                             # a spot behind each player: the beam from
        spot(c, x, -4, 37, 20, power=0.6 * pulse, lean=1.5 * sw * d)     # the truss, the smoke round his shoulders
        glow(c, x, 19, 6.5, 1.45 * pulse, 2.6, sx=1.0, sy=1.9)
    glow(c, 64, 10, 6.5, 1.4 * pulse, 2.6, sx=1.25)              # the light behind the drummer
    if v:
        for x, a in ((56, -0.30 - 0.05 * sw), (72, 0.30 + 0.05 * sw)):
            beam(c, x, 1, a, spread=0.045, power=1.4, length=45.0, w0=0.9, floor=37.0, profile=0.8)
        for x, a in ((18, -0.36 + 0.05 * sg), (110, 0.36 - 0.05 * sg)):
            beam(c, x, 1, a, spread=0.05, power=1.2, length=45.0, w0=0.9, floor=37.0, profile=0.8)
    else:
        beam(c, 6, 1, 0.95 + 0.05 * sg, spread=0.04, power=0.8, length=50.0, w0=0.9, floor=37.0)
        beam(c, 122, 1, -0.95 - 0.05 * sg, spread=0.04, power=0.8, length=50.0, w0=0.9, floor=37.0)
    fog_band(c, 43.5, 5.0, 1.05)
    for x in (34, 94):
        pool(c, x, 36.5, 8.0, 1.0, 0.3)
    riser(c, 46, 82, 23, 27)
    kit_shape(c, 64, 23.0, 0.27, arms=drum_arms(beat, jump), stand_px=True)
    amp_wall(c, 9.0, 9.0, 37.0, 11.0)
    amp_wall(c, 108.0, 9.0, 37.0, 11.0)
    mic_stand(c, 47.0, 37, 25, -3.0)
    mic_stand(c, 81.0, 37, 25, 3.0)
    pose, lift = guitarist_pose(beat, strum, jump)
    player(c, Rig(34.0, 4.0 - lift * 0.33, 0.33), pose, "long", rims=((0, -1, 1),), thresh=0.45)
    pose, hair = bassist_pose(beat, False, jump)
    player(c, Rig(94.0, 4.0, 0.33, flip=True), pose, hair, bass=True, rims=((0, -1, 1),), thresh=0.45)
    for x in (34, 94):
        lamp(c, x, 1.0, 0.8)
    m = D()                                                     # the lip of the stage
    m.rect(0, 37, 128, 38)
    c.white(m.m, 0.5)
    fan(c, wide_crowd(), 1.0 if crowd else 0.45)
    return c


def cam_wide(light, band, crowd):
    return wide_set(light, band, crowd, False)


def cam_wide_v(light, band, crowd):
    return wide_set(light, band, crowd, True)


def cam_guitar(light, band, crowd):
    """the guitarist from the front row, 62 px tall, in the spot behind him;
    the kit far back on its riser, the front row in the corner"""
    c = Canvas()
    beat, strum, jump = moves(band)
    pulse = 1.0 + 0.08 * beat
    haze(c, 0.05, 0.0, 0, 30)
    spot(c, 30, -8, 62, 40, power=0.9 * pulse, w_top=3.0, profile=0.9, lean=-2.0 + 3.0 * SWAY[light])
    glow(c, 30, 28, 11.0, 1.6 * pulse, 2.6, sx=1.1, sy=1.8)    # the smoke behind him, lit from behind
    beam(c, 122, -4, -0.30 + 0.05 * SWING[light], spread=0.05, power=1.1, length=60.0, w0=1.0, floor=46.0)
    glow(c, 101, 13, 6.5, 1.25, 2.6, sx=1.5)                   # the kit's own light
    riser(c, 80, 128, 22, 26)
    kit_shape(c, 101, 22.0, 0.2, arms=drum_arms(beat, jump), stand_px=True)
    fog_band(c, 47.0, 5.0, 1.15, x0=80)
    glow(c, 30, 56, 9.0, 1.15, 2.4, sx=2.2, sy=0.8)             # the smoke on the floor, lit from behind
    pose, lift = guitarist_pose(beat, strum, jump)
    player(c, Rig(30.0, 2.0 - lift * 0.62, 0.62), pose, "long", rims=((0, -1, 1),))
    fan(c, close_crowd(1), 1.0 if crowd else 0.45)
    return c


def cam_bass(light, band, crowd):
    """the bassist headbanging, the mirror of the guitarist's picture"""
    c = Canvas()
    beat, strum, jump = moves(band)
    pulse = 1.0 + 0.08 * beat
    haze(c, 0.05, 0.0, 0, 30)
    spot(c, 98, -8, 62, 40, power=0.9 * pulse, w_top=3.0, profile=0.9, lean=2.0 - 3.0 * SWAY[light])
    glow(c, 98, 28, 11.0, 1.6 * pulse, 2.6, sx=1.1, sy=1.8)
    beam(c, 6, -4, 0.30 - 0.05 * SWING[light], spread=0.05, power=1.1, length=60.0, w0=1.0, floor=46.0)
    glow(c, 27, 13, 6.5, 1.25, 2.6, sx=1.5)
    riser(c, 0, 48, 22, 26)
    kit_shape(c, 27, 22.0, 0.2, arms=drum_arms(beat, jump), stand_px=True)
    fog_band(c, 47.0, 5.0, 1.15, x1=48)
    glow(c, 98, 56, 9.0, 1.15, 2.4, sx=2.2, sy=0.8)
    pose, hair = bassist_pose(beat, strum, jump, close=True)
    player(c, Rig(98.0, 2.0, 0.62, flip=True), pose, hair, bass=True, rims=((0, -1, 1),))
    fan(c, close_crowd(-1), 1.0 if crowd else 0.45)
    return c


def cam_drums(light, band, crowd):
    """the kit from the front, close, with a wide lens: the drummer and his kit
    black against the light behind them, falling off to the sides; what is
    nearest (the snare, the hi-hat, the floor tom) comes out big at the
    sides; the hoops catch the light"""
    c = Canvas()
    beat, strum, jump = moves(band)
    pulse = 1.0 + 0.08 * beat

    def wall(px, py):
        d = math.hypot((px - 64.0) / 60.0, (py - 12.0) / 46.0)
        return (1.1 * (1.0 - smooth(0.12, 1.0, d)) + 0.22) * pulse
    c.add(field(wall, ss=1), 1.0)
    starburst(c, 64, 8, 2.4, power=1.4, rays=8, ray_len=24.0, turn=0.2 + 0.2 * light, halo_k=2.2)
    heads = kit_shape(c, 64, 6.0 + 78 * 0.6, 0.6, arms=drum_arms(beat, jump), bass_head=False, near=1.9)
    for x, y, rx, ry in heads:                                  # the near edge of every head, lit
        near_pts = sorted(p for p in ell_pts(x, y, rx - 0.5, ry, 0.0, 56) if p[1] > y + ry * 0.2)
        for a, b in zip(near_pts, near_pts[1:]):
            c.pixline(a, b, 1)
    return c


def cam_stage(light, band, crowd):
    """from the stage behind the drummer: his back and his sticks, the
    guitarist and the bassist at the edge of the stage (seen from behind the
    guitarist is on our right), the crowd out there in the haze the stage
    lights up"""
    c = Canvas()
    beat, strum, jump = moves(band)
    pulse = 1.0 + 0.08 * beat

    def arena(px, py):
        d = (py - 17.0) / 9.5
        return 1.3 * pulse * math.exp(-d * d * d * d)
    c.add(field(arena), 1.0)
    for x, y in ((40, 4), (88, 4)):
        starburst(c, x, y, 1.2, power=1.5, rays=4, ray_len=5.0, turn=0.35 * light, halo_k=2.0)
    fan(c, far_crowd(), 1.0 if crowd else 0.5)
    s = Shape()
    s.rect(0, 31, 128, 64)
    s.paint(c)
    pose, lift = guitarist_pose(beat, strum, jump)
    player(c, Rig(96.0, 7.5 - lift * 0.25, 0.25, flip=True), pose, "long")
    pose, hair = bassist_pose(beat, False, jump)
    player(c, Rig(32.0, 8.0, 0.25), pose, hair, bass=True)
    R = Rig(64.0, 12.0, 0.62)                                   # the drummer's back, close
    s = Shape()
    drummer_body(s, R, drum_arms(beat, jump))
    s.poly([R(*q) for q in ((-11.4, 18.4), (-4.0, 17.2), (4.0, 17.2), (11.4, 18.4), (11.0, 40.0), (-11.0, 40.0))])
    g0 = c.grey()
    halo(c, s.paint(c), g0, 0.3)
    return c


def cam_pit(light, band, crowd):
    """from the pit, looking up: the guitarist towers over the edge of the
    stage; further back the bassist in the light of his own lamp, the amp
    stack behind him, both cut off low down by the edge of the stage (we are
    below it); the truss and its lamps overhead, their beams coming down"""
    c = Canvas()
    beat, strum, jump = moves(band)
    pulse = 1.0 + 0.08 * beat
    edge = 44.0
    haze(c, 0.05, 0.0, 0, edge)
    lamps = ((32, -0.10, 1.05), (60, 0.03, 0.7), (86, 0.08, 0.7), (110, 0.16, 0.8))
    for i, (x, a, p) in enumerate(lamps):                       # soft cones down onto the stage
        a += 0.05 * SWAY[light] * (1 if i % 2 else -1)
        beam(c, x, 7.5, a, spread=0.17, power=p, length=60.0, w0=1.2, floor=edge, profile=1.3)
    glow(c, 33, 27, 6.5, 1.6 * pulse, 2.6, sx=1.0, sy=1.7)      # the smoke round the bassist, lit
    glow(c, 90, 24, 9.5, 1.6 * pulse, 2.6, sx=1.3, sy=1.8)      # and round the guitarist
    for y in (2.0, 5.0):                                        # the truss, lit from below
        c.pixline((20, y), (127, y), 1)
    x, k = 20, 0
    while x < 127:
        c.pixline((x, 2.0 if k % 2 == 0 else 5.0), (x + 3, 5.0 if k % 2 == 0 else 2.0), 1)
        x += 3
        k += 1
    for x, a, p in lamps:                                       # the lamps hanging under it
        s = Shape()
        s.rect(x - 1.5, 5.0, x + 1.5, 7.5)
        s.paint(c)
        starburst(c, x, 7.6, 1.0, power=1.6, rays=4, ray_len=4.0, turn=0.0, halo_k=1.4)
    amp_wall(c, 9.0, 13.0, edge + 6.0, 12.0)                    # the amp stack at the back of the stage
    pose, hair = bassist_pose(beat, False, jump)
    player(c, Rig(33.0, 17.0, 0.3, flip=True), pose, hair, bass=True, rims=((0, -1, 1),), thresh=0.45)
    s = Shape()                                                 # the front of the stage, its edge lit
    s.rect(0, edge, 128, 64)
    s.paint(c)
    c.pixline((0, edge), (127, edge), 1)
    pose, lift = guitarist_pose(beat, strum, jump)
    player(c, Rig(90.0, 1.0 - lift * 0.44, 0.44, low=0.3), pose, "long", rims=((0, -1, 1),))
    return c


# (name, how it is drawn, crowd poses): the order is the game's camera numbers
CAMERAS = [("WIDE", cam_wide, 2), ("WIDE_V", cam_wide_v, 2), ("GUITAR", cam_guitar, 2), ("BASS", cam_bass, 2),
           ("DRUMS", cam_drums, 1), ("STAGE", cam_stage, 2), ("PIT", cam_pit, 1)]
# the order the cameras cut in, every few bars (with star power on the wide
# shot is the one with the V of beams)
CUTS = ["WIDE", "GUITAR", "WIDE", "BASS", "DRUMS", "WIDE", "STAGE", "PIT"]


# ------------------------------------------------------------------ frames
def dither(c, matrix):
    """-> rows of 0/1: the fixed pixels as they are, the rest blue noise"""
    g = c.grey()
    out = []
    for y in range(H):
        row = []
        for x in range(W):
            i = y * W + x
            f = c.fix[i]
            if f is not None:
                row.append(f)
            else:
                v = clamp((g[i] - 0.04) / 0.92)
                row.append(1 if v > bluenoise.threshold(matrix, x, y) else 0)
        out.append(row)
    return out


def frame_bytes(bits):
    """-> the frame as the screen buffer has it: a page of eight rows at a
    time, a byte a column, the top row in bit 0"""
    out = bytearray()
    for page in range(H // 8):
        for x in range(W):
            v = 0
            for i in range(8):
                if bits[page * 8 + i][x]:
                    v |= 1 << i
            out.append(v)
    return bytes(out)


def frame_bits(data):
    return [[(data[(y >> 3) * W + x] >> (y & 7)) & 1 for x in range(W)] for y in range(H)]


def render_camera(fn, crowds, matrix):
    """-> the camera's frames, in the game's order"""
    out = []
    for light in range(LIGHTS):
        for band in range(BAND_STATES):
            for crowd in range(crowds):
                out.append(frame_bytes(dither(fn(light, band, crowd), matrix)))
    return out


def cached_frames(force=False):
    """-> {camera: [frame bytes]}, rendered when this file or the blue noise changed"""
    src = open(__file__, "rb").read() + open(os.path.join(HERE, "bluenoise.py"), "rb").read()
    key = hashlib.sha1(src).hexdigest()
    path = os.path.join(ROOT, "build", "scene_cache.json")
    if not force:
        try:
            with open(path) as f:
                c = json.load(f)
            if c.get("key") == key:
                return {k: [bytes.fromhex(h) for h in v] for k, v in c["frames"].items()}
        except (OSError, ValueError, KeyError):
            pass
    matrix = bluenoise.matrix()
    frames = {name: render_camera(fn, crowds, matrix) for name, fn, crowds in CAMERAS}
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump({"key": key, "frames": {k: [b.hex() for b in v] for k, v in frames.items()}}, f)
    return frames


def scenes():
    """-> [(name, crowds, bytes)]: every camera's frames, one after the other"""
    frames = cached_frames()
    return [(name, crowds, b"".join(frames[name])) for name, _, crowds in CAMERAS]


def preview(frames, scale=2):
    """build/scene_<camera>.png: every frame, a row for each light, at scale"""
    gap = 4
    for name, _, crowds in CAMERAS:
        per = BAND_STATES * crowds
        im = Image.new("L", ((W + gap) * per * scale, (H + gap) * LIGHTS * scale), 60)
        for k, data in enumerate(frames[name]):
            p = Image.new("L", (W, H))
            p.putdata([255 * b for row in frame_bits(data) for b in row])
            im.paste(p.resize((W * scale, H * scale), Image.NEAREST),
                     ((k % per) * (W + gap) * scale, (k // per) * (H + gap) * scale))
        path = os.path.join(ROOT, "build", "scene_%s.png" % name.lower())
        im.save(path)
        print("wrote", os.path.relpath(path, ROOT))


if __name__ == "__main__":
    import time
    t = time.time()
    frames = cached_frames("-f" in sys.argv)
    total = 0
    for name, crowds, data in scenes():
        print("%-7s %3d frames, %6d bytes" % (name, len(data) // 1024, len(data)))
        total += len(data)
    print("%d bytes in all, %.1f s" % (total, time.time() - t))
    preview(frames)
