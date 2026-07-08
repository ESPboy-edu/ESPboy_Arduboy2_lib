#include "MathsFunctions.h"

int16_t estimateDistance(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int h = (dx > dy) ? dx : dy;
    int l = (dx > dy) ? dy : dx;

    return h + (l >> 2);
}

int16_t estimateMagnitude(int16_t dx, int16_t dy)
{
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;

    int h = (dx > dy) ? dx : dy;
    int l = (dx > dy) ? dy : dx;

    return h + (l >> 2);
}

uint8_t calculateFacingDirection(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1; 

    int absDx = (dx < 0) ? -dx : dx;
    int absDy = (dy < 0) ? -dy : dy;

    if (absDx == 0 && absDy == 0)
        return NoDirection;

    if (dx >= 0) 
    {
        if (dy >= 0) 
        {
            return (absDx > 2 * absDy) ? East :
                (absDy > 2 * absDx) ? South : SouthEast;
        }
        else 
        {
            return (absDx > 2 * absDy) ? East :
                (absDy > 2 * absDx) ? North : NorthEast;
        }
    }
    else
    {
        if (dy >= 0) 
        {
            return (absDx > 2 * absDy) ? West :
                (absDy > 2 * absDx) ? South : SouthWest;
        }
        else 
        {
            return (absDx > 2 * absDy) ? West :
                (absDy > 2 * absDx) ? North : NorthWest;
        }
    }
}