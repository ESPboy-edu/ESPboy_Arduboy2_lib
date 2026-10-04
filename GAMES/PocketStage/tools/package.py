#!/usr/bin/env python3
"""
Bundle the program and its flash data into one .arduboy package.

A .arduboy file is a zip holding the .hex, the flash image and an info.json. It is
what the Ardens emulator and the Arduboy flashing tools load in one go.

  python3 tools/package.py build/game/PocketStage.ino.hex build/game/fxdata.bin build/game/PocketStage.arduboy

The title picture (assets/title.png) is what the console's game menu shows.
It is drawn by tools/logo.py: the title page at one moment.
"""
import json
import os
import re
import sys
import time
import zipfile

HERE = os.path.dirname(os.path.abspath(__file__))
TITLE = os.path.join(HERE, "..", "assets", "title.png")
VERSION = re.search(r'GAME_VERSION "([^"]+)"',
                    open(os.path.join(HERE, "..", "PocketStage", "version.h")).read()).group(1)


def main():
    hexf, binf, out = sys.argv[1:4]
    info = {
        "schemaVersion": 3,
        "title": "Pocket Stage",
        "description": "A Guitar Hero-like game for the Arduboy FX and FX-C.",
        "author": "Joel",
        "version": VERSION,
        "date": time.strftime("%Y-%m-%d"),
        "genre": "Music",
        "binaries": [{
            "title": "Pocket Stage",
            "filename": "PocketStage.hex",
            "device": "ArduboyFX",
            "flashdata": "fxdata.bin",
            "flashsave": "fxsave.bin",
            "cartImage": "title.png",
        }],
    }
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr("info.json", json.dumps(info, indent=2))
        z.write(hexf, "PocketStage.hex")
        z.write(binf, "fxdata.bin")
        # what the game may write to: next to the data, 4 KB of nothing
        z.write(os.path.join(os.path.dirname(binf), "fxsave.bin"), "fxsave.bin")
        z.write(TITLE, "title.png")
    print("wrote", out)


if __name__ == "__main__":
    main()
