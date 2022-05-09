#include "game.hpp"

static uint16_t const ENTITY_IMGS[] PROGMEM =
{
    0x0000, // none
    0x6ff6, // player
    0x0fa4, // bat
    0x0bd0, // snake
    0x0f5a, // rattlesnake
    0x9db9, // zombie
    0x0bf0, // goblin
    0x0f52, // phantom
    0x0f9f, // orc
    0x07a0, // tarantula
    0x0f2c, // hobgoblin
    0x0f2f, // mimic
    0x09f9, // incubus
    0x01f1, // troll
    0x069d, // griffin
    0xf996, // dragon
    0x0e5e, // fallen angel
    0x0f88, // Lord of Darkness
    0x6996, // invis player
};

static uint16_t const ITEM_IMGS[] PROGMEM =
{
    0x0000, // none
    0x9429, // food
    0x0bb0, // potion
    0x01b3, // scroll
    0x8421, // arrow
    0x1248, // wand
    0x0960, // bow
    0x04f4, // sword
    0x0aaa, // ring
    0x0606, // amulet
    0x0f90, // armor
    0x0f90, // helm
    0x0f90, // boots
};

static uint16_t const DOOR_IMGS[2] PROGMEM = { 0xefbe, 0xe11e };

static void custom_pixel(uint8_t x, uint8_t y, uint8_t c)
{
    if(x < 64 && y < 64)
    {
        uint8_t& b = buf[(uint16_t(y << 3) & 0xffc0) | x];
        uint8_t m = ymask(y);
        uint8_t t = b;
        if(c == 0)
            t |= m;
        if(c == 1)
            t &= ~m;
        if(c == 2)
            t ^= m;
        b = t;
    }
}

void set_pixel(uint8_t x, uint8_t y)
{
    custom_pixel(x, y, 0);
}

void clear_pixel(uint8_t x, uint8_t y)
{
    custom_pixel(x, y, 1);
}

void inv_pixel(uint8_t x, uint8_t y)
{
    custom_pixel(x, y, 2);
}

void set_hline(uint8_t x0, uint8_t x1, uint8_t y)
{
    ++x1;
    do set_pixel(x0, y);
    while(++x0 != x1);
}

void clear_hline(uint8_t x0, uint8_t x1, uint8_t y)
{
    ++x1;
    do clear_pixel(x0, y);
    while(++x0 != x1);
}

void inv_hline(uint8_t x0, uint8_t x1, uint8_t y)
{
    ++x1;
    do inv_pixel(x0, y);
    while(++x0 != x1);
}

void set_vline(uint8_t x, uint8_t y0, uint8_t y1)
{
    ++y1;
    do set_pixel(x, y0);
    while(++y0 != y1);
}

void clear_vline(uint8_t x, uint8_t y0, uint8_t y1)
{
    ++y1;
    do clear_pixel(x, y0);
    while(++y0 != y1);
}

void set_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
    ++y1;
    do set_hline(x0, x1, y0);
    while(++y0 != y1);
}

void clear_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
    ++y1;
    do clear_hline(x0, x1, y0);
    while(++y0 != y1);
}

void inv_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
    ++y1;
    do inv_hline(x0, x1, y0);
    while(++y0 != y1);
}

void set_box(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
    set_hline(x0, x1, y0);
    set_hline(x0, x1, y1);
    set_vline(x0, y0, y1);
    set_vline(x1, y0, y1);
}

void draw_box_pretty(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
    clear_rect(x0 - 1, x1 + 2, y0 - 1, y1 + 2);
    set_box(x0, x1, y0, y1);
    set_hline(x0 + 1, x1 + 1, y1 + 1);
    set_vline(x1 + 1, y0 + 1, y1 + 1);
}

static void set_img(
    uint8_t const* p, // NOT PROGMEM
    uint8_t w,
    uint8_t x, uint8_t y)
{
    for(uint8_t i = 0; i < w; ++i, ++x)
    {
        for(uint8_t tp = p[i], ty = y; tp; tp >>= 1, ++ty)
            if(tp & 1) set_pixel(x, ty);
    }
}

NOINLINE void set_img_prog(uint8_t const* p, uint8_t w, uint8_t x, uint8_t y)
{
    uint8_t tp[8];
    for(uint8_t i = 0; i < w; ++i)
        tp[i] = pgm_read_byte(&p[i]);
    set_img(tp, w, x, y);
}

