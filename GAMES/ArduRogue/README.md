# ArduRogue

ArduRogue is a traditional roguelike for the [Arduboy](https://www.arduboy.com/).

![](img/recording.gif)

[Try out the web version here.](https://tiberiusbrown.github.io/ardurogue.html)

[Read the guide here.](guide.md)

## Features

- Random dungeon generation
- Save/Load games: save (if it exists) destroyed on load (permadeath is enforced)
- High score tracking
- 16 types of enemies, each with their own quirks and abilities
- Variety of items:
  - 10 potion types
  - 9 scroll types
  - 8 ring types (up to 2 rings can be worn)
  - 8 amulet types (not including the amulet of Yendor)
  - 7 wand types
- Items can be cursed, identified, enchanted
- Throw a potion at an enemy to apply its effect to it
- Hidden doors
- Afflictions: confusion, invisibility, weaken, slow, etc.

## Wishlist (in order of priority)

If enough prog bytes could be freed up...

- Floor traps
- Different map types (cave style?)
- Wands
  - Lightning (arcs between monsters)

## Build Notes

Contents of `platform.local.txt`:
```
compiler.c.extra_flags=-mcall-prologues -mstrict-X -fno-inline-small-functions -fno-move-loop-invariants -fno-tree-scev-cprop -fweb -frename-registers -std=c++11 -Wall -Wextra -Wshadow -Wconversion -Wundef -Wunused-parameter -g3
compiler.cpp.extra_flags={compiler.c.extra_flags} 
compiler.c.elf.extra_flags=-Wl,--relax -g3
```
