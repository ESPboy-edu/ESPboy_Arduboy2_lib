#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

// costs about 600 bytes
#define ENABLE_MINIMAP 1

// costs about 200 bytes
#define ENABLE_DUNGEON_SCROLL 1

// costs about 100 bytes prog, 64 bytes ram
#define ENABLE_GOT_ENTS 0

// makes the player invulnerable (for debug)
#ifndef PLAYER_INVULNERABLE
#define PLAYER_INVULNERABLE 0
#endif

static constexpr uint16_t SAVE_VERSION = 1;

// platform functionality
void wait();        // wait about 50 ms
uint8_t wait_btn(); // wait for button press
void seed();        // sets rand_seed
void paint_offset(uint8_t x, bool clear = true);
uint8_t read_persistent(uint16_t addr);
void update_persistent(uint16_t addr, uint8_t data);
void flush_persistent(); // (does nothing for Arduino)

// game logic
void run();

#if !(__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900))
#error "Building ArduRogue requires support for at least C++11"
#endif

#if defined(__GNUC__)
#define FORCEINLINE __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define FORCEINLINE __forceinline
#define NOINLINE __declspec(noinline)
#else
#define FORCEINLINE inline
#define NOINLINE
#endif

#ifdef ARDUINO
#include <Arduino.h>

// disabling this breaks the C standard wrt varargs to save some bytes
// need this if multiple separate compressed substrs contain sprintf identifiers
#define SAFE_VA_LIST_ARG_PASS 0
/*
#if 1
// http://michael-buschbeck.github.io/arduino/2013/10/20/string-merging-pstr/
// limitation: cannot use string literal concatenation: PSTR("hi " "there")
#undef PSTR
#define PSTR(str_) \
    (__extension__({ \
        char const* ptr;  \
        asm volatile \
        ( \
            ".pushsection .progmem.pstrs, \"SM\", @progbits, 1" "\n\t" \
            "PSTR%=: .string " #str_                            "\n\t" \
            ".type PSTR%= STT_OBJECT"                           "\n\t" \
            ".popsection"                                       "\n\t" \
            "ldi %A0, lo8(PSTR%=)"                              "\n\t" \
            "ldi %B0, hi8(PSTR%=)"                              "\n\t" \
            : "=d" (ptr) \
        ); \
        ptr; \
    }))
#endif
*/
#define PSTR2(str_) \
    (__extension__({ static char const PROGSTR_[] PROGMEM = str_; PROGSTR_; }))

#else
#define SAFE_VA_LIST_ARG_PASS 1
#define PSTR(str_) str_
#define PSTR2(str_) str_
#define PROGMEM
inline uint8_t pgm_read_byte(void const* p) { return *(uint8_t*)p; }
inline uint16_t pgm_read_word(void const* p) { return *(uint16_t*)p; }
inline void const* pgm_read_ptr(void const* p) { return *(void const**)p; }
#endif

// useful when T is a pointer type, like function pointer or char const*
template<class T>
FORCEINLINE T pgmptr(T const* p) { return (T)pgm_read_ptr(p); }

template<class T, size_t N>
struct array
{
    T d_[N];
    FORCEINLINE T* data() { return d_; }
    FORCEINLINE constexpr T const* data() const { return d_; }
    FORCEINLINE T& operator[](size_t i) { verify(i); return d_[i]; }
    FORCEINLINE constexpr T const& operator[](size_t i) const
    {
#if defined(_MSC_VER) && !defined(NDEBUG)
        verify(i);
#endif
        return d_[i];
    }
    FORCEINLINE constexpr size_t size() const { return N; }
    FORCEINLINE T* begin() { return d_; }
    FORCEINLINE constexpr T const* begin() const { return d_; }
    FORCEINLINE T* end() { return d_ + N; }
    FORCEINLINE constexpr T const* end() const { return d_ + N; }
private:
    void verify(size_t i) const
    {
        (void)i;
#if defined(_MSC_VER) && !defined(NDEBUG)
        if(i >= N) __debugbreak();
#endif
    }
};

template<size_t N> struct bitset
{
    static constexpr size_t ND = (N + 7) / 8;
    array<uint8_t, ND> d_;
    constexpr bool test(size_t i) const { return (d_[i / 8] >> (i % 8)) & 1; }
    void set(size_t i) { d_[i / 8] |= (1 << (i % 8)); }
    void clear(size_t i) { d_[i / 8] &= ~(1 << (i % 8)); }
};

