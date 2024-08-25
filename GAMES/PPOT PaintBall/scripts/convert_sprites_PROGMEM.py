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

##convert_header_PROGMEM(IMAGES + '/Grids/Grid.png',                                      BASE + 'Tiles.h', 'Grid', 4, 102, 64)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_01.png',                                BASE + 'Tiles.h', 'Tile_00_01', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_02.png',                                BASE + 'Tiles.h', 'Tile_00_02', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_03.png',                                BASE + 'Tiles.h', 'Tile_00_03', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_04.png',                                BASE + 'Tiles.h', 'Tile_00_04', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_05.png',                                BASE + 'Tiles.h', 'Tile_00_05', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_06.png',                                BASE + 'Tiles.h', 'Tile_00_06', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_07.png',                                BASE + 'Tiles.h', 'Tile_00_07', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_00_08.png',                                BASE + 'Tiles.h', 'Tile_00_08', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_01.png',                                BASE + 'Tiles.h', 'Tile_01_01', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_02.png',                                BASE + 'Tiles.h', 'Tile_01_02', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_03.png',                                BASE + 'Tiles.h', 'Tile_01_03', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_04.png',                                BASE + 'Tiles.h', 'Tile_01_04', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_05.png',                                BASE + 'Tiles.h', 'Tile_01_05', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_06.png',                                BASE + 'Tiles.h', 'Tile_01_06', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_07.png',                                BASE + 'Tiles.h', 'Tile_01_07', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_01_08.png',                                BASE + 'Tiles.h', 'Tile_01_08', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_01.png',                                BASE + 'Tiles.h', 'Tile_02_01', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_02.png',                                BASE + 'Tiles.h', 'Tile_02_02', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_03.png',                                BASE + 'Tiles.h', 'Tile_02_03', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_04.png',                                BASE + 'Tiles.h', 'Tile_02_04', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_05.png',                                BASE + 'Tiles.h', 'Tile_02_05', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_06.png',                                BASE + 'Tiles.h', 'Tile_02_06', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_07.png',                                BASE + 'Tiles.h', 'Tile_02_07', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_02_08.png',                                BASE + 'Tiles.h', 'Tile_02_08', 4)
convert_header_PROGMEM(IMAGES + '/Tiles/Tile_Exploding.png',                            BASE + 'Tiles.h', 'Tile_Exploding', 4, 15, 16)
convert_header_PROGMEM(IMAGES + '/Enemy/Enemy.png',                                     BASE + 'Tiles.h', 'Enemy', 4)



closeFile(BASE + 'Tiles.h')
