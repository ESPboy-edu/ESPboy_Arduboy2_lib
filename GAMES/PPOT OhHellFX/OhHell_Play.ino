#include <ArduboyFX.h>  


uint8_t bid = 0;

void game_Init() {

    game.setup();

}

void play_Init() { 

    game.resetFrameCount();
    bid = 0;

    gameState = GameState::Play_Deal_1;

    game.fillCardsPile();
    game.shuffleCardsPile();
    game.players[Constants::HumanPlayer].sort();

    populateRotateDetails();

    game.setFirstPlayer((game.getDealer() + 1) % 4);
    game.resetEOR();

    for (uint8_t i = 0; i < Constants::PlayerCount; i++) {

        game.players[i].resetEOR();
        game.getTableCard(i).setSuit(Suits::None);

    }


    #ifdef DEBUG_CRASH
        game.setCardCount(9);
    #endif

}

void populateRotateDetails() {

    uint8_t card = static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Deal_1);
    
    dealPhysics.x = 110;
    dealPhysics.y = 1;

    dealPhysics.x2 = pgm_read_byte(Constants::DealCardCoords + (card * 5));
    dealPhysics.y2 = pgm_read_byte(Constants::DealCardCoords + (card * 5) + 1);
    dealPhysics.noOfIncs = pgm_read_byte(Constants::DealCardCoords + (card * 5) + 2);
    dealPhysics.rotStart = pgm_read_byte(Constants::DealCardCoords + (card * 5) + 3);
    dealPhysics.rotInc = pgm_read_byte(Constants::DealCardCoords +( card * 5) + 4);

    dealPhysics.xInc = (dealPhysics.x2 - dealPhysics.x) / dealPhysics.noOfIncs;
    dealPhysics.yInc = (dealPhysics.y2 - dealPhysics.y) / dealPhysics.noOfIncs;
    dealPhysics.x = 110 + dealPhysics.xInc;
    dealPhysics.y = -dealPhysics.yInc;
    dealPhysics.rotInc = (dealPhysics.rotInc / dealPhysics.noOfIncs);
    dealPhysics.rot = dealPhysics.rotStart;

}

// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void play_Update() { 

    uint8_t justPressed = getJustPressedButtons();

    game.incFrameCount();

    switch (gameState) {

        case GameState::Play_Deal_1 ... GameState::Play_Deal_10:
                
            if (dealPhysics.x > dealPhysics.x2 && dealPhysics.y < dealPhysics.y2) {

                dealPhysics.x = dealPhysics.x + dealPhysics.xInc;
                dealPhysics.y = dealPhysics.y + dealPhysics.yInc;
                dealPhysics.rot = dealPhysics.rot + dealPhysics.rotInc;

                if (dealPhysics.x <= dealPhysics.x2 || dealPhysics.y >= dealPhysics.y2) {
                        
                    #ifndef DEBUG_SOUND
                    playSFX(MusicSFX::SFX_Click);
                    #endif

                    Card card0 = game.pickUpCard();
                    game.players[0].addCard(card0);
                    game.setPlayedCard(card0.getSortValue(), game.getPlayedCard(card0.getSortValue()) | 0x10);

                    Card card1 = game.pickUpCard();
                    game.players[1].addCard(card1);
                    game.setPlayedCard(card1.getSortValue(), game.getPlayedCard(card1.getSortValue()) | 0x20);

                    Card card2 = game.pickUpCard();
                    game.players[2].addCard(card2);
                    game.setPlayedCard(card2.getSortValue(), game.getPlayedCard(card2.getSortValue()) | 0x40);

                    Card card3 = game.pickUpCard();
                    game.players[3].addCard(card3);
                    game.setPlayedCard(card3.getSortValue(), game.getPlayedCard(card3.getSortValue()) | 0x80);

                    gameState++;

                    if (gameState == static_cast<GameState>(static_cast<uint8_t>(GameState::Play_Deal_1) + game.getCardCount())) {
                        gameState = GameState::Play_Flip;
                    }

                    if (gameState < GameState::Play_Flip) {
                        populateRotateDetails();
                    }
                    else {

                        for (uint8_t i = 0; i < Constants::PlayerCount; i++) {            
                            game.players[i].sort();                        
                        }

                        game.resetFrameCount();

                    }

                }

            }

            break;

        case GameState::Play_Flip:

            if (game.getFrameCount() == 14 * 2) {

                #ifndef DEBUG_SOUND
                playSFX(MusicSFX::SFX_Click);
                #endif
                
                Card card = game.pickUpCard();
                game.getTurnedCard().setValue(card.getValue());                
                game.getTurnedCard().setSuit(card.getSuit());       
                game.setPlayedCard(game.getTurnedCard().getSortValue(), game.getPlayedCard(game.getTurnedCard().getSortValue()) | 0x0F);

                game.resetFrameCount();
                bid = game.players[Constants::HumanPlayer].getInitialBid(game.getTurnedCard().getSuit());
                gameState = static_cast<GameState>(static_cast<uint8_t>(GameState::Play_Bid_00) + ((game.getDealer() + 1) % 4));

                #ifdef DEBUG_CRASH

                    game.setDealer(1);
                    game.setFirstPlayer(2);

                    for (uint8_t i = 0; i < 52; i++) {
                        game.setPlayedCard(i, 0);
                    }

                    game.players[0].getCard(0).setSuit(Suits::Spade);
                    game.players[0].getCard(0).setValue(3);
                    game.players[0].getCard(1).setSuit(Suits::Spade);
                    game.players[0].getCard(1).setValue(7);
                    game.players[0].getCard(2).setSuit(Suits::Spade);
                    game.players[0].getCard(2).setValue(8);
                    game.players[0].getCard(3).setSuit(Suits::Spade);
                    game.players[0].getCard(3).setValue(10);
                    game.players[0].getCard(4).setSuit(Suits::Club);
                    game.players[0].getCard(4).setValue(8);
                    game.players[0].getCard(5).setSuit(Suits::Diamond);
                    game.players[0].getCard(5).setValue(3);
                    game.players[0].getCard(6).setSuit(Suits::Diamond);
                    game.players[0].getCard(6).setValue(10);
                    game.players[0].getCard(7).setSuit(Suits::Diamond);
                    game.players[0].getCard(7).setValue(14);
                    game.players[0].getCard(8).setSuit(Suits::Heart);
                    game.players[0].getCard(8).setValue(13);

                    game.setPlayedCard(game.players[0].getCard(0).getSortValue(), game.getPlayedCard(game.players[0].getCard(0).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(1).getSortValue(), game.getPlayedCard(game.players[0].getCard(1).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(2).getSortValue(), game.getPlayedCard(game.players[0].getCard(2).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(3).getSortValue(), game.getPlayedCard(game.players[0].getCard(3).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(4).getSortValue(), game.getPlayedCard(game.players[0].getCard(4).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(5).getSortValue(), game.getPlayedCard(game.players[0].getCard(5).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(6).getSortValue(), game.getPlayedCard(game.players[0].getCard(6).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(7).getSortValue(), game.getPlayedCard(game.players[0].getCard(7).getSortValue()) | 0x10);
                    game.setPlayedCard(game.players[0].getCard(8).getSortValue(), game.getPlayedCard(game.players[0].getCard(8).getSortValue()) | 0x10);

                    game.players[1].getCard(0).setSuit(Suits::Spade);
                    game.players[1].getCard(0).setValue(2);
                    game.players[1].getCard(1).setSuit(Suits::Club);
                    game.players[1].getCard(1).setValue(3);
                    game.players[1].getCard(2).setSuit(Suits::Club);
                    game.players[1].getCard(2).setValue(4);
                    game.players[1].getCard(3).setSuit(Suits::Club);
                    game.players[1].getCard(3).setValue(14);
                    game.players[1].getCard(4).setSuit(Suits::Diamond);
                    game.players[1].getCard(4).setValue(5);
                    game.players[1].getCard(5).setSuit(Suits::Diamond);
                    game.players[1].getCard(5).setValue(6);
                    game.players[1].getCard(6).setSuit(Suits::Diamond);
                    game.players[1].getCard(6).setValue(9);
                    game.players[1].getCard(7).setSuit(Suits::Heart);
                    game.players[1].getCard(7).setValue(6);
                    game.players[1].getCard(8).setSuit(Suits::Heart);
                    game.players[1].getCard(8).setValue(8);

                    game.setPlayedCard(game.players[1].getCard(0).getSortValue(), game.getPlayedCard(game.players[1].getCard(0).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(1).getSortValue(), game.getPlayedCard(game.players[1].getCard(1).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(2).getSortValue(), game.getPlayedCard(game.players[1].getCard(2).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(3).getSortValue(), game.getPlayedCard(game.players[1].getCard(3).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(4).getSortValue(), game.getPlayedCard(game.players[1].getCard(4).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(5).getSortValue(), game.getPlayedCard(game.players[1].getCard(5).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(6).getSortValue(), game.getPlayedCard(game.players[1].getCard(6).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(7).getSortValue(), game.getPlayedCard(game.players[1].getCard(7).getSortValue()) | 0x20);
                    game.setPlayedCard(game.players[1].getCard(8).getSortValue(), game.getPlayedCard(game.players[1].getCard(8).getSortValue()) | 0x20);

                    game.players[2].getCard(0).setSuit(Suits::Spade);
                    game.players[2].getCard(0).setValue(4);
                    game.players[2].getCard(1).setSuit(Suits::Spade);
                    game.players[2].getCard(1).setValue(5);
                    game.players[2].getCard(2).setSuit(Suits::Spade);
                    game.players[2].getCard(2).setValue(12);
                    game.players[2].getCard(3).setSuit(Suits::Club);
                    game.players[2].getCard(3).setValue(6);
                    game.players[2].getCard(4).setSuit(Suits::Club);
                    game.players[2].getCard(4).setValue(7);
                    game.players[2].getCard(5).setSuit(Suits::Club);
                    game.players[2].getCard(5).setValue(10);
                    game.players[2].getCard(6).setSuit(Suits::Club);
                    game.players[2].getCard(6).setValue(11);
                    game.players[2].getCard(7).setSuit(Suits::Heart);
                    game.players[2].getCard(7).setValue(4);
                    game.players[2].getCard(8).setSuit(Suits::Heart);
                    game.players[2].getCard(8).setValue(10);

                    game.setPlayedCard(game.players[2].getCard(0).getSortValue(), game.getPlayedCard(game.players[2].getCard(0).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(1).getSortValue(), game.getPlayedCard(game.players[2].getCard(1).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(2).getSortValue(), game.getPlayedCard(game.players[2].getCard(2).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(3).getSortValue(), game.getPlayedCard(game.players[2].getCard(3).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(4).getSortValue(), game.getPlayedCard(game.players[2].getCard(4).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(5).getSortValue(), game.getPlayedCard(game.players[2].getCard(5).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(6).getSortValue(), game.getPlayedCard(game.players[2].getCard(6).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(7).getSortValue(), game.getPlayedCard(game.players[2].getCard(7).getSortValue()) | 0x40);
                    game.setPlayedCard(game.players[2].getCard(8).getSortValue(), game.getPlayedCard(game.players[2].getCard(8).getSortValue()) | 0x40);

                    game.players[3].getCard(0).setSuit(Suits::Spade);
                    game.players[3].getCard(0).setValue(6);
                    game.players[3].getCard(1).setSuit(Suits::Spade);
                    game.players[3].getCard(1).setValue(9);
                    game.players[3].getCard(2).setSuit(Suits::Spade);
                    game.players[3].getCard(2).setValue(13);
                    game.players[3].getCard(3).setSuit(Suits::Club);
                    game.players[3].getCard(3).setValue(12);
                    game.players[3].getCard(4).setSuit(Suits::Diamond);
                    game.players[3].getCard(4).setValue(8);
                    game.players[3].getCard(5).setSuit(Suits::Diamond);
                    game.players[3].getCard(5).setValue(11);
                    game.players[3].getCard(6).setSuit(Suits::Heart);
                    game.players[3].getCard(6).setValue(3);
                    game.players[3].getCard(7).setSuit(Suits::Heart);
                    game.players[3].getCard(7).setValue(5);
                    game.players[3].getCard(8).setSuit(Suits::Heart);
                    game.players[3].getCard(8).setValue(9);

                    game.setPlayedCard(game.players[3].getCard(0).getSortValue(), game.getPlayedCard(game.players[3].getCard(0).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(1).getSortValue(), game.getPlayedCard(game.players[3].getCard(1).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(2).getSortValue(), game.getPlayedCard(game.players[3].getCard(2).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(3).getSortValue(), game.getPlayedCard(game.players[3].getCard(3).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(4).getSortValue(), game.getPlayedCard(game.players[3].getCard(4).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(5).getSortValue(), game.getPlayedCard(game.players[3].getCard(5).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(6).getSortValue(), game.getPlayedCard(game.players[3].getCard(6).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(7).getSortValue(), game.getPlayedCard(game.players[3].getCard(7).getSortValue()) | 0x80);
                    game.setPlayedCard(game.players[3].getCard(8).getSortValue(), game.getPlayedCard(game.players[3].getCard(8).getSortValue()) | 0x80);

                    game.getTurnedCard().setValue(14);                
                    game.getTurnedCard().setSuit(Suits::Spade);       
                    game.setPlayedCard(game.getTurnedCard().getSortValue(), game.getPlayedCard(game.getTurnedCard().getSortValue()) | 0x0F);

                    bid = game.players[Constants::HumanPlayer].getInitialBid(game.getTurnedCard().getSuit());
                    gameState = GameState::Play_Bid_02;

                #endif

                printPlayerHands();
                printPlayedCards();

            }

            break;


        case GameState::Play_Bid_01:
        case GameState::Play_Bid_11:
            

            // Handle increase / decrease of bidding ..

            if (justPressed & UP_BUTTON) {
             
                if (bid < game.getCardCount()) bid++;

            }
            
            if (justPressed & DOWN_BUTTON) {
             
                if (bid > 0) bid--;

            }


            // Accept bid ..

            if (justPressed & A_BUTTON) {

                uint8_t firstPlayer = (game.getDealer() + 1) % 4;
                uint8_t playerIdx = (static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Bid_00));
             
                game.players[Constants::HumanPlayer].setBid(bid);

                gameState++;
                game.resetFrameCount();

                if (firstPlayer != playerIdx && (firstPlayer == ((playerIdx + 1) % 4))) {
                    gameState = static_cast<GameState>(static_cast<uint8_t>(GameState::Play_Player_00) + game.getFirstPlayer());
                    game.setSelectedCard(Constants::NoCard);
                }                

                #ifndef DEBUG_SOUND

                    playSFX(MusicSFX::SFX_Bet2);

                #endif

            }


            // Pause game?

            if (justPressed & B_BUTTON) {

                game.setPrevGameState(gameState);
                gameState = GameState::Pause_01;

            }

            break;

        case GameState::Play_Bid_00:
        case GameState::Play_Bid_02:
        case GameState::Play_Bid_03:
        case GameState::Play_Bid_10:
        case GameState::Play_Bid_12:
            
            if (game.getFrameCount() == Constants::Delay) {
                
                uint8_t firstPlayer = (game.getDealer() + 1) % 4;
                uint8_t playerIdx = (static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Bid_00));
                game.players[playerIdx % 4].setBid(game.players[playerIdx % 4].getInitialBid(game.getTurnedCard().getSuit()));

                gameState++;
                game.resetFrameCount();

                if (firstPlayer != playerIdx && (firstPlayer == ((playerIdx + 1) % 4))) {
                    gameState = static_cast<GameState>(static_cast<uint8_t>(GameState::Play_Player_00) + game.getFirstPlayer());
                    game.setSelectedCard(Constants::NoCard);
                }       

                #ifndef DEBUG_SOUND

                    switch (playerIdx) {

                        case 0:
                            playSFX(MusicSFX::SFX_Bet1);
                            break;

                        case 1:
                            break;

                        case 2:
                            playSFX(MusicSFX::SFX_Bet3);
                            break;

                        case 3:
                            playSFX(MusicSFX::SFX_Bet4);
                            break;
                            
                    }
                    
                #endif

            }

            break;

        case GameState::Play_Player_01:
        case GameState::Play_Player_11:

            if (justPressed & LEFT_BUTTON) {
             
                for (int8_t i = game.getSelectedCard() - 1; i >= 0 ; i--) {

                    if (game.getValidCard(i)) {
                        
                        game.setSelectedCard(i);
                        break;

                    }

                }

            }

            if (justPressed & RIGHT_BUTTON) {

                for (int8_t i = game.getSelectedCard() + 1; i < game.players[Constants::HumanPlayer].getCardCount(); i++) {

                    if (game.getValidCard(i)) {
                       
                        game.setSelectedCard(i);
                        break;

                    }

                }

            }

            if (justPressed & UP_BUTTON || justPressed & A_BUTTON) {
                
                Card &tableCard = game.getTableCard(Constants::HumanPlayer);
                Card &selectedCard = game.players[Constants::HumanPlayer].getCard(game.getSelectedCard());

                tableCard.setSuit(selectedCard.getSuit());
                tableCard.setValue(selectedCard.getValue());

                selectedCard.setSuit(Suits::None);
                game.players[Constants::HumanPlayer].sort();

                gameState++;
                game.resetFrameCount();

                if (static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Player_00) - game.getFirstPlayer() >= 4) {

                    game.checkWinner(true);                    
                    gameState = GameState::Play_EOH;
                    bid = 0;

                    if (game.getWinner() == 1) {

                        #ifndef DEBUG_SOUND
                        playSFX(MusicSFX::SFX_Three_Inc);
                        #endif

                    }
                    else {

                        #ifndef DEBUG_SOUND
                        playSFX(MusicSFX::SFX_Three_Dec);
                        #endif

                    }

                }
                else {
                                            
                    #ifndef DEBUG_SOUND
                    playSFX(MusicSFX::SFX_Click);
                    #endif

                }

            }

            if (justPressed & B_BUTTON) {

                game.setPrevGameState(gameState);
                gameState = GameState::Pause_01;

            }

            break;

        case GameState::Play_Player_00:
        case GameState::Play_Player_02:
        case GameState::Play_Player_03:
        case GameState::Play_Player_10:
        case GameState::Play_Player_12:

            if (game.getFrameCount() == Constants::Delay) {

                uint8_t cardToPlay = Constants::NoCard;
                uint8_t playerIdx = (static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Player_00)) % 4;

                Player& player = game.players[playerIdx];
                Card& tableCard = game.getTableCard(playerIdx);
                Card& firstCardPlayed = game.getTableCard(game.getFirstPlayer());
                Card& turnedCard = game.getTurnedCard();

                bool win = player.getTricks() < player.getBid();
                bool hasSuit = player.hasSuit(firstCardPlayed.getSuit());

                if (playerIdx != game.getFirstPlayer()) {
                        
                    if (hasSuit) {

                        uint8_t highCard = player.getHighCard(firstCardPlayed.getSuit());
                        uint8_t lowCard = player.getLowCard(firstCardPlayed.getSuit());

                        if (win) {
                            
                            if (highCard != Constants::NoCard) {

                                Card &selectedCard = player.getCard(highCard);

                                if (!game.cardIsHighestOnTable(selectedCard)) {

                                    playCard(playerIdx, tableCard, player.getCard(lowCard));

                                }
                                else {
                                    
                                    playCard(playerIdx, tableCard, player.getCard(highCard));
                                }

                            }
                            else {

                                playCard(playerIdx, tableCard, player.getCard(lowCard));

                            }

                        }
                        else {

                            playCard(playerIdx, tableCard, player.getCard(lowCard));

                        }

                    }
                    else {

                        bool hasTrump = player.hasSuit(game.getTurnedCard().getSuit());

                        if (hasTrump) {

                            uint8_t highCard = player.getHighCard(game.getTurnedCard().getSuit());
                            uint8_t lowCard = player.getLowCard(game.getTurnedCard().getSuit());

                            if (win) {
                                
                                if (highCard != Constants::NoCard) {

                                    Card &selectedCard = player.getCard(highCard);

                                    if (!game.cardIsHighestOnTable(selectedCard)) {

                                        playCard(playerIdx, tableCard, player.getCard(lowCard));

                                    }
                                    else {

                                        playCard(playerIdx, tableCard, player.getCard(highCard));  

                                    }

                                }

                            }
                            else {

                                playCard(playerIdx, tableCard, player.getCard(lowCard));

                            }

                        }
                        else {

                            uint8_t lowCard = player.getLowCard();
                            playCard(playerIdx, tableCard, player.getCard(lowCard));

                        }

                    }

                }
                else {


                    // First card.

                    bool win = player.getTricks() < player.getBid();

                    uint8_t highCard = player.getHighCard(game.getTurnedCard().getSuit());
                    uint8_t lowCard = player.getLowCard(game.getTurnedCard().getSuit());

                    if (win) {

                        // Play Ace of trumps?

                        uint8_t aceOfTrumps = player.getCard(game.getTurnedCard().getSuit(), 14);

                        if (aceOfTrumps != Constants::NoCard) {
                            
                            Card &card = player.getCard(aceOfTrumps);
                            playCard(playerIdx, tableCard, card);

                        }
                        else {

                            // Play Ace of other suits?

                            uint8_t anyAce = player.getCard(Suits::None, 14);

                            if (anyAce != Constants::NoCard) {
                                
                                Card &card = player.getCard(anyAce);
                                playCard(playerIdx, tableCard, card);

                            }
                            else {

                                uint8_t bestCardIdx = player.getBestCard(game.getPlayedCards());
                                Card &bestCard = player.getCard(bestCardIdx);
                                playCard(playerIdx, tableCard, bestCard);

                            }

                        }

                    }
                    else {

                        // Lose!

                        uint8_t lowestCard = player.getLowCard();
                        playCard(playerIdx, tableCard, player.getCard(lowestCard));

                    }

                }
                 
                // printPlayerHands();                
                // printPlayedCards();

                gameState++;
                game.resetFrameCount();

                if (static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Play_Player_00) - game.getFirstPlayer() >= 4) {

                    game.checkWinner(true);
                    gameState = GameState::Play_EOH;
                    bid = 0;

                    if (game.getWinner() == 1) {

                        #ifndef DEBUG_SOUND
                        playSFX(MusicSFX::SFX_Three_Inc);
                        #endif

                    }
                    else {

                        #ifndef DEBUG_SOUND
                        playSFX(MusicSFX::SFX_Three_Dec);
                        #endif

                    }
                    
                }
                else {
       
                    #ifndef DEBUG_SOUND
                    playSFX(MusicSFX::SFX_Click);
                    #endif
                    
                }

            }

            break;

        case GameState::Play_EOH:

            if (justPressed & A_BUTTON) {
        
                if (game.players[0].getCardCount() > 0) {

                    game.resetFrameCount();
                    gameState = static_cast<GameState>(static_cast<uint8_t>(GameState::Play_Player_00) + game.getWinner());
                    game.resetEOH();

                }
                else {
                    
                    gameState = GameState::Play_EOR_ShowScore;

                }

            }

            if (justPressed & B_BUTTON) {

                game.setPrevGameState(gameState);
                gameState = GameState::Pause_01;

            }

            if (game.players[0].getCardCount() == 0 && game.getFrameCount() % 4 == 0) {
            
                if (bid < 6) bid++;

            }

            break;

        case GameState::Play_EOR_ShowScore:

            for (uint8_t i = 0; i < 4; i++) {

                if (game.players[i].getTricks() == game.players[i].getBid()) {
                    game.players[i].addScore(10 + game.players[i].getTricks());
                }
                else {
                    game.players[i].addScore(game.players[i].getTricks());
                }

                if (i == 1 && !(game.getDirection() == Direction::Up && game.getCardCount() == 10)) {

                    if (game.players[i].getTricks() == game.players[i].getBid()) {

                        #ifndef DEBUG_SOUND
                        playSFX(MusicSFX::SFX_Victory);
                        #endif

                    }
                    else {

                        #ifndef DEBUG_SOUND
                        playSFX(MusicSFX::SFX_Death);
                        #endif

                    }

                }

            }

            bid = 0;
            gameState = GameState::Play_EOR;
            game.resetFrameCount();
                
            cookie.hasSavedGame = true;
            saveCookie(true);

            break;


        case GameState::Play_EOR:

            if (justPressed & A_BUTTON) {

                increaseCardCount();
                game.setDealer(game.getDealer() + 1);

            }

            if (justPressed & B_BUTTON) {

                game.setPrevGameState(gameState);
                gameState = GameState::Pause_01;

            }
            
            if (game.getFrameCount() == 64 && game.getDirection() == Direction::Up && game.getCardCount() == 10) {
                
                gameState = GameState::Play_EOG;

            }

            break;

        case GameState::Play_EOG:

            if (bid < 36 && game.getFrameCount() % 2 == 0) {
            
                bid++;

            }

            if (justPressed & A_BUTTON) {

                cookie.hasSavedGame = false;
                saveCookie(true);
                gameState = GameState::Title_Init;

            }

            break;

        case GameState::Pause_01:

            if (justPressed & RIGHT_BUTTON) {

                gameState = GameState::Pause_02;

            }

            if (justPressed & A_BUTTON) {

                cookie.hasSavedGame = true;
                saveCookie(true);
                gameState = GameState::Title_OptResume;
                game.setFrameCount(Constants::Title_Start_Action + 150);

            }

            if (justPressed & B_BUTTON) {

                gameState = game.getPrevGameState();

            }

            break;

        case GameState::Pause_02:

            if (justPressed & LEFT_BUTTON) {

                gameState = GameState::Pause_01;

            }

            if (justPressed & A_BUTTON || justPressed & B_BUTTON) {

                gameState = game.getPrevGameState();

            }

            break;

    }

}

void play(ArduboyGBase_Config<ABG_Mode::L4_Triplane> &a) {

    uint8_t currentPlane = a.currentPlane();
    if (a.needsUpdate()) play_Update();

    switch (gameState) {

        case GameState::Play_Deal_1 ... GameState::Play_Deal_10:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            SpritesU::drawOverwriteFX(110, 1, Images::Hand_Full, currentPlane);

            if (dealPhysics.x > dealPhysics.x2 && dealPhysics.y < dealPhysics.y2) {
                SpritesU::drawPlusMaskFX(dealPhysics.x - 5, dealPhysics.y - 4, Images::Rotate, ((static_cast<uint8_t>(dealPhysics.rot) % 36) * 3) + currentPlane);
            }

            break;

        case GameState::Play_Flip:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            SpritesU::drawOverwriteFX(110, 1, Images::Hand_Full, currentPlane);
            SpritesU::drawPlusMaskFX(110, 1, Images::Flip, ((game.getFrameCount() / 2) * 3) + currentPlane);
            break;

        case GameState::Play_Bid_01:
        case GameState::Play_Bid_11:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            renderBids(currentPlane);
            renderPlayerHands(currentPlane, LIGHT_GREY);
            SpritesU::drawPlusMaskFX(109, 0, Images::Cards_Bottom_Grey, game.getTurnedCard().getCardIndex() + currentPlane);
            SpritesU::drawOverwriteFX(34, 21, Images::Bid, (bid * 3) + currentPlane);
            break;

        case GameState::Play_Bid_00:
        case GameState::Play_Bid_02:
        case GameState::Play_Bid_03:
        case GameState::Play_Bid_10:
        case GameState::Play_Bid_12:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            renderBids(currentPlane);
            renderPlayerHands(currentPlane, DARK_GREY);
            SpritesU::drawPlusMaskFX(109, 0, Images::Cards_Bottom_Grey, game.getTurnedCard().getCardIndex() + currentPlane);
            break;


        case GameState::Play_Player_01:
        case GameState::Play_Player_11:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            renderBids(currentPlane);
            renderPlayerHands_ValidOptions(currentPlane);
            renderTableCards(currentPlane, Constants::NoWinner);
            SpritesU::drawPlusMaskFX(109, 0, Images::Cards_Bottom_Grey, game.getTurnedCard().getCardIndex() + currentPlane);
            break;

        case GameState::Play_Player_00:
        case GameState::Play_Player_02:
        case GameState::Play_Player_03:
        case GameState::Play_Player_10:
        case GameState::Play_Player_12:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            renderBids(currentPlane);
            renderTableCards(currentPlane, Constants::NoWinner);
            renderPlayerHands(currentPlane, DARK_GREY);
            SpritesU::drawPlusMaskFX(109, 0, Images::Cards_Bottom_Grey, game.getTurnedCard().getCardIndex() + currentPlane);
            break;

        case GameState::Play_EOH:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            renderBids(currentPlane, bid);
            renderTableCards(currentPlane, game.getWinner());
            renderPlayerHands(currentPlane, LIGHT_GREY);
            SpritesU::drawPlusMaskFX(109, 0, Images::Cards_Bottom, game.getTurnedCard().getCardIndex() + currentPlane);
            break;

        case GameState::Play_EOR:
        case GameState::Play_EOR_ShowScore:
        case GameState::Play_EOG:

            SpritesU::drawOverwriteFX(105, 0, Images::HUD, currentPlane);
            SpritesU::drawOverwriteFX(109, 36, Images::Numbers, ((game.getCardCount() - 1) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(111, 52, Images::Arrows, (static_cast<uint8_t>(game.getDirection()) * 3) + currentPlane);
            SpritesU::drawOverwriteFX(0, 0, Images::HighScores, currentPlane);

            if (currentPlane < 1) {     // Rener on Dark Grey only.

                for (uint8_t i = 0; i < 4; i++) {

                    uint8_t offset = 0;
                    uint8_t playerdIdx = (i + 1) % 4;

                    font3x5.setCursor(4 + (i * 26), 17);
                    if (game.players[playerdIdx].getLastScore() < 10) font3x5.print("0");                    
                    font3x5.print(game.players[playerdIdx].getLastScore());

                }

            }

            for (uint8_t i = 0; i < 4; i++) {

                uint8_t playerdIdx = (i + 1) % 4;

                if (i == 0 || currentPlane < 2) {   // White (you) and Light Grey (rest of players) ..

                    for (uint8_t j = 0; j < 6; j++) {
                        
                        SpritesU::drawOverwriteFX(14 + (i * 26), 18 + (8 * j), Images::Numbers_3x5, (game.players[playerdIdx].getScores(j) * 3) + currentPlane);

                    }

                }

            }


            SpritesU::drawPlusMaskFX(109, 0, Images::Cards_Bottom, game.getTurnedCard().getCardIndex() + currentPlane);

            if (gameState == GameState::Play_EOG) {

                int8_t bounce = static_cast<int8_t>(pgm_read_byte(Constants::Bounce + bid));
                SpritesU::drawPlusMaskFX(4, bounce - 8, Images::GameOver, currentPlane);

            }

            break;


        case GameState::Pause_01 ... GameState::Pause_02:
            {
                uint8_t idx = static_cast<uint8_t>(gameState) - static_cast<uint8_t>(GameState::Pause_01);

                SpritesU::drawOverwriteFX(0, 0, Images::Fire, (3 * ( ((game.getFrameCount()/6) % 16))) + currentPlane);                
                SpritesU::drawPlusMaskFX(0, 0, Images::Pause,(idx * 3) + currentPlane);

            }
            break;

    }

}


void playCard(uint8_t playerIdx, Card &tableCard, Card &selectedCard) {

    #ifdef DEBUG
        
        DEBUG_PRINT("Player ");
        DEBUG_PRINT(playerIdx);
        DEBUG_PRINT(" plays ");
        DEBUG_PRINT(selectedCard.getValue());
        printCard_Suit(selectedCard.getSuit());
        DEBUG_PRINTLN("");

    #endif

    tableCard.setSuit(selectedCard.getSuit());
    tableCard.setValue(selectedCard.getValue());

    selectedCard.setSuit(Suits::None);
    game.players[playerIdx].sort();
    game.setSelectedCard(Constants::NoCard);
    game.setPlayedCard(tableCard.getSortValue(), game.getPlayedCard(tableCard.getSortValue()) | 0x0F);

}

void increaseCardCount() {

    switch (game.getDirection()) {

        case Direction::Down:

            if (game.getCardCount() == 1) {

                game.setCardCount(2);
                game.setDirection(Direction::Up);

            }                    
            else {

                game.setCardCount(game.getCardCount() - 1);

            }

            gameState = GameState::Play_Init;

            break;

        case Direction::Up:

            if (game.getCardCount() == 10) {

                gameState = GameState::Play_EOG;
                bid = 0;

            }                    
            else {

                game.setCardCount(game.getCardCount() + 1);
                gameState = GameState::Play_Init;

            }

            break;

    }

}