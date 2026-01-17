// ================ Gameplay Constants ================

const int mapWidth = 128;
const int mapHeight = 64;
const int lastWave = 9;
const int threatLevelMax = 8;
const int zombiesSlainTarget = 64;
const int beaconAnimationTime = 5;
const int beaconAnimationFrameCount = 4;
const int outroTime = 128;
const int explosionSpeed = 3;

// ================ Player Constants ================

const int playerSize = 6;
const int animationSpeed = 5;
const int playerIdleFrames = 3;
const int playerRunFrames = 6;
const int playerPosXStart = 64;
const int playerPosYStart = 32;
const int playerHealthStart = 8;
const int playerRegenTime = 5;
const int playerAmmoStart = 16;
const int fireTimeStart = 4;
const int reloadTimeStart = 80;
const int playerShieldTimeStart = 15;

// ================ Horde Constants ================

const int hordeCountStart = 8;
const int hordeCountMax = 64;
const int hordeIncreaseTimeStart = 300;

const int shamblerAnimationFrameMax = 3;

// ================ Gameplay Variables ================

int gamePlayState = 0;
int introStage = 0;
int currentWave = 0;
int currentThreatLevel = 0;
int zombiesSlain = 0;
int zombiesSlainTotal = 0;
int playTime = 0;
int timeCounter = 0;
int damageTaken = 0;
int supportObjectPosX = 0;
int supportObjectPosY = 0;
int outroCounter = 0;

bool crateIsPlaced = false;
bool beaconIsPlaced = false;
bool beaconIsCalled = false;

int beaconAnimationCounter = 0;
int beaconAnimationFrame = 0;

int availableUpgrades[4];
int chosenUpgrade = 0;
int lastUpgrade = 0;
int upgradeTimeCounter = 0;

// ================ Player Variables ================

int playerPosX = playerPosXStart;
int playerPosY = playerPosYStart;
int playerFrame = 0;
int playerFrameCounter = 0;
int playerHealthMax = playerHealthStart;
int playerHealth = 0;
int regenCounter = 0;
int playerAmmoMax = playerAmmoStart;
int playerAmmo = 0;
int ammoPenetration = 1;
int ammoDamage = 1;
int fireTime = fireTimeStart;
int fireCounter = fireTime;
int reloadTime = reloadTimeStart;
int reloadTimeShell = 0;
int reloadCounter = 0;
int playerArmour = 0;
int playerShieldTime = playerShieldTimeStart;
int playerShieldCounter = 0;

bool playerRegenerating = true;
bool playerReloading = true;
bool weaponOverheated = false;
bool playerFacesLeft = false;
bool playerIsRunning = false;
bool playerWasRunning = false;

// ================ Horde Variables ================

int hordeCount = hordeCountStart;
int hordeIncreaseTime = hordeIncreaseTimeStart;
int hordeIncreaseCounter = 0;
int hordeIndexOffset = 0;
int hordeIndexOffsetMax = hordeCountMax / 8;
int zombieSlayChance = 1;
int zombieDamage = 1;

// ================ Entity Arrays ================

int zombies[hordeCountMax][3];
// isAlive/animationFrame, xPos, yPos

int playerProjectiles[16][4];
// xDir/0 == inactive, xPos, yPos, penetrationPoints

int explosions[16][4];
// animation frame / 0 == inactive, animationCounter, xPos, yPos

// ================ Gameplay Functions ================

void SetGamePlayState(int newState){
  // Set gameplay state to a new state
  gamePlayState = newState;
}

void CalculateReloadTime(){
  // Function to calculate time to reload a single piece of ammo
  // Time is multiplied by 10 to better handle rounding.
  reloadTimeShell = reloadTime * 10 / playerAmmoMax;
}

