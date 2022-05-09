#include "game.hpp"

static constexpr entity_info decl_ent(
    uint8_t mean       = 0,
    uint8_t nomove     = 0,
    uint8_t regens     = 0,
    uint8_t invis      = 0,
    uint8_t poison     = 0,
    uint8_t vampire    = 0,
    uint8_t confuse    = 0,
    uint8_t paralyze   = 0,
    uint8_t fbreath    = 0,
    uint8_t opener     = 0,
    uint8_t see_invis  = 0,
    uint8_t strength   = 0,
    uint8_t dexterity  = 0,
    uint8_t speed      = 0,
    uint8_t defense    = 0,
    uint8_t max_health = 0,
    uint8_t xp         = 0
)
{
    return
    {
        { uint8_t(
            entity_info{}.mean     .make(mean     ) |
            entity_info{}.nomove   .make(nomove   ) |
            entity_info{}.regens   .make(regens   ) |
            entity_info{}.invis    .make(invis    ) |
            entity_info{}.poison   .make(poison   ) |
            entity_info{}.vampire  .make(vampire  ) |
            entity_info{}.confuse  .make(confuse  ) |
            entity_info{}.paralyze .make(paralyze ) |
        0) },
        { uint8_t(
            entity_info{}.fbreath  .make(fbreath  ) |
            entity_info{}.opener   .make(opener   ) |
            entity_info{}.see_invis.make(see_invis) |
        0) },
        strength,
        dexterity,
        speed,
        defense,
        max_health,
        xp,
    };
}

entity_info const MONSTER_INFO[] PROGMEM =
{
    /*
    *        mean        poison      fbreath      dexterity        xp
    *           nomove      vampire     opener        speed
    *              regens      confuse     see invis      defense
    *                 invis       paralyze    strength        max_health
    */
    decl_ent(),                                                         // none
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  4,  4,  4,  0,  10,  0), // player
    decl_ent(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1,  6,  8,  0,   1,  1), // bat
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  2,  3,  3,  0,   3,  2), // snake
    decl_ent(1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,  3,  3,  3,  0,   4,  3), // rattlesnake
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  4,  2,  2,  0,   6,  5), // zombie
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  5,  4,  4,  1,  10,  6), // goblin
    decl_ent(1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,  6,  4,  4,  1,  12,  7), // phantom
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  7,  4,  4,  3,  16,  8), // orc
    decl_ent(1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,  5,  4,  4,  0,  12,  9), // tarantula
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  8,  4,  4,  2,  20, 11), // hobgoblin
    decl_ent(1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  7,  4,  4,  3,  20, 11), // mimic
    decl_ent(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1,  9,  4,  4,  3,  24, 14), // incubus
    decl_ent(1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 10,  3,  3,  5,  32, 18), // troll
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  7,  6,  6,  1,  24, 18), // griffin
    decl_ent(1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 12,  4,  4,  8,  48, 25), // dragon
    decl_ent(1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 10,  6,  6,  3,  24, 35), // fallen angel
    decl_ent(1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 16,  6,  8,  8, 128, 90), // lord of darkness
};

entity_info entity_get_info(uint8_t i)
{
    entity_info r;
    entity_get_info(i, r);
    return r;
}

void entity_get_info(uint8_t i, entity_info& info)
{
    if(i == 0)
        info = pstats;
    else
        pgm_memcpy(&info, &MONSTER_INFO[ents[i].type], sizeof(info));
}

static uint8_t dist_to_player(uint8_t x, uint8_t y)
{
    uint8_t dx = u8abs(x - ents[0].x);
    uint8_t dy = u8abs(y - ents[0].y);
    return dx + dy; // manhatten distance
}

void monster_ai(uint8_t i, action& a)
{
    auto& e = ents[i];
    uint8_t ex = e.x, ey = e.y;
    entity_info info;
    entity_get_info(i, info);
    a.type = action::WAIT;
    if((info.nomove && !e.aggro) || player_is_dead())
        return;
    uint8_t dp = dist_to_player(e.x, e.y);
    bool mean = info.mean | e.aggro;

    if(!mean || (player_is_invisible() && !info.see_invis) || dp >= 10)
    {
        a.data = u8rand() & 3;
        auto c = dircoord(a.data);
        uint8_t nx = ex + c.x;
        uint8_t ny = ey + c.y;
        if(get_door(nx, ny))
            return; // don't open doors when moving randomly
        entity* e = get_entity(nx, ny);
        if(e && (!mean || e->type != entity::PLAYER))
            return;
        a.type = action::MOVE;
        return;
    }

    // fire breathing
    if(info.fbreath)
    {
        scan_result sr;
        uint8_t dir;
        for(dir = 0; dir < 4; ++dir)
        {
            scan_dir(i, dir, 5, sr);
            if(sr.i == 0)
                break;
        }
        if(sr.i == 0 && u8rand() % 2)
        {
            status_u(PSTR("@S breathes fire!"), i);
            render();
            wand_effect(i, dir, WND_FIRE);
            return;
        }
    }

    for(uint8_t i = 0; i < 4; ++i)
    {
        auto c = dircoord(i);
        uint8_t nx = ex + c.x;
        uint8_t ny = ey + c.y;
        if(tile_is_solid(nx, ny)) continue;
        if(entity* te = get_entity(nx, ny))
            if(te->type != entity::PLAYER)
                continue;
        uint8_t td = dist_to_player(nx, ny);
        if(e.scared)
        {
            if(td > dp)
            {
                a.type = action::MOVE;
                dp = td, a.data = i;
            }
        }
        else if(td < dp || (td == dp && (u8rand() & 1)))
        {
            a.type = action::MOVE;
            dp = td, a.data = i;
        }
    }
}
