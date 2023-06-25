#pragma once

#include <Arduboy2.h>
#include "../utils/Constants.h"
#include "../../fxdata/fxdata.h"
#include "Level.h"

extern void setTitleFrame(TitleFrameIndex index/*, uint8_t frame = 0*/);

struct TitleScreenVars {
    
    #ifdef POP_OR_POA
        uint8_t counter;
    #endif
    
    public:

        TitleScreenOptions option;

    void reset(uint8_t frameIndex) {

        this->option = TitleScreenOptions::Play;
        setTitleFrame((TitleFrameIndex)((uint8_t)TitleFrameIndex::Intro_PoP_Frame_NC + frameIndex));

    }

};
