#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include <vector>
#include <algorithm>

struct vert { int8_t x, y, z; };
struct face { uint8_t i0, i1, i2, pt; };

int main(int argc, char** argv)
{
    if(argc < 4) return 1;

    FILE* f = fopen(argv[1], "r");
    if(!f)
        return 1;
    
    uint8_t pat = 0;

    std::vector<vert> verts;
    std::vector<face> faces;
    while(!feof(f))
    {
        int c = fgetc(f);
        if(c == 'v')
        {
            float x = 0;
            float y = 0;
            float z = 0;
            int n = fscanf(f, " %f %f %f\n", &x, &y, &z);
            if(n != 3)
                goto fail;
            int8_t ix = int8_t(roundf(x * 4));
            int8_t iy = int8_t(roundf(y * 4));
            int8_t iz = int8_t(roundf(z * 4));
            verts.push_back({ ix, iy, iz });
        }
        else if(c == 'f')
        {
            int i0 = 0;
            int i1 = 0;
            int i2 = 0;
            int n = fscanf(f, " %d %d %d\n", &i0, &i1, &i2);
            i0 -= 1;
            i1 -= 1;
            i2 -= 1;
            if(n != 3)
                goto fail;
            uint8_t pt = 0;
            auto const& v0 = verts[i0];
            auto const& v1 = verts[i1];
            auto const& v2 = verts[i2];
            faces.push_back({ uint8_t(i0), uint8_t(i1), uint8_t(i2), pat });
        }
        else if(c == 'u')
        {
            while(!feof(f) && (c = fgetc(f)) != '\n')
                if(c >= '0' && c <= '5')
                    pat = uint8_t(c - '0');
        }
        else
        {
            while(!feof(f) && fgetc(f) != '\n')
                ;
        }
    }

    fclose(f);

    // sort by pattern
    std::sort(
        faces.begin(), faces.end(),
        [](face a, face b) { return a.pt < b.pt; });
    int pats[5] = {};
    for(int i = 0; i < 5; ++i)
        for(auto f : faces)
            if(f.pt == i)
                ++pats[i];

    f = fopen(argv[2], "w");
    if(!f) return 1;
    fprintf(f, "#pragma once\n\n");
    fprintf(f, "static constexpr int8_t LEVEL_%s_VERTS[%d] PROGMEM =\n{\n",
        argv[3], (int)verts.size() * 3);
    for(size_t i = 0; i < verts.size(); ++i)
    {
        fprintf(f, "%s%+4d, %+4d, %+4d,%c",
            i % 4 == 0 ? "    " : "",
            (int)verts[i].x, (int)verts[i].y, (int)verts[i].z,
            i % 4 == 3 ? '\n' : ' ');
    }
    fprintf(f, "\n};\n\n");
    fprintf(f, "static constexpr uint8_t LEVEL_%s_FACES[%d] PROGMEM =\n{\n",
        argv[3], (int)faces.size() * 3);
    for(size_t i = 0; i < faces.size(); ++i)
    {
        fprintf(f, "%s%3d, %3d, %3d,%c",
            i % 4 == 0 ? "    " : "",
            (int)faces[i].i0, (int)faces[i].i1, (int)faces[i].i2,
            i % 4 == 3 ? '\n' : ' ');
    }
    fprintf(f, "\n};\n\n");

    fprintf(f, "#define LEVEL_%s_NUM_FACES { ", argv[3]);
    for(int i = 0; i < 5; ++i)
        fprintf(f, "%d, ", pats[i]);
    fprintf(f, "}\n");

    fclose(f);

    return 0;

fail:
    fprintf(stderr, "Failed to convert obj\n");
    fclose(f);
    return 1;
}
