"""Game-wide tables: names, heroes, items. Chapters live in chapters/*.txt.

make_chapters.py turns this + the chapter files into:
  story.h              (PROGMEM: names, portraits, voices, hero base stats, item table)
  fxdata/chapters.bin  (FX: every chapter's map, units, objective, dialogue)
"""

# ------------------------------------------------------------- classes
# order = class id. weapons = the item kinds this class can wield (one or two). short = HUD label.
# promotes: the class a HERO CREST turns it into (at PROMOTE_LEVEL or above).
# bonus: what promotion adds (HP STR SKL SPD DEF); only promoted classes have one.
# map sprite and forecast shield are drawn per class (tools/woodcut/make_gfx.py CLASS_SPR).
# Promoted-class names and stats are placeholders -- rename freely.
CLASSES = [
    # name         short     weapons             mov  promotes     bonus
    ('LORD',      'CAV',    ('SWORD',),          6,  'HIGH LORD'),     # Brannock rides
    ('LANCER',    'LANCE',  ('LANCE',),          5,  'HUSSAR'),
    ('FIGHTER',   'FIGHTR', ('AXE',),            5,  'REAVER'),
    ('ARCHER',    'ARCHER', ('BOW',),            5,  'RANGER'),
    ('KNIGHT',    'KNIGHT', ('LANCE',),          4,  'WARDEN'),
    ('CLERIC',    'CLERIC', ('STAFF',),          5,  'BISHOP'),    # heals with staves, never attacks
    ('HORROR',    'HORROR', ('CLAW',),           5,  None),        # demons: natural weapons
    ('CAVALIER',  'RIDER',  ('LANCE', 'SWORD'),  6,  'PALADIN'),   # Oren; Dain horse
    ('SWORDSMAN', 'SWORD',  ('SWORD',),          5,  'BLADEMASTER'),   # Cass: fast, sharp, fragile
    ('MAGE',      'MAGE',   ('TOME',),           5,  'SAGE'),      # Aelis; the Undertow's cultists. STR is their magic
    ('WRAITH',    'WRAITH', ('CLAW',),           5,  None),        # bellwraiths: what the island's old bells became
    ('SKYRIDER',  'EAGLE',  ('LANCE',),          8,  'SKY LORD'),  # Saoirse: a sea-eagle under her, the widest move
    ('MONK',      'MONK',   ('STAFF', 'TOME'),   5,  'SAGE'),      # Thane: the island's bell-hymns heal and strike
    # promoted
    ('HIGH LORD', 'H.LORD', ('SWORD', 'LANCE'),  7,  None, (4, 2, 2, 1, 2)),
    ('HUSSAR',    'HUSSAR', ('LANCE', 'SWORD'),  7,  None, (4, 2, 1, 2, 2)),
    ('REAVER',    'REAVER', ('AXE', 'BOW'),      6,  None, (5, 3, 1, 1, 1)),
    ('RANGER',    'RANGER', ('BOW', 'SWORD'),    6,  None, (3, 1, 2, 2, 1)),
    ('WARDEN',    'WARDEN', ('LANCE', 'AXE'),    5,  None, (5, 2, 1, 1, 3)),
    ('BISHOP',    'BISHOP', ('STAFF',),          6,  None, (3, 1, 2, 1, 2)),
    ('PALADIN',   'PALADN', ('LANCE', 'SWORD'),  7,  None, (4, 2, 1, 1, 2)),
    ('BLADEMASTER', 'BLADE', ('SWORD',),         6,  None, (3, 2, 3, 2, 1)),
    ('SAGE',      'SAGE',   ('TOME', 'STAFF'),   6,  None, (3, 2, 2, 1, 2)),   # casts and heals
    ('SKY LORD',  'S.LORD', ('LANCE', 'SWORD'),  9,  None, (3, 2, 2, 2, 1)),
]
PROMOTE_LEVEL = 10

