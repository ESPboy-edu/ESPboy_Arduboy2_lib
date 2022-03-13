#include "src/Game.h"

#include "src/utils/Enums.h"
#include "src/utils/Arduboy2Ext.h"

//#ifndef DEBUG
//ARDUBOY_NO_USB
//#endif

Game game;

void setup() {
	game.setup();
}

void loop() {
	game.loop();
}
