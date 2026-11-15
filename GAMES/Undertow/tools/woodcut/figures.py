"""Battle figures: posed woodcut people (and Brannock's horse).

Every figure is built facing RIGHT on a 64x48 frame, feet on y=46.
Enemies are mirrored afterwards so they face left.
Parts are drawn back-to-front; each gets a black halo first, so overlaps
read as carved separations rather than outlines.
"""
import math
from wood import C, ellipse, poly, rectm, thick, halo, fill, hatch, edge

W, H = 64, 48
GROUND = 46


def lerp(a, b, t): return a + (b - a) * t


def polar(x, y, ang, L):
    """angle in degrees, 0 = right, 90 = up (screen y grows down)."""
    t = math.radians(ang)
    return x + L * math.cos(t), y - L * math.sin(t)


class Fig:
    def __init__(s, enemy):
        s.c = C(W, H)
        s.enemy = enemy

    def part(s, m, rule=None, halo_r=1, skin=False, far=False):
        """Draw a mass: black halo, then fill.
        Heroes: solid white (far limbs get sparse carved lines).
        Enemies: black cloth engraved with white diagonals and a white rim."""
        halo(s.c, m, halo_r)
        if skin:
            fill(s.c, m); return
        if rule is not None:
            fill(s.c, m, rule); return
        if s.enemy:
            fill(s.c, m, lambda x, y: (x + y) % 4 == 0)
            for p in edge(m): s.c.px(*p, 1)
        elif far:
            fill(s.c, m, lambda x, y: (x - y) % 4 != 0)
        else:
            fill(s.c, m)

    def shade(s, m, back_x):
        """Hero cloth: white, with carved vertical folds on the back (left) side."""
        if s.enemy:
            return None
        # two carved folds hugging the back edge only
        xs = sorted({x for (x, y) in m})
        lo = xs[0] if xs else 0
        return lambda x, y: not ((x == lo + 2 or x == lo + 4) and y % 4 != 0)


