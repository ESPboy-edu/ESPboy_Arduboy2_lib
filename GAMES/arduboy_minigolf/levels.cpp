#include "game.hpp"

#include "levels/level_00.hpp"
#include "levels/level_01.hpp"
#include "levels/level_02.hpp"
#include "levels/level_03.hpp"
#include "levels/level_04.hpp"
#include "levels/level_05.hpp"
#include "levels/level_06.hpp"
#include "levels/level_07.hpp"
#include "levels/level_08.hpp"
#include "levels/level_09.hpp"
#include "levels/level_10.hpp"
#include "levels/level_11.hpp"
#include "levels/level_12.hpp"
#include "levels/level_13.hpp"
#include "levels/level_14.hpp"
#include "levels/level_15.hpp"
#include "levels/level_16.hpp"
#include "levels/level_17.hpp"

#include "levels/level_00_info.hpp"
#include "levels/level_01_info.hpp"
#include "levels/level_02_info.hpp"
#include "levels/level_03_info.hpp"
#include "levels/level_04_info.hpp"
#include "levels/level_05_info.hpp"
#include "levels/level_06_info.hpp"
#include "levels/level_07_info.hpp"
#include "levels/level_08_info.hpp"
#include "levels/level_09_info.hpp"
#include "levels/level_10_info.hpp"
#include "levels/level_11_info.hpp"
#include "levels/level_12_info.hpp"
#include "levels/level_13_info.hpp"
#include "levels/level_14_info.hpp"
#include "levels/level_15_info.hpp"
#include "levels/level_16_info.hpp"
#include "levels/level_17_info.hpp"

uint8_t leveli;
level_info const* current_level = 0;

#define LEVEL_GEOM(n__) { \
    /* verts     */ LEVEL_##n__##_VERTS, \
    /* faces     */ LEVEL_##n__##_FACES, \
    /* boxes     */ LEVEL_##n__##_BOXES, \
    /* num_verts */ sizeof(LEVEL_##n__##_VERTS) / 3, \
    /* num_faces */ LEVEL_##n__##_NUM_FACES, \
    /* num_boxes */ sizeof(LEVEL_##n__##_BOXES) / sizeof(phys_box), \
    /* ball_pos  */ LEVEL_##n__##_BALL_POS, \
    /* flag_pos  */ LEVEL_##n__##_FLAG_POS, \
    }

level_info const LEVELS[NUM_LEVELS] PROGMEM =
{
    LEVEL_GEOM(00),
    LEVEL_GEOM(01),
    LEVEL_GEOM(02),
    LEVEL_GEOM(03),
    LEVEL_GEOM(04),
    LEVEL_GEOM(05),
    LEVEL_GEOM(06),
    LEVEL_GEOM(07),
    LEVEL_GEOM(08),
    LEVEL_GEOM(09),
    LEVEL_GEOM(10),
    LEVEL_GEOM(11),
    LEVEL_GEOM(12),
    LEVEL_GEOM(13),
    LEVEL_GEOM(14),
    LEVEL_GEOM(15),
    LEVEL_GEOM(16),
    LEVEL_GEOM(17),
};

uint8_t const PARS[NUM_LEVELS] PROGMEM =
{
    2, 3, 3, 4, 4, 4, 5, 4, 4, 3, 4, 4, 5, 5, 4, 5, 6, 6,
};