template<size_t B, size_t N = 1> struct u8bitfield
{
    uint8_t raw_;
    static constexpr uint8_t MASK = ((1 << N) - 1) << B;
    static constexpr uint8_t INV_MASK = ~MASK;

    // helper for constexpr constructors
    template<class T>
    static constexpr uint8_t make(T const& t)
    {
        return (uint8_t(t) << B) & MASK;
    }

    // assign from other bitfield
    template<size_t B2>
    FORCEINLINE void assign(u8bitfield<B2, N> t)
    {
        raw_ = (raw_ & INV_MASK) | (((t.raw_ >> B2) << B) & MASK);
    }

    FORCEINLINE constexpr operator uint8_t() const
    {
        return (raw_ & MASK) >> B;
    }

    template<class T>
    FORCEINLINE u8bitfield& operator=(T const& t)
    {
        return raw_ = (raw_ & INV_MASK) | ((uint8_t(t) << B) & MASK), *this;
    }
    template<class T>
    FORCEINLINE u8bitfield& operator+=(T const& t)
    {
        return *this = uint8_t(*this) + uint8_t(t);
    }
    template<class T>
    FORCEINLINE u8bitfield& operator-=(T const& t)
    {
        return *this = uint8_t(*this) - uint8_t(t);
    }
};

template<class T>
FORCEINLINE void swap(T& a, T& b)
{
    T c = a;
    a = b;
    b = c;
}

void pgm_memcpy(void* dst, void const* src, uint8_t n);
void memzero(void* dst, uint16_t n);

/*
static constexpr uint8_t BTN_UP    = 0x80;
static constexpr uint8_t BTN_DOWN  = 0x10;
static constexpr uint8_t BTN_LEFT  = 0x20;
static constexpr uint8_t BTN_RIGHT = 0x40;
static constexpr uint8_t BTN_A     = 0x08;
static constexpr uint8_t BTN_B     = 0x04;
*/
static constexpr uint8_t BTN_UP    = 0x02;
static constexpr uint8_t BTN_DOWN  = 0x04;
static constexpr uint8_t BTN_LEFT  = 0x01;
static constexpr uint8_t BTN_RIGHT = 0x08;
static constexpr uint8_t BTN_A     = 0x10;
static constexpr uint8_t BTN_B     = 0x20;


static constexpr uint8_t MAP_W = 64;
static constexpr uint8_t MAP_H = 32;
static constexpr uint8_t MAP_ITEMS = 32;
static constexpr uint8_t MAP_ENTITIES = 32;
static constexpr uint8_t MAP_ROOMS = 32;
static constexpr uint8_t MAP_DOORS = 32;
static constexpr uint8_t INV_ITEMS = 24;
static constexpr uint8_t NUM_MAPS = 16;

struct coord { uint8_t x, y; };

struct entity_info
{
    union
    {
        uint8_t raw0_;
        u8bitfield<0> mean;      // whether it is aggressive
        u8bitfield<1> nomove;    // stays put (mon only)
        u8bitfield<2> regens;    // regenerates health
        u8bitfield<3> invis;     // invisible
        u8bitfield<4> poison;    // chance to drain strength (weaken) on hit
        u8bitfield<5> vampire;   // chance to drain max hp on hit
        u8bitfield<6> confuse;   // chance to confuse on hit
        u8bitfield<7> paralyze;  // chance to paralyze on hit
    };
    union
    {
        uint8_t raw1_;
        u8bitfield<0> fbreath;   // ranged attack: breathe fire
        u8bitfield<1> opener;    // whether it can open doors
        u8bitfield<2> see_invis; // can see invisible player
    };

    uint8_t strength;    // higher is better
    uint8_t dexterity;   // higher is better
    uint8_t speed;       // lower is faster
    uint8_t defense;     // higher is better
    uint8_t max_health;

    uint8_t xp;          // xp granted for killing / current xp (player)
};

// item slots
enum
{
    SLOT_WEAPON,
    SLOT_ARMOR,
    SLOT_HELM,
    SLOT_BOOTS,
    SLOT_RING1,
    SLOT_RING2,
    SLOT_AMULET,
    NUM_SLOTS
};

// info specific to player
struct player_info
{
    uint8_t vamp_drain;   // amount of max health drained by vampire
    uint8_t invis_rem;
    array<uint8_t, NUM_SLOTS> equipped;
};