void PrepareGameplay(){
  // Reset variables in preparation for gameplay

  // Reset game score
  ResetGameScore();

  // Reset gameplay variables
  currentWave = 0;
  crateIsPlaced = false;
  beaconIsPlaced = false;
  beaconIsCalled = false;
  beaconAnimationCounter = 0;
  beaconAnimationFrame = 0;
  lastUpgrade = 0;
  zombiesSlain = 0;
  zombiesSlainTotal = 0;
  currentThreatLevel = 0;
  playTime = 0;
  timeCounter = 0;
  outroCounter = 0;
  damageTaken = 0;

  // Reset player variables
  playerPosX = playerPosXStart;
  playerPosY = -10;

  playerHealthMax = playerHealthStart;
  playerHealth = 0;
  playerAmmoMax = playerAmmoStart;
  playerAmmo = 0;
  reloadTime = reloadTimeStart;
  fireTime = fireTimeStart;
  fireCounter = fireTime;
  ammoPenetration = 1;
  ammoDamage = 1;
  playerArmour = 0;
  playerShieldTime = playerShieldTimeStart;
  playerFrame = 0;
  playerFrameCounter = 0;

  playerFacesLeft = false;
  playerIsRunning = false;
  playerWasRunning = false;

  playerRegenerating = true;
  playerReloading = true;
  weaponOverheated = false;

  // Calculate reload time
  CalculateReloadTime();

  // Clear zombies
  for(int i = 0; i < hordeCountMax; i++){
    zombies[i][0] = 0;
    zombies[i][1] = 0;
    zombies[i][2] = 0;
  }

  // Reset shambler variables
  hordeCount = hordeCountStart;
  hordeIncreaseTime = hordeIncreaseTimeStart;
  hordeIncreaseCounter = 0;
  zombieSlayChance = 1;
  zombieDamage = 1;

  // Clear projectiles
  for(int i = 0; i < 16; i++){
    playerProjectiles[i][0] = 0;
    playerProjectiles[i][1] = 0;
    playerProjectiles[i][2] = 0;
    playerProjectiles[i][3] = 0;
  }

  // Clear explosions
  for(int i = 0; i < 16; i++){
    explosions[i][0] = 0;
    explosions[i][1] = 0;
    explosions[i][2] = 0;
    explosions[i][3] = 0;
  }

  // When gameplay has been prepared, play intro
  introStage = 0;
  SetGamePlayState(0);
}

void CalculateThreatLevel(){
  // Calculate current threat level
  // Number of zombies plus number of times slayChance and damage have been empowered
  currentThreatLevel = hordeCount / 8 + zombieSlayChance + zombieDamage - 2;
}

void EmpowerHorde(){
  // 50% chance that zombies will be empowered
  if(random(0, 2) == 0) return;

  switch(random(0, 3)){
    case 0:
    // Increase the number of zombies
    hordeCount +=  max(1, 20 - hordeIncreaseTime / 30);

    // If too many zombies would be initiated, limit them to the maximum allowed
    if(hordeCount > hordeCountMax
  ) hordeCount = hordeCountMax;
    break;

    case 1:
    // Shamblers become more difficult to slay, capped to the current wave
    if(zombieSlayChance <= currentWave / 4) zombieSlayChance++;
    break;

    case 2:
    // Shamblers hit harder, capped to the current wave
     if(zombieDamage <= currentWave / 4) zombieDamage++;
    break;
  }

  // Calculate new threat level
  CalculateThreatLevel();
}

void AbortReload(){
  // Set reloading to false and reset the time counter
  playerReloading = false;
  reloadCounter = 0;
}

void AbortRegen(){
  // Set regenerating to false and reset the time counter
  playerRegenerating = false;
  regenCounter = 0;
}

void BeginNewWave(){
  // Tick up current wave and change gamePlayState to 1
  currentWave++;
  SetGamePlayState(1);
}

void ApplyUpgrade(){
  // The player has picked the upgrade they want
  switch(availableUpgrades[chosenUpgrade]){
    case 0:
    // Begin regenerating to full health
    playerRegenerating = true;
    break;

    case 1:
    // Call for a teleportation beacon
    beaconIsCalled = true;
    break;

    case 10:
    // Health Up
    playerHealthMax += 2;
    playerHealth += 2;

    // Player health is capped to 16
    if(playerHealthMax > 16) playerHealthMax = 16;
    break;

    case 11:
    // Ammo up, player ammo is capped to 32
    if(playerAmmoMax < 32) playerAmmoMax += 4;

    // Player gets a reload as a freebie
    playerAmmo = playerAmmoMax;

    // Calculate new reload time
    CalculateReloadTime();

    // If the player was reloading when the upgrade was picked, abort the relaod
    if(playerReloading) AbortReload();
    break;

    case 12:
    // Penetration up
    ammoPenetration++;
    break;

    case 13:
    // Reload time down
    if(reloadTime > 32) reloadTime -= 16;

    // Calculate new reload time
    CalculateReloadTime();
    break;

    case 14:
    // Increase time in frames to tick up shambler count
    hordeIncreaseTime += 150;
    break;

    case 15:
    // Shield duration up
    playerShieldTime += 5;
    break;

    case 16:
    // Shambler count down. The number of active zombies is halved, to a minimum of 8
    hordeCount = max(8, hordeCount / 2);

    // Calculate new threat level
    CalculateThreatLevel();
    break;

    case 17:
    // Armour up
    playerArmour++;
    break;

    case 18:
    // Ammo damage up
    ammoDamage++;
    break;
  }
  // Write down what upgrade whas chosen
  lastUpgrade = availableUpgrades[chosenUpgrade];

  // Reset upgrade time counter
  upgradeTimeCounter = 0;

  // Begin new wave
  BeginNewWave();
}

