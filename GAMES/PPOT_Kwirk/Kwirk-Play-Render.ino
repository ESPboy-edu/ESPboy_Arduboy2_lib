#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"

void render(uint8_t currentPlane, uint8_t suppressRightTiles) {


    // Calculate x rendering position ..

    Point_S8 map_Init = { 0, 0 };
    Point_S8 map = { 0, 0 };

    map.y = - 8;
    
    for (uint8_t y = 0; y < Constants::LevelMapHeight; y++) {

        Point_S8 player[4] = { 0, 0 };

        map.x = -8;
        Point_S8 mapWithView = { 0, 0 };
        mapWithView.x = map.x - level.getViewX();
        mapWithView.y = map.y - level.getViewY();

        uint8_t skipBackgroundImage = 0;
        uint8_t skipForegroundImage = 0;

        bool renderPlayer[Constants::PlayerCount] = { false, false, false, false };

        for (uint8_t x = 0; x < Constants::LevelMapWidth - suppressRightTiles; x++) {

            MapElement mapElement = level.getMapElement_ScreenCoords(x, y);
            MapElement mapElementNext = level.getMapElement_ScreenCoords((x + 1 < Constants::LevelMapWidth ? x + 1 : x), y);

            uint8_t itemIdx = level.getItemIdx_ScreenCoords(x, y);

            if (skipBackgroundImage > 0) {
                
                skipBackgroundImage--;

            }
            else {

                // Do not render the background block if an item has been found unless its a rotor at 45 degrees ..

                bool renderBlock = (itemIdx == Constants::ItemNotFound);

                if (itemIdx != Constants::ItemNotFound) {

                    Item &item = level.getItem(itemIdx & 0x7f);

                    switch (item.getItemType()) {

                        case ItemType::Rotator_Start ... ItemType::Rotator_End:

                            if (item.data.rotator.direction % 2 == 0) {

                                renderBlock = false;

                            }

                            break;

                        case ItemType::Stairs:

                            renderBlock = true;
                            break;

                        default:
                            
                            renderBlock = false;
                            break;

                    }

                }

                if (renderBlock) {
                    
                    if (mapElement != MapElement::Tile_Solid) {

                        switch (mapElement) {

                            case MapElement::Tile_FullRow:

                                #ifndef DEBUG_RENDER_TILES
                                SpritesU::drawOverwriteFX(mapWithView.x, mapWithView.y, Images::Tile_FullRow, currentPlane);
                                #endif

                                x = 128;
                                break;

                            case MapElement::Tile_HalfRow:

                                #ifndef DEBUG_RENDER_TILES
                                SpritesU::drawOverwriteFX(mapWithView.x, mapWithView.y, Images::Tile_HalfRow, currentPlane);
                                #endif

                                x = 128;
                                break;

                            case MapElement::Tile_FullWidth_Top:
                                {
                                    uint8_t c = 1;

                                    for (uint8_t i = x + 1; i < x + 5; i++) {
                                        
                                        if (level.getMapElement_ScreenCoords(i, y) == MapElement::Tile_FullWidth_Top) {
                                            c++;
                                        }
                                        else {
                                            break;
                                        }

                                    }

                                    skipBackgroundImage = c - 1;

                                    #ifndef DEBUG_RENDER_TILES

                                    if (c == 1) {

                                        SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Tile_04_01, currentPlane);

                                    }
                                    else {

                                        uint24_t imageIdx = FX::readIndexedUInt24(Images::Tile_04, c - 2);
                                        SpritesU::drawOverwriteFX(mapWithView.x, mapWithView.y, imageIdx, currentPlane);

                                    }
                                    #endif

                                }

                                break;

                            case MapElement::Tile_FullWidth:
                                {
                                    uint8_t c = 1;

                                    for (uint8_t i = x + 1; i < x + 5; i++) {
                                        
                                        if (level.getMapElement_ScreenCoords(i, y) == MapElement::Tile_FullWidth) {
                                            c++;
                                        }
                                        else {
                                            break;
                                        }

                                    }

                                    skipBackgroundImage = c - 1;

                                    #ifndef DEBUG_RENDER_TILES

                                    if (c == 1) {

                                        SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Tile_05_01, currentPlane);

                                    }
                                    else {

                                        uint24_t imageIdx = FX::readIndexedUInt24(Images::Tile_05, c - 2);
                                        SpritesU::drawOverwriteFX(mapWithView.x, mapWithView.y, imageIdx, currentPlane);

                                    }
                                    #endif

                                }

                                break;

                            case MapElement::Tile_UpperEdge:
                                {
                                    uint8_t c = 1;

                                    for (uint8_t i = x + 1; i < x + 5; i++) {
                                        
                                        if (level.getMapElement_ScreenCoords(i, y) == MapElement::Tile_UpperEdge) {
                                            c++;
                                        }
                                        else {
                                            break;
                                        }

                                    }

                                    skipBackgroundImage = c - 1;

                                    #ifndef DEBUG_RENDER_TILES

                                    if (c == 1) {

                                        SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Tile_10_01, currentPlane);

                                    }
                                    else {

                                        uint24_t imageIdx = FX::readIndexedUInt24(Images::Tile_10, c - 2);
                                        SpritesU::drawOverwriteFX(mapWithView.x, mapWithView.y, imageIdx, currentPlane);

                                    }
                                    #endif

                                }

                                break;

                            case MapElement::Tile_Overwrite_Start ... MapElement::Tile_RightSide:
                            case MapElement::Tile_LeftSide ... MapElement::Tile_Hole:

                                #ifndef DEBUG_RENDER_TILES
                                SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Tiles1, ((static_cast<uint8_t>(mapElement) - 1) * 3) + currentPlane);
                                #endif

                                break;

                            case MapElement::Tile_Masked_Start ... MapElement::Tile_Masked_End:

                                #ifndef DEBUG_RENDER_TILES
                                SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Tiles2, ((static_cast<uint8_t>(mapElement) - 11) * 3) + currentPlane);
                                #endif

                                break;

                        }

                    }

                }

            }



            if (skipForegroundImage > 0) {

                skipForegroundImage--;

            }
            else {

                if (itemIdx < Constants::ItemFound) {

                    Item &item = level.getItem(itemIdx);

                    switch (item.getItemType()) {

                        case ItemType::Rotator_One:
                            {
                                uint24_t imageIdx = FX::readIndexedUInt24(Images::Rotator_1, item.data.rotator.direction);

                                int8_t xPos = pgm_read_byte(Constants::Rotator_1_X + item.data.rotator.direction); 
                                int8_t yPos = pgm_read_byte(Constants::Rotator_1_Y + item.data.rotator.direction); 

                                switch (item.data.rotator.direction) {
            
                                    case 0:
                                    case 2:
                                    case 4:
                                    case 6:
                                        SpritesU::drawOverwriteFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);
                                        break;
            
                                    case 1:
                                    case 3:
                                    case 5:
                                    case 7:
                                        SpritesU::drawPlusMaskFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);
                                        break;

                                }

                            }

                            break;

                        case ItemType::Rotator_Two:
                            {
                                
                                bool drawTransparent = false;

                                switch (item.data.rotator.direction) {
            
                                    case 0:
                                    case 1:
                                    case 3:
                                    case 5:
                                    case 6:
                                    case 7:
                                        drawTransparent = true;

                                    default:            
                                        break;

                                }

                                int8_t xPos = pgm_read_byte(Constants::Rotator_2_X + item.data.rotator.direction); 
                                int8_t yPos = pgm_read_byte(Constants::Rotator_2_Y + item.data.rotator.direction); 

                                if (drawTransparent) {

                                    uint24_t imageIdx = FX::readIndexedUInt24(Images::Rotator_2_MK, item.data.rotator.direction);
                                    SpritesU::drawPlusMaskFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);

                                }
                                else {

                                    uint24_t imageIdx = FX::readIndexedUInt24(Images::Rotator_2_OW, item.data.rotator.direction);
                                    SpritesU::drawOverwriteFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);

                                }
            
                            }

                            break;

                        case ItemType::Rotator_Three:
                            {
                                
                                bool drawTransparent = false;

                                switch (item.data.rotator.direction) {
            
                                    case 0:
                                    case 1:
                                    case 2:
                                    case 3:
                                    case 5:
                                    case 6:
                                    case 7:
                                         drawTransparent = true;
                                         break;

                                    default:            
                                        break;

                                }

                                int8_t xPos = pgm_read_byte(Constants::Rotator_3_X + item.data.rotator.direction); 
                                int8_t yPos = pgm_read_byte(Constants::Rotator_3_Y + item.data.rotator.direction); 

                                if (drawTransparent) {

                                    uint24_t imageIdx = FX::readIndexedUInt24(Images::Rotator_3_MK, item.data.rotator.direction);
                                    SpritesU::drawPlusMaskFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);

                                }
                                else {

                                    uint24_t imageIdx = FX::readIndexedUInt24(Images::Rotator_3_OW, item.data.rotator.direction);
                                    SpritesU::drawOverwriteFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);

                                }
            
                            }

                            break;

                        case ItemType::Rotator_Four:
                            {
                                switch (item.data.rotator.direction) {
            
                                    case 0:
                                    case 2:
                                    case 4:
                                    case 6:
                                        SpritesU::drawPlusMaskFX(map.x - level.getViewX() - 8, map.y - level.getViewY() - 8, Images::Rotator_4_00_MK, currentPlane);
                                        break;
            
                                    case 1:
                                    case 3:
                                    case 5:
                                    case 7:
                                        SpritesU::drawPlusMaskFX(map.x - level.getViewX() - 8, map.y - level.getViewY() - 8, Images::Rotator_4_01_MK, currentPlane);
                                        break;

                                }

                            }

                            break;

                        case ItemType::Rotator_Five:
                            {
            
                                bool drawTransparent = false;

                                switch (item.data.rotator.direction) {
            
                                    case 1:
                                    case 3:
                                    case 5:
                                    case 7:
                                         drawTransparent = true;
                                         break;

                                    default:            
                                        break;

                                }

                                int8_t xPos = pgm_read_byte(Constants::Rotator_5_X + item.data.rotator.direction); 
                                int8_t yPos = pgm_read_byte(Constants::Rotator_5_Y + item.data.rotator.direction); 

                                if (drawTransparent) {

                                    uint24_t imageIdx = FX::readIndexedUInt24(Images::Rotator_5_MK, item.data.rotator.direction);
                                    SpritesU::drawPlusMaskFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);

                                }
                                else {

                                    uint24_t imageIdx = FX::readIndexedUInt24(Images::Rotator_5_OW, item.data.rotator.direction);
                                    SpritesU::drawOverwriteFX(map.x - level.getViewX() + xPos, map.y - level.getViewY() + yPos, imageIdx, currentPlane);

                                }

                            }

                            break;

                        case ItemType::Stairs:
                            {
                                SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Stairs, (((frameCount & 0x08) < 4) * 3) + currentPlane);
                            }

                            break;

                        case ItemType::Block_Start ... ItemType::Block_End:

                            renderBlock(item, x, y, mapWithView, currentPlane);
                            break;

                    }

                }

            }


            // // Player 0

            // if (x == level.getPlayer_X_ScreenCoords(0) && y == level.getPlayer_Y_ScreenCoords(0)) {

            //     int8_t moving_X = level.getPlayer(0).getMoving_X() % 8;
            //     int8_t moving_Y = level.getPlayer(0).getMoving_Y() % 8;

            //     switch (moving_X) {

            //         case -8 ... -1:
            //             moving_X = -8 - moving_X;
            //             break;

            //         case 1 ... 8:
            //             moving_X = 8 - moving_X;
            //             break;
                        
            //     }

            //     switch (moving_Y) {

            //         case -8 ... -1:
            //             moving_Y = -8 - moving_Y;
            //             break;

            //         case 1 ... 8:
            //             moving_Y = 8 - moving_Y;
            //             break;
                        
            //     }

            //     player[0].x = static_cast<uint8_t>(map.x - level.getViewX(level.getActivePlayer() != 0) + moving_X);
            //     player[0].y = static_cast<uint8_t>(map.y - level.getViewY(level.getActivePlayer() != 0) + moving_Y);

            // }


            // Player coords ..

            for (uint8_t playerIdx = 0; playerIdx < Constants::PlayerCount; playerIdx++) {
                    
                if (x == level.getPlayer_X_ScreenCoords(playerIdx) && y == level.getPlayer_Y_ScreenCoords(playerIdx)) {

                    renderPlayer[playerIdx] = true;
                    int8_t moving_X = level.getPlayer(playerIdx).getMoving_X();
                    int8_t moving_Y = level.getPlayer(playerIdx).getMoving_Y();
                        
                    switch (moving_X) {

                        case -8 ... -1:

                            moving_X = -8 - moving_X;
                            break;

                        case 1 ... 8:

                            moving_X = 8 - moving_X;
                            break;
                            
                    }

                    switch (moving_Y) {

                        case -8 ... -1:

                            moving_Y = -8 - moving_Y;
                            break;

                        case 1 ... 8:

                            moving_Y = 8 - moving_Y;
                            break;
                            
                    }

                    player[playerIdx].x = static_cast<uint8_t>(map.x - level.getViewX(level.getActivePlayer() != playerIdx) + moving_X);
                    player[playerIdx].y = static_cast<uint8_t>(map.y - level.getViewY(level.getActivePlayer() != playerIdx) + moving_Y);

                }

            }

            map.x = map.x + Constants::MapTileWidth;
            mapWithView.x = mapWithView.x + Constants::MapTileWidth;

        }



        // Render player?

        bool normal = level.getGameState() == GameState::Play_BouncingHead && (frameCount % 16 < 8);

        for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

            if (renderPlayer[i]) {

                uint8_t direction = static_cast<uint8_t>(level.getPlayer(i).getDirection());
                
                if (level.getActivePlayer() == i && level.getGameState() == GameState::Play_BouncingHead) {

                    if (normal) {

                        SpritesU::drawPlusMask(player[i].x, player[i].y - 1, Images::Player, (((i * 16) + direction) * 3) + currentPlane);

                    }
                    else {

                        SpritesU::drawPlusMask(player[i].x, player[i].y - 1 - 2, Images::Player, (((i * 16) + direction + 8) * 3) + currentPlane);

                    }

                }
                else {

                    bool bounce = (level.getActivePlayer() == i && (frameCount & 0x08) != 0);
                    SpritesU::drawPlusMask(player[i].x, player[i].y - 1, Images::Player, (((i * 16) + direction + bounce) * 3) + currentPlane);

                }

            }

        }

        map.y = map.y + Constants::MapTileHeight;

    }

}

