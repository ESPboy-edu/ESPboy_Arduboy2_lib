"""Demons: the third faction.

Heroes are white masses, men are black cloth engraved with diagonals.
Demons are black, carved with concentric contour lines that follow their own
outline -- like the grain of the woodblock showing through -- with lit eyes.
Same 64x48 frames as figures.py, built facing right, mirrored to face left.
"""
import math
from wood import C, ellipse, poly, rectm, thick, halo, fill, edge
from figures import W, H, GROUND


def depth(m):
    """distance (in steps) from each pixel of m to its outside edge."""
    d = {}
    frontier = [p for p in m if any((p[0] + dx, p[1] + dy) not in m for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))]
    for p in frontier: d[p] = 0
    k = 0
    while frontier:
        k += 1
        nxt = []
        for (x, y) in frontier:
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                q = (x + dx, y + dy)
                if q in m and q not in d:
                    d[q] = k; nxt.append(q)
        frontier = nxt
    return d


def grain(c, m, spacing=3, rings=3):
    """contour rings hugging the outline; the heart of the mass stays dark."""
    halo(c, m, 1)
    d = depth(m)
    for p, k in d.items():
        c.px(*p, 1 if (k % spacing == 0 and k < spacing * rings) else 0)


def eyes(c, pts, r=1):
    for (x, y) in pts:
        e = rectm(x, y, r + 1, r + 1)
        halo(c, e, 2); fill(c, e)


# ------------------------------------------------------------- the Drowned
def drowned(pose):
    """What the river kept: hunched, long-armed, dissolving into drips below the waist."""
    c = C(W, H)
    s = pose == 'strike'
    hx, hy = (30 + (5 if s else 0), 15 + (3 if s else 0))          # head
    body = poly([(18, 18), (hx - 2, hy + 2), (hx + 4, hy + 6), (34, 32), (30, 38), (16, 38), (14, 28)])
    # ragged hem that becomes drips
    for k, x in enumerate(range(15, 32, 3)):
        body |= thick(x, 36, x + (k % 2), 40 + (k * 5) % 6, 1.0)
    head = ellipse(hx, hy, 5, 4.5)
    hair = set()
    for k in range(6):                                              # lank hair hanging forward
        hair |= thick(hx - 4 + k, hy - 3, hx - 3 + k + (3 if s else 1), hy + 7 + (k % 3), 0.6)
    if s:
        arm = thick(26, 22, 40, 22, 1.4) | thick(40, 22, 52, 25, 1.2)
        arm2 = thick(22, 23, 34, 28, 1.3) | thick(34, 28, 46, 31, 1.1)
        claws = [(52, 25), (46, 31)]
    else:
        arm = thick(27, 22, 33, 32, 1.4) | thick(33, 32, 36, 42, 1.2)
        arm2 = thick(21, 23, 22, 33, 1.3) | thick(22, 33, 24, 42, 1.1)
        claws = [(36, 42), (24, 42)]
    grain(c, arm2)
    grain(c, body | head)
    grain(c, arm)
    for (x, y) in claws:
        for k in (-2, 0, 2):
            c.line(x, y, x + 3, y + k, 1)
    for p in hair: c.px(*p, 1 if p[0] % 2 == 0 else 0)
    eyes(c, [(hx + 1, hy - 2)])
    # drips falling beneath it
    for x, y in ((17, 44), (23, 45), (29, 43)):
        c.px(x, y); c.px(x, y + 1)
    return c


