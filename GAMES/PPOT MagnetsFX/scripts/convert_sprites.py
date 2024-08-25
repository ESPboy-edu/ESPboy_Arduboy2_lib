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
deleteFile(BASE + '6x5.hpp')

convert_header(IMAGES + 'Titles/PPOT.png',                                                     BASE + 'Images.hpp', 'PPOT', 4, 128, 64)
convert_header(IMAGES + 'Titles/Sound_Checkbox.png',                                           BASE + 'Images.hpp', 'Sound_Checkbox', 4)
convert_header(IMAGES + 'Titles/Title.png',                                                    BASE + 'Images.hpp', 'Title', 4, 48, 48, maskImage=False)
convert_header(IMAGES + 'Titles/Select.png',                                                   BASE + 'Images.hpp', 'Select', 4, 48, 40, maskImage=False)

convert_header(IMAGES + 'Titles/Hooray_00.png',                                                BASE + 'Images.hpp', 'Hooray_00', 4)
convert_header(IMAGES + 'Titles/Hooray_01.png',                                                BASE + 'Images.hpp', 'Hooray_01', 4)
convert_header(IMAGES + 'Titles/Hooray_02.png',                                                BASE + 'Images.hpp', 'Hooray_02', 4)
convert_header(IMAGES + 'Titles/Hooray_03.png',                                                BASE + 'Images.hpp', 'Hooray_03', 4)
convert_header(IMAGES + 'Titles/Hooray_04.png',                                                BASE + 'Images.hpp', 'Hooray_04', 4)
convert_header(IMAGES + 'Titles/Hooray_05.png',                                                BASE + 'Images.hpp', 'Hooray_05', 4)
convert_header(IMAGES + 'Titles/Hooray_06.png',                                                BASE + 'Images.hpp', 'Hooray_06', 4)

convert_header(IMAGES + 'HUD/HUD.png',                                                         BASE + 'Images.hpp', 'HUD', 4)
convert_header(IMAGES + 'HUD/Score_Numbers_Bottom.png',                                        BASE + 'Images.hpp', 'Score_Numbers_Bottom', 4, 7, 8)
convert_header(IMAGES + 'HUD/Score_Numbers_Left.png',                                          BASE + 'Images.hpp', 'Score_Numbers_Left', 4, 7, 8)
convert_header(IMAGES + 'HUD/Score_Numbers_Right.png',                                         BASE + 'Images.hpp', 'Score_Numbers_Right', 4, 7, 8)
# convert_header(IMAGES + 'HUD/Bid.png',                                                         BASE + 'Images.hpp', 'Bid', 4, 38, 16)
convert_header(IMAGES + 'HUD/Arrows.png',                                                      BASE + 'Images.hpp', 'Arrows', 4, 13, 8)
convert_header(IMAGES + 'HUD/HighScores.png',                                                  BASE + 'Images.hpp', 'HighScores', 4)
convert_header(IMAGES + 'HUD/Dealer.png',                                                      BASE + 'Images.hpp', 'Dealer', 4)

convert_header(IMAGES + 'Titles/Wave_Top.png',                                                 BASE + 'Images.hpp', 'Wave_Top', 4)
convert_header(IMAGES + 'Titles/Wave_Bot.png',                                                 BASE + 'Images.hpp', 'Wave_Bot', 4)
convert_header(IMAGES + 'Titles/Wave_Left.png',                                                BASE + 'Images.hpp', 'Wave_Left', 4)
convert_header(IMAGES + 'Titles/Wave_Right.png',                                               BASE + 'Images.hpp', 'Wave_Right', 4)
convert_header(IMAGES + 'Titles/Magnets.png',                                                  BASE + 'Images.hpp', 'Magnets', 4)
convert_header(IMAGES + 'Titles/InGame.png',                                                   BASE + 'Images.hpp', 'InGame', 4, 34, 40)
convert_header(IMAGES + 'Titles/InGame_Solid.png',                                             BASE + 'Images.hpp', 'InGame_Solid', 4, 46, 64)

convert_header(IMAGES + 'Tiles/Tile_0102.png',                                                 BASE + 'Images.hpp', 'Tile_0102', 4, 17, 8)
convert_header(IMAGES + 'Tiles/Tile_0403.png',                                                 BASE + 'Images.hpp', 'Tile_0403', 4, 17, 8)
convert_header(IMAGES + 'Tiles/Tile_1112.png',                                                 BASE + 'Images.hpp', 'Tile_1112', 4, 8, 24)
convert_header(IMAGES + 'Tiles/Tile_2221.png',                                                 BASE + 'Images.hpp', 'Tile_2221', 4, 8, 24)
convert_header(IMAGES + 'Tiles/Tile_3132.png',                                                 BASE + 'Images.hpp', 'Tile_3132', 4, 17, 8)
convert_header(IMAGES + 'Tiles/Tile_4142.png',                                                 BASE + 'Images.hpp', 'Tile_4142', 4, 8, 24)
convert_header(IMAGES + 'Tiles/Tile_5152.png',                                                 BASE + 'Images.hpp', 'Tile_5152', 4, 17, 8)
convert_header(IMAGES + 'Tiles/Tile_6162.png',                                                 BASE + 'Images.hpp', 'Tile_6162', 4, 8, 24)

convert_header(IMAGES + 'Cursor_X.png',                                                        BASE + 'Images.hpp', 'Cursor_X', 4, 8, 8)
convert_header(IMAGES + 'Cursor_Y.png',                                                        BASE + 'Images.hpp', 'Cursor_Y', 4, 1, 8)
convert_header(IMAGES + 'Plus.png',                                                            BASE + 'Images.hpp', 'Plus', 4)
convert_header(IMAGES + 'Minus.png',                                                           BASE + 'Images.hpp', 'Minus', 4)
convert_header(IMAGES + 'Titles/Arrow.png',                                                    BASE + 'Images.hpp', 'Arrow', 4)
convert_header(IMAGES + 'Titles/Hints.png',                                                    BASE + 'Images.hpp', 'Hints', 4, 43, 40)
convert_header(IMAGES + 'Titles/Hints_Solid.png',                                              BASE + 'Images.hpp', 'Hints_Solid', 4, 43, 40)
