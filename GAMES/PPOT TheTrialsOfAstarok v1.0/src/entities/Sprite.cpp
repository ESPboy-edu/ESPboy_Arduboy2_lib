#include "Sprite.h"
#include "../../AstarokGame.h"

uint8_t Sprite::getFlags() { 
    
    if (this->spriteData == nullptr) { return 0; }
    return pgm_read_byte(this->spriteData + SpriteDataLayout::Flags); 
    
}

uint8_t Sprite::getType() { 
    
    if (this->spriteData == nullptr) { return 0; }
    return pgm_read_byte(this->spriteData + SpriteDataLayout::Type); 
    
}

int16_t Sprite::getRightX()     { return this->x + this->getWidth() - 1; }
int16_t Sprite::getTopY()       { return this->y; }
int16_t Sprite::getBottomY()    { return this->y + this->getHeight() - 1; }
int16_t Sprite::getLeftX()      { return this->x; }

uint8_t Sprite::getWidth() { 

    if (this->getType() == ObjectTypes::Player) {
        return 12; 
    }
    else {
        return pgm_read_byte(&this->spriteImg[0]); 
    }

}

uint8_t Sprite::getHeight() { 

    if (this->getType() == ObjectTypes::Player) {
        return 12; 
    }
    else {
        return pgm_read_byte(&this->spriteImg[1]); 
    }

}

void Sprite::init(const uint8_t * data, const uint8_t * img, const uint8_t * mask, int tX, int tY) {

    this->spriteData = data;
    this->spriteImg = img;
    this->spriteMask = mask;
    this->x = tX; 
    this->y = tY;
    this->yInit = tY;
    this->vx = 0; 
    this->vy = 0;
    this->facing = Direction::Right;
    this->autoExpire = 0;
    this->explodeCounter = 0;

}

void Sprite::clear() {

    this->spriteData = nullptr;
    this->x = -1;
    this->y = -1;
    this->vx = 0;
    this->vy = 0;
    this->autoExpire = 0;
    this->explodeCounter = 0;

}

uint8_t Sprite::collide(int16_t tX, int16_t tY) {

    int16_t nX = tX / Constants::TileSize;
    int16_t nY = tY / Constants::TileSize;

    if (this->game->level.isTile(nX, nY)) return Constants::Collision_Platform;  

    uint8_t object = this->game->level.checkObject(nX, nY);

    switch (object) {

        case ObjectTypes::Spider ... ObjectTypes::UnderGroundExit:
        case ObjectTypes::Fireball:
            return object;

        case ObjectTypes::Sign:
            return 0;

    }

    return 0;
}

bool Sprite::isFalling() {
    return !this->collisionCheckY(Direction::Down);
}

void Sprite::applyGravity() {
    if (vy < 6) vy++;
}

bool Sprite::collisionCheckX(Direction direction) {

    for (uint8_t tY = 0; tY < (this->getHeight() / Constants::TileSize); tY++) {

        switch (direction) {

            case Left:
                if (this->collide(x - 1, y + (tY * Constants::TileSize)) || this->collide(x - 1, y + ((tY + 1) * Constants::TileSize) - 1)) {
                    return true;
                }
                break;

            case Right:
                if (this->collide(x + this->getWidth(), y + (tY * Constants::TileSize)) || this->collide(x + this->getWidth(), y + ((tY + 1) * Constants::TileSize) - 1)) {
                    return true;
                }
                break;

            default: break;

        }

    }

    return false;
}

bool Sprite::collisionCheckY(Direction direction) {

    uint8_t width = this->getType() == ObjectTypes::Player ? 9 : this->getWidth();

    for (uint8_t tX = 0; tX < (this->getWidth() / Constants::TileSize); tX++) {

        switch (direction) {

            case Up:
                if (this->collide(x + (tX * Constants::TileSize), y - 1) || this->collide(x + (tX * Constants::TileSize) + width - 1, y - 1)) {
                    return true;
                }
                break;

            case Down:
                if (this->collide(x + (tX * Constants::TileSize), y + this->getHeight()) || this->collide(x + (tX * Constants::TileSize) + width - 1, y + this->getHeight())) {
                    return true;
                }
                break;

            default: break;
        }

    }
    return false;

}

