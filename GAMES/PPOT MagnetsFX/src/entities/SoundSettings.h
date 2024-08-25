
#pragma once

#include <Arduboy2.h>
#include "../utils/Enums.h"
#include "../utils/Enums.h"
#include "../../fxdata/fxdata.h"


struct SoundSettings {

    RenderSize renderSize = RenderSize::Auto;

    RenderSize getRenderSize()              { return this->renderSize; }

    void setRenderSize(RenderSize val)      { this->renderSize = val; }

};
