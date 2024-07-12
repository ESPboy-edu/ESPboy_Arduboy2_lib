from PIL import Image
import os
from pathlib import Path

def get_shade(rgba, shades, shade):
    w = (254 + shades) // shades
    b = (shade + 1) * w
    return 1 if rgba[0] >= b else 0

def get_mask(rgba):
    return 1 if rgba[3] >= 128 else 0

def convert(fname, shades, sw = None, sh = None, num = None, maskImage = False):

    if not (shades >= 2 and shades <= 4):
        print('shades argument must be 2, 3, or 4')
        return None

    im = Image.open(fname).convert('RGBA')
    pixels = list(im.getdata())
    
    masked = maskImage
    q = 0
    for i in pixels:
        q = q + 1
        # print(i[0])
        # print(i[1])
        # print(i[2])
        # print(i[3])
        if i[3] < 255:
            # print('masked!!! ')
            # print(q)
            masked = True
            # exit()
            break

    print('{}, shades {}, masked {}'.format(fname, shades, masked))


    w = im.width
    h = im.height
    if sw is None: sw = w
    if sh is None: sh = h
    nw = w // sw
    nh = h // sh
    if num is None: num = nw * nh
    sp = (sh + 7) // 8
    
    if nw * nh <= 0:
        print('%s: Invalid sprite dimensions' % fname)
        return None
        
    bytes = bytearray([sw, sh])
    
    for n in range(num):
        bx = (n % nw) * sw
        by = (n // nw) * sh
        for shade in range(shades - 1):
            for p in range(sp):
                for ix in range(sw):
                    x = bx + ix
                    byte = 0
                    mask = 0
                    for iy in range(8):
                        y = p * 8 + iy
                        if y >= sh: break
                        y += by
                        i = y * w + x
                        rgba = pixels[i]
                        byte |= (get_shade(rgba, shades, shade) << iy)
                        mask |= (get_mask(rgba) << iy)
                    bytes += bytearray([byte])
                    if masked:
                        bytes += bytearray([mask])
    
    return bytes
    
def convert_header(fname, fout, sym, shades, sw = None, sh = None, num = None, maskImage = False):
    bytes = convert(fname, shades, sw, sh, num, maskImage)
    if bytes is None: return
    with open(fout, 'a') as f:
        # f.write('#pragma once\n\n#include <stdint.h>\n#include <avr/pgmspace.h>\n\n')
        # f.write('constexpr uint8_t %s[] PROGMEM =\n{\n' % sym)
        f.write('uint8_t %s[] = {\n  ' % sym)
        for n in range(len(bytes)):
            if n % 16 == 2:
                f.write('\n  ')
            f.write('%3d,' % bytes[n])
            # f.write(' ' if n % 16 != 15 else '\n')
            # f.write(' ' if n % 18 != 2 else '\n')
            f.write(' ')
        if len(bytes) % 16 != 2:
            f.write('\n')
        f.write('};\n\n')

def deleteFile(filename):
    if os.path.isfile(filename):
        os.remove(filename)

BASE = './images/'
IMAGES = '../images/'

deleteFile(BASE + 'Images.hpp')

convert_header(IMAGES + 'Player/Player.png',                                                   BASE + 'Images.hpp', 'Player', 4, 16, 16, maskImage=True)
convert_header(IMAGES + 'Player/Player_Thrust.png',                                            BASE + 'Images.hpp', 'Player_Thrust', 4, 16, 8, maskImage=True)
convert_header(IMAGES + 'Player/Bullets.png',                                                  BASE + 'Images.hpp', 'Bullets', 4, 20, 8)
convert_header(IMAGES + 'Enemy/Enemy_00.png',                                                  BASE + 'Images.hpp', 'Enemy_00', 4, 21, 16, maskImage=True)
convert_header(IMAGES + 'Enemy/Enemy_00_Pickup.png',                                           BASE + 'Images.hpp', 'Enemy_00_Pickup', 4, 21, 24, maskImage=True)
convert_header(IMAGES + 'Enemy/Enemy_01.png',                                                  BASE + 'Images.hpp', 'Enemy_01', 4, 9, 16, maskImage=True)
convert_header(IMAGES + 'Enemy/Enemy_02.png',                                                  BASE + 'Images.hpp', 'Enemy_02', 4, 7, 8, maskImage=True)
convert_header(IMAGES + 'Enemy/Enemy_03.png',                                                  BASE + 'Images.hpp', 'Enemy_03', 4, 8, 16, maskImage=True)

convert_header(IMAGES + 'Titles/PPOT.png',                                                     BASE + 'Images.hpp', 'PPOT', 4, 128, 64)
convert_header(IMAGES + 'Titles/Sound_Volume.png',                                             BASE + 'Images.hpp', 'Sound_Volume', 4, 22, 16)
convert_header(IMAGES + 'Titles/Sound_Checkbox.png',                                           BASE + 'Images.hpp', 'Sound_Checkbox', 4)
convert_header(IMAGES + 'Titles/Title.png',                                                    BASE + 'Images.hpp', 'Title', 4, 128, 24, maskImage=False)
convert_header(IMAGES + 'Titles/Title_Text.png',                                               BASE + 'Images.hpp', 'Title_Text', 4,maskImage=False)

convert_header(IMAGES + 'Titles/GameOver.png',                                                 BASE + 'Images.hpp', 'GameOver', 4)
convert_header(IMAGES + 'Titles/Quit.png',                                                     BASE + 'Images.hpp', 'Quit', 4)
convert_header(IMAGES + 'Titles/EnemyLanding.png',                                             BASE + 'Images.hpp', 'EnemyLanding', 4, 64, 16, maskImage=True)

convert_header(IMAGES + 'Scenery/BG_00.png',                                                   BASE + 'Images.hpp', 'BG_00', 4, 96, 40)
convert_header(IMAGES + 'Scenery/MG_00.png',                                                   BASE + 'Images.hpp', 'MG_00', 4, 96, 24)
convert_header(IMAGES + 'Scenery/FG_00.png',                                                   BASE + 'Images.hpp', 'FG_00', 4, 32, 24)