void Sprite::deactivate(bool explode) {

    this->active = false;

    if (!explode) {

        this->explodeCounter = 0;
        this->spriteData = nullptr;
        this->facing = Direction::Up;
        this->clear();

    }
    else {

        this->explodeCounter = 21;

    }

}

void Sprite::move() {
        

    // Capture skulls etc in walls ..

    if (this->collide(this->x, this->y) == Constants::Collision_Platform) {

        this->deactivate(false);
       
    }

    switch (this->getType()) {

        case ObjectTypes::Player:

            if (this->vx == 0 && this->vy == 0) {
                if (this->game->arduboy->getFrameCount(6) == 0) {
                this->frame = (this->frame + 1) % 3;
                }
            }

            break;

        case ObjectTypes::Health:
        case ObjectTypes::Coin:

            if (this->autoExpire > 0) {
    
                this->autoExpire--;
    
                if (this->autoExpire == 0) {

                    this->deactivate(false);

                }

            }

            break;

        case ObjectTypes::Spider:

            if (this->vy == 0 && !this->isFalling() && hash(this->game->seed) % 20 == 0) {
                this->vy = -(hash(this->game->seed) % 8);
            }

            break;
        
    }

    // Handle player frame if stationary ..

    if (this->getType() == ObjectTypes::Player) {

        if (this->vx == 0 && this->vy == 0) {
            if (this->game->arduboy->getFrameCount(6) == 0) {
               this->frame = (this->frame + 1) % 3;
            }
         }

    }
 
    switch (this->getType()) {
        
        case ObjectTypes::Health:
            break;

        case ObjectTypes::Fireball:

            if (this->vy == Constants::Fireball_NotMoving && hash(this->game->seed) % 40 == 0) {
                this->vy = Constants::Fireball_StartPos;
            }

            if (this->vy != Constants::Fireball_NotMoving) {

                if (this->y - 32 > static_cast<int16_t>(this->maxHeight)) {

                    this->y = this->y + (this->vy / 4);

                }
                else {
                
                    this->y = this->y + (this->vy / 4);                
                    this->vy++;

                }
            }

            if (this->y > this->yInit) {
                this->vy = Constants::Fireball_NotMoving;
                this->y = this->yInit;
            }

            break;

        default:

            if (this->getFlags() & 1) {
                x += vx;
                y += vy;
                return;
            }

            if (vy == 0) { // Start fall
                if (isFalling()) applyGravity();
            }

            if (vy > 0) { // Down
                for (int a = 0; a < vy; a++) {
                    if (isFalling()) {
                        y++;
                    }
                    else { 
                        vy = 0; 
                        break; 
                    }
                }

                if (isFalling()) {
                    applyGravity();
                }

            }


            if (vy < 0) { // Up

                applyGravity();

                for (int a = 0; a > vy; a--) {
                    if (this->collisionCheckY(Direction::Up)) { 
                        vy = 0; 
                        break; 
                    }
                    else {
                        y--;
                    }
                }

            }

            if (vx > 0) { // Right

                for (int a = 0; a < vx; a++) {

                    if (this->getRightX() > this->game->level.maxXPixel()) break;

                    if (this->collisionCheckX(Direction::Right)) {
                        vx = 0;
                        break;
                    }
                    else { 

                        this->x++;
                        if (vy == 0 && !isFalling()) {
                            this->frame = (this->frame + 1) % 4;
                        }

                    }

                }

            }

            if (vx < 0) { // Left

                for (int a = 0; a > vx; a--)  {

                    if (x < this->game->level.minXPixel() + 1) break;

                    if (this->collisionCheckX(Direction::Left)) {
                        vx = 0;
                        break;
                    }
                    else {

                        x--;
                        if (vy == 0 && !isFalling()) {
                            this->frame = (this->frame + 1) % 4;
                        }

                    }

                }

            }

            break;

    }

}

