#ifndef LEVELS_H
#define LEVELS_H

#define MAX_AMOUNT_OF_ROOMS                       32
#define MAX_AMOUNT_OF_INFLUENCING_OBJECTS         16
#define MAX_AMOUNT_OF_TRANSPORTERS                16
#define AMOUNT_OF_ROOMS_AT_BYTE                   0
#define AMOUNT_OF_TRANSPORTERS_AT_BYTE            1
#define AMOUNT_OF_INFLUENCING_OBJECTS_AT_BYTE     2
#define LEVEL_DOOR_DATA_START_AT_BYTE             3
#define LEVEL_ROOM_DATA_START_AT_BYTE             4
#define ROOMS_DATA_START_AT_BYTE                  5
#define DOORS_DATA_START_AT_BYTE                  ROOMS_DATA_START_AT_BYTE + 1
#define ELEMENTS_DATA_START_AT_BYTE               ROOMS_DATA_START_AT_BYTE + 5            
#define BYTES_USED_FOR_EVERY_ROOM                 13

// ROOM ORDER OF TILES
//                 /\
//                /  \
//               / 00 \
//              /\    /\
//             /  \  /  \
//            / 01 \/ 05 \
//            \    /\    /
// NORTH       \  /  \  /         EAST
//           02 \/ 06 \/ 10  
//        /\    /\    /\    /\
//       /  \  /  \  /  \  /  \
//      / 03 \/ 07 \/ 11 \/ 15 \
//     /\    /\    /\    /\    /\
//    /  \  /  \  /  \  /  \  /  \
//   / 04 \/ 08 \/ 12 \/ 16 \/ 20 \
//   \    /\    /\    /\    /\    /
//    \  /  \  /  \  /  \  /  \  /
//     \/ 09 \/ 13 \/ 17 \/ 21 \/
//      \    /\    /\    /\    /
//       \  /  \  /  \  /  \  /
//        \/ 14 \/ 18 \/ 22 \/
//              /\    /\    
// WEST        /  \  /  \         SOUTH
//            / 19 \/ 23 \
//            \    /\    /
//             \  /  \  /
//              \/ 24 \/
//               \    /
//                \  /
//                 \/


//const unsigned char PROGMEM centerOfTiles[][2] =
//{
//  { 58, 18}, {46, 24}, {34, 30}, {22, 36}, {10, 42}, // Tile  0  1  2  3  4
//  { 70, 24}, {58, 30}, {46, 36}, {34, 42}, {22, 48}, // Tile  5  6  7  8  9
//  { 82, 30}, {70, 36}, {58, 42}, {46, 48}, {34, 54}, // Tile 10 11 12 13 14
//  { 94, 36}, {82, 42}, {70, 48}, {58, 54}, {46, 60}, // Tile 15 16 17 18 19
//  {106, 42}, {94, 48}, {82, 56}, {70, 60}, {58, 66}, // Tile 20 21 22 23 24
//};

// NEXT LEVEL DOOR
//0b00000001,
//  |||||||└->  \  these 2 bits are used to determine what door is the next level door, make sure it exists
//  ||||||└-->  /. NORTH = 0B00000000; EAST = 0B00000001; SOUTH = 0B00000010; WEST : 0B00000011
//  |||||└--->  \ 
//  ||||└---->   |
//  |||└----->   | these 6 bits are used to set in which room the next level door is
//  ||└------>   |
//  |└------->   |
//  └-------->  / 

// NEXT LEVEL ROOM
//0b00000001,
//  |||||||└--->  \ 
//  ||||||└---->   |
//  |||||└----->   | these 6 bits are used to set in which room the next level TILE is
//  ||||└------>   |
//  |||└------->   |
//  ||└-------->  / 
//  |└--------->  NOT USED
//  └---------->  NOT USED