static void clear_img(
    uint8_t const* p, // NOT PROGMEM
    uint8_t w,
    uint8_t x, uint8_t y)
{
    for(uint8_t i = 0; i < w; ++i, ++x)
    {
        for(uint8_t tp = p[i], ty = y; tp; tp >>= 1, ++ty)
            if(tp & 1) clear_pixel(x, ty);
    }
}

static void draw_tile(
    uint8_t const* p, // PROGMEM, 5 set + 5 clear
    uint8_t x, uint8_t y)
{
    for(uint8_t i = 0; i < 5; ++i, ++x)
    {
        uint8_t t = pgm_read_byte(&p[i]);
        for(uint8_t ty = y; t; t >>= 1, ++ty)
            if(t & 1) set_pixel(x, ty);
        t = pgm_read_byte(&p[i + 5]);
        for(uint8_t ty = y; t; t >>= 1, ++ty)
            if(t & 1) clear_pixel(x, ty);
    }
}

static void draw_sprite_precise_nonprog(
    uint16_t tp,
    uint8_t x, uint8_t y)
{
    uint8_t tt[4];
    tt[0] = (tp >> 12) & 0xf;
    tt[1] = (tp >> 8) & 0xf;
    tt[2] = (tp >> 4) & 0xf;
    tt[3] = (tp >> 0) & 0xf;
    for(uint8_t i = 0; i < 8; ++i)
    {
        auto c = ddircoord(i);
        clear_img(tt, 4, x + c.x, y + c.y);
    }
    set_img(tt, 4, x, y);
}

static void draw_sprite_precise(
    uint16_t const* p, // PROGMEM
    uint8_t x, uint8_t y)
{
    uint16_t tp = pgm_read_word(p);
    draw_sprite_precise_nonprog(tp, x, y);
}

static NOINLINE void draw_sprite_nonprog(
    uint16_t tp,
    uint8_t x, uint8_t y)
{
    draw_sprite_precise_nonprog(tp, x * 5, y * 5);
}

static NOINLINE void draw_sprite(
    uint16_t const* p, // PROGMEM
    uint8_t x, uint8_t y)
{
    draw_sprite_precise(p, x * 5, y * 5);
}

void draw_info()
{
    draw_info_without_status();
    draw_status();
}

void draw_info_without_status()
{
    draw_textf(1, 0, PSTR("Dungeon Level @u"), map_index + 1);
    if(num_rooms == 0) return;
    draw_textf(1, 6, PSTR("LV @u   HP: @u/@u"), plevel + 1, healths[0], entity_max_health(0));
    {
        char const* s = STR_EMPTY;
        if(hunger == 255)
            s = STR_STARVING;
        else if(hunger > 200)
            s = STR_HUNGRY;
        draw_text(1, 12, s);
        set_hline(1, 63, 20);
    }
#if 0
    // for debug: how many entities there are
    // useful for debugging spawning frequency
    {
        uint8_t n = 0;
        for(uint8_t i = 1; i < MAP_ENTITIES; ++i)
            if(ents[i].type != entity::NONE)
                ++n;
        draw_textf(56, 12, PSTR("@u"), n);
    }
#endif
}

int8_t const DDIRX[16] PROGMEM =
{
    -1, +1, -1, +1, +1, -1, 0, 0,
    -1, -1, +1, +1, 0, 0, +1, -1,
};

static uint8_t ddir_mask(uint8_t tx, uint8_t ty)
{
    uint8_t i = 0;
    for(uint8_t j = 0; j < 8; ++j)
    {
        i <<= 1;
        auto c = ddircoord(j);
        i |= (uint8_t)tile_is_solid_or_unknown(tx + c.x, ty + c.y);
    }
    return i;
}

