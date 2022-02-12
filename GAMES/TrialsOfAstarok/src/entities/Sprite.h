#pragma once

#include "../utils/Arduboy2Ext.h"
#include "../utils/Constants.h"
#include "../utils/Enums.h"

class AstarokGame;

class Sprite {

    protected:
        const uint8_t       * spriteData;
        const uint8_t       * spriteImg;
        const uint8_t       * spriteMask;
        uint8_t               autoExpire;
        bool                  active;

    public:

        void init(const uint8_t * data, const uint8_t * img, const uint8_t * mask, int tX, int tY);
        bool isFalling();
        void move();
        bool jump();
        void draw();
        void deactivate(bool explode);

        uint8_t getWidth();
        uint8_t getHeight();
        int16_t getLeftX();
        int16_t getRightX();
        int16_t getTopY();
        int16_t getBottomY();

        uint8_t getFlags();
        uint8_t getType();
        uint8_t getFrame();

        Rect getRect();

        int16_t               x, y, yInit;
        int8_t                vx, vy;
        uint16_t              maxHeight = 0;        
        uint8_t               jumpBoost;
        uint8_t               explodeCounter;
        bool                  continuousBButton;

        Direction             facing;
        AstarokGame           * game;
        Arduboy2Ext           * arduboy;

    private:
        uint8_t               frame;

    protected:
        void clear();
        void applyGravity();
        uint8_t collide(int16_t tX, int16_t tY);
        bool collisionCheckX(Direction direction);
        bool collisionCheckY(Direction direction);

};