Headless Ardens driver: runs the real .hex + FX data in the Ardens emulator core,
presses buttons from a script, and saves screenshots (.pbm). Used to catch problems
the PC harness can't see (flash pin, timing, stack, real AVR code).

Build (needs a clone of github.com/tiberiusbrown/Ardens with submodules in ARDENS):
  printf '#pragma once\n#define MINIZ_EXPORT\n' > miniz_export.h
  g++ -std=c++20 -O2 -w -DNDEBUG -DARDENS_VERSION_MAJOR=0 -DARDENS_VERSION_MINOR=0 -DARDENS_VERSION_PATCH=0 \
      -I$ARDENS/src -I$ARDENS/deps -I$ARDENS/deps/bitsery/include -I$ARDENS/deps/miniz -I. \
      drive.cpp $ARDENS/src/absim_*.cpp -c
  gcc -O2 -w -I. -I$ARDENS/deps/miniz -c $ARDENS/deps/miniz/*.c $ARDENS/deps/yyjson/yyjson.c
  for f in $ARDENS/src/boot/*.c; do gcc -O1 -c $f -o boot_$(basename $f .c).o; done
  g++ *.o -o drive -lpthread

Run:  ./drive <hex> <fxdata.bin> <d1|e2> "<script>"
  d1 = original Arduboy FX flash pin (Ardens default), e2 = FX-C / Mini
  script: wMS wait · A B U D L R tap a button · sNAME save NAME.pbm
          t  speaker (PC6) toggles since the last t, and the pitch they make
          P  the lowest stack pointer seen (and where), for the RAM budget
          X  power-cycle with EEPROM wiped (as if another game wrote it): the FX save copy should
             put the save back -- E dumps the save's first bytes (EEPROM 80..85) to check
  ./shots.sh <hex> <bin> <port> <prefix>   boots, starts a game, walks into the map
