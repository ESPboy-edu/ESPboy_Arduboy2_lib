CHAPTER FILES -- one per chapter, played in file-name order.
Build: sh tools/build_data.sh  (stops with file:line on any mistake)
Examples: 03_red_palisade.txt (tripwire), 04_grey_road.txt (side chapter,
ambush), 09_dainholt.txt (a locked/unlocked door: needs 9 / needs !9),
13_hideout.txt (portal pools), tools/sim/test_chapters/ (portal, talk-recruit,
reinforcements, joins).

title: NAME              shown on the chapter card (14 characters max)
goal: TEXT               the objective plank on the card (16 max)
label: 3X                optional: the chapter's number on cards and the title
                         screen (default: the previous chapter's + 1)
requires: flag N         optional: only played if story flag N (1..16) was set
                         earlier; otherwise the campaign skips straight past it
joins: NAME, NAME        optional: heroes (joined=False in game_data) who join
                         the army as this chapter opens (they can deploy)
objective: seize X Y     lord stands on X Y and picks SEIZE
           rout          defeat every enemy (ambushers yet to appear count)
           boss          defeat the enemy flagged 'boss'
           survive N     last until the end of turn N
shop: ITEM, ITEM, ...     optional: up to 6 items on sale at the prep screen (a town).
                         Each needs a price in game_data.PRICES; goods go to the wagon
reward: N                optional: gold for clearing it, in tens (default 200 + 50 x
                         the chapter's number)
deploy: N                heroes allowed on the field, up to 8 (the deployment
                         screen appears when the army is bigger than this)
captive: NAME X Y [leaves T]
                         a hero (joined=False in game_data) waiting on the
                         map. Any hero next to them can TALK -> they join.
                         'leaves T': gone after the enemy phase of turn T.
                         Needs 'scene recruit'; 'leaves' needs 'scene taken'
                         ('scene warn' plays at the start of turn T).

map:                     24 x 16 rows of:
  . plain  T wood  h hill  ~ water  # wall  K keep  = ford  V village
  C chest  P palisade (impassable)  G gate  O portal  _ floor (halls, vaults, caves)
  D deck (ships, piers)  R rail (a ship's side, a mast: impassable)

slots:                   'x y' per line, up to 8; the first is the lord's

enemies:                 name class x y lv hp str skl spd def [flags] [items]
  a name with a space is written with _ (ROYAL_GUARD). For MAGE/SAGE, STR is their magic.
  up to 10 per chapter (RAM). Reinforcements reuse slots: a 'spawn' brings a
  fallen group member back fresh at its post.
  flags: boss     EXP bonus, speaks before the first fight
         hold     never moves
         reserveG asleep in group G (default 1): drawn greyed, never acts,
                  until an event wakes group G or a hero strikes one
         ambushG  in group G (default 1), not on the map until an event
                  spawns group G; appears on its tile or the first free one
                  beside it
         gG       an ordinary enemy in group G (so events can respawn it)
  items: up to 4, comma separated; ITEM* = dropped to whoever fells them
  A recruitable enemy carries a hero's name (a hero with joined=False) and
  needs a 'talk ... to NAME ; join' event.
  An enemy with 'hold' and no items is a bystander (never attacks) -- e.g. someone
  to TALK to.

village X Y:             one block per V tile. Dialogue lines, then
gift: ITEM               optional gift to the visitor (to the wagon if their pack is full)
gold: N                  ...or a purse instead: 10..1270, in tens
flag: N                  optional: visiting sets flag N
chest X Y:               one block per C tile: same as a village (OPEN in
                         the unit menu). Up to 4 villages + chests per chapter.

EVENTS (up to 8 per chapter)
event NAME: CONDITION ; ACTION ; ACTION ...
  conditions:
    tile X Y             a hero walks over X Y (passing through counts;
                         for 'warp', the hero has to stop on it)
    turn N [every K]     the start of turn N (N >= 2), and every K turns after
    falls UNIT           that enemy falls (its name must be unique here)
    talk HERO to UNIT    HERO (or 'anyone') stands next to UNIT and picks TALK
  actions:
    wake G               group G's reserves wake
    spawn G              group G appears: ambushers arrive, fallen members
                         come back fresh at their posts
    warp X Y             (tile events) the hero is moved to X Y, every time
    join                 (talk events) the enemy joins the army as that hero,
                         with the stats it had as an enemy
    set N                sets flag N
    until N              the event stops once flag N is set
    needs N              the event only fires while flag N is set ('needs !N': while it
                         isn't). Two events on one tile, 'needs 9' and 'needs !9', give
                         a locked door and an open one
    win                  the chapter is won
    repeat               fire every time (tile events) -- turn events with
                         'every K' repeat already
    demon                the demon motif plays (otherwise wake/spawn ring
                         the alarm bell)
  scene NAME:            optional dialogue played when event NAME fires
  Events fire once unless they repeat. When several fire at once, all their
  actions happen and the first one's scene plays. A repeating event plays its
  scene only the first time.

FLAGS
  1..16  story flags: saved with the army; chests/villages/events set them,
         'requires: flag N' reads them (side chapters, later recruits...)
  17..24 chapter flags: cleared when each chapter starts (e.g. 'falls KELL ;
         set 17' with 'turn 2 every 2 ; spawn 2 ; until 17')

scene open|boss|win|lose|recruit|warn|taken|<event name>:
SPEAKER: text            '/' breaks a line; 17 characters per line,
                         5 lines per box. Speakers need a portrait.
[N] SPEAKER: text        only if flag N is set      [!N] only if it isn't
[HERO] SPEAKER: text     only if HERO is in the army (joined, not fallen)
[!HERO] SPEAKER: text    only if they aren't
[FELL:HERO] SPEAKER: ... only if HERO joined and fell (a roll of the dead,
                         a line remembering them)   [!FELL:HERO] if not
[A][B] SPEAKER: text     every condition must hold (stack as many as you
                         like: each extra one costs a line of the 255 a
                         scene can hold)
                         Works in every scene and village. Use it for
                         endings, and to tell the player what a skipped
                         side chapter would have shown them. A speaker who
                         may be gone needs their own [HERO] too.
