#!/usr/bin/env python3
"""The crowd and the spotlights of the title page, the main menu and the
booed-off page, in the stage's look (Joel picked the samples of
build/look/v6.html on 2026-10-01, with PRESS B back under the logo and the
menu's crowd lower).

The crowd is the play page's wide shot's (tools/scene.py), across the whole
width: people as black silhouettes cut out of the glow at the front of the
stage, hands up (horns, fists, both arms, phones with their screens lit),
each jumping in their own time, two beats to a jump. On the booed-off page
the same people boo: the hands that were up shake a fist, a few turn a
thumb down, the phones keep filming, two throw cups at the stage by turns
(the game draws the cups, up into the dark: CUPS says where they leave the
hand). The glow falls off quickly below the heads, so no light shows between
their shoulders. The spotlights are soft light in the haze, as the stage's
are, swinging the way the title page always swung them.

Each page is two layers on the chip, a frame of the screen buffer's pages
each, so the spotlights and the crowd move on their own; the crowd's layer
goes over the light's from row CROWD_TOP down (the highest row the people
reach), and above that row the light stays, so the spotlights come down
into the glow behind the people and the credits roll up behind them:

  TITLE_LIGHT   rows 0..55 (7 pages): the spotlights at the 32 places of
                their swing, coming down into the top of the glow
  TITLE_CROWD   rows 48..63 (2 pages, nothing above CROWD_TOP): the glow
                and the crowd jumping, 16 steps; the title page, the main
                menu and the credits share it, so the crowd stays put
                between them
  BOO_LIGHT     rows 0..55 (7 pages): the spotlights as the booed-off page
                opens (both on, then the right one alone, at the places of
                its first 44 frames), and a last frame with them out
  BOO_CROWD     rows 48..63 (2 pages, nothing above CROWD_TOP): the glow
                and the crowd booing, 32 frames, the crowd as low as on the
                title page (Joel, 2026-10-02: "the crowd sits at the same
                height as the crowd on the title and credit screens")

The crowd stands three rows lower than it first did (Joel, 2026-10-02: "on
all screens that have the crowd I think we can move it down a few pixels"):
the glow behind it is bright from row 53, the heads are cut by the bottom
edge. PRESS B sits halfway between the line above it and that row.

  python3 tools/crowd.py       renders (or takes the frames from
                               build/crowd_cache.json) and writes
                               build/crowd.png to look at them
"""
import hashlib
import json
import math
import os
import random
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import scene as S  # noqa: E402
from scene import Canvas, Shape, D, field, beam, person  # noqa: E402

W, H = 128, 64
SIN32 = [0, 25, 49, 71, 90, 106, 117, 125, 127, 125, 117, 106, 90, 71, 49, 25,
         0, -25, -49, -71, -90, -106, -117, -125, -127, -125, -117, -106, -90, -71, -49, -25]
JUMP = [0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 2, 1, 0, 0, 0, 0]    # rows up, in 16 steps of two beats
SWING = 32                       # places of the spotlights' swing
STEPS = 16                       # steps of the crowd's jumping
BOO_FRAMES = 32                  # frames of the booing
TITLE_SPLIT, TITLE_DY = 48, 15.5     # the crowd's layer from this row; how far down the crowd stands
LIGHT_ROWS = 56                  # the light's layer: rows 0..55, under the crowd's from CROWD_TOP
BOO_DY = TITLE_DY                # the booed-off page's crowd: the same
BOO_LEFT_OFF, BOO_RIGHT_OFF = 30, 44  # frames the spotlights stay on as the booed-off page opens


# ---------------------------------------------------------------- the light
def stage_light(c, centre, up=4.6, down=2.2, gain=1.15):
    """the front of the stage, lit, behind the crowd: a glow round row centre,
    soft upward, gone quickly downward (below the heads)"""
    def f(px, py):
        d = (py - centre) / (up if py < centre else down)
        return gain * math.exp(-d * d)
    c.add(field(f, (0, centre - up * 3, W, centre + down * 3)), 1.0)


