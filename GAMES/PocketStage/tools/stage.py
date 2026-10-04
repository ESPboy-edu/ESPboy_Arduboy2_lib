#!/usr/bin/env python3
"""The stage of the play page, as sprites for the flash chip: the band, the
kit, the amps and the crowd.

After Joel's mock-up of 2026-09-30: a wide stage seen from the crowd, lamps
on a truss along the top with their beams in the haze, the kit on a riser
at the back, amp stacks at the sides, the guitarist and the bassist small
on the stage, and the crowd's heads and hands in front at the bottom
corners. Everything on the lit stage is a black shape with a white edge,
the guitars white.

The figures are modelled (tools/render.py) and rendered in the outline
style: a figure's parts in the picture plane at their depths, brought down
to the sprite. The kit, the amps and the cabinets are drawn as text, one
character a pixel ('#' white, '.' black, ' ' see-through). The crowd is
laid out here, people in three rows, the near ones big, and rendered.

The band seen close up (2026-09-30, shaded, kept at Joel's wish: the tag
band-closeup, notes/band-closeup.png) is still here under CLOSEUP:
`python3 tools/stage.py closeup` renders it to build/band_closeup.png.

`python3 tools/stage.py` writes build/stage.png (all the frames, five times
the size) and prints the sizes; build_data.py imports sprites(). Rendering
takes a while, so the result is kept in build/stage_cache.json against a
hash of this file and render.py (`-f` renders afresh).
"""
import hashlib
import json
import os
import random
import sys
from math import cos, sin

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from render import Capsule, Ellipse, Flip, Part, Poly, Scaled, render, sprite_bytes, text_frame, to_bits  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

SKIN = 1.0
EYE = 0.04


class Fig:
    """the parts of a figure; k and off scale and move what is added, fat
    thickens the rounded parts (small figures need it or their limbs break up)"""

    def __init__(self, k=1.0, off=(0.0, 0.0), fat=1.0):
        self.parts = []
        self.k, self.off, self.fat = k, off, fat
        self.obj = 0

    def add(self, shape, z, r, albedo, gloss=0.0, style=None):
        if self.k != 1.0 or self.off != (0.0, 0.0):
            shape = Scaled(shape, self.k, self.off)
        self.parts.append(Part(shape, z, r * self.k * self.fat, albedo, gloss, style, self.obj))


def add2(a, b):
    return (a[0] + b[0], a[1] + b[1])


def rot(p, c, ang):
    """p turned about c by ang (radians, clockwise on the screen)"""
    dx, dy = p[0] - c[0], p[1] - c[1]
    return (c[0] + dx * cos(ang) - dy * sin(ang), c[1] + dx * sin(ang) + dy * cos(ang))


def ik(s, t, l1, l2, side):
    """the elbow (or knee) of two bones from s to t, bent out to one side"""
    from math import hypot, sqrt
    dx, dy = t[0] - s[0], t[1] - s[1]
    d = hypot(dx, dy)
    reach = l1 + l2 - 0.05
    if d > reach:
        d = reach
    a = (l1 * l1 - l2 * l2 + d * d) / (2 * d)
    hh = sqrt(max(0.0, l1 * l1 - a * a))
    ux, uy = dx / hypot(dx, dy), dy / hypot(dx, dy)
    return (s[0] + a * ux - uy * hh * side, s[1] + a * uy + ux * hh * side)


