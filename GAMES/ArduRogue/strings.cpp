#include "game.hpp"

char const STR_YOU_P_THE_I[] PROGMEM = STRI_YOU "@p " STRI_THE "@i.";
char const STR_ARROW[] PROGMEM = "arrow";
char const STR_THE[] PROGMEM = "the ";
char const STR_CAPTHE[] PROGMEM = "The ";
char const STR_AMULET[] PROGMEM = "amulet";
char const STR_ARDUROGUE[] PROGMEM = "ArduRogue";

static char const NRNG_SEE_INVIS[] PROGMEM = "see invisible";
char const* const STR_INVISIBLE = NRNG_SEE_INVIS + 4; // merged with "see invisible"

static char const STR_STRI_WHICH_ITEM_Q[] PROGMEM = " which item?";
static char const STR_STRI_FOR_A_MOMENT_P[] PROGMEM = " for a moment.";
static char const STR_STRI_THE_I_GLOWS[] PROGMEM = STRI_CAPTHE "@i glows ";
static char const STR_STRI_STARVING[] PROGMEM = "starving";
static char const STR_STRI_TRENGTH[] PROGMEM = "trength";
static char const STR_STRI_YOU_ARE[] PROGMEM = STRI_YOU "are ";
static char const STR_STRI_YOU_ARE_UNABLE_TO[] PROGMEM = STRI_YOU_ARE "unable to ";
static constexpr char const* STR_STRI_P_THE_I = STR_YOU_P_THE_I + 1;
static constexpr char const* STR_STRI_ARROW = STR_ARROW;
static char const STR_STRI_YOU_FEEL[] PROGMEM = STRI_YOU "feel ";
static char const STR_STRI_YOU_CANT_HOLD_ANY_MORE[] PROGMEM = STRI_YOU "can't hold any more ";
static char const STR_STRI_YOU[] PROGMEM = "You ";
static char const STR_STRI_THE_DOOR_IS[] PROGMEM = STRI_CAPTHE "door is ";
static char const STR_STRI_THE_AMULET_OF_YENDOR[] PROGMEM = " " STRI_THE STRI_AMULET " of Yendor";
static char const STR_STRI_YOU_HAVE[] PROGMEM = STRI_YOU "have ";
static char const STR_STRI_TO_THE_SURFACE[] PROGMEM = " to " STRI_THE "surface";
static char const STR_STRI_SCAPED_WITH[] PROGMEM = "scaped with";
static constexpr char const* STR_STRI_THE = STR_THE;
static constexpr char const* STR_STRI_AMULET = STR_AMULET;
static constexpr char const* STR_STRI_CAPTHE = STR_CAPTHE;
static constexpr char const* STR_STRI_INVISIBLE = NRNG_SEE_INVIS + 4;
static char const STR_STRI_DROP_THE_I[] PROGMEM = "drop " STRI_THE "@i.";
static constexpr char const* STR_STRI_ARDUROGUE = STR_ARDUROGUE;
static char const STR_STRI_WELCOME[] PROGMEM = "Welcome ";
static char const STR_STRI_YOU_DISCOVER_IT[] PROGMEM = STRI_YOU "discover it ";

char const* const STRI_STRS[] PROGMEM =
{
    STR_STRI_WHICH_ITEM_Q,
    STR_STRI_FOR_A_MOMENT_P,
    STR_STRI_THE_I_GLOWS,
    STR_STRI_STARVING,
    STR_STRI_TRENGTH,
    STR_STRI_YOU_ARE,
    STR_STRI_YOU_ARE_UNABLE_TO,
    STR_STRI_P_THE_I,
    STR_STRI_ARROW,
    STR_STRI_YOU_FEEL,
    STR_STRI_YOU_CANT_HOLD_ANY_MORE,
    STR_STRI_YOU,
    STR_STRI_THE_DOOR_IS,
    STR_STRI_THE_AMULET_OF_YENDOR,
    STR_STRI_YOU_HAVE,
    STR_STRI_TO_THE_SURFACE,
    STR_STRI_SCAPED_WITH,
    STR_STRI_THE,
    STR_STRI_AMULET,
    STR_STRI_CAPTHE,
    STR_STRI_INVISIBLE,
    STR_STRI_DROP_THE_I,
    STR_STRI_ARDUROGUE,
    STR_STRI_WELCOME,
    STR_STRI_YOU_DISCOVER_IT,
};

