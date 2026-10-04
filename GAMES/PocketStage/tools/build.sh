#!/bin/sh
# Build everything: the song data, then the console program, then the .arduboy package.
# Output lands in build/game, or in build/$OUT.
#
#   tools/build.sh                 the game. build/game is what the list of games on the
#                                  console is made from (../CartManager)
#   OUT=dev tools/build.sh         the same, kept apart in build/dev: for trying out
#   EXTRA_FLAGS=... tools/build.sh with extra compiler flags
#
# The compiler and the Arduino tools are the ones in ../SmallTheftAuto/.toolchain
# (.toolchain here is a link to them). Two compiler options make the program
# smaller for free: -mrelax and -mstrict-X (see Small Theft Auto's build.sh).
set -e
cd "$(dirname "$0")/.."
T="$PWD/.toolchain"
SMALLER="-mrelax -mstrict-X"
export OUT="${OUT:-game}"
mkdir -p "build/$OUT"
python3 tools/build_data.py
"$T/bin/arduino-cli" --config-file "$T/arduino-cli.yaml" compile \
  --fqbn arduino:avr:leonardo \
  --build-property "upload.maximum_size=29696" \
  --build-property "compiler.cpp.extra_flags=${EXTRA_FLAGS:-} $SMALLER" \
  --build-property "compiler.c.extra_flags=$SMALLER" \
  --build-property "compiler.S.extra_flags=${EXTRA_FLAGS:-}" \
  --build-property "compiler.c.elf.extra_flags=$SMALLER" \
  --build-path "$PWD/build/$OUT-obj" \
  --output-dir "$PWD/build/$OUT" \
  PocketStage
O="build/$OUT"
python3 tools/package.py "$O/PocketStage.ino.hex" "$O/fxdata.bin" "$O/PocketStage.arduboy"
cp tools/web/ardens.html build/ardens.html
ls -la "$O/PocketStage.ino.hex" "$O/fxdata.bin" "$O/PocketStage.arduboy"
