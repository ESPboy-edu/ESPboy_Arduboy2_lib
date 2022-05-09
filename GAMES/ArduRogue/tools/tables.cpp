#include "../game.hpp"

#include <stdio.h>

// dummy platform
void wait() {}
uint8_t wait_btn() { return 0; }
void seed() {}
void paint_offset(uint8_t, bool) {}
uint8_t read_persistent(uint16_t) { return 0; }
void update_persistent(uint16_t, uint8_t) {}
void flush_persistent() {}

int main()
{
    {
        int max_att = 19;
        int max_def = 19;
        printf("<table>\n");
        printf("<tr><td /><td colspan=\"%d\" align=\"center\"><b>Defense Roll</b></td></tr>\n", max_def + 1);
        printf("<tr align=\"center\"><td><b>Damage Roll</b></td>");
        for(int def = 0; def <= max_def; ++def)
            printf("<td><b>%d</b></td>", def);
        printf("</tr>\n");
        for(int att = 0; att <= max_att; ++att)
        {
            printf("<tr align=\"right\"><td><b>%d</b></td>", att);
            for(int def = 0; def <= max_def; ++def)
            {
                int dam = calc_hit_dam_post_mod((uint8_t)att, (uint8_t)def);
                printf("<td>%d</td>", dam);
            }
            printf("</tr>\n");
        }
        printf("</table>\n");
    }

    printf("\n\n");

    {
        int max_dex = 14;
        printf("<table>\n");
        printf("<tr><td /><td colspan=\"%d\" align=\"center\"><b>Defender Dexterity</b></td></tr>\n", max_dex + 1);
        printf("<tr align=\"center\"><td><b>Attacker Dexterity</b></td>");
        for(int dex = 0; dex <= max_dex; ++dex)
            printf("<td><b>%d</b></td>", dex);
        printf("</tr>\n");
        for(int att = 0; att <= max_dex; ++att)
        {
            printf("<tr align=\"right\"><td><b>%d</b></td>", att);
            for(int def = 0; def <= max_dex; ++def)
            {
                double fd = double(def);
                double fa = double(att) * 3 + fd + 1;
                int f = int((1.0 - fd / fa) * 100.0 + 0.5);
                printf("<td>%d%%</td>", f);
            }
            printf("</tr>\n");
        }
        printf("</table>\n");
    }

    printf("\n\n");

    {
        int max_dex = 14;
        printf("<table>\n");
        printf("<tr><td /><td colspan=\"%d\" align=\"center\"><b>Defender Dexterity</b></td></tr>\n", max_dex + 1);
        printf("<tr align=\"center\"><td><b>Attacker Dexterity</b></td>");
        for(int dex = 0; dex <= max_dex; ++dex)
            printf("<td><b>%d</b></td>", dex);
        printf("</tr>\n");
        for(int att = 0; att <= max_dex; ++att)
        {
            printf("<tr align=\"right\"><td><b>%d</b></td>", att);
            for(int def = 0; def <= max_dex; ++def)
            {
                double fd = double(def);
                double fa = double(att / 2) + fd + 1;
                int f = int((1.0 - fd / fa) * 100.0 + 0.5);
                printf("<td>%d%%</td>", f);
            }
            printf("</tr>\n");
        }
        printf("</table>\n");
    }

    return 0;
}
