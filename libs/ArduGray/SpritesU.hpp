/*
Options

    SPRITESU_IMPLEMENTATION
    SPRITESU_OVERWRITE
    SPRITESU_PLUSMASK
    SPRITESU_FX
    SPRITESU_RECT
*/

#pragma once

#if defined(SPRITESU_FX)
#include <ArduboyFX.h>
#else
using uint24_t = uint32_t;//__uint24;
#endif

#define SFC_READ true

struct SpritesU
{
#ifdef SPRITESU_OVERWRITE
    static void drawOverwrite(
        int16_t x, int16_t y, uint8_t const* image, uint16_t frame);
    static void drawOverwrite(
        int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t const* image);
#endif

#ifdef SPRITESU_PLUSMASK
    static void drawPlusMask(
        int16_t x, int16_t y, uint8_t const* image, uint16_t frame);
    static void drawPlusMask(
        int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t const* image);
#endif

#if defined(SPRITESU_OVERWRITE) || defined(SPRITESU_PLUSMASK)
    static void drawSelfMask(
        int16_t x, int16_t y, uint8_t const* image, uint16_t frame);
    static void drawSelfMask(
        int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t const* image);
#endif

#ifdef SPRITESU_FX
    static void drawOverwriteFX(
        int16_t x, int16_t y, uint24_t image, uint16_t frame);
    static void drawOverwriteFX(
        int16_t x, int16_t y, uint8_t w, uint8_t h, uint24_t image, uint16_t frame);
    static void drawPlusMaskFX(
        int16_t x, int16_t y, uint24_t image, uint16_t frame);
    static void drawPlusMaskFX(
        int16_t x, int16_t y, uint8_t w, uint8_t h, uint24_t image, uint16_t frame);
    static void drawSelfMaskFX(
        int16_t x, int16_t y, uint24_t image, uint16_t frame);
    static void drawSelfMaskFX(
        int16_t x, int16_t y, uint8_t w, uint8_t h, uint24_t image, uint16_t frame);
#endif

#ifdef SPRITESU_RECT
    // color: zero for BLACK, 1 for WHITE
    static void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);
    static void fillRect_i8(int8_t x, int8_t y, uint8_t w, uint8_t h, uint8_t color);
#endif

    static constexpr uint8_t MODE_OVERWRITE   = 0;
    static constexpr uint8_t MODE_PLUSMASK    = 1;
    static constexpr uint8_t MODE_SELFMASK    = 4;
    static constexpr uint8_t MODE_OVERWRITEFX = 2;
    static constexpr uint8_t MODE_PLUSMASKFX  = 3;
    static constexpr uint8_t MODE_SELFMASKFX  = 6;

    static void drawBasic(
        int16_t x, int16_t y, uint8_t w, uint8_t h,
        uint24_t image, uint16_t frame, uint8_t mode);
    static void drawBasicNoChecks(
        uint16_t w_and_h,
        uint24_t image, uint8_t mode,
        int16_t x, int16_t y);
};

#ifdef SPRITESU_IMPLEMENTATION

// from Mr. Blinky's ArduboyFX library
static __attribute__((always_inline)) uint8_t SpritesU_bitShiftLeftUInt8(uint8_t bit)
{
    return 1 << (bit & 7);
}


void SpritesU::drawBasic(
    int16_t x, int16_t y, uint8_t w, uint8_t h,
    uint24_t image, uint16_t frame, uint8_t mode)
{
    if(x >= 128) return;
    if(y >= 64)  return;
    if(x + w <= 0) return;
    if(y + h <= 0) return;
    
    uint8_t oldh = h;    
    
    if(frame != 0)
    {
        h >>= 3;
        if(mode & 1) h <<= 1;
        uint16_t tmp = h * w;
        image += uint24_t(tmp) * frame;
    }

    drawBasicNoChecks((uint16_t(oldh) << 8) | w, image, mode, x, y);
}



