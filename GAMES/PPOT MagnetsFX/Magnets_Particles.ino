#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"

#ifdef PARTICLES

void launchParticles(int16_t x, int16_t y) {

    for (int i = 0; i < Constants::ParticlesMax; i++) {

        if (!particles[i].isActive()) {

            particles[i].setX(x);
            particles[i].setY(y);
            particles[i].setVelX(random(10) - 4);
            particles[i].setVelY(random(10) - 3);
            particles[i].setCounter(random(static_cast<uint8_t>(10), static_cast<uint8_t>(46)));
            particles[i].setSize(random(static_cast<uint8_t>(1), static_cast<uint8_t>(4)));
            particlesNeedRendering = true;

        }
    
    }

}


void updateAndRenderParticles(uint8_t frameCount) {

    bool update = (frameCount % 3) == 0;

    if (particlesNeedRendering) {
            
        particlesNeedRendering = false;

        for (int i = 0; i < Constants::ParticlesMax; i++) {

            if (particles[i].isActive() ) { 

                if (update) particles[i].update();

                uint8_t pSize = particles[i].getSize();
                particlesNeedRendering = true;

                // if (pSize == 1) {
                    ArduboyG::drawPixel(particles[i].getX(), particles[i].getY(), WHITE);
                // } 
                // else {
                //     ArduboyG::drawRect(particles[i].getX(), particles[i].getY(), pSize, pSize, WHITE);
                // }

            }

        }

    }

}

#endif