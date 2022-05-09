#include "game.hpp"

static constexpr uint8_t ROOM_BIG_CHANCE = 64;

// chances are out of 256
// chance to place a door
static constexpr uint8_t DOOR_CHANCE = 196;
// chance for any door to be secret
static constexpr uint8_t DOOR_SECRET_CHANCE = 48;

static constexpr uint8_t RANDOM_DOOR_SPACE = 4;

static constexpr uint8_t ITEM_CURSED_CHANCE = 32;

struct map_gen_info
{
    uint8_t monster_types[6];
    item guaranteed_item;
};

static map_gen_info const MAP_GEN_INFOS[NUM_MAPS] PROGMEM =
{
{ // level 1
    { entity::BAT, entity::SNAKE, entity::SNAKE, },
    item::make(item::FOOD), // extra food
},
{ // level 2
    { entity::SNAKE, entity::SNAKE, entity::SNAKE, entity::SNAKE, entity::RATTLESNAKE, entity::RATTLESNAKE, },
    item::make(item::SCROLL, SCR_REMOVE_CURSE),
},
{ // level 3
    { entity::ZOMBIE, entity::ZOMBIE, entity::ZOMBIE, entity::GOBLIN, entity::GOBLIN, entity::PHANTOM },
    item::make(item::SCROLL, SCR_ENCHANT),
},
{ // level 4
    { entity::ZOMBIE, entity::GOBLIN, entity::GOBLIN, entity::PHANTOM, entity::ORC, },
    item::make(item::SCROLL, SCR_REMOVE_CURSE),
},
{ // level 5
    { entity::PHANTOM, entity::PHANTOM, entity::PHANTOM, entity::PHANTOM, entity::PHANTOM, entity::PHANTOM, },
    item::make(item::POTION, POT_STRENGTH),
},
{ // level 6
    { entity::GOBLIN, entity::GOBLIN, entity::GOBLIN, entity::ORC, entity::HOBGOBLIN, },
    item::make(item::SCROLL, SCR_ENCHANT),
},
{ // level 7
    { entity::ORC, entity::ORC, entity::HOBGOBLIN, entity::TARANTULA, entity::MIMIC },
    item::make(item::POTION, POT_POISON),
},
{ // level 8
    { entity::ORC, entity::HOBGOBLIN, entity::TARANTULA, entity::TARANTULA, entity::TARANTULA, entity::MIMIC },
    item::make(item::SCROLL, SCR_REMOVE_CURSE),
},
{ // level 9
    { entity::HOBGOBLIN, entity::HOBGOBLIN, entity::HOBGOBLIN, entity::TARANTULA, entity::MIMIC, entity::INCUBUS },
    item::make(item::SCROLL, SCR_ENCHANT),
},
{ // level 10
    { entity::MIMIC, entity::MIMIC, entity::MIMIC, entity::MIMIC, entity::TARANTULA, entity::HOBGOBLIN, },
    item::make(item::POTION, POT_STRENGTH),
},
{ // level 11
    { entity::TARANTULA, entity::HOBGOBLIN, entity::MIMIC, entity::INCUBUS, entity::INCUBUS, entity::TROLL, },
    item::make(item::POTION, POT_CONFUSION),
},
{ // level 12
    { entity::HOBGOBLIN, entity::MIMIC, entity::INCUBUS, entity::TROLL, entity::TROLL, entity::GRIFFIN, },
    item::make(item::SCROLL, SCR_ENCHANT),
},
{ // level 13
    { entity::MIMIC, entity::INCUBUS, entity::TROLL, entity::GRIFFIN, entity::GRIFFIN, entity::DRAGON, },
    item::make(item::POTION, POT_CONFUSION),
},
{ // level 14
    { entity::INCUBUS, entity::TROLL, entity::GRIFFIN, entity::DRAGON, entity::DRAGON, entity::DRAGON, },
    item::make(item::POTION, POT_INVIS),
},
{ // level 15
    { entity::INCUBUS, entity::ANGEL, entity::ANGEL, entity::DRAGON, entity::DRAGON, entity::DRAGON, },
    item::make(item::SCROLL, SCR_ENCHANT),
},
{ // level 16
    { entity::INCUBUS, entity::INCUBUS, entity::ANGEL, entity::ANGEL, entity::ANGEL, entity::ANGEL, },
    item::make(item::POTION, POT_CONFUSION),
},
};

