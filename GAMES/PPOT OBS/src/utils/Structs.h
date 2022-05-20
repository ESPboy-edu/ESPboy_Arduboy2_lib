#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"

struct SplashScreenVars {

    uint8_t delay = 0;

};

struct TitleScreenVars {

    uint8_t counter = 0;
    TitleMode mode = TitleMode::OBS;
    uint8_t panel = 0;
    uint8_t index = 0;
    bool skipTypewriter = false;

    void reset() {

        this->mode = TitleMode::OBS;
        this->counter = 0;
        this->panel = 0;
        this->index = 0;
        this->skipTypewriter = false;

    }

};

struct Asteroid : public Point {

    uint8_t type = 0;
    
};

struct Bullet : public Point {

    uint8_t hitCount = 0;
    uint8_t muzzleIndex = 0;
    
    HitObject hitObject = HitObject::None;
    
    void reset() {
        this->x = -1;
        this->hitCount = 0;
        this->hitObject = HitObject::None;
    }

};

struct Bullets {

    Bullet bullets[Constants::Bullet_Count];
    
    void reset() {

        for (Bullet &bullet : this->bullets) {

            bullet.reset();

        }

    }

    uint8_t getInactiveBullet() {

        for (uint8_t i = 0; i < Constants::Bullet_Count; i++) {

            Bullet &bullet = this->bullets[i];

            if (bullet.x == -1) {

                return i;

            }

        }

        return Constants::Bullet_None;
    }

};

struct Enemy : public Point {

    bool active = false;
    Path path = Path::Small;
    Motion motion = Motion::Slow;

    uint8_t pathCount = 0; 
    uint8_t yOffset = 0;         
    uint8_t explodeCounter = 0;
        
    bool updateExplosion() {

        if (this->explodeCounter > 0) {

            this->explodeCounter--;

            if (this->explodeCounter == 0) {

                this->explodeCounter = 0;
                return true;

            }

        }

        return false;

    }

    bool getActive() {

        return this->active;

    }

};

struct Player {

    uint8_t y = 26;
    uint16_t score = 0;

    uint16_t health = (17 * Constants::Health_Factor) - 1;
    uint8_t explodeCounter = 0;

    Direction direction = Direction::None;

    bool updateExplosion() {

        if (this->explodeCounter > 0) {

            this->explodeCounter--;

            if (this->explodeCounter == 0) {

                this->explodeCounter = 0;
                return true;

            }

        }

        return false;

    }

    void reset() {

        this->y = 26;
        this->score = 0;
        this->direction = Direction::None;
        this->health = (17 * Constants::Health_Factor) - 1;
        this->explodeCounter = 0;

    }

};
