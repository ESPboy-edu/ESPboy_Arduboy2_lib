# Ardulem
Alban Nanty, 2017

## What is Ardulem?
Ardulem is an attempt to bring the legendary "Lemmings" game to the Arduboy platform.
This is my first game on Arduboy, which I did to learn that console, so please be indulgent with the result. :blush:

## Installation Guide
Ardulemn is hosted on `bitbucket` at this URL [https://bitbucket.org/banban/ardulem](https://bitbucket.org/banban/ardulem).

### Using the .arduboy file.

You can find a `Ardulem.arduboy` packaged version of the game, in the `Release` folder. then you can use a game loader program like the [ArduboyManager](https://community.arduboy.com/t/arduboy-manager-2-0-released/2809) (by crait) to load the game in your Arduboy. However, the packaged version is updated less often than the full repository.

1. Download the Release\Ardulem.arduboy file.
2. Import it in ArduboyManager
3. Select "Ardulem" in the list and click on the Transfert button.

### Using the Arduboy IDE

Ardulem doesn't need the Arduboy library, because I've included and modified it directly in the project for optimization reasons.

1. Clone or [download](https://bitbucket.org/banban/ardulem/downloads/) a zip of the the repository.
2. Connect your Arduboy on your computer and launch the Arduino IDE.
3. Open the `Ardulem.ino` file in the Arduino IDE.
4. Press the upload button. Ardulem should compile fine. 

## Game Overview
Ardulem was very, I mean very very, challenging regarding the memory usage (both progmem and RAM). I spent a lot of time optimizing a lot the progmem size, and I couldn't even add all the feature I wanted, such as a page for help in game. I've also get rid of the logo to save some more progmem. So the game start right in the main menu.

### Main Menu
You can navigate in the main menu using the arrow keys. The `Level` option let you choose which level you want to play without limitation (you don't need to complete the first levels to play the next ones). However, if you start with the first one, and finish it successfully, your progression will be saved in the EEPROM. The last option `Reset Save` let you delete your progression to start over.

Finally you have an option to turn on/off the Music (which is also saved in the EEPROM). The music may be a bit annoying after a while, but I suggest you to try at least one level with it. :relaxed:

### Game

#### The game screen

The screen is split in two parts, on the left you have your HUD or toolbar, providing you usefull information, and including the command bar, to give orders to your Lems. And on the right side, you have the level, that can scroll left and right with your cursor (there's no vertical scrolling).

Your cursor is a blinking cross (that changes to a square when hovering a Lem) just under the Lem's drop hatch. You can use the **arrow keys** to move the cursor. The goal of the game is to move the cursor over the Lems and press the **B button** to give them orders to lead them to the exit door which has a moving flag on top. If you succeed to save at least as much Lems than the required number for the level, then you won that level, and you can move on the next one.

To order your Lems, you need to first select your command in the left toolbar. The tool drawn in white is the current selected tool. To navigate in the toolbar, you can simply press the **A button** to select the next tool. Alternatively, if you want to navigate more precisely, **Hold A** and use the **arrow keys** at the same time.

#### The toolbar

From top to bottom of the toolbar, you can see:

- First you can see the percentage of Lems who has already drop.
- Then the percentage of Lems who have reached the exit.
- Then the percentage of Lems that you need to save to win the level. If you have reach that number, the little flag next to it, will move. This option is selectable in the toolbar. If you select it and press **B**, you will be prompt with a message asking you if you want to abandon. This is the normal way to return to the menu.
- Then you have the timer. When the time is out, the game is over. You can also press **B** on the timer, if you do so, it will pause the game, but you can still scroll the view to observe the level. The game always start with the timer selected, so you can immediately press B when you enter in a level, to take time to observe it. Moreover, if you **hold A + RIGHT** you can swith to Speedy Mode, which accelerate the time, and make everything goes faster. This is usefull if you have already create a path all the way to exit and don't want to wait too long for all the Lems to reach the exit. **hold A + LEFT** will return to normal mode.
- Then you have a 3x3 grid that regroups the 9 orders that you can give to your Lems. From left to right, top to bottom you have: Walker, Blocker, Exploder, Diagonal Digger, Horizontal Digger, Vertical Digger, Stair Builder, Climber, Parachuter. The first role "Walker" doesn't exist in the original game; this role can be used to stop a digger or a stair builder for example, and ask him to resum his walk. In each on the 9 cells, you can also see the number of available orders. When the number reach zero, nothing is drawn.
- Finally, the last tool represent the drop speed. A top little cursor is moving accross the bar, and each time it reaches the right border a Lem is drop. A bottom cursor allows you to control the Lem's drop speed. You can increase or decrease the dropping speed by **holding A** and use the **LEFT and RIGHT** arrows.


### Result page

If there's no more alive Lems in the level, and all of them have drop, then the game will end. The game also ends if the timer reaches zero, or if you abandon the game. In all these cases, you will land on the Result page which will display the level number, the number of saved Lems and the number of required Lems to win.

If you succeeded, the LED will pulse in green, otherwise it will pulse in red.

Pressing any key will return you to the menu.

## Key Binding
Here is a complete summary of the key bindings

### In Menu

- **UP/DOWN**: Choose the selected menu item.
- **LEFT/RIGHT**: Choose the level number - Enable or disable the music
- **A** or **B**: Start to play the level - Enable or disable the music - Reset the save (Warning! there's no feedback on this menu item)

### In Game

Here is the list of possible actions and how to perform them:

- *Move the cursor and/or scroll the screen:* **UP/DOWN/LEFT/RIGHT**
- *Move the selected tool in the toolbar:* **Hold A + UP/DOWN/LEFT/RIGHT**. Or simply press **A** to move to the next one.
- *Give an order to a Lem:* Select the correct order in the toolbar, then move the cursor on the Lem, and press **B**
- *Pause the game:* Move the selection on the timer, then press **B**. Press **B** again to remove the pause.
- *Switch to Speedy Mode:* Move the selection on the timer, then **hold A + RIGHT**
- *Switch to Normal Mode:* Move the selection on the timer, then **hold A + LEFT**
- *Abandon and return to Menu:* Move the selection on the flag, then press **B**, then confirm by pressing **A**.
- *Adjust the Lem drop speed:* Move the selection on the drop speed widget, then **hold A + LEFT/RIGHT**

Or if you prefer to see what each button does:

- **UP/DOWN/LEFT/RIGHT** alone: Move the cursor. If the cursor reach the border of the screen, the screen will scroll.
- **Hold A + UP/DOWN/LEFT/RIGHT**: navigate in the toolbar. While doing so, the cursor won't move on screen. On the drop speed widget it can adjust the drop speed, and on the timer it can switch between Speedy and Normal Mode.
- **A**: One single press on A will move the selected tool to the next one.
- **B**: This is the action button. It gives an order to a Lem if the cursor is on a Lem and the current selected tool is an Lem command - it pauses the game if the timer is selected - it leaves the game if the flag is selected.

## Level Editor
This game comes with a level editor written in C# (with very standard C#, no fancy stuff). I've developed it on Windows, but you wouldn't probably have too much difficulties to make it run on Linux or Mac with Mono.

There's a compiled version of the Editor in the `Release` folder. The Editor needs the 3 png images (`Home.png`, `MapSprite.png` and `Start.png`) in the same folder as the exe file, so you should also copy them if you move the `ArdulemEditor.exe` file elsewhere.

The Editor code can be found in the sub-folder named `ArdulemEditor`, just in case you want to modify it.

The editor use the clipboard to load/save the data that you want to edit. The data should be pasted into the `MapData.cpp` file of the Ardulem project, before recompiling/reuploading the game to the Arduboy.

More details on the editor may come later.

## Boring Stuff

This program is free and open source.
Author: Alban Nanty, 2017.
 
If you want to report bugs or feature request, please do it through the [bitbucket project page](https://bitbucket.org/banban/ardulem/issues) were
this code is hosted. However, there's no guaranty that I will fix them.

It you want to participate, mod the game, please do so, and I would love to know it. We are on an open source platform so a big part of the fun is to share! :smile:

If you've created amzingly fun level for the game, please send them to me. I'm definitely not the best level designer here, so I would be happy to add it in the official release version (or replace an existing level, depending of the progmem available size).

At some point, if there's too many great levels, I could also saved all of them in a separated text file, commited in the repository.