def spotlights(c, k, left=True, right=True, floor=44.0):
    """the two spotlights at place k of their swing: from above the top
    corners, as the title page swung them (the right one 11 places on)"""
    for on, sx, d, ph in ((left, -8.0, 1, 0), (right, 135.0, -1, 11)):
        if on:
            s = SIN32[(k + ph) & 31]
            ang = d * math.atan((22 + s * 16 / 127.0) / 16.0)
            beam(c, sx, -6.0, ang, spread=0.11, power=0.95, length=55.0, w0=1.2, floor=floor, profile=1.2)


# ---------------------------------------------------------------- the people
# (x, head y, radius, arm): arm (side, height in head radii, kind); kind 'v'
# is both arms up. The wide shot's crowd, across the width; the last six are
# the near ones, big, cut by the bottom edge
PEOPLE = [
    (3.0, 47.4, 2.4, None), (8.0, 46.6, 2.6, (1, 2.6, "fist")), (13.2, 47.6, 2.5, None),
    (18.0, 46.8, 2.7, (-1, 2.6, "horns")), (23.0, 47.8, 2.5, None), (27.6, 46.6, 2.6, (1, 2.5, "phone")),
    (32.6, 47.6, 2.5, None), (37.4, 46.8, 2.6, (1, 2.6, "v")), (42.6, 47.8, 2.5, None),
    (47.6, 47.4, 2.6, None), (52.6, 47.8, 2.5, (-1, 2.5, "phone")), (57.4, 46.8, 2.6, None),
    (62.4, 47.6, 2.5, (1, 2.6, "horns")), (67.2, 47.0, 2.6, None), (72.0, 47.8, 2.5, (-1, 2.4, "fist")),
    (76.8, 46.8, 2.7, None), (81.6, 47.6, 2.5, (1, 2.5, "phone")), (86.4, 46.8, 2.6, None),
    (91.4, 47.8, 2.5, (-1, 2.6, "horns")), (96.2, 47.0, 2.6, None), (101.0, 47.6, 2.5, (1, 2.6, "v")),
    (105.8, 46.8, 2.6, None), (110.6, 47.8, 2.5, (-1, 2.5, "fist")), (115.4, 46.6, 2.7, None),
    (120.2, 47.6, 2.5, (-1, 2.6, "horns")), (125.0, 47.0, 2.6, None),
    (11.0, 58.5, 4.4, (1, 2.2, "horns")), (27.0, 60.5, 4.6, None), (45.0, 61.0, 4.4, None),
    (83.0, 61.0, 4.4, None), (101.0, 60.5, 4.6, None), (117.0, 58.5, 4.4, (-1, 2.2, "horns"))]
JUMP_ROWS = 0.6                  # how much of JUMP a jump lifts them
BOO_DOWN = {3, 14, 18}           # booing: a thumb down
BOO_THROW = {7: (0, 1), 20: (16, -1)}   # booing: who throws, (the frame of the throw, towards)
CUP_FLIGHT = 16


def people(dy):
    return [(x, y + dy, r, a, (k * 5) % STEPS) for k, (x, y, r, a) in enumerate(PEOPLE)]


def phone_screens(c, screens):
    for fx, fy, rx, ry in screens:
        m = D()
        m.ellipse(fx, fy, rx + 0.2, ry + 0.2)
        c.white(m.m, 0.3)


def cheering(c, crowd, step):
    rng = random.Random(1)
    screens = []
    for x, y, r, arm, ph in crowd:
        s = Shape()
        yy = y - JUMP[(step + ph) % STEPS] * JUMP_ROWS
        if arm is None:
            person(s, x, yy, r, rng)
        else:
            side, h, kind = arm
            if kind == "v":
                person(s, x, yy, r, rng, arm=True, up=h, side=1)
                person(s, x, yy, r, rng, arm=True, up=h * 0.95, side=-1)
            else:
                sc = person(s, x, yy, r, rng, arm=True, phone=(kind == "phone"), horns=(kind == "horns"),
                            up=h, side=side)
                if sc:
                    screens.append(sc)
        s.paint(c)
    phone_screens(c, screens)


