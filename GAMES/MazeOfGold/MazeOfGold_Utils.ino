#include "src/Arduboy2Ext.h"
#include <Sprites.h>


void updateEnemys(uint8_t level) {

    if (arduboy.frameCount % 2 == 1) { return; }

    for (uint8_t i = 0; i < Constants::MaxEnemys; i++) {

        Enemy &enemy = maze.getEnemy(i);

        if (!enemy.isActive()) continue;

        if ((enemy.x % Constants::TileSize == 0) && (enemy.y % Constants::TileSize == 0)) {

            int16_t testX = enemy.x + (Constants::DirectionOffsetX[enemy.dir] * Constants::TileSize);
            int16_t testY = enemy.y + (Constants::DirectionOffsetY[enemy.dir] * Constants::TileSize);

            bool forwardClear = (maze.getCell(enemy.level, testX / Constants::TileSize, testY / Constants::TileSize) == CellTypes::Empty);
            bool wantTurn = (random(100) < 20);

            if (!forwardClear || wantTurn) {

                uint8_t dirs[2] = {(enemy.dir + 1) % 4, (enemy.dir + 3) % 4};

                if (random(2) == 0) {
                    uint8_t temp = dirs[0];
                    dirs[0] = dirs[1];
                    dirs[1] = temp;
                }

                bool moved = false;

                for (uint8_t d = 0; d < 2; d++) {

                    uint8_t newDir = dirs[d];
                    uint16_t newX = enemy.x + (Constants::DirectionOffsetX[newDir] * Constants::TileSize);
                    uint16_t newY = enemy.y + (Constants::DirectionOffsetY[newDir] * Constants::TileSize);

                    if (maze.getCell(enemy.level, newX / Constants::TileSize, newY / Constants::TileSize) == CellTypes::Empty) {

                        if (!(newX == enemy.lastX && newY == enemy.lastY)) {

                            enemy.lastX = enemy.x;
                            enemy.lastY = enemy.y;
                            enemy.dir = newDir;

                            switch (enemy.dir) {
                            
                                case 0:
                                case 2:
                                    enemy.y += Constants::DirectionOffsetY[enemy.dir] * 2;
                                    break;

                                default:
                                    enemy.x += Constants::DirectionOffsetX[enemy.dir] * 2;
                                    break;

                            }

                            moved = true;
                            break;

                        }
                    }
                }

                if (!moved) {

                    enemy.lastX = enemy.x;
                    enemy.lastY = enemy.y;
                    enemy.x += Constants::DirectionOffsetX[enemy.dir] * 2;
                    enemy.y += Constants::DirectionOffsetY[enemy.dir] * 2;

                    if (!forwardClear) {

                        enemy.dir = (enemy.dir + 2) % 4;
    
                    }

                }

            } 
            else {

                enemy.lastX = enemy.x;
                enemy.lastY = enemy.y;
                enemy.x += Constants::DirectionOffsetX[enemy.dir] * 2;
                enemy.y += Constants::DirectionOffsetY[enemy.dir] * 2;

            }

        } 
        else {

            // Continue movement ..

            uint16_t forwardX = enemy.x + (Constants::DirectionOffsetX[enemy.dir] * 2);
            uint16_t forwardY = enemy.y + (Constants::DirectionOffsetY[enemy.dir] * 2);

            enemy.lastX = enemy.x;
            enemy.lastY = enemy.y;
            enemy.x = forwardX;
            enemy.y = forwardY;

        }

    }

}