void RollUpgrades(){
  // The first option is either regenerate to full health, or call in teleportation beacon in later waves
  if(currentWave < lastWave) availableUpgrades[0] = 0;
  else availableUpgrades[0] = 1;

  // Randomize 3 upgrades to choose from
  for(int i = 0; i < 3; i++){
    
    // Generate a random upgrade
    int upgr = 10 + random(0, 9);

    // Boolean for the following while loop
    bool containsUpgrade = true;
    
    // Check that upgrade isn't already available. The while loop will continue until a new upgrade has been picked to avoid doubles
    while(containsUpgrade){

      // Set boolean to false
      containsUpgrade = false;

      // Loop through the array of upgrades to check if it already contains the currently rolled upgrade
      for(int ii = 0; ii < 3; ii++) if(availableUpgrades[ii + 1] == upgr) containsUpgrade = true;

      // Flag if the rolled upgrade is equal to the upgrade the player picked last upgrade round
      if(upgr == lastUpgrade) containsUpgrade = true;

      // If upgrade is flagged, tick upp the rolled upgrade
      if(containsUpgrade) upgr++;
      
      // If number would go out of bounds, loop around to 10
      if(upgr > 18) upgr = 10;
    }

    // Assign available upgrade. The first object in the array is already set, so the index is offset by 1
    availableUpgrades[i + 1] = upgr;
  }
}

void BeginUpgrading(){
  // Function is called when the player is about to begin upgrading

  // Make sure the screen isn't inverted
  arduboy.invert(false);

  // Set indicator to 1, 2nd in the line of upgrades
  chosenUpgrade = 1;

  // Change gameplay state
  SetGamePlayState(2);

  // Randomize available upgrades
  RollUpgrades();
}

void CrateIsPickedUp(){
  // Function is called when the player picks up a support crate
  // Remove the crate and start upgrading
  crateIsPlaced = false;
  BeginUpgrading();
}

void BeaconIsTouched(){
  // Function is called when the player is close enough to a teleport beacon to teleport away

  // Tick up shield counter so zombies won't cause damage
  playerShieldCounter = 30;

  // Remove the beacon
  beaconIsPlaced = false;

  // Set off explosion at beacon's position
  SetOffExplosion(supportObjectPosX, supportObjectPosY);

  // Set off explosion at player's position
  SetOffExplosion(playerPosX, playerPosY);

  // Turn off inverted colours
  arduboy.invert(false);

  // Start outro
  CalculateGameScore(true, currentWave, zombiesSlainTotal, playTime, damageTaken);
  SetGamePlayState(3);
}

void PlaceSupportObject(int objectType){
  // This function teleports in a support object based on the type specified
  bool objectHasBeenChosen = false;

  switch(objectType){
    case 0:
    // Support crate
    crateIsPlaced = true;
    objectHasBeenChosen = true;
    break;

    case 1:
    // Teleport beacon
    beaconIsPlaced = true;
    objectHasBeenChosen = true;
    break;
  }

  // If no valid object has been chosen, return out of function
  if(!objectHasBeenChosen) return;

  // Randomize the object's x and y positions
  supportObjectPosX = 36 + random(0, 64);
  supportObjectPosY = 20 + random(0, 32);

  // Check if the object has spawned too close to the player
  if(abs(playerPosX - supportObjectPosX) < 16 && abs(playerPosY - supportObjectPosY) < 16){
    // If the object has spawned too close to the player, shunt it to the left or to the right
    if(supportObjectPosX < 64) supportObjectPosX += 24;
    else supportObjectPosX -= 24;
  }

  // Set off an explosion at the object's position when it teleports in
  SetOffExplosion(supportObjectPosX, supportObjectPosY);
}

void ClearWave(){
  // Reset zombies slain counter
  zombiesSlain = 0;

  // Empower zombies
  EmpowerHorde();

  // If a teleport beacon has been called, place teleport beacon, else place down upgrade crate
  if(beaconIsCalled && !beaconIsPlaced) PlaceSupportObject(1);
  else if(!crateIsPlaced) PlaceSupportObject(0);
}

void SetOffExplosion(int xPos, int yPos){
  // Set off an explosion at the specified coordinates
  // Loop through the explosions array and check if a new explosion can be set off
  for(int i = 0; i < 16; i++){
    if(explosions[i][0] == 0){
      // Free explosion space found, set off explosion
      explosions[i][0] = 1;
      explosions[i][1] = explosionSpeed;
      explosions[i][2] = xPos;
      explosions[i][3] = yPos;

      // Explosion has been set off, return out of the function
      return;
    }
  }
}

void PlayerWeaponOverheat(){
  // The player's weapon has overheated, force slow reload
  playerReloading = true;
  weaponOverheated = true;

  // Play overheat warning sound effect
  PlayPrimaryAudio(aOverheat);
}

