int randomleg = 1;
int octopus_leg1 = 1;
bool octopus_leg1_reverse = false;
int octopus_leg2 = 1;
bool octopus_leg2_reverse = false;
int octopus_leg3 = 1;
bool octopus_leg3_reverse = false;
int octopus_leg4 = 1;
bool octopus_leg4_reverse = false;
int velocita = 10;

void Octopusleg(){
if(octopus_leg1 == 1){
  }else if(octopus_leg1 == 2){
      Sprites::drawPlusMask(46, 13, leg1_mask, 0);
  }else if(octopus_leg1 == 3){
      Sprites::drawPlusMask(46, 13, leg1_mask, 0);
      Sprites::drawPlusMask(39, 17, leg1_mask, 1);
  }else if(octopus_leg1 == 4){
      Sprites::drawPlusMask(46, 13, leg1_mask, 0);
      Sprites::drawPlusMask(39, 17, leg1_mask, 1);
      Sprites::drawPlusMask(33, 22, leg1_mask, 2);
  }else if(octopus_leg1 == 5){
      Sprites::drawPlusMask(46, 13, leg1_mask, 0);
      Sprites::drawPlusMask(39, 17, leg1_mask, 1);
      Sprites::drawPlusMask(33, 22, leg1_mask, 2);
      Sprites::drawPlusMask(28, 29, leg1_mask, 3);
  }

if(ingame == true){
 if(octopus_leg2 == 1){
  }else if(octopus_leg2 == 2){
      Sprites::drawPlusMask(63, 22, leg2bis_mask, 0);
  }else if(octopus_leg2 == 3){
      Sprites::drawPlusMask(63, 22, leg2bis_mask, 0);
      Sprites::drawPlusMask(61, 26, leg2bis_mask, 1);
  }else if(octopus_leg2 == 4){
      Sprites::drawPlusMask(63, 22, leg2bis_mask, 0);
      Sprites::drawPlusMask(61, 26, leg2bis_mask, 1);
      Sprites::drawPlusMask(60, 29, leg2bis_mask, 2);
  }else if(octopus_leg2 == 5){
      Sprites::drawPlusMask(63, 22, leg2bis_mask, 0);
      Sprites::drawPlusMask(61, 26, leg2bis_mask, 1);
      Sprites::drawPlusMask(60, 29, leg2bis_mask, 2);
      Sprites::drawPlusMask(59, 34, leg2bis_mask, 3);
  }else if(octopus_leg2 == 6){
      Sprites::drawPlusMask(63, 22, leg2bis_mask, 0);
      Sprites::drawPlusMask(61, 26, leg2bis_mask, 1);
      Sprites::drawPlusMask(60, 29, leg2bis_mask, 2);
      Sprites::drawPlusMask(59, 34, leg2bis_mask, 3);
      Sprites::drawPlusMask(57, 39, leg2bis_mask, 4);
  }
}
  
if(octopus_leg3 == 1){
  }else if(octopus_leg3 == 2){
      Sprites::drawPlusMask(75, 25, leg3_mask, 0);
  }else if(octopus_leg3 == 3){
      Sprites::drawPlusMask(75, 25, leg3_mask, 0);
      Sprites::drawPlusMask(74, 30, leg3_mask, 1);
  }else if(octopus_leg3 == 4){
      Sprites::drawPlusMask(75, 25, leg3_mask, 0);
      Sprites::drawPlusMask(74, 30, leg3_mask, 1);
      Sprites::drawPlusMask(76, 34, leg3_mask, 2);
  }else if(octopus_leg3 == 5){
      Sprites::drawPlusMask(75, 25, leg3_mask, 0);
      Sprites::drawPlusMask(74, 30, leg3_mask, 1);
      Sprites::drawPlusMask(76, 34, leg3_mask, 2);
      Sprites::drawPlusMask(77, 38, leg3_mask, 3);
}

 if(octopus_leg4 == 1){
  }else if(octopus_leg4 == 2){
      Sprites::drawPlusMask(92, 29, leg4_mask, 0);
  }else if(octopus_leg4 == 3){
      Sprites::drawPlusMask(92, 29, leg4_mask, 0);
      Sprites::drawPlusMask(91, 33, leg4_mask, 1);
  }else if(octopus_leg4 == 4){
      Sprites::drawPlusMask(92, 29, leg4_mask, 0);
      Sprites::drawPlusMask(91, 33, leg4_mask, 1);
      Sprites::drawPlusMask(96, 38, leg4_mask, 2);
  }
}

void Die(){
  Sprites::drawPlusMask(61, 28, leg2_mask, 0);
}

void Octopus(){
  Sprites::drawPlusMask(58, 13, octopus_mask, 0);

  if(arduboy.everyXFrames(velocita) && ingame == true){
  randomleg = random(1,5);
  sound.tones(OctopusSound);
  
  if(randomleg == 1){
    if(octopus_leg1 < 5 && octopus_leg1_reverse == false){
      ++octopus_leg1;
    }
    if(octopus_leg1 > 0 && octopus_leg1_reverse == true){
      --octopus_leg1;
    }
    if(octopus_leg1 == 1){
      octopus_leg1_reverse = false;
    }
    if(octopus_leg1 == 5){
      octopus_leg1_reverse = true;
    }
  }

    if(randomleg == 2){
    if(octopus_leg2 < 6 && octopus_leg2_reverse == false){
      ++octopus_leg2;
    }
    if(octopus_leg2 > 0 && octopus_leg2_reverse == true){
      --octopus_leg2;
    }
    if(octopus_leg2 == 1){
      octopus_leg2_reverse = false;
    }
    if(octopus_leg2 == 6){
      octopus_leg2_reverse = true;
    }
  }

    if(randomleg == 3){
    if(octopus_leg3 < 5 && octopus_leg3_reverse == false){
      ++octopus_leg3;
    }
    if(octopus_leg3 > 0 && octopus_leg3_reverse == true){
      --octopus_leg3;
    }
    if(octopus_leg3 == 1){
      octopus_leg3_reverse = false;
    }
    if(octopus_leg3 == 5){
      octopus_leg3_reverse = true;
    }
  }

    if(randomleg == 4){
    if(octopus_leg4 < 4 && octopus_leg4_reverse == false){
      ++octopus_leg4;
    }
    if(octopus_leg4 > 0 && octopus_leg4_reverse == true){
      --octopus_leg4;
    }
    if(octopus_leg4 == 1){
      octopus_leg4_reverse = false;
    }
    if(octopus_leg4 == 4){
      octopus_leg4_reverse = true;
    }
  }
  }
  Octopusleg();

  if(ingame == false){
    Die();
  }
}

void OctopusCollision(){
  if(octopus_leg1 == 5 && subMove == 2){
   getPoint = 0;
   ingame = false;
  }
  if(octopus_leg2 == 6 && subMove == 3){
   getPoint = 0;
   ingame = false;
  }
  if(octopus_leg3 == 5 && subMove == 4){
   getPoint = 0;
   ingame = false;
  }
  if(octopus_leg4 == 4 && subMove == 5){
   getPoint = 0;
   ingame = false;
  }
}
