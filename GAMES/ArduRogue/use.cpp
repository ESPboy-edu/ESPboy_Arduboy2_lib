#include "game.hpp"

static constexpr uint8_t WAND_RANGE = 6;

static void identify_item(uint8_t i)
{
    auto& it = inv[i];
    uint8_t st = it.subtype;
    if(it.type >= item::WAND)
        it.identified = 1;
    switch(it.type)
    {
    case item::POTION: identify_potion(st); break;
    case item::SCROLL: identify_scroll(st); break;
    case item::RING:   identify_ring(st);   break;
    case item::AMULET: identify_amulet(st); break;
    case item::WAND:   identify_wand(st);   break;
    case item::FOOD:
    case item::ARROW:
        break;
    default:
        break;
    }
}

void wand_effect(uint8_t i, uint8_t d, uint8_t subtype)
{
    scan_result sr;
    if(d < 4)
    {
        scan_dir(i, d, WAND_RANGE, sr);
        if(subtype != WND_DIGGING)
            draw_ray_anim(ents[i].x, ents[i].y, d, sr.n);
    }
    else
    {
        sr.x = ents[i].x;
        sr.y = ents[i].y;
        sr.i = i;
        d = u8rand() % 4;
    }

    entity* te = get_entity(sr.x, sr.y);
    if(subtype == WND_DIGGING)
    {
        uint8_t x = ents[i].x, y = ents[i].y;
        auto c = dircoord(d);
        for(uint8_t i = 0; i < WAND_RANGE; ++i)
        {
            x += c.x;
            y += c.y;
            if(x >= MAP_W || y >= MAP_H) break;
            dig_tile(x, y);
            if(door* d = get_door(x, y))
            {
                d->open = 1;
                d->secret = 0;
            }
        }
        return;
    }
    else if(subtype == WND_FIRE)
    {
        static int8_t const FIRE[18] PROGMEM =
        {
            0, 0, -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0,
        };
        draw_dungeon_at_player();
        for(uint8_t j = 0; j < 18; j += 2)
        {
            uint8_t tx = sr.x + pgm_read_byte(&FIRE[j]);
            uint8_t ty = sr.y + pgm_read_byte(&FIRE[j + 1]);
            draw_sprite_nonprog_rel_and_wait(0x0eae, tx, ty);
            if(entity* e = get_entity(tx, ty))
                entity_take_fire_damage_from_entity(i, index_of_entity(*e), u8rand(8) + 8);
        }
        paint_left();
        return;
    }
    else if(!te) return;
    uint8_t ti = index_of_entity(*te);
    switch(subtype)
    {
    case WND_FORCE:
    {
        status_u(PSTR("@U blasted back!"), ti);
        scan_result tsr;
        scan_dir_pos(te->x, te->y, d, 8, tsr);
        if(tsr.i < MAP_ENTITIES)
        {
            te->x = tsr.px;
            te->y = tsr.py;
            status_u(PSTR("@S crashes into"), ti);
            status_u(PSTR("@O."), tsr.i);
            paralyze_entity(tsr.i);
        }
        else
        {
            if(tsr.i == 254)
            {
                status_u(PSTR("@S hits a wall."), ti);
                paralyze_entity(ti);
            }
            te->x = tsr.x;
            te->y = tsr.y;
        }
        // TODO: if(entity* pe = get_entity(tsr.x, tsr.y))
        //           damage both te and pe
        break;
    }
    case WND_TELEPORT:
        if(sr.i < MAP_ENTITIES)
            teleport_entity(sr.i);
        break;
    case WND_STRIKING:
        entity_take_magic_damage_from_entity(i, sr.i, u8rand(12) + 12);
        break;
    case WND_ICE:
        slow_entity(sr.i);
        entity_take_magic_damage_from_entity(i, sr.i, u8rand(8) + 8);
        break;
    case WND_POLYMORPH:
    {
        uint8_t t = te->type;
        if(t > entity::BAT && t < entity::DARKNESS)
        {
            status_u(PSTR("@T form morphs into"), ti);
            te->type = u8rand() < 64 ? t + 1 : t - 1;
            status_u(PSTR("@O."), ti);
            healths[ti] = entity_max_health(sr.i);
        }
        break;
    }
    default:
        break;
    }

}

static void use_wand(uint8_t subtype)
{
    draw_dungeon_at_player();
    paint_left();
    {
        bool was_identified = wand_is_identified(subtype);
        identify_wand(subtype);
        if(!was_identified)
            status_i(PSTR2(STRI_YOU_DISCOVER_IT "is a @i."), item::make(item::WAND, subtype));
    }
    uint8_t d = 255;
    direction_menu(d); // canceling will mean target self
    wand_effect(0, d, subtype);
}

