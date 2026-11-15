"""Portrait style study: three candidate styles for 56x64 dialogue portraits.

  clean  -- front view, flat white face, crisp 1px features, solid hair/beard masses
  notan  -- front view, bold two-tone light/shadow shapes (no texture), on a lit disc
  cameo  -- profile silhouette carved into a coin/medallion (heraldic)

Each character is a spec dict; every style reads the same spec.
"""
import math
from wood import C, ellipse, poly, rectm, thick, halo, fill, edge

W, H = 56, 64
CAST = {
    'brannock': dict(hair='short', dark=True, beard=True, armor=True),
    'idra':     dict(hair='braid', scar=True),
    'osk':      dict(helm=True, crest=True, armor=True, frown=True, build=2),
    'hale':     dict(hair='bald', beard='full', build=3),
    'wren':     dict(hair='hood'),
    'maren':    dict(hair='veil'),                                  # the mill priestess
    'tam':      dict(hair='cap', beard=True, frown=True),           # old miller
    'goody':    dict(hair='kerchief'),                              # village wife
    'varga':    dict(hair='bald', beard='full', frown=True, scar=True, build=2),   # raider captain
    'kell':     dict(hair='short', dark=True, beard='full', frown=True, build=3),  # bandit chief of the palisade
    'voss':     dict(hair='hood', frown=True),                      # the paymaster in grey
    'fenn':     dict(hair='short', beard=True),                     # trapper outside the palisade
    'oren':     dict(hair='short', scar=True, build=1),             # Dain officer who changes sides
    'cass':     dict(hair='braid', dark=True, frown=True),          # sellsword
    'corvane':  dict(hair='short', dark=True, beard=True, frown=True),   # Dain's spymaster
    'gerault':  dict(helm=True, beard='full', armor=True, build=2),  # Dain's marshal
    'aelis':    dict(hair='long', circlet=True),                    # princess of Dain
    'silt':     dict(hair='veil', frown=True, build=1),             # high priestess of the Undertow
    'aldric':   dict(hair='short', dark=True, beard='full', crown=True, armor=True, build=1),   # the king, as he looks
    'hollow':   dict(hair='short', dark=True, beard='full', crown=True, armor=True, build=1, hollow=True),   # ...and what wears him
    'prisoner': dict(hair='short', beard=True, frown=True),         # in the river-gate cells
    'rook':     dict(hair='short', dark=True, beard=True, scar=True, build=1),   # smuggler captain
    'marl':     dict(hair='cap', beard='full', frown=True, scar=True, build=2),  # pirate captain
    'dockmaster': dict(hair='bald', beard=True, frown=True, build=2),            # the cult's man on the docks
    'islander': dict(hair='kerchief'),                               # hiding in the dunes
    'saoirse':  dict(hair='braid', scar=True),                       # sea-eagle rider of Vael
    'thane':    dict(hair='bald', beard=True),                       # the cliff village's monk
    'hesk':     dict(hair='long', dark=True, beard='full', frown=True, scar=True, build=3),   # the man on the salt flats
    'lir':      dict(hair='hood', frown=True, scar=True),         # Old Tam's grandson, one of the Drowned
    'brigid':   dict(hair='veil', scar=True),                        # the island's rebel leader
    'vey':      dict(helm=True, frown=True, armor=True, build=1),    # captain of the cult's garrison
    'gatekeeper': dict(hair='veil', frown=True, crown=True, hollow=True, build=2),   # what guards the first gate
    'oda':      dict(hair='cap', beard='full', scar=True, build=2),   # the last bellwright: a smith's cap, burn-scarred
    'nhal':     dict(hair='long', dark=True, crown=True, hollow=True, build=3),     # what pulls from under the cathedral
}