# ------------------------------------------------------------- the Bellwraith
def bellwraith(pose):
    """A drowned bell that learned to move: hovering, hollow-eyed, the clapper its tongue."""
    c = C(W, H)
    s = pose == 'strike'
    cx, top = 28 + (4 if s else 0), 6
    tilt = 4 if s else 0
    bell = set()
    for y in range(top, top + 28):
        t = (y - top) / 27
        half = 5 + 8 * t ** 1.6 + (3 if t > 0.85 else 0)
        sh = tilt * t
        for x in range(int(cx + sh - half), int(cx + sh + half) + 1): bell.add((x, y))
    bell |= rectm(cx - 2, top - 3, 5, 4)                             # the crown loop
    grain(c, bell)
    # lip of the bell: a bright carved band
    for x in range(cx - 16, cx + 17 + tilt):
        if (x, top + 24) in bell: c.px(x, top + 24, 1)
    # hollow eyes and a mouth-crack
    eyes(c, [(cx - 5 + tilt // 2, top + 11), (cx + 3 + tilt // 2, top + 11)], 2)
    c.line(cx - 3 + tilt // 2, top + 18, cx + 3 + tilt // 2, top + 17, 1)
    # clapper tongue swinging out from under
    cl = (cx + (9 if s else 1), top + 34)
    tongue = thick(cx + tilt, top + 27, *cl, 1.1) | ellipse(cl[0], cl[1], 2.2, 2.2)
    halo(c, tongue, 1); fill(c, tongue)
    # wisps trailing below (it never touches the ground)
    for k, x in enumerate(range(cx - 10, cx + 11, 5)):
        for j in range(6):
            c.px(x + (j % 2) - (2 if s else 0), top + 30 + j * 2 + k % 2)
    if s:   # the toll: rings of sound in front of it
        for r in (8, 13, 18):
            for a in range(-40, 41, 8):
                t = math.radians(a)
                c.px(round(cx + 16 + r * math.cos(t)), round(top + 16 + r * math.sin(t)))
    return c


# ------------------------------------------------------------- the Horned
def horned(pose):
    """The brute: goat legs, a bull's shoulders, horns that curl back."""
    c = C(W, H)
    s = pose == 'strike'
    hip = (24 + (3 if s else 0), 30)
    # digitigrade legs: thigh forward, shin back, hoof
    for (k1, k2, ft, far) in (((hip[0] - 3, 36), (hip[0] - 7, 41), (hip[0] - 4, GROUND), True),
                               ((hip[0] + 4, 36), (hip[0] + 1, 41), (hip[0] + 5, GROUND), False)):
        m = thick(*hip, *k1, 2.4) | thick(*k1, *k2, 1.8) | thick(*k2, *ft, 1.4) | rectm(int(ft[0]) - 1, GROUND - 1, 4, 2)
        grain(c, m)
    torso = poly([(hip[0] - 7, hip[1] + 2), (hip[0] + 7, hip[1] + 2), (hip[0] + 13, hip[1] - 12),
                  (hip[0] + 6, hip[1] - 19), (hip[0] - 8, hip[1] - 17), (hip[0] - 11, hip[1] - 9)])
    head = ellipse(hip[0] + 11, hip[1] - 20, 4.5, 4) | thick(hip[0] + 4, hip[1] - 15, hip[0] + 10, hip[1] - 19, 2.5)
    grain(c, torso, rings=2)
    grain(c, head, spacing=2, rings=1)
    # horns: crescents sweeping back from the crown, bright against the dark
    hx, hy = hip[0] + 10, hip[1] - 22
    horn = set()
    for k in range(10):
        a = math.radians(40 + k * 16)
        r = 7.5
        px_, py_ = hx - 3 + r * math.cos(a), hy + 1 - r * math.sin(a)
        horn |= thick(px_, py_, px_, py_, 1.7 - k * 0.12)
    halo(c, horn, 1); fill(c, horn)
    for k in range(0, 10, 3):
        a = math.radians(40 + k * 16); c.px(round(hx - 3 + 7.5 * math.cos(a)), round(hy + 1 - 7.5 * math.sin(a)), 0)   # ridges
    eyes(c, [(hx + 1, hy + 1)])
    for k in range(3): c.px(hx + 4, hy + 5 + k % 2)                  # muzzle line
    # arm + claws: raised overhead at rest, slashing down when it strikes
    sh = (hip[0] + 6, hip[1] - 15)
    if s:
        el, hand = (sh[0] + 9, sh[1] + 2), (sh[0] + 16, sh[1] + 10)
    else:
        el, hand = (sh[0] + 5, sh[1] + 7), (sh[0] + 9, sh[1] + 15)
    arm = thick(*sh, *el, 2.2) | thick(*el, *hand, 1.8)
    grain(c, arm)
    for k in (-3, 0, 3):
        a = (8, 6) if s else (4, 4)
        c.line(hand[0], hand[1] + k // 2, hand[0] + a[0], hand[1] + a[1] + k, 1)
    if s:   # claw trails
        for k in range(3):
            c.line(hand[0] - 6 + k * 3, hand[1] - 12, hand[0] + 2 + k * 3, hand[1] + 4, 1, dash=lambda i: i % 3 != 2)
    return c


def nhal(pose):
    """What pulls at the world from under the cathedral: no legs, just a whirl of water that
    rises into a body -- a crown of horns, three eyes, long arms that reach and drag."""
    c = C(W, H)
    s = pose == 'strike'
    cx, top = 26 + (2 if s else 0), GROUND - 31          # top: the crown of the head
    # the undertow: three broken rings turning round its base
    for k, r in enumerate((7, 12, 17)):
        ry = r * 0.25
        for a in range(0, 360, 2):
            if (a + k * 70 + (40 if s else 0)) % 120 < 45: continue
            t = math.radians(a)
            x, y = round(cx + r * math.cos(t)), round(GROUND - 2 + ry * math.sin(t))
            if 0 <= x < W and 0 <= y < H: c.px(x, y)
    # the body: a column twisting up out of the water into broad shoulders
    body = poly([(cx - 3, GROUND - 3), (cx + 4, GROUND - 3), (cx + 7, top + 16), (cx + 12, top + 9),
                 (cx + 6, top + 5), (cx - 6, top + 5), (cx - 11, top + 10), (cx - 6, top + 17)])
    grain(c, body, rings=3)
    head = ellipse(cx + 1, top + 4, 5, 4.5)
    grain(c, head, spacing=2, rings=1)
    # the crown: five horns fanned over the head, the middle one tallest
    crown = set()
    for k, (dx, h) in enumerate(((-5, 5), (-3, 8), (1, 10), (4, 8), (7, 5))):
        bx, by = cx + dx, top + 1
        crown |= thick(bx, by, bx + dx // 3, by - h, 1.2)
    crown = {(x, y) for (x, y) in crown if y >= 0}
    halo(c, crown, 1); fill(c, crown)
    eyes(c, [(cx - 2, top + 3), (cx + 3, top + 3), (cx + 1, top + 6)], 0)
    # arms: one reaches forward (down and dragging when it strikes), the far one trails behind
    sh = (cx + 9, top + 10)
    if s: el, hand = (sh[0] + 9, sh[1] + 5), (sh[0] + 18, sh[1] + 13)
    else: el, hand = (sh[0] + 6, sh[1] - 2), (sh[0] + 13, sh[1] + 3)
    arm = thick(*sh, *el, 2.2) | thick(*el, *hand, 1.7)
    grain(c, arm)
    for k in (-4, -1, 2, 5):
        a = (7, 5) if s else (5, 3)
        c.line(hand[0], hand[1] + k // 2, hand[0] + a[0], hand[1] + a[1] + k, 1)
    far = thick(cx - 9, top + 11, cx - 16, top + 18, 2) | thick(cx - 16, top + 18, cx - 17, top + 26, 1.5)
    grain(c, far)
    if s:   # the pull: dashed streaks dragged toward it
        for k in range(4):
            c.line(hand[0] + 12, hand[1] - 9 + k * 5, hand[0] + 2, hand[1] - 5 + k * 4, 1, dash=lambda i: i % 3 != 2)
    return c


DEMONS = {'drowned': drowned, 'bellwraith': bellwraith, 'horned': horned, 'nhal': nhal}


def all_frames():
    out = []
    for name, fn in DEMONS.items():
        for pose in ('idle', 'strike'):
            r = fn(pose)
            m = C(W, H); m.paste(r, 0, 0, mirror=True)
            out.append((f'{name}_{pose}', m))
    return out


# 8x8 map sprites, facing right (mirrored + drawn as outline-and-eyes in game)
SPR = {
 'drowned':    ["...##...", "..####..", "..#.##..", ".######.", "#.####.#", "..####..", ".#.#.#..", "#.#.#.#."],
 'bellwraith': ["...##...", "..####..", ".#.##.#.", ".######.", "########", "...##...", ".#.#.#..", "#...#..."],
 'horned':     ["#.....#.", ".#.##.#.", "..####..", ".#.##.#.", "########", ".######.", ".#....#.", "##....##"],
}
