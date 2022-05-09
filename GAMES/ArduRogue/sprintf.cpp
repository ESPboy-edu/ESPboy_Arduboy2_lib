#include "game.hpp"

uint8_t tsprintf(char* b, char const* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    uint8_t r = tvsprintf(b, fmt, ap);
    va_end(ap);
    return r;
}

static char* tstrcpy_prog(char* dst, char const* src)
{
    va_list undefined_args;
#if defined(__GNUC__) && defined(__AVR_ARCH__)
    asm("" : "=w"(undefined_args));
#else
//    undefined_args = {};
#endif
    dst += tvsprintf(dst, src, undefined_args);
    return dst;
}

static char* quantify_item(
    char* dst, uint8_t n, char const* s, char const* s2 = nullptr)
{
    if(n == 0)
    {
        dst = tstrcpy_prog(dst, s);
        if(s2) *dst++ = ' ', dst = tstrcpy_prog(dst, s2);
        return dst;
    }
    ++n;
    uint8_t t = n % 10;
    n /= 10;
    if(n != 0) *dst++ = '0' + n;
    *dst++ = '0' + t;
    *dst++ = ' ';
    dst = tstrcpy_prog(dst, s);
    if(s2) *dst++ = ' ', dst = tstrcpy_prog(dst, s2);
    *dst++ = 's';
    return dst;
}

static char const ITEM_NAME_ARMOR[] PROGMEM = "armor";
static char const ITEM_NAME_HELM [] PROGMEM = "helm";
static char const ITEM_NAME_BOOTS[] PROGMEM = "boots";
static char const* const ITEM_NAME_ARMORS[] PROGMEM =
{
    ITEM_NAME_ARMOR,
    ITEM_NAME_HELM,
    ITEM_NAME_BOOTS,
};

static NOINLINE uint8_t tsprintf_d(char* b, char const* fmt, uint8_t d)
{
    return tsprintf(b, fmt, d);
}

static char* item_name(char* dst, item it)
{
    int8_t t = it.level - ENCHANT_LEVEL_ZERO;
    uint8_t st = it.subtype;
    if(it.cursed && it.identified)
        dst = tstrcpy_prog(dst, PSTR("cursed "));

    static char const STR_SPACE_OF_SPACE[] PROGMEM = " of ";
    static char const STR_POTION[] PROGMEM = "potion";
    static char const STR_SCROLL[] PROGMEM = "scroll";
    static char const STR_DEF_BONUS[] PROGMEM = " [@d def]";
    switch(it.type)
    {
    case item::FOOD:
        dst = quantify_item(dst, it.quant, PSTR("food ration"));
        return dst;
    case item::ARROW:
        dst = quantify_item(dst, it.quant, STR_ARROW);
        return dst;
    case item::BOW:
    case item::SWORD:
        dst = tstrcpy_prog(dst, it.type == item::BOW ? PSTR("bow") : PSTR("sword"));
        if(it.identified)
            dst += tsprintf_d(dst, PSTR(" [@d atk]"), weapon_item_attack(it));
        return dst;
    case item::ARMOR:
    case item::HELM:
    case item::BOOTS:
        dst = tstrcpy_prog(dst, pgmptr(&ITEM_NAME_ARMORS[it.type - item::ARMOR]));
        if(it.identified)
        {
            char const* s = STR_DEF_BONUS;
            if(it.is_type(item::BOOTS, BOOTS_SPEED))
                s = PSTR(" of speed [+2 spd @d def]");
            dst += tsprintf_d(dst, s, armor_item_defense(it));
        }
        return dst;
    case item::WAND:
        if(wand_is_identified(st))
        {
            dst = tstrcpy_prog(dst, PSTR("wand of "));
            dst = tstrcpy_prog(dst, pgmptr(&WND_NAMES[st]));
            //dst += tsprintf_d(dst, PSTR(" [@u]"), n);
            return dst;
        }
        else
        {
            dst = tstrcpy_prog(dst, pgmptr(&UNID_WND_NAMES[perm_wnd[st]]));
            dst = tstrcpy_prog(dst, PSTR(" wand"));
            return dst;
        }
    case item::RING:
        if(ring_is_identified(st))
        {
            dst = tstrcpy_prog(dst, PSTR("ring of "));
            dst = tstrcpy_prog(dst, pgmptr(&RNG_NAMES[st]));
            if(it.identified)
            {
                char const* s = STR_EMPTY;
                switch(st)
                {
                case RNG_STRENGTH:
                    s = PSTR(" [@d str]");
                    break;
                case RNG_DEXTERITY:
                    s = PSTR(" [@d dex]");
                    break;
                case RNG_PROTECTION:
                    s = STR_DEF_BONUS;
                    break;
                case RNG_ATTACK:
                    s = PSTR(" [@d atk]");
                    break;
                default:
                    break;
                }
                dst += tsprintf_d(dst, s, (uint8_t)t);
            }
            return dst;
        }
        else
        {
            dst = tstrcpy_prog(dst, pgmptr(&UNID_RNG_AMU_NAMES[perm_rng[st]]));
            dst = tstrcpy_prog(dst, PSTR(" ring"));
            return dst;
        }
    case item::AMULET:
        if(amulet_is_identified(st))
        {
            dst = tstrcpy_prog(dst, PSTR2(STRI_AMULET " of "));
            dst = tstrcpy_prog(dst, pgmptr(&AMU_NAMES[st]));
            if(it.identified)
            {
                switch(st)
                {
                case AMU_SPEED:
                {
                    dst += tsprintf_d(dst, PSTR(" [@d spd]"),
                        uint8_t(int8_t((t + 1) & 0xfe) / 2));
                    break;
                }
                case AMU_VITALITY:
                {
                    dst += tsprintf_d(dst, PSTR(" [@d mhp]"),
                        uint8_t(t * AMU_VITALITY_BONUS));
                    break;
                }
                default:
                    // can't wear two amulets to cancel each other
                    //dst += tsprintf_d(dst, PSTR(" [@d]"), t);
                    break;
                }
            }
            return dst;
        }
        else
        {
            dst = tstrcpy_prog(dst, pgmptr(&UNID_RNG_AMU_NAMES[perm_amu[st]]));
            dst = tstrcpy_prog(dst, PSTR2(" " STRI_AMULET));
            return dst;
        }
    case item::POTION:
        if(potion_is_identified(st))
        {
            dst = quantify_item(dst, it.quant, STR_POTION);
            dst = tstrcpy_prog(dst, STR_SPACE_OF_SPACE);
            dst = tstrcpy_prog(dst, pgmptr(&POT_NAMES[st]));
            return dst;
        }
        else
        {
            return quantify_item(dst, it.quant,
                pgmptr(&UNID_POT_NAMES[perm_pot[st]]),
                STR_POTION);
        }
    case item::SCROLL:
        if(scroll_is_identified(st))
        {
            dst = quantify_item(dst, it.quant, STR_SCROLL);
            dst = tstrcpy_prog(dst, STR_SPACE_OF_SPACE);
            dst = tstrcpy_prog(dst, pgmptr(&SCR_NAMES[st]));
            return dst;
        }
        else
        {
            return quantify_item(dst, it.quant,
                pgmptr(&UNID_SCR_NAMES[perm_scr[st]]),
                STR_SCROLL);
        }

    default:
        break;
    }
    return dst;
}

