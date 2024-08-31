from PIL import Image
import os
from pathlib import Path

def get_shade_PROGMEM(rgba, shades, shade):
    w = (254 + shades) // shades
    b = (shade + 1) * w
    return 1 if rgba[0] >= b else 0

def get_mask_PROGMEM(rgba):
    return 1 if rgba[3] >= 128 else 0

def convert_PROGMEM(fname, shades, sw = None, sh = None, num = None, maskImage = False):

    if not (shades >= 2 and shades <= 4):
        print('shades argument must be 2, 3, or 4')
        return None

    im = Image.open(fname).convert('RGBA')
    pixels = list(im.getdata())
    
    masked = maskImage    
    for i in pixels:
        if i[3] < 255:
            masked = True
            break
    
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
                        byte |= (get_shade_PROGMEM(rgba, shades, shade) << iy)
                        mask |= (get_mask_PROGMEM(rgba) << iy)
                    bytes += bytearray([byte])
                    if masked:
                        bytes += bytearray([mask])
    
    return bytes
    
def convert_header_PROGMEM(fname, fout, sym, shades, sw = None, sh = None, num = None, maskImage = False):
    bytes = convert_PROGMEM(fname, shades, sw, sh, num, maskImage)
    if bytes is None: return
    with open(fout, 'a') as f:
        # f.write('#pragma once\n\n#include <stdint.h>\n#include <avr/pgmspace.h>\n\n')
        f.write('constexpr uint8_t %s[] PROGMEM =\n{\n' % sym)
        for n in range(len(bytes)):
            if n % 16 == 0:
                f.write('    ')
            f.write('%3d,' % bytes[n])
            f.write(' ' if n % 16 != 15 else '\n')
        if len(bytes) % 16 != 0:
            f.write('\n')
        f.write('};\n')

def openFile(fout, namespace):
    with open(fout, 'a') as f:
        f.write('#pragma once\n\n#include <stdint.h>\n#include <avr/pgmspace.h>\n\n')
        f.write('namespace %s {\n' % namespace)

def closeFile(fout):
    with open(fout, 'a') as f:
        f.write('};')


def deleteFile(filename):
    if os.path.isfile(filename):
        os.remove(filename)



BASE = './images/'
IMAGES = '../images/'

deleteFile(BASE + 'Tiles.h')

openFile(BASE + 'Tiles.h', 'Images')

convert_header_PROGMEM(IMAGES + '/Tiles/Tiles1.png',                                    BASE + 'Tiles.h', 'Tiles1', 4, 8, 8)
convert_header_PROGMEM(IMAGES + '/Tiles/Tiles2.png',                                    BASE + 'Tiles.h', 'Tiles2', 4, 8, 8, maskImage=True)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_Block.png',                                BASE + 'Tiles.h', 'Tile_Block', 4, 8, 8, maskImage=True)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_Blank.png',                                BASE + 'Tiles.h', 'Tile_Blank', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_04_01.png',                                BASE + 'Tiles.h', 'Tile_04_01', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_04_02.png',                                BASE + 'Tiles.h', 'Tile_04_02', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_04_03.png',                                BASE + 'Tiles.h', 'Tile_04_03', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_04_04.png',                                BASE + 'Tiles.h', 'Tile_04_04', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_04_05.png',                                BASE + 'Tiles.h', 'Tile_04_05', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_05_01.png',                                BASE + 'Tiles.h', 'Tile_05_01', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_05_02.png',                                BASE + 'Tiles.h', 'Tile_05_02', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_05_03.png',                                BASE + 'Tiles.h', 'Tile_05_03', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_05_04.png',                                BASE + 'Tiles.h', 'Tile_05_04', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_05_05.png',                                BASE + 'Tiles.h', 'Tile_05_05', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_10_01.png',                                BASE + 'Tiles.h', 'Tile_10_01', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_10_02.png',                                BASE + 'Tiles.h', 'Tile_10_02', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_10_03.png',                                BASE + 'Tiles.h', 'Tile_10_03', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_10_04.png',                                BASE + 'Tiles.h', 'Tile_10_04', 4)
# convert_header_PROGMEM(IMAGES + '/Tiles/Tile_10_05.png',                                BASE + 'Tiles.h', 'Tile_10_05', 4)

convert_header_PROGMEM(IMAGES + 'Blocks/Block_01_01.png',                               BASE + 'Tiles.h', 'Block_01_01', 4, 8, 16, maskImage=False)
# convert_header_PROGMEM(IMAGES + 'Blocks/Block_02_01.png',                               BASE + 'Tiles.h', 'Block_02_01', 4)
# convert_header_PROGMEM(IMAGES + 'Blocks/Block_03_01.png',                               BASE + 'Tiles.h', 'Block_03_01', 4)
# convert_header_PROGMEM(IMAGES + 'Blocks/Block_04_01.png',                               BASE + 'Tiles.h', 'Block_04_01', 4)
# convert_header_PROGMEM(IMAGES + 'Blocks/Block_05_01.png',                               BASE + 'Tiles.h', 'Block_05_01', 4)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_T1.png',                                  BASE + 'Tiles.h', 'Block_T1', 4, 8, 8, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_TM.png',                                  BASE + 'Tiles.h', 'Block_TM', 4, 8, 8, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_TL.png',                                  BASE + 'Tiles.h', 'Block_TL', 4, 8, 16, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_TR.png',                                  BASE + 'Tiles.h', 'Block_TR', 4, 8, 16, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_TL_2.png',                                BASE + 'Tiles.h', 'Block_TL_2', 4, 8, 8, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_TR_2.png',                                BASE + 'Tiles.h', 'Block_TR_2', 4, 8, 8, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_MV.png',                                  BASE + 'Tiles.h', 'Block_MV', 4, 8, 8, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_MH.png',                                  BASE + 'Tiles.h', 'Block_MH', 4, 8, 16, maskImage=True)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_ML.png',                                  BASE + 'Tiles.h', 'Block_ML', 4, 8, 8, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_MR.png',                                  BASE + 'Tiles.h', 'Block_MR', 4, 8, 8, maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_M.png',                                   BASE + 'Tiles.h', 'Block_M', 4,8, 8,  maskImage=False)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_B1.png',                                  BASE + 'Tiles.h', 'Block_B1', 4, 8, 16)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_BL.png',                                  BASE + 'Tiles.h', 'Block_BL', 4, 8, 16)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_BM.png',                                  BASE + 'Tiles.h', 'Block_BM', 4, 8, 16)
convert_header_PROGMEM(IMAGES + 'Blocks/Block_BR.png',                                  BASE + 'Tiles.h', 'Block_BR', 4, 8, 16)

convert_header_PROGMEM(IMAGES + '/Player/Player.png',                                   BASE + 'Tiles.h', 'Player', 4, 8, 16, maskImage=True)
convert_header_PROGMEM(IMAGES + '/Stairs.png',                                          BASE + 'Tiles.h', 'Stairs', 4, 8, 16, maskImage=True)

closeFile(BASE + 'Tiles.h')