char const STR_EMPTY[] PROGMEM = "";
char const STR_YOU_FEEL_STRONGER[] PROGMEM = STRI_YOU_FEEL "stronger.";
char const STR_YOU_FEEL_MORE_AGILE[] PROGMEM = STRI_YOU_FEEL "more agile.";
char const STR_NOTHING_HAPPENS[] PROGMEM = "Nothing happens.";
char const STR_CONFUSED[] PROGMEM = "confused";
char const STR_PARALYZED[] PROGMEM = "paralyzed";
char const STR_SLOWED[] PROGMEM = "slowed";
char const STR_WEAKENED[] PROGMEM = "weakened";
char const STR_HUNGRY[] PROGMEM = "hungry";
char const* const STR_STARVING = STR_STRI_STARVING;

char const STR_YOU_DISCOVER_IT_WAS_A_I[] PROGMEM = STRI_YOU_DISCOVER_IT "was a @i.";

static char const MON_BAT        [] PROGMEM = "bat";
static char const MON_RATTLESNAKE[] PROGMEM = "rattlesnake";
static constexpr char const* MON_SNAKE = MON_RATTLESNAKE + 6;
static char const MON_ZOMBIE     [] PROGMEM = "zombie";
static char const MON_GOBLIN     [] PROGMEM = "goblin";
static char const MON_PHANTOM    [] PROGMEM = "phantom";
static char const MON_ORC        [] PROGMEM = "orc";
static char const MON_TARANTULA  [] PROGMEM = "tarantula";
static char const MON_HOBGOBLIN  [] PROGMEM = "hobgoblin";
static char const MON_MIMIC      [] PROGMEM = "mimic";
static char const MON_INCUBUS    [] PROGMEM = "incubus";
static char const MON_TROLL      [] PROGMEM = "troll";
static char const MON_GRIFFIN    [] PROGMEM = "griffin";
static char const MON_DRAGON     [] PROGMEM = "dragon";
static char const MON_ANGEL      [] PROGMEM = "fallen angel";
static char const MON_DARKNESS   [] PROGMEM = "Lord of Darkness";

char const* const MONSTER_NAMES[] PROGMEM =
{
    nullptr, // none
    nullptr, // player
    MON_BAT,
    MON_SNAKE,
    MON_RATTLESNAKE,
    MON_ZOMBIE,
    MON_GOBLIN,
    MON_PHANTOM,
    MON_ORC,
    MON_TARANTULA,
    MON_HOBGOBLIN,
    MON_MIMIC,
    MON_INCUBUS,
    MON_TROLL,
    MON_GRIFFIN,
    MON_DRAGON,
    MON_ANGEL,
    MON_DARKNESS,
};

static char const NSCR_MASS_CONFUSE[] PROGMEM = "mass confusion";
static char const NSCR_MASS_POISON[] PROGMEM = "mass poison";

static char const NPOT_HEALING[] PROGMEM = "healing";
static constexpr char const* NPOT_CONFUSION = NSCR_MASS_CONFUSE + 5;
static constexpr char const* NPOT_POISON = NSCR_MASS_POISON + 5;
static char const NPOT_HARMING[] PROGMEM = "harming";
static char const NPOT_STRENGTH[] PROGMEM = "s" STRI_TRENGTH;
static char const NPOT_DEXTERITY[] PROGMEM = "dexterity";
static char const NPOT_PARALYSIS[] PROGMEM = "paralysis";
static char const NPOT_SLOWING[] PROGMEM = "slowing";
static char const NPOT_EXPERIENCE[] PROGMEM = "experience";
static char const NPOT_INVIS[] PROGMEM = "invisibility";

char const* const POT_NAMES[NUM_POT] PROGMEM =
{
    NPOT_HEALING,
    NPOT_CONFUSION,
    NPOT_POISON,
    NPOT_HARMING,
    NPOT_STRENGTH,
    NPOT_DEXTERITY,
    NPOT_PARALYSIS,
    NPOT_SLOWING,
    NPOT_EXPERIENCE,
    NPOT_INVIS,
};