# ------------------------------------------------------------- people
def person(f, spec, pose, ox=0, oy=0, mounted=False):
    """spec: look of the character.  pose: 'idle' or 'strike'."""
    c = f.c
    big = spec.get('build', 0)
    strike = pose == 'strike'
    lean = spec.get('lean_strike', 3) if strike else 0
    hip = (22 + ox + lean, (GROUND - 17) + oy)
    if mounted: hip = (ox, oy)
    neck = (hip[0] + lean + 1, hip[1] - 14 - big // 2)
    head = (neck[0] + 1, neck[1] - 5)

    # ---- legs
    robe = spec.get('robe') and not mounted
    if robe:                                                     # under a robe only the boot toes show
        for fx in ((hip[0] - 4, hip[0] + 5) if not strike else (hip[0] - 6, hip[0] + 7)):
            f.part(rectm(int(fx) - 1, GROUND - 2, 4, 2))
    elif not mounted:
        if strike:
            back_foot, front_foot = (hip[0] - 9, GROUND), (hip[0] + 9, GROUND)
            back_knee, front_knee = (hip[0] - 5, hip[1] + 9), (hip[0] + 7, hip[1] + 7)
        else:
            back_foot, front_foot = (hip[0] - 5, GROUND), (hip[0] + 5, GROUND)
            back_knee, front_knee = (hip[0] - 3, hip[1] + 9), (hip[0] + 4, hip[1] + 8)
        legr = 1.6 + big * 0.3
        for (k, ft, front) in ((back_knee, back_foot, False), (front_knee, front_foot, True)):
            m = thick(hip[0], hip[1], *k, legr) | thick(*k, *ft, legr - 0.2)
            m |= rectm(int(ft[0]) - 1, GROUND - 2, 4, 2)          # boot
            f.part(m, far=not front)
    else:
        knee = (hip[0] + 4, hip[1] + 6); ft = (hip[0] + 2, hip[1] + 12)
        m = thick(*hip, *knee, 1.8) | thick(*knee, *ft, 1.6) | rectm(int(ft[0]) - 1, int(ft[1]) - 1, 4, 2)
        f.part(m)

    # ---- far arm (shield / bow / off hand)
    sh_far = (neck[0] - 2, neck[1] + 2)
    far_hand = spec['far_hand'](pose, sh_far)
    far_elbow = ((sh_far[0] + far_hand[0]) / 2 - 1, (sh_far[1] + far_hand[1]) / 2 + 2)
    m = thick(*sh_far, *far_elbow, 1.4) | thick(*far_elbow, *far_hand, 1.3)
    f.part(m, far=True)

    # ---- cape (behind torso)
    if spec.get('cape'):
        sway = 3 if strike else 0
        m = poly([(neck[0] - 2, neck[1]), (neck[0] - 4, neck[1] + 2), (hip[0] - 10 - sway, hip[1] + (6 if not mounted else 2)),
                  (hip[0] - 4 - sway, hip[1] + (8 if not mounted else 4)), (hip[0] - 1, hip[1])])
        f.part(m, None if f.enemy else (lambda x, y: (x - y) % 3 != 0))

    # ---- torso + skirt
    sw = 6 + big
    torso = poly([(neck[0] - sw + 1, neck[1] + 1), (neck[0] + sw - 1, neck[1] + 1), (neck[0] + sw, neck[1] + 4),
                  (hip[0] + 5 + big // 2, hip[1]), (hip[0] - 5 - big // 2, hip[1]), (neck[0] - sw, neck[1] + 4)])
    if not mounted:
        skirt_l = 5 + (2 if strike else 0)
        hem = 0
        if robe: skirt_l, hem = GROUND - 2 - hip[1], 3                    # down to the ankles, flared
        torso |= poly([(hip[0] - 4 - big // 2, hip[1] - 1), (hip[0] + 4 + big // 2, hip[1] - 1),
                       (hip[0] + 6 + hem + big // 2, hip[1] + skirt_l), (hip[0] - 6 - hem - (2 if strike and robe else 0) - big // 2, hip[1] + skirt_l)])
    f.part(torso, f.shade(torso, neck[0] - 1))
    if robe:                                                     # long folds down the robe
        for k in (-3, 1, 4):
            for y in range(int(hip[1]) + 2, GROUND - 3):
                x = int(hip[0]) + k + (y - int(hip[1])) * k // 12
                if (x, y) in torso and y % 3 != 0: c.px(x, y, 1 if f.enemy else 0)
    # armour / clothing detail carved into the torso
    if spec.get('armor'):
        for y in range(int(neck[1]) + 4, int(hip[1]), 3):
            for x in range(int(neck[0]) - sw, int(neck[0]) + sw + 1):
                if (x, y) in torso and (x, y - 1) in torso: c.px(x, y, 0 if not f.enemy else 1)
        for (x, y) in edge(torso): c.px(x, y, 1)
    belt_y = int(hip[1]) - 1
    for x in range(int(hip[0]) - 6 - big, int(hip[0]) + 7 + big):
        if (x, belt_y) in torso: c.px(x, belt_y, 0)
    if not f.enemy:   # collar V
        for k in range(3):
            c.px(int(neck[0]) - k, int(neck[1]) + 1 + k, 0); c.px(int(neck[0]) + k, int(neck[1]) + 1 + k, 0)

    # ---- head
    hd = ellipse(head[0], head[1], 3.6, 4.4)
    nose = {(int(head[0]) + 4, int(head[1])), (int(head[0]) + 4, int(head[1]) + 1)}
    hd |= nose
    f.part(hd, skin=True)
    c.px(int(head[0]) + 2, int(head[1]) - 1, 0)                       # eye
    c.px(int(head[0]) + 2, int(head[1]) + 2, 0); c.px(int(head[0]) + 3, int(head[1]) + 2, 0)   # mouth line
    hair = spec.get('hair', 'short')
    hx, hy = int(head[0]), int(head[1])
    if hair == 'helm' or hair == 'crest':
        m = {(x, y) for (x, y) in ellipse(head[0], head[1] - 1, 4.4, 4.6) if y <= hy - 1}
        m |= {(x, y) for (x, y) in ellipse(head[0], head[1], 4.4, 4.8) if x <= hx - 1}
        halo(c, m, 1); fill(c, m)
        if f.enemy:
            for (x, y) in m:
                if x % 2 == 0 and y > hy - 4: c.px(x, y, 0)   # riveted plates
        for x in range(hx - 4, hx + 5): c.px(x, hy - 1, 0)          # visor slit
        c.px(hx + 2, hy - 1, 1)
        if hair == 'crest':
            for k in range(7):
                c.line(hx - 3 + k, hy - 6 - (3 - abs(k - 3)) // 1, hx - 3 + k, hy - 5, 1)
            for k in range(7): c.px(hx - 3 + k, hy - 10 + abs(k - 3) // 2 - 1, 0) if k % 2 else None
    elif hair == 'hood':
        m = {(x, y) for (x, y) in ellipse(head[0] - 0.5, head[1] - 0.5, 4.6, 5.2) if x <= hx + 1 or y <= hy - 3}
        m -= {(x, y) for (x, y) in ellipse(head[0] + 1.5, head[1] + 0.5, 2.6, 3.2)}
        m |= poly([(hx - 4, hy), (hx - 8, hy + 6), (hx - 2, hy + 4)])
        f.part(m)
    elif hair == 'bald':
        c.px(hx - 1, hy - 3, 0); c.px(hx, hy - 4, 0)
    elif hair == 'long':                                        # falls down the back to the shoulders
        m = {(x, y) for (x, y) in ellipse(head[0], head[1] - 1, 4.0, 4.4) if y <= hy - 2 or x <= hx - 2}
        m |= poly([(hx - 4, hy - 2), (hx - 1, hy - 1), (hx - 2, hy + 8), (hx - 7, hy + 9)])
        halo(c, m - hd, 1)
        fill(c, m, (lambda x, y: (x * 2 + y) % 5 != 0) if not spec.get('dark') else (lambda x, y: (x * 2 + y) % 5 == 0))
        for (x, y) in edge(m):
            if (x, y) not in hd or y < hy - 2: c.px(x, y, 1)
        for y in range(hy, hy + 8, 2): c.px(hx - 4, y, 0)                 # a strand carved down it
    else:
        m = {(x, y) for (x, y) in ellipse(head[0], head[1] - 1, 4.0, 4.4) if y <= hy - 2 or x <= hx - 2}
        if hair == 'braid':
            for k in range(9): m |= {(hx - 4 - (k % 2), hy + k), (hx - 5 - (k % 2), hy + k)}
        if hair == 'wild':
            for k in range(-3, 4, 2): m |= {(hx + k, hy - 6), (hx + k - 1, hy - 7)}
            m |= {(hx - 5, hy - 3), (hx - 6, hy - 1)}
        dark = spec.get('dark', False)
        fill(c, m, (lambda x, y: (x * 2 + y) % 5 == 0) if dark else (lambda x, y: (x * 2 + y) % 5 != 0))
        for (x, y) in edge(m):
            if (x, y) not in hd or y < hy - 2: c.px(x, y, 1)
    if spec.get('crown'):                                       # a spiked band over the brow
        band = rectm(hx - 3, hy - 6, 8, 2)
        for k in (-3, 0, 3): band |= {(hx + 1 + k, hy - 7), (hx + 1 + k, hy - 8)}
        halo(c, band, 1); fill(c, band)
        c.px(hx + 1, hy - 5, 0)                                 # a stone in the band
    if spec.get('beard'):
        for y in range(hy + 2, hy + 6):
            for x in range(hx - 1, hx + 4 - (y - hy - 2) // 2):
                c.px(x, y, 0 if x % 2 == 0 and y > hy + 2 else 1)
        c.px(hx + 2, hy + 2, 0)
    if spec.get('scar'):
        c.px(hx + 1, hy - 2, 0); c.px(hx, hy - 1, 0); c.px(hx - 1, hy, 0)

    # ---- far-hand gear in front of torso (shield)
    if spec.get('shield'):
        kind = spec['shield']
        if kind == 'round':
            m = ellipse(far_hand[0] + 2, far_hand[1], 4.8, 5.4)
            f.part(m, lambda x, y: True)
            ring = edge(ellipse(far_hand[0] + 2, far_hand[1], 3.2, 3.8))
            for p in ring: c.px(*p, 0)
            c.px(int(far_hand[0]) + 2, int(far_hand[1]), 0)
        elif kind == 'tower':
            m = rectm(int(far_hand[0]) - 2, int(far_hand[1]) - 9, 9, 17)
            m |= poly([(far_hand[0] - 2, far_hand[1] + 7), (far_hand[0] + 6, far_hand[1] + 7), (far_hand[0] + 2, far_hand[1] + 11)])
            f.part(m, lambda x, y: True)
            for p in edge(rectm(int(far_hand[0]), int(far_hand[1]) - 7, 5, 13)): c.px(*p, 0)
            c.px(int(far_hand[0]) + 2, int(far_hand[1]), 0)
        elif kind == 'book':                                    # an open book held at the chest
            m = rectm(int(far_hand[0]) - 1, int(far_hand[1]) - 3, 8, 5)
            f.part(m, lambda x, y: True)
            for y in range(int(far_hand[1]) - 2, int(far_hand[1]) + 2): c.px(int(far_hand[0]) + 3, y, 0)   # spine
            for x in (int(far_hand[0]) + 1, int(far_hand[0]) + 5): c.px(x, int(far_hand[1]) - 1, 0)      # lines of text
        elif kind == 'kite':
            m = poly([(far_hand[0] - 2, far_hand[1] - 7), (far_hand[0] + 6, far_hand[1] - 7), (far_hand[0] + 6, far_hand[1] + 2),
                      (far_hand[0] + 2, far_hand[1] + 9), (far_hand[0] - 2, far_hand[1] + 2)])
            f.part(m, lambda x, y: True)
            for y in range(int(far_hand[1]) - 5, int(far_hand[1]) + 6): c.px(int(far_hand[0]) + 2, y, 0)
            for x in range(int(far_hand[0]), int(far_hand[0]) + 5): c.px(x, int(far_hand[1]) - 2, 0)

    # ---- near arm + weapon
    sh = (neck[0] + 1, neck[1] + 2)
    hand, elbow, weapon = spec['near'](pose, sh)
    if spec.get('bow_draw') and strike:
        pass
    m = thick(*sh, *elbow, 1.6 + big * 0.2) | thick(*elbow, *hand, 1.4 + big * 0.2)
    f.part(m, f.shade(m, sh[0]) if not spec.get('bare') else (lambda x, y: True), skin=spec.get('bare', False))
    weapon(f, hand)
    return hip


# ------------------------------------------------------------- weapons
def w_sword(ang, L=13):
    def draw(f, hand):
        tip = polar(*hand, ang, L)
        guard_a = polar(*hand, ang + 90, 3); guard_b = polar(*hand, ang - 90, 3)
        m = thick(*hand, *tip, 0.9)
        halo(f.c, m, 1); fill(f.c, m)
        f.c.line(*guard_a, *guard_b, 1)
        pommel = polar(*hand, ang + 180, 2); f.c.px(round(pommel[0]), round(pommel[1]))
    return draw


def w_lance(ang, L=30, back=8, pennant=False):
    def draw(f, hand):
        tip = polar(*hand, ang, L); butt = polar(*hand, ang + 180, back)
        m = thick(*butt, *tip, 0.6)
        halo(f.c, m, 1); fill(f.c, m)
        head = poly([polar(*tip, ang, 4), polar(*tip, ang + 90, 2), polar(*tip, ang - 90, 2)])
        halo(f.c, head, 1); fill(f.c, head)
        if pennant:
            base = polar(*hand, ang, L - 7)
            p = poly([base, polar(*base, ang + 70, 5), polar(*base, ang + 10, 7)])
            fill(f.c, p, lambda x, y: (x + y) % 2 == 0)
    return draw


def w_axe(ang, L=13):
    def draw(f, hand):
        top = polar(*hand, ang, L); butt = polar(*hand, ang + 180, 3)
        m = thick(*butt, *top, 0.7)
        halo(f.c, m, 1); fill(f.c, m)
        # bearded blade hanging off the leading side, near the top of the haft
        a0 = polar(*top, ang + 180, 1); a1 = polar(*top, ang + 180, 6)
        lead = ang - 90
        blade = poly([a0, polar(*a0, lead, 3), polar(*polar(*a0, lead, 7), ang, 2),
                      polar(*polar(*a1, lead, 7), ang, -2), polar(*a1, lead, 3), a1])
        halo(f.c, blade, 1); fill(f.c, blade)
        # carved line separating blade from haft, and the bevel of the edge
        f.c.line(*polar(*a0, lead, 2), *polar(*a1, lead, 2), 0)
    return draw


def w_bow(drawn):
    def draw(f, hand):
        # bow in the FAR hand (passed in), string to the near hand
        x, y = hand
        arc = set()
        for k in range(-10, 11):
            ax = x + 3 - (k * k) / 26.0
            arc |= thick(ax, y + k, ax, y + k, 0.6)
        halo(f.c, arc, 1); fill(f.c, arc)
        sx = x - (4 if drawn else 0)
        f.c.line(x - 1, y - 10, sx - (6 if drawn else 1), y, 1)
        f.c.line(x - 1, y + 10, sx - (6 if drawn else 1), y, 1)
        if drawn:
            f.c.line(sx - 6, y, x + 8, y, 1)                 # nocked arrow
            f.c.px(x + 8, y - 1); f.c.px(x + 8, y + 1); f.c.px(x + 9, y)
    return draw


def w_staff(ang, L=24):
    def draw(f, hand):
        top = polar(*hand, ang, L); butt = polar(*hand, ang + 180, 10)
        m = thick(*butt, *top, 0.6)
        halo(f.c, m, 1); fill(f.c, m)
        bell = ellipse(top[0], top[1] + 2, 2.6, 2.6) | rectm(int(top[0]) - 3, int(top[1]) + 3, 7, 2)
        halo(f.c, bell, 1); fill(f.c, bell)
        f.c.px(round(top[0]), round(top[1]) + 6)                # clapper
    return draw


def near_staff(pose, sh):
    if pose == 'strike':                                        # raises the staff to ring it
        hand = (sh[0] + 6, sh[1] - 4); return hand, (sh[0] + 4, sh[1] + 2), w_staff(80, 18)
    hand = (sh[0] + 5, sh[1] + 8); return hand, (sh[0] + 1, sh[1] + 6), w_staff(88, 26)


def w_spark(big):
    """a casting hand: a carved star of light at the palm"""
    def draw(f, hand):
        x, y = round(hand[0]) + 2, round(hand[1]) - 1
        r = 4 if big else 2
        m = set()
        for d in range(-r, r + 1): m |= {(x + d, y), (x, y + d)}
        if big:
            for d in range(-2, 3): m |= {(x + d, y + d), (x + d, y - d)}
        halo(f.c, m, 1); fill(f.c, m)
    return draw


def near_cast(pose, sh):
    if pose == 'strike':                                        # the arm thrown forward, palm out
        hand = (sh[0] + 12, sh[1] - 2); return hand, (sh[0] + 6, sh[1]), w_spark(True)
    hand = (sh[0] + 5, sh[1] + 8); return hand, (sh[0] + 1, sh[1] + 6), w_spark(False)


def far_book(pose, sh):
    return (sh[0] + 5, sh[1] + 6) if pose == 'idle' else (sh[0] + 2, sh[1] + 8)


# ------------------------------------------------------------- the cast
def far_shield(pose, sh):
    return (sh[0] + (6 if pose == 'strike' else 4), sh[1] + 7)


def far_low(pose, sh):
    return (sh[0] + 3, sh[1] + 10)


def near_sword(pose, sh):
    if pose == 'strike':
        hand = (sh[0] + 11, sh[1] - 1); return hand, (sh[0] + 5, sh[1] + 1), w_sword(8, 14)
    hand = (sh[0] + 5, sh[1] + 9); return hand, (sh[0] + 1, sh[1] + 6), w_sword(70, 13)


def near_lance(pose, sh):
    if pose == 'strike':
        hand = (sh[0] + 9, sh[1] + 3); return hand, (sh[0] + 4, sh[1] + 5), w_lance(2, 26, 8)
    hand = (sh[0] + 5, sh[1] + 8); return hand, (sh[0] + 1, sh[1] + 6), w_lance(78, 24, 14)


def near_axe(pose, sh):
    if pose == 'strike':
        hand = (sh[0] + 10, sh[1] + 5); return hand, (sh[0] + 6, sh[1] + 1), w_axe(-25, 13)
    hand = (sh[0] + 5, sh[1] + 9); return hand, (sh[0] + 1, sh[1] + 6), w_axe(60, 13)


def near_bowstring(pose, sh):
    if pose == 'strike':
        hand = (sh[0] - 3, sh[1] + 1); return hand, (sh[0] - 4, sh[1] + 4), (lambda f, h: None)
    hand = (sh[0] + 3, sh[1] + 7); return hand, (sh[0], sh[1] + 5), (lambda f, h: None)


def far_bow(pose, sh):
    return (sh[0] + 13, sh[1] + 1) if pose == 'strike' else (sh[0] + 8, sh[1] + 7)


CAST = {
    # heroes
    'idra':   dict(hair='braid', scar=True, shield='round', far_hand=far_shield, near=near_lance),
    'hale':   dict(hair='bald', beard=True, build=2, bare=True, far_hand=lambda p, s: (s[0] + 4, s[1] + 9) if p == 'idle' else (s[0] + 8, s[1] + 4),
                   near=near_axe, lean_strike=4),
    'wren':   dict(hair='hood', far_hand=far_bow, near=near_bowstring, bow=True),
    'maren':  dict(hair='hood', far_hand=lambda p, s: (s[0] + 3, s[1] + 9), near=near_staff),
    # enemies (drawn facing right, mirrored later)
    'raider': dict(hair='wild', shield='round', far_hand=far_shield, near=near_axe),
    'bowman': dict(hair='hood', far_hand=far_bow, near=near_bowstring, bow=True),
    'spear':  dict(hair='helm', shield='kite', far_hand=far_shield, near=near_lance),
    'osk':    dict(hair='crest', armor=True, build=2, shield='tower', far_hand=far_shield, near=near_lance, lean_strike=2),
    'cass':   dict(hair='wild', scar=True, far_hand=lambda p, s: (s[0] + 2, s[1] + 10) if p == 'idle' else (s[0] - 3, s[1] + 6),
                   near=near_sword, lean_strike=5),
    'gerault': dict(hair='helm', armor=True, beard=True, build=2, shield='kite', far_hand=far_shield, near=near_sword, lean_strike=2),
    'corvane': dict(hair='short', dark=True, cape=True, far_hand=lambda p, s: (s[0] + 3, s[1] + 9), near=near_sword),
    # Act III
    'aelis':   dict(hair='long', robe=True, shield='book', far_hand=far_book, near=near_cast, lean_strike=2),
    'cultist': dict(hair='hood', robe=True, far_hand=far_low, near=near_cast, lean_strike=2),
    'silt':    dict(hair='hood', robe=True, cape=True, build=1, shield='book', far_hand=far_book, near=near_cast, lean_strike=1),
    # Act IV
    'rook':    dict(hair='wild', beard=True, scar=True, build=1, far_hand=lambda p, s: (s[0] + 3, s[1] + 9) if p == 'idle' else (s[0] + 7, s[1] + 5),
                    near=near_axe, lean_strike=4),
    'marl':    dict(hair='bald', beard=True, cape=True, build=2, far_hand=lambda p, s: (s[0] + 2, s[1] + 10), near=near_sword, lean_strike=3),
    'saoirse': dict(hair='braid', scar=True, far_hand=lambda p, s: (s[0] + 5, s[1] + 5), near=near_lance),
    'thane':   dict(hair='bald', beard=True, robe=True, far_hand=lambda p, s: (s[0] + 3, s[1] + 9), near=near_staff),
    'hesk':    dict(hair='wild', beard=True, scar=True, build=2, bare=True, far_hand=lambda p, s: (s[0] + 4, s[1] + 9) if p == 'idle' else (s[0] + 8, s[1] + 4),
                    near=near_axe, lean_strike=5),
    'oda':     dict(hair='bald', beard=True, build=2, robe=True, far_hand=lambda p, s: (s[0] + 4, s[1] + 9) if p == 'idle' else (s[0] + 8, s[1] + 4),
                    near=near_axe, lean_strike=4),                       # the bellwright: an apron, a smith's hammer
    'aldric':  dict(hair='short', dark=True, crown=True, beard=True, armor=True, cape=True, build=1, shield='kite',
                    far_hand=far_shield, near=near_lance, lean_strike=2),
}
RIDERS = {
    'oren':  dict(hair='short', far_hand=lambda p, s: (s[0] + 5, s[1] + 5), near=near_lance),
    'rider': dict(hair='helm', far_hand=lambda p, s: (s[0] + 5, s[1] + 5), near=near_lance),
}


def draw_person(name, pose, enemy):
    spec = CAST[name]
    f = Fig(enemy)
    person(f, spec, pose)
    if spec.get('bow'):
        sh_far = None
    if spec.get('bow'):
        # the bow is held in the far hand; recompute where that hand is
        hip_x = 22 + (spec.get('lean_strike', 3) if pose == 'strike' else 0)
        neck = (hip_x + (spec.get('lean_strike', 3) if pose == 'strike' else 0) + 1, GROUND - 17 - 14)
        hand = far_bow(pose, (neck[0] - 2, neck[1] + 2))
        w_bow(pose == 'strike')(f, hand)
    return f.c


# ------------------------------------------------------------- horse + rider
def draw_brannock(pose):
    return draw_rider(dict(hair='short', dark=True, beard=True, cape=True, far_hand=lambda p, s: (s[0] + 5, s[1] + 5),
                           near=near_sword), pose, False)


def draw_rider(spec, pose, enemy):
    """A horse and rider facing right (enemies are mirrored by the caller)."""
    f = Fig(enemy); c = f.c
    gallop = pose == 'strike'
    bx, by = 26 + (3 if gallop else 0), 30          # body centre
    # legs (far pair first, dithered)
    if gallop:
        legs = [((bx - 9, by + 3), (bx - 17, by + 12), (bx - 19, by + 15)), ((bx + 8, by + 3), (bx + 16, by + 7), (bx + 19, by + 5)),
                ((bx - 7, by + 4), (bx - 12, by + 12), (bx - 15, GROUND)), ((bx + 9, by + 4), (bx + 14, by + 10), (bx + 13, by + 15))]
    else:
        legs = [((bx - 10, by + 3), (bx - 11, by + 10), (bx - 10, GROUND)), ((bx + 8, by + 3), (bx + 9, by + 10), (bx + 9, GROUND)),
                ((bx - 7, by + 4), (bx - 6, by + 10), (bx - 7, GROUND)), ((bx + 10, by + 4), (bx + 12, by + 10), (bx + 12, GROUND))]
    for i, (a, k, ft) in enumerate(legs):
        m = thick(*a, *k, 1.6) | thick(*k, *ft, 1.1) | rectm(int(ft[0]) - 1, int(ft[1]) - 1, 3, 2)
        f.part(m, far=i < 2)
    # tail
    tail = set()
    for k in range(10):
        tx = bx - 14 - k * (0.9 if gallop else 0.5); ty = by - 1 + k * (0.5 if gallop else 1.2)
        tail |= thick(tx, ty, tx - 1, ty + 1, 0.9)
    f.part(tail)
    # body, neck, head
    body = ellipse(bx, by, 14, 6.5)
    neck = poly([(bx + 8, by - 4), (bx + 13, by - 14), (bx + 18, by - 13), (bx + 16, by + 1)])
    head = poly([(bx + 13, by - 15), (bx + 17, by - 18), (bx + 25, by - 11), (bx + 24, by - 8), (bx + 18, by - 9), (bx + 15, by - 10)])
    horse = body | neck | head
    halo(c, horse, 1)
    if enemy:                                      # a dark horse, engraved, lit along its back
        fill(c, horse, lambda x, y: (x + y) % 4 == 0)
        for p in edge(horse): c.px(*p, 1)
    else:
        fill(c, horse, lambda x, y: (x - y) % 3 != 0 if x < bx - 4 else True)
    # caparison: heraldic hatched cloth over the body
    cap = {(x, y) for (x, y) in body if y >= by - 2 and bx - 11 <= x <= bx + 9}
    fill(c, cap, lambda x, y: (x - y) % 4 != 0 or y < by)
    for x in range(bx - 11, bx + 10): c.px(x, by - 3, 0)
    for x in range(bx - 10, bx + 9, 3): c.px(x, by + 5, 0)
    # mane, eye, nostril, ear, bridle
    for k in range(6): c.px(bx + 12 + k // 2, by - 13 + k * 2 - k // 2, 0)
    c.px(bx + 19, by - 14, 0); c.px(bx + 24, by - 10, 0)
    c.px(bx + 15, by - 17); c.px(bx + 16, by - 18)
    c.line(bx + 17, by - 12, bx + 21, by - 10, 0)
    # rider
    person(f, spec, pose, ox=bx - 1, oy=by - 6, mounted=True)
    return c


# ------------------------------------------------------------- sea-eagle + rider
def draw_eagle_rider(spec, pose, enemy):
    """Saoirse on her sea-eagle, facing right: wings raised behind her, talons down."""
    f = Fig(enemy); c = f.c
    strike = pose == 'strike'
    bx, by = 25 + (3 if strike else 0), 29
    lines = (lambda x, y: (x + 2 * y) % 5 != 0) if not enemy else (lambda x, y: (x + y) % 4 == 0 or (x * 3 + y) % 7 == 0)
    # far wing: a great raised fan of primaries behind everything
    if strike:
        wing = poly([(bx - 2, by - 3), (bx - 4, by - 18), (bx - 1, by - 29), (bx - 9, by - 27), (bx - 17, by - 24),
                     (bx - 25, by - 17), (bx - 21, by - 14), (bx - 26, by - 9), (bx - 12, by - 2)])
    else:
        wing = poly([(bx - 3, by - 3), (bx - 8, by - 15), (bx - 13, by - 24), (bx - 19, by - 22), (bx - 26, by - 18),
                     (bx - 30, by - 11), (bx - 24, by - 9), (bx - 27, by - 4), (bx - 12, by)])
    f.part(wing, lines)
    for k in range(5):                               # primary feathers: carved quills fanning out
        a = (110 + k * 16) if strike else (125 + k * 13)
        x0, y0 = bx - 6, by - 4
        x1, y1 = polar(x0, y0, a, 19 + (k % 2) * 3)
        c.line(int(x0), int(y0), int(x1), int(y1), 0 if not enemy else 1, dash=lambda i: i % 4 != 3)
    # tail fan
    f.part(poly([(bx - 9, by - 2), (bx - 21, by + 1), (bx - 22, by + 5), (bx - 10, by + 5)]), lines)
    # legs and talons
    for lx in (bx - 3, bx + 3):
        m = thick(lx, by + 4, lx + 1, GROUND - 3, 1.3)
        m |= thick(lx + 1, GROUND - 3, lx + 4, GROUND - 1, 0.7) | thick(lx + 1, GROUND - 3, lx - 2, GROUND - 1, 0.7)
        f.part(m, far=lx < bx)
    # body, a short thick neck, a heavy head with a hooked beak
    body = ellipse(bx, by, 12, 6.5)
    neck = poly([(bx + 7, by - 4), (bx + 12, by - 8), (bx + 17, by - 7), (bx + 13, by + 2)])
    hd = ellipse(bx + 16, by - 8, 4.6, 3.6)
    beak = poly([(bx + 19, by - 10), (bx + 25, by - 8), (bx + 25, by - 5), (bx + 23, by - 4), (bx + 22, by - 6), (bx + 19, by - 6)])
    bird = body | neck | hd | beak
    halo(c, bird, 1)
    if enemy:
        fill(c, bird, lambda x, y: (x + y) % 4 == 0)
        for q in edge(bird): c.px(*q, 1)
    else:
        fill(c, bird, lambda x, y: not (x < bx - 2 and (x - y) % 3 == 0))
    fill(c, hd | beak, lambda x, y: 1)                              # a sea-eagle's pale head
    for q in edge(hd | beak):
        if q[1] > by - 6 or q[0] < bx + 13: c.px(*q, 0)
    c.px(bx + 17, by - 9, 0); c.px(bx + 18, by - 9, 0)            # fierce eye
    c.line(bx + 20, by - 7, bx + 23, by - 7, 0)                    # beak gape
    for k in range(4): c.px(bx - 6 + k * 4, by + 2 + (k % 2), 0 if not enemy else 1)   # breast feathers
    # near wing folded along the flank
    nw = poly([(bx + 4, by - 3), (bx - 8, by - 2), (bx - 16, by + 3), (bx - 4, by + 4)])
    f.part(nw, lines)
    # the rider
    person(f, spec, pose, ox=bx - 4, oy=by - 6, mounted=True)
    return c


def all_frames():
    """Returns ordered list of (key, canvas) — order is the FX frame index."""
    out = []
    out.append(('brannock_idle', draw_brannock('idle')))
    out.append(('brannock_strike', draw_brannock('strike')))
    for name in ('idra', 'hale', 'wren'):
        for pose in ('idle', 'strike'):
            out.append((f'{name}_{pose}', draw_person(name, pose, False)))
    for name in ('raider', 'bowman', 'spear', 'osk'):
        for pose in ('idle', 'strike'):
            r = draw_person(name, pose, True)
            m = C(W, H); m.paste(r, 0, 0, mirror=True)
            out.append((f'{name}_{pose}', m))
    for pose in ('idle', 'strike'):                     # figure 8: Maren (joined later)
        out.append((f'maren_{pose}', draw_person('maren', pose, False)))
    return out


def act3_frames():
    """Figures 17-20: Aelis (a hero), then the cultist, Mother Silt and King Aldric (enemies, facing left)."""
    out = [(f'aelis_{p}', draw_person('aelis', p, False)) for p in ('idle', 'strike')]
    for name in ('cultist', 'silt', 'aldric'):
        for pose in ('idle', 'strike'):
            m = C(W, H); m.paste(draw_person(name, pose, True), 0, 0, mirror=True)
            out.append((f'{name}_{pose}', m))
    return out


def ship_frames():
    """Figures 23-25: Rook (and mirrored, for when he's fought first), Black Marl (enemy)."""
    def mirrored(c):
        m = C(W, H); m.paste(c, 0, 0, mirror=True); return m
    out = [(f'rook_{p}', draw_person('rook', p, False)) for p in ('idle', 'strike')]
    out += [(f'rook_foe_{p}', mirrored(draw_person('rook', p, False))) for p in ('idle', 'strike')]
    out += [(f'marl_{p}', mirrored(draw_person('marl', p, True))) for p in ('idle', 'strike')]
    return out


def later_frames():
    """Figures 10+ (figure 9 is the Drowned, from demons.py)."""
    out = []
    def mirrored(c):
        m = C(W, H); m.paste(c, 0, 0, mirror=True); return m
    # recruitables fought first: their own figure, then the same mirrored (facing left, enemy side)
    for name, fn in (('oren', lambda p: draw_rider(RIDERS['oren'], p, False)),              # 10, 11: Oren
                     ('cass', lambda p: draw_person('cass', p, False))):                    # 12, 13: Cass
        for pose in ('idle', 'strike'): out.append((f'{name}_{pose}', fn(pose)))
        for pose in ('idle', 'strike'): out.append((f'{name}_foe_{pose}', mirrored(fn(pose))))
    for name, fn in (('rider', lambda p: draw_rider(RIDERS['rider'], p, True)),          # 14: Dain rider
                     ('gerault', lambda p: draw_person('gerault', p, True)),            # 15: Gerault
                     ('corvane', lambda p: draw_person('corvane', p, True))):           # 16: Corvane
        for pose in ('idle', 'strike'):
            m = C(W, H); m.paste(fn(pose), 0, 0, mirror=True)
            out.append((f'{name}_{pose}', m))
    return out


def island_frames():
    """Figures 26-31: Saoirse (+ mirrored: fought first), Thane, Hesk (+ mirrored), Lir (a Drowned on our side)."""
    import demons
    def mirrored(c):
        m = C(W, H); m.paste(c, 0, 0, mirror=True); return m
    sa = lambda p: draw_eagle_rider(CAST['saoirse'], p, False)
    out = [(f'saoirse_{p}', sa(p)) for p in ('idle', 'strike')]
    out += [(f'saoirse_foe_{p}', mirrored(sa(p))) for p in ('idle', 'strike')]
    out += [(f'thane_{p}', draw_person('thane', p, False)) for p in ('idle', 'strike')]
    out += [(f'hesk_{p}', draw_person('hesk', p, False)) for p in ('idle', 'strike')]
    out += [(f'hesk_foe_{p}', mirrored(draw_person('hesk', p, False))) for p in ('idle', 'strike')]
    out += [(f'lir_{p}', demons.drowned(p)) for p in ('idle', 'strike')]    # drawn facing right already
    return out


def finale_frames():
    """Figures 32-33: Oda (a hero, the last bellwright), Nhal (what waits under the cathedral)."""
    import demons
    out = [(f'oda_{p}', draw_person('oda', p, False)) for p in ('idle', 'strike')]
    for p in ('idle', 'strike'):
        m = C(W, H); m.paste(demons.nhal(p), 0, 0, mirror=True); out.append((f'nhal_{p}', m))
    return out