struct entity
{
    enum
    {
        NONE,
        PLAYER,
        BAT,
        SNAKE,
        RATTLESNAKE,
        ZOMBIE,
        GOBLIN,
        PHANTOM,
        ORC,
        TARANTULA,
        HOBGOBLIN,
        MIMIC,
        INCUBUS,
        TROLL,
        GRIFFIN,
        DRAGON,
        ANGEL,
        DARKNESS,
    };
    uint8_t x, y;
    union
    {
        uint8_t raw0_;
        u8bitfield<0> confused;
        u8bitfield<1> paralyzed;
        u8bitfield<2> weakened;
        u8bitfield<4> aggro;     // when a non-mean monster is attacked by player
        u8bitfield<3> invis;
        u8bitfield<5> scared;    // monster is fleeing
        u8bitfield<6> slowed;
    };
    uint8_t type;
};

struct action
{
    enum
    {
        WAIT,
        USE,
        SHOOT,
        DROP,
        THROW,
        CLOSE,
        MOVE,
    };
    uint8_t type;
    uint8_t data; // dir or item
};

// potion types
enum
{
    POT_HEALING,
    POT_CONFUSION,
    POT_POISON,
    POT_HARMING,
    POT_STRENGTH,
    POT_DEXTERITY,
    POT_PARALYSIS,
    POT_SLOWING,
    POT_EXPERIENCE,
    POT_INVIS,
    NUM_POT,
};

// scroll types
enum
{
    SCR_IDENTIFY,
    SCR_ENCHANT,
    SCR_REMOVE_CURSE,
    SCR_TELEPORT,
    SCR_MAPPING,
    SCR_FEAR,
    SCR_TORMENT,
    SCR_MASS_CONFUSE,
    SCR_MASS_POISON,
    NUM_SCR,
};

// ring types
enum
{
    RNG_SEE_INVIS,    // see invis
    RNG_STRENGTH,     // bonus to strength
    RNG_DEXTERITY,    // bonus to dexterity
    RNG_PROTECTION,   // bonus to armor
    RNG_FIRE_PROTECT, // immunity to fire breath
    RNG_ATTACK,       // bonus to attack
    RNG_SUSTENANCE,   // slows down hunger
    RNG_INVIS,        // permanent invis
    NUM_RNG,
};

// amulet types
enum
{
    AMU_SPEED,        // bonus to speed
    AMU_CLARITY,      // prevents becoming confused
    AMU_CONSERVATION, // chance to prevent consuming potions/scrolls
    AMU_REGENERATION, // (chance to) heal each turn
    AMU_VAMPIRE,      // chance to drain health on hit
    AMU_IRONBLOOD,    // paralysis immunity
    AMU_VITALITY,     // bonus to max health
    AMU_WISDOM,       // buff to experience gain
    AMU_YENDOR,
    NUM_AMU = AMU_YENDOR,
};

// wand types
enum
{
    WND_FORCE,        // pushes target back
    WND_TELEPORT,     // teleports target
    WND_DIGGING,      // digs through walls
    WND_FIRE,         // fire damage in a 3x3 area around target
    WND_STRIKING,     // strong magic damage to target
    WND_ICE,          // magic damage and slow to target
    WND_POLYMORPH,    // changes a monster to one level below (LoD immune)
    NUM_WND,
};

// boot types
enum
{
    BOOTS_NORMAL,
    BOOTS_SPEED,
};

static constexpr uint8_t AMU_VITALITY_BONUS = 5;
static constexpr uint8_t ENCHANT_LEVEL_ZERO = 32;
static constexpr uint8_t ENCHANT_LEVEL_MAX = ENCHANT_LEVEL_ZERO + 15;

static constexpr uint8_t NUM_IDENT =
    NUM_POT + NUM_SCR + NUM_RNG + NUM_AMU + NUM_WND;

