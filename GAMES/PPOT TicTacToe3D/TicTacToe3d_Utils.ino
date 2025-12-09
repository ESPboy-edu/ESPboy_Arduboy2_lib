#include <Arduboy2.h>

void startGame() {

	resetBoard();

	// board[0][0][0] = 1;
	// board[0][0][1] = 2;
	// board[0][0][2] = 1;

	// board[1][1][0] = 1;
	// board[1][1][1] = 2;
	// board[2][0][1] = 1;

	// board[2][2][0] = 2;
	// board[2][2][1] = 2;
			
	playerOrComp = 0;
	gameState = GameState::NumberoFPlayers;
	gameOverCounter = Constants::GameOverCounter_NoAction;
	playAgainCursor = 0;
    middlePositionCount = 0;

	flashCrosses = false;
	flashNoughts = false;

}

void rotateBoardX(uint8_t board[3][3][3], int8_t direction) {

  uint8_t temp[3][3][3];

    // Copy board to temp
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            for (int z = 0; z < 3; z++) {
                temp[y][x][z] = board[y][x][z];
            }
        }
    }

    if (direction > 0) {

        // Forward rotation: 180 degrees around X-axis
        // Y levels flip: top (2) -> bottom (0), bottom (0) -> top (2)
        // Z coords flip: front (2) -> back (0), back (0) -> front (2)

        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                for (int z = 0; z < 3; z++) {
                    board[2 - y][x][2 - z] = temp[y][x][z];
                }
            }
        }
    } 
    else {

        // Reverse rotation: -180 degrees around X-axis (same as +180)
        // Result is the same as forward for 180 degree rotation

        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3; x++) {
                for (int z = 0; z < 3; z++) {
                    board[2 - y][x][2 - z] = temp[y][x][z];
                }
            }
        }
    }

}

// Rotate the board array 90 degrees around Y-axis (spinning left/right)
// direction: 1 for clockwise (when viewed from above)
//           -1 for counter-clockwise (when viewed from above)
void rotateBoardY(uint8_t board[3][3][3], int8_t direction) {
    uint8_t temp[3][3][3];
    
    // Copy board to temp
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            for (int z = 0; z < 3; z++) {
                temp[y][x][z] = board[y][x][z];
            }
        }
    }
    
    if (direction > 0) {

        // Clockwise rotation: 90 degrees around Y-axis
        // Rotating around Y-axis: X and Z coordinates transform

        for (int y = 0; y < 3; y++) {

            for (int x = 0; x < 3; x++) {

                for (int z = 0; z < 3; z++) {
                    board[y][z][2 - x] = temp[y][x][z];
                }

            }

        }

    } 
    else {

        // Counter-clockwise rotation: -90 degrees around Y-axis

        for (int y = 0; y < 3; y++) {

            for (int x = 0; x < 3; x++) {

                for (int z = 0; z < 3; z++) {
                    board[y][2 - z][x] = temp[y][x][z];
                }

            }

        }

    }

}


// Rotate a point around X axis (vertical rotation)
// Rotates around a center point

void rotateX(Fixed &y, Fixed &z, Fixed angle, Fixed centerY) {

    // Translate to origin
    y -= centerY;

    Fixed cosA = Fixed(cos(float(angle)));
    Fixed sinA = Fixed(sin(float(angle)));
    Fixed newY = y * cosA - z * sinA;
    Fixed newZ = y * sinA + z * cosA;
    y = newY;
    z = newZ;

    // Translate back
    y += centerY;

}

// Rotate a point around Y axis (horizontal rotation)
void rotateY(Fixed &x, Fixed &z, Fixed angle) {

    Fixed cosA = Fixed(cos(float(angle)));
    Fixed sinA = Fixed(sin(float(angle)));
    Fixed newX = x * cosA - z * sinA;
    Fixed newZ = x * sinA + z * cosA;
    x = newX;
    z = newZ;

}