# ------------------------------------------------------------- names
# id = position. Heroes MUST be first, in roster order (hero slot = id); up to 16 heroes.
# portrait: FX portrait frame (None = none). voice: 0 low, 1 mid, 2 villain growl, 3 demon.
# figure: battle figure pair index in fxdata figures (idle, strike). A hero who can be fought
# before joining (talk-recruit) needs a mirrored pair right after theirs: figure + 1.
NAMES = [
    # name        portrait voice figure
    ('BRANNOCK',  0,       0,    0),
    ('IDRA',      2,       1,    1),
    ('HALE',      3,       0,    2),
    ('WREN',      4,       1,    3),
    ('MAREN',     5,       1,    8),      # priestess of the mill chapel; recruited in chapter 2
    ('OREN',      12,      0,    10),     # Dain cavalry officer who won't burn villages (Ch 4)
    ('CASS',      13,      1,    12),     # sellsword, Hale's old company-mate (Ch 6)
    ('AELIS',     16,      1,    17),     # princess of Dain, reads the old texts; a mage (Ch 9)
    ('ROOK',      21,      0,    23),     # smuggler captain with the only ship for Vael (Ch 11); fought first: 23 + mirror 24
    ('SAOIRSE',   25,      1,    26),     # sea-eagle rider of Vael, the woman Rook crossed the sea for (Ch 14); 26 + mirror 27
    ('THANE',     26,      0,    28),     # the cliff village's monk, locked in his chapel (Ch 14)
    ('HESK',      27,      0,    29),     # walks the salt flats killing anything that moves (Ch 16); 29 + mirror 30
    ('LIR',       28,      3,    31),     # Old Tam's grandson, taken in the raids: one of the Drowned (17X)
    ('ODA',       32,      0,    32),     # the last bellwright, a hermit by the old foundry in the marsh (19X)
    ('RAIDER',    None,    1,    4),
    ('BOWMAN',    None,    1,    5),
    ('SPEAR',     None,    1,    6),
    ('OSK',       1,       2,    7),
    ('VARGA',     8,       2,    4),      # raider captain (reuses the raider battle figure)
    ('OLD TAM',   6,       0,    4),      # villagers: speak, never fight
    ('GOODY ASH', 7,       1,    4),
    ('BANDIT',    None,    1,    4),      # stronghold rank and file (raider figure)
    ('KELL',      9,       2,    4),      # bandit chief of the Red Palisade
    ('VOSS',      10,      2,    5),      # the paymaster: hooded, a crossbow (bowman figure)
    ('DROWNED',   None,    3,    9),      # the first demon
    ('FENN',      11,      0,    4),      # a trapper outside the palisade (villager)
    ('CORVANE',   14,      2,    16),     # Dain's spymaster (5X)
    ('GERAULT',   15,      0,    15),     # Dain's marshal, an honest soldier (Ch 6)
    ('RIDER',     None,    1,    14),     # Dain cavalry
    ('SOLDIER',   None,    1,    6),      # Dain foot (spear figure)
    ('ARCHER',    None,    1,    5),      # Dain bow (bowman figure)
    ('COURIER',   None,    1,    5),      # carries Corvane's orders (Ch 5)
    ('CAPTAIN',   None,    1,    14),     # a Dain riding captain (Ch 4)
    ('GUARD',     None,    1,    6),      # Corvane's men (5X)
    ('CULTIST',   None,    1,    18),     # the Undertow: robed callers (Act III)
    ('MOTHER SILT', 17,    2,    19),     # high priestess of the Undertow (Ch 10)
    ('KING ALDRIC', 18,    2,    20),     # Dain's king, worn by a demon (Ch 8)
    ('HOLLOW KING', 19,    3,    20),     # the thing inside him, once it shows itself
    ('ROYAL GUARD', None,  1,    6),      # the palace's spears (Ch 7-8)
    ('PRISONER',  20,      0,    4),      # in the river-gate cells (Ch 7)
    ('BELLWRAITH', None,   3,    21),     # the sea stack's guardians (10X)
    ('PIRATE',    None,    1,    4),      # cult-crewed galley hands (Ch 12) -- raider figure
    ('SEA HORROR', None,   3,    22),     # things that climb out of the sea (Ch 12)
    ('BLACK MARL', 22,     2,    25),     # the galley's captain (Ch 12)
    ('DOCKMASTER', 23,     2,    6),      # the Undertow's man on the Saltmarch docks (Ch 11)
    ('DOCKHAND',  None,    1,    6),      # his hired spears
    ('ISLANDER',  24,      0,    4),      # hiding in the dunes on Vael (Ch 13)
    ('BRIGID',    29,      1,    4),      # the island's rebel leader (13X, 15)
    ('ZEALOT',    None,    1,    6),      # the Undertow's island garrison (spear figure)
    ('WATCHMAN',  None,    1,    5),      # its bowmen
    ('VEY',       30,      2,    15),     # captain of the garrison (Ch 15)
    ('HORNED',    None,    3,    22),     # the demons of the flats
    ('STILL ONE', 28,      3,    9),      # a Drowned who stands apart (Ch 17): Lir, though no one knows it yet
    ('GATEKEEPER', 31,     3,    22),     # what guards the first gate (Ch 17)
    ('VILLAGER',  None,    0,    4),      # the cliff village (Ch 14)
    ('NHAL',      33,      3,    33),     # what pulls at the world from under the drowned cathedral (Ch 22)
]
NUM_HEROES = 14