void PlayerPrimaryAction(){
  // Player's primary action is to shoot

  // Check if gun can be fired
  if(fireCounter > 0) return;

  // Check if player still has ammo
  if(playerAmmo > 0) playerAmmo--;
  else if(playerAmmo == 0){
    // If the player tries to shoot when they have no ammo, their gun overheats
    PlayerWeaponOverheat();
    return;
  }

   // Loop through player projectiles array and check wether a new projectile can be spawned
  for(int i = 0; i < 16; i++){
    if(playerProjectiles[i][0] == 0){
      // An empty slot has been found
      int xOffset = 0;
      int xDir = 0;

      // Set offsets and direction
      if(playerFacesLeft){
        xOffset = -4;
        xDir = -1;
      }
      else{
        xOffset = 4;
        xDir = 1;
      }

      // Set projectile horizontal direction and position
      playerProjectiles[i][0] = xDir;
      playerProjectiles[i][1] = playerPosX + xOffset;
      playerProjectiles[i][2] = playerPosY;
      playerProjectiles[i][3] = ammoPenetration;
      
      // A shot has been fired, reset rate of fire counter
      fireCounter = fireTime;

      // Play sound effect
      PlayPrimaryAudio(aGunshot);

      // A projectile has been initiated, break out of the loop
      break;
    }
  }
}

void PlayerSecondaryAction(){
  // Player's secondary action is to begin reloading
  playerReloading = true;
}

void UpdateReload(){
  // Increase reloadCounter, amount is based on whether the player's weapon is overheated or not
  if(weaponOverheated) reloadCounter += 5;
  else reloadCounter += 10;

  // If the counter is big enough to reload a single shell, subract the amount needed and tick up ammo
  // Subtracting amount instead of setting counter to 0 to better handle rounding
  if(reloadCounter >= reloadTimeShell){
    reloadCounter -= reloadTimeShell;
    playerAmmo++;

    // Play a sound effect if no other sounds are playing. The tone increases when ammo goes up
    PlaySecondaryNote(400 + playerAmmo * 40, 107);
  }

  // If player's ammo is fully reloaded, stop reloading and set overheated to false
  if(playerAmmo == playerAmmoMax) {
    reloadCounter = 0;
    playerReloading = false;
    weaponOverheated = false;
  }
}

void UpdateRegen(){
  // Tick up the regeneration counter, done once per frame when regenerating health
  regenCounter++;

  // If enough time has passed, set the counter to 0 and tick up the player's health
  if(regenCounter > playerRegenTime){
    regenCounter = 0;
    playerHealth++;

    // Play a sound effect if no other sounds are playing. The tone increases when health goes up
    PlaySecondaryNote(320 + playerHealth * 10, 107);
  }
  
  // When the player is at full health, turn off regeneration
  if(playerHealth == playerHealthMax) playerRegenerating = false;
}

void DamagePlayer(){
  // Check if shield is still up. If it is, no damage is dealt
  if(playerShieldCounter > 0) return;
  
  // Check if player is regenerating. If so, it is aborted
  if(playerRegenerating) AbortRegen();

  // Calculate damage to take
  int damageToTake = zombieDamage - playerArmour;

  if(zombieDamage - playerArmour < 1){
    // The player takes less than 1 point of damage

    // Random chance that the player still takes 1 point of damage, the higher the player's armour, the lower the chance of damage
    if(random(0, 2 - damageToTake) == 0) damageToTake = 1;
    else damageToTake = 0;
  }

  // Apply damage taken
  playerHealth -= damageToTake;
  damageTaken += damageToTake;

  // Set off explosion
    SetOffExplosion(playerPosX, playerPosY);

  if(playerHealth <= 0){
    // The player has been defeated
    CalculateGameScore(false, currentWave, zombiesSlainTotal, playTime, damageTaken);
    SetGamePlayState(4);
  }
  else{
    // The player is still alive, activate invulnerability shield
    playerShieldCounter = playerShieldTime;
    arduboy.invert(true);

    // Play sound
    PlayPrimaryAudio(aPlayerHurt);
  }
}

void SpawnZombie(int zombieId){
  // Set isAlive to 1 from 0, indicating that is is no longer inactive
  zombies[zombieId][0] = 1;

  // Randomize starting position
  switch(random(0,6)){
    case 0:
    // Top side, left half
    zombies[zombieId][1] = random(0, mapWidth / 2);
    zombies[zombieId][2] = -8;
    break;

    case 1:
    // Top side, right half
    zombies[zombieId][1] = random(mapWidth / 2, mapWidth);
    zombies[zombieId][2] = -8;
    break;

    case 2:
    // Bottom side, left half
    zombies[zombieId][1] = random(0, mapWidth / 2);
    zombies[zombieId][2] = mapHeight + 8;
    break;

    case 3:
    // Bottom side, right half
    zombies[zombieId][1] = random(mapWidth / 2, mapWidth);
    zombies[zombieId][2] = mapHeight + 8;
    break;

    case 4:
    // Left side
    zombies[zombieId][1] = -8;
    zombies[zombieId][2] = random(0, mapHeight);
    break;

    case 5:
    // Right side
    zombies[zombieId][1] = mapWidth + 8;
    zombies[zombieId][2] = random(0, mapHeight);
    break;
  }
}

