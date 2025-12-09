#pragma once

#include <FixedPoints.h>
#include <FixedPointsCommon.h>
using Fixed = SQ15x16;

namespace Constants {

    const uint8_t Z_Axis = 0;
    const uint8_t Y_Axis = 1;
    const uint8_t X_Axis = 2;

    const uint8_t Z_Axis_Offset = 5;
    const uint8_t Z_Axis_Height = 22;

    const uint8_t Cube_L_X = 96;
    const uint8_t Cube_L_Y = 18;

    const uint8_t Cube_R_X = 0;
    const uint8_t Cube_R_Y = 18;

    const uint8_t GameOverCounter_NoAction = 255;

    const Fixed Rotation_Speed = 0.08; 				// radians per frame
    const Fixed COS_ISO = 0.866;    				// cos(30°)
    const Fixed SIN_ISO = 0.5;      				// sin(30°)

};