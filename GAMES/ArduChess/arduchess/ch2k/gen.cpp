#include <stdio.h>

#include <array>

#include "chess2k.hpp"

#define COLS 8

static std::array<capt_code, 0x99 + 1 + 0x77> capts;
static std::array<pos_delta, 0x77 + 1 + 0x77> vecs;

static void set_capt(pos_delta i, capt_code c) { capts[0x99 + i] |= c; }
static void set_vec(pos_delta i, pos_delta pd) { vecs[0x77 + i] = pd; }

int main()
{
    for(auto& c : capts)
        c = 0;
    for(auto& v : vecs)
        v = 0;

    // contact captures
    set_capt(NORTH, C_FORW);
    set_capt(SOUTH, C_BACKW);
    set_capt(WEST, C_SIDE);
    set_capt(EAST, C_SIDE);
    set_capt(NORTHWEST, C_FDIAG);
    set_capt(NORTHEAST, C_FDIAG);
    set_capt(SOUTHWEST, C_BDIAG);
    set_capt(SOUTHEAST, C_BDIAG);

    for(int i = 0; i < 8; ++i)
    {
        set_capt(KNIGHT_DIRS[i], C_KNIGHT);
        set_vec(KNIGHT_DIRS[i], KNIGHT_DIRS[i]);
        // sliding captures
        pos_delta d = DIRS[i];
        set_vec(d, d);
        for(int j = 0; j < 6; ++j)
        {
            d += DIRS[i];
            if(i < 4)
                set_capt(d, C_ORTH);
            else
                set_capt(d, C_DIAG);
            set_vec(d, DIRS[i]);
        }
    }

    printf("capt_code pos_delta::capts[] PROGMEM =\n{");
    for(int i = 0; i < 0x99 + 1 + 0x77; ++i)
    {
        printf("%s0x%02x,",
            i % COLS ? " " : "\n    ",
            int(capts[i]));
    }
    printf("\n};\n\n");

    printf("pos_delta pos_delta::vecs[] PROGMEM =\n{");
    for(int i = 0; i < 0x77 + 1 + 0x77; ++i)
    {
        printf("%s%+3d,",
            i % COLS ? " " : "\n    ",
            int(vecs[i]));
    }
    printf("\n};\n\n");
}