void renderBlock(Item &item, uint8_t x, uint8_t y, Point_S8 &mapWithView, uint8_t currentPlane) {

    uint8_t blockIdx = static_cast<uint8_t>(item.getItemType()) - static_cast<uint8_t>(ItemType::Block_Start);
    uint8_t height = pgm_read_byte(Constants::Block_Heights + blockIdx);
    uint8_t width = pgm_read_byte(Constants::Block_Widths + blockIdx);
    uint8_t fallen = item.data.block.fallen;
    uint8_t x_WorldCoords = x + level.getMapX(); 
    uint8_t y_WorldCoords = y + level.getMapY(); 

    if (fallen != Constants::FallenCountMax) {
            
        if (width == 1 & height == 1) {

            SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_01_01, (3 * fallen) + currentPlane);

        }
        else {
            
            if (item.getY() == y_WorldCoords) { // Top Row

                if (width == 1) {

                    SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_T1, (3 * fallen) + currentPlane);

                }
                else {

                    if (item.getX() == x_WorldCoords) {

                        if (height > 1) {

                            SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_TL_2, (3 * fallen) + currentPlane);

                        }
                        else {

                            SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_TL, (3 * fallen) + currentPlane);

                        }

                    }
                    else {

                        if (item.getX() + width - 1 > x_WorldCoords) {

                            if (height > 1) {

                                SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_TM, (3 * fallen) + currentPlane);

                            }
                            else {

                                SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_MH, (3 * fallen) + currentPlane);

                            }

                        }
                        else {

                            if (height > 1) {

                                SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_TR_2, (3 * fallen) + currentPlane);

                            }
                            else {

                                SpritesU::drawPlusMask(mapWithView.x, mapWithView.y, Images::Block_TR, (3 * fallen) + currentPlane);

                            }

                        }

                    }
                    
                }

            }
            else {

                if (item.getY() + height - 1 > y_WorldCoords) { // Middle row

                    if (width == 1) {

                        SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_MV, currentPlane);
                        
                    }
                    else {

                        if (item.getX() == x_WorldCoords) {

                            SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_ML, currentPlane);

                        }
                        else {

                            if (item.getX() + width - 1 == x_WorldCoords) { // Right hand side

                                SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_MR, currentPlane);

                            }
                            else {

                                SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_M, currentPlane);

                            }

                        }

                    }

                }
                else { // Bottom Row

                    if (width == 1) {

                        SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_B1, (3 * fallen) + currentPlane);
                        
                    }
                    else {

                        if (item.getX() == x_WorldCoords) {

                            SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_BL, (3 * fallen) + currentPlane);

                        }
                        else {

                            if (item.getX() + width - 1 == x_WorldCoords) { // Right hand side

                                SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_BR, (3 * fallen) + currentPlane);

                            }
                            else {

                                SpritesU::drawOverwrite(mapWithView.x, mapWithView.y, Images::Block_BM, (3 * fallen) + currentPlane);

                            }

                        }

                    }

                }

            }

        }

    }

}