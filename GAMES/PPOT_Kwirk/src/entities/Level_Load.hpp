
        void initMap() {

            #if defined(DEBUG) && defined(DEBUG_LEVEL_LOAD)
                DEBUG_PRINT(F("- Level "));
                DEBUG_PRINT(this->levelNumber);
                DEBUG_PRINTLN(F("------------------------"));
            #endif

            uint24_t startPos = FX::readIndexedUInt24(Levels, this->levelNumber);
            FX::seekData(startPos);

            this->activePlayer = 0;
            this->width = FX::readPendingUInt8();
            this->height = FX::readPendingUInt8();

            for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

                this->getPlayer(i).setX_WorldCoords(FX::readPendingUInt8());
                this->getPlayer(i).setY_WorldCoords(FX::readPendingUInt8());
                this->getPlayer(i).setDirection(static_cast<Direction>(FX::readPendingUInt8()));

            }

            this->setViewX(FX::readPendingUInt8());
            this->setViewY(FX::readPendingUInt8());

            this->setAllowScrollX(FX::readPendingUInt8());
            this->setAllowScrollY(FX::readPendingUInt8());

            FX::readEnd();

            this->map.x = 127;
            this->map.y = 127;
            this->loadMap(true);


            // Clear items ..

            for (Item &item : this->items) {
                item.setItemType(ItemType::None);
                item.data.location.dataElement1 = 0;
                item.data.location.dataElement2 = 0;
                item.data.location.dataElement3 = 0;
            }


            // Clear undos ..

            #ifndef DEBUG_REMOVE_UNDO

                this->undos.head = 0;
                this->undos.tail = 0;

            #endif


            // Load items ..

            uint8_t itemIdx = 0;
            startPos = startPos + Constants::LevelLeadingMemOffset + (this->getWidth() * this->getHeight());

            FX::seekData(startPos);
            ItemType itemType = static_cast<ItemType>(FX::readPendingUInt8());

            while (itemType != ItemType::None) {

                Item &item = this->getItem(itemIdx);
                item.setItemType(itemType);
                item.setX(FX::readPendingUInt8());
                item.setY(FX::readPendingUInt8());
                item.data.location.dataElement1 = FX::readPendingUInt8();
                item.data.location.dataElement2 = FX::readPendingUInt8();

                if (itemType == ItemType::Stairs) {

                    this->stair.x = item.getX();
                    this->stair.y = item.getY();

                }

                itemType = static_cast<ItemType>(FX::readPendingUInt8());
                itemIdx++;

            }

            FX::readEnd();


            #if defined(DEBUG) && defined(DEBUG_LEVEL_LOAD)
            printItems();
            #endif

            this->loadItemBitset();

        }

        void loadMap(bool forceReload = false) {

            int8_t oldMapX = this->map.x;
            int8_t oldMapY = this->map.y;

            this->map.y = this->loadMap_MapY(this->getPlayer().getY_WorldCoords());
            this->map.x = this->loadMap_MapX(this->getPlayer().getX_WorldCoords());


            // if we do not need to load the map then exit early ..

            if (!forceReload && this->map.x == oldMapX && this->map.y == oldMapY) {
                
                return;

            }
            
            
            // Clear map ..

            for (uint8_t x = 0; x < Constants::LevelMapWidth; x++) {

                for (int8_t y = 0; y < Constants::LevelMapHeight; y++) {

                    mapElements[y][x] = MapElement::Tile_Solid;

                }

            }

            uint24_t startPos = FX::readIndexedUInt24(Levels, this->levelNumber);
            startPos = startPos + Constants::LevelLeadingMemOffset + (this->map.y * this->width) + this->map.x;

            #if defined(DEBUG) && defined(DEBUG_LEVEL_LOAD)
            DEBUG_PRINT(F("Width: "));
            DEBUG_PRINT(this->width);
            DEBUG_PRINT(F(", Height: "));
            DEBUG_PRINTLN(this->height);
            DEBUG_PRINT(F("Player x: "));
            DEBUG_PRINT(this->getPlayer().getX_WorldCoords());
            DEBUG_PRINT(F(", y: "));
            DEBUG_PRINTLN(this->getPlayer().getY_WorldCoords());
            DEBUG_PRINT(F("Map x: "));
            DEBUG_PRINT(this->map.x);
            DEBUG_PRINT(F(", y: "));
            DEBUG_PRINTLN(this->map.y);
            DEBUG_PRINT(F("Level Start: "));
            DEBUG_PRINTLN((uint32_t)Level_01);
            DEBUG_PRINT(F("StartPos: "));
            DEBUG_PRINTLN((uint32_t)startPos);
            #endif

            for (uint8_t y = 0; y < Constants::LevelMapHeight; y++) {

                FX::seekData(startPos);
                FX::readBytes((uint8_t*)&mapElements[y][0], Constants::LevelMapWidth);
                FX::readEnd();

                startPos = startPos + this->width;

            }

            #if defined(DEBUG) && defined(DEBUG_LEVEL_LOAD)
            this->printLevel();
            #endif

            this->loadItemBitset();

        }

        void setItemBitset(uint8_t x, uint8_t y, uint8_t val) {

            if (x >= this->map.x && x < this->map.x + Constants::LevelMapWidth &&
                y >= this->map.y && y < this->map.y + Constants::LevelMapHeight) {

                this->itemBitset[y - this->map.y][x - this->map.x] = val;

            }

        }

        void loadItemBitset() {

            // Load items bit map ..

            for (uint8_t y = 0; y < Constants::LevelMapHeight; y++) {

                for (uint8_t x = 0; x < Constants::LevelMapWidth; x++) {

                    this->itemBitset[y][x] = 255;

                }

            }

            for (uint8_t i = 0; i < Constants::ItemCount; i++) {

                Item& item = this->items[i];
                uint8_t itemX = item.getX();
                uint8_t itemY = item.getY();

                switch (item.getItemType()) {
                    
                    case ItemType::None: 
                        break;

                    case ItemType::Block_Start ... ItemType::Block_End:
                        {
                            uint8_t blockIdx = static_cast<uint8_t>(item.getItemType()) - static_cast<uint8_t>(ItemType::Block_Start);
                            uint8_t height = pgm_read_byte(Constants::Block_Heights + blockIdx);
                            uint8_t width = pgm_read_byte(Constants::Block_Widths + blockIdx);

                            for (uint8_t y = itemY; y < itemY + height; y++) {

                                for (uint8_t x = itemX; x < itemX + width; x++) {

                                    if (x >= this->map.x && x < this->map.x + Constants::LevelMapWidth &&
                                        y >= this->map.y && y < this->map.y + Constants::LevelMapHeight) {

                                        if (item.data.block.fallen != Constants::FallenCountMax) {

                                            this->itemBitset[y - this->map.y][x - this->map.x] = i;

                                        }
                                        else {

                                            switch (this->mapElements[y - this->map.y][x - this->map.x]) {

                                                case MapElement::Tile_Hole:

                                                    switch (this->mapElements[y - this->map.y - 1][x - this->map.x]) {

                                                        case MapElement::Tile_BottonEdge_Left:
                                                        case MapElement::Tile_BottonEdge_Right:
                                                        case MapElement::Tile_BottonEdge_Middle:
                                                        case MapElement::Tile_BottonEdge_FullWidth:
                                                            this->mapElements[y - this->map.y][x - this->map.x] = MapElement::Tile_UpperEdge;
                                                            break;

                                                        default:
                                                            this->mapElements[y - this->map.y][x - this->map.x] = MapElement::Tile_Solid;

                                                    }

                                                    break;

                                                case MapElement::Tile_Line_Left:
                                                case MapElement::Tile_Line_Middle:
                                                case MapElement::Tile_Line_Right:
                                                case MapElement::Tile_Line_FullWidth:

                                                    switch (this->mapElements[y - this->map.y - 1][x - this->map.x]) {

                                                        case MapElement::Tile_Solid:
                                                            this->mapElements[y - this->map.y][x - this->map.x] = MapElement::Tile_Solid;
                                                            break;

                                                        default:
                                                            this->mapElements[y - this->map.y][x - this->map.x] = MapElement::Tile_UpperEdge;

                                                    }

                                                    break;

                                                case MapElement::Tile_BottonEdge_Left:
                                                case MapElement::Tile_BottonEdge_Right:
                                                case MapElement::Tile_BottonEdge_Middle:
                                                case MapElement::Tile_BottonEdge_FullWidth:

                                                    this->mapElements[y - this->map.y][x - this->map.x] = MapElement::Tile_Solid;
                                                    break;

                                                default:
                                                    break;

                                            }

                                        }       

                                    }

                                }

                            }        


                            // Bottom row ..

                            if (item.data.block.fallen == Constants::FallenCountMax) {
                                    
                                uint8_t y = itemY + height;

                                for (uint8_t x = itemX; x < itemX + width; x++) {

                                    if (x >= this->map.x && x < this->map.x + Constants::LevelMapWidth &&
                                        y >= this->map.y && y < this->map.y + Constants::LevelMapHeight) {

                                        switch (this->mapElements[y - this->map.y][x - this->map.x]) {

                                            case MapElement::Tile_Hole:
                                                this->mapElements[y - this->map.y][x - this->map.x] = MapElement::Tile_BottonEdge_Left;
                                                break;

                                            case MapElement::Tile_UpperEdge:
                                                this->mapElements[y - this->map.y][x - this->map.x] = MapElement::Tile_Solid;
                                                break;

                                            default:
                                                break;

                                        }

                                    }

                                }

                            }

                        }

                        break;

                    case ItemType::Rotator_Start ... ItemType::Rotator_End:
                        {
                            uint8_t x = itemX;
                            uint8_t y = itemY;
                            uint8_t i80 = 0x80 | i;

                            this->setItemBitset(x, y, i);
                            
                            if (x >= this->map.x - 1 && x < this->map.x + Constants::LevelMapWidth + 1 &&
                                y >= this->map.y - 1 && y < this->map.y + Constants::LevelMapHeight + 1) {

                                switch (item.getItemType()) {

                                    case ItemType::Rotator_One:

                                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                                            case Direction::North:

                                                this->setItemBitset(x, y - 1, i80);
                                                break;   

                                            case Direction::East:

                                                this->setItemBitset(x + 1, y, i80);
                                                break;                             

                                            case Direction::South:

                                                this->setItemBitset(x, y + 1, i80);
                                                break;                        

                                            case Direction::West:

                                                this->setItemBitset(x - 1, y, i80);
                                                break;

                                        }

                                        break;

                                    case ItemType::Rotator_Two:

                                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                                            case Direction::North:

                                                this->setItemBitset(x, y - 1, i80);
                                                this->setItemBitset(x + 1, y, i80);
                                                break;

                                            case Direction::East:

                                                this->setItemBitset(x + 1, y, i80);
                                                this->setItemBitset(x, y + 1, i80);
                                                break;        

                                            case Direction::South:

                                                this->setItemBitset(x - 1, y, i80);
                                                this->setItemBitset(x, y + 1, i80);
                                                break;

                                            case Direction::West:

                                                this->setItemBitset(x - 1, y, i80);
                                                this->setItemBitset(x, y - 1, i80);
                                                break;                              

                                        }

                                        break;

                                    case ItemType::Rotator_Three:

                                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                                            case Direction::North:

                                                this->setItemBitset(x - 1, y, i80);
                                                this->setItemBitset(x, y - 1, i80);
                                                this->setItemBitset(x + 1, y, i80);
                                                break;

                                            case Direction::East:

                                                this->setItemBitset(x, y - 1, i80);
                                                this->setItemBitset(x + 1, y, i80);
                                                this->setItemBitset(x, y + 1, i80);
                                                break;                           

                                            case Direction::South:

                                                this->setItemBitset(x - 1, y, i80);
                                                this->setItemBitset(x, y + 1, i80);
                                                this->setItemBitset(x + 1, y, i80);
                                                break;           

                                            case Direction::West:

                                                this->setItemBitset(x, y - 1, i80);
                                                this->setItemBitset(x - 1, y, i80);
                                                this->setItemBitset(x, y + 1, i80);
                                                break;                                         

                                        }

                                        break; 

                                    case ItemType::Rotator_Four:

                                        this->setItemBitset(x, y - 1, i80);
                                        this->setItemBitset(x + 1, y, i80);
                                        this->setItemBitset(x, y + 1, i80);
                                        this->setItemBitset(x - 1, y, i80);
                                        break;                                         
                        
                                    case ItemType::Rotator_Five:

                                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                                            case Direction::North:
                                            case Direction::South:

                                                this->setItemBitset(x, y - 1, i80);
                                                this->setItemBitset(x, y + 1, i80);
                                                break;                  

                                            case Direction::East:
                                            case Direction::West:

                                                this->setItemBitset(x - 1, y, i80);
                                                this->setItemBitset(x + 1, y, i80);
                                                break;                                        

                                        }

                                        break;  

                                }

                                break;

                            }

                        }

                        break;

                    default:

                        this->setItemBitset(itemX, itemY, i);
                        break;

                }

            }

            #if defined(DEBUG) && defined(DEBUG_ITEM_BITSET)
            DEBUG_PRINTLN("-- Item Bitset ------------");            

            for (int8_t y = 0; y < Constants::LevelMapHeight; y++) {
        
                if (y < 10)  DEBUG_PRINT("0");
                DEBUG_PRINT(y);
                DEBUG_PRINT(") ");
    
                for (uint8_t x = 0; x < Constants::LevelMapWidth; x++) {

                    DEBUG_PRINT(" ");

                    if (this->itemBitset[y][x] != Constants::ItemNotFound) {
                        if (this->itemBitset[y][x] < 16) DEBUG_PRINT("0");
                        DEBUG_PRINT(this->itemBitset[y][x], HEX);
                    }
                    else {
                        DEBUG_PRINT("..");
                    }

                }

                DEBUG_PRINTLN("");            

            }            
            #endif

        }
 
        void printItems() {

            DEBUG_PRINTLN(F("- Items --------------------------"));
            for (uint8_t i = 0; i < Constants::ItemCount; i++) {

                Item& item = this->items[i];

                if (item.getItemType() != ItemType::None) {
                    if (i < 10) { DEBUG_PRINT(F("0")); }
                    DEBUG_PRINT(i);
                    DEBUG_PRINT(F(" - "));
                    DEBUG_PRINT(F("ItemType: "));
                    DEBUG_PRINT((uint8_t)item.getItemType());
                    DEBUG_PRINT(F(", x: "));
                    DEBUG_PRINT(item.getX());
                    DEBUG_PRINT(F(", y: "));
                    DEBUG_PRINT(item.getY());
                    DEBUG_PRINT(F(", data1: "));
                    DEBUG_PRINT(item.data.location.dataElement1);
                    DEBUG_PRINT(F(", data2: "));
                    DEBUG_PRINTLN(item.data.location.dataElement2);
                }

            }

        }
 
        void printLevel() {

            DEBUG_PRINTLN(F("   0         1"));
            DEBUG_PRINTLN(F("   012345678901234567"));
            DEBUG_PRINTLN(F("   ---------------"));

            for (int8_t y = 0; y < Constants::LevelMapHeight; y++) {

                if (y < 10) DEBUG_PRINT(" ");
                DEBUG_PRINT(y);
                DEBUG_PRINT(" ");

                for (int8_t x = 0; x < Constants::LevelMapWidth; x++) {

                    DEBUG_PRINT((uint8_t)this->getMapElement_ScreenCoords(x, y), HEX);

                }

                DEBUG_PRINTLN("");

            }

        }
