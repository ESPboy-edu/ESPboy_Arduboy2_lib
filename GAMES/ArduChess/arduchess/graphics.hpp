#pragma once

#include "vars.hpp"
#include "font.hpp"
#include "save.hpp"
#include "lib/ESPboyInit.h"

extern ESPboyInit myESPboy;

// piece image data: 7 wide x 8 tall
static uint8_t const PIECE_IMGS[][7] PROGMEM =
{
  { 0x00, 0x68, 0x7c, 0x7c, 0x7c, 0x68, 0x00 }, // white pawn
  { 0x00, 0x00, 0x14, 0x00, 0x14, 0x00, 0x00 }, // checkerboard
  { 0x18, 0x1c, 0x4a, 0x7e, 0x7e, 0x7c, 0x70 }, // white knight
  { 0x20, 0x4c, 0x7e, 0x37, 0x7a, 0x4c, 0x20 }, // white bishop
  { 0x46, 0x7c, 0x7e, 0x7c, 0x7e, 0x7c, 0x46 }, // white rook
  { 0x43, 0x64, 0x7d, 0x7e, 0x7d, 0x64, 0x43 }, // white queen
  { 0x44, 0x68, 0x7a, 0x7f, 0x7a, 0x68, 0x44 }, // white king
  { 0x00, 0x68, 0x54, 0x44, 0x54, 0x68, 0x00 }, // black pawn
  { 0x18, 0x14, 0x4a, 0x72, 0x42, 0x4c, 0x70 }, // black knight
  { 0x20, 0x4c, 0x72, 0x21, 0x72, 0x4c, 0x20 }, // black bishop
  { 0x46, 0x7c, 0x46, 0x44, 0x46, 0x7c, 0x46 }, // black rook
  { 0x43, 0x64, 0x5d, 0x46, 0x5d, 0x64, 0x43 }, // black queen
  { 0x44, 0x68, 0x5a, 0x4f, 0x5a, 0x68, 0x44 }, // black king  
};

static uint8_t const SMOOTHERSTEP[16] PROGMEM =
{
   0,  0,  1,  3,  7, 12, 19, 26,
  34, 42, 48, 54, 59, 62, 64, 64,
};

const uint8_t* get_piece_img(ch2k::piece pc)
{
  if(pc.color().is_white())
    return PIECE_IMGS[pc.type().x - 1];
  else
    return PIECE_IMGS[pc.type().x + 5];
}

static void draw_pixel(uint8_t y, uint8_t x)
{
  uint8_t m = 1 << (y & 7);
  buf[(uint16_t(y & 0xf8) << 3) + x] |= m;
}

static void draw_hline(uint8_t y, uint8_t x0, uint8_t x1)
{
  uint8_t m = 1 << (y & 7);
  uint8_t* p = buf + (uint16_t(y & 0xf8) << 3) + x0; 
  while(x0++ <= x1)
    *p++ |= m;
}

static void draw_vline(uint8_t y0, uint8_t y1, uint8_t x)
{
  uint8_t* p = buf + ((y0 & 0xf8) << 3) + x;
  *p |= (0xff << (y0 & 7));
  p += 64;
  y0 &= 0xf8;
  y0 += 8;
  while((y0 += 8) <= y1 + 1)
  {
    *p = 0xff;
    p += 64;
  }
  *p |= ((1 << ((y1 + 1) & 7)) - 1);
}

static void draw_piece_aligned(uint8_t const* i, uint8_t* p)
{
  *p++ = pgm_read_byte(i++);
  *p++ = pgm_read_byte(i++);
  *p++ = pgm_read_byte(i++);
  *p++ = pgm_read_byte(i++);
  *p++ = pgm_read_byte(i++);
  *p++ = pgm_read_byte(i++);
  *p   = pgm_read_byte(i);
}

static void draw_piece_unaligned(
  uint8_t const* i,
  uint8_t* p,
  uint8_t y,
  uint8_t x)
{
  if(!(y & 0x7))
    draw_piece_aligned(i, p + (uint16_t(y) << 3) + x);
  else
  {
    p += uint16_t(y & 0xf8) << 3;
    p += x;
    uint8_t f0 = y & 0x7;
    uint8_t f1 = 8-f0;
    uint8_t m0 = 0xfe << f0;
    uint8_t m1 = 0xff >> f1;
    for(uint8_t n = 0; n < 7; ++n)
    {
      uint8_t ip = pgm_read_byte(i++);
      uint8_t ip0 = ip << f0;
      uint8_t ip1 = ip >> f1;
      p[0] = (p[0] & ~m0) | ip0;
      p[64] = (p[64] & ~m1) | ip1;
      ++p;
    }
  }
}

