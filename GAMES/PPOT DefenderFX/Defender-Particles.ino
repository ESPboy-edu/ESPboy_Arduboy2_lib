#include <ArduboyFX.h>  
#include "src/utils/ArduboyG.h"
#include "src/utils/Constants.h"
#include "src/utils/Enums.h"
#include "fxdata/fxdata.h"
#include "src/entities/Entities.h"
#include "src/utils/SpritesU.hpp"


void launchParticles(int16_t x, int16_t y) {

    for (int i = 0; i < Constants::ParticlesMax; i++) {

        particles[i].setX(x);
        particles[i].setY(y);
        particles[i].setVelX(random(-4, 5));
        particles[i].setVelY(random(-1, 6));
        particles[i].setCounter(random(10, 46));
        particles[i].setSize(random(1, 3));
    
    }

    particlesNeedRendering = true;

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

                if (pSize == 1) {
                    ArduboyG::drawPixel(particles[i].getX(), particles[i].getY(), WHITE);
                } 
                else {
                    ArduboyG::drawRect( particles[i].getX(), particles[i].getY(), pSize, pSize, WHITE);
                }

            }

        }

    }

}

