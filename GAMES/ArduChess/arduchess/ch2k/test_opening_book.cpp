#include <stdio.h>

#include "ch2k.hpp"

int main()
{
    ch2k::game g;

    {
        g.new_game();

        g.execute_move(ch2k::move{ 0x61, 0x51 });
        g.execute_move(ch2k::move{ 0x06, 0x25 });

        uint8_t x[ch2k::EEPROM_GAME_DATA_SIZE];

        g.save_game_data(x);

        g.new_game();
        g.load_game_data(x);
    }

    for(int n = 0; n < 20; ++n)
    {
        g.new_game();
        while(g.gd.opening_index_ > 0)
        {
            g.ai_move();
            g.execute_move(g.best_);
            printf(" %5s [%4d]", g.best_.uci_str().c_str(), (int)g.gd.opening_index_);
        }
        printf("\n");
    }


    return 0;
}
