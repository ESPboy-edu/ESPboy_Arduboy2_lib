#include "src/Arduboy2Ext.h"
#include <Sprites.h>

void handleMenu_MoveUp() {

    if (player.getInventoryCount() == 0) {
        menu.y = 0;
        menu.top = 0;
    }
    else if (menu.y == 0 && menu.top == 0) {
        // do nothing
    }
    else if (player.getInventoryCount() == menu.y + 1) {
        menu.y--;
    }
    else if (menu.top > 0) {
        menu.top--;
        menu.y--;
    }
    else if (menu.y >= 1 && menu.top == 0) {
        menu.y--;
    }
    else if (menu.y ==  player.getInventoryCount() - 1) {
        menu.y--;
    }
    else if (menu.y <  player.getInventoryCount() - 1) {
        menu.y--;
        menu.top--;
    }
    
}

void handleMenu_MoveDown() {

    if (player.getInventoryCount() == 0) {
        menu.y = 0;
        menu.top = 0;
    }
    else if (menu.y == 0 && player.getInventoryCount() > 1) {
        menu.y++;
    }
    else if (menu.y <  player.getInventoryCount() - 2) {
        menu.y++;
        menu.top++;
    }
    else if (menu.y <  player.getInventoryCount() - 1) {
        menu.y++;
    }

}

void handleMenu() {

    uint8_t justPressed = arduboy.justPressedButtons();

    updateCamera();
    updateEnemys(level);
    checkCollisions(level);

    // drawMaze(level);
    // drawChests(level);
    // drawEnemies(level);
    // drawItems(level);
    // drawPlayer();
    // drawDeath();
    // drawFlashlight();    
    drawStuff();
    drawChestCount(106 - menu.x, 0);
    drawMenu();


    if (justPressed & UP_BUTTON) {

        handleMenu_MoveUp();

    }

    if (justPressed & DOWN_BUTTON) {

        handleMenu_MoveDown();

    }

    if ((justPressed & A_BUTTON) && menu.x == 128 - 22) {

        uint8_t playerX = player.x / Constants::TileSize;
        uint8_t playerY = player.y / Constants::TileSize;

        switch (player.getInventoryItem(menu.y)) {
        
            case ItemType::Map:
                
                switch (player.y) {
                
                    case 0 ... 63:
                        camera_Small.y = 0;
                        break;

                    case 64 ... 126:
                        camera_Small.y = (player.y - 63) / 5;
                        break;

                    default:
                        camera_Small.y = 26;
                        break;

                }
                
                mapLevel = level;
                gameState = GameState::Map;
                break;
        
            case ItemType::Bomb:
                {
                    uint8_t idx = maze.getEmptyItem();


                    Item &item = maze.getItem(idx);
                    item.level = level;
                    item.x = player.x;
                    item.y = player.y;
                    item.itemType = ItemType::Bomb_Active;
                    item.data = (13 * 6) - 1;
                    player.removeItem(menu.y);
                    menu.direction = MenuDirection::Closing;
                    handleMenu_MoveUp();
                }

                break;

            case ItemType::Gun:
                
                player.setHoldingGun(true);
                menu.direction = MenuDirection::Closing;
                handleMenu_MoveUp();
                break;

            case ItemType::Candle:
                
                if (darkMode) {
                    darkMode = false;
                    player.removeItem(menu.y);
                    player.setCandleCount(7);
                    menu.direction = MenuDirection::Closing;
                    handleMenu_MoveUp();
                }
                break;

            case ItemType::Key:
                
                if (maze.getCell(level, playerX - 1, playerY) == CellTypes::GateClosed) {

                    maze.setCell(level, playerX - 1, playerY, CellTypes::GateOpen);
                    player.removeItem(menu.y);
                    menu.direction = MenuDirection::Closing;
                    handleMenu_MoveUp();

                }
                else if (maze.getCell(level, playerX + 1, playerY) == CellTypes::GateClosed) {

                    maze.setCell(level, playerX + 1, playerY, CellTypes::GateOpen);
                    player.removeItem(menu.y);
                    menu.direction = MenuDirection::Closing;
                    handleMenu_MoveUp();

                }
                else if (maze.getCell(level, playerX, playerY - 1) == CellTypes::GateClosed) {

                    maze.setCell(level, playerX, playerY - 1, CellTypes::GateOpen);
                    player.removeItem(menu.y);
                    menu.direction = MenuDirection::Closing;
                    handleMenu_MoveUp();

                }
                else if (maze.getCell(level, playerX, playerY + 1) == CellTypes::GateClosed) {

                    maze.setCell(level, playerX, playerY + 1, CellTypes::GateOpen);
                    player.removeItem(menu.y);
                    menu.direction = MenuDirection::Closing;
                    handleMenu_MoveUp();

                }

                break;

        }

    }

    if (justPressed & B_BUTTON) {

        menu.direction = MenuDirection::Closing;
        
    }
    
}