void checkCollisions(uint8_t level) {

    Rect playerRect;
    playerRect.x = player.x + 1;
    playerRect.y = player.y;
    playerRect.width = 10;
    playerRect.height = 12;

    for (uint8_t i = 0; i < Constants::MaxChests; i++) {

        Item &chest = maze.getChest(i);
        Rect chestRect;

        if (chest.level != level) continue;

        chestRect.x = chest.x * Constants::TileSize;
        chestRect.y = chest.y * Constants::TileSize;
        chestRect.width = 12;
        chestRect.height = 12;

        if (arduboy.collide(chestRect, playerRect) && chest.isActive()) { 

            chest.x = 0;
            chest.y = 0;

            maze.setActiveChests(maze.getActiveChests() - 1);

            if (maze.getActiveChests() == 0) {
                gameState = GameState::LevelUp;
            }
            else {
            
                displayChestsCounter = 12;

            }
        
        }

    }


    for (uint8_t i = 0; i < Constants::MaxEnemys; i++) {

        Enemy &enemy = maze.getEnemy(i);
        Rect enemyRect;

        if (enemy.level != level || !enemy.isActive()) continue;

        enemyRect.x = enemy.x + 1;
        enemyRect.y = enemy.y;
        enemyRect.width = 10;
        enemyRect.height = 12;

        if (arduboy.collide(enemyRect, playerRect) && !player.isDead()) { 

            death.x = player.x;
            death.y = player.y;
            death.itemType = ItemType::Death_Player;
            death.data = 0;            
            player.setDead(true);

        }

        Point bulletPoint;
        bulletPoint.x = bullet.x;
        bulletPoint.y = bullet.y;

        if (arduboy.collide(bulletPoint, enemyRect)) { 

            death.x = enemy.x;
            death.y = enemy.y;
            death.itemType = ItemType::Death_Enemy;
            death.data = 0;

            enemy.x = 0;
            enemy.y = 0;
            bullet.reset();

        }

    }

    for (uint8_t i = 0; i < Constants::MaxItems; i++) {

        Item &item = maze.getItem(i);
        Rect itemRect;

        if (item.level != level || !item.isActive()) continue;

        itemRect.x = item.x + 1;
        itemRect.y = item.y;
        itemRect.width = 10;
        itemRect.height = 12;
        
        switch (item.itemType) {

            case ItemType::Bomb:
            case ItemType::Key:
            case ItemType::Candle:

                if (arduboy.collide(itemRect, playerRect)) { 

                    if (player.addItem(item.itemType)) {

                        item.reset();

                    }
                    
                }

                break;

            case ItemType::Bullets:

                if (arduboy.collide(itemRect, playerRect)) { 

                    if (player.hasItem(ItemType::Gun) && player.getBulletCount() == 0) {

                        player.setBulletCount(player.getBulletCount() + 2);
                        item.reset();
                        
                    }
                    else {

                        if (player.addItem(item.itemType)) {

                            item.reset();

                        }
                        
                    }
                    
                }

                break;

            case ItemType::Gun:

                if (arduboy.collide(itemRect, playerRect)) { 

                    if (player.addItem(item.itemType)) {

                        item.reset();
                        player.setBulletCount(player.getBulletCount() + 2);

                    }
                    
                }

                break;

        }

    }


    // Puff .. caused by bomb.

    if (puff.isActive() && puff.itemType == ItemType::Puff_Bomb) {

        Rect puffRect;
        puffRect.x = puff.x + 1;
        puffRect.y = puff.y;
        puffRect.width = 12;
        puffRect.height = 12;

        if (arduboy.collide(puffRect, playerRect) && !player.isDead()) { 

            death.x = player.x;
            death.y = player.y;
            death.itemType = ItemType::Death_Player;
            death.data = 0;            
            player.setDead(true);

        }
        else {
            
            for (uint8_t i = 0; i < Constants::MaxEnemys; i++) {

                Enemy &enemy = maze.getEnemy(i);
                Rect enemyRect;

                if (enemy.level != level || !enemy.isActive()) continue;

                enemyRect.x = enemy.x + 1;
                enemyRect.y = enemy.y;
                enemyRect.width = 10;
                enemyRect.height = 12;

                if (arduboy.collide(enemyRect, puffRect)) { 

                    death.x = enemy.x;
                    death.y = enemy.y;
                    death.itemType = ItemType::Death_Enemy;
                    death.data = 0;

                    enemy.x = 0;
                    enemy.y = 0;

                }

            }

        }

    }

}