# ------------------------------------------------------------------ people
def head(fig, c, ang, size, look, z=3.0):
    """a head: the face, eyes and mouth, hair before and behind; size = its half height"""
    rx, ry = size * 0.78, size
    fig.add(Ellipse(c, rx, ry, ang), z, rx * 0.95, SKIN)
    e0 = rot((c[0] - rx * 0.62, c[1] - ry * 0.08), c, ang)
    e1 = rot((c[0] + rx * 0.62, c[1] - ry * 0.08), c, ang)
    fig.add(Capsule(e0, e1, ry * 0.16), z + 1.0, 0.3, EYE)
    m0 = rot((c[0] - rx * 0.3, c[1] + ry * 0.5), c, ang)
    m1 = rot((c[0] + rx * 0.3, c[1] + ry * 0.5), c, ang)
    fig.add(Capsule(m0, m1, ry * 0.11), z + 1.0, 0.2, 0.12)
    hair = look.get("hair")
    if hair == "long":
        pts = [(-1.15, -0.55), (-0.85, -1.1), (0, -1.28), (0.85, -1.1), (1.2, -0.45), (1.3, 0.9),
               (1.4, 2.2), (0.72, 2.4), (0.42, 1.35), (-0.42, 1.35), (-0.72, 2.4), (-1.4, 2.2),
               (-1.3, 0.9)]
        fig.add(Poly([rot((c[0] + px * rx, c[1] + py * ry), c, ang) for px, py in pts], 0.4),
                z - 0.5, 1.6, look.get("hair_albedo", 0.55), 0.5)
        pts = [(-0.98, -0.6), (-0.6, -1.05), (0, -1.18), (0.62, -1.05), (1.0, -0.55), (0.75, -0.35),
               (0.1, -0.62), (-0.7, -0.3)]
        fig.add(Poly([rot((c[0] + px * rx, c[1] + py * ry), c, ang) for px, py in pts], 0.2),
                z + 0.7, 0.9, look.get("hair_albedo", 0.55), 0.35)
    elif hair == "short":
        pts = [(-1.05, -0.35), (-0.9, -0.9), (-0.45, -1.16), (0.3, -1.18), (0.9, -0.95), (1.08, -0.3),
               (0.98, 0.1), (0.85, -0.45), (0.35, -0.75), (-0.5, -0.7), (-0.95, -0.05)]
        fig.add(Poly([rot((c[0] + px * rx, c[1] + py * ry), c, ang) for px, py in pts], 0.25),
                z + 0.7, 0.8, look.get("hair_albedo", 0.3), 0.2)
    if look.get("beard"):
        pts = [(-0.95, 0.2), (-0.75, 0.85), (-0.3, 1.1), (0.3, 1.1), (0.75, 0.85), (0.95, 0.2),
               (0.7, 0.55), (0, 0.75), (-0.7, 0.55)]
        fig.add(Poly([rot((c[0] + px * rx, c[1] + py * ry), c, ang) for px, py in pts], 0.2),
                z + 0.8, 0.6, 0.3)


def body(fig, J, look):
    """a torso on two legs, the arms from J's shoulders to J's hands, bent out to a side"""
    ls, rs, lh, rh = J["ls"], J["rs"], J["lh"], J["rh"]
    shirt = look.get("shirt", 0.45)
    fig.add(Poly([ls, rs, rh, lh], 1.6), 1.0, 3.6, shirt)
    if look.get("vest"):
        mx = (ls[0] + rs[0]) / 2
        for sh, hip, sg in ((ls, lh, 1), (rs, rh, -1)):
            fig.add(Poly([(sh[0] - sg * 1.5, sh[1] + 0.5), (sh[0] + sg * 5.0, sh[1] + 0.5), (mx - sg * 1.3, sh[1] + 8.0),
                          (mx - sg * 1.3, hip[1] + 1.0), (hip[0] - sg * 2.5, hip[1] + 1.0)], 0.5), 1.25, 0.6, look["vest"])
    fig.add(Capsule(J["neck0"], J["neck1"], 1.7), 2.2, 1.7, SKIN)
    sleeve = 0.0 if look.get("sleeveless") else 0.55
    for side in ("l", "r"):
        s, hand = J[side + "s"], J[side + "hand"]
        z = J.get(side + "z", 2.5)
        el = ik(s, hand, J.get("upper", 12.0), J.get("fore", 11.0), J.get(side + "bend", 1))
        fig.add(Capsule(s, el, 2.2), z, 2.2, shirt if sleeve else SKIN)
        if sleeve:
            fig.add(Capsule(s, add2(s, ((el[0] - s[0]) * sleeve, (el[1] - s[1]) * sleeve)), 2.5), z + 0.1, 2.5, shirt)
        fig.add(Capsule(el, hand, 1.85), z + 0.2, 1.85, SKIN)
        fig.add(Ellipse(hand, 2.3, 2.0), J.get(side + "hz", z + 0.4), 1.9, SKIN)
    jeans = look.get("jeans", 0.5)
    for hip, knee, foot, z in ((lh, J["lknee"], J["lfoot"], 0.0), (rh, J["rknee"], J["rfoot"], 0.0)):
        fig.add(Capsule(hip, knee, 3.2), z + 0.3, 3.2, jeans)
        fig.add(Capsule(knee, foot, 2.5), z, 2.5, jeans)
        if look.get("feet"):
            fig.add(Ellipse((foot[0] + look["feet"], foot[1] + 2.0), 4.0, 2.0), z + 0.1, 1.5, 0.2)


