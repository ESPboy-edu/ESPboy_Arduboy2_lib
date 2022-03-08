#define CH2K_USE_STD_ARRAY 1
#define CH2K_TUNABLE_EVAL 1
#include "ch2k.hpp"

#include <stdio.h>

#include <fstream>
#include <string>
#include <vector>

#include <cmath>

struct texel_test
{
    std::string fen;
    double result;
};
static std::vector<texel_test> tests;
static ch2k::eval_settings_union e;
static ch2k::eval_settings_union grad;
static constexpr int const NS = sizeof(ch2k::eval_settings_union) / sizeof(double);
static ch2k::game g;

static double run_eval(double k)
{
    double t = 0.0;
    memset(&grad, 0, sizeof(grad));
    for(auto const& test : tests)
    {
        g.load_fen(test.fen.c_str());
        memset(&g.eval_gradient_, 0, sizeof(g.eval_gradient_));
        double f = g.eval();
        double d = 1.0 + pow(10.0, -k * f / 400.0);
        double s = 1.0 / d - test.result;
        for(int i = 0; i < NS; ++i)
            grad.data[i] += g.eval_gradient_.data[i] * s;
        t += (s * s);
    }
    return t / tests.size();
}

static void write_values()
{
    FILE* f = fopen("tuned_eval_settings.txt", "w");

    fprintf(f, "static eval_settings_union const DEFAULT_EVAL_SETTINGS PROGMEM =\n");
    fprintf(f, "{\n    {");

    int N = sizeof(ch2k::eval_settings_union) / sizeof(double);
    for(int n = 0; n < N; ++n)
    {
        if(n % 4 == 0) fprintf(f, "\n        ");
        fprintf(f, "%+4d, ", int(round(e.data[n])));
    }

    fprintf(f, "\n    }\n};");

    fclose(f);
}

int main()
{
    memcpy(&e, &ch2k::DEFAULT_EVAL_SETTINGS, sizeof(e));
    g.custom_eval_settings_ = &e;

    {
        printf("Reading \"%s\"...\n", TEXELFILE);
        std::ifstream f(TEXELFILE, std::ios::in);
        std::string line;
        int w = 0, b = 0, d = 0;
        while(!f.eof())
        {
            std::getline(f, line);
            auto x = line.find("\"");
            if(x == std::string::npos) break;
            tests.resize(tests.size() + 1);
            auto& test = tests.back();
            test.fen = line.substr(0, x);

            if(line[x + 3] == '0')
                ++w, test.result = 1.f;
            else if(line[x + 3] == '1')
                ++b, test.result = 0.f;
            else if(line[x + 3] == '2')
                ++d, test.result = 0.5f;

            //if(tests.size() == 1000) break;
        }
        printf("DONE: read %d tests\n", (int)tests.size());
        printf("   White: %d\n", w);
        printf("   Black: %d\n", b);
        printf("   Draw : %d\n", d);
    }

    double k = 1.75, mv;
    //if(0)
    {
        printf("Finding best K\n");
        double const gr = (sqrt(5.0) + 1) / 2;
        double const tol = 1e-5;
        double a = 0.0;
        double b = 4.0;
        double c = b - (b - a) / gr;
        double d = a + (b - a) / gr;
        int i = 0;
        printf("   iteration %2d: a = %+8.5f, b = %+8.5f\n", i, a, b);
        while(std::abs(c - d) > tol)
        {
            double ce = run_eval(c);
            double de = run_eval(d);
            if(ce < de)
                b = d;
            else
                a = c;
            c = b - (b - a) / gr;
            d = a + (b - a) / gr;
            printf("   iteration %2d: a = %+8.5f, b = %+8.5f (%.6f, %.6f)\n", ++i, a, b, ce, de);
        }
        k = (a + b) / 2;
        mv = run_eval(k);
        printf("   final       : K = %+8.5f, e = %f\n", k, mv);
    }
    //k = 1.0;

    for(int iter = 1; ; ++iter)
    {
        write_values();
        printf("Iteration %4d: ", iter); fflush(stdout);
        printf("%f\n", run_eval(k));
        for(int i = 0; i < NS; ++i)
        {
            e.data[i] -= grad.data[i] * (10.0 + abs(e.data[i])) / tests.size();
            if(e.data[i] < -1023) e.data[i] = -1023;
            if(e.data[i] > +1023) e.data[i] = +1023;
        }
    }
    write_values();

    return 0;
}
