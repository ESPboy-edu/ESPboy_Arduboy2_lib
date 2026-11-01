const unsigned char PROGMEM level01[] =
{
  5,          // amount of rooms
  2,          // amount of transporters
  3,          // amount of influenced objects 
  0b0000000, // data about the door and room that gets you to the next level

  // DOORS         NORTH       EAST       SOUTH       WEST         ENEMY1      ENEMY2        OBJECT3       FLOOR1      FLOOR2      FLOOR3      FLOOR4      FLOOR5
  0b11111001,   0b00000110, 0b00001011, 0b00001100, 0b00010001,   0b00000000, 0b00000000,   0b00110101,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room0
  0b01000000,   0b00000000, 0b00000000, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100100,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room1
  0b10000000,   0b00000000, 0b00000000, 0b00000000, 0b00000001,   0b00000000, 0b00000000,   0b01100001,   0b00000001, 0b00001010, 0b00010011, 0b00011011, 0b00100011, // room2
  0b00010000,   0b00000010, 0b00000000, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100101,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room3
  0b00100000,   0b00000000, 0b00000011, 0b00000000, 0b00000000,   0b00000000, 0b00000000,   0b01100000,   0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // room4  

  // transporters data, the order of the data is by ascending numbers
  // ROOM
  0b00000001,
  0b00000100,

  // data about the elements that get influenced
  // ROOM         ELEMENT
  0b00000001,  0b00000001,
  0b00000011,  0b00000011,
  0b00000100,  0b00011111,
};