class Guitar:
    """a guitar: the outline of its body in its own frame (u along the neck
    to the headstock, v across, the neck joint at 0,0), laid on the picture
    at a point and an angle, scaled to the body it hangs on"""

    def __init__(self, pos, ang, kind, scale=0.8):
        self.pos, self.ang, self.kind, self.s = pos, ang, kind, scale
        self.ca, self.sa = cos(ang), sin(ang)

    def at(self, u, v=0.0):
        u, v = u * self.s, v * self.s
        return (self.pos[0] + u * self.ca - v * self.sa, self.pos[1] + u * self.sa + v * self.ca)

    def along(self, px, v=0.0):
        """a point px pixels along the neck (not scaled with the body)"""
        return self.at(px / self.s, v)

    def build(self, fig, z):
        kind = self.kind
        was, fig.obj = fig.obj, fig.obj + 100
        if kind == "lp":                        # a single cutaway, the horn on the treble side
            pts = [(0, -2.4), (-3, -5.2), (-8, -6.8), (-13, -6.4), (-16.6, -3.4), (-17.2, 1.0), (-15.4, 5.4),
                   (-11, 7.6), (-6.5, 7.2), (-3.4, 5.6), (-1.6, 6.4), (1.2, 5.6), (0.3, 3.4), (0, 2.4)]
            neck_len, body_albedo, gloss = 17.0, 0.85, 1.0
        else:                                   # a bass: two horns, the upper one long
            pts = [(0, -2.6), (-1.2, -3.8), (0.6, -6.2), (3.0, -7.4), (1.0, -8.6), (-4, -8.8), (-9, -7.8),
                   (-14, -5.6), (-17.8, -1.4), (-17.8, 2.8), (-15, 6.6), (-10.5, 8.0), (-6, 7.6),
                   (-3, 5.8), (-1.6, 6.4), (0.8, 5.4), (-0.4, 3.6), (0, 2.6)]
            neck_len, body_albedo, gloss = 22.0, 0.85, 0.8
        fig.add(Poly([self.at(u, v) for u, v in pts], 0.9), z, 2.4, body_albedo, gloss, "white")
        if kind == "lp":
            for u in (-2.8, -7.4):
                fig.add(Poly([self.at(u + du, dv) for du, dv in ((-1.2, -3.0), (1.2, -3.0), (1.2, 3.0), (-1.2, 3.0))], 0.3),
                        z + 0.2, 0.3, 0.1)
        else:
            fig.add(Poly([self.at(u, v) for u, v in ((-1.0, 1.0), (-2.0, 4.6), (-5.0, 7.0), (-10.5, 7.2), (-14.0, 5.0),
                                                      (-14.6, 1.6), (-11.0, 0.8), (-5.0, 1.4))], 0.6), z + 0.2, 0.5, 0.1)
            fig.add(Poly([self.at(-4.5 + du, dv) for du, dv in ((-1.0, -2.6), (1.0, -2.6), (1.0, 2.6), (-1.0, 2.6))], 0.3),
                    z + 0.3, 0.3, 0.55)
        fig.add(Capsule(self.at(-13.5, -1.5), self.at(-13.5, 1.5), 0.7), z + 0.2, 0.5, 0.2)      # the bridge
        fig.add(Capsule(self.at(-1.0), self.along(neck_len), 1.2), z + 0.3, 1.2, 0.85, 0.0, "white")
        fig.add(Poly([self.along(neck_len - 0.5, -1.4), self.along(neck_len + 5.0, -2.1), self.along(neck_len + 5.5, 1.6),
                      self.along(neck_len - 0.5, 1.4)], 0.5), z + 0.4, 0.7, 0.22, 0.5, "white")
        fig.obj = was


GUITARIST_LOOK = {"hair": "long", "hair_albedo": 0.5, "shirt": 0.55, "jeans": 0.5, "feet": -2.0}
BASSIST_LOOK = {"hair": "short", "hair_albedo": 0.25, "beard": True, "shirt": 0.9, "vest": 0.15, "jeans": 0.35,
                "sleeveless": True, "feet": 2.0}


