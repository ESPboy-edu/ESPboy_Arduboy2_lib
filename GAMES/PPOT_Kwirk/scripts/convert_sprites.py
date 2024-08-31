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

convert_header(IMAGES + '/Tiles/Tile_FullRow.png',                              BASE + 'Images.hpp', 'Tile_FullRow', 4)
convert_header(IMAGES + '/Tiles/Tile_HalfRow.png',                              BASE + 'Images.hpp', 'Tile_HalfRow', 4)

convert_header(IMAGES + '/Rotator/Rotator_1_00.png',                            BASE + 'Images.hpp', 'Rotator_1_00', 4)
convert_header(IMAGES + '/Rotator/Rotator_1_01.png',                            BASE + 'Images.hpp', 'Rotator_1_01', 4)
convert_header(IMAGES + '/Rotator/Rotator_1_02.png',                            BASE + 'Images.hpp', 'Rotator_1_02', 4)
convert_header(IMAGES + '/Rotator/Rotator_1_03.png',                            BASE + 'Images.hpp', 'Rotator_1_03', 4)
convert_header(IMAGES + '/Rotator/Rotator_1_04.png',                            BASE + 'Images.hpp', 'Rotator_1_04', 4)
convert_header(IMAGES + '/Rotator/Rotator_1_05.png',                            BASE + 'Images.hpp', 'Rotator_1_05', 4)
convert_header(IMAGES + '/Rotator/Rotator_1_06.png',                            BASE + 'Images.hpp', 'Rotator_1_06', 4)
convert_header(IMAGES + '/Rotator/Rotator_1_07.png',                            BASE + 'Images.hpp', 'Rotator_1_07', 4)

convert_header(IMAGES + '/Rotator/Rotator_2_00_OW.png',                         BASE + 'Images.hpp', 'Rotator_2_00_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_00_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_00_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_01_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_01_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_02_OW.png',                         BASE + 'Images.hpp', 'Rotator_2_02_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_02_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_02_MK', 4, maskImage=True)
convert_header(IMAGES + '/Rotator/Rotator_2_03_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_03_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_04_OW.png',                         BASE + 'Images.hpp', 'Rotator_2_04_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_04_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_04_MK', 4, maskImage=True)
convert_header(IMAGES + '/Rotator/Rotator_2_05_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_05_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_06_OW.png',                         BASE + 'Images.hpp', 'Rotator_2_06_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_06_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_06_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_2_07_MK.png',                         BASE + 'Images.hpp', 'Rotator_2_07_MK', 4)

convert_header(IMAGES + '/Rotator/Rotator_3_00_OW.png',                         BASE + 'Images.hpp', 'Rotator_3_00_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_00_MK.png',                         BASE + 'Images.hpp', 'Rotator_3_00_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_01_MK.png',                         BASE + 'Images.hpp', 'Rotator_3_01_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_02_OW.png',                         BASE + 'Images.hpp', 'Rotator_3_02_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_02_MK.png',                         BASE + 'Images.hpp', 'Rotator_3_02_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_03_MK.png',                         BASE + 'Images.hpp', 'Rotator_3_03_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_04_OW.png',                         BASE + 'Images.hpp', 'Rotator_3_04_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_05_MK.png',                         BASE + 'Images.hpp', 'Rotator_3_05_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_06_OW.png',                         BASE + 'Images.hpp', 'Rotator_3_06_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_06_MK.png',                         BASE + 'Images.hpp', 'Rotator_3_06_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_3_07_MK.png',                         BASE + 'Images.hpp', 'Rotator_3_07_MK', 4)

convert_header(IMAGES + '/Rotator/Rotator_4_00_MK.png',                         BASE + 'Images.hpp', 'Rotator_4_00_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_4_01_MK.png',                         BASE + 'Images.hpp', 'Rotator_4_01_MK', 4)

convert_header(IMAGES + '/Rotator/Rotator_5_00_OW.png',                         BASE + 'Images.hpp', 'Rotator_5_00_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_5_01_MK.png',                         BASE + 'Images.hpp', 'Rotator_5_01_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_5_02_OW.png',                         BASE + 'Images.hpp', 'Rotator_5_02_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_5_03_MK.png',                         BASE + 'Images.hpp', 'Rotator_5_03_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_5_04_OW.png',                         BASE + 'Images.hpp', 'Rotator_5_04_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_5_05_MK.png',                         BASE + 'Images.hpp', 'Rotator_5_05_MK', 4)
convert_header(IMAGES + '/Rotator/Rotator_5_06_OW.png',                         BASE + 'Images.hpp', 'Rotator_5_06_OW', 4)
convert_header(IMAGES + '/Rotator/Rotator_5_07_MK.png',                         BASE + 'Images.hpp', 'Rotator_5_07_MK', 4)

