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

convert_header(IMAGES + 'Cards/Cards_Bottom.png',                                              BASE + 'Images.hpp', 'Cards_Bottom', 4, 18, 24)
convert_header(IMAGES + 'Cards/Cards_Left.png',                                                BASE + 'Images.hpp', 'Cards_Left', 4, 23, 24)
convert_header(IMAGES + 'Cards/Cards_Right.png',                                               BASE + 'Images.hpp', 'Cards_Right', 4, 23, 24)

convert_header(IMAGES + 'Cards/Hand_Left.png',                                                 BASE + 'Images.hpp', 'Hand_Left', 4, 5, 40)
convert_header(IMAGES + 'Cards/Hand_Right.png',                                                BASE + 'Images.hpp', 'Hand_Right', 4, 5, 40)
convert_header(IMAGES + 'Cards/Hand_Top.png',                                                  BASE + 'Images.hpp', 'Hand_Top', 4, 43, 8)


convert_header(IMAGES + 'Cards/Cards_Bottom_Grey.png',                                         BASE + 'Images.hpp', 'Cards_Bottom_Grey', 4, 18, 24)
convert_header(IMAGES + 'Cards/Cards_Left_Grey.png',                                           BASE + 'Images.hpp', 'Cards_Left_Grey', 4, 23, 24)
convert_header(IMAGES + 'Cards/Cards_Right_Grey.png',                                          BASE + 'Images.hpp', 'Cards_Right_Grey', 4, 23, 24)

convert_header(IMAGES + 'Titles/PPOT.png',                                                     BASE + 'Images.hpp', 'PPOT', 4, 128, 64)
convert_header(IMAGES + 'Titles/Sound_Volume_Grey.png',                                        BASE + 'Images.hpp', 'Sound_Volume_Grey', 4, 22, 16)
convert_header(IMAGES + 'Titles/Sound_Volume_White.png',                                       BASE + 'Images.hpp', 'Sound_Volume_White', 4, 22, 16)
convert_header(IMAGES + 'Titles/Sound_Checkbox.png',                                           BASE + 'Images.hpp', 'Sound_Checkbox', 4)
convert_header(IMAGES + 'Titles/Title.png',                                                    BASE + 'Images.hpp', 'Title', 4, 48, 40, maskImage=False)


convert_header(IMAGES + 'HUD/HUD.png',                                                         BASE + 'Images.hpp', 'HUD', 4)
convert_header(IMAGES + 'HUD/Score_Numbers_Bottom.png',                                        BASE + 'Images.hpp', 'Score_Numbers_Bottom', 4, 7, 8)
convert_header(IMAGES + 'HUD/Score_Numbers_Left.png',                                          BASE + 'Images.hpp', 'Score_Numbers_Left', 4, 7, 8)
convert_header(IMAGES + 'HUD/Score_Numbers_Right.png',                                         BASE + 'Images.hpp', 'Score_Numbers_Right', 4, 7, 8)
convert_header(IMAGES + 'HUD/Bid.png',                                                         BASE + 'Images.hpp', 'Bid', 4, 38, 16)
convert_header(IMAGES + 'HUD/Numbers.png',                                                     BASE + 'Images.hpp', 'Numbers', 4, 16, 16)
convert_header(IMAGES + 'HUD/Numbers_3x5.png',                                                 BASE + 'Images.hpp', 'Numbers_3x5', 4, 13, 8)
convert_header(IMAGES + 'HUD/Arrows.png',                                                      BASE + 'Images.hpp', 'Arrows', 4, 13, 8)
convert_header(IMAGES + 'HUD/HighScores.png',                                                  BASE + 'Images.hpp', 'HighScores', 4)
convert_header(IMAGES + 'HUD/Dealer.png',                                                      BASE + 'Images.hpp', 'Dealer', 4)

convert_header(IMAGES + 'Cards/Hand_Full.png',                                                 BASE + 'Images.hpp', 'Hand_Full', 4)
convert_header(IMAGES + 'Cards/Rotate/Rotate.png',                                             BASE + 'Images.hpp', 'Rotate', 4, 30, 32)
convert_header(IMAGES + 'Cards/Flip/Flip.png',                                                 BASE + 'Images.hpp', 'Flip', 4, 16, 24)

convert_header(IMAGES + 'Titles/Fire.png',                                                     BASE + 'Images.hpp', 'Fire', 4, 128, 64, maskImage=False)
convert_header(IMAGES + 'Titles/OhHell.png',                                                   BASE + 'Images.hpp', 'OhHell', 4)
convert_header(IMAGES + 'GameOver.png',                                                        BASE + 'Images.hpp', 'GameOver', 4)
convert_header(IMAGES + 'Instructions_01.png',                                                 BASE + 'Images.hpp', 'Instructions_01', 4)
convert_header(IMAGES + 'Instructions_02.png',                                                 BASE + 'Images.hpp', 'Instructions_02', 4)
convert_header(IMAGES + 'Instructions_Arrows.png',                                             BASE + 'Images.hpp', 'Instructions_Arrows', 4, 8, 8)
convert_header(IMAGES + 'Pause.png',                                                           BASE + 'Images.hpp', 'Pause', 4, 128, 64)