def guitarist_model(fig, frame):
    """the guitarist, about 66 units tall from the hair to the feet, the head at (19, 6.5)"""
    lift = 0.0
    hx, hy, tilt = 19.0, 6.5, 0.12          # the head, looking down at the strings a little
    stance = 0.0                            # the weight on one foot
    if frame == 1:
        stance = 1.0
        hx, tilt = 18.4, 0.06
    if frame == 2:                          # the strum: the head comes up
        hy, tilt = 6.2, -0.05
    if frame == 3:                          # the jump: the knees up, the head back
        lift = 4.0
        hy, tilt = 5.6, -0.22
    J = {
        "neck0": (hx, hy + 3.6), "neck1": (hx - 0.4, hy + 7.6),
        "ls": (hx - 9.2 + stance * 0.6, hy + 8.6 - stance * 0.4), "rs": (hx + 8.4 + stance * 0.6, hy + 8.4 + stance * 0.5),
        "lh": (hx - 5.6 + stance * 1.2, hy + 26.5), "rh": (hx + 5.2 + stance * 1.2, hy + 26.5),
        "lbend": 1, "rbend": -1, "lz": 4.8, "rz": 2.4, "rhz": 4.6,
    }
    g = Guitar((hx - 2.0, hy + 24.5), -0.38, "lp")
    J["rhand"] = g.along(11.0, -0.6)                                # on the neck
    J["lhand"] = g.at(-4.0, 1.0) if frame != 2 else g.at(-5.5, 5.5)   # over the strings; swept down
    J["lknee"] = (hx - 10.5 + stance, hy + 41.5 - lift * 2.2)
    J["rknee"] = (hx + 7.5 + stance * 2.0, hy + 40.5 - lift * 1.8)
    J["lfoot"] = (hx - 13.5, hy + 58 - lift * 3.5)
    J["rfoot"] = (hx + 8.5 + stance * 1.5, hy + 57 - lift * 3.0)
    body(fig, J, GUITARIST_LOOK)
    fig.add(Capsule(g.at(-3.5, -6.8), J["rs"], 0.9), 1.4, 0.9, 0.22)  # the strap
    g.build(fig, 3.6)
    head(fig, (hx, hy), tilt, 4.5, GUITARIST_LOOK)


def bassist_model(fig, frame):
    """the bassist stands straighter, the bass slung long; headbangs with star power"""
    hx, hy, tilt = 20.0, 6.5, 0.18
    stance = 1.0 if frame == 1 else 0.0
    if frame == 2:
        tilt = 0.10
    if frame == 3:                          # the headbang: the head down and forward
        hy, tilt = 9.0, 0.95
    J = {
        "neck0": (20.0, 10.0), "neck1": (19.6, 14.0),
        "ls": (11.0 + stance * 0.5, 15.2), "rs": (28.6 + stance * 0.5, 15.0),
        "lh": (14.6 + stance * 0.8, 33.0), "rh": (25.2 + stance * 0.8, 33.0),
        "lbend": 1, "rbend": -1, "lz": 4.8, "rz": 2.4, "rhz": 4.6, "upper": 12.5, "fore": 11.5,
    }
    g = Guitar((16.5, 33.5), -0.28, "pb")
    J["rhand"] = g.along(15.0, -0.7)
    J["lhand"] = g.at(-3.5, 0.5) if frame != 2 else g.at(-4.5, 3.8)
    J["lknee"] = (13.5 + stance * 1.5, 48.0)
    J["rknee"] = (26.0 + stance * 0.5, 48.5)
    J["lfoot"] = (12.5, 64.0)
    J["rfoot"] = (27.5, 64.0)
    body(fig, J, BASSIST_LOOK)
    fig.add(Capsule(g.at(-2.0, -8.0), J["rs"], 0.9), 1.4, 0.9, 0.22)
    g.build(fig, 3.6)
    head(fig, (hx, hy), tilt, 4.5, BASSIST_LOOK)


# the small figures on the stage: the model at a fraction of its size, the
# limbs thickened so they hold together at that size; the bassist mirrored,
# so that both necks point at the middle of the stage
FIG_K = 0.36
FIG_W, FIG_H = 20, 24


def stage_guitarist(frame):
    fig = Fig(FIG_K, (2.5, 0.0), 1.3)
    guitarist_model(fig, frame)
    return fig.parts


def stage_bassist(frame):
    fig = Fig(FIG_K, (2.0, 0.0), 1.3)
    bassist_model(fig, frame)
    for p in fig.parts:
        p.shape = Flip(p.shape, FIG_W)
    return fig.parts