static void render_board()
{
  // pieces and checkerboard
  uint8_t* p = buf;
  for(uint8_t r = 0; r < 8; ++r)
  {
    for(uint8_t c = 0; c < 8; ++c)
    {
      uint8_t rt = rotated ? 7 - r : r;
      uint8_t ct = rotated ? 7 - c : c;
      ch2k::piece pc = b[rt][ct];
      if(!pc.is_nothing())
        draw_piece_aligned(get_piece_img(pc), p);
      else if(!((r + c) & 1))
        draw_piece_aligned(PIECE_IMGS[1], p);
      p += 8;
    }
  }
}

// render animated move
static void render_anim()
{
  uint8_t x = nframe & 0xf;
  uint8_t y;
  uint8_t ax = (ta & 7) * 8, bx = (tb & 7) * 8;
  uint8_t ay = (ta >> 4) * 8, by = (tb >> 4) * 8;
  if(rotated)
  {
    ax = 56 - ax, bx = 56 - bx;
    ay = 56 - ay, by = 56 - by;
  }
  x = pgm_read_byte(&SMOOTHERSTEP[x]);
  y = uint8_t((uint16_t(x) * (by - ay) + 32) >> 6) + ay;
  x = uint8_t((uint16_t(x) * (bx - ax) + 32) >> 6) + ax;
  draw_piece_unaligned(
    get_piece_img(ch2k::piece{ tc }),
    buf, y, x);
}


static uint8_t const CURSOR_P[32] PROGMEM =
 {
     uint8_t(0-1), uint8_t(0-1), uint8_t(0-1), uint8_t(0-1),
     uint8_t(0-1), uint8_t(0-1), uint8_t(0-1), uint8_t(0-1),
     uint8_t(0-1), uint8_t(1-1), uint8_t(2-1), uint8_t(3-1),
     uint8_t(4-1), uint8_t(5-1), uint8_t(6-1), uint8_t(7-1),
     uint8_t(8-1), uint8_t(8-1), uint8_t(8-1), uint8_t(8-1),
     uint8_t(8-1), uint8_t(8-1), uint8_t(8-1), uint8_t(8-1),
     uint8_t(8-1), uint8_t(7-1), uint8_t(6-1), uint8_t(5-1),
     uint8_t(4-1), uint8_t(3-1), uint8_t(2-1), uint8_t(1-1),
 };

static void render_cursor_ex(uint8_t y, uint8_t x, bool solid)
{
    for(uint8_t i = 0; i < 32; ++i)
    {
        uint8_t yp = y + pgm_read_byte(&CURSOR_P[i]);
        uint8_t xp = x + pgm_read_byte(&CURSOR_P[(i + 8) & 0x1f]);
        if(yp < 63 && xp < 63 && (solid || ((i + nframe) & 4)))
            draw_pixel(yp, xp);
    }
}

static void render_cursor(uint8_t y, uint8_t x)
{
    render_cursor_ex(y, x, false);
}

static void render_solid_cursor(uint8_t y, uint8_t x)
{
    render_cursor_ex(y, x, true);
}

static void render_title_text()
{
  uint8_t const* msg = (uint8_t const*)pgm_read_dword(&MSGS_TITLE[ta]);
  uint8_t w = small_text_width_prog(msg, 255);
  draw_small_text_prog(msg, 255, 20, 21 - (w / 2));
}

static void draw_pretty_box(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2)
{
    draw_hline(y1, x1 + 1, x2 - 1);
    draw_hline(y2, x1 + 1, x2 - 1);
    draw_vline(y1 + 1, y2 - 1, x1);
    draw_vline(y1 + 1, y2 - 1, x2);
    draw_pixel(y1 + 1, x1 + 2);
    draw_pixel(y1 + 1, x2 - 2);
    draw_pixel(y2 - 1, x1 + 2);
    draw_pixel(y2 - 1, x2 - 2);
    draw_pixel(y1 + 2, x1 + 1);
    draw_pixel(y1 + 2, x2 - 1);
    draw_pixel(y2 - 2, x1 + 1);
    draw_pixel(y2 - 2, x2 - 1);
}

