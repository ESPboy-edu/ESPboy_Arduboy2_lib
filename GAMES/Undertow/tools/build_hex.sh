#!/bin/bash
# Build the release .hex files the way the Arduino IDE does, without the IDE.
#   HM=/path/to/Arduboy-homemade-package sh tools/build_hex.sh        (from the sketch folder)
# Board package: Mr.Blinky's Arduboy Homemade package (board "Arduboy FX" / "Arduboy FX-C",
# Arduboy optimized core, its bundled Arduboy2 / ArduboyFX libraries) and the link flags the IDE
# needs in platform.local.txt: -mcall-prologues -mrelax -fno-move-loop-invariants.
# Needs avr-gcc on the PATH. The v1.0 release was built with avr-gcc 7.3.0 (Atmel 3.7.0, the
# Debian/Ubuntu gcc-avr package); the IDE's own 7.3.0-atmel3.6.1-arduino7 gives an equivalent hex,
# about 100 bytes smaller, so its checksums differ.
# Writes release/Undertow-FX.hex and release/Undertow-FXC.hex, and prints flash and RAM use.
set -e
SK=$(pwd); HM=${HM:?set HM to a clone of github.com/MrBlinky/Arduboy-homemade-package}/board-package-source
OUT=${OUT:-/tmp/undertow-build}
for B in fx fxc; do
  O=$OUT/$B; mkdir -p $O; rm -f $O/*
  if [ $B = fx ]; then VAR=$HM/variants/arduboy-fx; CS=-DCART_CS_SDA; NAME=Undertow-FX; else VAR=$HM/variants/arduboy-fxc; CS=-DCART_CS_HWB; NAME=Undertow-FXC; fi
  CORE=$HM/cores/arduboy; LIB=$HM/libraries
  DEFS="-mmcu=atmega32u4 -DF_CPU=16000000L -DARDUINO=10819 -DARDUINO_AVR_ARDUBOY -DARDUINO_ARCH_AVR -DARDUBOY_10 $CS -DUSB_VID=0x2341 -DUSB_PID=0x8036"
  INC="-I$CORE -I$VAR -I$LIB/EEPROM/src -I$LIB/Arduboy2/src -I$LIB/ArduboyFX/src -I$SK"
  CXX="avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -flto"
  CC="avr-gcc -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects"
  objs=()
  for f in $CORE/*.c; do [ -f "$f" ] || continue; $CC $DEFS '-DUSB_MANUFACTURER="Arduboy Inc"' '-DUSB_PRODUCT="Arduboy"' $INC $f -o $O/c_$(basename $f).o; objs+=($O/c_$(basename $f).o); done
  for f in $CORE/*.cpp $LIB/Arduboy2/src/*.cpp $LIB/ArduboyFX/src/*.cpp; do
    $CXX $DEFS '-DUSB_MANUFACTURER="Arduboy Inc"' '-DUSB_PRODUCT="Arduboy"' $INC $f -o $O/$(basename $f).o; objs+=($O/$(basename $f).o); done
  for f in $CORE/*.S; do [ -f "$f" ] || continue; avr-gcc -c -g -x assembler-with-cpp -flto $DEFS $INC $f -o $O/$(basename $f).o; objs+=($O/$(basename $f).o); done
  avr-gcc-ar rcs $O/core.a "${objs[@]}"
  { echo '#include <Arduino.h>'; echo "#line 1 \"$SK/Undertow.ino\""; cat $SK/Undertow.ino; } > $O/sketch.cpp
  $CXX $DEFS $INC $O/sketch.cpp -o $O/sketch.o
  avr-gcc -w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -mcall-prologues -mrelax -fno-move-loop-invariants \
          -mmcu=atmega32u4 -o $O/Undertow.elf $O/sketch.o $O/core.a -lm
  avr-objcopy -O ihex -R .eeprom $O/Undertow.elf $SK/release/$NAME.hex
  echo "$NAME.hex: $(avr-size -C --mcu=atmega32u4 $O/Undertow.elf | grep -E 'Program|Data' | tr -s ' ' | tr '\n' ' ')"
done
