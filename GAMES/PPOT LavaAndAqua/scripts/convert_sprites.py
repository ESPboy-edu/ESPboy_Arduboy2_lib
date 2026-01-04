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

convert_header(IMAGES + 'Titles/PPOT.png',                                          BASE + 'Images.hpp', 'PPOT', 4, 128, 64)
convert_header(IMAGES + 'Titles/ClearProgress.png',                                 BASE + 'Images.hpp', 'ClearProgress', 4)
convert_header(IMAGES + 'Titles/Menu.png',                                          BASE + 'Images.hpp', 'Menu', 4, 32, 64)
convert_header(IMAGES + 'Titles/Title_Top.png',                                     BASE + 'Images.hpp', 'Title_Top', 4)
convert_header(IMAGES + 'Titles/Title_Bottom.png',                                  BASE + 'Images.hpp', 'Title_Bottom', 4)
convert_header(IMAGES + 'Titles/Title_Lava.png',                                    BASE + 'Images.hpp', 'Title_Lava', 4)
convert_header(IMAGES + 'Titles/Title_Amp.png',                                     BASE + 'Images.hpp', 'Title_Amp', 4)
convert_header(IMAGES + 'Titles/Title_Aqua.png',                                    BASE + 'Images.hpp', 'Title_Aqua', 4)
convert_header(IMAGES + 'Titles/Title_Good.png',                                    BASE + 'Images.hpp', 'Title_Good', 4)
convert_header(IMAGES + 'Titles/Title_Work.png',                                    BASE + 'Images.hpp', 'Title_Work', 4)

convert_header(IMAGES + 'Extras/Levels_Select.png',                                 BASE + 'Images.hpp', 'Levels_Select', 4, 17, 16)
convert_header(IMAGES + 'Extras/Levels_Cursor.png',                                 BASE + 'Images.hpp', 'Levels_Cursor', 4, 17, 16)
convert_header(IMAGES + 'Extras/Levels_HUD.png',                                    BASE + 'Images.hpp', 'Levels_HUD', 4, 53, 64)
convert_header(IMAGES + 'Extras/Levels_Status.png',                                 BASE + 'Images.hpp', 'Levels_Status', 4, 43, 8)
convert_header(IMAGES + 'Extras/Levels_Number.png',                                 BASE + 'Images.hpp', 'Levels_Number', 4, 11, 8)
convert_header(IMAGES + 'Extras/Levels_NumberOfMoves.png',                          BASE + 'Images.hpp', 'Levels_NumberOfMoves', 4)
convert_header(IMAGES + 'Extras/Levels_NumberOfMoves_Numbers.png',                  BASE + 'Images.hpp', 'Levels_NumberOfMoves_Numbers', 4, 5, 8)
convert_header(IMAGES + 'Extras/Puff.png',                                          BASE + 'Images.hpp', 'Puff', 4, 32, 32)

convert_header(IMAGES + 'HUD/Mini_HUD.png',                                         BASE + 'Images.hpp', 'Mini_HUD', 4)
convert_header(IMAGES + 'HUD/Numbers_HUD.png',                                      BASE + 'Images.hpp', 'Numbers_HUD', 4, 8, 8)

convert_header(IMAGES + 'Tiles/Tiles.png',                                          BASE + 'Images.hpp', 'Tiles', 4, 8, 8)
convert_header(IMAGES + 'Tiles/Lava_01.png',                                        BASE + 'Images.hpp', 'Lava_01', 4, 8, 8)
convert_header(IMAGES + 'Tiles/Lava_02.png',                                        BASE + 'Images.hpp', 'Lava_02', 4, 16, 8)
convert_header(IMAGES + 'Tiles/Lava_03.png',                                        BASE + 'Images.hpp', 'Lava_03', 4, 24, 8)
convert_header(IMAGES + 'Tiles/Lava_04.png',                                        BASE + 'Images.hpp', 'Lava_04', 4, 32, 8)
convert_header(IMAGES + 'Tiles/Lava_05.png',                                        BASE + 'Images.hpp', 'Lava_05', 4, 40, 8)
convert_header(IMAGES + 'Tiles/Lava_06.png',                                        BASE + 'Images.hpp', 'Lava_06', 4, 48, 8)
convert_header(IMAGES + 'Tiles/Lava_07.png',                                        BASE + 'Images.hpp', 'Lava_07', 4, 56, 8)
convert_header(IMAGES + 'Tiles/Lava_08.png',                                        BASE + 'Images.hpp', 'Lava_08', 4, 64, 8)
convert_header(IMAGES + 'Tiles/Lava_09.png',                                        BASE + 'Images.hpp', 'Lava_09', 4, 72, 8)
convert_header(IMAGES + 'Tiles/Lava_10.png',                                        BASE + 'Images.hpp', 'Lava_10', 4, 80, 8)
convert_header(IMAGES + 'Tiles/Lava_11.png',                                        BASE + 'Images.hpp', 'Lava_11', 4, 88, 8)
convert_header(IMAGES + 'Tiles/Lava_12.png',                                        BASE + 'Images.hpp', 'Lava_12', 4, 96, 8)
convert_header(IMAGES + 'Tiles/Lava_13.png',                                        BASE + 'Images.hpp', 'Lava_13', 4, 104, 8)
convert_header(IMAGES + 'Tiles/Lava_14.png',                                        BASE + 'Images.hpp', 'Lava_14', 4, 112, 8)