struct room_info
{
    uint8_t w;
    uint8_t h;
    uint8_t mask[8];
};

struct big_room_info
{
    uint8_t w;
    uint8_t h;
    uint8_t mask[32];
};

static constexpr uint8_t cbm(uint32_t x)
{
    return
        (((x & 0x00000001) ? 1 : 0) << 7) +
        (((x & 0x00000010) ? 1 : 0) << 6) +
        (((x & 0x00000100) ? 1 : 0) << 5) +
        (((x & 0x00001000) ? 1 : 0) << 4) +
        (((x & 0x00010000) ? 1 : 0) << 3) +
        (((x & 0x00100000) ? 1 : 0) << 2) +
        (((x & 0x01000000) ? 1 : 0) << 1) +
        (((x & 0x10000000) ? 1 : 0) << 0);
}

static constexpr big_room_info const BIG_ROOM_TYPES[] PROGMEM =
{
{
    15, 15, {
    cbm(0x11111000), cbm(0x00111111),
    cbm(0x11100000), cbm(0x00001111),
    cbm(0x11000000), cbm(0x00000111),
    cbm(0x10000000), cbm(0x00000011),
    cbm(0x10000000), cbm(0x00000011),
    cbm(0x00000111), cbm(0x11000001),
    cbm(0x00000100), cbm(0x01000001),
    cbm(0x00000100), cbm(0x01000001),
    cbm(0x00000100), cbm(0x01000001),
    cbm(0x00000110), cbm(0x11000001),
    cbm(0x10000000), cbm(0x00000011),
    cbm(0x10000000), cbm(0x00000011),
    cbm(0x11000000), cbm(0x00000111),
    cbm(0x11100000), cbm(0x00001111),
    cbm(0x11111000), cbm(0x00111111),
    cbm(0x11111111), cbm(0x11111111), }
},
{
    16, 16, {
    cbm(0x11111000), cbm(0x00011111),
    cbm(0x11100000), cbm(0x00000111),
    cbm(0x11000000), cbm(0x00000011),
    cbm(0x10000000), cbm(0x00000001),
    cbm(0x10001100), cbm(0x00110001),
    cbm(0x00001100), cbm(0x00110000),
    cbm(0x00000000), cbm(0x00000000),
    cbm(0x00000000), cbm(0x00000000),
    cbm(0x00000000), cbm(0x00000000),
    cbm(0x00000000), cbm(0x00000000),
    cbm(0x00001100), cbm(0x00110000),
    cbm(0x10001100), cbm(0x00110001),
    cbm(0x10000000), cbm(0x00000001),
    cbm(0x11000000), cbm(0x00000011),
    cbm(0x11100000), cbm(0x00000111),
    cbm(0x11111000), cbm(0x00011111), }
},
{
    13, 13, {
    cbm(0x11111101), cbm(0x11111111),
    cbm(0x11111000), cbm(0x11111111),
    cbm(0x11110000), cbm(0x01111111),
    cbm(0x11100000), cbm(0x00111111),
    cbm(0x11000000), cbm(0x00011111),
    cbm(0x10000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00000111),
    cbm(0x10000000), cbm(0x00001111),
    cbm(0x11000000), cbm(0x00011111),
    cbm(0x11100000), cbm(0x00111111),
    cbm(0x11110000), cbm(0x01111111),
    cbm(0x11111000), cbm(0x11111111),
    cbm(0x11111101), cbm(0x11111111),
    cbm(0x11111111), cbm(0x11111111),
    cbm(0x11111111), cbm(0x11111111),
    cbm(0x11111111), cbm(0x11111111), }
},
{
    12, 12, {
    cbm(0x11000000), cbm(0x00111111),
    cbm(0x10000000), cbm(0x00011111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x00000000), cbm(0x00001111),
    cbm(0x10000000), cbm(0x00011111),
    cbm(0x11000000), cbm(0x00111111),
    cbm(0x11111111), cbm(0x11111111),
    cbm(0x11111111), cbm(0x11111111),
    cbm(0x11111111), cbm(0x11111111),
    cbm(0x11111111), cbm(0x11111111), }
},
};
static constexpr uint8_t NUM_BIG_ROOM_TYPES =
sizeof(BIG_ROOM_TYPES) / sizeof(BIG_ROOM_TYPES[0]);

