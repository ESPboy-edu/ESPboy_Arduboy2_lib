


void updatePlayer(uint8_t frameCount) {

    switch (player.getPlayerMode()) {

        case PlayerMode::Rotate:

            if (frameCount % 6 != 0) return;

            switch (player.getDirectionMovement()) {

                case -1:

                    player.setDirection(player.getDirection() - 1);

                    if (player.getDirection() == 0) {

                        player.setDirectionMovement(1);

                    }

                    break;

                case 1:

                    player.setDirection(player.getDirection() + 1);

                    if (player.getDirection() == 12) {

                        player.setDirectionMovement(-1);

                    }

                    break;

            }



            // Collision with an enemy?

            {
                Rect recPlayer;
                recPlayer.x = player.getX_Screen();
                recPlayer.width = Constants::Player_Width_Full;
                recPlayer.height = Constants::Player_Width_Full;

                if (player.getY() >= Constants::Player_YMax - Constants::Player_Offset) {
                    recPlayer.y = player.getY_Screen() + (Constants::Player_Offset / 16);
                }
                else {
                    recPlayer.y = player.getY_Screen();
                }

                detectEnemyHits(recPlayer);

            }

            break;


        case PlayerMode::Moving:

            player.setX(player.getX() + Constants::Player_Movement_X[static_cast<uint8_t>(player.getMovement())]);
            player.setY(player.getY() + Constants::Player_Movement_Y[static_cast<uint8_t>(player.getMovement())]);


            if (player.getX() <= Constants::Player_XMin || player.getX()> Constants::Player_XMax) {
                
                if (player.getX() <= Constants::Player_XMin) {
                    player.setX(Constants::Player_XMin);
                }
                
                if (player.getX() > Constants::Player_XMax) {
                    player.setX(Constants::Player_XMax);
                }

                player.setMovement(static_cast<PlayerMovement>(Constants::Player_AltDirection[static_cast<uint8_t>(player.getMovement())]));
                
            }


            if (player.getY() <= Constants::Player_YMin) {

                player.setPlayerMode(PlayerMode::Rotate);
                player.setDirection(6);
                player.setY(Constants::Player_YMin);
                scoreMax = 0;

            }

            else if (player.getY() >= Constants::Player_YMax) {

                player.setY(Constants::Player_YMax);
                player.setPlayerMode(PlayerMode::Rotate);
                player.setDirection(6);
                player.setY(player.getY() - Constants::Player_Offset);
                scoreMax = 0;

            }
            
            if (player.getX_Screen() >= -4) {
                    
                if (gridPosition == 0) {

                    int8_t x = (player.getX_Screen() - gridPosition + 1) / Constants::CellWidth;
                    int8_t y = (player.getY_Screen() + 1) / Constants::CellHeight;
                    setGrid(x + 1, y, CellOwner::Player);

                    x = (player.getX_Screen() - gridPosition + Constants::Player_Width - 1) / Constants::CellWidth;
                    y = (player.getY_Screen() + 1) / Constants::CellHeight;
                    setGrid(x + 1, y, CellOwner::Player);

                    x = (player.getX_Screen() - gridPosition + 1) / Constants::CellWidth;
                    y = (player.getY_Screen() -  Constants::Player_Width - 1) / Constants::CellHeight;
                    setGrid(x + 1, y, CellOwner::Player);

                    x = (player.getX_Screen() - gridPosition + Constants::Player_Width - 1) /Constants::CellWidth;
                    y = (player.getY_Screen() + Constants::Player_Width - 1) / Constants::CellHeight;
                    setGrid(x + 1, y, CellOwner::Player);

                }
                else {

                    int8_t x = (player.getX_Screen() - gridPosition + Constants::CellWidth + 1) / Constants::CellWidth;
                    int8_t y = (player.getY_Screen() + 1) / Constants::CellHeight;
                    setGrid(x, y, CellOwner::Player);

                    x = (player.getX_Screen() - gridPosition + Constants::Player_Width + Constants::CellWidth - 1) / Constants::CellWidth;
                    y = (player.getY_Screen() + 1) / Constants::CellHeight;
                    setGrid(x, y, CellOwner::Player);

                    x = (player.getX_Screen() - gridPosition + Constants::CellWidth + 1) / Constants::CellWidth;
                    y = (player.getY_Screen() + Constants::Player_Width - 1) / Constants::CellHeight;
                    setGrid(x, y, CellOwner::Player);

                    x = (player.getX_Screen() - gridPosition + Constants::Player_Width + Constants::CellWidth - 1) / Constants::CellWidth;
                    y = (player.getY_Screen() + Constants::Player_Width - 1) / Constants::CellHeight;
                    setGrid(x, y, CellOwner::Player);
                }

            }      


            // Collision with an enemy?

            Rect recPlayer;
            recPlayer.x = player.getX_Screen();
            recPlayer.y = player.getY_Screen();
            recPlayer.width = Constants::Player_Width_Full;
            recPlayer.height = Constants::Player_Width_Full;

            detectEnemyHits(recPlayer);

            break;

    }

}


