#define CH2K_USE_STD_ARRAY 1
#define CH2K_MAX_MOVE_STACK 1
#include "ch2k.hpp"

#undef CH2K_NAMESPACE
#define CH2K_NAMESPACE ch2k_old
#include "ch2k_old.hpp"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include <fstream>
#include <vector>
#include <string>

#define NODE_LIMIT 2048
#define DEPTH_LIMIT 12

static ch2k::game g_new;
static ch2k_old::game g_old;

static int compete(std::string const& fen, bool new_is_white)
{
    g_new.load_fen(fen.c_str());
    g_old.load_fen(fen.c_str());

    g_new.max_nodes_ = g_old.max_nodes_ = NODE_LIMIT;
    g_new.max_depth_ = g_old.max_depth_ = DEPTH_LIMIT;

    g_new.contempt_ = g_old.contempt_ = 0;

    //g_old.max_nodes_ = 512;
    //g_old.max_depth_ = 1;
    //g_new.max_nodes_ = 8192;
    //g_new.contempt_ = 16;

    for(;;)
    {
        // check status of game
        auto status = g_new.check_status();
        if(status == ch2k::game::STATUS_MATED)
        {
            bool white_lost = g_new.gd.c_.is_white();
            return white_lost == new_is_white ? -1 : +1;
        }
        if(status > ch2k::game::STATUS_CHECK) return 0;

        // continue making moves
        bool whites_turn = g_new.gd.c_.is_white();
        if(whites_turn == new_is_white)
        {
            g_new.iterative_deepening();
            //printf("   %s\n", g_new.best_.uci_str().c_str());
            g_old.execute_move(*(ch2k_old::move*)&g_new.best_);
            g_new.execute_move(g_new.best_);
        }
        else
        {
            g_old.iterative_deepening();
            //printf("   %s\n", g_old.best_.uci_str().c_str());
            g_new.execute_move(*(ch2k::move*)&g_old.best_);
            g_old.execute_move(g_old.best_);
        }
    }
}

int main()
{
    printf("Loading starting positions from \"%s\"\n", POSFILE);
    std::vector<std::string> fens;
    {
        std::ifstream f(POSFILE);
        std::string fen;
        while(!f.eof())
        {
            std::getline(f, fen);
            fens.push_back(fen);
        }
    }
    printf("Loaded %u starting positions\n", unsigned(fens.size()));

    int total = 0, wins = 0, draws = 0, losses = 0;
    for(size_t n = 0; n < 10000 && n < fens.size(); ++n)
    {
        int r0 = compete(fens[n], true);
        total += r0; wins += (r0 == 1); draws += (r0 == 0); losses += (r0 == -1);
        int r1 = compete(fens[n], false);
        total += r1; wins += (r1 == 1); draws += (r1 == 0); losses += (r1 == -1);
        char c0 = r0 == 1 ? 'W' : r0 == 0 ? 'D' : 'L';
        char c1 = r1 == 1 ? 'W' : r1 == 0 ? 'D' : 'L';
        double winrate = (1.0 * wins + 0.5 * draws) / (wins + draws + losses);
        double elo = -(wins + draws > 0 ? 400.0 * log10(1.0 / winrate - 1.0) : 1000.0);
        printf("%5d: %-74s %c%c    %+4d  (ELO: %+8.2f)\n",
            int(n), fens[n].c_str(), c0, c1, total, elo);
    }
    printf("Total: %d (%d/%d/%d)", total, wins, draws, losses);

    printf("\nMax move stack: %d\n", (int)g_new.max_move_stack_);
}