void SpritesU::drawBasicNoChecks(
    uint16_t w_and_h,
    uint24_t image, uint8_t mode,
    int16_t x, int16_t y)
{
    uint8_t* buf;
    uint8_t pages;
    uint8_t count;
    uint8_t buf_data;
    uint16_t image_data;
    uint8_t cols;
    uint8_t buf_adv;
    uint16_t image_adv;
    uint16_t shift_mask;
    uint8_t shift_coef;
    bool bottom;
    int8_t page_start;
    uint8_t w;
    uint16_t mask_data;

    {
    uint8_t h;
    uint8_t col_start;
    
    w = uint8_t(w_and_h);
    h = uint8_t(w_and_h >> 8);
    buf = Arduboy2Base::sBuffer;
    pages = h;
    
    col_start = uint8_t(x);
    bottom = false;
    cols = w;

    pages >>= 3;

    // precompute vertical shift coef and mask
    shift_coef = SpritesU_bitShiftLeftUInt8(y);
    if(mode & 4)
        shift_mask = 0xffff;
    else
        shift_mask = ~(0xff * shift_coef);

    // y /= 8 (round to -inf)
    y >>= 3;
    
    // clip against top edge
    page_start = int8_t(y);
    if(page_start < -1)
    {
        page_start = ~page_start;
        pages -= page_start;
        if(mode & 1) page_start <<= 1;
        image += (uint8_t)page_start * w;
        page_start = -1;
    }

    // clip against left edge
    if(x < 0)
    {
        cols += x;
        if(mode & 1) x *= 2;
        image -= x;
        col_start = 0;
    }

    // compute buffer start address
    buf_adv = 128;
    buf += page_start * buf_adv + col_start;

    // clip against right edge
    buf_adv -= col_start;
    if(cols >= buf_adv)
        cols = buf_adv;

    // clip against bottom edge
    buf_adv = 7;
    buf_adv -= page_start;
    if(buf_adv < pages)
    {
        pages = buf_adv;
        bottom = true;
    }
    buf_adv = 128;
    buf_adv -= cols;
    image_adv = w;
    if(!(mode & 2))
        image_adv -= cols;
    if(mode & 1)
        image_adv <<= 1;


    }

#ifdef SPRITESU_OVERWRITE
    if(mode == MODE_OVERWRITE)
    {
        uint8_t const* image_ptr = (uint8_t const*)image;

        if(page_start < 0)
        {
            buf += 128;
            count = cols;
            do
            {
                image_data = pgm_read_byte(image_ptr++);
                uint16_t t = (uint8_t)image_data * shift_coef;
                buf_data = *buf;
                buf_data &= uint8_t(shift_mask >> 8);
                buf_data |= uint8_t(t >> 8);
                *buf++ = buf_data;
            } while(--count != 0);
            --pages;
            buf -= cols;
            image_ptr += image_adv;
        }
        if(pages != 0)
        {
            uint8_t* bufn = buf + 128;
            do
            {
                count = cols;
                do
                {
                    image_data = pgm_read_byte(image_ptr++);
                    uint16_t t = (uint8_t)image_data * shift_coef;
                    buf_data = *buf;
                    buf_data &= uint8_t(shift_mask >> 0);
                    buf_data |= uint8_t(t >> 0);
                    *buf++ = buf_data;
                    buf_data = *bufn;
                    buf_data &= uint8_t(shift_mask >> 8);
                    buf_data |= uint8_t(t >> 8);
                    *bufn++ = buf_data;
                } while(--count != 0);
                buf += buf_adv;
                bufn += buf_adv;
                image_ptr += image_adv;
            } while(--pages != 0);
        }
        if(bottom)
        {
            do
            {
                image_data = pgm_read_byte(image_ptr++);
                uint16_t t = (uint8_t)image_data * shift_coef;
                buf_data = *buf;
                buf_data &= uint8_t(shift_mask >> 0);
                buf_data |= uint8_t(t >> 0);
                *buf++ = buf_data;
            }
            while(--cols != 0);
        }
    }
    else
#endif
#ifdef SPRITESU_PLUSMASK
    if(mode == MODE_PLUSMASK)
    {
        uint8_t const* image_ptr = (uint8_t const*)image;

        if(page_start < 0)
        {
            buf += 128;
            count = cols;
            do
            {
                image_data = pgm_read_byte(image_ptr++);
                mask_data = pgm_read_byte(image_ptr++);
                image_data = (uint8_t)image_data * shift_coef;
                mask_data = (uint8_t)mask_data * shift_coef;
                buf_data = *buf;
                buf_data &= ~uint8_t(mask_data >> 8);
                buf_data |= uint8_t(image_data >> 8);
                *buf++ = buf_data;
            } while(--count != 0);
            --pages;
            buf -= cols;
            image_ptr += image_adv;
        }
        if(pages != 0)
        {
            uint8_t* bufn = buf + 128;
            do
            {
                count = cols;
                do
                {
                    image_data = pgm_read_byte(image_ptr++);
                    mask_data = pgm_read_byte(image_ptr++);
                    image_data = (uint8_t)image_data * shift_coef;
                    mask_data = (uint8_t)mask_data * shift_coef;
                    buf_data = *buf;
                    buf_data &= ~uint8_t(mask_data >> 0);
                    buf_data |= uint8_t(image_data >> 0);
                    *buf++ = buf_data;
                    buf_data = *bufn;
                    buf_data &= ~uint8_t(mask_data >> 8);
                    buf_data |= uint8_t(image_data >> 8);
                    *bufn++ = buf_data;
                } while(--count != 0);
                buf += buf_adv;
                bufn += buf_adv;
                image_ptr += image_adv;
            } while(--pages != 0);
        }
        if(bottom)
        {
            do
            {
                image_data = pgm_read_byte(image_ptr++);
                mask_data = pgm_read_byte(image_ptr++);
                image_data = (uint8_t)image_data * shift_coef;
                mask_data = (uint8_t)mask_data * shift_coef;
                buf_data = *buf;
                buf_data &= ~uint8_t(mask_data >> 0);
                buf_data |= uint8_t(image_data >> 0);
                *buf++ = buf_data;
            }
            while(--cols != 0);
        }
    }
    else
#endif
#ifdef SPRITESU_FX
    {
        uint8_t sfc_read = SFC_READ;
        uint8_t* bufn;
        uint8_t reseek;
        
        reseek = false;
        FX::seekData(image);
        
        if(page_start < 0)
        {
            // top
            buf += 128;
            count = cols;
            if(!(mode & 1))
            {
                do
                {
                    image_data = FX::readPendingUInt8();
                    image_data = (uint8_t)image_data * shift_coef;
                    buf_data = *buf;
                    buf_data &= uint8_t(shift_mask >> 8);
                    buf_data |= uint8_t(image_data >> 8);
                    *buf++ = buf_data;
                } while(--count != 0);
            }
            else
            {
                do
                {
                    image_data = FX::readPendingUInt8();
                    image_data = (uint8_t)image_data * shift_coef;
                    shift_mask = FX::readPendingUInt8();
                    shift_mask = (uint8_t)shift_mask * shift_coef;
                    buf_data = *buf;
                    buf_data &= ~uint8_t(shift_mask >> 8);
                    buf_data |= uint8_t(image_data >> 8);
                    *buf++ = buf_data;
                } while(--count != 0);
            }
            --pages;
            buf -= cols;
            reseek = (w != cols);
        }
        
        if(pages != 0)
        {
        
            do
            {
                if(reseek)
                {
                    (void)FX::readEnd();
                    image += image_adv;
                    FX::seekData(image);
                }
                reseek = (w != cols);
                
                bufn = buf + 128;
                count = cols;
                if(!(mode & 1))
                {
                    do
                    {
                        image_data = FX::readPendingUInt8();
                        image_data = (uint8_t)image_data * shift_coef;
                        buf_data = *buf;
                        buf_data &= uint8_t(shift_mask >> 0);
                        buf_data |= uint8_t(image_data >> 0);
                        *buf++ = buf_data;
                        buf_data = *bufn;
                        buf_data &= uint8_t(shift_mask >> 8);
                        buf_data |= uint8_t(image_data >> 8);
                        *bufn++ = buf_data;
                    } while(--count != 0);
                }
                else
                {
                    do
                    {
                        image_data = FX::readPendingUInt8();
                        image_data = (uint8_t)image_data * shift_coef;
                        shift_mask = FX::readPendingUInt8();
                        shift_mask = (uint8_t)shift_mask * shift_coef;
                        buf_data = *buf;
                        buf_data &= ~uint8_t(shift_mask >> 0);
                        buf_data |= uint8_t(image_data >> 0);
                        *buf++ = buf_data;
                        buf_data = *bufn;
                        buf_data &= ~uint8_t(shift_mask >> 8);
                        buf_data |= uint8_t(image_data >> 8);
                        *bufn++ = buf_data;
                    } while(--count != 0);
                }
                buf += buf_adv;
            } while(--pages != 0);
        }
        
        if(bottom)
        {
            if(reseek)
            {
                (void)FX::readEnd();
                image += image_adv;
                FX::seekData(image);
            }
            
            if(!(mode & 1))
            {
                do
                {
                    image_data = FX::readPendingUInt8();
                    image_data = (uint8_t)image_data * shift_coef;
                    buf_data = *buf;
                    buf_data &= uint8_t(shift_mask >> 0);
                    buf_data |= uint8_t(image_data >> 0);
                    *buf++ = buf_data;
                } while(--cols != 0);
            }
            else
            {
                do
                {
                    image_data = FX::readPendingUInt8();
                    image_data = (uint8_t)image_data * shift_coef;
                    shift_mask = FX::readPendingUInt8();
                    shift_mask = (uint8_t)shift_mask * shift_coef;
                    buf_data = *buf;
                    buf_data &= ~uint8_t(shift_mask >> 0);
                    buf_data |= uint8_t(image_data >> 0);
                    *buf++ = buf_data;
                } while(--cols != 0);
            }
        }
    
        (void)FX::readEnd();
    }
#endif
    {} // empty final else block, if needed
}

