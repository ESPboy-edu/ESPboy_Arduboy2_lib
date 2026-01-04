
#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"

class PopoutMenu {

    private:

        uint8_t x = 128;
        uint8_t select = 0;
        Direction direction = Direction::None;
        bool allowClose = true;

    public:

        uint8_t getX() const                                        { return this->x; }
        uint8_t getSelect() const                                   { return this->select; }
        Direction getDirection() const                              { return this->direction; }
        bool getAllowClose() const                                  { return this->allowClose; }

        void setX(uint8_t val)                                      { this->x = val; }
        void setSelect(uint8_t val)                                 { this->select = val; }
        void setDirection(Direction val)                            { this->direction = val; }
        void setAllowClose(bool val)                                { this->allowClose = val; }

};