static constexpr room_info const ROOM_TYPES[] PROGMEM =
{
{
    4, 4, {
    cbm(0x00001111),
    cbm(0x00001111),
    cbm(0x00001111),
    cbm(0x00001111),
    cbm(0x11111111),
    cbm(0x11111111),
    cbm(0x11111111),
    cbm(0x11111111), },
},
{
    5, 5, {
    cbm(0x00000111),
    cbm(0x00000111),
    cbm(0x00000111),
    cbm(0x00000111),
    cbm(0x00000111),
    cbm(0x11111111),
    cbm(0x11111111),
    cbm(0x11111111), },
},
{
    6, 6, {
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x11111111),
    cbm(0x11111111), },
},
{
    6, 6, {
    cbm(0x10000111),
    cbm(0x00000011),
    cbm(0x00110011),
    cbm(0x00110011),
    cbm(0x00000011),
    cbm(0x10000111),
    cbm(0x11111111),
    cbm(0x11111111), },
},
{
    6, 6, {
    cbm(0x10000111),
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x00000011),
    cbm(0x10000111),
    cbm(0x11111111),
    cbm(0x11111111), },
},
{
    7, 7, {
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x11111111), },
},
{
    7, 7, {
    cbm(0x11101111),
    cbm(0x11000111),
    cbm(0x10000011),
    cbm(0x00000001),
    cbm(0x10000011),
    cbm(0x11000111),
    cbm(0x11101111),
    cbm(0x11111111), },
},
{
    7, 7, {
    cbm(0x11000111),
    cbm(0x11000111),
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x00000001),
    cbm(0x11000111),
    cbm(0x11000111),
    cbm(0x11111111), },
},
{
    7, 7, {
    cbm(0x11000111),
    cbm(0x11000111),
    cbm(0x00010001),
    cbm(0x00111001),
    cbm(0x00010001),
    cbm(0x11000111),
    cbm(0x11000111),
    cbm(0x11111111), },
},
{
    8, 8, {
    cbm(0x11000011),
    cbm(0x10000001),
    cbm(0x00000000),
    cbm(0x00000000),
    cbm(0x00000000),
    cbm(0x00000000),
    cbm(0x10000001),
    cbm(0x11000011), },
},
{
    8, 8, {
    cbm(0x11100111),
    cbm(0x11100111),
    cbm(0x11000011),
    cbm(0x00000000),
    cbm(0x00000000),
    cbm(0x11000011),
    cbm(0x11100111),
    cbm(0x11100111), },
},
{
    8, 8, {
    cbm(0x00000000),
    cbm(0x01100110),
    cbm(0x01000010),
    cbm(0x00000000),
    cbm(0x00000000),
    cbm(0x01000010),
    cbm(0x01100110),
    cbm(0x00000000), },
},
};
static constexpr uint8_t NUM_ROOM_TYPES =
sizeof(ROOM_TYPES) / sizeof(ROOM_TYPES[0]);

static uint8_t random_room_type()
{
    if(u8rand() < ROOM_BIG_CHANCE)
        return 0x80 + u8rand(NUM_BIG_ROOM_TYPES);
    return u8rand(NUM_ROOM_TYPES);
}

uint8_t room::w() const
{
    if(type & 0x80) return pgm_read_byte(&BIG_ROOM_TYPES[type & 0x7f].w);
    return pgm_read_byte(&ROOM_TYPES[type].w);
}

uint8_t room::h() const
{
    if(type & 0x80) return pgm_read_byte(&BIG_ROOM_TYPES[type & 0x7f].h);
    return pgm_read_byte(&ROOM_TYPES[type].h);
}

bool room::solid(uint8_t rx, uint8_t ry) const
{
    if(type & 0x80)
    {
        uint8_t i = ry * 2 + rx / 8;
        uint8_t m = pgm_read_byte(&BIG_ROOM_TYPES[type & 0x7f].mask[i]);
        return (m & (1 << (rx & 7))) != 0;
    }
    uint8_t m = pgm_read_byte(&ROOM_TYPES[type].mask[ry]);
    return (m & (1 << rx)) != 0;
}