#ifdef SPRITESU_OVERWRITE
void SpritesU::drawOverwrite(
    int16_t x, int16_t y, uint8_t const* image, uint16_t frame)
{
    uint8_t w, h;

    w = pgm_read_byte(image++);
    h = pgm_read_byte(image++);
    drawBasic(x, y, w, h, (uint24_t)image, frame, MODE_OVERWRITE);
}

void SpritesU::drawOverwrite(
    int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t const* image)
{
    drawBasic(x, y, w, h, (uint24_t)image, 0, MODE_OVERWRITE);
}
#endif

#ifdef SPRITESU_PLUSMASK
void SpritesU::drawPlusMask(
    int16_t x, int16_t y, uint8_t const* image, uint16_t frame)
{
    uint8_t w, h;

    w = pgm_read_byte(image++);
    h = pgm_read_byte(image++);

    drawBasic(x, y, w, h, (uint24_t)image, frame, MODE_PLUSMASK);
}
void SpritesU::drawPlusMask(
    int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t const* image)
{
    drawBasic(x, y, w, h, (uint24_t)image, 0, MODE_PLUSMASK);
}
#endif

#if defined(SPRITESU_OVERWRITE) || defined(SPRITESU_PLUSMASK)
void SpritesU::drawSelfMask(
    int16_t x, int16_t y, uint8_t const* image, uint16_t frame)
{
    uint8_t w, h;

    w = pgm_read_byte(image++);
    h = pgm_read_byte(image++);
    drawBasic(x, y, w, h, (uint24_t)image, frame, MODE_SELFMASK);
}
void SpritesU::drawSelfMask(
    int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t const* image)
{
    drawBasic(x, y, w, h, (uint24_t)image, 0, MODE_SELFMASK);
}
#endif