# the band seen close up, kept (band-closeup)
def closeup_guitarist(frame):
    fig = Fig()
    guitarist_model(fig, frame)
    return fig.parts


def closeup_bassist(frame):
    fig = Fig()
    bassist_model(fig, frame)
    return fig.parts


def closeup_drummer(frame):
    """56 x 24: the kit from the front, the drummer behind it"""
    fig = Fig()
    nod = 0.7 if frame == 2 else 0.0
    hx, hy = 28.0, 5.0 + nod
    ls, rs = (20.5, 10.5), (35.5, 10.5)
    fig.add(Poly([ls, rs, (33.0, 19.0), (23.0, 19.0)], 1.2), 1.0, 2.8, 0.3)
    fig.add(Capsule((hx, hy + 2.6), (hx, hy + 5.4), 1.2), 2.2, 1.2, SKIN)
    head(fig, (hx, hy), 0.12 + nod * 0.4, 3.6, {"hair": "short", "hair_albedo": 0.25})
    ltip = (13.0, 12.6) if frame == 2 else (10.5, 7.5)
    rtip = (46.5, 4.6) if frame == 1 else (42.0, 1.5)
    for sh, tip, bend, back in ((ls, ltip, 1, (-4.0, 4.5)), (rs, rtip, -1, (4.0, 4.5))):
        hand = (tip[0] + back[0], tip[1] + back[1])
        el = ik(sh, hand, 6.5, 6.0, bend)
        fig.add(Capsule(sh, el, 1.5), 2.5, 1.5, 0.3)
        fig.add(Capsule(el, hand, 1.3), 2.7, 1.3, SKIN)
        fig.add(Ellipse(hand, 1.6, 1.4), 7.0, 1.3, SKIN)
        fig.add(Capsule(hand, tip, 0.45), 7.2, 0.45, 0.9)

    def stand(a, b):
        fig.add(Capsule(a, b, 0.45), 3.0, 0.45, 0.75, 0.5)

    def cymbal(c, rx, ang, z=6.0):
        fig.add(Ellipse(c, rx, rx * 0.26, ang), z, 0.9, 0.85, 1.0)
        fig.add(Ellipse(c, rx * 0.22, rx * 0.09, ang), z + 1.0, 0.6, 0.95, 1.0)

    def drum(c, rx, depth, shell):
        x, y = c
        fig.add(Poly([(x - rx, y), (x + rx, y), (x + rx, y + depth), (x - rx, y + depth)], 0.4), 5.0, 1.6, shell, 0.7)
        fig.add(Ellipse((x, y), rx, rx * 0.3), 5.5, 0.9, 0.95, 0.3)

    stand((5.0, 10.5), (5.0, 23.5))
    stand((11.0, 4.0), (12.5, 23.5))
    stand((47.0, 4.5), (45.5, 23.5))
    drum((28.0, 10.0), 4.6, 5.0, 0.2)
    drum((13.0, 13.5), 5.2, 4.5, 0.6)
    drum((45.0, 13.5), 5.6, 7.0, 0.2)
    cymbal((5.0, 10.0), 5.0, 0.0)
    cymbal((5.0, 8.6), 5.0, 0.0, 6.5)
    cymbal((11.0, 3.0), 6.5, 0.15)
    cymbal((47.0, 3.5), 7.0, -0.12)
    return fig.parts