struct item
{
    enum
    {
        NONE,
        FOOD,   // quantity
        POTION, // quantity
        SCROLL, // quantity
        ARROW,  // quantity
        WAND,   // charges
        BOW,    // level
        SWORD,  // level
        RING,   // level
        AMULET, // level
        ARMOR,  // level
        HELM,   // level
        BOOTS,  // level
        NUM_ITEM_TYPES,
    };
    union
    {
        uint8_t raw0_;
        uint8_t          quant;
        u8bitfield<0, 6> level;
        u8bitfield<7, 1> identified;
        u8bitfield<6, 1> cursed;
    };
    union
    {
        uint8_t raw1_;
        u8bitfield<4, 4> type;
        u8bitfield<0, 4> subtype;
    };
    constexpr bool stackable() const { return type <= ARROW; }
    constexpr bool is_same_type_as(item const& it) const
    {
        return raw1_ == it.raw1_;
    }
    constexpr bool is_nothing() const
    {
        return raw1_ == 0;
    }
    void reset()
    {
        raw1_ = 0;
    }
    static constexpr item make(
        uint8_t type,
        uint8_t subtype = 0,
        bool identified = false)
    {
        return
        {
            { uint8_t(
                item{}.identified.make(identified) |
            0) },
            { uint8_t(
                item{}.type.make(type) |
                item{}.subtype.make(subtype) |
            0) },
        };
    }
    constexpr bool is_type(uint8_t type_, uint8_t subtype_ = 0) const
    {
        return make(type_, subtype_).raw1_ == raw1_;
    }
    static item from_u16(uint16_t x)
    {
        union { uint16_t x; item it; } u{ x };
        return u.it;
    }
    uint16_t to_u16() const
    {
        union { item it; uint16_t x; } u{ *this };
        return u.x;
    }
};
static_assert(sizeof(item) == 2, "");

struct map_item
{
    item it;
    uint8_t x, y;
};

struct map_info
{
    bitset<MAP_ITEMS>    got_items; // picked-up items
#if ENABLE_GOT_ENTS
    bitset<MAP_ENTITIES> got_ents;  // defeated monsters
#endif
    bitset<MAP_ROOMS>    got_rooms; // explored rooms
    bitset<MAP_DOORS>    got_doors; // opened doors
};

struct room
{
    uint8_t x, y;
    uint8_t type; // bit 7 set if big
    uint8_t w() const;
    uint8_t h() const;
    bool solid(uint8_t rx, uint8_t ry) const;
    bool inside(uint8_t tx, uint8_t ty) const;
    bool inside_bb(uint8_t tx, uint8_t ty) const;
};

struct door
{
    union
    {
        uint8_t raw0_;
        u8bitfield<0, 7> x;
        u8bitfield<7, 1> secret; // must be zero if open
    };
    union
    {
        uint8_t raw1_;
        u8bitfield<0, 7> y;
        u8bitfield<7, 1> open;
    };
};

enum
{
    HS_NONE,
    HS_ESCAPED,   // escaped with the amulet
    HS_RETURNED,  // returned to the surface without the amulet
    HS_ABANDONED, // chose to end the game
    HS_ENTITY,    // killed by entity
    HS_SUICIDE,   // killed yourself (by a wand?)
    //HS_TRAP,      // killed by trap
    HS_STARVED,   // starved to death
};
struct high_score
{
    uint16_t score;
    uint8_t type;
    uint8_t data;     // type of entity/trap
};

static constexpr uint8_t NUM_WALL_STYLES = 4;
static constexpr uint8_t NUM_HIGH_SCORES = 4;

struct saved_data
{
    uint16_t                     game_seed;
    uint16_t                     rand_seed;
    high_score                   hs;
    array<map_info, NUM_MAPS>    maps;
    array<item, INV_ITEMS>       inv;
    array<entity, MAP_ENTITIES>  ents;
    array<uint8_t, MAP_ENTITIES> healths;
    array<map_item, MAP_ITEMS>   items;
    array<room, MAP_ROOMS>       rooms;
    array<door, MAP_DOORS>       doors;
    uint8_t                      num_rooms;
    uint8_t                      num_doors;
    uint8_t                      map_index;
    entity_info                  pstats;
    player_info                  pinfo;
    bitset<NUM_IDENT>            identified;
    uint8_t                      prev_action;
    uint8_t                      plevel;
    uint8_t                      hunger;

    // options, high score table, etc
    uint8_t                      wall_style;
    array<high_score, NUM_HIGH_SCORES> high_scores;
};

struct globals
{
    array<uint8_t, 64 * 64 / 8> buf;
    array<uint8_t, size_t(MAP_W) * MAP_H / 8> tmap; // 1: wall/door
    array<uint8_t, size_t(MAP_W) * MAP_H / 8> tfog; // 1: explored
    uint8_t xdn, ydn; // coords of down stairs
    uint8_t xup, yup; // coords of up stairs
    saved_data saved;
    char statusbuf[128];
    uint8_t statusn, statusx, statusy;
    array<uint8_t, NUM_POT> perm_pot;
    array<uint8_t, NUM_SCR> perm_scr;
    array<uint8_t, NUM_RNG> perm_rng;
    array<uint8_t, NUM_AMU> perm_amu;
    array<uint8_t, NUM_WND> perm_wnd;
    uint8_t gflags;
};