bool room::inside(uint8_t tx, uint8_t ty) const
{
    uint8_t rx = uint8_t(tx - x);
    uint8_t ry = uint8_t(ty - y);
    if(!(rx < w() && ry < h()))
        return false;
    return !solid(rx, ry);
}

NOINLINE bool room::inside_bb(uint8_t tx, uint8_t ty) const
{
    uint8_t rx = uint8_t(tx - x);
    uint8_t ry = uint8_t(ty - y);
    return (rx < w() && ry < h());
}

void dig_nonsecret_door_tiles()
{
    for(int i = 0; i < num_doors; ++i)
    {
        auto d = doors[i];
        if(!d.secret)
            dig_tile(d.x, d.y);
    }
}

void update_doors()
{
    for(int i = 0; i < num_doors; ++i)
    {
        auto d = doors[i];
        if(d.open)
            dig_tile(d.x, d.y);
        else
            fill_tile(d.x, d.y);
    }
}

static bool dig_room(
    uint8_t type, uint8_t x, uint8_t y)
{
    auto& r = rooms[num_rooms];
    r.x = x;
    r.y = y;
    r.type = type;

    if(x >= MAP_W || y >= MAP_H) return false;
    uint8_t bx = x + r.w();
    uint8_t by = y + r.h();
    if(bx > MAP_W || by > MAP_H) return false;

    // ensure room can fit here
    for(uint8_t ty = y; ty < by; ++ty)
        for(uint8_t tx = x; tx < bx; ++tx)
        {
            if(!r.inside(tx, ty))
                continue;
            for(uint8_t i = 0; i < 8; ++i)
            {
                auto c = ddircoord(i);
                if(!tile_is_solid(tx + c.x, ty + c.y))
                    return false;
            }
        }

    bool explored = maps[map_index].got_rooms.test(num_rooms);
    for(uint8_t iy = y; iy < by; ++iy)
        for(uint8_t ix = x; ix < bx; ++ix)
            if(r.inside(ix, iy))
            {
                dig_tile(ix, iy);
                if(explored)
                    set_tile_explored(ix, iy);
            }
    ++num_rooms;
    return true;
}

static void dig_initial_room()
{
    uint8_t t = random_room_type();
    if(map_index == NUM_MAPS - 1)
        t = 0x80;
    dig_room(t, MAP_W / 2 - 3, MAP_H / 2 - 3);
}

static void add_door(uint8_t x, uint8_t y)
{
    dig_tile(x, y);
    if(u8rand() >= DOOR_CHANCE) return;
    if(num_doors >= MAP_DOORS) return;
    if(get_door(x, y) != nullptr) return;
    auto& d = doors[num_doors];
    uint8_t t = u8rand();
    if(maps[map_index].got_doors.test(num_doors))
    {
        set_tile_explored(x, y);
        d.open = 1;
    }
    else if(t < DOOR_SECRET_CHANCE)
    {
        d.secret = 1;
        set_tile_explored(x, y);
    }
    d.x = x;
    d.y = y;
    ++num_doors;
}

static void random_room_edge(uint8_t d, uint8_t t, uint8_t& rx, uint8_t& ry)
{
    room r = { 0, 0, t };
    for(;;)
    {
        r.x = u8rand(r.w());
        r.y = u8rand(r.h());
        switch(d)
        {
        case 0: r.y = 0;         break;
        case 1: r.y = r.h() - 1; break;
        case 2: r.x = 0;         break;
        case 3: r.x = r.w() - 1; break;
        default: break;
        }
        if(!r.solid(r.x, r.y)) break;
    }
    rx = r.x, ry = r.y;
}

int8_t const DIRX[8] PROGMEM =
{
    0, 0, -1, 1, -1, 1, 0, 0
};

static bool try_generate_room()
{
    if(num_rooms >= MAP_ROOMS) return false;
    room pr = rooms[u8rand(num_rooms)];
    uint8_t d = u8rand() % 4; // direction to place new room
    uint8_t t = random_room_type(); // new room type
    uint8_t x0, y0; // current room edge (rel)
    uint8_t x1, y1; // new room edge (rel)
    uint8_t xd, yd; // door pos
    uint8_t xr, yr; // new room
    random_room_edge(d, pr.type, x0, y0);
    random_room_edge(d ^ 1, t, x1, y1);
    auto c = dircoord(d);
    xd = pr.x + x0 + c.x;
    yd = pr.y + y0 + c.y;
    xr = xd - x1 + c.x;
    yr = yd - y1 + c.y;
    if(!dig_room(t, xr, yr))
        return false;
    add_door(xd, yd);
    return true;
}