void updatePlayer() {

    uint8_t justPressed = arduboy.justPressedButtons();
	uint8_t tileX = player.x / Constants::TileSize;
	uint8_t tileY = player.y / Constants::TileSize;

    if (player.isDead()) return;

    if (player.isHoldingGun()) {

        if (justPressed & LEFT_BUTTON) {

            if (player.dir != 3) {
                player.dir = 3;
            }
            else {
                player.setHoldingGun(false);
            }

            return;

        } 
        else if (justPressed & RIGHT_BUTTON) {

            if (player.dir != 1) {
                player.dir = 1;
            }
            else {
                player.setHoldingGun(false);
            }

            return;

        } 
        else if (justPressed & UP_BUTTON) {
        
            if (player.dir != 0) {
                player.dir = 0;
            }
            else {
                player.setHoldingGun(false);
            }

            return;

        } 
        else if ((justPressed & DOWN_BUTTON)) {
        
            if (player.dir != 2) {
                player.dir = 2;
            }
            else {
                player.setHoldingGun(false);
            }

            return;

        }

        if (justPressed & B_BUTTON) {

            arduboy.previousButtonState = 0;
            arduboy.currentButtonState = 0;
            player.setHoldingGun(false);
            return;
        
        }
        
        if ((justPressed & A_BUTTON) && !bullet.isActive()) {
        
            if (player.getBulletCount() > 0) {
                
                player.setBulletCount(player.getBulletCount() - 1);
                bullet.itemType = ItemType::Bullet_Normal;

                switch (player.dir) {
                
                    case 0:
                        bullet.x = player.x + 5;
                        bullet.y = player.y + 4;
                        bullet.data = player.dir;
                        break;
                
                    case 1:
                        bullet.x = player.x + 12;
                        bullet.y = player.y + 7;
                        bullet.data = player.dir;
                        break;
                
                    case 2:
                        bullet.x = player.x + 5;
                        bullet.y = player.y + 7;
                        bullet.data = player.dir;
                        break;
                
                    case 3:
                        bullet.x = player.x - 2;
                        bullet.y = player.y + 7;
                        bullet.data = player.dir;
                        break;
                        
                }
                
            }
            else {

                bullet.itemType = ItemType::Bullet_None;
                bullet.x = player.x - 2;
                bullet.y = player.y + 4;
                bullet.data = 6;

            }


        }

    }

    else if (player.vx == 0 && player.vy == 0 && player.x % Constants::TileSize == 0 && player.y % Constants::TileSize == 0) {

        uint8_t pressed = arduboy.pressedButtons();

        if ((pressed & LEFT_BUTTON) && maze.isWalkable(level, tileX - 1, tileY)) {
            player.vx = -2;
            player.dir = 3;
        } 
        else if ((pressed & RIGHT_BUTTON) && maze.isWalkable(level, tileX + 1, tileY)) {
            player.vx = 2;
            player.dir = 1;
        } 
        else if ((pressed & UP_BUTTON) && maze.isWalkable(level, tileX, tileY - 1)) {
            player.vy = -2;
            player.dir = 0;
        } 
        else if ((pressed & DOWN_BUTTON) && maze.isWalkable(level, tileX, tileY + 1)) {
            player.vy = 2;
            player.dir = 2;
        }
        else if (pressed & B_BUTTON) {

            menu.direction = MenuDirection::Openning;
            menu.x = 128;
            gameState = GameState::ShowMenu;

        }
        else if ((pressed & A_BUTTON) && arduboy.frameCount > 32) {

            if (player.getItemIdx(ItemType::Gun) != Constants::NoItem) {
            
                player.setHoldingGun(true);

            }

        }

        if (player.vx != 0 || player.vy != 0) {

            player.setHoldingGun(false);

        }

    }

    player.x += player.vx;
    player.y += player.vy;

    // Stop when aligned with grid ..

    bool stop = false;

    if (player.vx != 0 && player.x % Constants::TileSize == 0) { 
        player.vx = 0;
        stop = true;
    }

    if (player.vy != 0 && player.y % Constants::TileSize == 0) {
        player.vy = 0;
        stop = true;
    }

    if (stop) {

        tileX = player.x / Constants::TileSize;
        tileY = player.y / Constants::TileSize;

        if (maze.getCell(level, tileX, tileY) == CellTypes::Stairs) {

            puff.x = player.x;
            puff.y = player.y;
            puff.data = 2 * 6;
            puff.itemType = ItemType::Puff_Stairs;
            level = !level;
        }

    }

}

void updateCamera() {

	camera.x = player.x  - (WIDTH / 2) + (Constants::TileSize / 2);
	camera.y = player.y  - (HEIGHT / 2) + (Constants::TileSize / 2);

	if (camera.x < 0) camera.x = 0;
	if (camera.y < 0) camera.y = 0;

	int16_t maxCameraX = Constants::MazeWidth * Constants::TileSize - WIDTH;
	int16_t maxCameraY = Constants::MazeHeight * Constants::TileSize - HEIGHT;

	if (camera.x > maxCameraX) camera.x = maxCameraX;
	if (camera.y > maxCameraY) camera.y = maxCameraY;

}