convert_header(IMAGES + 'Tiles/Water_01.png',                                       BASE + 'Images.hpp', 'Water_01', 4, 8, 8)
convert_header(IMAGES + 'Tiles/Water_02.png',                                       BASE + 'Images.hpp', 'Water_02', 4, 16, 8)
convert_header(IMAGES + 'Tiles/Water_03.png',                                       BASE + 'Images.hpp', 'Water_03', 4, 24, 8)
convert_header(IMAGES + 'Tiles/Water_04.png',                                       BASE + 'Images.hpp', 'Water_04', 4, 32, 8)
convert_header(IMAGES + 'Tiles/Water_05.png',                                       BASE + 'Images.hpp', 'Water_05', 4, 40, 8)
convert_header(IMAGES + 'Tiles/Water_06.png',                                       BASE + 'Images.hpp', 'Water_06', 4, 48, 8)
convert_header(IMAGES + 'Tiles/Water_07.png',                                       BASE + 'Images.hpp', 'Water_07', 4, 56, 8)
convert_header(IMAGES + 'Tiles/Water_08.png',                                       BASE + 'Images.hpp', 'Water_08', 4, 64, 8)
convert_header(IMAGES + 'Tiles/Water_09.png',                                       BASE + 'Images.hpp', 'Water_09', 4, 72, 8)
convert_header(IMAGES + 'Tiles/Water_10.png',                                       BASE + 'Images.hpp', 'Water_10', 4, 80, 8)
convert_header(IMAGES + 'Tiles/Water_11.png',                                       BASE + 'Images.hpp', 'Water_11', 4, 88, 8)
convert_header(IMAGES + 'Tiles/Water_12.png',                                       BASE + 'Images.hpp', 'Water_12', 4, 96, 8)
convert_header(IMAGES + 'Tiles/Water_13.png',                                       BASE + 'Images.hpp', 'Water_13', 4, 104, 8)
convert_header(IMAGES + 'Tiles/Water_14.png',                                       BASE + 'Images.hpp', 'Water_14', 4, 112, 8)

convert_header(IMAGES + 'Numbers/Numbers_5x3_2D_WB.png',                            BASE + 'Images.hpp', 'Numbers_5x3_2D_WB', 4, 8, 8)

convert_header(IMAGES + 'Levels/Level_00.png',                                      BASE + 'Images.hpp', 'Level_Img_00', 4)
convert_header(IMAGES + 'Levels/Level_01.png',                                      BASE + 'Images.hpp', 'Level_Img_01', 4)
convert_header(IMAGES + 'Levels/Level_02.png',                                      BASE + 'Images.hpp', 'Level_Img_02', 4)
convert_header(IMAGES + 'Levels/Level_03.png',                                      BASE + 'Images.hpp', 'Level_Img_03', 4)
convert_header(IMAGES + 'Levels/Level_04.png',                                      BASE + 'Images.hpp', 'Level_Img_04', 4)
convert_header(IMAGES + 'Levels/Level_05.png',                                      BASE + 'Images.hpp', 'Level_Img_05', 4)
convert_header(IMAGES + 'Levels/Level_06.png',                                      BASE + 'Images.hpp', 'Level_Img_06', 4)
convert_header(IMAGES + 'Levels/Level_07.png',                                      BASE + 'Images.hpp', 'Level_Img_07', 4)
convert_header(IMAGES + 'Levels/Level_08.png',                                      BASE + 'Images.hpp', 'Level_Img_08', 4)
convert_header(IMAGES + 'Levels/Level_09.png',                                      BASE + 'Images.hpp', 'Level_Img_09', 4)
convert_header(IMAGES + 'Levels/Level_10.png',                                      BASE + 'Images.hpp', 'Level_Img_10', 4)
convert_header(IMAGES + 'Levels/Level_11.png',                                      BASE + 'Images.hpp', 'Level_Img_11', 4)
convert_header(IMAGES + 'Levels/Level_12.png',                                      BASE + 'Images.hpp', 'Level_Img_12', 4)
convert_header(IMAGES + 'Levels/Level_13.png',                                      BASE + 'Images.hpp', 'Level_Img_13', 4)
convert_header(IMAGES + 'Levels/Level_14.png',                                      BASE + 'Images.hpp', 'Level_Img_14', 4)
convert_header(IMAGES + 'Levels/Level_15.png',                                      BASE + 'Images.hpp', 'Level_Img_15', 4)
convert_header(IMAGES + 'Levels/Level_16.png',                                      BASE + 'Images.hpp', 'Level_Img_16', 4)
convert_header(IMAGES + 'Levels/Level_17.png',                                      BASE + 'Images.hpp', 'Level_Img_17', 4)
convert_header(IMAGES + 'Levels/Level_18.png',                                      BASE + 'Images.hpp', 'Level_Img_18', 4)
convert_header(IMAGES + 'Levels/Level_19.png',                                      BASE + 'Images.hpp', 'Level_Img_19', 4)
convert_header(IMAGES + 'Levels/Level_20.png',                                      BASE + 'Images.hpp', 'Level_Img_20', 4)
convert_header(IMAGES + 'Levels/Level_21.png',                                      BASE + 'Images.hpp', 'Level_Img_21', 4)
convert_header(IMAGES + 'Levels/Level_22.png',                                      BASE + 'Images.hpp', 'Level_Img_22', 4)
convert_header(IMAGES + 'Levels/Level_23.png',                                      BASE + 'Images.hpp', 'Level_Img_23', 4)