static void try_add_random_door()
{
    auto r = rooms[u8rand(num_rooms)];
    uint8_t d = u8rand() % 4;
    uint8_t x, y;
    random_room_edge(d, r.type, x, y);
    {
        auto c = dircoord(d);
        x += r.x + c.x;
        y += r.y + c.y;
    }
    if(!tile_is_solid(x, y)) return;
    for(uint8_t i = 0; i < num_rooms; ++i)
        if(rooms[i].inside_bb(x, y)) return;
    // verify not adjacent to another door
    uint8_t m = 0;
    for(uint8_t i = 0; i < 4; ++i)
    {
        auto c = dircoord(i);
        uint8_t tx = x + c.x;
        uint8_t ty = y + c.y;
        if(get_door(tx, ty))
            return;
        m <<= 1;
        m |= (uint8_t)tile_is_solid(tx, ty);
    }
    if(d < 2) // north/south
    {
        if(m & 0xc) return;
        for(uint8_t i = 0; i < RANDOM_DOOR_SPACE; ++i)
            if(!tile_is_solid(x - i, y) || !tile_is_solid(x + i, y))
                return;
    }
    else // west/east
    {
        if(m & 0x3) return;
        for(uint8_t i = 0; i < RANDOM_DOOR_SPACE; ++i)
            if(!tile_is_solid(x, y - i) || !tile_is_solid(x, y + i))
                return;
    }
    add_door(x, y);
}

bool occupied(uint8_t x, uint8_t y)
{
    if(tile_is_solid(x, y)) return true;
    if(x == xdn && y == ydn) return true;
    if(x == xup && y == yup) return true;
    for(auto const& e : ents)
        if(e.type != entity::NONE && e.x == x && e.y == y)
            return true; 
    for(auto i : items)
        if(!i.it.is_nothing() && i.x == x && i.y == y)
            return true;
    for(uint8_t i = 0; i < num_doors; ++i)
        if(doors[i].x == x && doors[i].y == y)
            return true;
    return false;
}

bool find_unoccupied(uint8_t& rx, uint8_t& ry)
{
    for(uint16_t tries = 0; tries < 1024; ++tries)
    {
        uint8_t x = u8rand(MAP_W);
        uint8_t y = u8rand(MAP_H);
        if(!occupied(x, y))
            return rx = x, ry = y, true;
    }
    return false;
}

void find_unoccupied_guaranteed(uint8_t& x, uint8_t& y)
{
    while(!find_unoccupied(x, y))
        (void)0;
}

coord find_unoccupied_guaranteed()
{
    coord c;
    find_unoccupied_guaranteed(c.x, c.y);
    return c;
}

static void generate_item(uint8_t i, item it)
{
    if(maps[map_index].got_items.test(i))
        return;
    auto& mit = items[i];
    find_unoccupied_guaranteed(mit.x, mit.y);
    if(maps[map_index].got_items.test(i))
        it.reset();
    mit.it = it;
}