static void use_scroll(uint8_t subtype)
{
    uint8_t i;
    {
        bool was_identified = scroll_is_identified(subtype);
        identify_scroll(subtype);
        if(!was_identified)
            status_i(STR_YOU_DISCOVER_IT_WAS_A_I, item::make(item::SCROLL, subtype));
    }
    switch(subtype)
    {
    case SCR_IDENTIFY:
    {
        i = inventory_menu(PSTR2("Identify" STRI_WHICH_ITEM_Q));
        if(i < INV_ITEMS)
        {
            identify_item(i);
            status_i(PSTR2(STRI_YOU "identified: @i."), inv[i]);
        }
        else
            status_simple(STR_NOTHING_HAPPENS);
        break;
    }
    case SCR_ENCHANT:
    {
        i = inventory_menu(PSTR2("Enchant" STRI_WHICH_ITEM_Q));
        if(i < INV_ITEMS)
        {
            auto& it = inv[i];
            if(it.stackable())
                status_simple(STR_NOTHING_HAPPENS);
            else
            {
                uint8_t n = 1;
                if(it.type == item::WAND) n = 4;
                it.level += n;
                if(it.level > ENCHANT_LEVEL_MAX)
                    it.level = ENCHANT_LEVEL_MAX;
                static char const MSG2[] PROGMEM = STRI_THE_I_GLOWS "blue" STRI_FOR_A_MOMENT_P;
                status_i(MSG2, it);
            }
        }
        else
            status_simple(STR_NOTHING_HAPPENS);
        break;
    }
    case SCR_REMOVE_CURSE:
    {
        i = inventory_menu(PSTR2("Uncurse" STRI_WHICH_ITEM_Q));
        if(i < INV_ITEMS)
        {
            auto& it = inv[i];
            if(it.type <= item::WAND || !it.cursed)
                status_simple(STR_NOTHING_HAPPENS);
            else
            {
                it.cursed = 0;
                static char const MSG2[] PROGMEM = STRI_THE_I_GLOWS "white" STRI_FOR_A_MOMENT_P;
                status_i(MSG2, it);
            }
        }
        else
            status_simple(STR_NOTHING_HAPPENS);
        break;
    }
    case SCR_TELEPORT:
        teleport_entity(0);
        break;
    case SCR_MAPPING:
        for(auto& t : tfog)
            t = 0xff;
        status_simple(PSTR2(STRI_YOU "become aware of your surroundings."));
        break;
    case SCR_MASS_POISON:
        status_simple(PSTR("There is a poisonous blast."));
        /* fallthrough */
    case SCR_FEAR:
    case SCR_TORMENT:
    case SCR_MASS_CONFUSE:
    {
        bool found = false;
        for(uint8_t i = 0; i < MAP_ENTITIES; ++i)
        {
            auto& e = ents[i];
            if(e.type == entity::NONE) continue;
            if(!player_can_see_entity(i)) continue;
            aggro_monster(i);
            if(subtype == SCR_FEAR)
            {
                if(i != 0)
                {
                    found = true;
                    e.scared = 1;
                    status_u(PSTR("@S flees!"), i);
                }
            }
            else
            {
                found = true;
                if(subtype == SCR_TORMENT)
                {
                    healths[i] /= 2;
                    status_u(PSTR("@U stricken!"), i);
                }
                else if(subtype == SCR_MASS_CONFUSE)
                    confuse_entity(i);
                else if(subtype == SCR_MASS_POISON)
                    poison_entity(i);
            }
        }
        if(!found)
            status_simple(STR_NOTHING_HAPPENS);
        break;
    }
    default:
        break;
    }
}

uint8_t slot_of_item(uint8_t type)
{
    switch(type)
    {
    case item::BOW:
    case item::SWORD:  return SLOT_WEAPON;
    case item::HELM:   return SLOT_HELM;
    case item::ARMOR:  return SLOT_ARMOR;
    case item::BOOTS:  return SLOT_BOOTS;
    case item::RING:
    {
        uint8_t j = pinfo.equipped[SLOT_RING1];
        // if slot 1 empty, return slot 1
        if(j >= INV_ITEMS) return SLOT_RING1;
        // if slot 2 cursed, return slot 1
        j = pinfo.equipped[SLOT_RING2];
        if(j < INV_ITEMS && inv[j].cursed) return SLOT_RING1;
        // otherwise return slot 2
        return SLOT_RING2;
    }
    case item::AMULET: return SLOT_AMULET;
    default: break;
    }
    return 255;
}