#if ENABLE_MINIMAP
void draw_map_offset(uint8_t ox)
{
    for(uint8_t y = 0, t = 0; y < 64; y += 2, t ^= 1)
        for(uint8_t x = t; x < 64; x += 2)
            set_pixel(x, y);

    for(uint8_t y = 0, py = 0; y < MAP_H; ++y, py += 2)
    {
        for(uint8_t tx = 0, px = 0; tx < 32; ++tx, px += 2)
        {
            uint8_t x = tx + ox;
            if(tile_is_solid_or_unknown(x, y))
            {
                uint8_t m;
                if(tile_is_solid(x, y) && (m = ddir_mask(x, y)) != 0xff)
                {
                    clear_rect(px, px + 2, py - 1, py + 1);
                    set_pixel(px + 1, py);

                    static uint8_t const TESTS[] PROGMEM =
                    {
                        0x09, 0x01, 0x06, 0x04, 0x05, 0x01, 0x05, 0x04,
                        0x49, 0x09, 0x26, 0x06, 0x85, 0x05, 0x85, 0x05,
                    };

                    uint8_t i = 16;
                    do
                    {
                        i -= 4;
                        if((m & pgm_read_byte(&TESTS[i + 0])) == pgm_read_byte(&TESTS[i + 1]))
                            set_pixel(px + 1, py - 1);
                        if((m & pgm_read_byte(&TESTS[i + 2])) == pgm_read_byte(&TESTS[i + 3]))
                            set_pixel(px, py);
                    } while(i != 0);
                }
            }
            else
            {
                clear_hline(px, px + 1, py);
            }
        }
    }

    // draw doors
    for(uint8_t i = 0; i < num_doors; ++i)
    {
        auto d = doors[i];
        if(d.secret | d.open) continue;
        uint8_t x = d.x, y = d.y;
        if(!tile_is_explored(x, y)) continue;
        uint8_t px = (x - ox) * 2, py = y * 2;
        clear_rect(px, px + 2, py - 1, py + 1);
        set_pixel(px + 1, py);
    }

#if 0
    // draw items
    for(auto const& mit : items)
    {
        if(mit.it.type == item::NONE) continue;
        if(!tile_is_explored(mit.x, mit.y)) continue;
        uint8_t px = (mit.x - ox) * 2, py = mit.y * 2;
        set_pixel(px + 1, py + 1);
    }
#endif
    
    // draw stairs
    if(tile_is_explored(xdn, ydn))
    {
        uint8_t tx = (xdn - ox) * 2, ty = ydn * 2;
        set_hline(tx, tx + 1, ty + 1);
        set_pixel(tx, ty);
    }
    if(tile_is_explored(xup, yup))
    {
        uint8_t tx = (xup - ox) * 2, ty = yup * 2;
        set_hline(tx, tx + 1, ty + 1);
        set_pixel(tx + 1, ty);
    }
}
#endif