void SlayZombie(int zombieId){

  // Set zombie's animation frame to 0, indicating that it is slain
  zombies[zombieId][0] = 0;

  // Tick up zombies slain
  zombiesSlain++;
  zombiesSlainTotal++;

  // Check if wave is cleared
  if(zombiesSlain >= zombiesSlainTarget){
    // Don't clear wave if player continues slaying zombies when wave is already cleared
    if(!crateIsPlaced && !beaconIsPlaced) ClearWave();
    else zombiesSlain = 0;
  }

  // Set off an explosion at the slain zombie's position
  SetOffExplosion(zombies[zombieId][1], zombies[zombieId][2]);
}

void HitShambler(int zombieId){
  // There is a chance that a zombie kan survive getting hit. The player can negate this by upgrading ammo damage
  if(random(0, zombieSlayChance + 1) - ammoDamage <= 0) SlayZombie(zombieId);
}

void RemovePlayerProjectile(int projectileId){
  // Set projectile's x direction to 0, indicating it is inactive
  playerProjectiles[projectileId][0] = 0;
}

// ================ Draw Functions ================

void DrawSupportObjects(){
  // Draw crate
  if(crateIsPlaced) Sprites::drawOverwrite(supportObjectPosX - 4, supportObjectPosY - 4, sCrate, 0);

  // Draw beacon
  if(beaconIsPlaced){
    beaconAnimationCounter++;
    
    if(beaconAnimationCounter == beaconAnimationTime){
      beaconAnimationCounter = 0;
      beaconAnimationFrame++;
    }

    if(beaconAnimationFrame == beaconAnimationFrameCount) beaconAnimationFrame = 0;

    Sprites::drawOverwrite(supportObjectPosX - 4, supportObjectPosY - 4, sBeacon, beaconAnimationFrame);
  }
}

void DrawShamblers(){
  for(int i = 0; i < hordeCountMax; i++){
    // Skip drawing shambler if it is inactive
    if(zombies[i][0] == 0) continue;
    else{
      // Check horizontal position relative to the player and flip sprite if needed
      if(zombies[i][1] < playerPosX) Sprites::drawExternalMask(zombies[i][1] - 4, zombies[i][2] - 4, sShamblerWalk, mShamblerWalk, zombies[i][0] - 1, zombies[i][0] - 1);
      else Sprites::drawExternalMask(zombies[i][1] - 4, zombies[i][2] - 4, sShamblerWalk, mShamblerWalk, zombies[i][0] + 2, zombies[i][0] + 2);
    }
  }
}

void DrawPlayer(){
  // Draw the player

  // playerFrameCounter is ticked up in the PlayerUpdate() function

  // Check if the player is moving or standing still
  if(playerIsRunning){
    // The player is moving
    if(!playerWasRunning){
      // The player was standing still last frame, so they have just started running
      playerFrameCounter = 0;
      playerFrame = 0;
    }

    // If enough time has passed, reset the counter and tick up animation frame
    if(playerFrameCounter == animationSpeed){
      playerFrameCounter = 0;
      playerFrame++;

      // Loop back to 0 if the animation frame would become too big
      if(playerFrame == playerRunFrames) playerFrame = 0;
    }

    // Draw the player running sprite, flipped if they are facing to the left
    if(playerFacesLeft) Sprites::drawExternalMask(playerPosX - (playerSize - 2), playerPosY - (playerSize - 2), sTrooperRun, mTrooperRun, playerFrame + playerRunFrames, playerFrame + playerRunFrames);
    else Sprites::drawExternalMask(playerPosX - (playerSize - 2), playerPosY - (playerSize - 2), sTrooperRun, mTrooperRun, playerFrame, playerFrame);
  }
  else{
    // The player is standing still
    if(playerWasRunning){
      // The player was running last frame, so they have just stopped
      playerFrameCounter = 0;
      playerFrame = 0;
    }

    // If enough time has passed, reset the counter and tick up animation frame
    if(playerFrameCounter == animationSpeed){
      playerFrameCounter = 0;
      playerFrame++;
      if(playerFrame == playerIdleFrames) playerFrame = 0;
    }

    // Draw the player running sprite, flipped if they are facing to the left
    if(playerFacesLeft) Sprites::drawExternalMask(playerPosX - (playerSize - 2), playerPosY - (playerSize - 2), sTrooperIdle, mTrooperIdle, playerFrame + playerIdleFrames, playerFrame + playerIdleFrames);
    else Sprites::drawExternalMask(playerPosX - (playerSize - 2), playerPosY - (playerSize - 2), sTrooperIdle, mTrooperIdle, playerFrame, playerFrame);
  }

  // Last thing we do is set playerWasRunning to prepare for the next frame
  playerWasRunning = playerIsRunning;
}

