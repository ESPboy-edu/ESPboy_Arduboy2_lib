        void rotateClockwise(Item &item) {

            #if !defined(DEBUG_NO_ROTATION)

                item.data.rotator.direction = (item.data.rotator.direction + 1) % 8;
                item.data.rotator.rotationDirection = static_cast<uint8_t>(Rotation::Clockwise);
                item.data.rotator.rotationDelay = Constants::RotationDelay;

                loadItemBitset();

                #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                DEBUG_PRINT(F("New Direction "));
                DEBUG_PRINTLN(item.data.rotator.direction);
                #endif

            #endif

        }

        void rotateAntiClockwise(Item &item) {

            #if !defined(DEBUG_NO_ROTATION)

                item.data.rotator.direction = (item.data.rotator.direction + 7) % 8;
                item.data.rotator.rotationDirection = static_cast<uint8_t>(Rotation::AntiClockwise);
                item.data.rotator.rotationDelay = Constants::RotationDelay;

                loadItemBitset();

                #if defined(DEBUG) && defined(DEBUG_MOVEMENTS_ROTATORS)
                DEBUG_PRINT(F("New Direction "));
                DEBUG_PRINTLN(item.data.rotator.direction);
                #endif

            #endif

        }
        
        bool canRotate(Item &item, uint8_t itemIdx, Rotation rotation) {

            #if !defined(DEBUG_NO_ROTATION)

                Direction direction = Direction::North;
                uint8_t val = 0;

                uint8_t itemX = item.getX();
                uint8_t itemY = item.getY();

                switch (item.getItemType()) {

                    case ItemType::Rotator_One:

                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                            case Direction::North:

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Nth AC");
                                        #endif

                                        //    xb    <
                                        //    xB

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX - 1, itemY, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Nth CW");
                                        #endif

                                        // >   bx
                                        //     Bx

                                        return (this->isEmpty_WorldCoords(itemX + 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY, MapElement::None, itemIdx));

                                }

                                return true;   

                            case Direction::East:

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Est AC");
                                        #endif

                                        //     xx
                                        //     Bb    
                                        //      ^

                                        return (this->isEmpty_WorldCoords(itemX, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY - 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Est CW");
                                        #endif

                                        //      V
                                        //     Bb    
                                        //     xx

                                        return (this->isEmpty_WorldCoords(itemX, itemY + 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY + 1, MapElement::None, itemIdx));

                                }

                                return true;                             

                            case Direction::South:

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Sth AC");
                                        #endif

                                        //     Bx
                                        // >   bx

                                        return (this->isEmpty_WorldCoords(itemX + 1, itemY, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY + 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Sth CW");
                                        #endif

                                        //    xB
                                        //    xb   <

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX - 1, itemY + 1, MapElement::None, itemIdx));

                                }

                                return true;                       

                            case Direction::West:

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Wst AC");
                                        #endif

                                        //    V
                                        //    bB
                                        //    xx

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY + 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX, itemY + 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_One Wst CW");
                                        #endif

                                        //    xx
                                        //    bB
                                        //    ^

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX, itemY - 1, MapElement::None, itemIdx));

                                }

                                return true;                            

                        }

                        break;

                    case ItemType::Rotator_Two:

                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                            case Direction::North:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Two Nth");
                                #endif

                                //     bx
                                //     Bb

                                if (!this->isEmpty_WorldCoords(itemX + 1, itemY - 1, MapElement::None)) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Nth AC");
                                        #endif

                                        //    xb   <
                                        //    xBb

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX - 1, itemY, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Nth CW");
                                        #endif

                                        //  >  b    
                                        //     Bb   
                                        //     xx

                                        return (this->isEmpty_WorldCoords(itemX, itemY + 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY + 1, MapElement::None, itemIdx));

                                }

                                return true;   

                            case Direction::East:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Two Est");
                                #endif

                                //     Bb
                                //     bx

                                if (!this->isEmpty_WorldCoords(itemX + 1, itemY + 1, MapElement::None)) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Est AC");
                                        #endif

                                        //     xx
                                        //     Bb
                                        // >   b

                                        return (this->isEmpty_WorldCoords(itemX, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY - 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Est CW");
                                        #endif

                                        //    xBb
                                        //    xb    <

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX - 1, itemY + 1, MapElement::None, itemIdx));

                                }

                                return true;                              

                            case Direction::South:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Two Sth");
                                #endif

                                //     bB
                                //     xb

                                if (this->isItemAtWorldCoords(itemX - 1, itemY + 1, itemIdx) != Constants::ItemNotFound) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Sth AC");
                                        #endif

                                        //     bBx
                                        // >    bx

                                        return (this->isEmpty_WorldCoords(itemX + 1, itemY, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY + 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Sth CW");
                                        #endif

                                        //     xx
                                        //     bB 
                                        //      b  <
                                        
                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX, itemY - 1, MapElement::None, itemIdx));

                                }

                                return true;                              

                            case Direction::West:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Two Wst");
                                #endif

                                //     xb
                                //     bB
                                
                                if (this->isItemAtWorldCoords(itemX - 1, itemY - 1, itemIdx) != Constants::ItemNotFound) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Wst AC");
                                        #endif

                                        //      b   <
                                        //     bB
                                        //     xx

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY + 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX, itemY + 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Two Wst CW");
                                        #endif

                                        //  >   bx   
                                        //     bBx

                                        return (this->isEmpty_WorldCoords(itemX + 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY, MapElement::None, itemIdx));

                                }

                                return true;                             

                        }

                        break;

                    case ItemType::Rotator_Three:

                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                            case Direction::North:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Three Nth");
                                #endif

                                //     xbx   
                                //     bBb

                                if (this->isItemAtWorldCoords(itemX - 1, itemY - 1, itemIdx) != Constants::ItemNotFound) return false;
                                if (this->isItemAtWorldCoords(itemX + 1, itemY - 1, itemIdx) != Constants::ItemNotFound) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Nth AC");
                                        #endif

                                        //      b    <
                                        //     bBb
                                        //     xx

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY + 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX, itemY + 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Nth CW");
                                        #endif

                                        // >    b    
                                        //     bBb
                                        //      xx

                                        return (this->isEmpty_WorldCoords(itemX, itemY + 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY + 1, MapElement::None, itemIdx));

                                }

                                return true;   

                            case Direction::East:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Three Est");
                                #endif

                                //      bx   
                                //      Bb
                                //      bx   

                                if (this->isItemAtWorldCoords(itemX + 1, itemY - 1, itemIdx) != Constants::ItemNotFound) return false;
                                if (this->isItemAtWorldCoords(itemX + 1, itemY + 1, itemIdx) != Constants::ItemNotFound) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Est AC");
                                        #endif

                                        //     xb   
                                        //     xBb
                                        // >    b

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX - 1, itemY, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Est CW");
                                        #endif

                                        // >    b   
                                        //     xBb
                                        //     xb

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX - 1, itemY + 1, MapElement::None, itemIdx));

                                }

                                return true;                            

                            case Direction::South:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Three Sth");
                                #endif

                                //     bBb
                                //     xbx   

                                if (this->isItemAtWorldCoords(itemX - 1, itemY + 1, itemIdx) != Constants::ItemNotFound) return false;
                                if (this->isItemAtWorldCoords(itemX + 1, itemY + 1, itemIdx) != Constants::ItemNotFound) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Sth AC");
                                        #endif

                                        //      xx
                                        //     bBb
                                        // >    b    

                                        return (this->isEmpty_WorldCoords(itemX, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY - 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Sth CW");
                                        #endif

                                        //     xx
                                        //     bBb
                                        //      b    <

                                        return (this->isEmpty_WorldCoords(itemX - 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX, itemY - 1, MapElement::None, itemIdx));

                                }

                                return true;                             

                            case Direction::West:

                                #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                DEBUG_PRINTLN("Rotator_Three Wst");
                                #endif

                                //     xb  
                                //     bB
                                //     xb 

                                if (this->isItemAtWorldCoords(itemX - 1, itemY - 1, itemIdx) != Constants::ItemNotFound) return false;
                                if (this->isItemAtWorldCoords(itemX - 1, itemY + 1, itemIdx) != Constants::ItemNotFound) return false;

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Wst AC");
                                        #endif

                                        //      b  
                                        //     bBx
                                        // >    bx  

                                        return (this->isEmpty_WorldCoords(itemX + 1, itemY, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY + 1, MapElement::None, itemIdx));

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Three Wst CW");
                                        #endif

                                        // >    bx
                                        //     bBx
                                        //      b  

                                        return (this->isEmpty_WorldCoords(itemX + 1, itemY - 1, MapElement::None, itemIdx) &&
                                                this->isEmpty_WorldCoords(itemX + 1, itemY, MapElement::None, itemIdx));

                                }

                                return true;                            

                        }

                        return true;   

                    case ItemType::Rotator_Four:

                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                        DEBUG_PRINTLN("Rotator_Four");
                        #endif

                        //     xbx  
                        //     bBb
                        //     xbx  
                        
                        if (this->isItemAtWorldCoords(itemX - 1, itemY - 1, itemIdx) != Constants::ItemNotFound) return false;
                        if (this->isItemAtWorldCoords(itemX + 1, itemY - 1, itemIdx) != Constants::ItemNotFound) return false;
                        if (this->isItemAtWorldCoords(itemX - 1, itemY + 1, itemIdx) != Constants::ItemNotFound) return false;
                        if (this->isItemAtWorldCoords(itemX + 1, itemY + 1, itemIdx) != Constants::ItemNotFound) return false;

                        return true;                            

                    case ItemType::Rotator_Five:

                        switch (static_cast<Direction>(item.data.rotator.direction)) {

                            case Direction::North:
                            case Direction::South:

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Five Nth/Sth AC");
                                        #endif

                                        //     xb  
                                        //     xBx
                                        // >    bx  

                                        return (this->isItemAtWorldCoords(itemX - 1, itemY - 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX - 1, itemY, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX + 1, itemY, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX + 1, itemY + 1, itemIdx) == Constants::ItemNotFound);

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Five Nth/Sth CW");
                                        #endif

                                        // >    bx  
                                        //     xBx
                                        //     xb  

                                        return (this->isItemAtWorldCoords(itemX - 1, itemY, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX - 1, itemY + 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX + 1, itemY - 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX + 1, itemY, itemIdx) == Constants::ItemNotFound);

                                }

                                return true;                            

                            case Direction::East:
                            case Direction::West:

                                switch (rotation) {

                                    case Rotation::AntiClockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Five Est/Wst AC");
                                        #endif

                                        // V    xx 
                                        //     bBb
                                        //     xx

                                        return (this->isItemAtWorldCoords(itemX, itemY - 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX + 1, itemY - 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX - 1, itemY + 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX, itemY + 1, itemIdx) == Constants::ItemNotFound);

                                    case Rotation::Clockwise:

                                        #if defined(DEBUG) && defined(DEBUG_CAN_ROTATOR)
                                        DEBUG_PRINTLN("Rotator_Five Est/Wst CW");
                                        #endif

                                        //     xx 
                                        //     bBb
                                        // ^    xx

                                        return (this->isItemAtWorldCoords(itemX - 1, itemY - 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX, itemY - 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX, itemY + 1, itemIdx) == Constants::ItemNotFound) &&
                                            (this->isItemAtWorldCoords(itemX + 1, itemY + 1, itemIdx) == Constants::ItemNotFound);

                                }

                                return true;                            

                        }

                        return true;  

                }

            #else

                return false;

            #endif
         return(0);
        }
