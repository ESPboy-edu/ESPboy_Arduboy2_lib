// ================ Save Definitions ================

#define EEPROM_START_C1     EEPROM_STORAGE_SPACE_START
#define EEPROM_START_C2     EEPROM_START_C1 + 1
#define EEPROM_START_DATA   EEPROM_START_C1 + 2

// ================ Save Structs ================

struct SaveData {
  // A struct to contain game data to be saved and loaded

  // An array for highscores
  uint16_t savedHighScores[3] = {0, 0, 0};

  // A boolean to save sound setting
  bool savedSoundSetting = true;
};

// ================ Save Functions ================

void CheckGameData() {
  // A function to check if EEPROM contains zombie slayer save data

  // The first adresses are used for the check
  uint8_t c1 = EEPROM.read(EEPROM_START_C1);
  uint8_t c2 = EEPROM.read(EEPROM_START_C2);

  // If the values aren't Z and S (short for Zombie Slayer), prepare the EEPROM for Zombie Slayer data
  if (c1 != 'Z' || c2 != 'S') {   
    EEPROM.write(EEPROM_START_C1, 'Z');
    EEPROM.write(EEPROM_START_C2, 'S');
    ResetGameData();
  }
  EEPROM.commit();
}

void LoadGameData(){
  // A function to load game data

  // Create a struct for the data to be loaded
  SaveData newSave;

  // Get data from EEPROM
  EEPROM.get(EEPROM_START_DATA, newSave);

  // Get highscores from newSave and set highscore variables
  for(int i = 0; i < 3; i++) SetHighScore(i, newSave.savedHighScores[i]);

  // Get saved audio setting and either enable or disable audio
  SetAudioEnabled(newSave.savedSoundSetting);
}

void SaveGameData(){
  // A function to save game data

  // Create a struct for the data to be saved
  SaveData newSave;

  // Get current highscore variables and write them down
  for(int i = 0; i < 3; i++) newSave.savedHighScores[i] = GetHighScore(i);

  // Get current soundsetting and write it down
  newSave.savedSoundSetting = arduboy.audio.enabled();

  // Save game data
  EEPROM.put(EEPROM_START_DATA, newSave);
  EEPROM.commit();
}

void ResetGameData(){
  // A function to reset game data

  // Create a game data struct
  SaveData newSave;

  // Save the unmodified struct, resetting game data
  EEPROM.put(EEPROM_START_DATA, newSave);
  EEPROM.commit();
}