# ------------------------------------------------------------------ the crowd
def crowd(frame):
    """44 x 32, the left corner of the screen from row 32 down (the right one
    is its mirror image): a sea of heads from the bottom edge up, the near
    ones big and cut by the edge, black with a white edge; fists and horns
    in the air, up on the beat (frame 1)."""
    rng = random.Random(11)
    fig = Fig()
    up = 3.0 if frame else 0.0
    bob = 1.0 if frame else 0.0
    rows = [                                # from the back: (y of the heads, their radius, the spacing, depth)
        (9.0, 2.2, 5.0, 1.0),
        (14.5, 3.0, 6.5, 2.0),
        (20.5, 4.0, 8.5, 3.0),
        (27.5, 5.5, 11.0, 4.0),
    ]
    obj = 1
    for y, r, dx, z in rows:
        x = rng.uniform(-3.0, 2.0)
        while x < 48:
            fig.obj = obj
            obj += 1
            hx, hy = x + rng.uniform(-1.0, 1.0), y + rng.uniform(-0.8, 0.8) - bob
            fig.add(Ellipse((hx, hy), r * 0.9, r), z + 0.5, r * 0.8, 0.0)                     # the head
            fig.add(Ellipse((hx, hy + r * 1.7), r * 2.0, r * 0.9), z, r * 0.6, 0.0)          # the shoulders
            if rng.random() < 0.6:                                                           # an arm up
                side = rng.choice((-1, 1))
                sx, sy = hx + side * r * 1.3, hy + r * 1.2
                fx, fy = sx + side * r * 0.5 + rng.uniform(-1, 1), hy - r * 3.0 - rng.uniform(0, 5.0) - up
                fig.add(Capsule((sx, sy), (fx, fy), r * 0.36), z + 0.2, r * 0.36, 0.0)
                fig.add(Ellipse((fx, fy), r * 0.55, r * 0.6), z + 0.3, r * 0.45, 0.0)        # the fist
                if rng.random() < 0.5:                                                       # the horns
                    for hdx in (-0.35, 0.45):
                        fig.add(Capsule((fx + hdx * r, fy - r * 0.35), (fx + hdx * r * 1.3, fy - r * 1.4), r * 0.14),
                                z + 0.3, r * 0.14, 0.0)
            x += dx + rng.uniform(-1.0, 1.0)
    return fig.parts


# ------------------------------------------------------------------ text art
# the kit on the riser, 30 x 16: the drummer's head and shoulders behind two
# toms, the kick in front (its lower part under the far end of the highway),
# the snare left, the floor tom right, a crash and a ride on stands; three
# frames: the sticks up, the right one on the ride, the left one on the snare
KIT0 = [
    "             .###.            ",
    "             #...#            ",
    "        #    #...#    #       ",
    "         #   .###.   #        ",
    "         ####.....########### ",
    "  ########.#.......#.#  #     ",
    "     #   #####...#####        ",
    "         #...#...#...#  #     ",
    "     #   #...#####...#        ",
    "         #####...##### #######",
    "  ######    #.....#    #.....#",
    "  #....#   #.......#   #.....#",
    "  #....#   #...#...#   #.....#",
    "  ######   #.......#   #######",
    "            #.....#           ",
    "             #...#            ",
]
KIT1 = list(KIT0)                       # the right stick comes down on the ride, which dips
KIT1[2] = "        #    #...#            "
KIT1[3] = "         #   .###.            "
KIT1[4] = "         ####.....######      "
KIT1[5] = "  ########.#........######### "
KIT2 = list(KIT0)                       # the left stick on the snare, the head nods
KIT2[0] = "                              "
KIT2[1] = "             .###.            "
KIT2[2] = "             #...#    #       "
KIT2[3] = "             #...#   #        "
KIT2[4] = "         ####.###.########### "
KIT2[5] = "  ########..#......#.#  #     "
KIT2[7] = "         ##..#...#...#  #     "
KIT2[9] = "        ######...##### #######"
# an amp stack, 9 x 24, as tall as the two at the front: the head with its
# panel, two cabinets with a grille
AMP = [
    "#########",
    "#.......#",
    "#.#.#.#.#",
    "#########",
    "#########",
    "#.......#",
    "#.#.#.#.#",
    "#.......#",
    "#.#.#.#.#",
    "#.......#",
    "#.#.#.#.#",
    "#.......#",
    "#.#.#.#.#",
    "#########",
    "#########",
    "#.......#",
    "#.#.#.#.#",
    "#.......#",
    "#.#.#.#.#",
    "#.......#",
    "#.#.#.#.#",
    "#.......#",
    "#.#.#.#.#",
    "#########",
]
# a cabinet by the riser, 7 x 7
CAB = [
    "#######",
    "#.....#",
    "#######",
    "#.#.#.#",
    "#.....#",
    "#.#.#.#",
    "#######",
]


class Rendered:
    def __init__(self, name, build, w, h, frames, style="outline", cover=0.3):
        self.name, self.build, self.w, self.h, self.frames, self.style, self.cover = name, build, w, h, frames, style, cover

    def render(self):
        out = []
        for k in range(self.frames):
            own, lum = render(self.build(k), self.w, self.h, self.style == "shaded")
            out.append(to_bits(own, lum, self.w, self.h, self.style, self.cover))
        return out