# ------------------------------------------------------------ front-view shapes
def front_masks(spec, cx=27, cy=26):
    head = ellipse(cx, cy, 10, 13)
    neck = rectm(cx - 5, cy + 9, 11, 10)
    b = spec.get('build', 0)
    body = poly([(-1 - b, 64), (3 - b, 50), (14, 44), (40, 44), (51 + b, 50), (57 + b, 64)])
    hair = set()
    if spec.get('hair') == 'short':
        cap = ellipse(cx, cy - 3, 11.5, 13)
        hair = {(x, y) for (x, y) in cap if y < cy - 6 + (1 if (x // 3) % 2 else 0) or (y < cy + 1 and abs(x - cx) > 9)}
    elif spec.get('hair') == 'braid':
        cap = ellipse(cx, cy - 3, 11.5, 13.5)
        hair = {(x, y) for (x, y) in cap if y < cy - 7 + abs(x - cx) // 4 or (abs(x - cx) > 9 and y < cy + 6)}
        for k in range(8):                                     # braid over the shoulder
            hair |= ellipse(cx + 12 + (k % 2), cy + 7 + k * 3, 2.2, 1.8)
    elif spec.get('hair') == 'hood':
        outer = ellipse(cx, cy - 2, 13.5, 16) | poly([(cx - 13, cy + 6), (cx + 13, cy + 6), (cx + 17, cy + 20), (cx - 17, cy + 20)])
        opening = {(x, y) for (x, y) in ellipse(cx, cy + 2, 8.5, 12) if y > cy - 10}
        hair = outer - opening
    elif spec.get('hair') == 'veil':                           # hood with a long fall of cloth
        outer = ellipse(cx, cy - 3, 13, 15.5) | poly([(cx - 13, cy - 2), (cx + 13, cy - 2), (cx + 19, 64), (cx - 19, 64)])
        opening = {(x, y) for (x, y) in ellipse(cx, cy + 1, 8.5, 12.5) if y > cy - 11}
        hair = outer - opening
    elif spec.get('hair') == 'long':                           # parted, falling past the shoulders
        cap = ellipse(cx, cy - 3, 12, 13.5)
        hair = {(x, y) for (x, y) in cap if y < cy - 7 + abs(x - cx) // 5 or (abs(x - cx) > 9)}
        hair |= poly([(cx - 12, cy - 4), (cx - 8, cy), (cx - 7, cy + 18), (cx - 15, cy + 20)])
        hair |= poly([(cx + 12, cy - 4), (cx + 8, cy), (cx + 7, cy + 18), (cx + 15, cy + 20)])
        hair -= rectm(cx, cy - 16, 1, 8)                       # the parting
    elif spec.get('hair') == 'cap':                            # flat cap with a brim
        hair = {(x, y) for (x, y) in ellipse(cx, cy - 8, 12, 6.5) if y <= cy - 7} | rectm(cx - 12, cy - 8, 25, 2)
    elif spec.get('hair') == 'kerchief':                       # cloth knotted under the chin
        outer = ellipse(cx, cy - 2, 12.5, 15)
        hair = {(x, y) for (x, y) in outer if y < cy - 6 or abs(x - cx) > 8} | poly([(cx - 3, cy + 12), (cx + 3, cy + 12), (cx, cy + 17)])
    if spec.get('hair') == 'bald':
        head = head | ellipse(cx - 10, cy + 1, 2, 3) | ellipse(cx + 10, cy + 1, 2, 3)     # ears
    helm = set()
    if spec.get('helm'):
        dome = ellipse(cx, cy - 3, 12, 14)
        helm = {(x, y) for (x, y) in dome if y <= cy - 4 or abs(x - cx) >= 8 and y <= cy + 7}
        helm |= rectm(cx - 1, cy - 4, 3, 10)                  # nasal guard
        if spec.get('crest'):
            for k in range(-5, 6):
                helm |= thick(cx + k, cy - 16, cx + k * 1.2, cy - 23 + abs(k) // 2, 0.7)
    beard = set()
    if spec.get('beard'):
        big = spec.get('beard') == 'full'
        beard = {(x, y) for (x, y) in ellipse(cx, cy + 6, 9.5, 9) if y >= cy + 3 and (x, y) in head | ellipse(cx, cy + 9, 8, 7)}
        if big: beard |= {(x, y) for (x, y) in ellipse(cx, cy + 11, 10, 9) if y >= cy + 6}
        beard -= rectm(cx - 3, cy + 6, 7, 2)                  # the mouth opening
    if spec.get('crown'):                                      # a spiked crown worn as a helm-mass
        helm |= rectm(cx - 11, cy - 15, 23, 4)
        for k in (-9, -3, 3, 9): helm |= poly([(cx + k - 2, cy - 15), (cx + k + 2, cy - 15), (cx + k, cy - 21)])
    if spec.get('circlet'):                                    # a thin band across the brow
        helm |= {(x, y) for x in range(cx - 10, cx + 11) for y in (cy - 11, cy - 10)}
    return head, neck, body, hair, helm, beard


def features(c, spec, cx, cy, ink, skin_side=None):
    """eyes, brows, nose, mouth. ink = pixel value used for the lines."""
    by = cy - 3
    fr = 1 if spec.get('frown') else 0
    for x in range(cx - 7, cx - 2): c.px(x, by - (fr if x > cx - 5 else 0), ink)
    for x in range(cx + 2, cx + 7): c.px(x, by - (fr if x < cx + 4 else 0), ink)
    for ex in (cx - 5, cx + 3):
        c.rect(ex, by + 2, 3, 2, ink)
        c.px(ex + 1, by + 2, 1 - ink)                          # glint
    for y in range(cy, cy + 5): c.px(cx + 1, y, ink)          # nose ridge
    c.px(cx, cy + 5, ink); c.px(cx + 2, cy + 5, ink); c.px(cx - 1, cy + 4, ink)
    my = cy + 8
    for x in range(cx - 3, cx + 5): c.px(x, my, ink)
    if spec.get('frown'): c.px(cx - 4, my + 1, ink); c.px(cx + 5, my + 1, ink)
    else: c.px(cx - 4, my - 1, ink)                           # a small, wry lift
    if spec.get('scar'):
        for k in range(7): c.px(cx + 3 + k // 2, cy - 7 + k, ink)


def rays(c, cx, cy, step=15, r0=16, thin=False):
    for y in range(H):
        for x in range(W):
            a = (math.degrees(math.atan2(y - cy, x - cx)) + 360) % 360
            d = math.hypot(x - cx, y - cy)
            if d < r0: continue
            if thin:
                if int(a) % step == 0: c.px(x, y)
            elif int(a // step) % 2 == 0: c.px(x, y)


# ------------------------------------------------------------ style: clean line
def clean(spec):
    c = C(W, H); cx, cy = 27, 26
    head, neck, body, hair, helm, beard = front_masks(spec, cx, cy)
    for a in range(0, 360, 12):                               # a few long, clean rays
        t = math.radians(a)
        for r in range(17, 60):
            c.px(round(cx + r * math.cos(t)), round(cy + r * math.sin(t)))
    sil = head | neck | body | hair | helm
    halo(c, sil, 2)
    fill(c, body)
    for (x, y) in edge(body): c.px(x, y, 0) if y > 46 else None
    for k in range(6): c.px(cx - 6 + k, 45 + k, 0); c.px(cx + 6 - k, 45 + k, 0)       # collar V
    if spec.get('armor'):
        for y in (53, 58):
            for x in range(W): c.px(x, y, 0) if (x, y) in body else None
        for x in range(0, W, 5):
            for y in range(53, 64): c.px(x, y, 0) if (x, y) in body and y > 53 else None
    fill(c, neck); c.line(cx - 5, cy + 12, cx - 5, cy + 18, 0)                         # neck shadow line
    fill(c, head)
    for (x, y) in edge(head):                                  # jaw line on the shadow side only
        if x > cx + 4 and y > cy: c.px(x, y, 0)
    if spec.get('hair') in ('hood', 'veil', 'kerchief', 'cap'):
        kind = spec.get('hair')
        halo(c, hair - head, 1)
        fill(c, hair)
        for (x, y) in edge(hair): c.px(x, y, 0)
        if kind in ('hood', 'veil'):
            for k in (-12, 12):                                 # two folds, on the cloth only
                y1 = cy + (34 if kind == 'veil' else 14)
                for t in range(0, 60):
                    x = round(cx + k * 0.5 + (k * 0.5) * t / 59); y = round(cy - 13 + (y1 - cy + 13) * t / 59)
                    if (x, y) in hair and (x, y) not in head and t % 4 != 3: c.px(x, y, 0)
        if kind == 'veil':                                      # a small bell pendant on the brow
            fill(c, ellipse(cx, cy - 13, 2.2, 2), lambda x, y: False)
            c.rect(cx - 1, cy - 14, 3, 2, 1); c.px(cx, cy - 11, 1)
        if kind == 'kerchief':
            for x in range(cx - 11, cx + 12, 3): c.px(x, cy - 9, 0)                    # stitched hem
        if kind == 'cap':
            for x in range(cx - 11, cx + 12): c.px(x, cy - 9, 0)
        hair = set()
    if spec.get('hair') == 'bald':
        c.px(cx - 10, cy + 1, 0); c.px(cx + 10, cy + 1, 0)
    if hair:
        dark = spec.get('dark')
        halo(c, hair - head, 1)
        fill(c, hair, (lambda x, y: False) if dark else (lambda x, y: True))
        for (x, y) in edge(hair): c.px(x, y, 1 if dark else 0) if (x, y) not in head or y < cy - 8 else None
        if not dark:
            for k in range(-2, 3):                             # a few strands, curving with the skull
                for t in range(9):
                    x = cx + k * 4 + round(math.sin(t / 3.0)); y = cy - 15 + t
                    if (x, y) in hair: c.px(x, y, 0)
        else:
            c.line(cx - 6, cy - 12, cx - 2, cy - 14, 1); c.line(cx + 2, cy - 14, cx + 6, cy - 12, 1)   # two highlights
    if beard:
        fill(c, beard, lambda x, y: x % 3 == 0 and y > cy + 9)
    if helm:
        halo(c, helm - head, 1)
        fill(c, helm)
        for (x, y) in edge(helm): c.px(x, y, 0)
        if spec.get('helm'):
            for x in range(cx - 11, cx + 12): c.px(x, cy - 5, 0); c.px(x, cy - 8, 0)       # brow band
            for x in range(cx - 9, cx + 10, 3): c.px(x, cy - 7, 0)                         # rivets
            c.line(cx, cy - 15, cx, cy - 9, 0)                                              # ridge
            c.rect(cx, cy - 4, 1, 9, 0)                                                      # nasal edge
        if spec.get('circlet'): c.px(cx, cy - 11, 0); c.px(cx, cy - 10, 0)              # its stone
    features(c, spec, cx, cy, 0)
    if beard: c.rect(cx - 3, cy + 7, 8, 1, 1); c.rect(cx - 3, cy + 8, 8, 1, 0)
    return c


# ------------------------------------------------------------ style: notan (light & shadow)
def notan(spec):
    c = C(W, H); cx, cy = 27, 26
    head, neck, body, hair, helm, beard = front_masks(spec, cx, cy)
    fill(c, ellipse(cx, cy + 2, 25, 27))                      # lit disc behind the head
    for (x, y) in edge(ellipse(cx, cy + 2, 25, 27)): c.px(x, y, 0)
    sil = head | neck | body | hair | helm
    halo(c, sil, 1)
    # light from the left: everything right of a wavering terminator is solid black
    def lit(x, y): return x < cx + 3 + int(2 * math.sin(y / 3.0))
    fill(c, body, lambda x, y: x < cx + 1 and not (x % 6 == 0 and y > 50))
    fill(c, neck, lambda x, y: x < cx - 1)
    fill(c, head, (lambda x, y: False) if spec.get('hollow') else lit)   # the hollow king: no face at all
    if hair:
        fill(c, hair, (lambda x, y: False) if spec.get('dark') else (lambda x, y: x < cx - 1 and (x + y // 2) % 4 != 0))
        for (x, y) in edge(hair):
            if x < cx: c.px(x, y, 1)                       # rim light on the lit side
    if beard: fill(c, beard, lambda x, y: x < cx - 2 and y % 3 == 0)
    if helm:
        fill(c, helm, lambda x, y: x < cx)
        for (x, y) in edge(helm):
            if x >= cx: c.px(x, y, 1)
        if spec.get('helm'):
            for x in range(cx - 11, cx + 12): c.px(x, cy - 5, 1 if x > cx + 2 else 0)
        if spec.get('crown'):
            for x in range(cx - 9, cx + 10, 4): c.px(x, cy - 13, 1 if x >= cx else 0)       # stones in the band
    # features: black ink on the lit side, white ink in the shadow
    tmp = C(W, H); features(tmp, spec, cx, cy, 1)
    for y in range(H):
        for x in range(W):
            if tmp.a[y][x] and tmp.p[y][x]:
                c.px(x, y, 1 if spec.get('hollow') else (0 if lit(x, y) else 1))
    if spec.get('hollow'):                                     # two pale holes where the eyes were
        for ex in (cx - 4, cx + 4):
            for (x, y) in edge(ellipse(ex, cy - 1, 2.6, 2.2)): c.px(x, y, 1)
    for (x, y) in edge(head):
        if not lit(x, y): c.px(x, y, 1)                      # rim light on the shadow edge
    return c


# ------------------------------------------------------------ style: cameo (profile on a coin)
def cameo(spec):
    """White profile raised on a dark coin, carved with black detail -- like a struck medal."""
    c = C(W, H)
    coin = ellipse(28, 32, 26.5, 31)
    field = ellipse(28, 32, 23.5, 28)
    fill(c, coin)
    fill(c, {(x, y) for (x, y) in coin - field if (x * 3 + y) % 4 == 0}, lambda x, y: 0)   # milled rim
    fill(c, field, lambda x, y: 0)
    b = spec.get('build', 0)
    head = ellipse(25, 27, 11.5, 13.5)
    face = poly([(25, 13), (33, 14), (37, 18), (38, 22), (37, 24), (39, 28), (42, 32), (41, 34), (38, 34), (39, 36),
                 (37, 37), (38, 39), (37, 42), (37, 44), (34, 46), (28, 46), (24, 41)])
    neck = poly([(22, 38), (33, 44), (34, 52), (14, 54), (17, 44)])
    body = poly([(3 - b, 64), (7 - b, 57), (20, 52), (36, 53), (48 + b, 58), (52 + b, 64)])
    fig = (head | face | neck | body) & field
    fill(c, fig)
    for (x, y) in edge(fig): c.px(x, y, 0) if (x, y) in field else None
    # carved detail (black on white)
    c.rect(33, 24, 3, 1, 0); c.px(34, 25, 0)                    # eye
    c.line(31, 21, 36, 21, 0)                                   # brow
    c.line(25, 27, 25, 31, 0); c.px(26, 26, 0); c.px(26, 32, 0); c.px(24, 29, 0)  # ear
    c.line(38, 37, 35, 37, 0)                                   # mouth
    c.line(35, 30, 39, 33, 0)                                   # nostril / cheek line
    c.line(20, 52, 34, 53, 0)                                   # collar
    for x in range(10, 48, 5): c.line(x, 56, x + 2, 63, 0)      # folds
    if spec.get('armor'):
        for x in range(8, 50, 2): c.px(x, 58, 0) if (x, 58) in fig else None
    if spec.get('hair') in ('short', 'braid'):
        hair = {(x, y) for (x, y) in head if y < 22 + (x - 14) // 3 and x < 33 or x < 19}
        if spec.get('dark'):
            fill(c, hair & field, lambda x, y: 0)
            for k in range(6): c.line(16 + k * 3, 15, 14 + k * 2, 22 + k, 1)          # white strands in dark hair
        else:
            for k in range(6): c.line(16 + k * 3, 15, 14 + k * 2, 22 + k, 0)
        if spec.get('hair') == 'braid':
            for k in range(8):
                e = ellipse(15 - k * 0.2, 33 + k * 3, 2.4, 1.8) & field
                fill(c, e); c.px(13 - int(k * 0.2), 33 + k * 3, 0); c.px(17, 33 + k * 3 + 1, 0)
    if spec.get('scar'): c.line(34, 27, 37, 31, 0)
    if spec.get('beard'):
        bd = poly([(25, 33), (37, 38), (38, 43), (33, 48), (24, 46)]) & fig
        fill(c, bd, lambda x, y: (x + y) % 3 != 0)
        c.line(38, 37, 35, 37, 0)
    if spec.get('helm'):
        dome = {(x, y) for (x, y) in ellipse(25, 25, 13, 14.5) if y < 23}
        cheek = poly([(15, 22), (34, 22), (35, 31), (29, 40), (16, 37)])
        hm = (dome | cheek) & field
        halo(c, hm, 1); fill(c, hm)
        for (x, y) in edge(hm): c.px(x, y, 0)
        c.rect(30, 24, 6, 2, 0)                                 # eye slit
        for x in range(14, 36, 3): c.px(x, 20, 0)               # rivet band
        c.line(13, 21, 37, 21, 0)
        if spec.get('crest'):
            crest = set()
            for k in range(16): crest |= thick(12 + k, 11 - (6 - abs(k - 8)) // 2, 10 + k, 4 + abs(k - 8) // 3, 0.6)
            crest &= field
            halo(c, crest, 1); fill(c, crest, lambda x, y: x % 2 == 0)
    if spec.get('frown'): c.line(32, 21, 36, 22, 0)
    return c


STYLES = {'clean': clean, 'notan': notan, 'cameo': cameo}
