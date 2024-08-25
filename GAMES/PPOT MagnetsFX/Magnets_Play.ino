#include <ArduboyFX.h>  
 
uint8_t bounce[7] = { 0, 0, 0, 0, 0, 0, 0 };

void play_Init() { 

    gameState = GameState::Play;
    game.setup();
    game.loadPuzzle();

    #ifdef PARTICLES

        for (Particle &particle : particles) {

            particle.setCounter(0);

        }

    #endif

    for (uint8_t i = 0; i < 7; i++) {

        bounce[i] = (6 - i) * 2;

    }

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void play_Update() { 

    uint8_t justPressed = getJustPressedButtons();

    frameCount++;

    if (gameState == GameState::Play) {

        if (justPressed & RIGHT_BUTTON) {

            if (game.getX() < game.getWidth()) {
                game.setX(game.getX() + 1);
                frameCount = 0;
            }
            else {

                gameState = GameState::Play_Menu1;

            }
            
        }

        if (justPressed & LEFT_BUTTON) {

            if (game.getX() > 1) {
                game.setX(game.getX() - 1);
                frameCount = 0;
            }
            
        }

        if (justPressed & DOWN_BUTTON) {

            if (game.getY() < game.getHeight()) {
                game.setY(game.getY() + 1);
                frameCount = 0;
            }
            
        }

        if (justPressed & UP_BUTTON) {

            if (game.getY() > 1) {
                game.setY(game.getY() - 1);
                frameCount = 0;
            }
            
        }

        if (justPressed & A_BUTTON) {

            frameCount = 0;
            ValidMove isValidMove = ValidMove::Neutral;

            switch (game.puzzle[game.getY()][game.getX()]) {


                // Horizontal ..

                case Tiles::Horizontal_MinusPlus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_PlusMinus_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_PlusMinus_End;
                    isValidMove = game.isValidMove(game.getX(), game.getY());
                    break;

                case Tiles::Horizontal_MinusPlus_End:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_PlusMinus_End;
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_PlusMinus_Start;
                    isValidMove = game.isValidMove(game.getX() - 1, game.getY());
                    break;
        
                case Tiles::Horizontal_PlusMinus_End:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Blank_End;
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_Blank_Start;
                    break;

                case Tiles::Horizontal_PlusMinus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Blank_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_Blank_End;            
                    break;

                case Tiles::Horizontal_Neutral_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_PlusMinus_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_PlusMinus_End;                  
                    isValidMove = game.isValidMove(game.getX(), game.getY());
                    break;

                case Tiles::Horizontal_Neutral_End:
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_PlusMinus_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_PlusMinus_End;                  
                    isValidMove = game.isValidMove(game.getX() - 1, game.getY());
                    break;

                case Tiles::Horizontal_Blank_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_MinusPlus_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_MinusPlus_End;                  
                    isValidMove = game.isValidMove(game.getX(), game.getY());
                    break;

                case Tiles::Horizontal_Blank_End:
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_MinusPlus_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_MinusPlus_End;                  
                    isValidMove = game.isValidMove(game.getX() - 1, game.getY());
                    break;


                // Vertical ..

                case Tiles::Vertical_MinusPlus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_PlusMinus_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_PlusMinus_End;            
                    isValidMove = game.isValidMove(game.getX(), game.getY());
                    break;

                case Tiles::Vertical_MinusPlus_End:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_PlusMinus_End;
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_PlusMinus_Start;                  
                    isValidMove = game.isValidMove(game.getX(), game.getY() - 1);
                    break;

                case Tiles::Vertical_PlusMinus_End:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Blank_End;
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_Blank_Start;                  
                    break;

                case Tiles::Vertical_PlusMinus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Blank_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_Blank_End;                  
                    break;

                case Tiles::Vertical_Neutral_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_PlusMinus_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_PlusMinus_End;                  
                    isValidMove = game.isValidMove(game.getX(), game.getY());
                    break;

                case Tiles::Vertical_Neutral_End:
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_PlusMinus_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_PlusMinus_End;                  
                    isValidMove = game.isValidMove(game.getX(), game.getY() - 1);
                    break;

                case Tiles::Vertical_Blank_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_MinusPlus_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_MinusPlus_End;                  
                    isValidMove = game.isValidMove(game.getX(), game.getY());
                    break;

                case Tiles::Vertical_Blank_End:
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_MinusPlus_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_MinusPlus_End;                  
                    isValidMove = game.isValidMove(game.getX(), game.getY() - 1);
                    break;

            }

            game.updateNumbers_Puzzle();

            if (game.isGameComplete()) {

                #ifdef PARTICLES
                    launchParticles();
                #endif

                gameState = GameState::Play_Particles;
                frameCount = 0;

            }
                
            switch (isValidMove) {

                case ValidMove::Valid:
                    
                    #ifdef USE_LED
                        a.digitalWriteRGB(GREEN_LED, RGB_ON);
                        LED_Counter = Constants::LED_Period;
                    #endif
                    
                    #ifdef USE_LED_NOT_BRIGHT
                        a.setRGBled(GREEN_LED, 16);
                        LED_Counter = Constants::LED_Period;
                    #endif

                    break;

                case ValidMove::NotValid:

                    #ifdef USE_LED
                        a.digitalWriteRGB(RED_LED, RGB_ON);
                        LED_Counter = Constants::LED_Period;
                    #endif
                    
                    #ifdef USE_LED_NOT_BRIGHT
                        a.setRGBled(RED_LED, 16);
                        LED_Counter = Constants::LED_Period;
                    #endif

                    break;

                case ValidMove::Neutral:
                    break;
                    
            }

        }

        if (justPressed & B_BUTTON) {

            frameCount = 0;

            switch (game.puzzle[game.getY()][game.getX()]) {

                // Horizontal ..

                case Tiles::Horizontal_MinusPlus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Neutral_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_Neutral_End;
                    break;

                case Tiles::Horizontal_MinusPlus_End:
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_Neutral_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Neutral_End;
                    break;
        
                case Tiles::Horizontal_PlusMinus_End:
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_Neutral_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Neutral_End;
                    break;

                case Tiles::Horizontal_PlusMinus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Neutral_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_Neutral_End;            
                    break;

                case Tiles::Horizontal_Neutral_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Blank_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_Blank_End;                  
                    break;

                case Tiles::Horizontal_Neutral_End:
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_Blank_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Blank_End;                  
                    break;

                case Tiles::Horizontal_Blank_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Neutral_Start;
                    game.puzzle[game.getY()][game.getX() + 1] = Tiles::Horizontal_Neutral_End;                  
                    break;

                case Tiles::Horizontal_Blank_End:
                    game.puzzle[game.getY()][game.getX() - 1] = Tiles::Horizontal_Neutral_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Horizontal_Neutral_End;                  
                    break;

                // Vertical ..

                case Tiles::Vertical_MinusPlus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Neutral_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_Neutral_End;            
                    break;

                case Tiles::Vertical_MinusPlus_End:
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_Neutral_Start;                  
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Neutral_End;
                    break;

                case Tiles::Vertical_PlusMinus_End:
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_Neutral_Start;                  
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Neutral_End;
                    break;

                case Tiles::Vertical_PlusMinus_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Neutral_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_Neutral_End;                  
                    break;

                case Tiles::Vertical_Neutral_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Blank_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_Blank_End;                  
                    break;

                case Tiles::Vertical_Neutral_End:
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_Blank_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Blank_End;                  
                    break;

                case Tiles::Vertical_Blank_Start:
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Neutral_Start;
                    game.puzzle[game.getY() + 1][game.getX()] = Tiles::Vertical_Neutral_End;                  
                    break;

                case Tiles::Vertical_Blank_End:
                    game.puzzle[game.getY() - 1][game.getX()] = Tiles::Vertical_Neutral_Start;
                    game.puzzle[game.getY()][game.getX()] = Tiles::Vertical_Neutral_End;                  
                    break;

            }

            game.updateNumbers_Puzzle();

            #ifdef DEBUG_FORCE_EOG
            if (!game.isGameComplete()) {
            #else
            if (game.isGameComplete()) {
            #endif

                #ifdef PARTICLES
                    launchParticles();
                #endif

                gameState = GameState::Play_Particles;
                frameCount = 0;

            }


        }

    }
    else if (gameState == GameState::Play_Particles) {

        if (frameCount % 2 == 0) {

            for (uint8_t  i = 0; i < 7; i++) {

                if (bounce[i] < 50) {
                
                    bounce[i]++;

                }

            }

        }

        if (bounce[0] == 50 && (justPressed & B_BUTTON || justPressed & A_BUTTON)) {

            gameState = GameState::Title_Init;

        }
        
    }
    else {

        // Menu ..

        if (justPressed & LEFT_BUTTON || justPressed & B_BUTTON) {

            gameState = GameState::Play;
            hint.count = 0;
            
        }

        if (justPressed & UP_BUTTON && gameState > GameState::Play_Menu1) {

            gameState--;
            hint.count = 0;
            
        }

        if (justPressed & DOWN_BUTTON && gameState < GameState::Play_Menu4) {

            gameState++;
            hint.count = 0;
            
        }

        if (justPressed & A_BUTTON) {

            switch (gameState) {

                case GameState::Play_Menu1:
                    game.clearPuzzle();
                    game.setX(1);
                    game.setY(1);
                    gameState = GameState::Play;
                    break;

                case GameState::Play_Menu2:
                    {
                        hint = game.getHint();

                        switch (hint.hintType) {

                            case HintType::InvalidTile:

                                game.setX(hint.x);
                                game.setY(hint.y);
                                gameState = GameState::Play_Hint0;

                                break;

                            case HintType::RandomTile:

                                game.puzzle[hint.y][hint.x] = hint.tile;
                                game.puzzle[hint.y2][hint.x2] = hint.tile2;
                                game.setX(hint.x);
                                game.setY(hint.y);
                                game.updateNumbers_Puzzle();
                                gameState = GameState::Play_Hint1;

                                break;

                        }

                    }
                    break;

                case GameState::Play_Menu3:
                    game.copy_SolutionToPuzzle();
                    game.updateNumbers_Puzzle();
                    break;

                case GameState::Play_Menu4:
                    gameState = GameState::Title_Init;
                    break;

            }
            
        }

    }

}

void play(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    uint8_t currentPlane = a.currentPlane();
    if (a.needsUpdate()) play_Update();

    if (frameCount % 4 == 0) {

        xBot = xBot - 1;        if (xBot == -200) xBot = 0;

    }

    if (frameCount % 5 == 0) {

        xTop = xTop + 1;        if (xTop == 200) xTop = 0;

    }

    if (game.getGameSize() != GameSize::Large) {
   
        SpritesU::drawOverwriteFX(88, xTop, Images::Wave_Left, currentPlane);
        SpritesU::drawOverwriteFX(88, xTop - 200, Images::Wave_Left, currentPlane);
        SpritesU::drawOverwriteFX(108, xBot, Images::Wave_Right, currentPlane);
        SpritesU::drawOverwriteFX(108, xBot + 200, Images::Wave_Right, currentPlane);

    }

    switch (gameState) {

        case GameState::Play:

            if (game.renderMenu()) {

                if (hint.count == 0) {
                    SpritesU::drawPlusMaskFX(92, 3, Images::InGame, currentPlane);
                }

            }
            else {
                SpritesU::drawOverwriteFX(123, 1, Images::Arrow, currentPlane);
            }

            renderGrid(currentPlane, true);

            // if (hint.count != 0) {
            //     SpritesU::drawPlusMaskFX(86, 3, Images::Hints, (static_cast<uint8_t>(hint.hintType) * 3) + currentPlane);
            // }

            break;

        case GameState::Play_Particles:
            {
                // SpritesU::drawPlusMaskFX(92, 3, Images::InGame, currentPlane);

                #ifdef PARTICLES

                    if (frameCount % 32) {
                        launchParticles();
                    }

                #endif

                renderGrid(currentPlane, false);

                SpritesU::drawPlusMaskFX(3,   static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bounce[0])) - 16, Images::Hooray_00, currentPlane);
                SpritesU::drawPlusMaskFX(21,  static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bounce[1])) - 16, Images::Hooray_01, currentPlane);
                SpritesU::drawPlusMaskFX(44,  static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bounce[2])) - 16, Images::Hooray_02, currentPlane);
                SpritesU::drawPlusMaskFX(68,  static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bounce[3])) - 16, Images::Hooray_03, currentPlane);
                SpritesU::drawPlusMaskFX(84,  static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bounce[4])) - 16, Images::Hooray_04, currentPlane);
                SpritesU::drawPlusMaskFX(101, static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bounce[5])) - 16, Images::Hooray_05, currentPlane);
                SpritesU::drawPlusMaskFX(118, static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bounce[6])) - 16, Images::Hooray_06, currentPlane);
    
            }
            break;

        case GameState::Play_Menu0 ... GameState::Play_Menu4:

            if (!(game.getRenderSize() == RenderSize::Large && game.getGameSize() == GameSize::Large)) {
                SpritesU::drawPlusMaskFX(92, 3, Images::InGame, ((static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Menu0)) * 3) + currentPlane);
            }

            renderGrid(currentPlane, false);

            if (game.getRenderSize() == RenderSize::Large && game.getGameSize() == GameSize::Large) {
                SpritesU::drawOverwriteFX(128 - 46, 0, Images::InGame_Solid, ((static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Menu0)) * 3) + currentPlane);
            }

            break;

        case GameState::Play_Hint0 ... GameState::Play_Hint1:

            if (!(game.getRenderSize() == RenderSize::Large && game.getGameSize() == GameSize::Large)) {
                SpritesU::drawPlusMaskFX(86, 3, Images::Hints, (static_cast<uint8_t>(hint.hintType) * 3) + currentPlane);
            }

            renderGrid(currentPlane, true);

            if (game.getRenderSize() == RenderSize::Large && game.getGameSize() == GameSize::Large) {
                SpritesU::drawOverwriteFX(128 - 46, 0, Images::InGame_Solid, (5 * 3) + currentPlane);
                SpritesU::drawOverwriteFX(85, 3, Images::Hints_Solid, (static_cast<uint8_t>(hint.hintType) * 3) + currentPlane);
            }

            break;

        default:
            renderGrid(currentPlane, true);
            break;
    }




    // Render particles and scores ..

    #ifdef PARTICLES
        updateAndRenderParticles(frameCount);
    #endif

}

#ifdef PARTICLES
    void launchParticles() {

        launchParticles(64, 32);

    }
#endif