// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 MostlyBuilds

#include "Artwork.h"

namespace MostlyLogic {
namespace Artwork {

// Sprite data starts with width and height. Each frame then stores vertical
// 8-pixel columns from left to right, one display page at a time. Keep the
// encoded frames beside these pixel previews so the artwork remains editable.
// clang-format off

/*
 * Two 8x8 deflector frames: X followed by dot.
 *
 * ........  ........
 * .######.  .######.
 * .##..##.  .#....#.
 * .#.##.#.  .#.##.#.
 * .#.##.#.  .#.##.#.
 * .##..##.  .#....#.
 * .######.  .######.
 * ........  ........
 */
const uint8_t DEFLECTOR_SPRITES[] PROGMEM = {
    8, 8,
    0x00, 0x7E, 0x66, 0x5A, 0x5A, 0x66, 0x7E, 0x00,
    0x00, 0x7E, 0x42, 0x5A, 0x5A, 0x42, 0x7E, 0x00,
};
static_assert(sizeof(DEFLECTOR_SPRITES) == 2 + 2 * 8,
              "Expected two 8x8 deflector frames");

/*
 * 8x8 splitter.
 *
 * ........
 * .######.
 * .#..#.#.
 * .#..#.#.
 * .######.
 * .#..#.#.
 * .######.
 * ........
 */
const uint8_t SPLITTER_SPRITE[] PROGMEM = {
    8, 8,
    0x00, 0x7E, 0x52, 0x52, 0x7E, 0x52, 0x7E, 0x00,
};
static_assert(sizeof(SPLITTER_SPRITE) == 2 + 8,
              "Expected one 8x8 splitter frame");

/*
 * Two 13x10 speaker frames: sound on followed by sound off.
 *
 * ......#......  ......#......
 * .....##......  .....##.#...#
 * ....#.#......  ....#.#..#.#.
 * ####..#.#....  ####..#..#.#.
 * ###...#..#...  ###...#...#..
 * ###...#..#...  ###...#...#..
 * ####..#.#....  ####..#..#.#.
 * ....#.#......  ....#.#..#.#.
 * .....##......  .....##.#...#
 * ......#......  ......#......
 */
const uint8_t SPEAKER_SPRITES[] PROGMEM = {
    13, 10,
    0x78, 0x78, 0x78, 0x48, 0x84, 0x02, 0xFF, 0x00, 0x48, 0x30, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x78, 0x78, 0x78, 0x48, 0x84, 0x02, 0xFF, 0x00, 0x02, 0xCC, 0x30, 0xCC, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
};
static_assert(sizeof(SPEAKER_SPRITES) == 2 + 2 * 13 * 2,
              "Expected two 13x10 speaker frames");

// clang-format on

}  // namespace Artwork
}  // namespace MostlyLogic
