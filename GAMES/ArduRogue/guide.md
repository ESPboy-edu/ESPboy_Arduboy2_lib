# Guide to ArduRogue

**WARNING: Spoilers ahead!**

Jump to Section:
- [How to Play](#howtoplay)
    - [Controls](#controls)
    - [Action Menu](#actionmenu)
    - [Game Menu](#gamemenu)
    - [Saving and Loading](#saving)
- [Game Reference](#gamereference)
    - [Items](#itemtypes)
        - [Potions](#potions)
        - [Scrolls](#scrolls)
        - [Rings](#rings)
        - [Amulets](#amulets)
        - [Wands](#wands)
    - [Enemies](#enemytypes)
    - [Combat Formulas](#combatformulas)
      - [Melee Combat](#meleecombat)
      - [Ranged Combat](#rangedcombat)
      - [Magic Damage](#magicdamage)

## How to Play <a name="howtoplay"></a>

### Controls <a name="controls"></a>

Use the arrow buttons to move in the cardinal directions.
Unlike most roguelikes, ArduRogue does not support diagonal movement or actions.
- Moving into an [enemy](#enemytypes) will perform a melee attack on that enemy.
- Moving onto an [item](#itemtypes) will produce a prompt asking whether to pick up the item.
- Moving onto stairs will produce a prompt asking whether to descend or ascend the stairs.

When presented with a prompt, press A to accept or B to decline.

### Action Menu <a name="actionmenu"></a>

During normal gameplay, press B to access the action menu.
The action menu allows you to perform non-movement actions, such as shooting an arrow or drinking a potion.
While in the action menu, use UP or DOWN to cycle the menu options, and LEFT or RIGHT to switch to the game menu.
Press A to select an action to perform, or B to leave the menu with no selection.

During normal gameplay, press A to repeat the previously performed action from the action menu.
This is useful to perform the same action multiple times without having to access the action menu each time, such as when
- searching for a hidden door,
- using a bow and arrow for ranged combat,
- dropping several items to clean out your inventory, or
- using items multiple times in a row, like a wand.

### Game Menu <a name="gamemenu"></a>

While in the action menu, press LEFT or RIGHT to switch to the game menu.
The game menu provides access to additional information, such as a map of the current dungeon level or player stats.
It also provides the option to [save your game](#saving).

### Saving and Loading <a name="saving"></a>

After pressing A at the title screen, ArduRogue will either continue from a saved game, if it exists, or begin a new game.

**Beware! ArduRogue enforces permadeath by deleting the saved game after it is loaded. Be sure to save every time when you're finished playing.**
Saving a game returns you to the title screen.

ArduRogue will only write to EEPROM when
- you select "Save and Exit" from the game menu,
- you continue playing from an existing saved game (it is then deleted), or
- you consent to saving a high score after the end of a game.

Settings are also stored with the saved game, but are not deleted when loading.

The EEPROM is not written to when beginning a new game. If you never save your progress or save a high score then ArduRogue will never touch EEPROM.

## Game Reference <a name="gamereference"></a>

### Items <a name="itemtypes"></a>

| Symbol              | Name        | Stackable | Equippable | Notes |
| :-----------------: | :---------: | :-------: | :--------: | ----- |
| ![](img/food.png)   | food ration | &#9745;   |            | Consumable. Staves off hunger. |
| ![](img/potion.png) | potion      | &#9745;   |            | Provides a temporary magical effect. Affects the user when drunk, or the target when thrown. |
| ![](img/scroll.png) | scroll      | &#9745;   |            | Performs some magical effect when read. |
| ![](img/wand.png)   | wand        |           |            | Performs some directed ranged magical effect when used. |
| ![](img/bow.png)    | bow         |           | Weapon     | Must be equipped for ranged combat. |
| ![](img/arrow.png)  | arrow       | &#9745;   |            | Required for ranged combat. |
| ![](img/sword.png)  | sword       |           | Weapon     | Gives bonus to attack. |
| ![](img/ring.png)   | ring        |           | Hand       | Can wear up to two rings at a time. Provides a constant magical effect. |
| ![](img/amulet.png) | amulet      |           | Neck       | Provides a constant magical effect. |
| ![](img/armor.png)  | armor       |           | Body       | Gives bonus to defense. |
| ![](img/armor.png)  | helm        |           | Head       | Gives bonus to defense. |
| ![](img/armor.png)  | boots       |           | Feet       | Gives bonus to defense. |

#### Potions <a name="potions"></a>

| Potion of... | Effect |
| ------------ | ------ |
| healing      | Restores the target's strength if weakened. Heals the target for 1/4 to 3/4 of max health. |
| strength     | Restores the target's strength if weakened. Otherwise, increases the target's strength by 1. |
| dexterity    | Increases the target's dexterity by 1. |
| experience   | Immediately grants a set amount of experience. |
| invisibility | Makes the target temporarily invisible. |
| harming      | Damages the target for 1/8 to 3/8 of max health, capped at 10. |
| poison       | Weakens the target. |
| confusion    | Temporarily confused the target. |
| paralysis    | Temporarily paralyzes the target. |
| slowing      | Temporarily slows the target. |

#### Scrolls <a name="scrolls"></a>

| Scroll of...  | Effect |
| ------------- | ------ |
| identify      | Reveals the type and quality of a chosen item. |
| enchant       | Enhances the quality of a chosen item, or recharges a wand. |
| remove curse  | Removes the curse on an item, allowing it to be removed. |
| teleport      | Teleports the reader. |
| magic mapping | Reveals the full layout of the current dungeon level. |
| fear          | Causes all enemies in view to flee temporarily. |
| torment       | Immediately halves the health of the reader and all enemies in view. |
| mass confuse  | Temporarily confuses the reader and all enemies in view. |
| mass poison   | Weakens the reader and all enemies in view. |

#### Rings <a name="rings"></a>

| Ring of...    | Effect | Cursed Effect |
| ------------- | ------ | ------------- |
| see invisible | Allows the wearer to see invisible beings. | The wearer has a 50% chance to be unable to see an enemy, whether or not it is invisible. |
| strength      | Grants a bonus to the wearer's strength. | Gives a penalty to the wearer's strength. |
| dexterity     | Grants a bonus to the wearer's dexterity. | Gives a penalty to the wearer's dexterity. |
| protection    | Grants a bonus to the wearer's defense, as if wearing better armor. | Gives a penalty to the wearer's defense. |
| fire immunity | The wearer is immune to fire. | Fire damages the wearer twice as much. |
| attack        | Grants a bonus to the wearer's attack, as if holding a stronger weapon. | Gives a penalty to the wearer's attack. |
| sustenance    | Slows down the advance of hunger. | Hastens the advance of hunger. |
| invisibility  | The wearer is permanently invisible. | The wearer cannot become invisible, even temporarily. |

#### Amulets <a name="amulets"></a>

| Amulet of... | Effect | Cursed Effect |
| ------------ | ------ | ------------- |
| speed        | Grants a bonus to the wearer's speed. | Gives a penalty to the wearer's speed. |
| clarity      | The wearer cannot be confused. | The wearer occasionally becomes confused. |
| conservation | Chance for the wearer to not consume the item when using scrolls or potions. | Chance for the wearer to consume two items. |
| regeneration | The wearer slowly heals. | Halves all healing the wearer receives. |
| the vampire  | The wearer heals for a small amount on each melee hit. | Nothing. |
| ironblood    | The wearer cannot be paralyzed. | The wearer occasionally becomes paralyzed. |
| vitality     | Grants a bonus to the wearer's maximum health. | Gives a penalty to the wearer's maximum health. |
| wisdom       | The wearer gains experience 50% more quickly. | The wearer gains experience 50% more slowly. |

#### Wands <a name="wands"></a>

| Wand of... | Effect |
| ---------- | ------ |
| force      | Pushes the target back. |
| teleport   | Teleports the target. |
| digging    | Digs through walls in the given direction. |
| fire       | Target and all others nearby take fire damage. |
| striking   | Target takes strong magic damage. |
| ice        | Target takes magic damage and is slowed. |
| polymorph  | Target is transormed into another creature: 75% chance to be a weaker creature, 25% chance to be stronger. |

### Enemies <a name="enemytypes"></a>

| Symbol                   | Name             | Str | Dex | Spd | Def | Health | Notes |
| ------------------------ | ---------------- | --: | --: | --: | --: | -----: | ----- |
| ![](img/bat.png)         | bat              |   1 |   6 |   8 |   0 |      1 | Passive: does not attack until attacked. |
| ![](img/snake.png)       | snake            |   2 |   3 |   3 |   0 |      3 |  |
| ![](img/rattlesnake.png) | rattlesnake      |   3 |   3 |   3 |   0 |      4 | Chance to poison on hit. |
| ![](img/zombie.png)      | zombie           |   4 |   2 |   2 |   0 |      6 |  |
| ![](img/goblin.png)      | goblin           |   5 |   4 |   4 |   1 |     10 |  |
| ![](img/phantom.png)     | phantom          |   6 |   4 |   4 |   1 |     12 | Invisible. |
| ![](img/orc.png)         | orc              |   7 |   4 |   4 |   3 |     16 |  |
| ![](img/tarantula.png)   | tarantula        |   5 |   4 |   4 |   0 |     12 | Chance to paralyze on hit. |
| ![](img/hobgoblin.png)   | hobgoblin        |   8 |   4 |   4 |   2 |     20 |  |
| ![](img/mimic.png)       | mimic            |   7 |   4 |   4 |   3 |     20 | Disguises itself as an item. |
| ![](img/incubus.png)     | incubus          |   9 |   4 |   4 |   3 |     24 | Chance to confuse on hit. |
| ![](img/troll.png)       | troll            |  10 |   3 |   3 |   5 |     32 | Regenerates health. |
| ![](img/griffin.png)     | griffin          |   7 |   6 |   6 |   1 |     24 |  |
| ![](img/dragon.png)      | dragon           |  12 |   4 |   4 |   8 |     48 | Breathes fire. |
| ![](img/angel.png)       | fallen angel     |  10 |   6 |   6 |   3 |     24 | Chance to confuse or paralyze on hit. |
| ![](img/darkness.png)    | Lord of Darkness |  16 |   6 |   8 |   8 |    128 | Regenerates. Chance to poison, confuse, or paralyze on hit. |

### Combat Formulas <a name="combatformulas"></a>

When a physical attack (melee or ranged) lands, two random values are calculated:
- the damage roll for the attacker and
- the defense roll for the defender.

How the damage roll is calculated differs between melee and ranged combat.
The defense roll depends only on the defense stat, and is a random value from floor(Def/4) to Def, inclusive.
For example, for a defense stat of 13, the defense roll will be a random value from 3 to 13.
The following table shows the damage dealt for different values of damage and defense rolls.

<table>
<tr><td /><td colspan="20" align="center"><b>Defense Roll</b></td></tr>
<tr align="center"><td><b>Damage Roll</b></td><td><b>0</b></td><td><b>1</b></td><td><b>2</b></td><td><b>3</b></td><td><b>4</b></td><td><b>5</b></td><td><b>6</b></td><td><b>7</b></td><td><b>8</b></td><td><b>9</b></td><td><b>10</b></td><td><b>11</b></td><td><b>12</b></td><td><b>13</b></td><td><b>14</b></td><td><b>15</b></td><td><b>16</b></td><td><b>17</b></td><td><b>18</b></td><td><b>19</b></td></tr>
<tr align="right"><td><b>0</b></td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
<tr align="right"><td><b>1</b></td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
<tr align="right"><td><b>2</b></td><td>2</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
<tr align="right"><td><b>3</b></td><td>3</td><td>2</td><td>2</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
<tr align="right"><td><b>4</b></td><td>4</td><td>3</td><td>3</td><td>2</td><td>2</td><td>2</td><td>2</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
<tr align="right"><td><b>5</b></td><td>5</td><td>4</td><td>4</td><td>3</td><td>3</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
<tr align="right"><td><b>6</b></td><td>6</td><td>5</td><td>5</td><td>4</td><td>4</td><td>3</td><td>3</td><td>3</td><td>3</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>1</td><td>1</td><td>1</td><td>1</td></tr>
<tr align="right"><td><b>7</b></td><td>7</td><td>6</td><td>5</td><td>5</td><td>4</td><td>4</td><td>4</td><td>3</td><td>3</td><td>3</td><td>3</td><td>3</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td><td>2</td></tr>
<tr align="right"><td><b>8</b></td><td>8</td><td>7</td><td>6</td><td>6</td><td>5</td><td>5</td><td>5</td><td>4</td><td>4</td><td>4</td><td>4</td><td>3</td><td>3</td><td>3</td><td>3</td><td>3</td><td>3</td><td>2</td><td>2</td><td>2</td></tr>
<tr align="right"><td><b>9</b></td><td>9</td><td>8</td><td>7</td><td>7</td><td>6</td><td>6</td><td>5</td><td>5</td><td>5</td><td>4</td><td>4</td><td>4</td><td>4</td><td>4</td><td>3</td><td>3</td><td>3</td><td>3</td><td>3</td><td>3</td></tr>
<tr align="right"><td><b>10</b></td><td>10</td><td>9</td><td>8</td><td>8</td><td>7</td><td>7</td><td>6</td><td>6</td><td>6</td><td>5</td><td>5</td><td>5</td><td>5</td><td>4</td><td>4</td><td>4</td><td>4</td><td>4</td><td>3</td><td>3</td></tr>
<tr align="right"><td><b>11</b></td><td>11</td><td>10</td><td>9</td><td>9</td><td>8</td><td>8</td><td>7</td><td>7</td><td>6</td><td>6</td><td>6</td><td>5</td><td>5</td><td>5</td><td>5</td><td>5</td><td>4</td><td>4</td><td>4</td><td>4</td></tr>
<tr align="right"><td><b>12</b></td><td>12</td><td>11</td><td>10</td><td>10</td><td>9</td><td>9</td><td>8</td><td>8</td><td>7</td><td>7</td><td>7</td><td>6</td><td>6</td><td>6</td><td>6</td><td>5</td><td>5</td><td>5</td><td>5</td><td>5</td></tr>
<tr align="right"><td><b>13</b></td><td>13</td><td>12</td><td>11</td><td>11</td><td>10</td><td>9</td><td>9</td><td>8</td><td>8</td><td>8</td><td>7</td><td>7</td><td>7</td><td>6</td><td>6</td><td>6</td><td>6</td><td>6</td><td>5</td><td>5</td></tr>
<tr align="right"><td><b>14</b></td><td>14</td><td>13</td><td>12</td><td>12</td><td>11</td><td>10</td><td>10</td><td>9</td><td>9</td><td>9</td><td>8</td><td>8</td><td>8</td><td>7</td><td>7</td><td>7</td><td>7</td><td>6</td><td>6</td><td>6</td></tr>
<tr align="right"><td><b>15</b></td><td>15</td><td>14</td><td>13</td><td>13</td><td>12</td><td>11</td><td>11</td><td>10</td><td>10</td><td>9</td><td>9</td><td>9</td><td>8</td><td>8</td><td>8</td><td>7</td><td>7</td><td>7</td><td>7</td><td>7</td></tr>
<tr align="right"><td><b>16</b></td><td>16</td><td>15</td><td>14</td><td>14</td><td>13</td><td>12</td><td>12</td><td>11</td><td>11</td><td>10</td><td>10</td><td>10</td><td>9</td><td>9</td><td>9</td><td>8</td><td>8</td><td>8</td><td>8</td><td>7</td></tr>
<tr align="right"><td><b>17</b></td><td>17</td><td>16</td><td>15</td><td>14</td><td>14</td><td>13</td><td>13</td><td>12</td><td>12</td><td>11</td><td>11</td><td>10</td><td>10</td><td>10</td><td>9</td><td>9</td><td>9</td><td>8</td><td>8</td><td>8</td></tr>
<tr align="right"><td><b>18</b></td><td>18</td><td>17</td><td>16</td><td>15</td><td>15</td><td>14</td><td>14</td><td>13</td><td>13</td><td>12</td><td>12</td><td>11</td><td>11</td><td>10</td><td>10</td><td>10</td><td>10</td><td>9</td><td>9</td><td>9</td></tr>
<tr align="right"><td><b>19</b></td><td>19</td><td>18</td><td>17</td><td>16</td><td>16</td><td>15</td><td>14</td><td>14</td><td>13</td><td>13</td><td>12</td><td>12</td><td>12</td><td>11</td><td>11</td><td>11</td><td>10</td><td>10</td><td>10</td><td>9</td></tr>
</table>

#### Melee Combat <a name="meleecombat"></a>

The chance to land a melee attack depends on both the attacker's and defender's dexterity stats.
The following table shows the chance to successfully land a hit for different stat combinations.

<table>
<tr><td /><td colspan="15" align="center"><b>Defender Dexterity</b></td></tr>
<tr align="center"><td><b>Attacker Dexterity</b></td><td><b>0</b></td><td><b>1</b></td><td><b>2</b></td><td><b>3</b></td><td><b>4</b></td><td><b>5</b></td><td><b>6</b></td><td><b>7</b></td><td><b>8</b></td><td><b>9</b></td><td><b>10</b></td><td><b>11</b></td><td><b>12</b></td><td><b>13</b></td><td><b>14</b></td></tr>
<tr align="right"><td><b>0</b></td><td>100%</td><td>50%</td><td>33%</td><td>25%</td><td>20%</td><td>17%</td><td>14%</td><td>13%</td><td>11%</td><td>10%</td><td>9%</td><td>8%</td><td>8%</td><td>7%</td><td>7%</td></tr>
<tr align="right"><td><b>1</b></td><td>100%</td><td>80%</td><td>67%</td><td>57%</td><td>50%</td><td>44%</td><td>40%</td><td>36%</td><td>33%</td><td>31%</td><td>29%</td><td>27%</td><td>25%</td><td>24%</td><td>22%</td></tr>
<tr align="right"><td><b>2</b></td><td>100%</td><td>88%</td><td>78%</td><td>70%</td><td>64%</td><td>58%</td><td>54%</td><td>50%</td><td>47%</td><td>44%</td><td>41%</td><td>39%</td><td>37%</td><td>35%</td><td>33%</td></tr>
<tr align="right"><td><b>3</b></td><td>100%</td><td>91%</td><td>83%</td><td>77%</td><td>71%</td><td>67%</td><td>63%</td><td>59%</td><td>56%</td><td>53%</td><td>50%</td><td>48%</td><td>45%</td><td>43%</td><td>42%</td></tr>
<tr align="right"><td><b>4</b></td><td>100%</td><td>93%</td><td>87%</td><td>81%</td><td>76%</td><td>72%</td><td>68%</td><td>65%</td><td>62%</td><td>59%</td><td>57%</td><td>54%</td><td>52%</td><td>50%</td><td>48%</td></tr>
<tr align="right"><td><b>5</b></td><td>100%</td><td>94%</td><td>89%</td><td>84%</td><td>80%</td><td>76%</td><td>73%</td><td>70%</td><td>67%</td><td>64%</td><td>62%</td><td>59%</td><td>57%</td><td>55%</td><td>53%</td></tr>
<tr align="right"><td><b>6</b></td><td>100%</td><td>95%</td><td>90%</td><td>86%</td><td>83%</td><td>79%</td><td>76%</td><td>73%</td><td>70%</td><td>68%</td><td>66%</td><td>63%</td><td>61%</td><td>59%</td><td>58%</td></tr>
<tr align="right"><td><b>7</b></td><td>100%</td><td>96%</td><td>92%</td><td>88%</td><td>85%</td><td>81%</td><td>79%</td><td>76%</td><td>73%</td><td>71%</td><td>69%</td><td>67%</td><td>65%</td><td>63%</td><td>61%</td></tr>
<tr align="right"><td><b>8</b></td><td>100%</td><td>96%</td><td>93%</td><td>89%</td><td>86%</td><td>83%</td><td>81%</td><td>78%</td><td>76%</td><td>74%</td><td>71%</td><td>69%</td><td>68%</td><td>66%</td><td>64%</td></tr>
<tr align="right"><td><b>9</b></td><td>100%</td><td>97%</td><td>93%</td><td>90%</td><td>88%</td><td>85%</td><td>82%</td><td>80%</td><td>78%</td><td>76%</td><td>74%</td><td>72%</td><td>70%</td><td>68%</td><td>67%</td></tr>
<tr align="right"><td><b>10</b></td><td>100%</td><td>97%</td><td>94%</td><td>91%</td><td>89%</td><td>86%</td><td>84%</td><td>82%</td><td>79%</td><td>78%</td><td>76%</td><td>74%</td><td>72%</td><td>70%</td><td>69%</td></tr>
<tr align="right"><td><b>11</b></td><td>100%</td><td>97%</td><td>94%</td><td>92%</td><td>89%</td><td>87%</td><td>85%</td><td>83%</td><td>81%</td><td>79%</td><td>77%</td><td>76%</td><td>74%</td><td>72%</td><td>71%</td></tr>
<tr align="right"><td><b>12</b></td><td>100%</td><td>97%</td><td>95%</td><td>93%</td><td>90%</td><td>88%</td><td>86%</td><td>84%</td><td>82%</td><td>80%</td><td>79%</td><td>77%</td><td>76%</td><td>74%</td><td>73%</td></tr>
<tr align="right"><td><b>13</b></td><td>100%</td><td>98%</td><td>95%</td><td>93%</td><td>91%</td><td>89%</td><td>87%</td><td>85%</td><td>83%</td><td>82%</td><td>80%</td><td>78%</td><td>77%</td><td>75%</td><td>74%</td></tr>
<tr align="right"><td><b>14</b></td><td>100%</td><td>98%</td><td>96%</td><td>93%</td><td>91%</td><td>90%</td><td>88%</td><td>86%</td><td>84%</td><td>83%</td><td>81%</td><td>80%</td><td>78%</td><td>77%</td><td>75%</td></tr>
</table>

The damage roll for a melee hit depends on the attacker's attack stat.
For an attack stat value N, the damage roll will be in the inclusive range from floor(N/4) to N.
For example, for an attack stat of 9, the damage roll will be a random value from 2 to 9.

#### Ranged Combat <a name="rangedcombat"></a>

The chance to land a ranged attack depends on both the attacker's and defender's dexterity stats.
The following table shows the chance to successfully land a hit for different stat combinations.

<table>
<tr><td /><td colspan="15" align="center"><b>Defender Dexterity</b></td></tr>
<tr align="center"><td><b>Attacker Dexterity</b></td><td><b>0</b></td><td><b>1</b></td><td><b>2</b></td><td><b>3</b></td><td><b>4</b></td><td><b>5</b></td><td><b>6</b></td><td><b>7</b></td><td><b>8</b></td><td><b>9</b></td><td><b>10</b></td><td><b>11</b></td><td><b>12</b></td><td><b>13</b></td><td><b>14</b></td></tr>
<tr align="right"><td><b>0</b></td><td>100%</td><td>50%</td><td>33%</td><td>25%</td><td>20%</td><td>17%</td><td>14%</td><td>13%</td><td>11%</td><td>10%</td><td>9%</td><td>8%</td><td>8%</td><td>7%</td><td>7%</td></tr>
<tr align="right"><td><b>1</b></td><td>100%</td><td>50%</td><td>33%</td><td>25%</td><td>20%</td><td>17%</td><td>14%</td><td>13%</td><td>11%</td><td>10%</td><td>9%</td><td>8%</td><td>8%</td><td>7%</td><td>7%</td></tr>
<tr align="right"><td><b>2</b></td><td>100%</td><td>67%</td><td>50%</td><td>40%</td><td>33%</td><td>29%</td><td>25%</td><td>22%</td><td>20%</td><td>18%</td><td>17%</td><td>15%</td><td>14%</td><td>13%</td><td>13%</td></tr>
<tr align="right"><td><b>3</b></td><td>100%</td><td>67%</td><td>50%</td><td>40%</td><td>33%</td><td>29%</td><td>25%</td><td>22%</td><td>20%</td><td>18%</td><td>17%</td><td>15%</td><td>14%</td><td>13%</td><td>13%</td></tr>
<tr align="right"><td><b>4</b></td><td>100%</td><td>75%</td><td>60%</td><td>50%</td><td>43%</td><td>38%</td><td>33%</td><td>30%</td><td>27%</td><td>25%</td><td>23%</td><td>21%</td><td>20%</td><td>19%</td><td>18%</td></tr>
<tr align="right"><td><b>5</b></td><td>100%</td><td>75%</td><td>60%</td><td>50%</td><td>43%</td><td>38%</td><td>33%</td><td>30%</td><td>27%</td><td>25%</td><td>23%</td><td>21%</td><td>20%</td><td>19%</td><td>18%</td></tr>
<tr align="right"><td><b>6</b></td><td>100%</td><td>80%</td><td>67%</td><td>57%</td><td>50%</td><td>44%</td><td>40%</td><td>36%</td><td>33%</td><td>31%</td><td>29%</td><td>27%</td><td>25%</td><td>24%</td><td>22%</td></tr>
<tr align="right"><td><b>7</b></td><td>100%</td><td>80%</td><td>67%</td><td>57%</td><td>50%</td><td>44%</td><td>40%</td><td>36%</td><td>33%</td><td>31%</td><td>29%</td><td>27%</td><td>25%</td><td>24%</td><td>22%</td></tr>
<tr align="right"><td><b>8</b></td><td>100%</td><td>83%</td><td>71%</td><td>63%</td><td>56%</td><td>50%</td><td>45%</td><td>42%</td><td>38%</td><td>36%</td><td>33%</td><td>31%</td><td>29%</td><td>28%</td><td>26%</td></tr>
<tr align="right"><td><b>9</b></td><td>100%</td><td>83%</td><td>71%</td><td>63%</td><td>56%</td><td>50%</td><td>45%</td><td>42%</td><td>38%</td><td>36%</td><td>33%</td><td>31%</td><td>29%</td><td>28%</td><td>26%</td></tr>
<tr align="right"><td><b>10</b></td><td>100%</td><td>86%</td><td>75%</td><td>67%</td><td>60%</td><td>55%</td><td>50%</td><td>46%</td><td>43%</td><td>40%</td><td>38%</td><td>35%</td><td>33%</td><td>32%</td><td>30%</td></tr>
<tr align="right"><td><b>11</b></td><td>100%</td><td>86%</td><td>75%</td><td>67%</td><td>60%</td><td>55%</td><td>50%</td><td>46%</td><td>43%</td><td>40%</td><td>38%</td><td>35%</td><td>33%</td><td>32%</td><td>30%</td></tr>
<tr align="right"><td><b>12</b></td><td>100%</td><td>88%</td><td>78%</td><td>70%</td><td>64%</td><td>58%</td><td>54%</td><td>50%</td><td>47%</td><td>44%</td><td>41%</td><td>39%</td><td>37%</td><td>35%</td><td>33%</td></tr>
<tr align="right"><td><b>13</b></td><td>100%</td><td>88%</td><td>78%</td><td>70%</td><td>64%</td><td>58%</td><td>54%</td><td>50%</td><td>47%</td><td>44%</td><td>41%</td><td>39%</td><td>37%</td><td>35%</td><td>33%</td></tr>
<tr align="right"><td><b>14</b></td><td>100%</td><td>89%</td><td>80%</td><td>73%</td><td>67%</td><td>62%</td><td>57%</td><td>53%</td><td>50%</td><td>47%</td><td>44%</td><td>42%</td><td>40%</td><td>38%</td><td>36%</td></tr>
</table>


#### Magic Damage <a name="magicdamage"></a>