void DrawProjectiles(){
  // Draw player projectiles
  for(int i = 0; i < 16; i++){

    // Skip drawing the projectile if it is inactive
    if(playerProjectiles[i][0] == 0) continue;

    // Draw projectile as a simple circle, 
    arduboy.fillCircle(playerProjectiles[i][1], playerProjectiles[i][2], 2);
  }
}

void DrawExplosions(){
  // Loop through explosions array
  for(int i = 0; i < 16; i++){

    // Skip drawing the explosion if it is inactive
    if(explosions[i][0] == 0) continue;

    // Tick down animation counter
    explosions[i][1]--;

    // If counter is 0, advance animation
    if(explosions[i][1] == 0){
      explosions[i][1] = explosionSpeed;
      explosions[i][0]++;
    } 

    // What to be displayed is determined by the explosion's frame number (first value)
    switch(explosions[i][0]){
      case 1:
      // first frame, draw a white circle
      arduboy.fillCircle(explosions[i][2], explosions[i][3], 3, WHITE);
      break;

      case 2:
      // second frame, draw a black circle
      arduboy.fillCircle(explosions[i][2], explosions[i][3], 6, BLACK);
      break;

      case 3:
      // third frame, draw a white circle
      arduboy.fillCircle(explosions[i][2], explosions[i][3], 4, WHITE);
      break;

      case 4:
      // animation end, deactivate the explosion
      explosions[i][0] = 0;
      break;
    }
  }
}

void DrawActors(){
  // Function for drawing game actors
  DrawSupportObjects();
  DrawShamblers();
  DrawPlayer();
  DrawProjectiles();
  DrawExplosions();
}

void DrawUi(){
  // Draw UI

  // Draw player's health bar
  for(int i = 0; i < playerHealthMax; i++){
    if(i <= playerHealth - 1) Sprites::drawOverwrite(i * 4, 0, sHealthBarFull, 0);
    else Sprites::drawOverwrite(i * 4, 0, sHealthBarEmpty, 0);
  }

  // Draw player's ammo bar
  for(int i = 0; i < playerAmmoMax; i++){
    if(i <= playerAmmo - 1) Sprites::drawOverwrite(i * 2, 8, sAmmoBarFull, 0);
    else Sprites::drawOverwrite(i * 2, 8, sAmmoBarEmpty, 0);
  }

  // Draw threat level
  Sprites::drawOverwrite(120, 0, sThreatIndicatorSkull, 0);

  for(int i = 0; i < threatLevelMax; i++){
    if(i <= currentThreatLevel - 1) Sprites::drawOverwrite(118 - i * 2, 0, sAmmoBarFull, 0);
    else Sprites::drawOverwrite(118 - i * 2, 0, sAmmoBarEmpty, 0);
  }
}

void DrawUpgradeUi(){
  // Draw upgrade UI panel

  // Loop through the array of available upgrades and draw the appropriate sprite
  for(int i = 0; i < 4; i++){
    switch(availableUpgrades[i]){
      case 0:
      // Regenerate to full health
      Sprites::drawOverwrite(33 + i * 16, 25, sFullHeal, 0);
      break;

      case 1:
      // Call in teleportation beacon
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeCallBeacon, 0);
      break;

      case 10:
      // Health up
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeHealthUp, 0);
      break;

      case 11:
      // Ammo up
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeAmmoUp, 0);
      break;

      case 12:
      // Penetration up
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradePenetrationUp, 0);
      break;

      case 13:
      // Reload Speed up
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeReloadSpeedUp, 0);
      break;

      case 14:
      // Shambler spawn rate increase down
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeShamblerSpawnDown, 0);
      break;

      case 15:
      // Shield duration up
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeShieldDurationUp, 0);
      break;

      case 16:
      // Shambler count down
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeShamblerCountDown, 0);
      break;

      case 17:
      // Armour up
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeArmourUp, 0);
      break;

      case 18:
      // Ammo damage up
      Sprites::drawOverwrite(33 + i * 16, 25, sUpgradeAmmoDmgUp, 0);
      break;
    }
  }

  // Outline the currently highlighted upgrade with a white square
  arduboy.drawRect(32 + chosenUpgrade * 16, 24, 16, 16, WHITE);
}

void DrawFadeOut(){
  // Transition to black, borders close in on the player

  // Left border
  if(playerPosX + outroCounter - 128 > 0) arduboy.fillRect(0, 0, playerPosX + outroCounter - 128, 64, BLACK);

  // Right border
  if(playerPosX + 128 - outroCounter < 128) arduboy.fillRect(playerPosX + 128 - outroCounter, 0, playerPosX - outroCounter, 64, BLACK);

  // Upper border
  if(playerPosY + outroCounter / 2 - 64 > 0) arduboy.fillRect(0, 0, 128, playerPosY + outroCounter / 2 - 64, BLACK);

  // Lower border
  if(playerPosY + 64 - outroCounter / 2 < 64) arduboy.fillRect(0, playerPosY + 64 - outroCounter / 2, 128, playerPosY - outroCounter / 2, BLACK);
}

