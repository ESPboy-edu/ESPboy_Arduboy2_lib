<img src="/distributable/SplashScreen.png" data-canonical-src="/distributable/SplashScreen.png" width="700" height="257" />

## Background

Mini Rogue is a 9-card game in which a single player delves into a deep dungeon to get the famous ruby called The Og's Blood on the bottom floor. The player must choose how to spend their resources in order to be powerful enough to confront ever difficult monsters. Random events and encounters make every play-through a unique experience.

The game was originally developed by **Paolo Di Stefano** and **Gabriel Gendron** for the 2016 9-Card Nanogame Design Contest (https://boardgamegeek.com/thread/1491476/2016-9-card-nanogame-pnp-design-contest). It won:

* 1st place - Best New Designers
* 2nd place - Best Overall Game
* 2nd place - Best Solitaire Game
* 2nd place - Best Artwork
* 3rd place - Best Thematic Game
* 3rd place - Best Written Rules

More information about the game can be found here &gt; https://gumroad.com/l/MiniRogue12

Source code can be found here &gt; https://github.com/filmote/MiniRogue

HEX file can be found here &gt; https://github.com/filmote/MiniRogue/tree/master/distributable

Play online here &gt; https://felipemanga.github.io/ProjectABE/?hex=https://raw.githubusercontent.com/filmote/MiniRogue/master/distributable/MiniRogue.ino.hex

The Arduboy version of Mini Rogue was created with the permission of the original authors. Mini Rogue is published by Mountain Gold Games. Copyright 2016 Mountain Gold Games. All Rights Reserved.
The source code for the game is released under the Apache Licence, Version 2.0.
It uses libraries by Ignacio Vina @igvina (ArdBitMap) and an application framework by @Pharap.  These too are distributed under the Apache licence.

## Object of the Game

In this solitaire game, you play as an adventurer that delves into a dungeon, room after room, area after area, level after level, to loot the **Og's Blood**: a fabled and mysterious ruby gemstone. Each area of the dungeon is laid out as a branching of rooms. Each time you face two rooms, you’ll have to choose one. You will resolve each encounter by rolling dice, and decide on how to pursue the adventure. In each room, you may encounter a monster, find a treasure, discover a resting area, meet a merchant, dodge traps, or face other surprising events. Each level ends with a powerful boss monster that you’ll have to defeat to continue onto to the next level. You win the game if you can reach the last room of the Dungeon and defeat the final Boss Monster.

## The Arduboy Version

In adapting the game to the Arduboy, @Vampirics and I (@Filmote) have taken a few liberties but in the whole have tried to stay true to the original game by keeping the dice and card metaphor.

The following instructions have been summarised from the complete instructions originally written by **Paolo Di Stefano** and **Gabriel Gendron**. We encourage you to read the full instructions as those below are incomplete and probably contain errors! The instructions and card artwork are included in **\distributable** directory of the project repo.

## Game Play

### Starting the Game

Mini Rogue has four starting levels that dictate the amount of armour, HP, gold and food you start with.

| Level&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | HP&nbsp;&nbsp;&nbsp;&nbsp; | Gold&nbsp;&nbsp;&nbsp; | Food&nbsp;&nbsp;&nbsp; |
| --- | --- | --- | --- |
|Easy|1|5|5|6|
|Normal|0|5|3|6|
|Hard|0|4|2|5|
|Hell|0|3|1|3|

These statistics and others are displayed on the right-hand side of the screen, as shown below.


<img src="/distributable/Images/MR_Icons_001.png" data-canonical-src="/distributable/Images/MR_Icons_001.png" width="50" height="22" />&nbsp;&nbsp;&nbsp;&nbsp;**Rank**: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;As you accumulate experience (XP) in the dungeon your rank.  This will assist in your
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ability to fight the various monsters you will find in the dungeon, refer **Monster Card** and
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;**Boss Monster Card** below.

<img src="/distributable/Images/MR_Icons_001.png" data-canonical-src="/distributable/Images/MR_Icons_002.png" width="50" height="22" />&nbsp;&nbsp;&nbsp;&nbsp;**XP**: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;You will be rewarded XP as you defeat monsters or stumble across treasures in the 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;dungeon.

<img src="/distributable/Images/MR_Icons_003.png" data-canonical-src="/distributable/Images/MR_Icons_003.png" width="50" height="22" />&nbsp;&nbsp;&nbsp;&nbsp;**HP**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Your health will take a beating as you battle monsters but you can top it up when you visit 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;a Merchant or discover it hidden in the dungeon.

<img src="/distributable/Images/MR_Icons_004.png" data-canonical-src="/distributable/Images/MR_Icons_004.png" width="50" height="22" />&nbsp;&nbsp;&nbsp;&nbsp;**Armour**: Armour will protect you when you inevitably encounter a monster in the dungeon. Armour
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;can be bought at the Merchant or destroyed through acid rain.

<img src="/distributable/Images/MR_Icons_005.png" data-canonical-src="/distributable/Images/MR_Icons_005.png" width="50" height="22" />&nbsp;&nbsp;&nbsp;&nbsp;**Gold**:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;You can buy your way out of trouble with enough gold. Gain extra gold be killing monsters 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;and finding treasures.

<img src="/distributable/Images/MR_Icons_006.png" data-canonical-src="/distributable/Images/MR_Icons_006.png" width="50" height="22" />&nbsp;&nbsp;&nbsp;&nbsp;**Food**:&nbsp;&nbsp;&nbsp;&nbsp;Food is essential to life in the dungeon.  You will need consume a food ration every time
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;uou complete an area in the dungeon.

<br/>

### Rank and XP

As you become more experienced fighting monsters in the dungeon, your XP points will increase.  Once you have accumulated enough XP points, your rank will be upgraded (to a maximum of four).  Increased rank provides you with additional weapons when fighting the dungeon's monsters.  Refer to the section **Monster Card** and **Boss Monster Card** below.

| Starting Rank | Number of XP |
| --- | --- |
|  1 | 6 XP |
| 2 | 12 XP |
| 3 | 18 XP |
| 4 | Maximum Rank! |

### Progressing through the Dungeon

Each level in the dungeon has a number of areas and each area has a number of rooms. The rooms are represented by 7 cards and are dealt randomly on each level. You must progress through the rooms before delving to the next area. When completing the last area of a level, you will fight a Boss monster and be rewarded handsomely if you win.

When starting a level or area, the room cards are dealt in the pattern shown below. If the current area is not the last one for the level, the last card shown will be a ‘delving’ card that will take you to the next area (as shown on the left). If not, the last card will be a Boss Monster which will hamper your movement between areas (as shown on the right).

<img src="/distributable/Images/MR_Init_001.png" data-canonical-src="/distributable/Images/MR_Init_001.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Init_002.png" data-canonical-src="/distributable/Images/MR_Init_002.png" width="320" height="160" />

As you can see, the current level and area are displayed in the top left hand corner.

Play starts by flipping the first card. As there is only one visible card, you must complete the room by pressing the A button before moving on. Refer to the room description below for a brief description. Once the room is complete, you will return to the dungeon screen and the next two cards will be revealed. You can scroll between them by pressing up or down and selecting the either room to investigate.

Once you have completed the last room in an area you must consume 1 Food before proceeding to the next area or level. As in real life, if you run out of food its game over!

The dungeon levels and area are laid out as shown below:

<img src="/distributable/Images/MR_Levels_001.png" data-canonical-src="/distributable/Images/MR_Levels_001.png" width="200" height="320" />

### The Rooms

**Treasure Card**

<img src="/distributable/Images/MR_Treasure_001.png" data-canonical-src="/distributable/Images/MR_Treasure_001.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Treasure_002.png" data-canonical-src="/distributable/Images/MR_Treasure_002.png" width="320" height="160" /> 

<img src="/distributable/Images/MR_Treasure_003.png" data-canonical-src="/distributable/Images/MR_Treasure_003.png" width="320" height="200" />

On your journey, you will find forgotten stashes of loot in varying quantities. Monsters still protect the most valuable treasures in the dungeon.

Your reward is determined by the roll of a die. If you roll four or below, you are simply rewarded with gold. If you roll a five or above, you are rewarded a random treasure from the list below. In either case, the amount of gold you receive is depends on whether you have defeated any Monster earlier in this same Area – if so, you gain 2 Gold pieces otherwise you only gain 1 Gold piece.

Possible rewards include:

* Armour Piece: Immediately gain 1 Armour.

* Better Weapon: Immediately gain 2 XP.

* Fireball Spell: In combat, inflict 8 damage points to a Monster.

* Ice Spell: In combat, freeze a Monster for one turn. The Monster does not counter-attack.

* Poison Spell: In combat, inflict 5 extra damage per turn for the remainder of the fight.

* Healing Spell: Gain 8 HP in combat.

<br/>

**Trap Card**

<img src="/distributable/Images/MR_Trap_001.png" data-canonical-src="/distributable/Images/MR_Trap_001.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Trap_002.png" data-canonical-src="/distributable/Images/MR_Trap_002.png" width="320" height="160" />

<img src="/distributable/Images/MR_Trap_003.png" data-canonical-src="/distributable/Images/MR_Trap_003.png" width="320" height="200" />

Dungeon means danger, and danger sometimes means traps. Many adventurers perished not through combat, but through lack of scrutiny. You can avoid a trap if you are sufficiently skilled. This is determined by a roll of a die – if the result is less than or equal to your rank you can skip the trap. If not, you must endure one of the following traps:

* Mould: A terrible stench seems to have added a layer of white and blue hair on your meat. Lose 1 Food ration.

* Tripwire: You tripped and fell hard to the ground. A Gold piece was ejected from your bag. Lose 1 Gold.

* Acid Mist: Powerful acid falls from the ceiling and damages your equipment. Lose 1 Armour.

* Spring Blades: You walked on a pressure plate and jumped just in time to avoid losing your head. Lose 1 HP.

* Moving Walls: Moving walls were about to crush you, but you sacrificed your sword to save yourself. Lose 1 XP.

* Pit: You fell into a hole and landed a Level below. You broke a bone. Lose 2 HP and drop a level in the maze.

<br/>

**Event Card**

Like the Trap Card (above), your fate will be tested by your skill level. Roll a number greater than your rank, and you will be awarded with something useful. Occasionally you will be made to fight an extra monster - sometimes when you least expect it.

<img src="/distributable/Images/MR_Event_001.png" data-canonical-src="/distributable/Images/MR_Event_001.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Event_002.png" data-canonical-src="/distributable/Images/MR_Eevnt_002.png" width="320" height="160" />

Roll a number less than or equal to your current rank and you will be able to choose your winnings. Scroll between the options using the **Left** and **Right Buttons** and select your choice using the **A Button**. 

<img src="/distributable/Images/MR_Event_003.png" data-canonical-src="/distributable/Images/MR_Event_003.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Event_004.png" data-canonical-src="/distributable/Images/MR_Eevnt_004.png" width="320" height="160" />

You may even stumble upon items of great assistance. Most of the rewards are good but occasionally you will be forced to fight a monster.

* Found Ration: You don’t know what this meat is, and you don’t care. Gain 1 Food.

* Found Health Potion: A Monster’s favourite drink. Might as well take a sip too. Gain 2 HP.

* Found Loot: You have found a coin hidden in a crack between two stones. Gain 2 Gold.

* Found Whetstone: You have found a Monster’s blade sharpening tools. Gain 2 XP.

* Found Armour: A piece of armour found on a Monster’s carcass. Gain 1 Armour.

* Monster Fight! The monster will inflict damage to the value of the current Level x 2. You will receive 2 XP if you defeat it.

<br/>

**Resting Card**

<img src="/distributable/Images/MR_Rest_001.png" data-canonical-src="/distributable/Images/MR_Rest_001.png" width="320" height="160" /> 

Every man needs respite, for death could come anytime soon. When resting, you may choose only one of the available options by scrolling the highlight and pressing the A button to select.

* Reinforce your weapon. Gain 1 XP.

* Search for Ration. Gain 1 Food.

* Take the time to Heal. Gain 2 HP.

<br/>

**Merchant Card**

<img src="/distributable/Images/MR_Merchant_001.png" data-canonical-src="/distributable/Images/MR_Merchant_001.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Merchant_002.png" data-canonical-src="/distributable/Images/MR_Merchant_001.png" width="320" height="160" /> 

Some unsettling beings that call themselves merchants can be seen wandering in the dungeon. Surely a peculiar place for them to be doing business..

When you encounter the Merchant card, you may spend any number of Gold pieces to purchase items that will improve your Armour, Food, HP as well as Spells. You may buy or sell any number of items on your turn, as long as you have the money. It is never mandatory; you may skip the Merchant card if needed by pressing the **B Button**.

Scroll through the available choices using the **Up** and **Down Buttons**. Commit to a transaction by pressing the **A Button**. Toggle between buying and selling by pressing the **Left** and **Right Buttons**. When complete, press the **B Button** to return to the dungeon.

| Item | Buy Price | Sell Price | Effect |
| --- | --- | --- | --- |
| Ration | 1 Gold | | Gain 1 Food. |
| Health Potion|1 Gold||Gain 1 HP.|
| Big Health Potion&nbsp;&nbsp;|3 Gold||Gain 4 HP.|
| Armor Piece|6 Gold|3 Gold|Gain 1 Armour.|
| Any 1 Spell|8 Gold|4 Gold|Gain any 1 Spell.|

<br/>

**Monster Card**

<img src="/distributable/Images/MR_Monster_001.png" data-canonical-src="/distributable/Images/MR_Monster_001.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Monster_002.png" data-canonical-src="/distributable/Images/MR_Monster_002.png" width="320" height="160" />

You have encountered a Monster and must defeat him in order to progress. The monsters HP increases in line with your own level and is calculated by adding a dice roll to the level value. If you are on level 2, the monster could have an HP in the range of 3 to 8 (ie. level 2 plus a dice roll of 1 – 6). The damage a monster inflicts and the reward you will receive if you defeat it are also dependent on your level and are detailed in the card below:

You attack first by rolling a dice. The dice score indicates the amount of damage you will inflict on the monster, however ..

* If you roll a 1, you will inflict no damage.
* If you roll a 6, you may re-roll the dice and combine the newly rolled value and the original 6 together. Rolling a dice again is risky – if you happen to roll a 1 you have lost everything. To re-roll a dice, highlight it by scrolling left or right and press the A button.

As the game progresses, your XP will increase and your rank will increase as detailed in the section **Rank and XP**. In addition to the rank increase, the number of dice you possess will also increase making combat against stronger foes possible.

Once you have finished rolling, select the ‘Arrow’ and press **A Button** to apply the damage, if any, to the monster.

If you have collected any wands, you may cast a spell now. The various wands will be displayed along with your current inventory holdings. To apply a spell, highlight the appropriate wand and press the **A Button** to invoke it. You do not have to use a wand and you may highlight the arrow instead and press the **A Button** to continue.

The monster will now attack. The damage inflicted is shown in the top left hand corner of the screen. If you are lucky enough to have armour, then the damage value is reduced by the amount of armour you have.

Play continues until you or the monster is dead. If you are victorious then you will be rewarded with gold and XP before returning to the dungeon.

| Level | HP&nbsp;&nbsp;&nbsp; | Reward&nbsp;&nbsp;&nbsp; |
| --- | --- | --- |
| 1 Undead Soldier | 2 | 1XP |
| 2 Skeleton | 2 | 1XP |
| 3 Undead Knight | 6 | 2XP |
| 4 Serpent Knight&nbsp;&nbsp;&nbsp; | 8 | 2XP |
| 5 OG's Sanctum Guard&nbsp;&nbsp;&nbsp; | 10&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 3XP |

<br/>

**Boss Monster Card**

<img src="/distributable/Images/MR_Boss_001.png" data-canonical-src="/distributable/Images/MR_Boss_001.png" width="320" height="160" />&nbsp;&nbsp; <img src="/distributable/Images/MR_Boss_002.png" data-canonical-src="/distributable/Images/MR_Boss_002.png" width="320" height="160" />

These fallen titans were once great Lords. Only cats and hags know their original identities, for their names have been forsaken.

The Boss Monster’s HP and damage inflicted are detailed in the card below. In addition to these perks, you will receive a random treasure (see **Treasure Card** above) as well.

| Level | HP | Damage | Reward |
| --- | --- | --- | --- |
| 1 Undead Giant | 10 | 3 | 2 Gold + 2 XP + Item |
| 2 Skeleton Lord | 15 | 5 | 2 Gold + 3 XP + Item |
| 3 Undead Lord | 20 | 7 | 3 Gold + 4 XP + Item |
| 4 Serpent Demon&nbsp;&nbsp;&nbsp; | 25 | 9 | 3 Gold + 5 XP + Item |
| 5 OG's Remains |30 | 12 | Og's Blood |

<br/>

Game play is identical to the **Monster Card** described above.

</br>

## Scoring

You are rewarded points for every area you enter, boss monster's you kill and the collection of goodies along the way.  To reset the scores, hold down the **Up Button** and **Down Buttons** simultaneously for about 5 seconds while on this screen.

<img src="/distributable/Images/MR_Score.png" data-canonical-src="/distributable/Images/MR_Score.png" width="320" height="160" />

## Uploading a new Game

This game removes the USB support to allow extra memory for graphics.  To upload a new game, press and hold the down button whilst on the title screen to drop to the boot loader (the screen will go black).  Once done, you can upload a new game using the Arduino IDE or AVRDUDE.

More details can be found here >
https://github.com/MLXXXp/Arduboy2
