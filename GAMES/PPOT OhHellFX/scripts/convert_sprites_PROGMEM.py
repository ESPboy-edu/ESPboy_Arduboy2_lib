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

deleteFile(BASE + 'Images.h')
openFile(BASE + 'Images.h', 'Images')

# convert_header_PROGMEM(IMAGES + 'Numbers/Numbers_5x3_2D_MB.png',                                  BASE + 'Images.h', 'Numbers_5x3_2D_MB', 4, 7, 8)
# convert_header_PROGMEM(IMAGES + 'Numbers/Numbers_5x3_2D_WB.png',                                  BASE + 'Images.h', 'Numbers_5x3_2D_WB', 4, 7, 8)

closeFile(BASE + 'Images.h')
