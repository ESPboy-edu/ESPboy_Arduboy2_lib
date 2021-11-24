#ifndef CRATE_H
#define CRATE_H

extern int scorePlayer;

PROGMEM const unsigned char crate_plus_mask[] = {
  // width, height
  8, 8,
  0x77, 0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0x75, 0xFF, 0x75, 0xFF,
  0x77, 0xFF, 0xFF, 0xFF, 0x77, 0xFF,
};


class Crate
{
  public:
    byte weapon;
    byte x, y;

    class Location {
      public:
        byte x, y;
    };
    Location location[11];

    Crate()
    {
      setupLocations();
      randomizeCrate();
    }

    void setupLocations() {
      location[0].x = 47; location[0].y = 7;
      location[1].x = 74; location[1].y = 7;
      location[2].x = 10; location[2].y = 23;
      location[3].x = 39; location[3].y = 23;
      location[4].x = 82; location[4].y = 23;
      location[5].x = 110; location[5].y = 23;
      location[6].x = 60; location[6].y = 39;
      location[7].x = 10; location[7].y = 55;
      location[8].x = 41; location[8].y = 55;
      location[9].x = 80; location[9].y = 55;
      location[10].x = 110; location[10].y = 55;
    }
    void randomizeCrate()
    {
      int currentWeapon = weapon;
      while (weapon == currentWeapon)
      {
        weapon = random(0, 6);
      }
      byte oldx = x;
      byte oldy = y;
      while (x == oldx && y == oldy)
      {
        byte pos = random(0, 8);
        x = location[pos].x; y = location[pos].y;
      }
    }
};

Crate crate;


void checkCrateCollisions()
{
  Rect crateRect;
  Rect playerRect;

  playerRect.x = player.x;
  playerRect.y = player.y;
  playerRect.width = 8;
  playerRect.height = 8;

  crateRect.x = crate.x;
  crateRect.y = crate.y;
  crateRect.width = 8;
  crateRect.height = 8;

  if (arduboy.collide(playerRect, crateRect)) {
    scorePlayer += 100;
    sound.tone(300, 100);
    switch (crate.weapon) {
      case 0:
        CURRENT_WEAPON = DEFAULT_GUN;
        weapon.weaponSet(0);
        break;
      case 1:
        CURRENT_WEAPON = SHOTGUN;
        weapon.weaponSet(1);
        break;
      case 2:
        CURRENT_WEAPON = MAGNUM;
        weapon.weaponSet(2);
        break;
      case 3:
        CURRENT_WEAPON = MACHINEGUN;
        weapon.weaponSet(3);
        break;
      case 4:
        CURRENT_WEAPON = FLAMETHROWER;
        weapon.weaponSet(4);
        break;
      case 5:
        CURRENT_WEAPON = DUAL_PISTOLS;
        weapon.weaponSet(5);
        break;
    }
    crate.randomizeCrate();
  }
}


void drawCrates()
{
  sprites.drawPlusMask(crate.x, crate.y, crate_plus_mask, 0);
}

#endif
