#define CH2K_USE_STD_ARRAY 1

#include "ch2k.hpp"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_set>

int main()
{
    printf("Reading \"%s\"...\n", TEXELFILE);
    std::ifstream f(TEXELFILE, std::ios::in);
    std::unordered_set<std::string> fens;
    std::ofstream fout(POSFILE);
    std::string line;
    int w = 0, b = 0, d = 0;
    while(!f.eof())
    {
        std::getline(f, line);
        auto x = line.find("\"");
        if(x == std::string::npos) break;
        std::string fen = line.substr(0, x - 4);

        if(line[x + 3] != '2') continue;

        int mat =
            1 * std::count(fen.begin(), fen.end(), 'p') +
            1 * std::count(fen.begin(), fen.end(), 'P') +
            3 * std::count(fen.begin(), fen.end(), 'n') +
            3 * std::count(fen.begin(), fen.end(), 'N') +
            3 * std::count(fen.begin(), fen.end(), 'b') +
            3 * std::count(fen.begin(), fen.end(), 'B') +
            5 * std::count(fen.begin(), fen.end(), 'r') +
            5 * std::count(fen.begin(), fen.end(), 'R') +
            9 * std::count(fen.begin(), fen.end(), 'q') +
            9 * std::count(fen.begin(), fen.end(), 'Q') +
            0;

        if(mat >= 78)
            fens.insert(fen);
    }

    for(auto const& fen : fens)
        fout << fen << std::endl;

    return 0;
}
