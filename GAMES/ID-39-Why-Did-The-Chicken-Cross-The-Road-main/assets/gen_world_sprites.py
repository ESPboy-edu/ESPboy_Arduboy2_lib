#!/usr/bin/env python3
"""Generates bitmaps_world.h : isometric box sprites (plus-mask format) for ID-39.

Projection (world px -> screen px), same as the firmware:
    sx = X + Y            X = along the lane (down-right on screen)
    sy = (X - Y)/2 - Z    Y = across lanes, 8 units = 1 lane (up-right on screen)
Every sprite is anchored on the projection of the *cell origin* (X=0,Y=0,Z=0);
<NAME>_OX / <NAME>_OY say where the sprite's top-left pixel sits relative to it.
"""
import sys
from PIL import Image, ImageDraw


def proj(X, Y, Z):
    return (X + Y, (X - Y) // 2 - Z)


class Canvas:
    def __init__(self):
        self.shapes = []  # drawing ops, replayed after bbox is known

    def box(self, X0, Y0, Z0, Lu, Dv, H, top=255, front=255, end=255):
        self.shapes.append(("box", X0, Y0, Z0, Lu, Dv, H, top, front, end))

    def pix(self, X, Y, Z, v):
        self.shapes.append(("pix", X, Y, Z, v))

    def line3(self, a, b, v):
        self.shapes.append(("line", a, b, v))

    def render(self):
        pts = []
        for s in self.shapes:
            if s[0] == "box":
                _, X0, Y0, Z0, Lu, Dv, H, *_ = s
                for X in (X0, X0 + Lu):
                    for Y in (Y0, Y0 + Dv):
                        for Z in (Z0, Z0 + H):
                            pts.append(proj(X, Y, Z))
        minx = min(p[0] for p in pts); maxx = max(p[0] for p in pts)
        miny = min(p[1] for p in pts); maxy = max(p[1] for p in pts)
        w, h = maxx - minx + 1, maxy - miny + 1
        img = Image.new("L", (w, h), 0)
        msk = Image.new("L", (w, h), 0)
        di, dm = ImageDraw.Draw(img), ImageDraw.Draw(msk)

        def P(X, Y, Z):
            x, y = proj(X, Y, Z)
            return (x - minx, y - miny)

        for s in self.shapes:
            if s[0] == "box":
                _, X0, Y0, Z0, Lu, Dv, H, top, front, end = s
                X1, Y1, Z1 = X0 + Lu, Y0 + Dv, Z0 + H
                faces = [
                    ([P(X0, Y0, Z1), P(X1, Y0, Z1), P(X1, Y1, Z1), P(X0, Y1, Z1)], top),
                    ([P(X0, Y0, Z0), P(X1, Y0, Z0), P(X1, Y0, Z1), P(X0, Y0, Z1)], front),
                    ([P(X1, Y0, Z0), P(X1, Y1, Z0), P(X1, Y1, Z1), P(X1, Y0, Z1)], end),
                ]
                for poly, fill in faces:
                    dm.polygon(poly, fill=255, outline=255)
                    di.polygon(poly, fill=255, outline=0)
                    if fill != 255:  # pattern: 'checker' or 'hatch'
                        xs = [p[0] for p in poly]; ys = [p[1] for p in poly]
                        tmp = Image.new("L", (w, h), 0)
                        ImageDraw.Draw(tmp).polygon(poly, fill=255)
                        # interior only: erode by skipping outline pixels
                        ol = Image.new("L", (w, h), 0)
                        ImageDraw.Draw(ol).polygon(poly, fill=0, outline=255)
                        for yy in range(min(ys), max(ys) + 1):
                            for xx in range(min(xs), max(xs) + 1):
                                if tmp.getpixel((xx, yy)) and not ol.getpixel((xx, yy)):
                                    if fill == 128 and (xx + yy) % 2 == 0:
                                        di.point((xx, yy), 0)
                                    elif fill == 64 and (xx % 2 == 0 and yy % 2 == 0):
                                        di.point((xx, yy), 0)
            elif s[0] == "pix":
                _, X, Y, Z, v = s
                x, y = P(X, Y, Z)
                di.point((x, y), v)
            elif s[0] == "line":
                _, a, b, v = s
                di.line([P(*a), P(*b)], fill=v)
        return img, msk, minx, miny


def pack(img, msk):
    w, h = img.size
    out = []
    for page in range((h + 7) // 8):
        for x in range(w):
            ib = mb = 0
            for bit in range(8):
                y = page * 8 + bit
                if y < h:
                    if msk.getpixel((x, y)):
                        mb |= 1 << bit
                        if img.getpixel((x, y)):
                            ib |= 1 << bit
            out += [ib, mb]
    return out


def emit(name, cv, f, preview):
    img, msk, ox, oy = cv.render()
    w, h = img.size
    data = pack(img, msk)
    f.write(f"#define {name}_OX {ox}\n#define {name}_OY {oy}\n")
    f.write(f"const unsigned char PROGMEM {name}[] =\n{{\n  {w}, {h},\n")
    for i in range(0, len(data), 16):
        f.write("  " + ", ".join(f"0x{b:02x}" for b in data[i:i + 16]) + ",\n")
    f.write("};\n\n")
    # preview (white = white, black = black, transparent = magenta)
    pv = Image.new("RGB", (w, h), (255, 0, 255))
    for y in range(h):
        for x in range(w):
            if msk.getpixel((x, y)):
                pv.putpixel((x, y), (255, 255, 255) if img.getpixel((x, y)) else (0, 0, 0))
    preview.append((name, pv))


def bush():
    c = Canvas()
    c.box(0, 0, 0, 8, 8, 7, top=255, front=255, end=128)
    # leaf specks on top + front
    for (X, Y) in ((2, 3), (5, 5), (4, 1), (6, 2)):
        c.pix(X, Y, 7, 0)
    c.pix(3, 0, 5, 0); c.pix(6, 0, 3, 0); c.pix(2, 0, 2, 0)
    return c


def car():
    c = Canvas()
    c.box(1, 1, 0, 16, 6, 4, top=255, front=255, end=128)
    c.box(5, 1, 4, 8, 6, 3, top=255, front=255, end=128)
    # windows (front face of cabin) and wheels
    c.line3((7, 1, 5), (11, 1, 5), 0)
    c.pix(4, 1, 0, 0); c.pix(5, 1, 0, 0); c.pix(13, 1, 0, 0); c.pix(14, 1, 0, 0)
    return c


def truck():
    c = Canvas()
    c.box(1, 1, 0, 24, 6, 4, top=255, front=255, end=128)
    c.box(1, 1, 4, 16, 6, 4, top=255, front=64, end=255)   # cargo box
    c.box(19, 1, 4, 6, 6, 2, top=255, front=255, end=128)  # cab
    c.pix(5, 1, 0, 0); c.pix(6, 1, 0, 0); c.pix(19, 1, 0, 0); c.pix(20, 1, 0, 0)
    c.line3((21, 1, 5), (23, 1, 5), 0)
    return c


def log(n):
    L = 8 * n
    c = Canvas()
    c.box(1, 1, 0, L, 6, 3, top=255, front=128, end=255)
    # bark lines on top
    for k in range(2, L - 2, 6):
        c.line3((k, 3, 3), (k + 2, 3, 3), 0)
        c.line3((k + 3, 5, 3), (k + 5, 5, 3), 0)
    # ring on end face
    c.pix(L + 1, 3, 1, 0); c.pix(L + 1, 4, 1, 0)
    return c


def main(outdir):
    prev = []
    with open(f"{outdir}/bitmaps_world.h", "w") as f:
        f.write("// Generated by gen_sprites.py -- isometric world sprites (plus-mask format)\n")
        f.write("// Anchor: projection of the object's cell origin. Sprite top-left = anchor + (_OX, _OY).\n")
        f.write("#ifndef BITMAPS_WORLD_H\n#define BITMAPS_WORLD_H\n\n")
        emit("bushSprite", bush(), f, prev)
        emit("carSprite", car(), f, prev)
        emit("truckSprite", truck(), f, prev)
        emit("log2Sprite", log(2), f, prev)
        emit("log3Sprite", log(3), f, prev)
        emit("log4Sprite", log(4), f, prev)
        f.write("#endif\n")
    # contact sheet
    S = 6
    W = sum(p.width for _, p in prev) + 4 * (len(prev) + 1)
    H = max(p.height for _, p in prev) + 8
    sheet = Image.new("RGB", (W, H), (255, 0, 255))
    x = 4
    for _, p in prev:
        sheet.paste(p, (x, 4)); x += p.width + 4
    sheet.resize((W * S, H * S), Image.NEAREST).save(f"{outdir}/sprites_preview.png")


if __name__ == "__main__":
    main(sys.argv[1])
