#include "Arduboy2Ext.h"
#include <Sprites.h>
#include "BaseItem.h"

class Enemy : public BaseItem {

    public:

        int8_t vx;
        int8_t vy;
        int16_t lastX;
        int16_t lastY;
        int8_t dir;

        void reset() {

            BaseItem::reset();
            this->dir = 0;

        }
        
};
