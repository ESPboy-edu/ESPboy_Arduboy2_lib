
void getRoomDimensions(uint8_t roomIdx) {

  uint8_t * levelDataStart = rooms[roomIdx - 1];
  roomTileWidth = pgm_read_byte(levelDataStart + roomRead );
  roomRead++;
  roomTileHeight = pgm_read_byte(levelDataStart + roomRead );
  roomRead++;
  
}


void fillRoom(uint8_t roomIdx)
{
  int x;
  int y;
  int type;
  int c;

  enemyExist[0] = false;
  lookIconExist = false;
  doubleIconExist = false;
  swordIconExist = false;

  uint8_t *levelDataStart = rooms[roomIdx - 1];
  c = pgm_read_byte(levelDataStart + roomRead); // number of additional tiles;
  roomRead++;

  for (int i = 0 ; i < c ; i ++)
  {
    x = pgm_read_byte(levelDataStart + roomRead);
    roomRead++;
    y = pgm_read_byte(levelDataStart + roomRead);
    roomRead++;
    type = pgm_read_byte(levelDataStart + roomRead);
    roomRead++;
    roomTiles[x][y] = type; // x, y, type
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