void handleMenu_ShowMap() {

    uint8_t pressed = arduboy.pressedButtons();
    uint8_t justPressed = arduboy.justPressedButtons();

    updateEnemys(level);
    checkCollisions(level);
    drawMaze_Small(mapLevel);
    drawChests_Small(mapLevel);
    drawEnemies_Small(mapLevel);
    drawPlayer_Small();

    Sprites::drawOverwrite(92, 2, Images::HUD_Maze, mapLevel);
    drawChestCount(102, 16);

    if (pressed & LEFT_BUTTON) {
        mapLevel = 0;
    }
    if (pressed & RIGHT_BUTTON) {
        mapLevel = 1;
    }

    if ((pressed & UP_BUTTON) && camera_Small.y > 0) {
        camera_Small.y--;
    }

    if ((pressed & DOWN_BUTTON) && camera_Small.y < 26) {
        camera_Small.y++;
    }

    if (justPressed & B_BUTTON) {
        gameState = GameState::ShowMenu;
        menu.direction = MenuDirection::Closing;
    }

}

void drawMenu() {

    arduboy.fillRect(menu.x, 0, 30, 64, BLACK);
    arduboy.drawFastVLine(menu.x + 1, 0, 64, WHITE);


    if (menu.top > 0) {
    
        Sprites::drawSelfMasked(menu.x + 8, 1, Images::MenuArrows, 0);

    }
    else {

        Sprites::drawSelfMasked(menu.x + 8, 1, Images::MenuArrows, 1);
    
    }

    uint8_t y = 10;

    for (uint8_t i = menu.top; i < menu.top + 3; i++) {
        
        if (i < player.getInventoryCount()) {

            if (player.getInventoryItem(i) != ItemType::Gun) {

                Sprites::drawSelfMasked(menu.x + 6, y, Images::MenuItems, static_cast<uint8_t>(player.getInventoryItem(i)));

            }
            else {

                Sprites::drawSelfMasked(menu.x + 6, y, Images::MenuItems_Gun, player.getBulletCount());

            }

        }
        y = y + 16;

    }

    if (player.getInventoryCount() > 0 && arduboy.frameCount % 24 < 12) {
        Sprites::drawSelfMasked(menu.x + 4, 8 + ((menu.y - menu.top) * 16), Images::MenuCursor, 0);
    }

    if (player.getInventoryCount() < 3 || player.getInventoryCount() == menu.top + 3) {
        Sprites::drawSelfMasked(menu.x + 8, 55, Images::MenuArrows, 3);
    }
    else {
        Sprites::drawSelfMasked(menu.x + 8, 55, Images::MenuArrows, 2);
    }

    if (menu.direction == MenuDirection::Openning && menu.x > 128 - 22) {
    
        menu.x = menu.x - 2;

        if (menu.x == 128 - 22) {
        
            menu.direction = MenuDirection::None;

        }

    }

    if (menu.direction == MenuDirection::Closing) {
    
        if (menu.x < 128) {
    
            menu.x = menu.x + 2;

            if (menu.x == 128) {
            
                menu.direction = MenuDirection::None;
                gameState = GameState::GamePlay;
                    
            }

        }

    }

}

void drawChestCount(int8_t x, uint8_t y) {

    if (darkMode && gameState != GameState::Map) {

    }
    else {

        arduboy.fillRect(x - 1, y - 1, 17, 14, BLACK);
        Sprites::drawExternalMask(x, y, Images::Score, Images::Score_Mask, 10 - maze.getActiveChests(), 0);

    }
    
}