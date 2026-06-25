# AGENTS.md - All Men's Morris

## Role

You are working on All Men's Morris, an Arduboy / Arduboy FX strategy game
inspired by Nine Men's Morris and its board/rules variants. Keep the codebase
small, deterministic, and easy to extend into new Morris-family modes.

## Project Context

- Target device: Arduboy / Arduboy FX.
- Local build system: Nix flake plus Arduino CLI.
- Local emulator: RetroArch with the Ardens libretro core packaged by the flake.
- Current sketch entrypoint: `all-mens-morris.ino`.
- Core game loop: `src/Game.cpp`.
- Board topology data: `src/Board.*`.
- Rules state machine: `src/Rules.*`.
- Assets module: `src/Assets.*`.

## Ground Rules

- Prefer small, reversible commits using Conventional Commits.
- Preserve working gameplay during refactors unless a behavior change is explicit.
- Do not directly upload to an Arduboy FX as if it were a classic Arduboy.
- Keep generated build outputs, backups, and local Arduino downloads out of git.
- Store reusable project automation in this repository, not under the user's home.
- Prefer ASCII in source and docs unless a file already has a reason to use
  non-ASCII text.

## Development Commands

Enter the development shell:

```sh
nix develop
```

Install Arduino core and libraries into local project directories:

```sh
make setup
```

Compile the game:

```sh
make compile
```

Compile the debug variant:

```sh
make compile-debug
```

Run locally with libretro:

```sh
make libretro
```

Prepare an Arduboy FX catalog entry:

```sh
make fx-entry
```

Run standard checks:

```sh
nix flake check
nix develop -c make compile
nix develop -c make compile-debug
```

## Architecture Direction

- `Game`: Arduboy loop, input, rendering, and scene dispatch.
- `Board`: board coordinates, mills, and adjacency data for each topology.
- `Rules`: legal actions, turn state, phase transitions, and win/loss rules.
- `Assets`: sprites, banners, and audio data.
- Future modules should separate AI, variants, save data, and debug tooling.

Keep rule logic independent from screen coordinates wherever possible. Board
variants should be data-first so classic, smaller boards, and custom modes can
share the same engine.

## Documentation

- Update `README.md` when user-facing commands change.
- Update `docs/roadmap.md` when the plan changes materially.
- Update `docs/game-design.md` when rules, modes, or controls change.
- Update `docs/codebase.md` when architecture changes.
- Update `docs/fx-workflow.md` when FX packaging or upload behavior changes.
