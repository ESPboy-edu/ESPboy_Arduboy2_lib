#include <ArduboyFX.h>  


void renderPlayerHands(uint8_t currentPlane, uint8_t color) {

    uint8_t cardCount = game.players[Constants::HumanPlayer].getCardCount();

    uint8_t xStart = 48 - (cardCount * 4);

    for (uint8_t i = 0; i < cardCount; i++) {

        if (game.players[Constants::HumanPlayer].getCard(i).getSuit() != Suits::None) {

            if (color == LIGHT_GREY) {
                SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom, game.players[Constants::HumanPlayer].getCard(i).getCardIndex() + currentPlane);
            }
            else {
                SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom_Grey, game.players[Constants::HumanPlayer].getCard(i).getCardIndex() + currentPlane);
            }

        }

    }

    if (color == LIGHT_GREY) {

        if (game.players[2].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(0, 10, Images::Hand_Left, (game.players[2].getCardCount() * 3) + currentPlane);
        }

        if (game.players[3].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(31, -3, Images::Hand_Top, (game.players[3].getCardCount() * 3) + currentPlane);
        }

        if (game.players[0].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(100, 10, Images::Hand_Right, (game.players[0].getCardCount() * 3) + currentPlane);
        }

    }
    else {

        if (game.players[2].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(0, 10, Images::Hand_Left, ((game.players[2].getCardCount() + 11) * 3) + currentPlane);
        }

        if (game.players[3].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(31, -3, Images::Hand_Top, ((game.players[3].getCardCount() + 11) * 3) + currentPlane);
        }

        if (game.players[0].getCardCount() > 0) {
            SpritesU::drawOverwriteFX(100, 10, Images::Hand_Right, ((game.players[0].getCardCount() + 11) * 3) + currentPlane);
        }

    }

}

void renderPlayerHands_ValidOptions(uint8_t currentPlane) {

    uint8_t playerCardCount = game.players[Constants::HumanPlayer].getCardCount();
    uint8_t xStart = 48 - (playerCardCount * 4);

    if (playerCardCount == 1) {

        game.setSelectedCard(0);
        renderHumanHand_LightGrey(currentPlane);

    }
    else {

        uint8_t tableCardCount = game.getTableCardCount();

        if (tableCardCount == 0) {

            renderHumanHand_LightGrey(currentPlane);

        }
        else {

            bool hasSuit = game.players[Constants::HumanPlayer].hasSuit(game.getTableCard(game.getFirstPlayer()).getSuit());
            Card &firstCard = game.getTableCard(game.getFirstPlayer());

            if (hasSuit) {

                for (uint8_t i = 0; i < playerCardCount; i++) {

                    Card &card = game.players[Constants::HumanPlayer].getCard(i);

                    if (card.getSuit() != Suits::None) {

                        if (card.getSuit() == firstCard.getSuit()) {

                            if (game.getSelectedCard() == Constants::NoCard || game.getSelectedCard() == i) {

                                game.setSelectedCard(i);
                                SpritesU::drawPlusMaskFX(xStart + (i * 8), 43, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

                            }
                            else {

                                SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

                            }

                            game.setValidCard(i, true);

                        }
                        else {

                            game.setValidCard(i, false);
                            SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom_Grey, card.getCardIndex() + currentPlane);

                        }

                    }

                }

            }
            else {

                renderHumanHand_LightGrey(currentPlane);

            }

        }

    }

    SpritesU::drawOverwriteFX(0, 10, Images::Hand_Left, (game.players[2].getCardCount() * 3) + currentPlane);
    SpritesU::drawOverwriteFX(31, -3, Images::Hand_Top, (game.players[3].getCardCount() * 3) + currentPlane);
    SpritesU::drawOverwriteFX(100, 10, Images::Hand_Right, (game.players[0].getCardCount() * 3) + currentPlane);


}