// ================ Update Functions ================

void UpdatePlayer(){

  // Set playerIsRunning to false. If the player is running in any direction, it will be set to true
  playerIsRunning = false;

  // Check if the player is moving. Apply position change and set appropriate booleans
  if(arduboy.pressed(UP_BUTTON)){
    if(playerPosY > 4) playerPosY--;
    playerIsRunning = true;
  }
  else if(arduboy.pressed(DOWN_BUTTON)){
    if(playerPosY < mapHeight - 4) playerPosY++;
    playerIsRunning = true;
  }

  if(arduboy.pressed(LEFT_BUTTON)){
    if(playerPosX > 4) playerPosX--;
    playerFacesLeft = true;
    playerIsRunning = true;
  }
  else if(arduboy.pressed(RIGHT_BUTTON)){
    if(playerPosX < mapWidth - 4) playerPosX++;
    playerFacesLeft = false;
    playerIsRunning = true;
  }

  // If a support crate is placed, check if player is close enough to pick it up
  if(crateIsPlaced && abs(playerPosX - supportObjectPosX) < playerSize && abs(playerPosY - supportObjectPosY) < playerSize) CrateIsPickedUp();

  // If beacon is placed, check if player is close enough to pick it up
  if(beaconIsPlaced && abs(playerPosX - supportObjectPosX) < playerSize && abs(playerPosY - supportObjectPosY) < playerSize) BeaconIsTouched();

  // If the player is regenerating health, update regeneration
  if(playerRegenerating) UpdateRegen();

  // If the player is reloading, update reload
  // If the player isn't locked into reloading, they can perform either a primary or secondary action
  if(playerReloading) UpdateReload();
  else if(arduboy.pressed(A_BUTTON)) PlayerPrimaryAction();
  else if(arduboy.justPressed(B_BUTTON)) PlayerSecondaryAction();

  // Tick down player shield counter
  if(playerShieldCounter > 0){
    playerShieldCounter--;
    if(playerShieldCounter == 0) arduboy.invert(false);
  }

  // Tick down fire counter
  if(fireCounter > 0) fireCounter--;

  // Tick up animation counter
  playerFrameCounter++;
}

void UpdateZombie(int zombieId){
  // This function handles the logic of an individual zombie

  // If zombie is dead, spawn a new one if its ID is within shambler count limit
  if(zombies[zombieId][0] == 0){
    if(zombieId >= hordeCount) return;
    else SpawnZombie(zombieId);
  }

  // Move shambler towards the player, stop 2 pixels away to not completely overlap with the player
  if(zombies[zombieId][1] < playerPosX - 2) zombies[zombieId][1]++;
  else if(zombies[zombieId][1] > playerPosX + 2) zombies[zombieId][1]--;

  if(zombies[zombieId][2] < playerPosY - 2) zombies[zombieId][2]++;
  else if(zombies[zombieId][2] > playerPosY + 2) zombies[zombieId][2]--;

  // Check if shampler collides with the player. If yes, harm the player
  if(abs(zombies[zombieId][1] - playerPosX) < playerSize && abs(zombies[zombieId][2] - playerPosY) < playerSize && playerHealth > 0) DamagePlayer();

  // Increase shambler animation frame
  zombies[zombieId][0]++;

  // Loop back to the first frame if counter gets too big. Counter is offset by 1 because 0 indicates the shambler is inactive
  if(zombies[zombieId][0] == shamblerAnimationFrameMax + 1) zombies[zombieId][0] = 1;
}

void UpdateHorde(){
  // Update the undead horde

  // Tick up counter for horde increase
  hordeIncreaseCounter++;
  if(hordeIncreaseCounter > hordeIncreaseTime){
    // Enough time has passed to increase the number of zombies
    hordeIncreaseCounter = 0;

    // Tick up number of active zombies
    hordeCount++;

    // If too many shambler would be active, limit them to the maximum allowed
    if(hordeCount > hordeCountMax
  ) hordeCount = hordeCountMax
  ;

    // Calculate new threat level
    CalculateThreatLevel();
  }

  // Only 1/8th of all zombies are updated each frame
  for(int i = 0; i < 8; i++) UpdateZombie(i * 8 + hordeIndexOffset);

  // Increase the index offset to activate new set of 8 zombies next frame
  hordeIndexOffset++;

  // Loop the offset back to 0 if it would get too big
  if(hordeIndexOffset == hordeIndexOffsetMax) hordeIndexOffset = 0;
}

