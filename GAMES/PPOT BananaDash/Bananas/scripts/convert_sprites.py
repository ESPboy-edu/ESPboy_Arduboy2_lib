from PIL import Image
import os
from pathlib import Path

def get_shade(rgba, shades, shade):
    if rgba[0] == 252 and rgba[1] == 111 and rgba[2] == 207 and rgba[3] == 255: 
        return 0

    w = (254 + shades) // shades
    b = (shade + 1) * w
    return 1 if rgba[0] >= b else 0

def get_mask(rgba):

    if rgba[0] == 252 and rgba[1] == 111 and rgba[2] == 207 and rgba[3] == 255: 
        return 0
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
        if i[0] == 252 and i[1] == 111 and i[2] == 207 and i[3] == 255: 
            masked = True
            break
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

convert_header(IMAGES + 'Titles/PPOT.png',                                                        BASE + 'Images.hpp', 'PPOT', 4, 128, 64)
convert_header(IMAGES + 'Titles/Title.png',                                                       BASE + 'Images.hpp', 'Title', 4, 128, 64)
convert_header(IMAGES + 'Titles/HUD_Banana.png',                                                  BASE + 'Images.hpp', 'HUD_Banana', 4, 35, 8)
convert_header(IMAGES + 'Titles/HUD_Time.png',                                                    BASE + 'Images.hpp', 'HUD_Time', 4, 26, 8)
convert_header(IMAGES + 'Titles/HUD_Directions.png',                                              BASE + 'Images.hpp', 'HUD_Directions', 4, 13, 8)
convert_header(IMAGES + 'Titles/HUD_Health.png',                                                  BASE + 'Images.hpp', 'HUD_Health', 4, 35, 8)
convert_header(IMAGES + 'Titles/Font_Alpha.png',                                                  BASE + 'Images.hpp', 'Font_Alpha', 4, 9, 8)
convert_header(IMAGES + 'Titles/Font_Alpha_Big.png',                                              BASE + 'Images.hpp', 'Font_Alpha_Big', 4, 16, 16)
convert_header(IMAGES + 'Titles/Font_Numbers.png',                                                BASE + 'Images.hpp', 'Font_Numbers', 4, 7, 8)
convert_header(IMAGES + 'Titles/HUD_TimesUp.png',                                                 BASE + 'Images.hpp', 'HUD_TimesUp', 4)
convert_header(IMAGES + 'Titles/HUD_GameOver.png',                                                BASE + 'Images.hpp', 'HUD_GameOver', 4)
convert_header(IMAGES + 'Titles/SpinningBananas.png',                                             BASE + 'Images.hpp', 'SpinningBananas', 4, 16, 16)

convert_header(IMAGES + 'Players/Player.png',                                                     BASE + 'Images.hpp', 'Player', 4, 16, 16)

convert_header(IMAGES + 'Background/Tree1.png',                                                   BASE + 'Images.hpp', 'Tree1', 4)
convert_header(IMAGES + 'Background/Tree2.png',                                                   BASE + 'Images.hpp', 'Tree2', 4)
convert_header(IMAGES + 'Background/Tree3.png',                                                   BASE + 'Images.hpp', 'Tree3', 4)
convert_header(IMAGES + 'Background/Tree4.png',                                                   BASE + 'Images.hpp', 'Tree4', 4)
convert_header(IMAGES + 'Background/Tree5.png',                                                   BASE + 'Images.hpp', 'Tree5', 4)
convert_header(IMAGES + 'Background/Tree6.png',                                                   BASE + 'Images.hpp', 'Tree6', 4)
convert_header(IMAGES + 'Background/Tree7.png',                                                   BASE + 'Images.hpp', 'Tree7', 4)
convert_header(IMAGES + 'Background/Tree8.png',                                                   BASE + 'Images.hpp', 'Tree8', 4)

convert_header(IMAGES + 'Background/Foreground.png',                                              BASE + 'Images.hpp', 'Foreground', 4)
convert_header(IMAGES + 'Background/Background.png',                                              BASE + 'Images.hpp', 'Background', 4)

convert_header(IMAGES + 'Levels/LowerOnly.png',                                                   BASE + 'Images.hpp', 'LowerOnly', 4, 16, 24)
convert_header(IMAGES + 'Levels/UpperOnly.png',                                                   BASE + 'Images.hpp', 'UpperOnly', 4, 16, 48)
convert_header(IMAGES + 'Levels/Both.png',                                                        BASE + 'Images.hpp', 'Both', 4, 16, 40)

convert_header(IMAGES + 'Enemies/Banana.png',                                                     BASE + 'Images.hpp', 'Banana', 4)
convert_header(IMAGES + 'Enemies/Puff.png',                                                       BASE + 'Images.hpp', 'Puff', 4, 16, 16)
convert_header(IMAGES + 'Enemies/Barrel.png',                                                     BASE + 'Images.hpp', 'Barrel', 4, 16, 16)
convert_header(IMAGES + 'Enemies/Bird.png',                                                       BASE + 'Images.hpp', 'Bird', 4, 14, 16)
convert_header(IMAGES + 'Enemies/Spider.png',                                                     BASE + 'Images.hpp', 'Spider', 4, 16, 16)
convert_header(IMAGES + 'Enemies/Heart.png',                                                      BASE + 'Images.hpp', 'Heart', 4, 9, 8)
convert_header(IMAGES + 'Enemies/Fire.png',                                                       BASE + 'Images.hpp', 'Fire', 4, 16, 16)
convert_header(IMAGES + 'Enemies/Fruits.png',                                                     BASE + 'Images.hpp', 'Fruits', 4, 16, 16)

convert_header(IMAGES + 'Gorilla6_2.png',                                                     BASE + 'Images.hpp', 'Gorilla', 4, 48, 48)