void renderHumanHand_LightGrey(uint8_t currentPlane) {
  
    uint8_t cardCount = game.players[Constants::HumanPlayer].getCardCount();
    uint8_t xStart = 48 - (cardCount * 4);

    for (uint8_t i = 0; i < cardCount; i++) {

        Card &card = game.players[Constants::HumanPlayer].getCard(i);

        if (card.getSuit() != Suits::None) {

            game.setValidCard(i, true);

            if (game.getSelectedCard() == Constants::NoCard || game.getSelectedCard() == i) {

                game.setSelectedCard(i);
                SpritesU::drawPlusMaskFX(xStart + (i * 8), 43, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

            }
            else {

                SpritesU::drawPlusMaskFX(xStart + (i * 8), 47, Images::Cards_Bottom, card.getCardIndex() + currentPlane);

            }

        }

    }

}

void renderTableCards(uint8_t currentPlane, uint8_t winner) {

    if (winner == Constants::NoWinner) {
            
        if (game.getTableCard(3).getSuit() != Suits::None)  {
            SpritesU::drawPlusMaskFX(38, 9, Images::Cards_Bottom, game.getTableCard(3).getCardIndex() + currentPlane); 
        }

        if (game.getTableCard(1).getSuit() != Suits::None)  {
            SpritesU::drawPlusMaskFX(49, 19, Images::Cards_Bottom, game.getTableCard(1).getCardIndex() + currentPlane); 
        }

        if (game.getTableCard(0).getSuit() != Suits::None)  {
            SpritesU::drawPlusMaskFX(60, 13, Images::Cards_Right, game.getTableCard(0).getCardIndex() + currentPlane); 
        }

        if (game.getTableCard(2).getSuit() != Suits::None)  {
            SpritesU::drawPlusMaskFX(20, 21, Images::Cards_Left, game.getTableCard(2).getCardIndex() + currentPlane); 
        }

    }
    else {
 
        if (winner == 3 && game.getFrameCount() % 64 < 32) {
            SpritesU::drawPlusMaskFX(38, 9, Images::Cards_Bottom, game.getTableCard(3).getCardIndex() + currentPlane); 
        }
        else {
            SpritesU::drawPlusMaskFX(38, 9, Images::Cards_Bottom_Grey, game.getTableCard(3).getCardIndex() + currentPlane); 
        }

        if (winner == 1 && game.getFrameCount() % 64 < 32) {
            SpritesU::drawPlusMaskFX(49, 19, Images::Cards_Bottom, game.getTableCard(1).getCardIndex() + currentPlane); 
        }
        else {
            SpritesU::drawPlusMaskFX(49, 19, Images::Cards_Bottom_Grey, game.getTableCard(1).getCardIndex() + currentPlane); 
        }

        if (winner == 0 && game.getFrameCount() % 64 < 32) {
            SpritesU::drawPlusMaskFX(60, 13, Images::Cards_Right, game.getTableCard(0).getCardIndex() + currentPlane); 
        }
        else {
            SpritesU::drawPlusMaskFX(60, 13, Images::Cards_Right_Grey, game.getTableCard(0).getCardIndex() + currentPlane); 
        }

        if (winner == 2 && game.getFrameCount() % 64 < 32) {
            SpritesU::drawPlusMaskFX(20, 21, Images::Cards_Left, game.getTableCard(2).getCardIndex() + currentPlane); 
        }
        else {
            SpritesU::drawPlusMaskFX(20, 21, Images::Cards_Left_Grey, game.getTableCard(2).getCardIndex() + currentPlane); 
        }

    }

}

void renderBids(uint8_t currentPlane) {
    
    renderBids(currentPlane, 0);

}

void renderBids(uint8_t currentPlane, uint8_t offset) {

    uint8_t dealer = game.getDealer() % 4;

    if (game.players[0].getBid() != Constants::NoBid) {
        
        SpritesU::drawOverwriteFX(92 + offset, 18, Images::Score_Numbers_Right, (game.players[0].getBid() * 3) + currentPlane); 
        SpritesU::drawOverwriteFX(92 + offset, 24, Images::Score_Numbers_Right, 33 + currentPlane); 
        SpritesU::drawOverwriteFX(92 + offset, 28, Images::Score_Numbers_Right, (game.players[0].getTricks() * 3) + currentPlane); 
        
        if (dealer == 0) {
            SpritesU::drawOverwriteFX(93 + offset, 36, Images::Dealer, currentPlane); 
        }

    }
    else {
        
        if (dealer == 0) {
            SpritesU::drawOverwriteFX(93 + offset, 24, Images::Dealer, currentPlane); 
        }

    }

    if (game.players[1].getBid() != Constants::NoBid) {

        SpritesU::drawOverwriteFX(44, 40 + (offset * 3 > 16 ? 16 : offset * 3), Images::Score_Numbers_Bottom, (game.players[1].getTricks() * 3) + currentPlane); 
        SpritesU::drawOverwriteFX(50, 40 + (offset * 3 > 16 ? 16 : offset * 3), Images::Score_Numbers_Bottom, 33 + currentPlane); 
        SpritesU::drawOverwriteFX(54, 40 + (offset * 3 > 16 ? 16 : offset * 3), Images::Score_Numbers_Bottom, (game.players[1].getBid() * 3) + currentPlane); 
        
        if (dealer == 1) {
            SpritesU::drawOverwriteFX(62, 41 + (offset * 3 > 16 ? 16 : offset * 3), Images::Dealer, currentPlane); 
        }

    }
    else {
        
        if (dealer == 1) {
            SpritesU::drawOverwriteFX(50, 41 + (offset * 3 > 16 ? 16 : offset * 3), Images::Dealer, currentPlane); 
        }

    }

    if (game.players[2].getBid() != Constants::NoBid) {

        SpritesU::drawOverwriteFX(6 - offset, 18, Images::Score_Numbers_Left, (game.players[2].getTricks() * 3) + currentPlane); 
        SpritesU::drawOverwriteFX(6 - offset, 24, Images::Score_Numbers_Left, 33 + currentPlane); 
        SpritesU::drawOverwriteFX(6 - offset, 28, Images::Score_Numbers_Left, (game.players[2].getBid() * 3) + currentPlane); 
        
        if (dealer == 2) {
            SpritesU::drawOverwriteFX(7 - offset, 36, Images::Dealer, currentPlane); 
        }

    }
    else {
        
        if (dealer == 2) {
            SpritesU::drawOverwriteFX(7 - offset, 24, Images::Dealer, currentPlane); 
        }

    }

    if (game.players[3].getBid() != Constants::NoBid) {

        SpritesU::drawOverwriteFX(44, 6 - offset, Images::Score_Numbers_Bottom, (game.players[3].getTricks() * 3) + currentPlane); 
        SpritesU::drawOverwriteFX(50, 6 - offset, Images::Score_Numbers_Bottom, 33 + currentPlane); 
        SpritesU::drawOverwriteFX(54, 6 - offset, Images::Score_Numbers_Bottom, (game.players[3].getBid() * 3) + currentPlane); 

        if (dealer == 3) {
            SpritesU::drawOverwriteFX(62, 7 - offset, Images::Dealer, currentPlane); 
        }

    }
    else {

        if (dealer == 3) {
            SpritesU::drawOverwriteFX(50, 7 - offset, Images::Dealer, currentPlane); 
        }

    }

}