
        MapElement getMapElement_RelativeToPlayer(int8_t offsetX, int8_t offsetY) {

            int16_t x = this->getPlayer().getX_WorldCoords() - this->map.x + offsetX;
            int16_t y = this->getPlayer().getY_WorldCoords() - this->map.y + offsetY;

            return mapElements[y][x];

        }


        MapElement getMapElement_WorldCoords(int8_t x, int8_t y) {

            if (x - this->getMapX() < 0 || x - this->getMapX() >= Constants::LevelMapWidth ||
                y - this->getMapY() < 0 || y - this->getMapY() >= Constants::LevelMapHeight) {
                
                    uint24_t startPos = FX::readIndexedUInt24(Levels, this->levelNumber);
                    startPos = startPos + Constants::LevelLeadingMemOffset + (y * this->width) + x;

                    FX::seekData(startPos);
                    uint8_t item = FX::readPendingUInt8();
                    FX::readEnd();

                    return static_cast<MapElement>(item);

            }
            else {

                return mapElements[y - this->getMapY()][x - this->getMapX()];

            }

        }


        MapElement getMapElement_ScreenCoords(int8_t x, int8_t y) {

            return mapElements[y][x];

        }


        bool canBlockFall(Item &item) {

            uint8_t blockIdx = static_cast<uint8_t>(item.getItemType()) - static_cast<uint8_t>(ItemType::Block_Start);
            uint8_t height = pgm_read_byte(Constants::Block_Heights + blockIdx);
            uint8_t width = pgm_read_byte(Constants::Block_Widths + blockIdx);
            
            for (uint8_t y = 0; y < height; y++) {

                for (uint8_t x = 0; x < width; x++) {

                    MapElement mapElement = this->getMapElement_WorldCoords(item.getX() + x, item.getY() + y);

                    if (!this->isEmptyForFall_WorldCoords(x, y, mapElement)) {

                        return false;

                    }
                    
                }

            }

            return true;

        }


        uint8_t loadMap_MapY(uint8_t player_Y_WorldCoords) {

            if (this->getAllowScrollY()) {

                if (player_Y_WorldCoords < 4) {
                    return 0;
                }
                else if (player_Y_WorldCoords >= 4 && player_Y_WorldCoords < this->height - 5) {
                    return player_Y_WorldCoords - 4;
                }
                else {
                    return this->height - 9;
                }

            }
            else {

                return 0;

            }

        }


        uint8_t loadMap_MapX(uint8_t player_X_WorldCoords) {

            if (this->getAllowScrollX()) {

                if (player_X_WorldCoords < 7) {
                    return 0;
                }
                else if (player_X_WorldCoords >= 7 && player_X_WorldCoords <= this->width - 11) {
                    return player_X_WorldCoords - 7;
                }
                else {
                    return this->width - 17;
                }

            }
            else {

                return 0;

            }

        }


        int8_t getViewX(bool incOffset = true) { 

            if (incOffset) {

                switch (this->moving.x) {

                    case 0:
                        return this->view.x;

                    case -8 ... -1:
                        return this->view.x - (-8 - this->moving.x);

                    case 1 ... 8:
                        return this->view.x - (8 - this->moving.x);

                }

            }
            else {

               return this->view.x;

            }
        return 0;
        }

        int8_t getViewY(bool incOffset = true) { 

            if (incOffset) {
        
                switch (this->moving.y) {

                    case 0:
                        return this->view.y;

                    case -8 ... -1:
                        return this->view.y - (this->moving.y != 0 ? -8 - this->moving.y : 0);

                    case 1 ... 8:
                        return this->view.y - (this->moving.y != 0 ? 8 - this->moving.y : 0);

                }

            }
            else {

               return this->view.y;

            }
        return 0;
        }


        uint8_t getPlayer_X_ScreenCoords(uint8_t idx) {

            return this->players[idx].getX_WorldCoords() - this->getMapX();
        }


        uint8_t getPlayer_Y_ScreenCoords(uint8_t idx) {

            return this->players[idx].getY_WorldCoords() - this->getMapY();
        }


        void incLevelNumber() {

            this->levelNumber++;

        }


        void incGameState() {

            this->gameState = static_cast<GameState>(static_cast<uint8_t>(this->gameState) + 1);

        }


        void decGameState() {

            this->gameState = static_cast<GameState>(static_cast<uint8_t>(this->gameState) - 1);

        }


        uint8_t getItemIdx_ScreenCoords(uint8_t x, uint8_t y) {

            if (this->itemBitset[y][x] == Constants::ItemNotFound) {
                
                return Constants::ItemNotFound;

            }

            return this->itemBitset[y][x];

        }


        bool isEmpty_WorldCoords(uint8_t x, uint8_t y) {

            return this->itemBitset[y][x] != Constants::ItemNotFound;

        }


        uint8_t getItemIdx_WorldCoords(uint8_t x, uint8_t y) {

            return this->itemBitset[y - this->map.y][x - this->map.x];

        }


        int8_t getXOffset(Direction direction) {

            switch (direction) {

                case Direction::North:      return 0;
                case Direction::East:       return 1;
                case Direction::South:      return 0;
                default:                    return -1;

            }

        } 

        int8_t getYOffset(Direction direction) {

            switch (direction) {

                case Direction::North:      return -1;
                case Direction::East:       return 0;
                case Direction::South:      return 1;
                default:                    return 0;

            }

        } 


        bool isEmpty_WorldCoords(uint8_t x, uint8_t y, MapElement mapElement, uint8_t ignoreItemIdx = Constants::ItemNotFound) {

            if (mapElement == MapElement::None) {

                mapElement = this->getMapElement_WorldCoords(x, y);

            }

            switch (mapElement) {

                case MapElement::Tile_Solid:
                case MapElement::Tile_Hole:
                case MapElement::Tile_UpperEdge:
                case MapElement::Tile_BottonEdge_Left:
                case MapElement::Tile_BottonEdge_Right:
                case MapElement::Tile_BottonEdge_Middle:
                case MapElement::Tile_BottonEdge_FullWidth:
                case MapElement::Tile_Line_Left:
                case MapElement::Tile_Line_Middle:
                case MapElement::Tile_Line_Right:
                case MapElement::Tile_Line_FullWidth:

                    if (this->isItemAtWorldCoords(x, y, ignoreItemIdx) != Constants::ItemNotFound) {

                        return false;

                    }

                    for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

                        if (this->activePlayer == i) continue;

                        if (this->getPlayer(i).getX_WorldCoords() == x &&
                            this->getPlayer(i).getY_WorldCoords() == y) {

                            return false; 

                        }

                    }

                    return true;

                default:

                    return false;

            }

        }


        bool isEmptyForFall_WorldCoords(uint8_t x, uint8_t y, MapElement mapElement, uint8_t ignoreItemIdx = Constants::ItemNotFound) {

            switch (mapElement) {

                case MapElement::Tile_Hole:             // 9
                case MapElement::Tile_Line_Left:        // 14
                case MapElement::Tile_Line_Middle:         // 18
                case MapElement::Tile_Line_Right:       // 15
                case MapElement::Tile_Line_FullWidth:       // 13
                case MapElement::Tile_BottonEdge_Left:       // 11
                case MapElement::Tile_BottonEdge_Right:       // 12
                case MapElement::Tile_BottonEdge_Middle:       // 16
                case MapElement::Tile_BottonEdge_FullWidth:       // 17

                    if (this->isItemAtWorldCoords(x, y, ignoreItemIdx) != Constants::ItemNotFound) {
                        return false;
                    }

                    return true;

                default:

                    return false;

            }

        }

        bool canBlockMove(uint8_t itemIdx, Direction direction) {

            Item &item = this->getItem(itemIdx);            
            uint8_t itemType = static_cast<uint8_t>(item.getItemType()) - static_cast<uint8_t>(ItemType::Block_Start);

            int8_t x = item.getX() + this->getXOffset(direction);
            int8_t y = item.getY() + this->getYOffset(direction);

            for (uint8_t y1 = y; y1 < y + pgm_read_byte(Constants::Block_Heights + itemType); y1++) {

                for (uint8_t x1 = x; x1 < x + pgm_read_byte(Constants::Block_Widths + itemType); x1++) {

                    if (x1 < item.getX() || x1 >= item.getX() + pgm_read_byte(Constants::Block_Widths + itemType) ||
                        y1 < item.getY() || y1 >= item.getY() + pgm_read_byte(Constants::Block_Heights + itemType)) {

                        MapElement mapElement = this->getMapElement_WorldCoords(x1, y1);

                        if (!this->isEmpty_WorldCoords(x1, y1, mapElement, itemIdx)){

                            return false;

                        }

                        // Make sure player is not there ..

                        for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

                            if (this->getPlayer(i).getX_WorldCoords() == x1 &&
                                this->getPlayer(i).getY_WorldCoords() == y1) {

                                return false; 

                            }

                        }

                    }

                }
                
            }

            return true;

        }

        RotorTurning updateItems() {

            for (uint8_t i = 0; i < Constants::ItemCount; i++) {

                Item& item = this->items[i];

                switch (item.getItemType()) {

                    case ItemType::Rotator_Start ... ItemType::Rotator_End:

                        switch (static_cast<Rotation>(item.data.rotator.rotationDirection)) {

                            case Rotation::AntiClockwise:

                                if (item.data.rotator.rotationDelay == 0) {
                                    item.data.rotator.direction = (item.data.rotator.direction + 7) % 8;
                                    item.data.rotator.rotationDirection = static_cast<uint8_t>(Rotation::None);
                                    return RotorTurning::FinishedTurning;
                                }
                                else {
                                    item.data.rotator.rotationDelay--;
                                    return RotorTurning::StillTurning;
                                }

                                break;

                            case Rotation::Clockwise:

                                if (item.data.rotator.rotationDelay == 0) {
                                    item.data.rotator.direction = (item.data.rotator.direction + 1) % 8;
                                    item.data.rotator.rotationDirection = static_cast<uint8_t>(Rotation::None);
                                    return RotorTurning::FinishedTurning;
                                }
                                else {
                                    item.data.rotator.rotationDelay--;
                                    return RotorTurning::StillTurning;
                                }

                                break;

                        } 

                        break;

                    default:
                        return RotorTurning::NoneTurning;

                }

            }

            return RotorTurning::NoneTurning;

        }


        bool updateFallingBlocks() {

            for (uint8_t i = 0; i < Constants::ItemCount; i++) {

                Item& item = this->items[i];

                switch (item.getItemType()) {

                    case ItemType::Block_Start ... ItemType::Block_End:

                        if (item.data.block.startFall && (item.data.block.fallen < Constants::FallenCountMax)) {
                            item.data.block.fallen++;

                            if (item.data.block.fallen == Constants::FallenCountMax) {
                                return true;
                            }
                            else {
                                return false;
                            }

                        }

                        break;

                }

            }

            return false;

        }


        uint8_t getCompletedLevels() {

            if (this->allFloorsComplete(2))     return 2;
            if (this->allFloorsComplete(1))     return 2;
            if (this->allFloorsComplete(0))     return 1;
            if (!this->allFloorsComplete(0))    return 0;
            return 0;

        }


        bool allFloorsComplete() {

            uint8_t skillLevel = static_cast<uint8_t>(this->skillLevel);

            return this->allFloorsComplete(skillLevel);

        }


        bool allFloorsComplete(uint8_t skillLevel) {

            for (uint8_t i = skillLevel * 10; i < (skillLevel * 10) + 10; i++) {
                if (!this->getLevelComplete(i)) return false;
            }

            return true;

        }


        void incMenuLeft() {

            this->menuLeft = this->menuLeft + 2;
            
        }


        void decMenuLeft() {

            this->menuLeft = this->menuLeft - 2;
            
        }


        bool isObscuringBackground(MapElement mapElement) {

            switch (mapElement) {

                case MapElement::Tile_Hole:
                case MapElement::Tile_UpperEdge:
                    return true;

                default:
                    return false;


            }

        }


        #if defined(DEBUG) && defined(DEBUG_UNDOS)

            void printUndos() {

                

                    DEBUG_PRINTLN("------------------------------");
                    DEBUG_PRINT(undos.head);
                    DEBUG_PRINT(" ");
                    DEBUG_PRINTln(undos.tail);

                    for (uint8_t i = 0; i < Constants::UndoCount; i++) {
                        
                        Undo &undo = undos.undo[i];

                        DEBUG_PRINT(undo.playerIdx);
                        DEBUG_PRINT(" ");
                        DEBUG_PRINT(undo.playerX);
                        DEBUG_PRINT(" ");
                        DEBUG_PRINT(undo.playerY);
                        DEBUG_PRINT(" - ");
                        DEBUG_PRINT(undo.itemIdx);
                        DEBUG_PRINT(" ");
                        DEBUG_PRINT(undo.itemX);
                        DEBUG_PRINT(" ");
                        DEBUG_PRINT(undo.itemY);
                        DEBUG_PRINT(" ");
                        DEBUG_PRINTLN(undo.dataElement);

                    }            

            }

        #endif