static char const HEX_CHARS[] PROGMEM = "0123456789ABCDEF";

uint8_t tvsprintf(char* b, char const* fmt, tvsprintf_va_list ap)
{
    char c;
    uint8_t u;
    uintptr_t u16;
    uint8_t dec[5];
    char const* s;
    char* b_orig = b;
    for(;;)
    {
        c = (char)pgm_read_byte(fmt++);
        if((uint8_t)c >= 0x80)
        {
            b += tvsprintf(b, pgmptr(&STRI_STRS[uint8_t(c & 0x7f)]), ap);
            continue;
        }
        if(c != '@')
        {
            *b++ = c;
            if(c == '\0') goto end;
            continue;
        }
        c = (char)pgm_read_byte(fmt++);
        u16 = va_arg(ap, uintptr_t);
        switch(c)
        {
        case 'c': // char
            *b++ = (char)u16;
            break;
        case 'X': // hex word
            *b++ = pgm_read_byte(&HEX_CHARS[(u16 >> 12) & 0xf]);
            *b++ = pgm_read_byte(&HEX_CHARS[(u16 >>  8) & 0xf]);
        //case 'x': // hex byte
            *b++ = pgm_read_byte(&HEX_CHARS[(u16 >>  4) & 0xf]);
            *b++ = pgm_read_byte(&HEX_CHARS[(u16 >>  0) & 0xf]);
            break;
        case 's': // ram string
            s = (char const*)u16;
            while((c = *s++) != '\0')
                *b++ = c;
            break;
        case 'p': // progmem string
            s = (char const*)u16;
            b = tstrcpy_prog(b, s);
            break;
        case 'M': // monster type
            u = (uint8_t)u16;
            b = tstrcpy_prog(b, pgmptr(&MONSTER_NAMES[u]));
            break;
        case 'S': // subject
        case 'O': // object
        case 'T': // subject possessive
        case 'P': // object possessive
        case 'U': // subject is/are
        case 'W': // subject verb
            u = ents[(uint8_t)u16].type;
            if((uint8_t)u16 == 0) // player
            {
                *b++ = (c >= 'S' ? 'Y' : 'y');
                *b++ = 'o';
                *b++ = 'u';
                if(!(c & 1)) *b++ = 'r'; // 'T' or 'P'
            }
            else
            {
                if(player_can_see_entity((uint8_t)u16))
                {
                    b = tstrcpy_prog(b, c >= 'S' ? STR_CAPTHE : STR_THE);
                    b = tstrcpy_prog(b, pgmptr(&MONSTER_NAMES[u]));
                }
                else
                {
                    *b++ = (c >= 'S' ? 'S' : 's');
                    b = tstrcpy_prog(b, PSTR("omething"));
                }
                if(!(c & 1)) // 'T' or 'P'
                {
                    *b++ = '\'';
                    *b++ = 's';
                }
            }
            if(c == 'U')
                b = tstrcpy_prog(b, u == entity::PLAYER ? PSTR(" are") : PSTR(" is"));
            if(c == 'W')
            {
                *b++ = ' ';
                s = va_arg(ap, char const*);
                b = tstrcpy_prog(b, s);
                if(u != entity::PLAYER)
                    *b++ = 's';
            }
            break;
        case 'i': // item
            b = item_name(b, item::from_u16((uint16_t)u16));
            break;
        case 'd': // int8_t only (int16_t not supported)
        case 'u': // uint8_t or uint16_t
            u16 &= 0xffff; // no-op on avr
            if(c == 'd')
            {
                *b++ = (u16 & 0x80 ? '-' : '+');
                if(u16 & 0x80) u16 = uint8_t(-int8_t(u16));
            }
            u = 0;
            do
            {
                dec[u++] = u16 % 10;
                u16 /= 10;
            } while(u16 != 0);
            do
            {
                *b++ = '0' + dec[--u];
            } while(u != 0);
            break;
        default:
            break;
        }
    }
end:
    return (uint8_t)(ptrdiff_t)(b - b_orig) - 1;
}

uint8_t tstrlen(char const* s)
{
    uint8_t r = 0;
    while(*s++ != '\0') ++r;
    return r;
}
