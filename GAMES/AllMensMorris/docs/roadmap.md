# Roadmap

## Completed

- Initialized an Arduboy/Nix project scaffold.
- Added classic board topology data.
- Added a first rules state machine.
- Added a playable placement-phase UI.
- Added mandatory capture after forming a mill.
- Added movement, flying, and classic win detection.
- Added a main menu, board-direction cursor navigation, and hold-A quick menu.
- Added sound/RGB feedback and a centered board layout.
- Added debug-only scenarios for fast mill, flying, and blocked-game-over tests.
- Added basic draw detection after 50 movement turns without capture.
- Added a first visual pass for animated cursor, simplified board, HUD, and
  inverted horizontal main-menu board selector.
- Added a custom inverted boot animation that skips the stock Arduboy logo and
  LED startup animation.
- Added `BoardDefinition` and `RuleSet` scaffolding so future variants can
  change board graph data and rule knobs independently.
- Added the first TableTop Studio slice for board profile viewing, graph
  editing, validation, duplication, and JSON editing.
- Added a Six Men's Morris study profile in TableTop Studio.
- Added `make board-data` generation from board profiles into firmware
  `BoardDefinition`/`RuleSet` data.
- Made Six Men's Morris playable from the main menu.
- Added mill behavior as a rule knob so variants can capture on mills or win
  immediately on mills.
- Added a playable Three Men's Morris profile.
- Reworked Long Morris from a duplicated classic-board profile into a selectable
  ruleset that can be applied to any generated board.
- Added Lesker Morris as a selectable ruleset with mixed placement/movement from
  the start.
- Added Flower as a playable custom 20-point board profile.
- Added Hexagon as a playable 36-point board with three concentric hexagons
  and twelve standard pieces per player.
- Added Crossroads, Ninja, Hourglass, and Fortress as additional playable board
  profiles for 64x64 playtests, and retired Diamond/Clover after board
  readability review.
- Added a generic CPU opponent that works across generated board/rule profiles.
- Added visible CPU cursor/action animation with turn-paced movement sounds.
- Added CPU Easy and CPU Hard opponent settings, with Hard using depth-2
  minimax over the shared heuristic and weighted top-action choice.
- Added reproducible menu music generation from editable TableTop Studio audio
  data.
- Added a TableTop Studio piano-roll audio editor for menu music and sound
  effect drafts.
- Replaced the longer menu melody with three compact original bossa-style loops
  that rotate after attract mode demos.
- Compact board/rule data pass: flat adjacency lists, packed mill triples,
  bitwise rule flags, and two-bit board occupancy. This recovered about 790
  bytes of stable flash and 66 bytes of RAM while keeping gameplay features.
- Measured a first Arduboy FX-C link-cable budget with ArduboyI2C in a
  temporary build: about +542 bytes flash/+39 bytes RAM for minimal I2C setup,
  or about +970 bytes flash/+41 bytes RAM with handshake and cable-flip checks
  enabled before adding game protocol/UI.
- Fixed result panels so `PLAYER 1`/`PLAYER 2` refer to the local match slot
  instead of the internal black/white player enum.
- Added a first Arduboy FX-C link-cable build target and linked multiplayer
  slice, then guarded discovery so a failed I2C beacon can recover instead of
  freezing a single console without a peer.

## Next Cycle

1. Cut and publish `v1.0.0` release artifacts.
2. Keep only low-risk release fixes on this branch unless a real hardware bug
   appears.
3. Move larger ideas into a future sequel/advanced edition that can target
   Arduboy FX/FX-C storage more aggressively.
4. Make the Nix workflow more reproducible: move Arduino core/library setup out
   of the ad-hoc `make setup` path where practical, add Darwin targets, and
   expose a useful `nix run github:Jujuyeh/all-mens-morris` entry that launches
   the libretro build when public.

## Later

- More board/rule variant definitions.
- More advanced draw rules, such as repeated-position detection.
- TableTop Studio for designing board sprites and playable graph data together.
- Save/settings support.
- Advanced Arduboy FX/FX-C edition with larger board/rule libraries.