#convert_header(IMAGES + '/Rotator/Rotator_Right.png',                           BASE + 'Images.hpp', 'Rotator_Right', 4)

convert_header(IMAGES + '/Menu.png',                                            BASE + 'Images.hpp', 'Menu', 4, 40, 64)
convert_header(IMAGES + '/Numbers/Numbers_5x3_1D_WB.png',                       BASE + 'Images.hpp', 'Numbers_1D_WB', 4, 3, 8)
convert_header(IMAGES + '/Numbers/Numbers_5x3_2D_WB.png',                       BASE + 'Images.hpp', 'Numbers_2D_WB', 4, 7, 8)
convert_header(IMAGES + '/Title.png',                                           BASE + 'Images.hpp', 'Title', 4, 128, 64, maskImage=False)
convert_header(IMAGES + 'Tick.png',                                             BASE + 'Images.hpp', 'Tick', 4)

# convert_header(IMAGES + '/Tiles/Tile_04_01.png',                                BASE + 'Images.hpp', 'Tile_04_01', 4)
convert_header(IMAGES + '/Tiles/Tile_04_02.png',                                BASE + 'Images.hpp', 'Tile_04_02', 4)
convert_header(IMAGES + '/Tiles/Tile_04_03.png',                                BASE + 'Images.hpp', 'Tile_04_03', 4)
convert_header(IMAGES + '/Tiles/Tile_04_04.png',                                BASE + 'Images.hpp', 'Tile_04_04', 4)
convert_header(IMAGES + '/Tiles/Tile_04_05.png',                                BASE + 'Images.hpp', 'Tile_04_05', 4)
# convert_header(IMAGES + '/Tiles/Tile_05_01.png',                                BASE + 'Images.hpp', 'Tile_05_01', 4)
convert_header(IMAGES + '/Tiles/Tile_05_02.png',                                BASE + 'Images.hpp', 'Tile_05_02', 4)
convert_header(IMAGES + '/Tiles/Tile_05_03.png',                                BASE + 'Images.hpp', 'Tile_05_03', 4)
convert_header(IMAGES + '/Tiles/Tile_05_04.png',                                BASE + 'Images.hpp', 'Tile_05_04', 4)
convert_header(IMAGES + '/Tiles/Tile_05_05.png',                                BASE + 'Images.hpp', 'Tile_05_05', 4)
# convert_header(IMAGES + '/Tiles/Tile_10_01.png',                                BASE + 'Images.hpp', 'Tile_10_01', 4)
convert_header(IMAGES + '/Tiles/Tile_10_02.png',                                BASE + 'Images.hpp', 'Tile_10_02', 4)
convert_header(IMAGES + '/Tiles/Tile_10_03.png',                                BASE + 'Images.hpp', 'Tile_10_03', 4)
convert_header(IMAGES + '/Tiles/Tile_10_04.png',                                BASE + 'Images.hpp', 'Tile_10_04', 4)
convert_header(IMAGES + '/Tiles/Tile_10_05.png',                                BASE + 'Images.hpp', 'Tile_10_05', 4)

convert_header(IMAGES + 'Blocks/Block_02_01.png',                               BASE + 'Images.hpp', 'Block_02_01', 4)
convert_header(IMAGES + 'Blocks/Block_03_01.png',                               BASE + 'Images.hpp', 'Block_03_01', 4)
convert_header(IMAGES + 'Blocks/Block_04_01.png',                               BASE + 'Images.hpp', 'Block_04_01', 4)
convert_header(IMAGES + 'Blocks/Block_05_01.png',                               BASE + 'Images.hpp', 'Block_05_01', 4)

convert_header(IMAGES + 'PPOT.png',                                             BASE + 'Images.hpp', 'PPOT', 4, 128, 64)
convert_header(IMAGES + 'Sound_Volume.png',                                     BASE + 'Images.hpp', 'Sound_Volume', 4, 22, 16)
convert_header(IMAGES + 'Sound_Checkbox.png',                                   BASE + 'Images.hpp', 'Sound_Checkbox', 4)