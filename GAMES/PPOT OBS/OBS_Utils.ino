#include <Arduboy2.h>

// Launch a large Asteroid making sure there is no overlap ..

void launchLargeAsteroid(Asteroid &asteroid) {

    while (true) {

        asteroid.x = 128 + random(0, 96);
        asteroid.y = random(0, 46);
        asteroid.type = random(0, 2);

        Rect asteroid1_Rect = { asteroid.x, asteroid.y, 19, 19 };

        bool collision = false;

        for (Asteroid &largeAsteroid : largeAsteroids) {

            if (asteroid.x != largeAsteroid.x || asteroid.y != largeAsteroid.y) {

                Rect asteroid2_Rect = { largeAsteroid.x - 4, largeAsteroid.y - 4, 27, 27 };

                if (arduboy.collide(asteroid1_Rect, asteroid2_Rect)) {

                    collision = true;
                    break;

                }

            }

        }

        if (!collision) break;                    

    }

}

// Launch a large Asteroid making sure there is no overlap ..

void launchEnemy(Enemy &enemy) {

    while (true) {

        enemy.x = 128 + random(0, 96);
        enemy.y = random(0, 46);
        enemy.pathCount = random(6, 12);
        enemy.motion = static_cast<Motion>(random(0, 2));
        enemy.path = static_cast<Path>(random(1, 3));
        enemy.active = true;

        switch (enemy.path) {

            case Path::Large:
                enemy.yOffset = 0;
                break;

            case Path::Medium:
                enemy.yOffset = random(0, 20);
                break;

            case Path::Small:
                enemy.yOffset = random(0, 40);
                break;
            
        }


        Rect enemy_Rect = { enemy.x, enemy.y, 6, 9 };

        bool collision = false;


        // Check for overlap with large asteroids ..
        
        for (Asteroid &largeAsteroid : largeAsteroids) {

            Rect asteroid_Rect = { largeAsteroid.x - 4, largeAsteroid.y - 4, 27, 27 };

            if (arduboy.collide(enemy_Rect, asteroid_Rect)) {

                collision = true;
                break;

            }

        }


        // Check for overlap with other enemies ..
        
        if (!collision) {
            
            for (Enemy &enemy2 : enemies) {

                if (enemy.x != enemy2.x || enemy.y != enemy2.y) {

                    Rect enemy_Rect2 = { enemy2.x - 2, enemy2.y - 2, 8, 11 };

                    if (arduboy.collide(enemy_Rect, enemy_Rect2)) {

                        collision = true;
                        break;

                    }

                }

            }

        }

        if (!collision) break;                    

    }

}


/* ----------------------------------------------------------------------------
 *  Detect a collision between two separate images ..
 *  
 */