// Rotate the grid around X axis (tilting forward/backward)
// direction: 1 for forward, -1 for reverse
void rotateGridX(int8_t direction) {

    rotationAngle += Constants::Rotation_Speed * direction;

    if (direction > 0 && rotationAngle >= Fixed(PI)) {

        rotateBoardX(board, -direction);
        xRotate = false;

    } 
    else if (direction < 0 && rotationAngle <= Fixed(-PI)) {

        rotateBoardX(board, -direction);
        xRotate = false;

    }

}

// Rotate the grid around Y axis (spinning left/right)
// direction: 1 for clockwise, -1 for counter-clockwise
void rotateGridY(int8_t direction) {

    rotationAngle += Constants::Rotation_Speed * direction;

    if (direction > 0 && rotationAngle >= Fixed(PI/2)) {
        rotateBoardY(board, -direction);
        yRotate = false;
    } 
    else if (direction < 0 && rotationAngle <= Fixed(-PI/2)) {
        rotateBoardY(board, -direction);
        yRotate = false;
    }

}

// 3D to 2D isometric projection
void isoProject(Fixed x, Fixed y, Fixed z, int16_t &screenX, int16_t &screenY) {
    // Isometric projection formula
    screenX = 64 + int16_t((x - z) * Constants::COS_ISO);
    screenY = 38 + int16_t((x + z) * Constants::SIN_ISO - y);
}

void resetBoard() {

	for (uint8_t z = 0; z < 3; z++) {

		for (uint8_t y = 0; y < 3; y++) {  

			for (uint8_t x = 0; x < 3; x++) {

				board[z][y][x] = Players::None;

			}

		}

	}

	cursor[0] = 0;
	cursor[1] = 0;
	cursor[2] = 0;

}

