# All Men's Morris

All Men's Morris is an Arduboy / Arduboy FX strategy game built around Nine
Men's Morris and related board variants.

The v1.0 release implements playable Morris-family variants with placement,
movement, flying with three pieces, mill detection, mandatory capture after a
mill, win/draw detection, CPU opponents, Arduboy FX-C link-cable play, and
sound/RGB feedback. The project is structured so future modes can add different
boards, rule variants, AI, and campaign-style ideas without mixing rule logic
into rendering code.

## Status

Release-ready v1.0.0. Current controls:

- Main menu up/down: switch between board, ruleset, first-player, and opponent
  settings.
- Main menu left/right: change the selected board, ruleset, first-player
  setting, or opponent setting.
- Main menu B: start the selected playable board/ruleset pairing, cycle the
  selected setting, or show `SOON` for unavailable boards.
- Main menu A: rotate audio between Music+FX, FX Only, and Muted, with a brief
  bottom-left status overlay.
- Main menu idle: after 20 seconds without input, enter a silent CPU demo.
- Demo mode: any button returns to the main menu and restarts menu music,
  rotating through short menu themes.
- In game d-pad: move the cursor in that board direction, preferring connected
  neighboring points; press two non-opposite directions together for diagonals.
- B: place/select/confirm/capture.
- Hold A: show quick menu.
- Hold A + left: rewind through the fixed in-RAM game history.
  In linked games, this sends the same rewind step to the peer console.
- Hold A + down: return to the main menu, with confirmation.
  In linked games, confirming this returns both consoles to the main menu.
- Hold A + right: redo through the fixed in-RAM game history.
  In linked games, this sends the same redo step to the peer console.
- Debug build only, hold A + up: cycle test scenarios for mill, flying,
  blocked-game-over, and draw states.

## Development

Enter the development shell:

```sh
nix develop
```

Install Arduino dependencies locally:

```sh
make setup
```

`make setup` pins ArduboyI2C from GitHub because the FX-C link build needs the
newer bus-busy and cable-orientation support that is not present in the older
registry header.

Compile:

```sh
make compile
```

Compile debug:

```sh
make compile-debug
```

Compile the Arduboy FX-C link-cable build:

```sh
make compile-fxc
```

Regenerate firmware board/rule data from `boards/*.json`:

```sh
make board-data
```

Regenerate menu music from the editable TableTop Studio audio source:

```sh
make music-data
```

Run with libretro:

```sh
make libretro
```

Prepare an Arduboy FX catalog entry:

```sh
make fx-entry
```

The entry is prepared under the `TableTop` category by default.

Prepare an Arduboy FX-C catalog entry with link-cable support:

```sh
make fx-entry-fxc
```

Build a distributable `.arduboy` package:

```sh
make package-arduboy ARDUBOY_VERSION=v1.0.0
```

Open TableTop Studio:

```sh
make tabletop-studio
```

The Studio loads board profiles from `boards/`, draws and edits the playable
graph overlay, validates connections/mills/rule settings, edits global sprites,
and includes a piano-roll audio editor. `make board-data` converts board
profiles into firmware C++ data; `make music-data` converts the editable menu
music source into `src/MenuMusic.*`.

## Release Notes

`v1.0.0` is the first complete playable release. Known limitations: FX-C link
play expects both consoles to run the same build, advanced desync recovery is
out of scope, no persistent save/settings storage is implemented, and the FX-C
build is close to the ATmega32U4 flash limit.

## Layout

```text
.
|-- all-mens-morris.ino
|-- boards/
|-- src/
|   |-- Assets.*
|   |-- Board.*
|   |-- Rules.*
|   |-- Game.cpp
|   `-- AllMensMorrisGame.h
|-- docs/
|-- tools/tabletop-studio/
|-- tools/board-data/
|-- skills/
|-- nix/
|-- .github/workflows/
|-- flake.nix
`-- Makefile
```

## License

Code and project tooling are released under GPL-3.0-or-later. Art, docs, and
game data are released under CC-BY-SA-4.0. See `LICENSES/README.md`.