enum
{
    GFLAG_JUST_MOVED = (1 << 0),
    GFLAG_JUST_SAVED = (1 << 1),
};

extern globals globals_;

// breakout stuff from struct for ease of coding
static auto& buf = globals_.buf;
static auto& tmap = globals_.tmap;
static auto& tfog = globals_.tfog;
static auto& xdn = globals_.xdn;
static auto& ydn = globals_.ydn;
static auto& xup = globals_.xup;
static auto& yup = globals_.yup;
static auto& num_rooms = globals_.saved.num_rooms;
static auto& num_doors = globals_.saved.num_doors;
static auto& ents = globals_.saved.ents;
static auto& healths = globals_.saved.healths;
static auto& items = globals_.saved.items;
static auto& maps = globals_.saved.maps;
static auto& rooms = globals_.saved.rooms;
static auto& doors = globals_.saved.doors;
static auto& map_index = globals_.saved.map_index;
static auto& inv = globals_.saved.inv;
static auto& game_seed = globals_.saved.game_seed;
static auto& pstats = globals_.saved.pstats;
static auto& pinfo = globals_.saved.pinfo;
static auto& rand_seed = globals_.saved.rand_seed;
static auto& wall_style = globals_.saved.wall_style;
static auto& statusbuf = globals_.statusbuf;
static auto& statusn = globals_.statusn;
static auto& statusx = globals_.statusx;
static auto& statusy = globals_.statusy;
static auto& perm_pot = globals_.perm_pot;
static auto& perm_scr = globals_.perm_scr;
static auto& perm_rng = globals_.perm_rng;
static auto& perm_amu = globals_.perm_amu;
static auto& perm_wnd = globals_.perm_wnd;
static auto& identified = globals_.saved.identified;
static auto& prev_action = globals_.saved.prev_action;
static auto& plevel = globals_.saved.plevel;
static auto& gflags = globals_.gflags;
static auto& hs = globals_.saved.hs;
static auto& high_scores = globals_.saved.high_scores;
static auto& hunger = globals_.saved.hunger;

inline bool potion_is_identified(uint8_t subtype)
{
    return identified.test(subtype);
}
inline bool scroll_is_identified(uint8_t subtype)
{
    return identified.test(NUM_POT + subtype);
}
inline bool ring_is_identified(uint8_t subtype)
{
    return identified.test(NUM_POT + NUM_SCR + subtype);
}
inline bool amulet_is_identified(uint8_t subtype)
{
    return subtype == AMU_YENDOR ||
        identified.test(NUM_POT + NUM_SCR + NUM_RNG + subtype);
}
inline bool wand_is_identified(uint8_t subtype)
{
    return identified.test(NUM_POT + NUM_SCR + NUM_RNG + NUM_AMU + subtype);
}

inline void identify_potion(uint8_t subtype)
{
    identified.set(subtype);
}
inline void identify_scroll(uint8_t subtype)
{
    identified.set(NUM_POT + subtype);
}
inline void identify_ring(uint8_t subtype)
{
    identified.set(NUM_POT + NUM_SCR + subtype);
}
inline void identify_amulet(uint8_t subtype)
{
    identified.set(NUM_POT + NUM_SCR + NUM_RNG + subtype);
}
inline void identify_wand(uint8_t subtype)
{
    identified.set(NUM_POT + NUM_SCR + NUM_RNG + NUM_AMU + subtype);
}

template<class T>
inline T const& tmin(T const& a, T const& b)
{
    return a < b ? a : b;
}
template<class T>
inline T const& tmax(T const& a, T const& b)
{
    return a < b ? b : a;
}

inline uint8_t u8abs(uint8_t x)
{
    return (x & 0x80) ? -x : x;
}
inline uint8_t u8max(uint8_t a, uint8_t b)
{
    return a < b ? b : a;
}

inline uint8_t light_radius() { return 5; }
inline uint8_t light_radius2()
{
    uint8_t r = light_radius();
    return r * r + r / 2;
}

inline bool player_is_dead() { return ents[0].type == entity::NONE; }