Players checkForWin() {
    
    // Check all possible 3-in-a-row combinations
    
    // 1. Check rows in each layer (9 rows total, 3 per layer) ..

    for (uint8_t z = 0; z < 3; z++) {

        for (uint8_t y = 0; y < 3; y++) {

            if (board[z][y][0] != Players::None &&
                board[z][y][0] == board[z][y][1] &&
                board[z][y][1] == board[z][y][2]) {
                return (Players) board[z][y][0];
            }

        }

    }
    

    // 2. Check columns in each layer (9 columns total, 3 per layer) ..

    for (uint8_t z = 0; z < 3; z++) {

        for (uint8_t x = 0; x < 3; x++) {

            if (board[z][0][x] != Players::None &&
                board[z][0][x] == board[z][1][x] &&
                board[z][1][x] == board[z][2][x]) {
                return (Players) board[z][0][x];
            }

        }

    }

    
    // 3. Check diagonals in each layer (6 diagonals total, 2 per layer) ..

    for (uint8_t z = 0; z < 3; z++) {

        // Diagonal top-left to bottom-right
        if (board[z][0][0] != Players::None &&
            board[z][0][0] == board[z][1][1] &&
            board[z][1][1] == board[z][2][2]) {
            return (Players) board[z][0][0];
        }

        // Diagonal top-right to bottom-left
        if (board[z][0][2] != Players::None &&
            board[z][0][2] == board[z][1][1] &&
            board[z][1][1] == board[z][2][0]) {
            return (Players) board[z][0][2];
        }
		
    }
    

    // 4. Check vertical lines through all layers (9 lines total) ..

    for (uint8_t y = 0; y < 3; y++) {

        for (uint8_t x = 0; x < 3; x++) {

            if (board[0][y][x] != Players::None &&
                board[0][y][x] == board[1][y][x] &&
                board[1][y][x] == board[2][y][x]) {
                return (Players) board[0][y][x];
            }

        }

    }
    

    // 5. Check diagonals through layers in X-Z plane (6 lines) ..

    for (uint8_t y = 0; y < 3; y++) {

        // Diagonal front-left to back-right
        if (board[0][y][0] != Players::None &&
            board[0][y][0] == board[1][y][1] &&
            board[1][y][1] == board[2][y][2]) {
            return (Players) board[0][y][0];
        }

        // Diagonal front-right to back-left
        if (board[0][y][2] != Players::None &&
            board[0][y][2] == board[1][y][1] &&
            board[1][y][1] == board[2][y][0]) {
            return (Players) board[0][y][2];
        }

    }
    

    // 6. Check diagonals through layers in Y-Z plane (6 lines) ..

    for (uint8_t x = 0; x < 3; x++) {

        // Diagonal top-front to bottom-back
        if (board[0][0][x] != Players::None &&
            board[0][0][x] == board[1][1][x] &&
            board[1][1][x] == board[2][2][x]) {
            return (Players) board[0][0][x];
        }

        // Diagonal bottom-front to top-back
        if (board[0][2][x] != Players::None &&
            board[0][2][x] == board[1][1][x] &&
            board[1][1][x] == board[2][0][x]) {
            return (Players) board[0][2][x];
        }

    }
    
    // 7. Check 4 main 3D diagonals (corner to corner through center)
    // Front-top-left to back-bottom-right ..

    if (board[0][0][0] != Players::None &&
        board[0][0][0] == board[1][1][1] &&
        board[1][1][1] == board[2][2][2]) {
        return (Players) board[0][0][0];
    }
    

    // Front-top-right to back-bottom-left ..

    if (board[0][0][2] != Players::None &&
        board[0][0][2] == board[1][1][1] &&
        board[1][1][1] == board[2][2][0]) {
        return (Players) board[0][0][2];
    }
    

    // Front-bottom-left to back-top-right ..

    if (board[0][2][0] != Players::None &&
        board[0][2][0] == board[1][1][1] &&
        board[1][1][1] == board[2][0][2]) {
        return (Players) board[0][2][0];
    }
    

    // Front-bottom-right to back-top-left ..

    if (board[0][2][2] != Players::None &&
        board[0][2][2] == board[1][1][1] &&
        board[1][1][1] == board[2][0][0]) {
        return (Players) board[0][2][2];
    }
    

    // Draw?

    for (uint8_t z = 0; z < 3; z++) {

        for (uint8_t y = 0; y < 3; y++) {

            for (uint8_t x = 0; x < 3; x++) {

                if (board[z][y][x] == Players::None) {

                    return Players::None;

                }

            }

        }

    }

    return Players::Draw;

}