bool updateEnemy(Enemy &enemy, uint8_t frameCount) {

    switch (enemy.getSpeed()) {

        case Speed::Slow:
            if (frameCount % 5 != 0) return false;

        case Speed::Medium:
            if (frameCount % 4 != 0) return false;

        case Speed::Fast:
            if (frameCount % 3 != 0) return false;

    }

    if (!enemy.isActive()) return false;


    enemy.setX(enemy.getX() + Constants::Enemy_Movement_X[static_cast<uint8_t>(enemy.getMovement())]);
    enemy.setY(enemy.getY() + Constants::Enemy_Movement_Y[static_cast<uint8_t>(enemy.getMovement())]);

    if (enemy.getY() < 24 || enemy.getY()  > 924) {

        if (enemy.getY()  < 24) {

            enemy.setY(24);

        }

        if (enemy.getY() > 924) {

            enemy.setY(924);

        }

        enemy.setMovement(static_cast<Movement>(Constants::Enemy_AltDirection[static_cast<uint8_t>(enemy.getMovement())]));

    }

    return (enemy.getX() >= 1568);

}


void updateAndRenderScores(uint8_t frameCount, uint8_t currentPlane) {

    if (scoresNeedRendering) {
            
        scoresNeedRendering = false;

        for (Score &score : scores) {

            if (score.isActive()) {

                SpritesU::drawPlusMaskFX(score.getX_Screen(), score.getY_Screen(), Images::Plus8, (((score.getScore() * 3) + (score.getCounter() / Constants::ScoreFade)) * 3) + currentPlane);
                
                bool active = score.update();

                if (active) {
                    scoresNeedRendering = true;
                }

            }

        }

    }

}

void detectEnemyHits(Rect &recPlayer) {

    for (Enemy &enemy : enemies) {

        if (enemy.isActive()) {

            Rect recEnemy;
            recEnemy.x = enemy.getX_Screen();
            recEnemy.y = enemy.getY_Screen();
            recEnemy.width = Constants::EnemyWidth + 1;
            recEnemy.height = Constants::EnemyWidth + 1;

            if (Arduboy2::collide(recPlayer, recEnemy)) {

                launchParticles(recEnemy.x + 4, recEnemy.y + 4);
                enemy.setActive(false);

                bool bActive = false;

                for (Enemy &enemy : enemies) {

                    if (enemy.isActive()) bActive = true;

                }

                if (bActive) {

                    launchEnemyCounter = 1; // launchImmediately

                }

                for (Score &thisScore : scores) {

                    if (!thisScore.isActive()) {

                        thisScore.setActive(true);
                        thisScore.setCounter(0);
                        thisScore.setScore(scoreMax);

                        if (enemy.getX() < 120) {
                            thisScore.setX(120);
                        }
                        else {
                            thisScore.setX(enemy.getX());
                        }

                        if (enemy.getY() > 896) {
                            thisScore.setY(896);
                        }
                        else {
                            thisScore.setY(enemy.getY());
                        }                                

                        thisScore.setMovement(player.getMovement());
                        scoresNeedRendering = true;

                        if (scoreMax < 4) scoreMax++;
                        cookie.score = cookie.score + ((scoreMax + 1) * 50);
                        playSFX(MusicSFX::SFX_Three_Inc);
                        break;

                    }

                }

            }

        }

    }
    
}