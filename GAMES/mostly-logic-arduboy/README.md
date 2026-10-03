# Mostly Logic

Mostly Logic is a seven-level puzzle game for Arduboy. Place deflectors to route projectiles through switches, gates, tripwires, and splitters. X turns a projectile counterclockwise; the dot turns it clockwise.

![Mostly Logic launcher artwork](packaging/mostly-logic-launcher.png)

## Playing

GitHub Releases provides the packaged game and its raw firmware:

- `MostlyLogic.arduboy` is the complete game package, including metadata and launcher artwork.
- `MostlyLogic.hex` is the raw firmware for uploaders and emulators that accept Intel HEX files.

## Controls

- Title screen: A starts the campaign; B toggles sound and saves the setting.
- Level intro: A or B dismisses the intro card and starts the simulation.
- Puzzle: The D-pad moves the cursor. A or B cycles an editable cell through empty, X deflector, dot deflector, and empty.
- Puzzle: Press Left and Right together to restart the current level.
- Level result: A advances after a win; B retries. After a failure, A or B retries when the failure animation finishes.
- Final screen: A starts the campaign again; B returns to the title screen.
- Hidden level selector: Hold A and B, then press Up three times. Use the D-pad to choose an entry, A to select it, or B to return to the title screen.

## Building with PlatformIO

[PlatformIO](https://platformio.org/) is the supported firmware build system. From the repository root, install PlatformIO Core and run:

```bash
pio run
```

The `arduboy` environment pins Atmel AVR platform 5.3.0, Arduboy2 6.0.0, and ArduboyTones 1.0.3. It writes the firmware to `.pio/build/arduboy/firmware.hex`.

## Release binaries

Release metadata lives in `packaging/arduboy-package.json`. Its `version` controls the package version and must match the Git tag used to publish the release.

For each release:

1. Update `version` and `date` in `packaging/arduboy-package.json`, and review the remaining package information for anything that changed.
2. Generate the checked-in release files:

    ```bash
    python3 scripts/build-release.py
    ```

3. Run the tests and confirm the generated files are current:

    ```bash
    sh tests/run-host-tests.sh
    python3 scripts/build-release.py --check
    ```

4. Test the build on hardware or in an emulator, then commit and push the source, metadata, and both files under `dist/`.
5. Create and push a Git tag equal to `v` followed by the metadata version. For version `1.0.0`:

    ```bash
    git tag v1.0.0
    git push origin v1.0.0
    ```

Pushing the tag runs `.github/workflows/release.yml`. The workflow rejects a tag that does not match the metadata version, runs the tests, verifies the checked-in artifacts against a fresh build, creates the GitHub Release, and attaches `MostlyLogic.hex` and `MostlyLogic.arduboy`.

The `.arduboy` file contains schema version 4 metadata, the full license text, the firmware, and the launcher artwork. The game does not use FX data or FX save files, so those optional entries are not included.

## Tests

Host-side tests cover direction handling, tripwire collision, projectile scheduling, packed coordinates, and campaign-data validation:

```bash
sh tests/run-host-tests.sh
```

GitHub Actions runs these tests, checks C++ formatting, builds the PlatformIO firmware, and verifies the checked-in release artifacts. Automated checks do not replace hardware or emulator playtesting.

## Implementation overview

The Arduino sketch delegates to ordinary C++ modules. `Game::loop()` uses Arduboy2's frame limiter, polls buttons once per frame, advances gameplay, and asks the renderer to display the resulting state. Projectile movement uses a fixed 22 ms simulation step with bounded catch-up after a slow frame. New projectiles created by a splitter do not move until the next simulation step.

The game avoids dynamic allocation. Board state, projectiles, animation paths, and particles all use fixed-capacity storage; immutable levels and bitmap artwork live in AVR program memory.

## Code organization

- `MostlyLogic/Game.cpp` owns input, mutable state, simulation, circuit behavior, and game flow.
- `MostlyLogic/Renderer.cpp` draws the state exposed through the internal `GameState.h` bridge without advancing gameplay.
- `MostlyLogic/GameTypes.h` contains shared constants, types, and portable helpers.
- `MostlyLogic/Artwork.cpp` contains fixed bitmap artwork; `Artwork.h` names the assets and animation frames.
- `MostlyLogic/Levels.cpp` contains the seven campaign definitions in program memory.
- `MostlyLogic/Levels.h` contains the authoritative level-authoring instructions and invariants.
- Host tests under `tests/` check portable helpers and level-data invariants. Rendering, timing, audio, and puzzle solutions require playtesting.

## Project structure

```text
mostly-logic-arduboy/
├── MostlyLogic/       Arduino sketch and game implementation
├── dist/              Checked-in release binaries
├── packaging/         Arduboy package metadata and launcher artwork
├── scripts/           Build and release automation
├── tests/             Host-side tests and level validation
├── platformio.ini     Pinned Arduboy build configuration
└── README.md
```

## Launcher artwork

`packaging/mostly-logic-launcher.png` is a 128 by 64 pixel, one-bit grayscale PNG. The release builder validates those properties and embeds the image in `dist/MostlyLogic.arduboy` as its cart image.

## Development note

AI tools were used extensively during development, particularly to build and refine the procedural rendering and animations. Fixed artwork uses bitmap sprites where appropriate. Moving, pulsing, expanding, and geometry-driven visuals are drawn with primitives because they are easier to tune directly than a collection of nearly identical sprite frames.

## License

Copyright © 2026 MostlyBuilds. Source code, documentation, and original artwork in this repository are licensed under the [GNU General Public License v3.0 only](LICENSE).