static void render_game_info_side(ch2k::piece_color c, uint8_t yo)
{
  constexpr uint8_t giw = 63;
  constexpr uint8_t gih = 13;
  uint8_t ci = c.binary_index();

  // draw boxes
  draw_pretty_box(yo, 0, yo + gih - 1, giw - 1);

  // draw current turn indicator
  if(state != STATE_GAME_OVER && turn == ci)
  {
    uint8_t y = yo + 2;
    for(uint8_t i = 3; i < giw-3; i += 2)
    {
      draw_pixel(y, i);
      draw_pixel(y+gih-5, i);
    }
    for(uint8_t i = 1; i < 8; i += 2)
    {
      draw_pixel(y+i, 2);
      draw_pixel(y+i, giw-3);
    }
  }

  // draw ai face and info
  if(ailevel[ci] != 0)
  {
    uint8_t t = SF_BIGFROWN + aihappy[ci];
    draw_small_text(&t, 1, yo+4, 4);
    uint8_t a[3];
    a[0] = SF_L; a[1] = SF_V; a[2] = SF_1 - 1 + ailevel[ci];
    draw_small_text(a, 3, yo+4, 11);
  }
  else
  {
    draw_small_text_prog(MSG_HUMAN, 5, yo+4, 4);
  }
  if(!(turn == ci && (ailevel[ci] != 0) && state == STATE_AI_START))
    draw_small_text_prog(ci ? MSG_BLACK : MSG_WHITE, 5, yo+4, 35);
}

static void render_game_info()
{
    render_game_info_side(ch2k::piece_color::W, rotated ? 0 : 50);
    render_game_info_side(ch2k::piece_color::B, rotated ? 50 : 0);
    
    uint8_t y = 14 + 6 * 5;
    uint16_t move_counter = (g.gd.ply_ + 1) / 2;
    if(move_counter < MOVEHIST_SIZE)
        y -= (MOVEHIST_SIZE - (uint8_t)move_counter) * 6;
    else if(state == STATE_GAME_OVER)
        y -= 6;
    uint8_t t[8];
    for(uint8_t i = 0; y >= 14; ++i)
    {
        if(i >= g.gd.ply_) break;
        uint8_t x, x2;
        get_san_from_hist(t, g.get_rep_move(i), sanhist[i]);
        if(((i ^ g.gd.ply_) & 1) != 0)
        {
            x = (small_text_width(t, 7) + 0) >> 1;
            if(x > 12) x = 12;
                x = 26 - x;
            draw_small_text(t, 7, y, x);
            y -= 6;
        }
        else
        {
            x = small_text_width(t, 7);
            x2 = 52 - ((x + 1) >> 1);
            if(x2 + x > 64)
                x2 = 64 - x;
            draw_small_text(t, 7, y, x2);
        }
    }
    if(state == STATE_GAME_OVER)
    {
        if(game_status == ch2k::game::STATUS_MATED)
            draw_small_text_prog(MSG_MATE, 9, 44, 0);
        else if(game_status >= ch2k::game::STATUS_DRAW_50MOVE)
        {
            draw_small_text_prog(MSG_DRAW, 5, 44, 0);
            switch(game_status)
            {
            case ch2k::game::STATUS_DRAW_50MOVE:
                draw_small_text_prog(MSG_50M, 8, 44, 22); break;
            case ch2k::game::STATUS_DRAW_REPETITION:
                draw_small_text_prog(MSG_REP, 10, 44, 22); break;
            case ch2k::game::STATUS_DRAW_MATERIAL:
                draw_small_text_prog(MSG_MAT, 8, 44, 22); break;
            case ch2k::game::STATUS_DRAW_STALEMATE:
                draw_small_text_prog(MSG_STALE, 9, 44, 22); break;
            default: break;
            }
        }
    }
    if(g.gd.ply_ == 0) return;
    uint8_t n[3];
    {
        n[2] = SF_0 + move_counter % 10;
        uint16_t mc2 = move_counter / 10;
        n[1] = SF_0 + mc2 % 10;
        n[0] = SF_0 + mc2 / 10;
    }
    y = 14 + 6 * 5;
    if(move_counter < MOVEHIST_SIZE)
        y -= (MOVEHIST_SIZE - (uint8_t)move_counter) * 6;
    else if(state == STATE_GAME_OVER)
        y -= 6;
    for(uint8_t i = 0; y >= 14; ++i)
    {
        t[0] = n[0], t[1] = n[1], t[2] = n[2];
        if(t[0] == SF_0)
        {
            t[0] = SF_SPACE3;
            if(t[1] == SF_0) t[1] = SF_SPACE3;
        }
        draw_small_text(t, 3, y, 0);
        if(n[2] == SF_0)
        {
            n[2] = SF_9;
            if(n[1] == SF_0)
            {
                n[1] = SF_9;
                if(n[0] == SF_0) n[0] = SF_9;
                else --n[0];
            }
            else --n[1];
        }
        else --n[2];
        y -= 6;
    }
}

