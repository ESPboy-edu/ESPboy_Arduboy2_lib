#!/usr/bin/env python3
"""
Pocket Stage - the logo, and what it goes on.

  python3 tools/logo.py     writes PocketStage/logo.h (the logo as a sprite with a
                            mask, for the title page) and assets/title.png (the
                            picture the console's game menu shows)

The name in Impact: "POCKET" stretched to the width of "STAGE" under it, a
thickness behind the letters in a checkerboard, and a black line around all
of it, so that it stands out against the spotlights of the title page.
title() makes the booed-off page's title the same way (tools/build_data.py
puts it on the flash chip).
"""
import os

from PIL import Image, ImageChops, ImageDraw, ImageFilter, ImageFont

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
FONT = "/System/Library/Fonts/Supplemental/Impact.ttf"
HEADER = os.path.join(ROOT, "PocketStage", "logo.h")
PICTURE = os.path.join(ROOT, "assets", "title.png")

WIDTH = 84                 # the big word is stretched to this
STAGE_SIZE = 27            # font size of the big word
POCKET_SIZE = 15           # of the small one: rendered small, a letter at a time with air between
POCKET_W = 72              # and stretched to this
GAP = 2
DEPTH = 3                  # the thickness behind the big word


def word(text, size):
    """the word in white on black, tight around the ink"""
    font = ImageFont.truetype(FONT, size)
    l, t, r, b = font.getbbox(text)
    im = Image.new("L", (r - l, b - t), 0)
    ImageDraw.Draw(im).text((-l, -t), text, font=font, fill=255)
    return im


def threshold(im):
    return im.point(lambda v: 255 if v >= 128 else 0).convert("1")


def spaced(text, size, air):
    """the word a letter at a time, with air between the letters"""
    letters = [word(ch, size) for ch in text]
    w = sum(l.size[0] for l in letters) + air * (len(letters) - 1)
    h = max(l.size[1] for l in letters)
    im = Image.new("L", (w, h), 0)
    x = 0
    for l in letters:
        im.paste(l, (x, h - l.size[1]))
        x += l.size[0] + air
    return im


def small_word():
    pocket = spaced("POCKET", POCKET_SIZE, 2)
    return threshold(pocket.resize((POCKET_W, pocket.size[1]), Image.LANCZOS))


def big_word():
    stage = word("STAGE", STAGE_SIZE)
    return threshold(stage.resize((WIDTH, stage.size[1]), Image.LANCZOS))


def shifted(im, dx, dy, size):
    out = Image.new("1", size, 0)
    out.paste(im, (dx, dy))
    return out


def dilate(im):
    """every pixel next to a lit one is lit too"""
    return threshold(im.convert("L").filter(ImageFilter.MaxFilter(3)))


def checkerboard(size):
    im = Image.new("1", size, 0)
    px = im.load()
    for y in range(size[1]):
        for x in range(y & 1, size[0], 2):
            px[x, y] = 1
    return im


def build():
    """-> (lit, mask): the pixels to light and the pixels the logo owns"""
    pocket, stage = small_word(), big_word()
    w = max(pocket.size[0], stage.size[0])
    h = pocket.size[1] + GAP + stage.size[1]
    size = (w + DEPTH + 2, h + DEPTH + 2)
    main = shifted(pocket, 1 + (w - pocket.size[0]) // 2, 1, size)
    stage_y = 1 + pocket.size[1] + GAP
    main = ImageChops.logical_or(main, shifted(stage, 1, stage_y, size))
    body = main                                          # the letters plus the big word's thickness
    for d in range(1, DEPTH + 1):
        body = ImageChops.logical_or(body, shifted(stage, 1 + d, stage_y + d, size))
    mask = dilate(body)                                  # a line around everything
    depth = ImageChops.logical_and(ImageChops.logical_xor(body, main), checkerboard(size))
    lit = ImageChops.logical_or(main, depth)
    return lit, mask


def title(text, size):
    """a page's title in the logo's style (the booed-off page's): the word in
    Impact, its thickness behind it in a checkerboard, a black line round all
    of it -> (lit, mask)"""
    letters = threshold(word(text, size))
    w, h = letters.size
    size = (w + DEPTH + 2, h + DEPTH + 2)
    main = shifted(letters, 1, 1, size)
    body = main
    for d in range(1, DEPTH + 1):
        body = ImageChops.logical_or(body, shifted(letters, 1 + d, 1 + d, size))
    depth = ImageChops.logical_and(ImageChops.logical_xor(body, main), checkerboard(size))
    return ImageChops.logical_or(main, depth), dilate(body)


def arduboy_bytes(im, header):
    """a bitmap -> the Arduboy sprite format: width, height, then the pixels
    a column at a time, eight rows to a byte, top row in bit 0"""
    w, h = im.size
    px = im.load()
    out = [w, h] if header else []
    for page in range((h + 7) // 8):
        for x in range(w):
            b = 0
            for i in range(8):
                y = page * 8 + i
                if y < h and px[x, y]:
                    b |= 1 << i
            out.append(b)
    return out


def c_bytes(name, values, note):
    lines = []
    for i in range(0, len(values), 16):
        lines.append("  " + ", ".join("0x%02x" % v for v in values[i:i + 16]) + ",")
    return "// %s\nstatic const uint8_t %s[] PROGMEM = {\n%s\n};\n\n" % (note, name, "\n".join(lines))


def write_header(lit, mask):
    w, h = lit.size
    with open(HEADER, "w") as f:
        f.write("// Generated by tools/logo.py - do not edit by hand.\n#pragma once\n#include <avr/pgmspace.h>\n\n")
        f.write("#define LOGO_W %d\n#define LOGO_H %d\n\n" % (w, h))
        f.write(c_bytes("LOGO", arduboy_bytes(lit, True), "the logo: the letters, and their thickness in a checkerboard"))
        f.write(c_bytes("LOGO_MASK", arduboy_bytes(mask, False), "what the logo owns: the letters, the thickness and a line around them"))


PICTURE_SWING, PICTURE_STEP = 20, 9   # the moment of the title page the menu shows


def write_picture(lit, mask):
    """the console's menu picture: the title page at one moment, the logo
    over the crowd and the spotlights as the game shows them (tools/crowd.py)"""
    import crowd
    data = crowd.cached()
    buf = data["title_light"][PICTURE_SWING] + data["title_crowd"][PICTURE_STEP]
    im = Image.new("1", (128, 64), 0)
    for y in range(64):
        for x in range(128):
            if (buf[(y >> 3) * 128 + x] >> (y & 7)) & 1:
                im.putpixel((x, y), 1)
    w, h = lit.size
    x0 = (128 - w) // 2
    lit_px, mask_px = lit.load(), mask.load()
    for y in range(h):
        for x in range(w):
            if mask_px[x, y]:
                im.putpixel((x0 + x, y), 1 if lit_px[x, y] else 0)
    im.save(PICTURE)


def main():
    lit, mask = build()
    write_header(lit, mask)
    write_picture(lit, mask)
    w, h = lit.size
    print("logo %d x %d: %s, %s" % (w, h, os.path.relpath(HEADER, ROOT), os.path.relpath(PICTURE, ROOT)))


if __name__ == "__main__":
    main()