#ifdef SPRITESU_FX
void SpritesU::drawOverwriteFX(
    int16_t x, int16_t y, uint24_t image, uint16_t frame)
{
    FX::seekData(image);
    uint8_t w = FX::readPendingUInt8();
    uint8_t h = FX::readEnd();
    drawBasic(x, y, w, h, image + 2, frame, MODE_OVERWRITEFX);
}
void SpritesU::drawOverwriteFX(
    int16_t x, int16_t y, uint8_t w, uint8_t h, uint24_t image, uint16_t frame)
{
    drawBasic(x, y, w, h, image + 2, frame, MODE_OVERWRITEFX);
}
void SpritesU::drawPlusMaskFX(
    int16_t x, int16_t y, uint24_t image, uint16_t frame)
{
    FX::seekData(image);
    uint8_t w = FX::readPendingUInt8();
    uint8_t h = FX::readEnd();
    drawBasic(x, y, w, h, image + 2, frame, MODE_PLUSMASKFX);
}
void SpritesU::drawPlusMaskFX(
    int16_t x, int16_t y, uint8_t w, uint8_t h, uint24_t image, uint16_t frame)
{
    drawBasic(x, y, w, h, image + 2, frame, MODE_PLUSMASKFX);
}
void SpritesU::drawSelfMaskFX(
    int16_t x, int16_t y, uint24_t image, uint16_t frame)
{
    FX::seekData(image);
    uint8_t w = FX::readPendingUInt8();
    uint8_t h = FX::readEnd();
    drawBasic(x, y, w, h, image + 2, frame, MODE_SELFMASKFX);
}
void SpritesU::drawSelfMaskFX(
    int16_t x, int16_t y, uint8_t w, uint8_t h, uint24_t image, uint16_t frame)
{
    drawBasic(x, y, w, h, image + 2, frame, MODE_SELFMASKFX);
}
#endif

