# LATE - Little Arduboy Timekiller Endeavour
A game that features several modes that fit into Arduboy memmory. I find them simple enough to code and pack together, so I'll be able to enjoy them on Arduboy too.

The game state is saved in the EEPROM when the pause menu is issued (usually by pressing [B]).

As of now, there's no support for RGB LED and sound.

Music is not planned at all.

## Tips and tricks:

Hold [Left] while pressing [A] in the menu for reset.
That way you can reset the current game state to start a new game (in the pause menu) or even reset the whole game data for the current mode (in the main menu), including high scores and any game state.

If you don't own Arduboy you can still build and play the game online [here](https://felipemanga.github.io/ProjectABE/?url=https://github.com/core1024/LATE) thaks to Project ABE.

## Modes:
 * **Blocks Arcade** - Tetris clone. Graphics inspied by the Gameboy version. Gameplay features ghost piece, wall kicks and lock delay.
    ### Fetures
    - Classic Tetris gameplay
    - Reconfigurable buttons: While playing the game, hold together Left and Right and then press Up, Down or A to change function. The changes are saved in the program memory when the game is paused. They are reset with the scores.

        - Up: Instant drop/rotate
        - Down: fast/instant drop
        - A: rotate (counter) clockwise - This also affects Up if configured as rotate.


    ### TODO:
    - Wall kicks sometimes feel wired
    - Line animation
    - RGB/Sound signals for start, lock, line and game over.

 * **Blocks Puzzle** - A clone of 1010! by Gram Games.
    ### Fetures
    - Enjoyable gameplay
    - Easy on the eyes graphics
    ### TODO:
    - Performance tweaks.
    - Scoring systems? I need to verify with the origial for correctness.
    - Random generator. According to [this page](http://blog.coelho.net/games/2016/07/28/1010-game.html) 1010! uses uneven random distribution accross the pieces.
    - RGB/Sound signals for obstructed/placed/picking block and game over.

 * **Bridge Builder** - A clone of Stick Hero by Ketchapp.
    ### Fetures
    - Enjoyable gameplay
    ### TODO:
    - Performance tweaks.
    - Animate coming of the next platform.
    - RGB/Sound signals for bridge building, received bonus and game over.