void UpdateProjectiles(){
  // Update player projectiles

  // Loop through the projectile array
  for(int i = 0; i < 16; i++){

    // If the projectile's horizontal speed != 0, it is active
    if(playerProjectiles[i][0] == 0) continue;

    // Move projectile
    playerProjectiles[i][1] += playerProjectiles[i][0] * 4;

    // Check if the projectile has left the map
    if(playerProjectiles[i][1] < -8 || playerProjectiles[i][1] > mapWidth + 8){

      // The projectile has left the map, destroy it
      RemovePlayerProjectile(i);

      // The projectile is destroyed, break out of this loop, but continue checking projectiles
      continue;
    }

    // Loop through zombies to check if a hit is registered
    for(int ii = 0; ii < hordeCountMax
  ; ii++){

      // A hit can't be regisered if the shambler is inactive
      if(zombies[ii][0] == 0) continue;

      // Check if collision occurs, projectile size is 4
      if(abs(zombies[ii][1] - playerProjectiles[i][1]) < 4 && abs(zombies[ii][2] - playerProjectiles[i][2]) < 4){
        // A hit has been registered
        HitShambler(ii);

        // Tick down the projectile's penetration points
        playerProjectiles[i][3]--;

        // Check if projectile has lost penetration and should be removed
        if(playerProjectiles[i][3] == 0) RemovePlayerProjectile(i);
      }

      // Stop looping through zombies if the projectile has been removed
      if(playerProjectiles[i][0] == 0) break;
    }
  }
}

void UpdateUpgrade(){
  // Update function for when the player is picking upgrades

  // Tick up the upgrade time counter. The player can only pick an upgrade after 1 second has passed
  if (upgradeTimeCounter < 30) upgradeTimeCounter++;
  
  if(arduboy.justPressed(LEFT_BUTTON) && chosenUpgrade > 0) chosenUpgrade--;
  else if(arduboy.justPressed(RIGHT_BUTTON) && chosenUpgrade < 3) chosenUpgrade++;
  else if(arduboy.justPressed(A_BUTTON) && upgradeTimeCounter == 30) ApplyUpgrade();
}

void UpdateIntro(){
  // Update function for the intro

  // What to be updated is dictated by the introStage variable
  switch(introStage){
    case 0:
    // Fill up player's health
    UpdateRegen();

    // When health is full, advance intro to next stage
    if(!playerRegenerating) introStage++;
    break;

    case 1:
    // Fill up player's ammo
    UpdateReload();

    // When ammo is full, advance intro to next stage
    if(!playerReloading) introStage++;
    break;

    case 2:
    // Teleport in player
    playerPosY = playerPosYStart;

    SetOffExplosion(playerPosXStart ,playerPosYStart);
    introStage++;
    break;

    case 3:
    // Set gameplayState to 1 to begin gameplay
    CalculateThreatLevel();
    SetGamePlayState(1);
    break;
  }
}

void UpdateExtraction(){
  // Update function for when the player has teleported away
  outroCounter++;
  if(outroCounter == outroTime) Victory();
}

void UpdateDefeat(){
  // Update function for when the player has been defeated
  outroCounter++;

  // The player is consumed by explosions until enough time has passed, then proceed to game over
  if(outroCounter == outroTime) GameOver();
  else if(outroCounter % 4 == 0) SetOffExplosion(playerPosX - 4 + random(0, 8), playerPosY - 4 + random(0, 8));
}

void UpdateGameplay(){
  // The main update loop for gameplay

  // What happens is controlled by gamePlayState
  switch(gamePlayState){

    case 0:
    // Intro before gameplay starts

    // Update intro
    UpdateIntro();

    // Draw actors
    DrawActors();

    // Draw UI
    DrawUi();
    break;

    case 1:
    // Normal gameplay

    // Tick up time played
    timeCounter++;
    if(timeCounter == 30){
      timeCounter = 0;
      playTime++;
    }

    // Update actors and projectiles
    UpdatePlayer();
    UpdateHorde();
    UpdateProjectiles();

    // Draw actors
    DrawActors();

    // Draw UI
    DrawUi();
    break;

    case 2:
    // Player is choosing upgrades

    // Update upgrade picking process
    UpdateUpgrade();

    // Draw actors
    DrawActors();

    // Draw upgrade UI
    DrawUpgradeUi();
    break;

    case 3:
    // The player has teleported away and won

    // Update extraction
    UpdateExtraction();

    // Update horde and projectiles
    UpdateHorde();
    UpdateProjectiles();

    // Draw actors. Player isn't displayed so DrawActors function isn't used
    DrawShamblers();
    DrawProjectiles();
    DrawExplosions();
    DrawFadeOut();
    break;

    case 4:
    // Player has been defeated

    // Update defeat
    UpdateDefeat();

    // Update horde and projectiles
    UpdateHorde();
    UpdateProjectiles();
    
    // Draw actors
    DrawActors();
    DrawFadeOut();
    break;
  }
}