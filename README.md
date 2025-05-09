# ESPboy (ESP8266 gadget) port of the Arduboy2 library

This port of following Arduboy dev libraries (check "libs" folder):
1. Arduboy2
2. Arduboy Tones
3. Arduboy FX
4. Arduboy Tones FX
5. Arduboy Grayscale lib (Arduboy G)
6. Arduboy SynthU
7. FixedPointsArduino

and separate Arduboy libs
1. Arduboy playtune -> [ESPboy_Playtune](https://github.com/ESPboy-edu/ESPboy_Playtune)
2. Arduboy ATMlib -> [ATMlib](https://github.com/ESPboy-edu/ESPboy_ATMlib)

from [Arduboy project ](https://arduboy.com/ "Arduboy project ") intended to run on[ ESPboy project](https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun " ESPboy project").

It supports ESPboy buttons, LED, sound and TFT display with y-axis scaling.

This port compiles for ESP8266 ESPboy, existing Arduboy2-compatible games and apps. 
Some of them can use it as a drop-in replacement for the original **"Arduboy2" library**, other games will run after the slight source code modifications.

# Setting up Arduino IDE
1. Clone/download this repo somewhere on your computer.
2. Now, we are going to replace the existing Arduboy2 libraries on your computer (if you have them installed).
3. Navigate to Arduino\libraries folder on your computer and delete "Arudboy2" and "ArduboyTones" folders; take backup of both of them, if required.
4. Copy "ESPboy_Arduboy2_lib" folder into Arduino\libraries folder and rename it to "Arudboy2".
    - If you had downloaded the code as zip file instead of git clone, then you will have ESPboy_Arduboy2_lib_master folder instead of ESPboy_Arduboy2_lib.
5. From within this new Arudboy2 folder, goto libs folder and remove "_master" from both directories.
    - ArduboyTones-master --> ArduboyTones
    - FixedPointsArduino-master --> FixedPointsArduino
    - ArduboyFX-main --> ArduboyFX
    - ArduboyTonesFX-master --> ArduboyTonesFX
6. Copy the newly renamed libs into the main Arduino\libraries folder.
7. Clone/download the [ESPboy_Playtune](https://github.com/ESPboy-edu/ESPboy_Playtune) and [ATMlib](https://github.com/ESPboy-edu/ESPboy_ATMlib)repo somewhere on your computer.
8. Copy "ESPboy_Playtune" and "ATMlib" folders into Arduino\libraries folder.
    - If you had downloaded the code as zip file instead of git clone, then you will have ESPboy_Playtune_master folder instead of ESPboy_Playtune.
9. If game uses ArduboyG.h or SynthU.hpp, rewrite them with the same files from this repo "libs..." folder
11. Goto Arduino\libraries\Arudboy2\GAMES folder and try to compile any main .ino file to check if everything is working fine; example Karateka\Karateka.ino.
    - Note: the name of the game folder directory should be exactly same as the main .ino file; please rename the game folder directory as required, example "PPOT RoadTrip" --> "RoadTrip"
12. FX-lib using is more complicated and i have to write it later :) ask me to force me to do this :)

# Migrating the game from Arduboy to ESPboy
1. replace the **"Arduboy2" library** and **"Arduboy tones" library** in your Arduino Studio libraries folder with these versions.
2. do ingame modifications according to the following notes:
- change **"#include arduboy.h"** to **"#include arduboy2.h"**
- some games use a function pointer array to pass control to a different parts of the code as the game state changes. In ATMEGA32U4 the memory address is 2 bytes (single word) long, in ESP8266, the memory addresses is 4 bytes (double word) long, So you need to change all "pgm_read_word" to "pgm_read_dword" or "pgm_read_ptr" at the pointers
- if EEPROM is used by the game to keep configs/high scores:
- - change EEPROM.update() to EEPROM.write()
- - add EEPROM.commit() after the last EEPROM.put(), EEPORM.write() of each blocks of code.
- For **"ArduboyPlaytune"** use [ESPboy ported verstion](https://github.com/ESPboy-edu/ESPboy_Playtune) and be sure you don't use "tone();" function anywhere (standard "tone();" function catches an interrupt which is ArduboyPlaytune also uses causing a reset)
- For **"ATMlib"** use [ESPboy ported version](https://github.com/ESPboy-edu/ESPboy_ATMlib) and be sure to rename the folder from "ESPboy_ATMlib" to "ATMlib"
- you have to put ESP.wdtFeed(); in all loops like while(1) {...}. EPS8266 needs time to process WiFi stack and other internal SDK interrupts and can do it during the pauses like delay(0). Otherwise it watchdog resets.
- games that directly control the SPI or I2C bus to write to OLED display need much more work to port instead of the simple steps above.
- font() array is used in TFT_eSPI display library so you have to change all "font" to "font_"
- there is a problem with a "char" data (signed/unsigned problem). By default Arduino AVR "char" is signed and ESP "char" is unsigned. So you have to change all (where the variable is used not as char, but as int8_t) "char" to "signed char".
- it's also better to change all "short" to "int16_t", "unsigned shot" to "uint16_t", "byte" to "uint8_t", "int" to "int16_t", "unsigned int" to "uint16_t", "long" to "int32_t", "unsigned long" to "uint32_t" 
- in AVR compiler "bool function();" returns FALSE in case of function out without "return(value);", but in ESP8266 compiler it returns TRUE
- abs(); function does not work properly with type "float" and "double". It rounds number to integer, so you have to implement your own "abs_();" function as "#define abs(x) ((x)<0 ? -(x) : (x))"
- "%" operator. "a%b" with "b==0" will lead to different results on AVR and ESP architecture. AVR returns "a" and ESP will reset with Error code "Illegal instruction" because a%b uses devision and devision by 0 forbidden.
- AVR allows ‘return’ in ‘void’ functions and absence of ‘return’ in non-void functions. ESP compiler does not allow this.
- AVR compiler Arduino IDE allows division by 0. i.e. the expression 
```cpp
uint8_t a, b, c;
a = 4; 
b = 0;
c = a / b;
```
will be compiled and will work without errors.
ESP compiler Arduino IDE will also compile such code, but ESP will reboot with the error ‘Devision by zero exception’ so it's a good way to avoid these situations


# Advantages of ESPboy (ESP8266)
- 8 keys
- TFT color LCD 128x128
- Faster MCU 80Mh/160Mhz
- RGB neopixel LED
- More memory for program and data.  http://0x04.net/~mwk/doc/xtensa.pdf
- Internal flash memory mounted as SPIFFS disk 4-16mb for additional data
- WiFi onboard opens up the possibility for online games, top scores posting online
- OTA (on the air firmware update) update the firmware through WiFi from a web site, the same way as Android playmarket/ Apple istore.
- Cheaper overall cost


# Credits and Documentation

**99% of the work on the library was done by the contributors to the following repositories:**

**Arduboy2 library:**
- https://github.com/Arduboy/Arduboy 
- https://github.com/MLXXXp/Arduboy2
- https://github.com/harbaum/Arduboy2 
- https://github.com/hartmann1301/Arduboy2 hartmann1301 migrated the library to the ESP8266 platform
- https://github.com/cheungbx/esp8266_arduboy2
- https://github.com/edgarborja/Arduboy2ESP

**Arduboy tones library:**
- https://github.com/MLXXXp/ArduboyTones
- https://github.com/hartmann1301/ArduboyTones

**And all the [ARDUBOY COMMUNITY!](https://community.arduboy.com/ "ARDUBOY COMMUNITY")**

*Please do read through the extensive documentation included there.*

# There are a few games in this package
**Check the LICENCE files in every folder!**

If authors against posting the code in this repository for the purpose of learning programming and fun, please let me know, I'll remove it.

- **Suit Shooter** (Unknown) by [crait](https://community.arduboy.com/t/suit-shooter-a-falling-block-shooter)
- **Train Dodge** (Unknown) by [crait](https://community.arduboy.com/t/train-dodge-single-button-reflex-game)
- **Roo Run** (Unknown) by [crait](https://community.arduboy.com/t/roo-run-a-kangaroo-escape-story)
- **KPA qiest** (Unknown) by [crait](https://community.arduboy.com/t/kpa-quest-a-new-mini-puzzle-game)
- **Insane Skulls** (Unknown) by [crait](https://community.arduboy.com/t/insane-skulls-game-jam-5)
- **ChickenLauncher** (Unknown) by [crait]
- **Rogue boy** (Unknown) by [Dreamer2345](https://community.arduboy.com/t/earlyrelease-rogueboy)
- **UAPs-Must-DIE** (GPL-3.0)  by [Onebit Matt](https://community.arduboy.com/t/isometric-shooter-uaps-must-die)
- **Factor 9** (GPL-3.0)  by [Onebit Matt](https://community.arduboy.com/t/factor-9)
- **Big wave Dave G** (GPL-3.0)  by [Onebit Matt](https://community.arduboy.com/t/big-wave-dave)
- **Rhombus FX G** (GPL-3.0)  by [Onebit Matt](https://community.arduboy.com/t/rhombus-grayscale-fx)
- **Paintball FX G** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/paint-ball-v1-0)
- **Oh Hell! FX G** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/a-summer-as-hot-as-hell)
- **Magnets FX G** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/magnets-fx)
- **Shroom Knight** (GPL-3.0)  by [Onebit Matt](https://community.arduboy.com/t/shroom-knight)
- **Bone Shakers** (GPLv3) by [James Howard](https://community.arduboy.com/t/bone-shakers-unofficial-game-jam-4)
- **Defender FX G** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/defender-fx-v1-0)
- **Sensitive G** (???) [spinal](https://community.arduboy.com/t/game-sensitive)
- **KUBE G** (GPL-3.0) [Onebit Matt](https://community.arduboy.com/t/kube-isometric-grayscale-game)
- **Greenhorn Syndicate FX** (???) [Atom_Y](https://community.arduboy.com/t/fx-greenhorn-syndicate)
- **Wolfenduino 3D FX** [James Howard](https://community.arduboy.com/t/wolfenduino-3d-fx/11597)
- **Octopus** (???) [Roby](https://community.arduboy.com/t/octopus-game-watch)
- **Arduban** (MIT) [Rob Prouse](https://community.arduboy.com/t/arduban-a-port-of-the-popular-sokoban-puzzle-game)
- **Prince of Arabia FX** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/prince-of-arabia-fx/10926)
- **Piracy - Avast Ye Landlubbers!** (MIT license) by [Prototype (Bert Veer)](https://community.arduboy.com/t/piracy-avast-ye-landlubbers/10692)
- **Choplifter** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/choplifter-v1-1-1/4867)
- **1943: The Battle of Midway** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/1943-the-battle-of-midway/4070)
- **ArduGolf - 18-Hole Mini Golf** (MPL-2.0 license) by [brow1067](https://community.arduboy.com/t/ardugolf-18-hole-mini-golf/10462)
- **OBS - OneButtonShoter** (BSD-3-Clause) by [PPOT](https://community.arduboy.com/t/obs-one-button-shooter/10445)
- **ArduRogue** (MPL-2.0) by [tiberiusbrown](https://github.com/tiberiusbrown/ardurogue)
- **Ardulem** (Open source)[Alban Nanty](https://github.com/Lswbanban/Ardulem)
- **RickArdurous** (Open source) [Alban Nanty](https://github.com/Lswbanban/RickArdurous)
- **1944 Return to Midway** (MIT) [PPOT](https://github.com/Press-Play-On-Tape/Nineteen44)
- **Evasion** (MIT) by [OBONO](https://community.arduboy.com/t/evasion-a-simple-action-game/9226)
- **MiniRogue** (BSD-3) by [PPOT](https://github.com/Press-Play-On-Tape/MiniRogue)
- **Arduchess** (MPL-2.0) by [tiberiusbrown](https://community.arduboy.com/t/arduchess-play-chess-on-the-arduboy/9676)
- **A-maze** (MIT) by [Alojz Jakob](https://community.arduboy.com/t/a-maze-a-game-of-random-generated-mazes/5856)
- **Long Cat** (MIT) by [jaguile6, Dreamer2345](https://community.arduboy.com/t/longcat-puzzle-game/10331)
- **Trials of Astarok** (Unknonw) by [press play on tape](https://github.com/Press-Play-On-Tape/TrialsOfAstarok)
- **Number Puzzle** (???) by [t-iwasaki](https://github.com/t-iwasaki/arduboy-NumberPuzzle)
- **Horde and LATE** (???) by [justcallmekoko](https://github.com/justcallmekoko/ArduboyTheHorde/releases/tag/v0.3)
- **Onychophora** (MIT) by [renato-grottesi](https://community.arduboy.com/t/onychophora/8352)
- **Cooties attack!** (???) by [Diego Tarragona](https://github.com/graziel666/Cooties-Attack)
- **Crates 3D** (MIT) by [Brian](https://github.com/briansm-github/Crates3D)
- **Space Quarth** (MIT) by [Schuemi](https://github.com/Schuemi/SpaceQuarth)
- **ECOMD** (MIT) by [Team ARG](https://github.com/Team-ARG-Museum/ID-33-ECOMD)
- **Ardubrain** (Proprietary) by [renato-grottesi](https://github.com/renato-grottesi/arduboy/tree/master/ardubrain)
- **Arduminer** (Proprietary) by [Bergasms](http://www.bergasms.com/arduboy/Arduminer.html)
- **Asteroid Belt** (???) by [Xhaku Hans](https://gist.github.com/Gitonym/30718996c12ea4b918b0daf47d0409b3)
- **Diamonds** (MIT) by [redbug - Miguel Vanhove](https://community.arduboy.com/t/diamonds-a-breakout-puzzle-game-1st-kyuran-game/3368)
- **Crates** (MIT) by [jessemillar](https://community.arduboy.com/t/crates-pedal-to-the-metal-car-crime/6744)
- **CascadePath** (???) by [Chris](https://community.arduboy.com/t/cascade-path-wagon-train/1258) 
- **AsteroidBelt** (???) by [Xhaku](https://community.arduboy.com/t/asteroid-belt-tower-shooter/5627)
- **Arduminer** (???) by [Bergasms](https://community.arduboy.com/t/arduminer-terraria-like-game-by-bergasms/2939)
- **Ardubrain** (???) by [renato-grottesi](https://community.arduboy.com/t/ardubrain-a-brain-trainer/8301)
- **MayQ** (Proprietary) by [cobinee](http://blog.livedoor.jp/cobinee/archives/1930726.html)
- **SFZ** (Proprietary) by [cobinee](http://blog.livedoor.jp/cobinee/archives/1910024.html)
- **Lagunita** (MIT) by [renato-grottesi](https://community.arduboy.com/t/lagunita-a-1d-wild-west-town-builder/8135)
- **TeenyTank** (MIT) by [Jezzamon](https://community.arduboy.com/t/multiplayer-game-teenytank/585) 
- **Humanity revenge** (???) by [Giangregorio](https://community.arduboy.com/t/humanity-revenge-a-simple-shoot-em-up/1475)
- **1nvader** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/1nvader-v1-0/9665)
- **Bomberman** (???) by [LHW-HWT](https://github.com/createskyblue/Bomberman)
- **Juno First** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/juno-first-1-01/5554)
- **Domino** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/domino-all-fives-two-player/8087)
- **Road Trip** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/road-trip-v1-0/9716)
- **Blade Runner** (Apache-2.0) by [NoobGeek Ilya](https://community.arduboy.com/t/blade-runner/9698/17)
- **Leon Tamer** (BSD 3-Clause) by [Press Play on Tape](https://github.com/Press-Play-On-Tape/Lion)
- **Turtle bridge** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/turtle-bridge-1-3/7786)
- **Ninja Fuzzgrauth** (???) by [Hundstrasse](https://hundstrasse.com/arduboy-games/ninja-fuzzgrawth/)
- **Puzzle pack** (???) by [Atomic](https://community.arduboy.com/t/atomic-puzzle-pack/459)
- **Trolly Fish** (MIT) by [Team ARG](https://community.arduboy.com/t/trolly-fish-sixth-team-a-r-g-game/929)
- **Kong** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/kong-1-0-3/7962)
- **OilPanic** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/oil-panic-v1-0/9278)
- **FirePanic** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/fire-panic-1-0/7468)
- **Bomberboy** (???) by [evgenykzz2](https://github.com/evgenykzz2/Bomberboy)
- **CyberHack** (BSD 3-Clause) by [Press Play on Tape](https://community.arduboy.com/t/cyberhack-v1-0/9639)
- **DarkStar** (CC0 1.0) by [Mr.Blinky](https://github.com/MrBlinky/DarkStar)
- **Artillery** (???) by [Bergasms](http://www.bergasms.com/arduboy/Artillery.html)
- **SanSan** (???) by [Chamekan](https://community.arduboy.com/t/sansan-son-son-rygar-inspired-action-game/4621)
- **MicroTank** (Apache-2.0) by [Thomas Hartmann](https://github.com/hartmann1301/MicroTank)
- **LeannaTheLionAB** (MIT) by [SHDWWZRD of Neo Retro Games](http://www.neoretro.games)
- **Shattered Lands - Towers Of Perdition** (MIT) by [tuxinator2009](https://github.com/tuxinator2009/ShatteredLands) 
- **NewBlocksOnTheKid** (GNU) by [bateske Kevin](https://community.arduboy.com/t/new-blocks-on-the-kid/9197)
- **unicorn_dash** (MIT) by [Yossa Von K](https://yossagames.itch.io/unicorn-dash)
- **Square Nose** (by-nc-sa 4.0) by [JuiceLizard](https://community.arduboy.com/t/square-nose-the-squarrel/6226/27)
- **Festive fight** (Proprietary) by [Jonathan Holmes (crait) & Mario Vespa (greycove)](http://www.crait.net)
- **Kung-Fu escape** (MIT) by [Anthony Clarke (Sazazel)](https://github.com/MrSazazel/Kung_Fu_Escape)
- **Midnight Wild** (Proprietary) by [Jonathan Holmes (crait) & Mario Vespa (greycove)](http://www.crait.net)
- **Helii** (MIT)[BHSPitMonkey](https://github.com/BHSPitMonkey/Helii-Arduboy)
- **Pocket Fighter** (unknown) [WangRenxin](https://community.arduboy.com/t/pocket-fighter-an-ftg/2989)
- **Ardu Indy** (unknown) by [Jacoste](https://github.com/JacosteCode/ArduIndy)
- **Chrome Dyno** (unknown) by [flaki](https://github.com/ArduboyCollection/arduboy-rund-ino)
- **Space rocks** (LGPL-2.1) by [nootropicdesign](https://github.com/nootropicdesign/arduino-asteroids)
- **Reversi** (MIT) by [OBONO](https://community.arduboy.com/t/reversi-a-simple-board-game/6754)
- **Hopper** (MIT) by [OBONO](https://community.arduboy.com/t/hopper-a-simple-action-game/4293)
- **Evasion** (MIT) by [OBONO](https://community.arduboy.com/t/evasion-a-simple-action-game/9226)
- **Hollow Seeker** (MIT) by [OBONO](https://community.arduboy.com/t/hollow-seeker-a-simple-action-game/2594)
- **Ardubullet** (MIT) by [OBONO](https://community.arduboy.com/t/ardubullets-a-short-time-shmups-game/8684)
- **ArduBloxx** (Apache-2.0) by [NoobGeek Ilya](https://community.arduboy.com/t/ardubloxx-remake-of-city-bloxx/9420)
- **Catacombs of the damned** (MIT) by [James Howard](https://community.arduboy.com/t/catacombs-of-the-damned-formerly-another-fps-style-3d-demo/6565 "James Howard")
- **Mystic Balloon** (MIT) by [TEAM ARG](http://www.team-arg.org/ "by TEAM ARG")
- **Sun Fire** (MIT) by [TEAM ARG](http://www.team-arg.org/ "by TEAM ARG")
- **Sirene** (MIT) by [TEAM ARG](http://www.team-arg.org/ " by TEAM ARG")
- **Arduventure** (MIT) by [TEAM ARG](http://www.team-arg.org/ "by TEAM ARG")
- **CastleBoy** (MIT) by [jlauener](https://github.com/ArduboyCollection/CastleBoy)
- **Dark And Under** (BSD-3-Clause) by [Garage Collective (Cyril Guichard (Luxregina), Simon Holmes (Filmote), Pharap)](https://github.com/ArduboyCollection/Dark-And-Under)
- **HelloCommander** (MIT) by [Felipe Manga (FManga)](https://github.com/ArduboyCollection/HelloCommander)
- **Blob Attack** (MIT) by [TEAM ARG](http://www.team-arg.org/ "by TEAM ARG")
- **Virus-LQP-79** (MIT) by [TEAM ARG](http://www.team-arg.org/ "by TEAM ARG")
- **Kong-II** (MIT) by [Vampirics and Filmote](https://github.com/Press-Play-On-Tape/Kong-II)
- **LodeRunner (all 154 levels in a single game!)** (BSD-3) by [Simon Holmes (filmote)](https://community.arduboy.com/t/lode-runner-a-trilogy-in-four-parts/5385)
- **MicroCity** (GPL-3.0) by [James Howard](https://community.arduboy.com/t/microcity-city-simulation-in-your-pocket/4808)
- **SpaceCab** (GPL-3.0) by [Stephane C (vampirics) and Simon Holmes (filmote)](https://community.arduboy.com/t/space-cab-v1-0-1/6055)
- **The Curse Of Astarok** (MIT) by [Simon Holmes (filmote)](https://community.arduboy.com/t/the-curse-of-astarok-v1-0-with-sound/8203)
- **Rayne the rogue** (MIT) by [shdwwzrd](http://neoretro.games)
- **Space Battle - Trench Run** (MIT) by [Lucas Cardinali (lscardinali)](https://community.arduboy.com/t/space-battle-trench-run/8075)
- **MicroTD** (CC0-1.0) by [drummyfish](https://community.arduboy.com/t/microtd-tower-defense-for-arduboy/6223)
- **Shadow-Runner-1.6.1** (MIT) by [TEAM ARG](http://www.team-arg.org/)
- **Evade** (MIT) by [Modus Create](https://community.arduboy.com/t/evade-side-scrolling-space-shooter/2712)
- **Rooftop Rescue** (MIT) by [Bert van't Veer](https://community.arduboy.com/t/rooftop-rescue/7269)
- **Crates 3D** (MIT) by [Badja (original) & Brian (port)](https://community.arduboy.com/t/crates-3d-a-port-of-the-ti-83-puzzle-game/1540)
- **JetPac** (Proprietary) by [Mike McRoberts](https://community.arduboy.com/t/arduboy-jet-pac/2888)
- **Squario** (Proprietary) Squario by [arduboychris](https://community.arduboy.com/t/squario-the-ultimate-adventure/1257)
- **Circuit Dude** (Proprietary) by [Jonathan Holmes](http://www.crait.net), [upgrade to full color game!](http://circuitdude.com)
- **Omega Horizon** (Proprietary) by [shdwwzrd](http://neoretro.games)
- **Ard-Drivin** (MIT) by [Rem and LP](https://community.arduboy.com/t/ard-drivin-fast-paced-racing-sim/3178)
- **Tamaguino** (GNU GPL v3.0) by [Alojz Jakob](https://alojzjakob.github.io/Tamaguino/), ported to Arduboy by [Scott R](https://community.arduboy.com/t/tamaguino-ab-tamagotchi-for-arduino/4806)
- **Glove** (MIT) by [fuopy](https://github.com/ArduboyCollection/glove)
- **Blocks** (GPL-3.0) by [w3woody](https://github.com/ArduboyCollection/blocks)
- **Galaxion** (MIT) by [tako2](https://github.com/ArduboyCollection/galaxion)
- **Karateka** (BSD-3-Clause) by [Simon Holmes (filmote)](https://community.arduboy.com/t/karateka-v0-9/4852)
- **Tackle Box** (MIT) by [Matt](https://www.city41.games/tacklebox)
- **Not Just a Hat Rack** (Proprietary) by [Hundstrasse](https://hundstrasse.com/arduboy-games/not-just-a-hat-rack/)
- **Super-Crate-Buino-branched** (LGPL-3.0-or-later) by [Aurélien Rodot](http://legacy.gamebuino.com/forum/viewtopic.php?f=17&t=3151) [(ported by uXe)](https://github.com/Rodot/Super-Crate-Buino)
- **JumpBoy** (Proprietary) by [t-iwasaki](https://community.arduboy.com/t/jumpboy-action-game/2469)
- **ESP8266_ArduBOYNG** - UNKNOWN
- **ESP8266_breakout-v** - UNKNOWN
- **ESP8266_picovaders** - UNKNOWN

# Contributions and Disclaimer
Contributions to the documentation, code or electronics are welcome. 

Use this library at your own risk.

# ESPboy project links
- [Youtube videos](https://www.youtube.com/channel/UCsMjlCb8CK-Cw54lRrkpbQw "Youtube videos")
- [Community](https://www.espboy.com "Community")
- [Hackaday page](https://hackaday.io/project/164830-espboy-games-iot-stem-for-education-fun "Hackaday page")
- [Software](https://github.com/ESPboy-edu "Software")
- [Schematic, PCB, Gerber](https://easyeda.com/roman.sokolov/espboy-rev3-5 "Schematic, PCB, Gerber")
- E-mail: espboy.edu@gmail.com



# Arduboy2 Library

The Arduboy2 library is maintained in a git repository hosted on [GitHub](https://github.com/) at:

https://github.com/MLXXXp/Arduboy2

The **Arduboy2** library is a fork of the [Arduboy library](https://github.com/Arduboy/Arduboy), which provides a standard *application programming interface* (API) to the display, buttons and other hardware of the Arduino based [Arduboy miniature game system](https://www.arduboy.com/).

The name *Arduboy2* doesn't indicate that it's for a new "next generation" of the Arduboy hardware. The name was changed so it can coexist in the Arduino IDE with the current *Arduboy* library, without conflict. This way, existing sketches can continue to use the *Arduboy* library and class, without changes, while new sketches can be written (or old ones modified) to use and take advantage of the capabilities of the *Arduboy2* class and library.

For notes on the differences between the *Arduboy2* library and the original *Arduboy* library, and for information on migrating a sketch currently using the *Arduboy* library, see the sections at the end of this document.

## Library documentation

Comments in the library header files are formatted for the [Doxygen](http://www.doxygen.org) document generation system. The HTML files generated using the configuration file _extras/Doxyfile_ can be found at:

https://MLXXXp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/index.html

A generated PDF file can be found at:

https://MLXXXp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/pdf/Arduboy2.pdf

## Installation

The Arduboy2 library can be installed using the Arduino IDE Library Manager:

- In the Arduino IDE select from the menus: `Sketch > Include Library > Manage Libraries...`
- In the Library Manager *Filter your search...* field enter *arduboy2*.
- Click somewhere within the Arduboy2 entry.
- Click on the *Install* button.

For more library installation information see

[Installing Additional Arduino Libraries - Using the Library Manager](https://www.arduino.cc/en/Guide/Libraries#toc3)

## Start up features

The *begin()* function, used to initialize the library, includes features that are intended to be available to all sketches using the library (unless the sketch developer has chosen to disable one or more of them to free up some code space):

### The boot logo

At the start of the sketch, the **ARDUBOY** logo scrolls down from the top of the screen to the center.

The RGB LED lights red then green then blue while the logo is scrolling. (If your Arduboy is one of those that has the RGB LED installed incorrectly, then it will light blue then off then red). For users who do not wish to have the RGB LED flash during the boot logo sequence, a flag can be set in system EEPROM to have it remain off. The included *SetSystemEEPROM* example sketch can be used to set this flag.

A user settable *unit name* of up to 6 characters can be saved in system EEPROM memory. If set, this name will be briefly displayed at the bottom of the boot logo screen, after the logo stops scrolling down. This feature is only available if the *Arduboy2* class is used, not the *Arduboy2Base* class. This is because it requires the text display functions, which are only available in the *Arduboy2* class. A flag in system EEPROM controls whether or not the *unit name* is displayed on the boot logo screen, regardless of whether the *unit name* itself has been set. The included *SetSystemEEPROM* example sketch can be used to set both the *unit name* and this flag.

Once the logo display sequence completes, the sketch continues.

For developers who wish to quickly begin testing, or impatient users who want to go strait to playing their game, the boot logo sequence can be bypassed by holding the *RIGHT* button while powering up, and then releasing it. Alternatively, the *RIGHT* button can be pressed while the logo is scrolling down.

For users who wish to always disable the displaying of the boot logo sequence on boot up, a flag in system EEPROM is available for this. The included *SetSystemEEPROM* example sketch can be used to set this flag.

### "Flashlight" mode

If the *UP* button is pressed and held when the Arduboy is powered on, it enters *flashlight* mode. This turns the RGB LED fully on, and all the pixels of the screen are lit, resulting in a bright white light suitable as a small flashlight. (For an incorrect RGB LED, only the screen will light). To exit *flashlight* mode the Arduboy must be restarted.

*Flashlight* mode is also sometimes useful to allow uploading of new sketches, in case the sketch currently loaded uses a large amount of RAM which creates a bootloader problem.

### Audio mute control

Pressing and holding the *B* button when powering on will enter *System Control* mode. The RGB LED will light blue (red for an incorrect LED) to indicate that you are in *system control* mode. You must continue to hold the *B* button to remain in this mode. The only *system control* function currently implemented is *audio mute control*.

Pressing the *UP* button (while still holding *B*) will set a flag in system EEPROM indicating *audio enabled*. The RGB LED will flash green once (off for an incorrect LED) to indicate this action.

Pressing the *DOWN* button (while still holding *B*) will set the flag to *audio disabled* (muted). The RGB LED will flash red once (blue for an incorrect LED) to indicate this action.

Releasing the *B* button will exit *system control* mode and the sketch will continue.

Note that the audio control feature only sets a flag in EEPROM. Whatever code actually produces the sound must use the *audio.enabled()* function to check and honor the mute state. Audio libraries written with the Arduboy system in mind, such as the available *ArduboyPlaytune* and *ArduboyTones*, should do this. However, be aware that for some sketches, which don't use the Arduboy2 or other compliant library and generate sounds in their own way, this method of muting sound may not work.

## Using the library in a sketch

As with most libraries, to use Arduboy2 in your sketch you must include its header file at the start:

```cpp
#include <Arduboy2.h>
```

You must then create an Arduboy2 class object:

```cpp
Arduboy2 arduboy;
```

Naming the object *arduboy* has become somewhat of a standard, but you can use a different name if you wish.

To initialize the library, you must call its *begin()* function. This is usually done at the start of the sketch's *setup()* function:

```cpp
void setup()
{
  arduboy.begin();
  // more setup code follows, if required
}
```

The rest of the Arduboy2 functions will now be available for use.

If you wish to use the Sprites class functions you must create a Sprites object:

```cpp
Sprites sprites;
```

Sample sketches have been included with the library as examples of how to use it. To load an example, for examination and uploading to the Arduboy, using the Arduino IDE menus select:

`File > Examples > Arduboy2`

More information on writing sketches for the Arduboy can be found in the [Arduboy Community Forum](http://community.arduboy.com/).

### Using EEPROM in a sketch

The Arduboy2 library reserves an area at the start of EEPROM for storing system information, such as the current audio mute state and the Unit Name and Unit ID. A sketch **must not** use this reserved area for its own purposes. A sketch may use any EEPROM past this reserved area. The first EEPROM address available for sketch use is given as the defined value *EEPROM_STORAGE_SPACE_START*

### Audio control functions

The library includes an Arduboy2Audio class. This class provides functions to enable and disable (mute) sound and also save the current mute state so that it remains in effect over power cycles and after loading a different sketch. It doesn't contain anything to actually produce sound.

The Arduboy2Base class, and thus the Arduboy2 class, creates an Arduboy2Audio class object named *audio*, so a sketch doesn't need to create its own Arduboy2Audio object.

Example:

```cpp
#include <Arduboy2.h>

Arduboy2 arduboy;

// Arduboy2Audio functions can be called as follows:
  arduboy.audio.on();
  arduboy.audio.off();
```

### Simple tone generation

The *BeepPin1* and *BeepPin2* classes are available to generate simple square wave tones using speaker pin 1 and speaker pin 2 respectively. These classes are documented in file *Arduboy2Beep.h*. Also, *BeepDemo* is included as one of the example sketches, which demonstrates basic use.

NOTE: These functions will not work with a DevKit Arduboy because the speaker pins used cannot be directly controlled by a timer/counter. "Dummy" functions are provided so a sketch will compile and work properly but no sound will be produced.

### Ways to make more code space available to sketches

#### Sound effects and music

If all you want is to play single tones, using the built in *BeepPin1* or *BeepPin2* classes will be very efficient.

If you want to be able to play sequences of tones or background music, using the [*ArduboyTones*](https://github.com/MLXXXp/ArduboyTones) library will be more code efficient than using [*ArduboyPlaytune*](https://github.com/Arduboy/ArduboyPlayTune) or most other sound libraries compatible with the Arduboy. *ArduboyTones* even produces less code than the [Arduino built in *tone()* function](https://www.arduino.cc/en/Reference/Tone). You'll have to decide on the appropriate library or functions you use to generate sound, based on the features required and how much memory you want it to use.

#### Remove the text functions

If your sketch doesn't use any of the functions for displaying text, such as *setCursor()* and *print()*, you can remove them. You could do this if your sketch generates whatever text it requires by some other means. Removing the text functions frees up code by not including the font table and some code that is always pulled in by inheriting the [Arduino *Print* class](http://playground.arduino.cc/Code/Printclass).

To eliminate text capability in your sketch, when creating the library object simply use the *Arduboy2Base* class instead of *Arduboy2*:

For example, if the object will be named *arduboy*:

Replace

```cpp
Arduboy2 arduboy;
```

with

```cpp
Arduboy2Base arduboy;
```

#### Remove boot up features

As previously described, the *begin()* function includes features that are intended to be available to all sketches during boot up. However, if you're looking to gain some code space, you can call *boot()* instead of *begin()*. This will initialize the system but not include any of the extra boot up features. If desired, you can then add back in any of these features by calling the functions that perform them. You will have to trade off between the desirability of having a feature and how much memory you can recover by not including it.

A good way to use *boot()* instead of *begin()* is to copy the code from the body of the *begin()* function, in file *Arduboy2.cpp*, into your sketch and then edit it to retain the *boot()* call and any feature calls desired.

As of this writing, the begin function is:

```cpp
void Arduboy2Base::begin()
{
  boot(); // raw hardware

  display(); // blank the display (sBuffer is global, so cleared automatically)

  flashlight(); // light the RGB LED and screen if UP button is being held.

  // check for and handle buttons held during start up for system control
  systemButtons();

  audio.begin();

  bootLogo();

  waitNoButtons(); // wait for all buttons to be released
}
```

To incorporate it into your sketch just keep *boot()* and whatever feature calls are desired, if any. Comment out or delete the rest. Remember to add the class object name in front of each function call, since they're now being called from outside the class itself. If your sketch uses sound, it's a good idea to keep the call to *audio.begin()*.

For example: Let's say a sketch has its own code to enable, disable and save the *audio on/off* setting, and wants to keep the *flashlight* function. In *setup()* it could replace *begin()* with:

```cpp
  arduboy.boot(); // raw hardware

// *** This particular sketch clears the display soon, so it doesn't need this:
//  display(); // blank the display (sBuffer is global, so cleared automatically)

  arduboy.flashlight(); // light the RGB LED and screen if UP button is being held.

  // check for and handle buttons held during start up for system control
//  systemButtons();

  arduboy.audio.begin();

//  bootLogo();

//  waitNoButtons(); // wait for all buttons to be released
```

This saves whatever code *display()*, *systemButtons()*, *bootLogo()* and *waitNoButtons()* would use.

There are a few functions provided that are roughly equivalent to the standard functions used by *begin()* but which use less code space.

- *bootLogoCompressed()*, *bootLogoSpritesSelfMasked()*, *bootLogoSpritesOverwrite()*, *bootLogoSpritesBSelfMasked()* and *bootLogoSpritesBOverwrite()* will do the same as *bootLogo()* but will use *drawCompressed()*, or *Sprites* / *SpritesB* class *drawSelfMasked()* or *drawOverwrite()* functions respectively, instead of *drawBitmask()*, to render the logo. If the sketch uses one of these functions, then using the boot logo function that also uses it may reduce code size. It's best to try each of them to see which one produces the smallest size.
- *bootLogoText()* can be used in place *bootLogo()* in the case where the sketch uses text functions. It renders the logo as text instead of as a bitmap (so doesn't look as good).
- *safeMode()* can be used in place of *flashlight()* for cases where it's needed to allow uploading a new sketch when the bootloader "magic key" problem is an issue. It only lights the red RGB LED, so you don't get the bright light that is the primary purpose of *flashlight()*.

#### Use the SpritesB class instead of Sprites

The *SpritesB* class has functions identical to the *Sprites* class. The difference is that *SpritesB* is optimized for small code size rather than execution speed. If you want to use the sprites functions, and the slower speed of *SpritesB* doesn't affect your sketch, you may be able to use it to gain some code space.

Even if the speed is acceptable when using *SpritesB*, you should still try using *Sprites*. In some cases *Sprites* will produce less code than *SpritesB*, notably when only one of the functions is used.

You can easily switch between using *Sprites* or *SpritesB* by using one or the other to create an object instance:

```cpp
Sprites sprites;  // Use this to optimize for execution speed
SpritesB sprites; // Use this to (likely) optimize for code size
```

#### Eliminate the USB stack code

**Warning:** Although this will free up a fair amount of code and some RAM space, without an active USB interface uploader programs will be unable to automatically force a reset to invoke the bootloader. This means the user will have to manually initiate a reset in order to upload a new sketch. This can be an inconvenience or even frustrating for a user, due to the fact that timing the sequence can sometimes be tricky. Therefore, using this technique should be considered as a last resort. If it is used, the sketch documentation should state clearly what will be involved to upload a new sketch.

The *ARDUBOY_NO_USB* macro is used to eliminate the USB code. The *exitToBootloader()* function is available to make it easier for a user to invoke the bootloader. For more details, see the documentation provided for these.

----------

## What's different from Arduboy library V1.1

A main goal of Arduboy2 is to provide ways in which more code space can be freed for use by large sketches. Another goal is to allow methods other than the *tunes* functions to be used to produce sounds. Arduboy2 remains substantially compatible with [Arduboy library V1.1](https://github.com/Arduboy/Arduboy/releases/tag/v1.1), which was the latest stable release at the time of the fork. Arduboy2 is based on the code targeted for Arduboy library V1.2, which was still in development and unreleased at the time it was forked.

Main differences between Arduboy2 and Arduboy V1.1 are:

- The *ArduboyTunes* subclass, which provided the *tunes.xxx()* functions, has been removed. It's functionality is available in a separate [*ArduboyPlaytune* library](https://github.com/Arduboy/ArduboyPlayTune). By removing these functions, more code space may become available because interrupt routines and other support code was being compiled in even if a sketch didn't make use them. Another benefit is that without the automatic installation of timer interrupt service routines, other audio generating functions and libraries, that need access to the same interrupts, can now be used. Removal of the *tunes* functions is the main API incompatibility with Arduboy V1.1. Sketches written to use *tunes* functions will need some minor modifications in order to make them work with Arduboy2 plus ArduboyPlaytune, [ArduboyTones](https://github.com/MLXXXp/ArduboyTones), or some other audio library.
- Arduboy library V1.1 uses timer 1 for the *tunes* functions. This causes problems when attempting to control the Arduboy's RGB LED using PWM, such as with *setRGBled()*, because it also requires timer 1. Since the *tunes* functionality has been removed from Arduboy2, there are no problems with using the RGB LED (except those caused by the RGB LED being incorrectly installed). Of course, using an external library that uses timer 1, such as *ArduboyPlaytune*, may reintroduce the problems. However, using a library that doesn't use timer 1, such as *ArduboyTones*, is now an option.
- The code to generate text output, using *setCursor()*, *print()*, etc., can be removed to free up code space, if a sketch doesn't use any text functions. The *Arduboy2* class includes the text functions but using the *Arduboy2Base* class instead will eliminate them. With text functions included, the font table and some support functions are always compiled in even if not used. The API for using text functions is the same as Arduboy V1.1 with some additional functions added:
 - *setTextColor()* and *setTextBackground()* allow for printing black text on a white background.
 - *getCursorX()* and *getCursorY()* allow for determining the current text cursor position.
 - The *clear()* function will now reset the text cursor to home position 0, 0.
- A new feature has been added which allows the *audio on/off* flag in system EEPROM to be configured by the user when the sketch starts. The flag is used by the Arduboy and Arduboy2 *audio* subclass, along with external sound functions and libraries, to provide a standardized sound mute capability. See the information above, under the heading *Audio mute control*, for more details.
- The *color* parameter, which is the last parameter for most of the drawing functions, has been made optional and will default to WHITE if not included in the call. This doesn't save any code but has been added as a convenience, since most drawing functions are called with WHITE specified.
- A new function *digitalWriteRGB()* has been added to control the RGB LED digitally instead of using PWM. This uses less code if just turning the RGB LEDs fully on or off is all that's required.
- The *beginNoLogo()* function is not included. This function could be used in Arduboy V1.1 in place of *begin()* to suppress the displaying of the ARDUBOY logo and thus free up the code that it required. Instead, Arduboy2 allows a sketch to call *boot()* and then add in any extra features that *begin()* provides by calling their functions directly after *boot()*, if desired.
- The *ArduboyCore* and *ArduboyAudio* base classes, previously only available to, and used to derive, the *Arduboy* class, have been made publicly available for the benefit of developers who may wish to use them as the base of an entirely new library. This change doesn't affect the existing API.

As of version 2.1.0 functionality from the [Team A.R.G.](http://www.team-arg.org/) *Arglib* library has been added:

- The sprite drawing functions, collision detection functions, and button handling functions that Team A.R.G. incorporated from the [ArduboyExtra](https://github.com/yyyc514/ArduboyExtra) project. The *poll()* function was renamed *pollButtons()* for clarity. The *Sprites* class doesn't require a parameter for the constructor, whereas in *Arglib* a pointer to an Arduboy class object is required.
- The *drawCompressed()* function, which allows compressed bitmaps to be drawn. Saving bitmaps in compressed form may reduce overall sketch size.

Team A.R.G. has now migrated all of their games and demos to use the Arduboy2 library.

## Migrating a sketch from Arduboy library V1.1 to Arduboy2

Since the Arduboy2 library can coexist in the Arduino IDE alongside the Arduboy library V1.1, a currently working sketch that uses Arduboy V1.1 doesn't have to be migrated to Arduboy2. However, if you want to switch a sketch to Arduboy2 for further development, in order to take advantage of any of the changes and enhancements, it's generally relatively easy.

The Arduboy2 library, for the most part, is compatible with Arduboy library V1.1 but migrating a sketch to Arduboy2 will require some small changes, and more so if it uses the *tunes* functions, such as *tunes.tone()* or *tunes.playScore()*.

### Required changes

The first thing to do is change the `include` for the library header file:

```cpp
#include <Arduboy.h>
```

becomes

```cpp
#include <Arduboy2.h>
```

If it was "Arduboy.h" (in quotes), it's still better to change it to &lt;Arduboy2.h&gt; (in angle brackets).

The same thing has to be done with creating the library object. (If the object name isn't *arduboy*, keep whatever name is used.):

```cpp
Arduboy arduboy;
```

becomes

```cpp
Arduboy2 arduboy;
```

If the sketch doesn't use any *tunes* functions, there's a good chance this is all that has to be done to make it compile.

### Sketch uses only *tunes.tone()* for sound

If the sketch has sound but only uses *tunes.tone()*, solutions are:

#### Solution 1: Switch to using Arduino *tone()*

An easy change is to use the Arduino built in *tone()* function. You can add a function to the sketch that wraps *tone()* so that it works like *tunes.tone()*, like so:

```cpp
// Wrap the Arduino tone() function so that the pin doesn't have to be
// specified each time. Also, don't play if audio is set to off.
void playTone(unsigned int frequency, unsigned long duration)
{
  if (arduboy.audio.enabled() == true)
  {
    tone(PIN_SPEAKER_1, frequency, duration);
  }
}
```

You then change all *tunes.tone()* calls to *playTone()* calls using the same parameter values. For example:

```cpp
  arduboy.tunes.tone(1000, 250);
```

becomes

```cpp
  playTone(1000, 250);
```

#### Solution 2: Switch to using the ArduboyTones library

Changing to the *ArduboyTones* library is slightly more complicated. The advantage is that it will generate less code than using *tone()* and will also allow you to easily enhance the sketch to play tone sequences instead of just single tones. ArduboyTones can also play each tone at either normal or a higher volume.

You have to add an include for the ArduboyTones header file:

```cpp
#include <ArduboyTones.h>
```

You then have to create an object for the *ArduboyTones* class and pass it a pointer to the Arduboy2 *audio.enabled()* function. This must go after the creation of the Arduboy2 object, like so:

```cpp
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
```

You then change all Arduboy *tunes.tone()* calls to ArduboyTones *tone()* calls using the same parameter values. For example:

```cpp
  arduboy.tunes.tone(1000, 250);
```

becomes

```cpp
  sound.tone(1000, 250);
```

See the [ArduboyTones](https://github.com/MLXXXp/ArduboyTones) README file for more information on installing and using it.

#### Solution 3: Switch to using the ArduboyPlaytune library.

See the following for how to do this:

### Sketch uses *tunes.playScore()*

If the sketch uses *tunes.playScore()*, probably the easiest solution is to use the *ArduboyPlaytune* library. *ArduboyPlaytune* is essentially the code that was in the Arduboy V1.1 *tunes* subclass, which has been removed from Arduboy2. It's been cleaned up and a few enhancements have been added, but all the Arduboy V1.1 *tunes* functions are available.

You have to add an include for the ArduboyPlaytune header file:

```cpp
#include <ArduboyPlaytune.h>
```

You then have to create an object for the *ArduboyPlaytune* class and pass it a pointer to the Arduboy2 *audio.enabled()* function. This must go after the creation of the Arduboy2 object, like so:

```cpp
Arduboy2 arduboy;
ArduboyPlaytune tunes(arduboy.audio.enabled);
```

The sound channels must then be initialzed and assigned to the speaker pins. This code would go in the *setup()* function:

```cpp
  // audio setup
  tunes.initChannel(PIN_SPEAKER_1);
#ifndef AB_DEVKIT
  // if not a DevKit
  tunes.initChannel(PIN_SPEAKER_2);
#else
  // if it's a DevKit
  tunes.initChannel(PIN_SPEAKER_1); // use the same pin for both channels
  tunes.toneMutesScore(true);       // mute the score when a tone is sounding
#endif
```

If you name the ArduboyPlaytune object *tunes* as shown above, then you just have to remove the Arduboy object name from any *tunes* calls. For example:

```cpp
  arduboy.tunes.playScore(mySong);
```

becomes

```cpp
  tunes.playScore(mySong);
```

See the [*ArduboyPlaytune* library](https://github.com/Arduboy/ArduboyPlayTune) documentation for more information.

If you don't need to play scores containing two parts, and don't require tones to be played in parallel with a score that's playing, then as an alternative to using *ArduboyPlaytune* you may wish to consider switching to *ArduboyTones*. This may require a bit of work because any *ArduboyPlaytune* scores would have to be converted to *ArduboyTones* format. It would involve changing note numbers to frequencies. This could be simplified by using the provided *NOTE_* defines. Also, durations would have to be converted, including adding silent "rest" tones as necessary.

The benefit of using *ArduboyTones* would be reduced code size and possibly easier addition of new sequences without the need of a MIDI to Playtune format converter.

### Sketch uses the *beginNoLogo()* function instead of *begin()*

The *beginNoLogo()* function has been removed. Instead, *boot()* can be used with additional functions following it to add back in desired boot functionality. See the information above, under the heading *Remove boot up features*, for more details. Assuming the object is named *arduboy*, a direct replacement for *beginNoLogo()* would be:

```cpp
  arduboy.boot();
  arduboy.display();
  arduboy.flashlight();
  arduboy.audio.begin();
```

----------