static char const UNID_POT_0[] PROGMEM = "red";
static char const UNID_POT_1[] PROGMEM = "clear";
static char const UNID_POT_2[] PROGMEM = "orange";
static char const UNID_POT_3[] PROGMEM = "green";
static char const UNID_POT_4[] PROGMEM = "blue";
static char const UNID_POT_5[] PROGMEM = "white";
static char const UNID_POT_6[] PROGMEM = "yellow";
static char const UNID_POT_7[] PROGMEM = "violet";
static char const UNID_POT_8[] PROGMEM = "black";
static char const UNID_POT_9[] PROGMEM = "pink";

char const* const UNID_POT_NAMES[NUM_POT] PROGMEM =
{
    UNID_POT_0,
    UNID_POT_1,
    UNID_POT_2,
    UNID_POT_3,
    UNID_POT_4,
    UNID_POT_5,
    UNID_POT_6,
    UNID_POT_7,
    UNID_POT_8,
    UNID_POT_9,
};

static char const NSCR_IDENTIFY[] PROGMEM = "identify";
static char const NSCR_ENCHANT[] PROGMEM = "enchanting";
static char const NSCR_REMOVE_CURSE[] PROGMEM = "remove curse";
static char const NSCR_TELEPORT[] PROGMEM = "teleportation";
static char const NSCR_MAPPING[] PROGMEM = "magic mapping";
static char const NSCR_FEAR[] PROGMEM = "fear";
static char const NSCR_TORMENT[] PROGMEM = "torment";

char const* const SCR_NAMES[NUM_SCR] PROGMEM =
{
    NSCR_IDENTIFY,
    NSCR_ENCHANT,
    NSCR_REMOVE_CURSE,
    NSCR_TELEPORT,
    NSCR_MAPPING,
    NSCR_FEAR,
    NSCR_TORMENT,
    NSCR_MASS_CONFUSE,
    NSCR_MASS_POISON,
};

static char const UNID_SCR_0[] PROGMEM = "faded";
static char const UNID_SCR_1[] PROGMEM = "yellowed";
static char const UNID_SCR_2[] PROGMEM = "tattered";
static char const UNID_SCR_3[] PROGMEM = "glowing";
static char const UNID_SCR_4[] PROGMEM = "shimmering";
static char const UNID_SCR_5[] PROGMEM = "humming";
static char const UNID_SCR_6[] PROGMEM = "dark";
static char const UNID_SCR_7[] PROGMEM = "bright";
static char const UNID_SCR_8[] PROGMEM = "brilliant";

char const* const UNID_SCR_NAMES[NUM_SCR] PROGMEM =
{
    UNID_SCR_0,
    UNID_SCR_1,
    UNID_SCR_2,
    UNID_SCR_3,
    UNID_SCR_4,
    UNID_SCR_5,
    UNID_SCR_6,
    UNID_SCR_7,
    UNID_SCR_8,
};

static constexpr char const* NRNG_STRENGTH = NPOT_STRENGTH;
static constexpr char const* NRNG_DEXTERITY = NPOT_DEXTERITY;
static char const NRNG_PROTECTION[] PROGMEM = "protection";
static char const NRNG_FIRE_PROTECT[] PROGMEM = "fire immunity";
static char const NRNG_ATTACK[] PROGMEM = "attack";
static char const NRNG_SUSTENANCE[] PROGMEM = "sustenance";
static constexpr char const* NRNG_INVIS = NPOT_INVIS;

char const* const RNG_NAMES[NUM_RNG] PROGMEM =
{
    NRNG_SEE_INVIS,
    NRNG_STRENGTH,
    NRNG_DEXTERITY,
    NRNG_PROTECTION,
    NRNG_FIRE_PROTECT,
    NRNG_ATTACK,
    NRNG_SUSTENANCE,
    NRNG_INVIS,
};

