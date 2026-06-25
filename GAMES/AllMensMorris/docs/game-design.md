# Game Design

## Core Premise

All Men's Morris starts from Nine Men's Morris and expands into a collection of
Morris-family variants: classic rules, alternate boards, faster modes, and
custom rule experiments.

## Classic Mode Target

Classic Nine Men's Morris has three phases:

1. Placement: players alternate placing nine pieces.
2. Movement: players move pieces to adjacent points.
3. Flying: when a player has only three pieces, they may move to any empty
   point.

Whenever a player forms a mill, they remove one opponent piece that is not
inside a mill unless no such piece exists.

The current draw rule is a practical early implementation: after movement has
started, 50 completed turns without a capture ends the game as a draw. Immediate
win conditions, such as blocking the opponent, take priority over this draw.

## Current Playable Slice

Implemented:

- Classic 24-point board.
- Local two-player placement phase.
- Turn switching.
- Mill detection and feedback.
- Capture/removal after a mill, including the classic restriction that pieces
  inside mills are protected unless every opponent piece is inside a mill.
- Movement after all pieces are placed.
- Flying to any empty point when the active player has exactly three pieces.
- Win/loss detection when an opponent is reduced below three pieces after all
  reserves are placed, or when the active player has no legal move.
- Draw detection after 50 movement turns without a capture.
- Main menu scaffold for choosing board/ruleset pairings, with Classic Nine
  Men's Morris, Crossroads, Ninja, Hourglass, Fortress, Hexagon, Six Men's
  Morris, Three Men's Morris, and Flower active as boards.
- Main menu ruleset selector for Standard, Long Morris, and Lesker Morris.
  Long Morris scales to `standard pieces * 2 + 3`, fills until one empty point
  remains, allows mixed placement/movement, skips fully blocked reserve turns,
  and uses a 100-turn no-capture draw limit. Lesker Morris scales to
  `standard pieces + 1` and uses standard rules except that placement and
  movement are both available from the start.
- Main menu opponent setting for local two-player, CPU Easy, or CPU Hard play.
  In CPU mode, the human takes the selected first-player color and the CPU
  plays the opposite color.
- Horizontal board selector in the main menu, with left/right cycling board
  titles and up/down moving between board, ruleset, first-player, and opponent
  settings.
- Data-backed `BoardDefinition` and `RuleSet` generation for board and rule
  variants.
- Initial TableTop Studio for studying and editing board profiles before
  regenerating firmware data.
- Playable Six Men's Morris profile with a two-square board, six pieces per
  player, and side-line mill triples.
- Playable Three Men's Morris profile with a 3x3 board, three pieces per
  player, diagonal mills, and immediate win on mill formation instead of
  capture.
- Playable Flower profile with a custom 20-point graph, eight mill lines, and
  classic capture/flying rules.
- Playable Hexagon profile with three regular concentric hexagons, 36 valid
  points, 24 mill lines, and twelve standard pieces per player.
- Playable Crossroads, Ninja, Hourglass, and Fortress board profiles for
  alternate graph shapes that still fit the 64x64 playfield.
- Main menu setting for whether white or black moves first.
- Directional cursor navigation that follows the board geometry, preferring
  connected neighboring points before falling back to a wider geometric search.
  D-pad chords support eight-direction cursor movement on freer board layouts.
- Hold-A quick menu for debug scenarios in debug builds, fixed-depth rewind,
  redo, and returning to the main menu.
- Mixed placement/movement variants choose `PUT` automatically when the cursor
  is on a legal empty placement point; otherwise B uses selection/movement.
- Centered board layout with compact side HUD panels. The side HUD shows P1/P2,
  the active action label, and stacked reserve-piece sprites.
- Animated dashed cursor selector.
- First visual pass for the simplified board, side HUD, and inverted main menu
  with lightweight pattern decoration.
- Custom inverted boot animation, with the stock Arduboy boot logo and LED
  animation skipped.
- Sound effects for menu/action feedback and RGB LED flashing on mills.
- Menu music uses three short original bossa-style loops generated from
  TableTop Studio audio data. Startup picks one theme at random; subsequent
  returns from attract mode rotate through the next theme. Playback starts
  after a one second pause, and gameplay remains effects-only. Pressing A on
  the main menu rotates audio through Music+FX, FX Only, and Muted, with a
  one-second bottom-left status overlay. Menu tones intentionally leave short
  gaps between notes so the loop feels softer without requiring hardware volume
  control.
- Attract/demo mode after 20 seconds of main-menu inactivity. The game fades
  out, opens with alternating horizontal curtain bars, then runs a silent CPU
  Easy vs CPU Hard demo on a random playable board from a legally generated
  mid-game state, excluding Hexagon because its 36-point topology currently
  stalls too hard when generating a demo state. Any button returns to the main
  menu through the curtain and restarts menu music; otherwise the demo returns
  after 30 seconds or three seconds after a demo win/draw. Demo CPU turns wait
  a random 0.2-0.8 seconds, and demo results are labeled as Player 1/Player 2
  rather than CPU.
- Blinking result panel for completed games, with local player, CPU, and tie
  labels plus short win/lose/draw fanfares.
- Debug-only hold-A/up scenarios for mill, flying, blocked-game-over, and draw
  testing.
- Generic CPU opponent for all generated board/rule profiles. Easy uses one-ply
  action simulation and a compact tactical heuristic; Hard uses the same
  heuristic through a depth-2 minimax pass.
- CPU action choice is weighted among the highest-scored legal options so CPU
  games are less deterministic.
- CPU turns animate the cursor over valid board connections, with short cursor
  step tones and lower select/confirm tones before the simulated action is
  applied.
- Arduboy FX-C build support behind `BUILD=fxc`, with a first linked
  multiplayer slice. Two consoles in the main menu exchange compact I2C
  beacons; when a peer is detected the opponent selector unlocks and selects
  `VS LINK`. Discovery beacons are rate-limited, jittered, use maximum
  ArduboyI2C bus-busy checking, and are only sent after the I2C lines have
  sampled idle to reduce multi-master bus collisions. The console that starts a
  linked match controls the first-player side for that match. Local cursor
  steps are mirrored as compact cursor packets with the same short step tone
  used by CPU turns, while local actions are sent as
  compact mode/from/to packets and applied immediately on the receiving
  console. Rewind and redo use a reserved compact action packet so both
  consoles step through the shared history together. Returning to the main menu
  sends a compact menu packet so both consoles leave the linked match together.
  Only the active local player can act, fresh remote-turn input flashes a
  one-frame rejection effect, local turns light the RGB LED green, and mills
  keep the red/blue police LED flash on both consoles.

Not implemented yet:

- More alternate boards.

## Variant Ideas

- Classic Nine Men's Morris.
- Three Men's Morris and Six Men's Morris.
- Larger or asymmetric boards.
- Puzzle challenges with fixed starting states.
- Timed tactical mode.
- Arduboy FX catalog of board/rule presets.
