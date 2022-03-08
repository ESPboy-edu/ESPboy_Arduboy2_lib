#define CH2K_USE_STD_ARRAY 1
#include "ch2k.hpp"

#include <chrono>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#define FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
//#define FEN "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"
//#define FEN "r1bqkbnr/pppppppp/2n5/8/4P3/2N5/PPPP1PPP/R1BQKBNR b KQkq e3"
//#define FEN "4kb1r/p4pp1/2n3n1/6qp/4B3/6QP/brP1PP2/R4K1R b k - 1 12"
//#define FEN "bn6/k7/8/8/3K4/8/8/8 w - -"
//#define FEN "r4r2/p1q1n1kp/2n1ppp1/8/3P2N1/3BPP2/2Q2P1P/R3K2R w KQ - 0 19"
#define DEPTH 6

static uint64_t perft(ch2k::game& g, ch2k::move* m, int d)
{
    int n = g.gen_moves(m);
    assert(n != ch2k::game::GEN_MOVES_OUT_OF_MEM);
    if(d <= 1)
        return uint64_t(n);
    uint64_t r = 0;
    while(--n >= 0)
    {
        auto mv = m[n];
        g.do_move(mv);
        r += perft(g, m + n, d - 1);
        g.undo_move(mv);
    }
    return r;
}

int main()
{
    //uint64_t c[DEPTH];

    ch2k::game g;
    //g.load_fen(FEN);
    g.new_game();

    printf("%s\n", g.str().c_str());

#if 0
    auto t0 = clock();
    g.max_nodes_ = 2000;
    g.max_depth_ = 2;
    for(int n = 0; n < 1000; ++n)
        g.iterative_deepening();
    printf("best: %s\n", g.best_.uci_str().c_str());
    printf("nodes: %llu\n", uint64_t(g.nodes_));
    printf("depth: %llu\n", uint64_t(g.depth_));
    auto t1 = clock();
    printf("time: %f\n", (double(t1) - double(t0)) / CLOCKS_PER_SEC);
    return 0;
#endif

    int n = g.gen_moves(&g.mvs_[0]);
    uint64_t t = 0;
    for(int i = 0; i < n; ++i)
    {
        printf("%-5s ", g.mvs_[i].uci_str().c_str());
        if(DEPTH > 1)
        {
            g.do_move(g.mvs_[i]);
            uint64_t p = perft(g, &g.mvs_[n], DEPTH - 1);
            t += p;
            printf("%llu\n", p);
            g.undo_move(g.mvs_[i]);
        }
        else
        {
            ++t;
            printf("1\n");
        }
    }

    printf("\n%llu\n", t);

    return 0;
}