#ifdef SPRITESU_RECT
void SpritesU::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color)
{
    if(x >= 128) return;
    if(x + w <= 0) return;
    if(y + h <= 0) return;
    fillRect_i8((int8_t)x, (int8_t)y, w, h, color);
}

// from Mr. Blinky's ArduboyFX library
static __attribute__((always_inline)) uint8_t SpritesU_bitShiftLeftMaskUInt8(uint8_t bit)
{
    return (0xFF << (bit & 7)) & 0xFF;
}

void SpritesU::fillRect_i8(int8_t x, int8_t y, uint8_t w, uint8_t h, uint8_t color)
{
    if(w == 0 || h == 0) return;
    if(y >= 64)  return;
    if(x + w <= 0) return;
    if(y + h <= 0) return;

    if(color & 1) color = 0xff;

    // clip coords
    uint8_t xc = x;
    uint8_t yc = y;

    // TODO: extreme clipping behavior

    // clip
    if(y < 0)
        h += y, yc = 0;
    if(x < 0)
        w += x, xc = 0;
    if(h >= uint8_t(64 - yc))
        h = 64 - yc;
    if(w >= uint8_t(128 - xc))
        w = 128 - xc;
    uint8_t y1 = yc + h;

    uint8_t c0 = SpritesU_bitShiftLeftMaskUInt8(yc); // 11100000
    uint8_t m1 = SpritesU_bitShiftLeftMaskUInt8(y1); // 11000000
    uint8_t m0 = ~c0; // 00011111
    uint8_t c1 = ~m1; // 00111111

    uint8_t r0 = yc;
    uint8_t r1 = y1 - 1;

    r0 >>= 3;
    r1 >>= 3;

    uint8_t* buf = Arduboy2Base::sBuffer;

    buf += r0 * 128 + xc;

    uint8_t rows = r1 - r0; // middle rows + 1
    uint8_t f = 0;
    uint8_t bot = c1;
    if(m0  == 0) ++rows; // no top fragment
    if(bot == 0) ++rows; // no bottom fragment
    c0 &= color;
    c1 &= color;

    uint8_t col;
    uint8_t buf_adv = 128 - w;

    if(rows == 0)
    {
        m1 |= m0;
        c1 &= c0;
    }
    else
    {
        if(m0 != 0)
        {
            col = w;
            do
            {
                uint8_t t = *buf;
                t &= m0;
                t |= c0;
                *buf++ = t;
            } while(--col != 0);
            buf += buf_adv;
        }
        
        if(--rows != 0)
        {
            do
            {
                col = w;
                do
                {
                    *buf++ = color;
                } while(--col != 0);
                buf += buf_adv;
            } while(--rows != 0);
        }
    }
    
    if(bot)
    {
        do
        {
            uint8_t t = *buf;
            t &= m1;
            t |= c1;
            *buf++ = t;
        } while(--w != 0);
    }
    
}
#endif

#endif