static void generate_random_item(uint8_t i)
{
    item it;
    uint8_t type = 0;
    uint8_t subtype = 0;

    bool cursed = (u8rand() < ITEM_CURSED_CHANCE);
    
    uint8_t quant = 0;
    uint8_t enchant = ENCHANT_LEVEL_ZERO + 1;
    {
        uint8_t chance = 96 + map_index * 8;
        while(enchant < ENCHANT_LEVEL_MAX && u8rand() < chance)
            ++enchant;
        if(cursed)
            enchant = ENCHANT_LEVEL_ZERO * 2 - enchant;
    }

#if 1

    static uint8_t const TYPES[] PROGMEM =
    {
        item::POTION, item::POTION, item::POTION, item::POTION, item::POTION,
        item::POTION, item::POTION, item::POTION, item::POTION, item::POTION,
        item::POTION,
        item::SCROLL, item::SCROLL, item::SCROLL, item::SCROLL, item::SCROLL,
        item::SCROLL, item::SCROLL,
        item::WAND, item::WAND,
        item::ARROW, item::ARROW,
        item::FOOD, item::FOOD,
        item::BOW,
        item::SWORD, item::SWORD,
        item::ARMOR, item::HELM, item::BOOTS,
        item::RING, item::AMULET,
    };

    type = pgm_read_byte(&TYPES[u8rand() % 32]);

    if(type == item::POTION)
        subtype = u8rand(NUM_POT);
    else if(type == item::SCROLL)
        subtype = u8rand(NUM_SCR);
    else if(type == item::RING)
    {
        subtype = u8rand(NUM_RNG - 1);
        if(u8rand() < 5) subtype = RNG_INVIS;
    }
    else if(type == item::AMULET)
        subtype = u8rand(NUM_AMU);
    else if(type == item::WAND)
    {
        subtype = u8rand(NUM_WND);
        quant = 3 + u8rand() % 8; // charges
    }
    else if(type == item::ARROW)
        quant = u8rand() % 4 + 2;

#else

    constexpr double P_FOOD   = 6;
    constexpr double P_POTION = 25;
    constexpr double P_SCROLL = 15;
    constexpr double P_ARROW  = 10;
    constexpr double P_BOW    = 4;
    constexpr double P_SWORD  = 4;
    constexpr double P_RING   = 2;
    constexpr double P_AMULET = 1;
    constexpr double P_ARMOR  = 3;
    constexpr double P_HELM   = 3;
    constexpr double P_BOOTS  = 3;

    constexpr double P_TOTAL =
        P_FOOD + P_POTION + P_SCROLL + P_ARROW + P_BOW + P_SWORD +
        P_RING + P_AMULET + P_ARMOR + P_HELM + P_BOOTS;

    constexpr double N_FOOD   = (P_FOOD   * 256.5 / P_TOTAL + 0       );
    constexpr double N_POTION = (P_POTION * 256.5 / P_TOTAL + N_FOOD  );
    constexpr double N_SCROLL = (P_SCROLL * 256.5 / P_TOTAL + N_POTION);
    constexpr double N_ARROW  = (P_ARROW  * 256.5 / P_TOTAL + N_SCROLL);
    constexpr double N_BOW    = (P_BOW    * 256.5 / P_TOTAL + N_ARROW );
    constexpr double N_SWORD  = (P_SWORD  * 256.5 / P_TOTAL + N_BOW   );
    constexpr double N_RING   = (P_RING   * 256.5 / P_TOTAL + N_SWORD );
    constexpr double N_AMULET = (P_AMULET * 256.5 / P_TOTAL + N_RING  );
    constexpr double N_ARMOR  = (P_ARMOR  * 256.5 / P_TOTAL + N_AMULET);
    constexpr double N_HELM   = (P_HELM   * 256.5 / P_TOTAL + N_ARMOR );
    constexpr double N_BOOTS  = (P_BOOTS  * 256.5 / P_TOTAL + N_HELM  );

    uint8_t r = u8rand();

    if(r < (uint8_t)N_FOOD)
        type = item::FOOD;
    else if(r < (uint8_t)N_POTION)
    {
        type = item::POTION;
        subtype = u8rand(NUM_POT);
    }
    else if(r < (uint8_t)N_SCROLL)
    {
        type = item::SCROLL;
        subtype = u8rand(NUM_SCR);
    }
    else if(r < (uint8_t)N_ARROW)
    {
        type = item::ARROW;
        quant = u8rand() % 4 + 3;
    }
    else if(r < (uint8_t)N_BOW)
        type = item::BOW;
    else if(r < (uint8_t)N_SWORD)
        type = item::SWORD;
    else if(r < (uint8_t)N_RING)
    {
        type = item::RING;
        subtype = u8rand(NUM_RNG);
    }
    else if(r < (uint8_t)N_AMULET)
    {
        type = item::AMULET;
        subtype = u8rand(NUM_AMU);
    }
    else if(r < (uint8_t)N_ARMOR)
        type = item::ARMOR;
    else if(r < (uint8_t)N_HELM)
        type = item::HELM;
    else if(r < (uint8_t)N_BOOTS)
        type = item::BOOTS;

#endif

    if(type == item::BOOTS)
    {
        if(u8rand() < 12) subtype = BOOTS_SPEED;
    }

    it.type = type;
    it.subtype = subtype;

    if(type <= item::WAND)
        it.quant = quant;
    else
    {
        it.level = enchant;
        it.identified = 0;
        it.cursed = cursed;
    }

    generate_item(i, it);
}

