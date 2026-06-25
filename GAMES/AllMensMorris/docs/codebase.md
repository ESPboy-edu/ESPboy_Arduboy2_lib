# Codebase Notes

## Current Shape

The sketch entrypoint is intentionally small:

```text
all-mens-morris.ino -> gameSetup() / gameLoop()
```

The implementation is split into:

- `src/Game.cpp`: Arduboy setup, custom boot animation, input, rendering,
  sound/RGB feedback, main menu, quick menu, CPU turn animation/dispatch, and
  current UI.
- `src/Ai.*`: generic CPU opponent. It enumerates legal rule actions, simulates
  them on `MorrisGameState`, and scores the resulting state without knowing
  about screen coordinates. Easy uses the direct heuristic, while Hard reuses
  the same evaluator through a depth-2 minimax pass. `Game.cpp` owns the
  visible cursor animation before applying the chosen result.
- `src/Board.*`: board helper APIs for compact `BoardDefinition` graph
  coordinates, packed mill triples, and flat adjacency lists. Adjacency is both
  the visible board connection and the legal movement edge.
- `src/Rules.*`: mutable game state, `RuleSet` configuration, legal actions,
  mill capture rules, flying, win detection, and phase transitions. Rule
  booleans are stored as bit flags, and board occupancy uses two bits per
  point.
- `src/GeneratedBoards.*`: generated firmware board and standard rule data
  from `boards/*.json`, currently Classic Nine Men's Morris, Crossroads,
  Ninja, Hourglass, Fortress, Hexagon, Flower, Six Men's Morris, and Three
  Men's Morris.
  `Game.cpp` derives selectable global rulesets such as Long Morris and Lesker
  Morris from each board's standard rule data.
- `src/MenuMusic.*`: generated compact menu music arrays from TableTop Studio
  audio data. Current menu themes are short original bossa-style loops emitted
  as one firmware voice.
- `src/Link.*`: optional Arduboy FX-C I2C link layer compiled with
  `ALL_MENS_MORRIS_FXC_LINK`. The normal stable/debug builds use no-op stubs;
  the FX-C build exchanges menu beacons, match-start packets, compact cursor
  packets, and compact action packets while keeping `Rules` responsible for
  applying game state changes. Link sends are bus-idle gated and discovery
  beacons are jittered to avoid two consoles repeatedly becoming I2C masters at
  the same time. The FX-C build uses the maximum ArduboyI2C bus-busy check
  count to reduce lockups when another connected program is also touching I2C.
  `make setup` pins ArduboyI2C from GitHub so the FX-C build gets the newer
  bus-busy and cable-orientation support required by the USB-C link.
- Arduino builds use `-mcall-prologues` and `-fno-inline-small-functions` to
  keep the stable, debug, and FX-C binaries inside the ATmega32U4 flash budget.
- `src/Assets.*`: shared PROGMEM sprites, currently including title and boot
  logo assets.
- `boards/*.json`: editable board/rule profiles consumed by TableTop Studio and
  used as the source for generated firmware board data.
- `tools/tabletop-studio/`: local browser tool for inspecting board graphs,
  validating profiles, and editing JSON board/rule data.
- `tools/board-data/`: JSON validator/generator for compact
  `src/GeneratedBoards.*` board/rule data.
- `tools/music/`: Arduboy menu music generator, editable audio JSON sources,
  and optional checked-in public-domain MIDI source files.

Startup uses `arduboy.beginDoFirst()` and `arduboy.waitNoButtons()` instead of
`arduboy.begin()` so the stock Arduboy boot logo and LED animation are skipped.
A lightweight custom boot animation runs with inverted Pocket Pixel styling and
keeps the RGB LED off throughout startup.

## Design Constraints

Arduboy has very limited RAM, so board definitions live in `PROGMEM` and runtime
state uses fixed-size arrays. Avoid dynamic allocation in gameplay code.

The rule engine should not know about screen layout. It should operate on point
indices and board data so later variants can swap topology without rewriting the
state machine.

Board and rules data are deliberately separate. `make board-data` generates a
board definition from the graph overlay, and a standard rule set from variant
settings such as piece count, flying, capture protection, material wins,
blocked wins, reserve-gated block wins, mill behavior, and draw counters.
Runtime ruleset selectors can then scale and modify those standard rules
without duplicating board geometry.

## Near-Term Refactor Targets

- Separate scene state from match state.
- Continue moving variant-specific menu/debug behavior onto generated profile
  metadata.
- Keep the CPU opponent generic by adding rule/board helpers instead of
  variant-specific AI branches.
- Add debug-only helpers behind `ALL_MENS_MORRIS_DEBUG`.
- Add save/settings support only after gameplay rules settle.
- Use the TableTop Studio plan in `docs/tabletop-studio.md` when board variant
  data and editor tooling become the next focus.
- Keep planned Arduboy FX-C link-cable multiplayer behind an explicit module
  and build target. The link layer should exchange compact action packets and
  leave `Rules` as the single authority for applying moves/captures.