def throw_hand(x, y, r, side, t):
    """a thrower's hand, t frames into the booing: back behind his head for
    8 frames, then up and forward (the cup gone)"""
    sx = x + side * r * 1.5
    if t < 8:
        k8 = t / 8.0
        return sx - side * r * (0.6 + 0.8 * k8), y - r * (1.5 + 0.5 * k8)
    return sx + side * r * 0.9, y - r * 2.4


def booing(c, crowd, f):
    rng = random.Random(1)
    screens = []
    for k, (x, y, r, arm, ph) in enumerate(crowd):
        s = Shape()
        if arm is None:
            person(s, x, y, r, rng)
            s.paint(c)
            continue
        side, h, kind = arm
        if kind == "phone":                                     # still filming
            screens.append(person(s, x, y, r, rng, arm=True, phone=True, up=h, side=side))
            s.paint(c)
            continue
        person(s, x, y, r, rng)
        if k in BOO_THROW:
            t0, side = BOO_THROW[k]
            fx, fy = throw_hand(x, y, r, side, (f - t0) % BOO_FRAMES)
        else:                                                   # a fist, shaking; or a thumb down
            shake = ((f >> 3) + k) & 1
            fx, fy = x + side * r * 1.9, y - r * (2.5 - 0.45 * shake)
            if k in BOO_DOWN:
                tx = fx - side * r * 0.25
                s.line((tx, fy + r * 0.3), (tx, fy + r * 1.35), max(0.7, r * 0.34))
        s.line((x + side * r * 1.5, y + r * 1.4), (fx, fy), max(0.9, r * 0.7))
        s.ellipse(fx, fy, r * 0.62, r * 0.66)
        s.paint(c)
    phone_screens(c, screens)


def cups():
    """-> [(hand x, hand y, towards, frame of the throw)]: where the game lets
    each cup go"""
    crowd = people(BOO_DY)
    out = []
    for k, (t0, side) in sorted(BOO_THROW.items(), key=lambda e: e[1][0]):
        x, y, r, arm, ph = crowd[k]
        hx, hy = throw_hand(x, y, r, side, 8)
        out.append((int(round(hx)), int(round(hy - r * 0.6)), side, t0))
    return out


# ---------------------------------------------------------------- the layers
def title_light(k):
    c = Canvas()
    stage_light(c, 44.0 + TITLE_DY)
    spotlights(c, k, floor=41.5 + TITLE_DY)
    return c


def title_crowd(step):
    c = Canvas()
    stage_light(c, 44.0 + TITLE_DY)
    cheering(c, people(TITLE_DY), step)
    return c


def boo_light(k, left, right):
    c = Canvas()
    stage_light(c, 44.0 + BOO_DY)
    spotlights(c, k, left, right, floor=41.5 + BOO_DY)
    return c


def boo_crowd(f):
    c = Canvas()
    stage_light(c, 44.0 + BOO_DY)
    booing(c, people(BOO_DY), f)
    return c


def boo_light_frames():
    """the booed-off page's light, frame by frame as it opens: (place of the
    swing, left on, right on) for each distinct picture, and which picture
    each of the first BOO_RIGHT_OFF frames shows; then one with both out"""
    pics, order = [], []
    for f in range(BOO_RIGHT_OFF):
        key = (f >> 3, f < BOO_LEFT_OFF, True)
        if key not in pics:
            pics.append(key)
        order.append(pics.index(key))
    pics.append((0, False, False))
    return pics, order


def pages(bits, first, last):
    """rows of 0/1 -> the screen buffer's pages first..last-1 (bit 0 on top)"""
    out = bytearray()
    for page in range(first, last):
        for x in range(W):
            v = 0
            for i in range(8):
                if bits[page * 8 + i][x]:
                    v |= 1 << i
            out.append(v)
    return bytes(out)


def crowd_top(frames, dy):
    """the highest row the people reach in any of the frames (rows of 0/1):
    the first row where one of them is not the glow alone"""
    c = Canvas()
    stage_light(c, 44.0 + dy)
    glow = S.dither(c, S.bluenoise.matrix())
    return min(next(y for y in range(H) if f[y] != glow[y]) for f in frames)


