#include "game.hpp"

static constexpr uint8_t MORE_WIDTH =
    2 + 3 + 4 + 4 + 3 + 4 + 2;

static inline bool is_white(char c)
{
    return c <= ' ';
}

uint8_t NOINLINE advance_white(char* b, uint8_t i)
{
    while(!is_white(b[i])) ++i;
    b[i] = '\0';
    return i + 1;
}

void draw_status()
{
    uint8_t sx = 1, sy = STATUS_START_Y;
    uint8_t n = 0;
    while(n < statusn)
    {
        uint8_t w = text_width_nonprog(&statusbuf[n]);
        if(sx + w > 64)
            sx = 1, sy += 6;
        draw_text_nonprog(sx, sy, &statusbuf[n]);
        sx += w + SPACE_WIDTH + 1;
        while(statusbuf[n++] != '\0')
            ;
    }
}

void NOINLINE status_cursed_amulet()
{
    status_simple(PSTR2("Your cursed " STRI_AMULET "burns at you."));
}

void NOINLINE reset_status()
{
    statusn = 0;
    statusx = 1;
    statusy = STATUS_START_Y;
}

void status_more()
{
    draw_dungeon_at_player();
    paint_left();
    draw_info();
    draw_text(65 - MORE_WIDTH, 59, PSTR("[more]"));
    paint_right();
    while(wait_btn() != BTN_A)
        ;
    reset_status();
}

NOINLINE void status_simple(char const* s)
{
    status(s);
}

NOINLINE void status_i(char const* fmt, item it)
{
    status(fmt, it.to_u16());
}

NOINLINE void status_si(char const* s, char const* p, item it)
{
    status(s, p, it.to_u16());
}

NOINLINE void status_u(char const* fmt, uint8_t a)
{
    status(fmt, a);
}

NOINLINE void status_usu(char const* fmt, uint8_t a, char const* b, uint8_t c)
{
    status(fmt, a, b, c);
}

NOINLINE void status_you_are_no_longer(char const* s)
{
    status(PSTR2(STRI_YOU_ARE "no longer @p."), s);
}


void status(char const* fmt, ...)
{
    char buf[128];
    va_list ap;
    va_start(ap, fmt);
    tvsprintf(buf, fmt, ap);
    va_end(ap);
    uint8_t len = tstrlen(buf);
    uint8_t a = 0, b = advance_white(buf, 0);
    for(;;)
    {
        if(uint8_t(statusn + b - a) > sizeof(statusbuf))
            status_more();
        uint8_t n = text_width_nonprog(&buf[a]);
        if(statusy >= 59 && statusx + n > 64 - MORE_WIDTH)
            status_more();
        for(uint8_t i = a; i < b; ++i)
            statusbuf[statusn++] = buf[i];
        if(statusx + n > 64)
            statusx = 1, statusy += 6;
        statusx += n + SPACE_WIDTH + 1;
        if(b >= len) break;
        a = b;
        b = advance_white(buf, a);
    }
}