class Text:
    def __init__(self, name, frames, w, h):
        self.name, self.frames_art, self.w, self.h, self.frames = name, frames, w, h, len(frames)

    def render(self):
        return [text_frame(rows, self.w, self.h) for rows in self.frames_art]


def mirrored(fn, w):
    def build(frame):
        parts = fn(frame)
        for p in parts:
            p.shape = Flip(p.shape, w)
        return parts
    return build


STAGE = [
    Rendered("GUITARIST", stage_guitarist, FIG_W, FIG_H, 4, "solid"),
    Rendered("BASSIST", stage_bassist, FIG_W, FIG_H, 4, "solid"),
    Text("KIT", [KIT0, KIT1, KIT2], 30, 16),
    Text("AMP", [AMP], 9, 24),
    Text("CAB", [CAB], 7, 8),
    Rendered("CROWD_L", crowd, 44, 32, 2),
    Rendered("CROWD_R", mirrored(crowd, 44), 44, 32, 2),
]
CLOSEUP = [
    Rendered("GUITARIST", closeup_guitarist, 44, 56, 4, "shaded", 0.5),
    Rendered("BASSIST", closeup_bassist, 44, 56, 4, "shaded", 0.5),
    Rendered("DRUMMER", closeup_drummer, 56, 24, 3, "shaded", 0.5),
]


# ------------------------------------------------------------------ sprites
def render_all(figures):
    """-> {name: [(image rows, mask rows), ...]} for every figure's frames"""
    return {f.name: f.render() for f in figures}


def cached_frames(figures=STAGE, tag="stage", force=False):
    src = open(__file__, "rb").read() + open(os.path.join(HERE, "render.py"), "rb").read()
    key = hashlib.sha1(src).hexdigest() + tag
    path = os.path.join(ROOT, "build", "%s_cache.json" % tag)
    if not force:
        try:
            with open(path) as f:
                c = json.load(f)
            if c.get("key") == key:
                return c["frames"]
        except (OSError, ValueError, KeyError):
            pass
    frames = render_all(figures)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump({"key": key, "frames": frames}, f)
    return frames


def sprites():
    """-> [(name, width, pages, frames, bytes)]: the stage's sprites, the frames one after the other"""
    frames = cached_frames()
    out = []
    for f in STAGE:
        data = bytearray()
        for img, msk in frames[f.name]:
            data += sprite_bytes(img, msk, f.w, f.h)
        out.append((f.name, f.w, f.h // 8, f.frames, bytes(data)))
    return out


def preview(path, figures, frames):
    """all the frames at five times the size: white on black, the mask's
    halo and the outside in two greys, over a dithered light like the stage's"""
    from PIL import Image
    scale, gap = 5, 6
    cols = max(f.frames for f in figures)
    cell_w = max(f.w for f in figures)
    cell_h = sum(f.h + gap for f in figures)
    im = Image.new("RGB", (cols * (cell_w + gap) * scale, cell_h * scale), (40, 40, 40))
    px = im.load()
    y0 = 0
    for f in figures:
        for k, (img, msk) in enumerate(frames[f.name]):
            x0 = k * (cell_w + gap)
            for y in range(f.h):
                for x in range(f.w):
                    if img[y][x]:
                        col = (255, 255, 255)
                    elif msk[y][x]:
                        col = (0, 0, 0)
                    else:
                        col = (95, 95, 95) if (x ^ y) & 1 == 0 and x > f.w // 3 else (12, 12, 20)
                    for dy in range(scale):
                        for dx in range(scale):
                            px[(x0 + x) * scale + dx, (y0 + y) * scale + dy] = col
        y0 += f.h + gap
    im.save(path)
    print("wrote", os.path.relpath(path, ROOT))


if __name__ == "__main__":
    force = "-f" in sys.argv
    os.makedirs(os.path.join(ROOT, "build"), exist_ok=True)
    if "closeup" in sys.argv:
        frames = cached_frames(CLOSEUP, "closeup", force)
        preview(os.path.join(ROOT, "build", "band_closeup.png"), CLOSEUP, frames)
    else:
        frames = cached_frames(STAGE, "stage", force)
        for name, w, pages, n, data in sprites():
            print("%-10s %2d x %2d, %d frames, %d bytes" % (name, w, pages * 8, n, len(data)))
        preview(os.path.join(ROOT, "build", "stage.png"), STAGE, frames)