def render():
    m = S.bluenoise.matrix()
    lp, cp = LIGHT_ROWS // 8, TITLE_SPLIT // 8
    pics, order = boo_light_frames()
    tc = [S.dither(title_crowd(s), m) for s in range(STEPS)]
    bc = [S.dither(boo_crowd(f), m) for f in range(BOO_FRAMES)]
    top = min(crowd_top(tc, TITLE_DY), crowd_top(bc, BOO_DY))
    assert TITLE_SPLIT <= top < LIGHT_ROWS, top

    def crowd(f):                  # nothing above the people: the light's rows (the same glow) stay
        return pages([[0] * W if y < top else row for y, row in enumerate(f)], cp, 8)
    return {
        "title_light": [pages(S.dither(title_light(k), m), 0, lp) for k in range(SWING)],
        "title_crowd": [crowd(f) for f in tc],
        "boo_light": [pages(S.dither(boo_light(k, l, r), m), 0, lp) for k, l, r in pics],
        "boo_crowd": [crowd(f) for f in bc],
        "boo_order": order,
        "crowd_top": top,
    }


PLAIN = ("boo_order", "crowd_top")    # in the cache as they are, not as frames


def cached():
    """the frames, rendered when this file, scene.py or the blue noise changed"""
    src = b"".join(open(os.path.join(HERE, n), "rb").read() for n in ("crowd.py", "scene.py", "bluenoise.py"))
    key = hashlib.sha1(src).hexdigest()
    path = os.path.join(ROOT, "build", "crowd_cache.json")
    try:
        with open(path) as f:
            c = json.load(f)
        if c.get("key") == key:
            d = c["data"]
            return {k: (v if k in PLAIN else [bytes.fromhex(h) for h in v]) for k, v in d.items()}
    except (OSError, ValueError, KeyError):
        pass
    data = render()
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump({"key": key, "data": {k: (v if k in PLAIN else [b.hex() for b in v])
                                        for k, v in data.items()}}, f)
    return data


def preview(data, scale=2):
    """build/crowd.png: the title's light and crowd put together at a few
    places, and the booed-off page's"""
    from PIL import Image
    cp, top = TITLE_SPLIT // 8, data["crowd_top"]

    def picture(light, crowd):
        def on(y, x):
            if y < top:
                return (light[(y >> 3) * W + x] >> (y & 7)) & 1
            return (crowd[((y >> 3) - cp) * W + x] >> (y & 7)) & 1
        im = Image.new("L", (W, H))
        im.putdata([255 if on(y, x) else 0 for y in range(H) for x in range(W)])
        return im.resize((W * scale, H * scale), Image.NEAREST)
    pics = [picture(data["title_light"][k * 4], data["title_crowd"][k * 2]) for k in range(8)]
    pics += [picture(data["boo_light"][i], data["boo_crowd"][i * 4]) for i in range(len(data["boo_light"]))]
    cols = 4
    rows = (len(pics) + cols - 1) // cols
    out = Image.new("L", ((W * scale + 6) * cols, (H * scale + 6) * rows), 60)
    for i, p in enumerate(pics):
        out.paste(p, ((i % cols) * (W * scale + 6), (i // cols) * (H * scale + 6)))
    path = os.path.join(ROOT, "build", "crowd.png")
    out.save(path)
    print("wrote", os.path.relpath(path, ROOT))


if __name__ == "__main__":
    import time
    t = time.time()
    d = cached()
    print("crowd: title light %d x %d bytes, title crowd %d x %d, booed light %d x %d, booed crowd %d x %d; "
          "the people from row %d; cups %s; %.1f s"
          % (len(d["title_light"]), len(d["title_light"][0]), len(d["title_crowd"]), len(d["title_crowd"][0]),
             len(d["boo_light"]), len(d["boo_light"][0]), len(d["boo_crowd"]), len(d["boo_crowd"][0]),
             d["crowd_top"], cups(), time.time() - t))
    preview(d)