bool Sprite::jump() {

    if (this->collisionCheckY(Direction::Down)) { 
        this->vy = -5; 
        this->jumpBoost = 0;
        return true; 
    }

    return false;

}

void Sprite::draw() {

    switch (this->getType()) {

       case ObjectTypes::Player:

            if (this->isFalling()) {
                Sprites::drawExternalMask(x - this->game->camera.x - 2, y - 1 - this->game->camera.y, Images::Player_Jumping, Images::Player_Jumping_Mask, this->facing == Direction::Right, this->facing == Direction::Right);
            }
            else if (this->vx == 0 && this->vy == 0) {
                Sprites::drawExternalMask(x - this->game->camera.x - 2, y - 1 - this->game->camera.y, (const uint8_t*)pgm_read_ptr(&Images::Player_Idle[this->getFrame()]), (const uint8_t*)pgm_read_ptr(&Images::Player_Idle_Masks[this->getFrame()]), this->facing == Direction::Right, this->facing == Direction::Right);
            }
            else {
                Sprites::drawExternalMask(x - this->game->camera.x - 2, y - 1 - this->game->camera.y, (const uint8_t*)pgm_read_ptr(&Images::Player_Images[this->getFrame()]), (const uint8_t*)pgm_read_ptr(&Images::Player_Masks[this->getFrame()]), this->facing == Direction::Right, this->facing == Direction::Right);
            }
            break;

        case ObjectTypes::Skull:
        case ObjectTypes::Spider:
            Sprites::drawExternalMask(x - this->game->camera.x, y - this->game->camera.y, this->spriteImg, this->spriteMask, this->facing == Direction::Right, this->facing == Direction::Right);
            break;

        case ObjectTypes::Fireball:
            if (this->game->mapNumber % 2 == 0) {

                Sprites::drawExternalMask(x - this->game->camera.x, y - this->game->camera.y, this->spriteImg, this->spriteMask, this->vy > 0, this->vy > 0);

            }
            else {

                if (this->vy > 0) {
                    Sprites::drawExternalMask(x - this->game->camera.x, y - this->game->camera.y, Images::Pirahna_Down, Images::Pirahna_Down_Mask, arduboy->getFrameCountHalf(8), 0);
                }
                else {
                    Sprites::drawExternalMask(x - this->game->camera.x, y - this->game->camera.y, Images::Pirahna_Up, Images::Pirahna_Up_Mask, arduboy->getFrameCountHalf(8), 0);
                }

            }
            break;            

        case ObjectTypes::Coin:

            if (this->autoExpire > 20 || (this->autoExpire / 4) % 2 == 0) {
                Sprites::drawPlusMask(x +2 - this->game->camera.x, y +2 - this->game->camera.y, Images::Coins, arduboy->getFrameCount(16) / 4);
            }
            break;

        case ObjectTypes::Health:

            if (this->autoExpire > 20 || (this->autoExpire / 4) % 2 == 0) {
                Sprites::drawExternalMask(x - this->game->camera.x, y - this->game->camera.y, this->spriteImg, this->spriteMask, 0, 0);
            }
            break;

        default:

            Sprites::drawExternalMask(x - this->game->camera.x, y - this->game->camera.y, this->spriteImg, this->spriteMask, 0, 0);
            break;
            
    }

}

uint8_t Sprite::getFrame() {

    if (this->vx == 0 && this->vy == 0 && this->frame >= 3) this->frame = 0;
    return this->frame;

}

Rect Sprite::getRect() {

    switch (this->getType()) {

        case ObjectTypes::Player:
            return Rect { this->getLeftX(), this->getTopY(), 9, this->getHeight()};

        case ObjectTypes::Fireball:
            return Rect { this->getLeftX() + 2, this->getTopY() + 2, this->getWidth() - 4, this->getHeight() - 4};

        default:
            return Rect { this->getLeftX(), this->getTopY(), this->getWidth(), this->getHeight()};

    }

}