bool checkForWinningPiece(uint8_t z_Test, uint8_t y_Test, uint8_t x_Test) {
    
    // Check all possible 3-in-a-row combinations
    
    // 1. Check rows in each layer (9 rows total, 3 per layer) ..

    for (uint8_t z = 0; z < 3; z++) {

        for (uint8_t y = 0; y < 3; y++) {

            if (board[z][y][0] != Players::None &&
                board[z][y][0] == board[z][y][1] &&
                board[z][y][1] == board[z][y][2]) {

                return (z_Test == z && y_Test == y);

            }

        }

    }
    

    // 2. Check columns in each layer (9 columns total, 3 per layer) ..

    for (uint8_t z = 0; z < 3; z++) {

        for (uint8_t x = 0; x < 3; x++) {

            if (board[z][0][x] != Players::None &&
                board[z][0][x] == board[z][1][x] &&
                board[z][1][x] == board[z][2][x]) {

                return (z_Test == z && x_Test == x);

            }

        }

    }

    
    // 3. Check diagonals in each layer (6 diagonals total, 2 per layer) ..

    for (uint8_t z = 0; z < 3; z++) {

        // Diagonal top-left to bottom-right
        if (board[z][0][0] != Players::None &&
            board[z][0][0] == board[z][1][1] &&
            board[z][1][1] == board[z][2][2]) {

            return (z_Test == z && y_Test == x_Test);

        }

        // Diagonal top-right to bottom-left
        if (board[z][0][2] != Players::None &&
            board[z][0][2] == board[z][1][1] &&
            board[z][1][1] == board[z][2][0]) {

            return (z_Test == z && ((y_Test == 0 && x_Test == 2) || (y_Test == 1 && x_Test == 1) || (y_Test == 2 && x_Test == 0)));

        }
		
    }
    

    // 4. Check vertical lines through all layers (9 lines total) ..

    for (uint8_t y = 0; y < 3; y++) {

        for (uint8_t x = 0; x < 3; x++) {

            if (board[0][y][x] != Players::None &&
                board[0][y][x] == board[1][y][x] &&
                board[1][y][x] == board[2][y][x]) {

                return (y_Test == y && x_Test == x);

            }

        }

    }
    

    // 5. Check diagonals through layers in X-Z plane (6 lines) ..

    for (uint8_t y = 0; y < 3; y++) {

        // Diagonal front-left to back-right
        if (board[0][y][0] != Players::None &&
            board[0][y][0] == board[1][y][1] &&
            board[1][y][1] == board[2][y][2]) {

            return (y_Test == y && ((z_Test == 0 && x_Test == 0) || (z_Test == 1 && x_Test == 1) || (z_Test == 2 && x_Test == 2)));

        }

        // Diagonal front-right to back-left
        if (board[0][y][2] != Players::None &&
            board[0][y][2] == board[1][y][1] &&
            board[1][y][1] == board[2][y][0]) {

            return (y_Test == y && ((z_Test == 0 && x_Test == 2) || (z_Test == 1 && x_Test == 1) || (z_Test == 2 && x_Test == 0)));

        }

    }
    

    // 6. Check diagonals through layers in Y-Z plane (6 lines) ..

    for (uint8_t x = 0; x < 3; x++) {

        // Diagonal top-front to bottom-back
        if (board[0][0][x] != Players::None &&
            board[0][0][x] == board[1][1][x] &&
            board[1][1][x] == board[2][2][x]) {

            return (x_Test == x && ((z_Test == 0 && y_Test == 0) || (z_Test == 1 && y_Test == 1) || (z_Test == 2 && y_Test == 2)));

        }

        // Diagonal bottom-front to top-back
        if (board[0][2][x] != Players::None &&
            board[0][2][x] == board[1][1][x] &&
            board[1][1][x] == board[2][0][x]) {

            return (x_Test == x && ((z_Test == 0 && y_Test == 2) || (z_Test == 1 && y_Test == 1) || (z_Test == 2 && y_Test == 0)));

        }

    }
    
    // 7. Check 4 main 3D diagonals (corner to corner through center)
    // Front-top-left to back-bottom-right ..

    if (board[0][0][0] != Players::None &&
        board[0][0][0] == board[1][1][1] &&
        board[1][1][1] == board[2][2][2]) {

        return ((z_Test == 0 && y_Test == 0 && x_Test == 0) || (z_Test == 1 && y_Test == 1 && x_Test == 1) || (z_Test == 2 && y_Test == 2 && x_Test == 2));

    }
    

    // Front-top-right to back-bottom-left ..

    if (board[0][0][2] != Players::None &&
        board[0][0][2] == board[1][1][1] &&
        board[1][1][1] == board[2][2][0]) {

        return ((z_Test == 0 && y_Test == 0 && x_Test == 2) || (z_Test == 1 && y_Test == 1 && x_Test == 1) || (z_Test == 2 && y_Test == 2 && x_Test == 0));

    }
    

    // Front-bottom-left to back-top-right ..

    if (board[0][2][0] != Players::None &&
        board[0][2][0] == board[1][1][1] &&
        board[1][1][1] == board[2][0][2]) {

        return ((z_Test == 0 && y_Test == 2 && x_Test == 0) || (z_Test == 1 && y_Test == 1 && x_Test == 1) || (z_Test == 2 && y_Test == 0 && x_Test == 2));

    }
    

    // Front-bottom-right to back-top-left ..

    if (board[0][2][2] != Players::None &&
        board[0][2][2] == board[1][1][1] &&
        board[1][1][1] == board[2][0][0]) {

        return ((z_Test == 0 && y_Test == 2 && x_Test == 2) || (z_Test == 1 && y_Test == 1 && x_Test == 1) || (z_Test == 2 && y_Test == 0 && x_Test == 0));

    }
    
    return Players::None;

}