// strings.cpp
#define STRI_WHICH_ITEM_Q           "\x80" /* " which item?"             */
#define STRI_FOR_A_MOMENT_P         "\x81" /* " for a moment."           */
#define STRI_THE_I_GLOWS            "\x82" /* "The @i glows "            */
#define STRI_STARVING               "\x83" /* "starving"                 */
#define STRI_TRENGTH                "\x84" /* "trength"                  */
#define STRI_YOU_ARE                "\x85" /* "You are "                 */
#define STRI_YOU_ARE_UNABLE_TO      "\x86" /* "You are unable to "       */
#define STRI_P_THE_I                "\x87" /* "@p the @i."               */
#define STRI_ARROW                  "\x88" /* "arrow"                    */
#define STRI_YOU_FEEL               "\x89" /* "You feel "                */
#define STRI_YOU_CANT_HOLD_ANY_MORE "\x8a" /* "You can't hold any more " */
#define STRI_YOU                    "\x8b" /* "You "                     */
#define STRI_THE_DOOR_IS            "\x8c" /* "The door is "             */
#define STRI_THE_AMULET_OF_YENDOR   "\x8d" /* " the amulet of Yendor"    */
#define STRI_YOU_HAVE               "\x8e" /* "You have "                */
#define STRI_TO_THE_SURFACE         "\x8f" /* " to the surface"          */
#define STRI_SCAPED_WITH            "\x90" /* "scaped with"              */
#define STRI_THE                    "\x91" /* "the "                     */
#define STRI_AMULET                 "\x92" /* "amulet"                   */
#define STRI_CAPTHE                 "\x93" /* "The "                     */
#define STRI_INVISIBLE              "\x94" /* "invisible"                */
#define STRI_DROP_THE_I             "\x95" /* "drop the @i."             */
#define STRI_ARDUROGUE              "\x96" /* "ArduRogue"                */
#define STRI_WELCOME                "\x97" /* "Welcome "                 */
#define STRI_YOU_DISCOVER_IT        "\x98" /* "You discover it "         */
extern char const* const STRI_STRS[] PROGMEM;
extern char const STR_EMPTY[] PROGMEM;
extern char const* const MONSTER_NAMES[] PROGMEM;
extern char const* const POT_NAMES[] PROGMEM;
extern char const* const SCR_NAMES[] PROGMEM;
extern char const* const RNG_NAMES[] PROGMEM;
extern char const* const AMU_NAMES[] PROGMEM;
extern char const* const WND_NAMES[] PROGMEM;
extern char const* const UNID_POT_NAMES[] PROGMEM;
extern char const* const UNID_SCR_NAMES[] PROGMEM;
extern char const* const UNID_RNG_AMU_NAMES[] PROGMEM;
extern char const* const UNID_WND_NAMES[] PROGMEM;
extern char const* const INV_CATEGORIES[] PROGMEM;
extern char const STR_YOU_FEEL_STRONGER[] PROGMEM;
extern char const STR_YOU_FEEL_MORE_AGILE[] PROGMEM;
extern char const STR_NOTHING_HAPPENS[] PROGMEM;
extern char const STR_CONFUSED[] PROGMEM;
extern char const STR_PARALYZED[] PROGMEM;
extern char const STR_SLOWED[] PROGMEM;
extern char const STR_WEAKENED[] PROGMEM;
extern char const* const STR_INVISIBLE;      // merged with "see invisible"
extern char const STR_HUNGRY[] PROGMEM;
extern char const* const STR_STARVING;       // same as STRI_STARVING
extern char const STR_YOU_P_THE_I[] PROGMEM; // "You @p the @i."
extern char const STR_ARROW[] PROGMEM;       // "arrow"
extern char const STR_THE[] PROGMEM;         // "the "
extern char const STR_CAPTHE[] PROGMEM;      // "The "
extern char const STR_AMULET[] PROGMEM;      // "amulet"
extern char const STR_ARDUROGUE[] PROGMEM;   // "ArduRogue"
extern char const STR_YOU_DISCOVER_IT_WAS_A_I[] PROGMEM; // "You discover it was a @i."

// game.cpp
void dig_tile(uint8_t x, uint8_t y);
void fill_tile(uint8_t x, uint8_t y);
void paint_left();  // draw to left half screen
void paint_right(); // draw to right half screen
void paint_left_no_clear();
void paint_right_no_clear();
uint8_t u8rand();
uint8_t u8rand(uint8_t m);
bool tile_is_solid(uint8_t x, uint8_t y);
bool tile_is_explored(uint8_t x, uint8_t y);
bool tile_is_unknown(uint8_t x, uint8_t y);
bool tile_is_solid_or_unknown(uint8_t x, uint8_t y);
door* get_door(uint8_t x, uint8_t y); 
entity* get_entity(uint8_t x, uint8_t y);
uint8_t index_of_door(door const& d);
uint8_t index_of_entity(entity const& e);
uint8_t xp_for_level();
uint8_t armor_item_defense(item it);
uint8_t weapon_item_attack(item it);
void player_gain_xp(uint8_t xp);
bool player_pickup_item(uint8_t i); // map item
void player_remove_item(uint8_t i); // inv item
void put_item_on_ground(uint8_t x, uint8_t y, item it);
void render();
uint8_t process_high_score();
void advance_hunger();
void add_to_score(uint8_t amount);