# ------------------------------------------------------------- items
# kind: SWORD LANCE AXE BOW CLAW (weapons), STAFF, HEAL (might = HP restored), PROMO (class change).
ITEMS = [
    # Common weapons and staves (uses 1) never wear out. Fine ones -- silver, killer, thunder,
    # mend -- have uses and break when they run out. HEAL and PROMO items are used up too.
    # name           kind     mt  hit crt minR maxR uses
    ('-',            'HEAL',   0,   0,  0, 0, 0,  0),   # 0 = empty slot
    ('IRON SWORD',   'SWORD',  5,  90,  0, 1, 1,  1),
    ('IRON LANCE',   'LANCE',  6,  80,  0, 1, 1,  1),
    ('IRON AXE',     'AXE',    7,  75,  0, 1, 1,  1),
    ('IRON BOW',     'BOW',    6,  85,  0, 2, 2,  1),
    ('STEEL SWORD',  'SWORD',  8,  75,  0, 1, 1,  1),
    ('JAVELIN',      'LANCE',  4,  65,  0, 1, 2,  1),
    ('HAND AXE',     'AXE',    5,  60,  0, 1, 2,  1),
    ('KILLER BOW',   'BOW',    7,  75, 30, 2, 2, 15),
    ('VULNERARY',    'HEAL',  10,   0,  0, 0, 0,  3),
    ('ELIXIR',       'HEAL',  99,   0,  0, 0, 0,  1),
    ('HEAL',         'STAFF', 10,   0,  0, 1, 1,  1),   # might = HP restored to an adjacent ally
    ('CLAWS',        'CLAW',   6,  75,  5, 1, 1,  1),   # demons only
    ('STEEL LANCE',  'LANCE',  9,  70,  0, 1, 1,  1),
    ('HERO CREST',   'PROMO',  0,   0,  0, 0, 0,  1),   # promotes a hero at PROMOTE_LEVEL+
    ('HOUSE BLADE',  'SWORD',  6,  85,  5, 1, 1,  1),   # Brannock's family sword
    ('STEEL AXE',    'AXE',   10,  65,  0, 1, 1,  1),
    ('STEEL BOW',    'BOW',    9,  70,  0, 2, 2,  1),
    ('SILVER SWORD', 'SWORD', 12,  80,  0, 1, 1, 20),   # fine weapons wear out: uses > 1
    ('SILVER LANCE', 'LANCE', 13,  75,  0, 1, 1, 20),
    ('SILVER AXE',   'AXE',   14,  70,  0, 1, 1, 20),
    ('SILVER BOW',   'BOW',   12,  75,  0, 2, 2, 20),
    # tomes: magic strikes at range 1-2 and counts only half the target's DEF (STR is the caster's power)
    ('FIRE',         'TOME',   5,  90,  0, 1, 2,  1),
    ('THUNDER',      'TOME',   8,  75, 10, 1, 2, 20),
    ('MIRE',         'TOME',   6,  80,  0, 1, 2,  1),   # the Undertow's drowning spell
    ('MEND',         'STAFF', 20,   0,  0, 1, 1, 15),   # a stronger heal
    ('HYMN',         'TOME',   6,  85,  0, 1, 2,  1),   # Vael's bell-hymn: Thane's, the Drowned hate it
    # boosters: used up for good. mt = amount, hit = the stat (0 HP 1 STR 2 SKL 3 SPD 4 DEF 5 MOV)
    # eight in the whole game, hidden where exploring pays: never sold
    ('OX HORN',      'BOOST',  2,   1,  0, 0, 0,  1),
    ('HAWK FEATHER', 'BOOST',  2,   2,  0, 0, 0,  1),
    ("HARE'S FOOT",  'BOOST',  2,   3,  0, 0, 0,  1),
    ('IRON BARK',    'BOOST',  2,   4,  0, 0, 0,  1),
    ('HEART ROOT',   'BOOST',  5,   0,  0, 0, 0,  1),
    ('WINGED BOOTS', 'BOOST',  1,   5,  0, 0, 0,  1),   # +1 MOV: the only pair
    # new items go at the end: a save stores item ids, so the order above must never change
    ('UNDERTOW',     'TOME',  10,  85,  5, 1, 2,  1),   # Nhal's: the pull itself (never sold, never dropped)
]
# shop prices in gold (multiples of 10, at most 2550). Only priced items can be put on sale.
PRICES = {'VULNERARY': 300, 'IRON SWORD': 460, 'IRON LANCE': 560, 'IRON AXE': 420, 'IRON BOW': 540,
          'JAVELIN': 400, 'HAND AXE': 380, 'STEEL SWORD': 900, 'STEEL LANCE': 1000, 'STEEL AXE': 960,
          'STEEL BOW': 960, 'FIRE': 600, 'HEAL': 600, 'ELIXIR': 2000}
