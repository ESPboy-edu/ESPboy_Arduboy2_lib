#include <Arduboy2.h>


// Draw a line in 3D space (isometric) with X-axis rotation
void drawLine3D_X(Fixed x1, Fixed y1, Fixed z1, Fixed x2, Fixed y2, Fixed z2,
                  Fixed centerY) {

	// Apply rotation around X axis (vertical rotation)
	rotateX(y1, z1, rotationAngle, centerY);
	rotateX(y2, z2, rotationAngle, centerY);

	int16_t sx1, sy1, sx2, sy2;
	isoProject(x1, y1, z1, sx1, sy1);
	isoProject(x2, y2, z2, sx2, sy2);
	arduboy.drawLine(sx1, sy1, sx2, sy2);

}

// Draw a line in 3D space (isometric) with Y-axis rotation
void drawLine3D_Y(Fixed x1, Fixed y1, Fixed z1, Fixed x2, Fixed y2, Fixed z2) {

	// Apply rotation around Y axis (horizontal rotation)
	rotateY(x1, z1, rotationAngle);
	rotateY(x2, z2, rotationAngle);

	int16_t sx1, sy1, sx2, sy2;
	isoProject(x1, y1, z1, sx1, sy1);
	isoProject(x2, y2, z2, sx2, sy2);
	arduboy.drawLine(sx1, sy1, sx2, sy2);

}

// Draw a 3x3 grid at a given Y level (height) with X-axis rotation
void drawGridLayer_X(Fixed yLevel, Fixed gridSize, Fixed centerY) {
					 
	Fixed half = gridSize / 2;
	Fixed third = gridSize / 3;

	// Draw outer square
	drawLine3D_X(-half, yLevel, -half, half, yLevel, -half, centerY);  // back
	drawLine3D_X(half, yLevel, -half, half, yLevel, half, centerY);    // right
	drawLine3D_X(half, yLevel, half, -half, yLevel, half, centerY);    // front
	drawLine3D_X(-half, yLevel, half, -half, yLevel, -half, centerY);  // left


	// Draw vertical grid lines (parallel to Z axis) ..

	for (int i = 1; i < 3; i++) {
		Fixed x = -half + Fixed(i) * third;
		drawLine3D_X(x, yLevel, -half, x, yLevel, half, centerY);
	}


	// Draw horizontal grid lines (parallel to X axis) ..

	for (int i = 1; i < 3; i++) {
		Fixed z = -half + Fixed(i) * third;
		drawLine3D_X(-half, yLevel, z, half, yLevel, z, centerY);
  	}

}

// Draw a 3x3 grid at a given Y level (height) with Y-axis rotation
void drawGridLayer_Y(Fixed yLevel, Fixed gridSizee) {

	Fixed half = gridSize / 2;
	Fixed third = gridSize / 3;


	// Draw outer square
	drawLine3D_Y(-half, yLevel, -half, half, yLevel, -half);  // back
	drawLine3D_Y(half, yLevel, -half, half, yLevel, half);    // right
	drawLine3D_Y(half, yLevel, half, -half, yLevel, half);    // front
	drawLine3D_Y(-half, yLevel, half, -half, yLevel, -half);  // left

	// Draw vertical grid lines (parallel to Z axis)
	for (int i = 1; i < 3; i++) {
		Fixed x = -half + Fixed(i) * third;
		drawLine3D_Y(x, yLevel, -half, x, yLevel, half);
	}

	// Draw horizontal grid lines (parallel to X axis)
	for (int i = 1; i < 3; i++) {
		Fixed z = -half + Fixed(i) * third;
		drawLine3D_Y(-half, yLevel, z, half, yLevel, z);
	}

}

void renderBoard() {

	for (int8_t z = 0; z <= 2; z++) {

		Sprites::drawExternalMask(38, 40 - (z * Constants::Z_Axis_Height), Images::Background, Images::Background_Mask, 0, 0);


		// Render cursor ..

		if ((2 - z) == cursor[Constants::Z_Axis]) {

			renderCursor();

		}


		// Render pieces ..

		renderPieces(0, (2 - z), false);


	}

}


void renderGameTally() {

	Sprites::drawOverwrite(107, 32, Images::P1P2, playerOrComp + (numberOfPlayers < 2 ? 2 : 0));

	if (!flashNoughts || (arduboy.frameCount % 32) < 16)		Sprites::drawSelfMasked(119, 39, Images::Numbers_2Digit, player1Win);
	if (!flashCrosses || (arduboy.frameCount % 32) < 16)		Sprites::drawSelfMasked(119, 45, Images::Numbers_2Digit, player2Win);

}


void renderPieces(uint8_t xOffset, uint8_t zAxis, bool checkPieces) {
 
	int8_t zOffset = -3;

	for (uint8_t x = 0; x <= 2; x++) {

		for (uint8_t y = 0; y <= 2; y++) {

            bool winningPiece = checkForWinningPiece(zAxis, y, x);

			if (board[zAxis][y][x] == Players::Noughts) {

                if (!checkPieces || !winningPiece || (checkPieces && (arduboy.frameCount %32 < 16) && winningPiece)) {

    				Sprites::drawSelfMasked(60 - xOffset + Background_M_Cursor_X[x] + Background_M_Cursor_X_YOffset[y], 
											zOffset + (zAxis * Constants::Z_Axis_Height) + 3 + Background_M_Cursor_Y[y]+ Background_M_Cursor_Y_XOffset[x], 
											Images::Nought, 0);
			
                }
			
			}

			if (board[zAxis][y][x] == Players::Crosses) {

                if (!checkPieces || !winningPiece || (checkPieces && (arduboy.frameCount %32 < 16) && winningPiece)) {

    				Sprites::drawSelfMasked(60 - xOffset + Background_M_Cursor_X[x] + Background_M_Cursor_X_YOffset[y], 
											zOffset + (zAxis * Constants::Z_Axis_Height) + 3 + Background_M_Cursor_Y[y]+ Background_M_Cursor_Y_XOffset[x], 
											Images::Cross, 0);

                }
			
			}

		}

	}

}

void renderCursor() {

	int8_t zOffset = -3;//cursor[Constants::Z_Axis];

	if (arduboy.frameCount % 32 < 16) {

		Sprites::drawSelfMasked(55 + Background_M_Cursor_X[cursor[Constants::X_Axis]] + Background_M_Cursor_X_YOffset[cursor[Constants::Y_Axis]], 
								zOffset + (cursor[Constants::Z_Axis] * Constants::Z_Axis_Height) + Background_M_Cursor_Y[cursor[Constants::Y_Axis]]+ Background_M_Cursor_Y_XOffset[cursor[Constants::X_Axis]], 
								Images::Background_Cursor, 0);

	}

} 