// ALL THE DATA FOR EACH ROOM AND EACH ROOM HAS 13 BYTES
// DOORS         NORTH        EAST       SOUTH        WEST       ENEMY1      ENEMY2     OBJECT3     FLOOR1      FLOOR2      FLOOR3      FLOOR4      FLOOR5
//0b11001110, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
//  ||||||||    ||||||||                                        ||||||||                ||||||||    ||||||||
//  ||||||||    ||||||||                                        ||||||||                ||||||||    |||||||└->0  \  these 3 bits are used to determine kind of sprite used for the floor
//  ||||||||    ||||||||                                        ||||||||                ||||||||    ||||||└-->1   | 0 = none; 1 = box; 2 = spike; 3 = piramide; 4 = pit; 5 = ; 6 =; 7= LEVEL UP
//  ||||||||    ||||||||                                        ||||||||                ||||||||    |||||└--->2  /
//  ||||||||    ||||||||                                        ||||||||                ||||||||    ||||└---->3  \
//  ||||||||    ||||||||                                        ||||||||                ||||||||    |||└----->4   | if the floor is 
//  ||||||||    ||||||||                                        ||||||||                ||||||||    ||└------>5   | these 5 bits are used to determine on what floor tile the element is
//  ||||||||    ||||||||                                        ||||||||                ||||||||    |└------->6   |
//  ||||||||    ||||||||                                        ||||||||                ||||||||    └-------->7  /  if all 8 bits == 0 => no floor element
//  ||||||||    ||||||||                                        ||||||||                ||||||||
//  ||||||||    ||||||||                                        ||||||||                |||||||└->0  \   these 3 bits are used to determine kind of sprite used for the object
//  ||||||||    ||||||||                                        ||||||||                ||||||└-->1   |  0 = black card; 1 = white card; 2 = battery; 3 = bullet; 4 = chip; 5 = teleport, 6 = switch OFF, 7 switch = ON,
//  ||||||||    ||||||||                                        ||||||||                |||||└--->2  /
//  ||||||||    ||||||||                                        ||||||||                ||||└---->3  \
//  ||||||||    ||||||||                                        ||||||||                |||└----->4   |
//  ||||||||    ||||||||                                        ||||||||                ||└------>5   |  these 5 bits are used to determine on what floor tile the object is
//  ||||||||    ||||||||                                        ||||||||                |└------->6   |
//  ||||||||    ||||||||                                        ||||||||                └-------->7  /   if all 8 bits == 0 => no object
//  ||||||||    ||||||||                                        |||||||| 
//  ||||||||    ||||||||                                        |||||||└->0  \   hese 3 bits are used to determine kind of sprite used for the enemy
//  ||||||||    ||||||||                                        ||||||└-->1   |  0 = ; 1 = ; 2 = ; 3 = ;
//  ||||||||    ||||||||                                        |||||└--->2  /
//  ||||||||    ||||||||                                        ||||└---->3  \
//  ||||||||    ||||||||                                        |||└----->4   |
//  ||||||||    ||||||||                                        ||└------>5   |  these 5 bits are used to determine on what floor tile the enemy is
//  ||||||||    ||||||||                                        |└------->6   |
//  ||||||||    ||||||||                                        └-------->7  /   if all 8 bits == 0 => no enemy
//  ||||||||    ||||||||
//  ||||||||    |||||||└->0  \  these 2 bits are used to determine what door you'll go to
//  ||||||||    ||||||└-->1  /
//  ||||||||    |||||└--->2  \
//  ||||||||    ||||└---->3   |
//  ||||||||    |||└----->4   | these 6 bits are used for the roomnumber you'll go to
//  ||||||||    ||└------>5   |
//  ||||||||    |└------->6   |
//  ||||||||    └-------->7  /
//  ||||||||
//  |||||||└->0  DOOR NORTH  is closed (0 = false / 1 = true)
//  ||||||└-->1  DOOR EAST   is closed (0 = false / 1 = true)
//  |||||└--->2  DOOR SOUTH  is closed (0 = false / 1 = true)
//  ||||└---->3  DOOR WEST   is closed (0 = false / 1 = true)
//  |||└----->4  DOOR NORTH  exists    (0 = false / 1 = true)
//  ||└------>5  DOOR EAST   exists    (0 = false / 1 = true)
//  |└------->6  DOOR SOUTH  exists    (0 = false / 1 = true)
//  └-------->7  DOOR WEST   exists    (0 = false / 1 = true)
//
//
//
// transporters data, the order of the data is by ascending numbers (ROOM X, ROOM Y, ROOM Z , ...)
// GOTO ROOM
//0b00000001,
//  |||||||└->0  \
//  ||||||└-->1   |
//  |||||└--->2   | these 6 bits are used for the roomnumber you'll go to
//  ||||└---->3   |
//  |||└----->4   |
//  ||└------>5  /
//  |└------->6 NOT USED
//  └-------->7 NOT USED
//
//
//
// data about the elements that get influenced: in what room, coming from what switch in which room
// ELEMENTS     OBJECT       WHAT
//  IN ROOM    AT  ROOM    ELEMENTS
//0b00000011, 0b00000000, 0b00011111,
//  ||||||||    ||||||||    ||||||||
//  ||||||||    ||||||||    |||||||└->0 FLOOR  5 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||    ||||||└-->1 FLOOR  4 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||    |||||└--->2 FLOOR  3 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||    ||||└---->3 FLOOR  2 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||    |||└----->4 FLOOR  1 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||    ||└------>5 OBJECT 3 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||    |└------->6 ENEMY  2 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||    └-------->7 ENEMY  1 INFLUENCED (0 = false / 1 = true)
//  ||||||||    ||||||||
//  ||||||||    |||||||└->0  \
//  ||||||||    ||||||└-->&   |
//  ||||||||    |||||└--->2   | these 6 bits are used for the roomnumber where the elements are influenced
//  ||||||||    ||||└---->3   |
//  ||||||||    |||└----->4   |
//  ||||||||    ||└------>5  /
//  ||||||||    |└------->6 NOT USED
//  ||||||||    └-------->7 NOT USED
//  ||||||||
//  |||||||└->0  \
//  ||||||└-->1   |
//  |||||└--->2   | these 6 bits are used for the roomnumber where the elements are influenced
//  ||||└---->3   |
//  |||└----->4   |
//  ||└------>5  /
//  |└------->6 NOT USED
//  └-------->7 NOT USED
//
//


