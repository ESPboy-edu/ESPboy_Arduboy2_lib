# Building Pocket Stage

It was built on a Mac. It takes Python 3 with Pillow, and `arduino-cli` with
the Arduino AVR core (1.8.8 was used) and the Arduboy2 library (6.0.0) in a
folder `.toolchain` at the top of the repository:

```bash
mkdir -p .toolchain/bin
curl -L https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_macOS_ARM64.tar.gz | tar -xz -C .toolchain/bin
printf 'directories:\n  data: %s/.toolchain/data\n  downloads: %s/.toolchain/downloads\n  user: %s/.toolchain/user\n' "$PWD" "$PWD" "$PWD" > .toolchain/arduino-cli.yaml
.toolchain/bin/arduino-cli --config-file .toolchain/arduino-cli.yaml core update-index
.toolchain/bin/arduino-cli --config-file .toolchain/arduino-cli.yaml core install arduino:avr@1.8.8
.toolchain/bin/arduino-cli --config-file .toolchain/arduino-cli.yaml lib install Arduboy2@6.0.0
```

The compiler is an Intel program: a Mac with Apple's own processor needs
Rosetta 2. The title of the booed-off page is drawn in Impact, from the copy
macOS has (`/System/Library/Fonts/Supplemental/Impact.ttf`); on another
system, point `FONT` in `tools/logo.py` at one.

```bash
tools/build.sh
```

turns the songs and the pictures into the data for the flash chip, compiles
the program and puts both into `build/game/PocketStage.arduboy`.

## The songs

A song is a Python file in `songs/`, written as text, a note a token;
`songs/setlist.py` says which fifteen are in the game and in what order. The
notation is at the top of `tools/build_data.py`, and
`python3 tools/song_check.py <name>` looks a song over before it goes in.

## The data for the flash chip

The build makes it: `build/game/fxdata.bin`. A copy as it was built last is
[download/fxdata.bin](../download/fxdata.bin); the same file is inside
`PocketStage.arduboy`, which is a zip. What is where in it:
`PocketStage/fxdata.h`, and the format at the top of `tools/build_data.py`.

## The FX-C

The same build runs on the FX-C and the Mini, whose flash chip hangs on PE2
instead of PD1: the game looks on both when it starts.
