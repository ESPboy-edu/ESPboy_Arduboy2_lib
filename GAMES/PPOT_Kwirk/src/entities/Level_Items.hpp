
        Item& getItem(uint8_t idx) {

            return this->items[idx];

        }

        Item* getItem_Reference(uint8_t idx) {

            if (idx != Constants::ItemNotFound) {

                return &this->items[idx];

            }
            else {

                return nullptr;
            }

        }

        uint8_t isItemAtWorldCoords(uint8_t x, uint8_t y, uint8_t ignoreItemIdx = Constants::ItemNotFound) {

            #if defined(DEBUG) && defined(DEBUG_LEVEL_LOAD)
            printItems();
            #endif
            
            for (uint8_t i = 0; i < Constants::ItemCount; i++) {

                if (i == ignoreItemIdx) continue;

                Item &item = this->getItem(i);
                ItemType itemType = item.getItemType();
                uint8_t itemX = item.getX();
                uint8_t itemY = item.getY();

                switch (itemType) {

                    case ItemType::None: 
                        return Constants::ItemNotFound;

                    case ItemType::Stairs:

                        if (x == itemX && y == itemY) {
                            return i;
                        }

                        break;

                    case ItemType::Block_Start ... ItemType::Block_End:
                        {
                            uint8_t itemType = static_cast<uint8_t>(item.getItemType()) - static_cast<uint8_t>(ItemType::Block_Start);

                            if (item.data.block.fallen < Constants::FallenCountMax) {

                                uint8_t width = pgm_read_byte(Constants::Block_Widths + itemType);
                                uint8_t height = pgm_read_byte(Constants::Block_Heights + itemType);

                                if (x >= itemX && x < itemX + width && y >= itemY && y < itemY + height) {

                                    return i;

                                }

                            }

                        }

                        break;

                    case ItemType::Rotator_Start ... ItemType::Rotator_End:

                        if (x == itemX && y == itemY) return i;

                        switch (itemType) {

                            case ItemType::Rotator_One:

                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                    case Direction::North:
                                        
                                        if (x == itemX && y == itemY - 1) {

                                            return i;
                                        }

                                        break;

                                    case Direction::East:
                                        
                                        if (x == itemX + 1 && y == itemY) {

                                            return i;
                                        }

                                        break;

                                    case Direction::South:
                                        
                                        if (x == itemX && y == itemY + 1) {

                                            return i;
                                        }

                                        break;

                                    case Direction::West:
                                        
                                        if (x == itemX - 1 && y == itemY) {

                                            return i;
                                        }

                                        break;

                                }

                                break;

                            case ItemType::Rotator_Two:

                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                    case Direction::North:
                                        
                                        if ((x == itemX && y == itemY - 1) ||
                                            (x == itemX + 1 && y == itemY)) {

                                            return i;
                                        }

                                        break;

                                    case Direction::East:
                                        
                                        if ((x == itemX + 1 && y == itemY) ||
                                            (x == itemX && y == itemY + 1)) {
                                            return i;
                                        }

                                        break;

                                    case Direction::South:
                                        
                                        if ((x == itemX - 1 && y == itemY) ||
                                            (x == itemX && y == itemY + 1)) {
                                            return i;
                                        }

                                        break;

                                    case Direction::West:
                                        
                                        if ((x == itemX - 1 && y == itemY) ||
                                            (x == itemX && y == itemY - 1)) {
                                            return i;
                                        }

                                        break;

                                }

                                break;

                            case ItemType::Rotator_Three:

                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                    case Direction::North:
                                        
                                        if ((x == itemX - 1 && y == itemY) ||
                                            (x == itemX && y == itemY - 1) ||
                                            (x == itemX + 1 && y == itemY)) {
                                            return i;
                                        }

                                        break;

                                    case Direction::East:
                                        
                                        if ((x == itemX && y == itemY - 1) ||
                                            (x == itemX + 1 && y == itemY) ||
                                            (x == itemX && y == itemY + 1)) {
                                            return i;
                                        }

                                        break;

                                    case Direction::South:
                                        
                                        if ((x == itemX - 1 && y == itemY) ||
                                            (x == itemX && y == itemY + 1) ||
                                            (x == itemX + 1 && y == itemY)) {
                                            return i;
                                        }

                                        break;

                                    case Direction::West:
                                        
                                        if ((x == itemX - 1 && y == itemY) ||
                                            (x == itemX && y == itemY - 1) ||
                                            (x == itemX && y == itemY + 1)) {
                                            return i;
                                        }

                                        break;

                                }

                                break;

                            case ItemType::Rotator_Four:

                                if ((x == itemX && y == itemY - 1) ||
                                    (x == itemX && y == itemY + 1) ||
                                    (x == itemX - 1 && y == itemY) ||
                                    (x == itemX + 1 && y == itemY)) {

                                    return i;

                                }

                                break;

                            case ItemType::Rotator_Five:

                                switch (static_cast<Direction>(item.data.rotator.direction)) {

                                    case Direction::North:
                                    case Direction::South:
                                        
                                        if ((x == itemX && y == itemY - 1) ||
                                            (x == itemX && y == itemY + 1)) {
                                            return i;
                                        }

                                        break;

                                    case Direction::East:
                                    case Direction::West:
                                        
                                        if ((x == itemX - 1 && y == itemY) ||
                                            (x == itemX + 1 && y == itemY)) {
                                            return i;
                                        }

                                        break;

                                }

                                break;

                        }

                        break;

                }

            }

            return Constants::ItemNotFound;
         
        }