bool collide(int16_t x1, int16_t y1, const uint8_t *img1, int16_t x2, int16_t y2, const uint8_t *img2) {

    #define IMG_DATA_OFFSET 2

    uint8_t w1 = pgm_read_byte(&img1[0]);
    uint8_t h1 = pgm_read_byte(&img1[1]);
    uint8_t w2 = pgm_read_byte(&img2[0]);
    uint8_t h2 = pgm_read_byte(&img2[1]);



    // Do the images overlap at all ? 

    if (!(x2       >= x1 + w1 ||
          x2 + w2  <= x1      ||
          y2       >= y1 + h1 ||
          y2 + h2  <= y1)) {


        // Determine overlapping rectangle between the two images ..

        uint16_t overlap_left = max(x1, x2);
        uint16_t overlap_right = min(x1 + w1, x2 + w2);
        uint16_t overlap_top = max(y1, y2);
        uint16_t overlap_bottom = min(y1 + h1, y2 + h2);


        // The data in an image is defined left to right in bands (rows) of 8 pixel bands with the least 
        // significant bit at the top and the most significant at the bottom.  Consecutive rows of data 
        // describe vertical bits 8 - 15, 16 - 23 and so on.

        // Determine the portion of the first image that is constrained by the overlapping rectangle.  
        // The top row and bit describe how far from the top of the image the overlap begins, likewise the
        // bottom row and bit describe the lower range.
        //
        // Consider an image that is 24 x 24 pixels wide.  If the overlap started at coordinates (11, 11) 
        // and continued to (24, 24), then the top row would be equal to 1 (ie. the second row of 8 bits) 
        // and the top bits would be equal to 3 - the third pixel down.  The bottom row would be calculated
        // as 2 (ie. the third row) with the bottom bits calculated to 0. 

        int16_t img1_left = (overlap_left - x1);
        int16_t img1_right = (overlap_right - x1);
        int16_t img1_top_row = (overlap_top - y1) / 8;
        int16_t img1_top_bit = (overlap_top - y1) % 8;
        int16_t img1_bottom_row = (overlap_bottom - y1 - 1) / 8;
        int16_t img1_bottom_bit = (overlap_bottom - y1) % 8;

        int16_t img2_left = (overlap_left - x2);
        int16_t img2_top_row = (overlap_top - y2) / 8;
        int16_t img2_top_bit = (overlap_top - y2) % 8;
        int16_t img2_bottom_row = (overlap_bottom - y2 - 1) / 8;
        int16_t img2_bottom_bit = (overlap_bottom - y2) % 8;


        // Data is retrieved from each image using a separate index. The index is calculated by
        // multiplying the starting row by the image width and adding the left column of the overlapping
        // area.  An additional two bytes are added to account for the image height and width that prefix
        // image data.

        int16_t i1 = (img1_top_row * w1) + img1_left + IMG_DATA_OFFSET;
        int16_t i2 = (img2_top_row * w2) + img2_left + IMG_DATA_OFFSET;

        while (true) {


            // Retrieve the byte of data from the current column.  If the overlap is less than one row high and
            // the bottom bit is not zero, then we need to mask off the lower bits as they are out of range ..

            uint16_t d1 = pgm_read_byte(&img1[i1]) & (img1_top_row == img1_bottom_row && img1_bottom_bit != 0 ? pgm_read_byte(&lookup[img1_bottom_bit]) : 0xFF);
            uint16_t d2 = pgm_read_byte(&img2[i2]) & (img2_top_row == img2_bottom_row && img2_bottom_bit != 0 ? pgm_read_byte(&lookup[img2_bottom_bit]) : 0xFF);


            // If we are not at the last row of the overlap, retrieve the byte if data exactly below the one
            // retrieved in the last step.  The two bytes form an upper and lower byte of a 16 pixel range which
            // we will trim down to 8 bits per image for comparison.  This is done as the top pixel per image may
            // not be zero and hence to compare a contiguous 8 bits we need to harvest them from two rows ..

            if (img1_top_bit > 0 && img1_top_row < img1_bottom_row) { 
                d1 = d1 | ((pgm_read_byte(&img1[i1 + w1]) & (img1_top_row + 1 == img1_bottom_row ? pgm_read_byte(&lookup[img1_bottom_bit]) : 0xFF )) << 8); 
            }

            if (img2_top_bit > 0 && img2_top_row < img2_bottom_row) { 
                d2 = d2 | ((pgm_read_byte(&img2[i2 + w2]) & (img2_top_row + 1 == img2_bottom_row ? pgm_read_byte(&lookup[img2_bottom_bit]) : 0xFF )) << 8); 
            }


            // Finally, we bit shift the result if necessary and 'prune' to 8 bits ..

            d1 = (d1 >> img1_top_bit ) & 0xFF;
            d2 = (d2 >> img2_top_bit ) & 0xFF;


            // If there has been a collision, then we can exit out of here!  This algorithm checks collisions by 
            // scanning for collisions left to right then row by row.  Collisions detected near the top-left hand 
            // corner will be found quickly compared to a collision in the lower right-hand corner ..

            if ((d1 & d2) > 0) { 
                return true;
            }


            // Increase the column index for both images.  If we have exceeded the maximum
            // width of the image, then wrap to the next row ..

            if (i1 < (img1_top_row * w1) + img1_right + IMG_DATA_OFFSET) {
                ++i1;
                ++i2;
            }
            else {

                if (img1_top_row < img1_bottom_row) {

                    ++img1_top_row;
                    ++img2_top_row;
                    i1 = (img1_top_row * w1) + img1_left + IMG_DATA_OFFSET;
                    i2 = (img2_top_row * w2) + img2_left + IMG_DATA_OFFSET;

                }
                else {

                    return false;

                }

            }

        }

    }

    return false;

}

void moveRenderStarfield() {

    for (Point &star : starfield) {

        if (arduboy.getFrameCount(6) == 0) {

            star.x--;

            if (star.x == -1) {
                star.x = 128;
                star.y = random(0, 64);
            }

        }

        arduboy.drawPixel(star.x + xOffset, star.y + yOffset);
        
    }

}

void moveRenderSmallAsteroids(bool alternate) {


    // Move and render small asteroids ..

    for (uint8_t i = 0; i < Constants::SmallAsteroid_Size; i++) {

        Asteroid &smallAsteroid = smallAsteroids[i];

        if (arduboy.getFrameCount(3) == 0) {

            smallAsteroid.x--;

            if (smallAsteroid.x == -9) {
                smallAsteroid.x = 128 + random(0, 96);
                smallAsteroid.y = random(0, 56);
            }

        }

        if (!alternate || (i % 2 == 0)) {
            Sprites::drawExternalMask(smallAsteroid.x + xOffset, smallAsteroid.y + yOffset, Images::SmallAsteroid, Images::SmallAsteroid_Mask, 0, 0);
        }
        
    }

}

void moveRenderLargeAsteroids(bool alternate) {

    for (uint8_t i = 0; i < Constants::LargeAsteroid_Size; i++) {
    
        Asteroid &largeAsteroid = largeAsteroids[i];

        if (arduboy.getFrameCount(2) == 0) {

            largeAsteroid.x--;

            if (largeAsteroid.x == -19) {

                launchLargeAsteroid(largeAsteroid);

            }

        }


        if (!alternate || (i % 2 == 0)) {
            Sprites::drawExternalMask(largeAsteroid.x + xOffset, largeAsteroid.y + yOffset, Images::BigAsteroid[largeAsteroid.type], Images::BigAsteroid_Mask[largeAsteroid.type], 0, 0);
        }
        
    }

}