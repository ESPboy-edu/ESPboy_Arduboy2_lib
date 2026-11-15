#!/bin/sh
# Rebuild every generated asset. Run from the sketch folder.
#   APU=/path/to/Arduboy-Python-Utilities sh tools/build_data.sh
set -e
python3 tools/chapters/make_chapters.py
python3 tools/audio/make_sfx.py
python3 tools/woodcut/make_gfx.py
cd fxdata && python3 "${APU:-../../Arduboy-Python-Utilities}/fxdata-build.py" fxdata.txt