bool unequip_item(uint8_t i)
{
    auto it = inv[i];
    char const* verb = it.type <= item::SWORD ?
        PSTR("stop using") : PSTR("take off");
    if(it.cursed)
    {
        status_si(PSTR2(STRI_YOU_ARE_UNABLE_TO STRI_P_THE_I), verb, it);
        return false;
    }
    for(auto& j : pinfo.equipped)
        if(j == i)
        {
            status_si(STR_YOU_P_THE_I, verb, it);
            j = 255;
            adjust_health_to_max_health(0);
            return true;
        }
    return false;
}

bool equip_item(uint8_t i)
{
    item it = inv[i];
    uint8_t type = it.type;
    uint8_t slot = slot_of_item(type);
    uint8_t& j = pinfo.equipped[slot];
    if(j < INV_ITEMS && !unequip_item(j))
        return false;
    bool weap = type <= item::SWORD;
    identify_item(i);
    status_si(STR_YOU_P_THE_I,
        weap ? PSTR("ready") : PSTR("put on"),
        inv[i]);
    if(it.is_type(item::AMULET, AMU_CLARITY))
        end_confusion(0);
    adjust_health_to_max_health(0);
    j = i;
    return true;
}

bool use_item(uint8_t i)
{
    item it = inv[i];
    uint8_t subtype = it.subtype;
    {
        if(it.stackable())
        {
            int8_t cb = amulet_bonus(AMU_CONSERVATION);
            if(cb <= 0 || u8rand() < 192)
                player_remove_item(i);
            if(cb < 0 && u8rand() < 64)
                player_remove_item(i);
        }
    }
    switch(it.type)
    {
    case item::FOOD:
    {
        hunger = 0;
        status_you_are_no_longer(STR_HUNGRY);
        return true;
    }
    case item::POTION:
        entity_apply_potion(subtype, 0);
        return true;
    case item::SCROLL:
        use_scroll(subtype);
        return true;
    case item::WAND:
    {
        uint8_t n = it.quant;
        if(n <= 1)
        {
            player_remove_item(i);
            status_i(PSTR2(STRI_CAPTHE "@i crumbles to dust."), it);
        }
        else
            inv[i].quant = n - 1;
        use_wand(subtype);
        return true;
    }
    case item::BOW:
    case item::SWORD:
    case item::HELM:
    case item::ARMOR:
    case item::BOOTS:
    case item::RING:
    case item::AMULET:
        if(item_is_equipped(i))
            return unequip_item(i);
        return equip_item(i);
    default:
        break;
    }
    return false;
}

void entity_apply_potion(uint8_t subtype, uint8_t i)
{
    {
        bool was_identified = potion_is_identified(subtype);
        identify_potion(subtype);
        if(!was_identified)
            status_i(STR_YOU_DISCOVER_IT_WAS_A_I, item::make(item::POTION, subtype));
    }
    switch(subtype)
    {
    case POT_HEALING:
    {
        uint8_t mhp = entity_max_health(i);
        entity_heal(i, u8rand(mhp / 2 + 1) + mhp / 4);
        entity_restore_strength(i);
        break;
    }
    case POT_CONFUSION:
        confuse_entity(i);
        break;
    case POT_POISON:
        poison_entity(i);
        break;
    case POT_HARMING:
    {
        uint8_t dam = entity_max_health(i) / 8 + 1;
        dam += u8rand(dam * 2);
        if(dam > 10) dam = 10;
        status_u(PSTR2("@U harmed by " STRI_THE "potion!"), i);
        entity_take_damage_from_entity(0, i, dam);
        break;
    }
    case POT_STRENGTH:
        if(ents[i].weakened)
            entity_restore_strength(i);
        else if(i == 0)
        {
            status_simple(STR_YOU_FEEL_STRONGER);
            pstats.strength += 1;
        }
        break;
    case POT_DEXTERITY:
        if(i == 0)
        {
            status_simple(STR_YOU_FEEL_MORE_AGILE);
            pstats.dexterity += 1;
        }
        break;
    case POT_INVIS:
        if(i == 0)
        {
            if(ring_bonus(RNG_INVIS) < 0)
                break;
            pinfo.invis_rem = u8rand() % 16 + 12;
            status_simple(PSTR2(STRI_YOU "turn " STRI_INVISIBLE "."));
        }
        ents[i].invis = 1;
        break;
    case POT_PARALYSIS:
        paralyze_entity(i);
        break;
    case POT_SLOWING:
        slow_entity(i);
        break;
    case POT_EXPERIENCE:
        if(i == 0)
            player_gain_xp(50);
        break;
    default:
        // TODO
        //status(PSTR("NOT IMPLEMENTED"));
        break;
    }
}

NOINLINE bool item_is_equipped(uint8_t i)
{
    for(auto j : pinfo.equipped)
        if(i == j) return true;
    return false;
}