void draw_dungeon(uint8_t mx, uint8_t my)
{
    mx -= 6;
    my -= 6;

    static uint8_t const TILES[] PROGMEM =
    {
        0x01, 0x04, 0x10, 0x02, 0x08, // unexplored
        0x00, 0x00, 0x00, 0x00, 0x00,

        // sides
        0x01, 0x01, 0x01, 0x01, 0x01, // S
        0x02, 0x02, 0x02, 0x02, 0x02,
        0x08, 0x08, 0x08, 0x08, 0x08, // N
        0x14, 0x14, 0x14, 0x14, 0x14,
        0x1f, 0x00, 0x00, 0x00, 0x00, // E
        0x00, 0x06, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x1f, 0x00, // W
        0x00, 0x00, 0x16, 0x00, 0x1f,

        // corners
        0x00, 0x00, 0x00, 0x18, 0x08, // NW
        0x00, 0x00, 0x1c, 0x04, 0x14,
        0x18, 0x00, 0x00, 0x00, 0x00, // NE
        0x04, 0x1c, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x01, 0x01, // SW
        0x00, 0x00, 0x03, 0x02, 0x02,
        0x01, 0x00, 0x00, 0x00, 0x00, // SE
        0x02, 0x03, 0x00, 0x00, 0x00,
    };

    for(uint8_t y = 0; y < 13; ++y)
    {
        for(uint8_t x = 0; x < 13; ++x)
        {
            uint8_t tx = x + mx;
            uint8_t ty = y + my;
            uint8_t px = x * 5;
            uint8_t py = y * 5;

            if(tile_is_unknown(tx, ty))
            {
                draw_tile(&TILES[0], px, py);
                continue;
            }
            uint8_t i = ddir_mask(tx, ty);
            if(!tile_is_solid_or_unknown(tx, ty))
            {
                uint8_t ws = wall_style;
                if(player_can_see(tx, ty))
                    set_pixel(px + 2, py + 2 + (ws == 3));
                if(tile_is_solid(tx, ty - 1))
                {
                    uint8_t a = px - 1;
                    uint8_t b = px + 4;
                    if(!(i & 0x80)) ++a;
                    if(!(i & 0x40)) --b;
                    a += (a >> 7);
                    for(uint8_t j = 0; j < ws; ++j)
                        set_hline(a, b, py - 1 + j);
                    clear_hline(a, b, py - 1 + ws);
                }
                continue;
            }

            draw_tile(&TILES[0], px, py);

            static uint8_t const TESTS[16] PROGMEM =
            {
                0x01, 0x02, 0x04, 0x08, 0x1a, 0x26, 0x49, 0x85, // masks
                0x00, 0x00, 0x00, 0x00, 0x0a, 0x06, 0x09, 0x05, // tests
            };
            for(uint8_t ti = 0, pi = 10; ti < 8; ++ti, pi += 10)
            {
                uint8_t mask = pgm_read_byte(&TESTS[ti + 0]);
                uint8_t test = pgm_read_byte(&TESTS[ti + 8]);
                if((i & mask) == test)
                    draw_tile(&TILES[pi], px, py);
            }
        }
    }

    // draw doors
    for(uint8_t i = 0; i < num_doors; ++i)
    {
        auto const& d = doors[i];
        if(d.secret) continue;
        uint8_t x = d.x, y = d.y;
        uint8_t dx = x - mx;
        uint8_t dy = y - my;
        if(dx >= 13 || dy >= 13) continue;
        if(tile_is_explored(x, y) ||
            (!tile_is_solid(x, y + 1) && tile_is_explored(x, y + 1)))
        {
            uint8_t oy = 0;
            uint8_t px = dx * 5;
            uint8_t py = dy * 5;
            if(!tile_is_solid_or_unknown(x, y + 1))
                oy = wall_style;
            clear_rect(px == 0 ? 0 : px - 1, px + 4, py, py + 4 + oy);
            draw_sprite(&DOOR_IMGS[d.open], dx, dy);
            if(tile_is_solid(x - 1, d.y))
            {
                set_vline(px - 2, py, py + 3);
                set_vline(px + 5, py, py + 3);
            }
            else
            {
                set_hline(px + 1, px + 2, py - 2);
                set_hline(px + 1, px + 2, py + 5);
            }
        }
    }

    // draw stairs
    {
        uint8_t dx, dy;
        dx = xdn - mx;
        dy = ydn - my;
        if(dx < 13 && dy < 13 && tile_is_explored(xdn, ydn))
            draw_sprite_nonprog(0xfec8, dx, dy);
        dx = xup - mx;
        dy = yup - my;
        if(dx < 13 && dy < 13 && tile_is_explored(xup, yup))
            draw_sprite_nonprog(0x8cef, dx, dy);
    }

    // draw items
    for(auto const& i : items)
    {
        if(i.it.is_nothing()) continue;
        uint8_t ix = i.x - mx;
        uint8_t iy = i.y - my;
        if(ix >= 13 || iy >= 13) continue;
        if(tile_is_explored(i.x, i.y))
            draw_sprite(&ITEM_IMGS[i.it.type], ix, iy);
    }

    // draw entities
    for(uint8_t i = 0; i < MAP_ENTITIES; ++i)
    {
        auto const& e = ents[i];
        if(e.type == entity::NONE) continue;
        uint8_t ex = e.x - mx;
        uint8_t ey = e.y - my;
        if(ex >= 13 || ey >= 13) continue;
        if(!tile_is_explored(e.x, e.y)) continue;
        uint16_t const* img = &ENTITY_IMGS[0];
        if(e.type == entity::PLAYER && player_is_invisible())
            img = &ENTITY_IMGS[entity::DARKNESS + 1];
        else if(e.type == entity::MIMIC && !e.aggro)
            img = &ITEM_IMGS[healths[i]];
        else if(player_can_see_entity(index_of_entity(e)))
            img = &ENTITY_IMGS[e.type];
        draw_sprite(img, ex, ey);
    }

    // draw generate message
    if(num_rooms == 0)
    {
        draw_box_pretty(10, 52, 24, 40);
        draw_text(17, 30, PSTR("Loading..."));
    }
}

void draw_dungeon_at_player()
{
    draw_dungeon(ents[0].x, ents[0].y);
}

void draw_sprite_nonprog_rel_and_wait(uint16_t tp, uint8_t x, uint8_t y)
{
    x = x - ents[0].x + 6;
    y = y - ents[0].y + 6;
    draw_sprite_nonprog(tp, x, y);
    paint_left_no_clear();
    wait();
}

void draw_ray_anim(uint8_t x, uint8_t y, uint8_t d, uint8_t n)
{
    auto c = dircoord(d);
    draw_dungeon_at_player();
    while(n-- > 0)
    {
        x += c.x;
        y += c.y;
        draw_sprite_nonprog_rel_and_wait(0x0eae, x, y);
    }
    paint_left();
}