void swapToPlayerMove(uint8_t x, uint8_t y, uint8_t z) {

	cursor[Constants::X_Axis] = x;
	cursor[Constants::Y_Axis] = y;
	cursor[Constants::Z_Axis] = z;

    board[z][y][x] = Players::Crosses;

	playerOrComp = 0;

}



void computerMove() {
    

    // Strategy 1: Try to win ..

    for (uint8_t z = 0; z < 3; z++) {

        for (uint8_t y = 0; y < 3; y++) {

            for (uint8_t x = 0; x < 3; x++) {

                if (board[z][y][x] == Players::None) {

                    board[z][y][x] = Players::Crosses;

                    if (checkForWin() != Players::None) {

						swapToPlayerMove(x, y, z);
                        return; // Winning move found
                    }

					board[z][y][x] = Players::None;

                }

            }

        }

    }
    

    // Strategy 2: Block player from winning ..

    for (uint8_t z = 0; z < 3; z++) {

        for (uint8_t y = 0; y < 3; y++) {

            for (uint8_t x = 0; x < 3; x++) {

                if (board[z][y][x] == Players::None) {

                    board[z][y][x] = Players::Noughts;

                    if (checkForWin() != Players::None) {

						swapToPlayerMove(x, y, z);
                        board[z][y][x] = Players::Crosses;
                        return; // Block found

                    }

					board[z][y][x] = Players::None;

                }

            }

        }

    }
    

    // Strategy 3: Take center if available ..

    if (board[1][1][1] == Players::None && middlePositionCount >= 4) {

		swapToPlayerMove(1, 1, 1);
        return;

    }
    

    // Strategy 4: Take corners ..

    uint8_t corners[8][3] = {
        {0,0,0}, {0,0,2}, {0,2,0}, {0,2,2},
        {2,0,0}, {2,0,2}, {2,2,0}, {2,2,2}
    };
    
    for (uint8_t j = 0; j < 12; j++) {

        uint8_t i = random(8);

        uint8_t z = corners[i][0];
        uint8_t y = corners[i][1];
        uint8_t x = corners[i][2];

        if (board[z][y][x] == Players::None) {

			swapToPlayerMove(x, y, z);
            return;
        }

    }
    

    // Strategy 5: Take any available space ..

    uint8_t r = random(6);

    for (uint8_t a = 0; a < 3; a++) {

        for (uint8_t b = 0; b < 3; b++) {

            for (uint8_t c = 0; c < 3; c++) {

                switch (r) {

                    case 0:

                        if (board[a][b][c] == Players::None) {

                            swapToPlayerMove(a, b, c);
                            return;

                        }

                        break;

                    case 1:

                        if (board[a][c][b] == Players::None) {

                            swapToPlayerMove(a, c, b);
                            return;

                        }

                        break;

                    case 2:

                        if (board[b][a][c] == Players::None) {

                            swapToPlayerMove(b, a, c);
                            return;

                        }

                        break;

                    case 3:

                        if (board[b][c][a] == Players::None) {

                            swapToPlayerMove(b, c, a);
                            return;

                        }

                        break;

                    case 4:

                        if (board[c][a][b] == Players::None) {

                            swapToPlayerMove(c, a, b);
                            return;

                        }

                        break;

                    case 5:

                        if (board[c][b][a] == Players::None) {

                            swapToPlayerMove(c, b, a);
                            return;

                        }

                        break;

                }

            }

        }

    }

}