static char const NAMU_SPEED[] PROGMEM = "speed";
static char const NAMU_CLARITY[] PROGMEM = "clarity";
static char const NAMU_CONSERVATION[] PROGMEM = "conservation";
static char const NAMU_REGENERATION[] PROGMEM = "regeneration";
static char const NAMU_VAMPIRE[] PROGMEM = STRI_THE "vampire";
static char const NAMU_IRONBLOOD[] PROGMEM = "ironblood";
static char const NAMU_VITALITY[] PROGMEM = "vitality";
static char const NAMU_WISDOM[] PROGMEM = "wisdom";
static char const NAMU_YENDOR[] PROGMEM = "Yendor";

char const* const AMU_NAMES[NUM_AMU + 1] PROGMEM =
{
    NAMU_SPEED,
    NAMU_CLARITY,
    NAMU_CONSERVATION,
    NAMU_REGENERATION,
    NAMU_VAMPIRE,
    NAMU_IRONBLOOD,
    NAMU_VITALITY,
    NAMU_WISDOM,
    NAMU_YENDOR,
};

static char const UNID_RNG_AMU_0[] PROGMEM = "diamond";
static char const UNID_RNG_AMU_1[] PROGMEM = "ruby";
static char const UNID_RNG_AMU_2[] PROGMEM = "emerald";
static char const UNID_RNG_AMU_3[] PROGMEM = "topaz";
static char const UNID_RNG_AMU_4[] PROGMEM = "gold";
static char const UNID_RNG_AMU_5[] PROGMEM = "silver";
static char const UNID_RNG_AMU_6[] PROGMEM = "platinum";
static char const UNID_RNG_AMU_7[] PROGMEM = "iron";

char const* const UNID_RNG_AMU_NAMES[NUM_RNG < NUM_AMU ? NUM_AMU : NUM_RNG] PROGMEM =
{
    UNID_RNG_AMU_0,
    UNID_RNG_AMU_1,
    UNID_RNG_AMU_2,
    UNID_RNG_AMU_3,
    UNID_RNG_AMU_4,
    UNID_RNG_AMU_5,
    UNID_RNG_AMU_6,
    UNID_RNG_AMU_7,
};

static char const  NWND_FORCE[] PROGMEM = "force";
static constexpr char const*  NWND_TELEPORT = NSCR_TELEPORT;
static char const  NWND_DIGGING[] PROGMEM = "digging";
static char const  NWND_FIRE[] PROGMEM = "fire";
static char const  NWND_STRIKING[] PROGMEM = "striking";
static char const  NWND_ICE[] PROGMEM = "ice";
static char const  NWND_POLYMORPH[] PROGMEM = "polymorph";

char const* const WND_NAMES[NUM_WND] PROGMEM =
{
    NWND_FORCE,
    NWND_TELEPORT,
    NWND_DIGGING,
    NWND_FIRE,
    NWND_STRIKING,
    NWND_ICE,
    NWND_POLYMORPH,
};

static char const UNID_WND_0[] PROGMEM = "long";
static char const UNID_WND_1[] PROGMEM = "short";
static char const UNID_WND_2[] PROGMEM = "slender";
static char const UNID_WND_3[] PROGMEM = "thick";
static char const UNID_WND_4[] PROGMEM = "twisted";
static char const UNID_WND_5[] PROGMEM = "curved";
static char const UNID_WND_6[] PROGMEM = "glossy";

char const* const UNID_WND_NAMES[NUM_WND] PROGMEM =
{
    UNID_WND_0,
    UNID_WND_1,
    UNID_WND_2,
    UNID_WND_3,
    UNID_WND_4,
    UNID_WND_5,
    UNID_WND_6,
};

static char const INV_CAT_WEAPON[] PROGMEM = "Weapons";
static char const INV_CAT_ARMOR[] PROGMEM = "Armor";
static char const INV_CAT_WAND[] PROGMEM = "Wands";
static char const INV_CAT_POTION[] PROGMEM = "Potions";
static char const INV_CAT_SCROLL[] PROGMEM = "Scrolls";
static char const INV_CAT_JEWELRY[] PROGMEM = "Jewelry";
static char const INV_CAT_MISC[] PROGMEM = "Misc";

char const* const INV_CATEGORIES[] PROGMEM =
{
    INV_CAT_WEAPON,
    INV_CAT_ARMOR,
    INV_CAT_WAND,
    INV_CAT_POTION,
    INV_CAT_SCROLL,
    INV_CAT_JEWELRY,
    INV_CAT_MISC,
};