struct scan_result
{
    uint8_t px, py; // prior to landing spot
    uint8_t x, y; // landing spot
    uint8_t i;    // entity
    uint8_t n;    // length
};
// scan from entity in a direction up to n tiles
void scan_dir(uint8_t i, uint8_t d, uint8_t n, scan_result& r);
void scan_dir_pos(uint8_t x, uint8_t y, uint8_t d, uint8_t n, scan_result& r);

// font.cpp
uint8_t draw_char(uint8_t x, uint8_t y, char c); // returns width of char
void draw_text(uint8_t x, uint8_t y, const char* p);
void draw_text_nonprog(uint8_t x, uint8_t y, const char* p);
// fmt is PROGMEM
void draw_textf(uint8_t x, uint8_t y, const char* fmt, ...); // max 64

uint8_t char_width(char c);
uint8_t text_width_nonprog(char const* s);

// draw.cpp
extern int8_t const DDIRX[16] PROGMEM;
static int8_t const* DDIRY = &DDIRX[8];
static constexpr uint8_t SPACE_WIDTH = 1;
void set_img_prog(uint8_t const* p, uint8_t w, uint8_t x, uint8_t y);
void set_pixel(uint8_t x, uint8_t y);
void clear_pixel(uint8_t x, uint8_t y);
void inv_pixel(uint8_t x, uint8_t y);
void set_hline(uint8_t x0, uint8_t x1, uint8_t y);
void clear_hline(uint8_t x0, uint8_t x1, uint8_t y);
void inv_hline(uint8_t x0, uint8_t x1, uint8_t y);
void set_vline(uint8_t x, uint8_t y0, uint8_t y1);
void clear_vline(uint8_t x, uint8_t y0, uint8_t y1);
void set_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
void clear_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
void inv_rect(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
void set_box(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
void draw_box_pretty(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
void draw_info();
void draw_info_without_status();
void draw_map_offset(uint8_t ox);
void draw_dungeon(uint8_t mx, uint8_t my);
void draw_dungeon_at_player();
void draw_ray_anim(uint8_t x, uint8_t y, uint8_t d, uint8_t n);
void draw_sprite_nonprog_rel_and_wait(uint16_t tp, uint8_t x, uint8_t y);

// generate.cpp
void new_entity(uint8_t i, uint8_t type, uint8_t x, uint8_t y);
void dig_nonsecret_door_tiles();
void update_doors();   // set tile to solid for closed doors
void generate_dungeon();
bool occupied(uint8_t x, uint8_t y); // door/stairs/item/entitity
bool find_unoccupied(uint8_t& x, uint8_t& y); // returns true if found
void find_unoccupied_guaranteed(uint8_t& x, uint8_t& y);
void try_spawn_monster();
coord find_unoccupied_guaranteed();
coord dircoord(uint8_t d);
coord ddircoord(uint8_t d);
extern int8_t const DIRX[8] PROGMEM;
static int8_t const* DIRY = &DIRX[4];

// light.cpp
bool player_can_see(uint8_t x, uint8_t y);
bool player_can_see_entity(uint8_t i); // takes into account invis
bool path_clear(
    uint8_t x0, uint8_t y0,
    uint8_t x1, uint8_t y1);
void set_tile_explored(uint8_t x, uint8_t y);
void update_light();

// sprintf.cpp
char* uncompress(char* dst, char const* src);
uint8_t tsprintf(char* b, char const* fmt, ...);
#if SAFE_VA_LIST_ARG_PASS
using tvsprintf_va_list = va_list&;
#else
using tvsprintf_va_list = va_list;
#endif
uint8_t tvsprintf(char* b, char const* fmt, tvsprintf_va_list ap);
uint8_t tstrlen(char const* s); // s not progmem

// status.cpp: fmt is PROGMEM
static constexpr uint8_t NUM_STATUS_ROWS = 7;
static constexpr uint8_t STATUS_START_Y = 65 - 6 * NUM_STATUS_ROWS;
uint8_t advance_white(char* b, uint8_t i);
void draw_status();
void status(char const* fmt, ...);
void status_simple(char const* s);
void status_i(char const* fmt, item it);
void status_si(char const* s, char const* a, item it);
void status_u(char const* fmt, uint8_t a);
void status_usu(char const* fmt, uint8_t a, char const* b, uint8_t c);
void status_you_are_no_longer(char const* s);
void status_more();
void reset_status();
void status_cursed_amulet();

// entity.cpp
void adjust_health_to_max_health(uint8_t i);
uint8_t entity_speed(uint8_t i);
uint8_t entity_max_health(uint8_t i);
uint8_t entity_strength(uint8_t i);
uint8_t entity_dexterity(uint8_t i);
uint8_t entity_attack(uint8_t i);
uint8_t entity_defense(uint8_t i);
bool test_attack_hit(uint8_t atti, uint8_t defi); // 0 for miss
uint8_t calc_hit_dam_post_mod(uint8_t ta, uint8_t td);
uint8_t calculate_hit_damage(uint8_t atti, uint8_t defi);
uint8_t calculate_arrow_damage(uint8_t defi); // 0 for miss
void entity_restore_strength(uint8_t i);
void entity_heal(uint8_t i, uint8_t amount);
void entity_take_damage(uint8_t i, uint8_t dam);
void entity_take_damage_from_entity(uint8_t atti, uint8_t defi, uint8_t dam);
void entity_take_fire_damage_from_entity(uint8_t atti, uint8_t defi, uint8_t dam);
void entity_take_magic_damage_from_entity(uint8_t atti, uint8_t defi, uint8_t dam);
void entity_take_melee_damage_from_entity(uint8_t atti, uint8_t defi, uint8_t dam);
void teleport_entity(uint8_t i);
void confuse_entity(uint8_t i);
void poison_entity(uint8_t i);
void paralyze_entity(uint8_t i);
void slow_entity(uint8_t i);
void advance_entity(uint8_t i);
bool wearing_uncursed_amulet(uint8_t subtype);
bool wearing_uncursed_ring(uint8_t subtype);
int8_t amulet_bonus(uint8_t subtype);
int8_t ring_bonus(uint8_t subtype);
bool entity_perform_action(uint8_t i, action a);
bool player_is_invisible();
bool entity_is_invisible(uint8_t i);
void aggro_monster(uint8_t i);
void end_paralysis(uint8_t i);
void end_confusion(uint8_t i);
void end_slow(uint8_t i);

// monsters.cpp
extern entity_info const MONSTER_INFO[] PROGMEM;
entity_info entity_get_info(uint8_t i);
void entity_get_info(uint8_t i, entity_info& info);
void monster_ai(uint8_t i, action& a);

// menus.cpp
void draw_yesno(uint8_t x, uint8_t y);
uint8_t inventory_menu(char const* s);
void show_high_scores(uint8_t i);
bool yesno_menui(char const* fmt, item it);
bool yesno_menu(char const* s);
bool direction_menu_nocancel(uint8_t& d, char const* s);
bool direction_menu(uint8_t& d, char const* s);
bool direction_menu(uint8_t& d);
bool repeat_action(action& a);
bool action_menu(action& a);

// stack.cpp
void stack_canary_init();
uint16_t unused_stack();

// use.cpp
bool equip_item(uint8_t i);
bool unequip_item(uint8_t i);
uint8_t slot_of_item(uint8_t type);
bool item_is_equipped(uint8_t i);
bool use_item(uint8_t i);
void entity_apply_potion(uint8_t subtype, uint8_t i);
void wand_effect(uint8_t i, uint8_t d, uint8_t subtype);

// save.cpp
void save();
void destroy_save();
bool save_valid();
void load();

static FORCEINLINE uint8_t ymask(uint8_t y)
{
    uint8_t m;
    // really reaching with the size optimizations now...
    // this saves a whole 16 bytes!!! :(
#if defined(__GNUC__) && defined(__AVR_ARCH__)
    asm volatile(
        "      ldi  %[m], 1  \n\t"
        "      andi %[y], 7  \n\t"
        "      breq L%=2     \n\t"
        "L%=1: lsl  %[m]     \n\t"
        "      dec  %[y]     \n\t"
        "      brne L%=1     \n\t"
        "L%=2:               \n\t"
        : [m] "=&d" (m),
          [y] "+d"  (y)
    );
#else
    m = 1 << (y % 8);
#endif
    return m;
}

static constexpr uint16_t SAVE_FILE_BYTES = sizeof(saved_data);
static constexpr uint16_t GAME_DATA_BYTES = sizeof(globals);
