void roomOne()
{
  roomTileWidth = pgm_read_byte(&room1[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room1[roomRead]);
  roomRead++;
}

void roomTwo()
{
  roomTileWidth = pgm_read_byte(&room2[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room2[roomRead]);
  roomRead++;
}

void roomThree()
{
  roomTileWidth = pgm_read_byte(&room3[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room3[roomRead]);
  roomRead++;
}

void roomFour()
{
  roomTileWidth = pgm_read_byte(&room4[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room4[roomRead]);
  roomRead++;
}

void roomFive()
{
  roomTileWidth = pgm_read_byte(&room5[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room5[roomRead]);
  roomRead++;
}

void roomSix()
{
  roomTileWidth = pgm_read_byte(&room6[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room6[roomRead]);
  roomRead++;
}

void roomSeven()
{
  roomTileWidth = pgm_read_byte(&room7[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room7[roomRead]);
  roomRead++;
}

void roomEaight()
{
  roomTileWidth = pgm_read_byte(&room8[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room8[roomRead]);
  roomRead++;
}

void roomNine()
{
  roomTileWidth = pgm_read_byte(&room9[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room9[roomRead]);
  roomRead++;
}

void roomTen()
{
  roomTileWidth = pgm_read_byte(&room10[roomRead]);
  roomRead++;
  roomTileHeight = pgm_read_byte(&room10[roomRead]);
  roomRead++;
}

void fillRoom()
{
  int x;
  int y;
  int type;
  int c;

  enemyExist[0] = false;
  lookIconExist = false;
  doubleIconExist = false;
  swordIconExist = false;

  if (roomNumber == 1)
  {
    c = pgm_read_byte(&room1[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room1[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room1[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room1[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 2)
  {
    c = pgm_read_byte(&room2[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room2[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room2[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room2[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 3)
  {
    c = pgm_read_byte(&room3[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room3[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room3[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room3[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 4)
  {
    c = pgm_read_byte(&room4[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room4[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room4[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room4[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 5)
  {
    c = pgm_read_byte(&room5[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room5[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room5[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room5[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 6)
  {
    c = pgm_read_byte(&room6[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room6[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room6[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room6[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 7)
  {
    c = pgm_read_byte(&room7[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room7[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room7[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room7[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 8)
  {
    c = pgm_read_byte(&room8[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room8[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room8[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room8[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 9)
  {
    c = pgm_read_byte(&room9[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room9[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room9[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room9[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  if (roomNumber == 10)
  {
    c = pgm_read_byte(&room10[roomRead]); // number of additional tiles;
    roomRead++;

    for (int i = 0 ; i < c ; i ++)
    {
      x = pgm_read_byte(&room10[roomRead]);
      roomRead++;
      y = pgm_read_byte(&room10[roomRead]);
      roomRead++;
      type = pgm_read_byte(&room10[roomRead]);
      roomRead++;
      roomTiles[x][y] = type; // x, y, type
    }
  }

  int i = 0;
  int k = 0;

  for (y = 0; y < 24; y++) // Spawn Enemy // and spawn objects
  {
    for (x = 0; x < 24; x++)
    {
      if(roomTiles[x][y] == 3)
      {
        enemyExist[i] = 3;
        enemyX[i] = x * 160;
        enemyY[i] = y * 160;
        i++;
      }

      if(roomTiles[x][y] == 10)
      {
        droneExist[k] = true;
        droneX[k] = x * 160;
        droneY[k] = y * 160;
        k++;
      }

      if(roomTiles[x][y] == 11)
      {
        bossExist = 50;
        bossX = x * 160;
        bossY = y * 160;
        bossGoRight = true;
      }

      if((roomTiles[x][y] == 4)&&(!lookSkill))
      {
        lookIconExist = true;
        lookIconX = x * 16;
        lookIconY = y * 16;
      }

      if((roomTiles[x][y] == 5)&&(!doubleSkill))
      {
        doubleIconExist = true;
        doubleIconX = x * 16;
        doubleIconY = y * 16;
      }

      if((roomTiles[x][y] == 6)&&(!swordSkill))
      {
        swordIconExist = true;
        swordIconX = x * 16;
        swordIconY = y * 16;
      }

      if((roomTiles[x][y] == 1)&&(!gunSkill))
      {
        gunIconExist = true;
        gunIconX = x * 16;
        gunIconY = y * 16;
      }
    }
  }
}