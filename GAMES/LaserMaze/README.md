# LaserMaze
Arduboy version form the board game Laser Maze.

Laser Maze is a game in which you have to use mirrors to bend the light of a laser beam (the arrow) so that you hit the targets (open circles) on the playing field. 

For this you have 3 types of mirrors: 
1. Normal mirror (triangular in shape). It bends the light 90 degrees.
2. Semi-transparent mirror (straight in shape). It splits the incoming light. 1 jet goes straight ahead and 1 jet bends 90 degrees.
3. A prism (thicker and dotted in shape) that splits the light into 2 different wavelengths. 
   - The straight laser beam should hit the target which consists of straight lines. 
   - The dotted laser beam should hit the target which consists of dotted lines.

The screen is divided into 2 fields. On the left is the playing field (5x5). You can place and rotate the mirrors in this field. If there is already a mirror in the playing field at the start of the level, this mirror can be rotated but cannot be moved. 
On the right are a maximum of 5 mirrors that you have to place in the playing field to solve the level. You can select these mirrors again after placing them in the playing field. Then you can move or rotate the mirror.

Buttons: 
1. Arrow buttons: Move cursor (square).
2. A button: If you press A, it selects the mirror that is below the cursor (it will blink). 
3. A button: If you press A on an empty cursor position, the selected mirror will move to the cursor position. 
4. A button: If you press A on a selected mirror (which flashes), the mirror rotates 90 degrees. 
5. B button: If you press B, you turn on the laser. The course of the laser beam can be seen on the screen. If you hit a target, the target lights up. 
   - If you see a thumbs up, you have hit all targets. 
   - If you see a cross, you have not yet hit all the targets. 
6. B button: If you press B again, you turn off the laser and you can rotate or move the mirrors again. If you had hit all the targets (thumbs up) you go to the next level. 

Levels: 
In total, the set consists of 70 levels. 
If the game is interrupted halfway through, you will be asked to continue the game from the last level played or to start again from level 1.


Good luck solving the levels.

Copyright: 2025 Frank van de Ven 
Licence:   MIT