const unsigned char PROGMEM level01[] =
{
  5,          // amount of rooms
  2,          // amount of transporters
  2,          // amount of rooms with influenceable objects

  // NEXT LEVEL DOOR
  0b0000000,  // data about the door and room that gets you to the next level

  // NEXT LEVEL ROOM
  0b0000001,  // data about which room gets you to the next level

  // ALL THE DATA FOR EACH ROOM AND EACH ROOM HAS 13 BYTES
  // DOORS         NORTH       EAST       SOUTH       WEST         ENEMY1      ENEMY2        OBJECT3       FLOOR1      FLOOR2      FLOOR3      FLOOR4      FLOOR5
  0b11111001,   0b00000110, 0b00001011, 0b00001100, 0b00010001,   0b00000000, 0b00000000,   0b00110101,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room0
  0b01000000,   0b00000000, 0b00000000, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b00000000,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room1
  0b10000000,   0b00000000, 0b00000000, 0b00000000, 0b00000001,   0b00000000, 0b00000000,   0b01100001,   0b00000001, 0b00001010, 0b00010011, 0b00011011, 0b00100011, // room2
  0b00010000,   0b00000010, 0b00000000, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100110,   0b00000100, 0b00101100, 0b01010100, 0b01111100, 0b10100100, // room3
  0b00100000,   0b00000000, 0b00000011, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100110,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room4  

  // transporters data, the order of the data is by ascending numbers (ROOM X, ROOM Y, ROOM Z , ...)
  // GOTO ROOM
  0b00000001,
  0b00000100,

  // data about the elements that get influenced
  // ELEMENTS    OBJECT       WHAT
  // IN ROOM    AT  ROOM    ELEMENTS
  0b00000011,  0b00000011, 0b00011111,
  0b00000010,  0b00000100, 0b00000111,
};

const unsigned char PROGMEM level02[] =
{
  5,          // amount of rooms
  2,          // amount of transporters
  1,          // amount of rooms with influenceable objects 
  0b0000000,  // data about the door and room that gets you to the next level
  0b0000010,  // data about which room gets you to the next level

  // DOORS         NORTH       EAST       SOUTH       WEST         ENEMY1      ENEMY2        OBJECT3       FLOOR1      FLOOR2      FLOOR3      FLOOR4      FLOOR5
  0b11111001,   0b00000110, 0b00001011, 0b00001100, 0b00010001,   0b00000000, 0b00000000,   0b00110101,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room0
  0b01000000,   0b00000000, 0b00000000, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100100,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room1
  0b11111111,   0b00000000, 0b00000000, 0b00000000, 0b00000001,   0b00000000, 0b00000000,   0b01100001,   0b00000001, 0b00001010, 0b00010011, 0b00011011, 0b00100011, // room2
  0b00010000,   0b00000010, 0b00000000, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100110,   0b00000100, 0b00101100, 0b01010100, 0b01111100, 0b10100100, // room3
  0b00100000,   0b00000000, 0b00000011, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100000,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room4  

  // transporters data, the order of the data is by ascending numbers (ROOM X, ROOM Y, ROOM Z , ...)
  // GOTO ROOM
  0b00000001,
  0b00000100,

  // data about the elements that get influenced
  // ELEMENTS   OBJECT       WHAT
  // IN ROOM    AT  ROOM   ELEMENTS
  0b00000011,  0b00000011, 0b00011111,
};


const unsigned char *levels[] =
{
  level01, level02,
};


#endif