KINDS = ['SWORD', 'LANCE', 'AXE', 'BOW', 'HEAL', 'STAFF', 'CLAW', 'PROMO', 'TOME', 'BOOST']

# ------------------------------------------------------------- heroes at the start of the game
# lord: the unit whose death ends the game. joined=False: not in the army until recruited.
# items: up to 4 names. growth %: HP STR SKL SPD DEF
# death: last words when they fall (speaker, text) -- the lord's are the chapter's 'lose' scene instead
HEROES = {
    'BRANNOCK': dict(cls='LORD',    lv=1, hp=20, str=6, skl=7, spd=8, df=5, lord=True,
                     items=['HOUSE BLADE', 'VULNERARY'], growth=(80, 45, 50, 50, 35)),
    'IDRA':     dict(cls='LANCER',  lv=1, hp=18, str=6, skl=6, spd=7, df=6,
                     items=['IRON LANCE', 'JAVELIN'],    growth=(70, 45, 45, 55, 30),
                     death=[('IDRA', "BRANNOCK... KEEP/YOUR GUARD UP./I CAN'T DO IT/FOR YOU NOW...")]),
    'HALE':     dict(cls='FIGHTER', lv=1, hp=24, str=8, skl=4, spd=5, df=4,
                     items=['IRON AXE', 'HAND AXE'],     growth=(90, 60, 30, 25, 30),
                     death=[('HALE', "HA... ALWAYS SAID/I'D GO OUT/LAUGHING. THIS/ISN'T... FUNNY.")]),
    'WREN':     dict(cls='ARCHER',  lv=1, hp=16, str=5, skl=7, spd=6, df=3,
                     items=['IRON BOW', 'VULNERARY'],    growth=(55, 40, 60, 50, 20),
                     death=[('WREN', 'TELL MY FATHER.../I HIT WHAT I/AIMED AT. MOST/OF THE TIME.')]),
    'MAREN':    dict(cls='CLERIC',  lv=2, hp=17, str=1, skl=5, spd=6, df=2, joined=False,
                     items=['HEAL', 'VULNERARY'],  growth=(50, 10, 45, 50, 15),
                     death=[('MAREN', 'THE BELL... ONE/OF YOU HAS TO/RING THE BELL...')]),
    # recruited by talking to them mid-battle: they join with the stats their enemy line gives them
    'OREN':     dict(cls='CAVALIER', lv=4, hp=23, str=7, skl=6, spd=6, df=6, joined=False,
                     items=['IRON LANCE', 'JAVELIN'],    growth=(75, 45, 45, 40, 35),
                     death=[('OREN', 'DAIN, FORGIVE ME./HOLLIN... YOU/TOO. I TRIED.')]),
    'CASS':     dict(cls='SWORDSMAN', lv=5, hp=20, str=6, skl=10, spd=11, df=4, joined=False,
                     items=['STEEL SWORD', 'VULNERARY'], growth=(60, 35, 60, 65, 20),
                     death=[('CASS', "SHOULD'VE ASKED/FOR MORE COIN.../HALE. YOU STILL/OWE ME A DRINK.")]),
    'ROOK':     dict(cls='FIGHTER', lv=10, hp=33, str=12, skl=9, spd=9, df=7, joined=False,
                     items=['STEEL AXE', 'HAND AXE'], growth=(80, 55, 40, 40, 30),
                     death=[('ROOK', "SAOIRSE... I GOT/CLOSE. I GOT SO/CLOSE.")]),
    'SAOIRSE':  dict(cls='SKYRIDER', lv=12, hp=30, str=11, skl=11, spd=14, df=7, joined=False,
                     items=['STEEL LANCE', 'JAVELIN'], growth=(65, 40, 50, 60, 25),
                     death=[('SAOIRSE', 'ROOK... THE SKY/WAS SO WIDE./TELL HIM I FLEW.')]),
    'THANE':    dict(cls='MONK', lv=10, hp=26, str=7, skl=8, spd=8, df=6, joined=False,
                     items=['HYMN', 'HEAL'], growth=(60, 35, 45, 40, 30),
                     death=[('THANE', 'RING IT... FOR/ME TOO. THE/LONG BELL...')]),
    'HESK':     dict(cls='REAVER', lv=4, hp=40, str=17, skl=10, spd=10, df=9, joined=False,
                     items=['SILVER AXE', 'HAND AXE'], growth=(85, 55, 35, 35, 30),
                     death=[('HESK', "QUIET... AT LAST./IT'S SO QUIET.")]),
    'LIR':      dict(cls='HORROR', lv=12, hp=32, str=12, skl=7, spd=9, df=8, joined=False,
                     items=['CLAWS', 'VULNERARY'], growth=(70, 45, 35, 40, 35),
                     death=[('LIR', "TELL GRANDDAD.../I WASN'T AFRAID./NOT AT THE END.")]),
    'ODA':      dict(cls='FIGHTER', lv=15, hp=38, str=14, skl=10, spd=6, df=10, joined=False,
                     items=['STEEL AXE', 'HAND AXE', 'VULNERARY'], growth=(70, 50, 40, 20, 35),
                     death=[('ODA', "THE BRONZE.../DON'T LET IT/COOL WRONG. THE/BELL MUST RING/TRUE.")]),
    'AELIS':    dict(cls='MAGE', lv=6, hp=19, str=8, skl=7, spd=8, df=3, joined=False,
                     items=['FIRE', 'VULNERARY'],        growth=(50, 55, 45, 50, 20),
                     death=[('AELIS', "FATHER... I'M/SORRY. I READ/TOO SLOWLY...")]),
}