static void render_ai_progress()
{
  constexpr uint8_t xoff = 25;
  constexpr uint8_t h = 3;
  uint8_t level = ailevel[turn]*2 + 2;
  uint8_t progress = uint8_t(g.nodes_ >> level);
  uint8_t offset = ((!turn) ^ rotated) ? 55 : 5;
  for(uint8_t i = 0; i < h; ++i)
    draw_hline(i + offset, xoff, xoff + progress);
  draw_hline(offset - 1, xoff, xoff + 32);
  draw_hline(offset + h, xoff, xoff + 32);
  draw_vline(offset, offset + h-1, xoff - 1);
  draw_vline(offset, offset + h-1, xoff + 32 + 1);
}

static void render_promotion_menu()
{
  uint8_t* p;
  p = buf + 64*2 + 12;
  for(uint8_t i = 0; i <= 38; ++i) *p++ &= 0x0f;
  p = buf + 64*3 + 12;
  for(uint8_t i = 0; i <= 38; ++i) *p++ = 0;
  p = buf + 64*4 + 12;
  for(uint8_t i = 0; i <= 38; ++i) *p++ &= 0xf8;
  draw_pretty_box(20, 12, 34, 50);
  uint8_t po = g.gd.c_.is_white() ? 0 : 6;
  draw_piece_aligned(PIECE_IMGS[po + 2], buf + 64 * 3 + 16);
  draw_piece_aligned(PIECE_IMGS[po + 3], buf + 64 * 3 + 24);
  draw_piece_aligned(PIECE_IMGS[po + 4], buf + 64 * 3 + 32);
  draw_piece_aligned(PIECE_IMGS[po + 5], buf + 64 * 3 + 40);
}

static void render_new_game_menu()
{
    draw_pretty_box(0, 0, 14, 62);
    draw_small_text_prog(MSG_T_NEW, 8, 5, 15);
    draw_small_text_prog(MSG_P_BACK, 4, 18, 3);
    draw_small_text_prog(MSG_START_GAME, 10, 26, 3);
    draw_small_text_prog(MSG_WHITE, 6, 34, 3);
    draw_small_text_prog(MSG_BLACK, 6, 50, 3);
    for(uint8_t i = 0; i < 2; ++i)
    {
        uint8_t j = i * 16;
        draw_small_text_prog(
            (uint8_t const*)pgm_read_dword(&MSGS_PLAYER[ailevel[i]]),
            255, 34 + j, 34);
        //uint8_t const* ct = (uint8_t const*)pgm_read_dword(&MSGS_CONTEMPT[aiblunder[i]]);
        //uint8_t cw = small_text_width_prog(ct, 255);
        //draw_small_text_prog(ct, 255, 42 + j, 60 - cw);
        draw_small_text_prog(MSG_BLUNDER, 8, 42 + j, 7);
        uint8_t const* ct = (uint8_t const*)pgm_read_dword(&MSGS_BLUNDER[aiblunder[i]]);
        draw_small_text_prog(ct, 255, 42 + j, 42);
        if(ailevel[i] == 0)
        {
            uint8_t* b = &buf[(i * 2 + 5) * 64];
            for(uint8_t k = 7; k < 61; ++k)
                b[k] &= 0x55;
        }
    }
    uint8_t* b = &buf[ta * 64 + 64*2];
    uint8_t x1 = 33, x2 = 61;
    if(ta == 0) x1 = 2, x2 = 19;
    if(ta == 1) x1 = 2, x2 = 45;
    if(ta == 3 || ta == 5) x1 = 41;
    for(uint8_t i = x1; i < x2; ++i)
        b[i] ^= 0xfe;
}

static void render_game_paused()
{
  draw_pretty_box(0, 0, 14, 62);
  if(game_status > ch2k::game::STATUS_CHECK)
    draw_small_text_prog(MSG_GAME_OVER, 9, 5, 12);
  else
    draw_small_text_prog(MSG_GAME_PAUSED, 11, 5, 9);
  static constexpr uint8_t const yo = 18;
  static constexpr uint8_t const xo = 11;
  for(uint8_t n = 0, y = yo; n < NUM_MSGS_PAUSE; ++n, y += 8)
  {
      uint8_t const* t = (uint8_t const*)pgm_read_dword(&MSGS_PAUSE[n]);
      draw_small_text_prog(t, 255, y, xo);
  }
  uint8_t* b = &buf[ta * 64 + 64*2];
  for(uint8_t i = xo - 1; i < xo + 42; ++i)
    b[i] ^= 0xfe;
}