coord dircoord(uint8_t d)
{
    coord r;
    r.x = pgm_read_byte(&DIRX[d]);
    r.y = pgm_read_byte(&DIRY[d]);
    return r;
}

coord ddircoord(uint8_t d)
{
    coord r;
    r.x = pgm_read_byte(&DDIRX[d]);
    r.y = pgm_read_byte(&DDIRY[d]);
    return r;
}

static void add_random_monster(uint8_t i)
{
    auto& e = ents[i];
    if(!find_unoccupied(e.x, e.y))
        return;
    for(;;)
    {
        uint8_t j = u8rand(6);
        j = pgm_read_byte(&MAP_GEN_INFOS[map_index].monster_types[j]);
        if(j != 0)
        {
            e.type = j;
            if(j == entity::MIMIC)
                healths[i] = u8rand(item::NUM_ITEM_TYPES - 1) + 1;
#if ENABLE_GOT_ENTS
            if(maps[map_index].got_ents.test(i))
                e.type = entity::NONE;
#endif
            break;
        }
    }
}

void try_spawn_monster()
{
    uint8_t j = u8rand(MAP_ENTITIES);
    if(ents[j].type != entity::NONE) return;
    add_random_monster(j);
    if(player_can_see_entity(j))
        ents[j].type = entity::NONE;
}

void generate_dungeon()
{
    rand_seed = game_seed;
    for(uint8_t i = 0; i < map_index; ++i)
        u8rand();

    for(auto& t : tmap) t = 0xff;
    memzero(&tfog, sizeof(tfog));
    memzero(&items, sizeof(items));
    memzero(&rooms, sizeof(rooms));
    memzero(&doors, sizeof(doors));
    num_rooms = 0;
    num_doors = 0;

    render(); // show "Loading..."

    dig_initial_room();
    for(uint16_t i = 0; i < 4096; ++i)
        try_generate_room();

    for(uint16_t i = 0; i < 1024; ++i)
        try_add_random_door();

    // set all solid tiles to explored (makes walls fill out better)
    for(uint16_t i = 0; i < tfog.size(); ++i)
        tfog[i] |= tmap[i];
    update_doors();

    if(map_index < NUM_MAPS - 1)
        find_unoccupied_guaranteed(xdn, ydn);
    else
        xdn = ydn = 255;
    find_unoccupied_guaranteed(xup, yup);

    if(map_index == NUM_MAPS - 1)
    {
        uint8_t x = rooms[0].x + 7;
        uint8_t y = rooms[0].y + 7;
        {
            auto& e = ents[1];
            e.type = entity::DARKNESS;
            e.x = x;
            e.y = y;
        }
        {
            uint8_t i = num_doors;
            if(i == MAP_DOORS) --i;
            else ++num_doors;
            auto& d = doors[i];
            d.secret = 0;
            d.x = x;
            d.y = y + 2;
        }
    }

    //
    // add monsters
    //

    for(uint8_t i = 2; i < MAP_ENTITIES; ++i)
    {
        ents[i].type = entity::NONE;
        add_random_monster(i);
    }

    //
    // add items
    //

    {
        uint8_t i = 0;
        generate_item(i++, item::make(item::POTION, POT_HEALING));
        generate_item(i++, item::make(item::FOOD));
        {
            union { uint16_t a; item b; } u = {
                pgm_read_word(&MAP_GEN_INFOS[map_index].guaranteed_item)
            };
            if(!u.b.is_nothing())
                generate_item(i++, u.b);
        }

        for(; i < MAP_ITEMS - 4; ++i)
            generate_random_item(i);
    }

}

void new_entity(uint8_t i, uint8_t type, uint8_t x, uint8_t y)
{
    auto& e = ents[i];
    e = {};
    e.type = type;
    healths[i] = entity_max_health(i);
    e.x = x;
    e.y = y;
}
