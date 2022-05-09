#include "game.hpp"

bool player_can_see(uint8_t x, uint8_t y)
{
    uint8_t px = ents[0].x;
    uint8_t py = ents[0].y;
    int8_t dx = px - x;
    int8_t dy = py - y;
    return
        dx * dx + dy * dy < light_radius2() &&
        path_clear(px, py, x, y);
}

bool player_can_see_entity(uint8_t i)
{
    if(i == 0) return true;
    if(entity_is_invisible(i) && !wearing_uncursed_ring(RNG_SEE_INVIS))
        return false;
    return player_can_see(ents[i].x, ents[i].y);
}

bool path_clear(
    uint8_t x0, uint8_t y0,
    uint8_t x1, uint8_t y1)
{
    int8_t xdir = x0 < x1 ? 1 : x0 == x1 ? 0 : -1;
    int8_t ydir = y0 < y1 ? 1 : y0 == y1 ? 0 : -1;

    //if(tile_is_solid(x0, y0))
    //    x0 += xdir, y0 += ydir;

    uint8_t xdist = u8abs(x0 - x1);
    uint8_t ydist = u8abs(y0 - y1);
    int8_t xi = ydist / 2;
    int8_t yi = xdist / 2;

    while(x0 != x1 || y0 != y1)
    {
        if(tile_is_solid(x0, y0))
            return false;
        xi += xdist;
        yi += ydist;
        if(xi >= ydist)
            x0 += xdir, xi -= ydist;
        if(yi >= xdist)
            y0 += ydir, yi -= xdist;
    }

    return true;
}

void set_tile_explored(uint8_t x, uint8_t y)
{
    if(x >= MAP_W || y >= MAP_H) return;
    uint16_t const i = y / 8 * MAP_W + x;
    uint8_t const m = 1 << (y % 8);
    tfog[i] |= m;
}

void update_light()
{
    uint8_t px = ents[0].x;
    uint8_t py = ents[0].y;
    uint8_t r = light_radius();
    uint8_t ay = py - r;
    uint8_t ax = px - r;
    uint8_t by = py + r + 1;
    uint8_t bx = px + r + 1;
    for(uint8_t y = ay; y != by; ++y)
        for(uint8_t x = ax; x != bx; ++x)
            if(player_can_see(x, y))
                set_tile_explored(x, y);

    // set rooms explored
    for(uint8_t i = 0; i < num_rooms; ++i)
    {
        if(maps[map_index].got_rooms.test(i)) continue;
        room r = rooms[i];
        uint8_t bx = r.x + r.w();
        uint8_t by = r.y + r.h();
        for(uint8_t y = r.y; y < by; ++y)
            for(uint8_t x = r.x; x < bx; ++x)
                if(tile_is_explored(x, y) && r.inside(x, y))
                    maps[map_index].got_rooms.set(i);
    }
}