static void render_save_game_title()
{
    draw_pretty_box(0, 0, 14, 62);
    draw_small_text_prog(tc ? MSG_T_LOAD : MSG_P_SAVE, 9, 5, 12);
}

static void render_save_game()
{
    render_save_game_title();
    static constexpr uint8_t const yo = 18;
    static constexpr uint8_t const xo = 20;
    uint8_t check = SF_CHECKMARK;
    draw_small_text_prog(MSG_P_BACK, 4, yo, xo);
    for(uint8_t n = 0, y = yo + 8; n < NUM_SAVE_FILES; ++n, y += 8)
    {
        draw_small_text_prog(MSG_P_SAVE, 4, y, xo);
        uint8_t c = SF_1 + n;
        draw_small_text(&c, 1, y, xo + 18);
        if(save_is_valid(n))
            draw_small_text(&check, 1, y, xo - 6);
    }
    uint8_t* b = &buf[ta * 64 + 64*2];
    for(uint8_t i = xo - 1; i < xo + 22; ++i)
        b[i] ^= 0xfe;
}

static void render_save_game_board()
{
    // render save file board
    bool valid = ta > 0 && save_is_valid(ta - 1);
    if(valid)
    {
        update_board_cache_from_save(ta - 1);
    }
    else
    {
        for(uint8_t i = 0; i < 64; ++i)
            ((ch2k::piece*)b)[i] = ch2k::piece::NOTHING;
    }
    render_board();
}

static void render_save_game_overwrite()
{
    render_save_game_title();
    static constexpr uint8_t const yo = 18;
    static constexpr uint8_t const xo = 12;
    draw_small_text_prog(MSG_OVERWRITE_SAVE, 15, yo, 1);
    draw_small_text_prog(MSG_OVERWRITE_SAVE, 9, yo + 8, xo);
    draw_small_text_prog(MSG_CANCEL, 6, yo + 16, xo);
    uint8_t* b = &buf[tb * 64 + 64*3];
    for(uint8_t i = xo - 1; i < xo + 39; ++i)
        b[i] ^= 0xfe;
}

static void render_exit_confirm()
{
    draw_pretty_box(0, 0, 30, 62);
    draw_small_text_prog(MSG_EXIT_CONFIRM1, 8, 4, 16);
    draw_small_text_prog(MSG_P_MAIN, 9, 10, 11);
    draw_small_text_prog(MSG_EXIT_CONFIRM2, 7, 16, 16);
    draw_small_text_prog(MSG_EXIT_CONFIRM3, 7, 22, 17);
    draw_small_text_prog(MSG_P_BACK, 4, 34, 11);
    draw_small_text_prog(MSG_P_MAIN, 9, 42, 11);
    uint8_t* b = &buf[ta * 64 + 64*4];
    for(uint8_t i = 10; i < 53; ++i)
        b[i] ^= 0xfe;
}

static void clear_buf()
{
  for(auto& i : buf) i = 0;
}

static void paint_half(bool side, uint8_t *b, bool clear)
{
  static uint16_t oBuffer[WIDTH/2*HEIGHT] __attribute__ ((aligned));

  if (!side) myESPboy.tft.setAddrWindow(0, 20, WIDTH/2, HEIGHT);
  else myESPboy.tft.setAddrWindow(64, 20, WIDTH/2, HEIGHT);

  static uint8_t currentDataByte;
  static uint16_t foregroundColor, backgroundColor, xPos, yPos, addr;

  foregroundColor = TFT_YELLOW;
  backgroundColor = TFT_BLACK;

  for (xPos = 0; xPos < WIDTH/2; xPos++) {
    for (yPos = 0; yPos < HEIGHT; yPos++) {    
      if (!(yPos % 8)) currentDataByte = b[xPos + (yPos>>3) * WIDTH/2];
        addr =  yPos*WIDTH/2+xPos;
        if (currentDataByte & 0x01) oBuffer[addr] = foregroundColor;
        else oBuffer[addr] = backgroundColor;
      currentDataByte = currentDataByte >> 1;
    }
    }
    wdt_reset();
    myESPboy.tft.pushColors(oBuffer, WIDTH/2*HEIGHT);

    if(clear) memset(b, 0, WIDTH/2*HEIGHT/8);
}

static void paint_left_half(bool clear)
{
  paint_half(0, buf, clear);
}

static void paint_right_half(bool clear)
{
  paint_half(1, buf, clear);
}
