// ================ Audio Constants ================

const int minimumNote = 320;
const int maximumNote = 3200;

// ================ Audio Functions ================

void TurnOnAudio(){
  // Turn on audio
  arduboy.audio.begin();
}

void SetAudioEnabled(bool b) {
  // Enable/disable audio
  if(arduboy.audio.enabled() != b) arduboy.audio.toggle();
}

void PlayPrimaryAudio(uint16_t pSound[]){
  // Play the sound, and interrupt current sounds
  sound.tones(pSound);
}

void PlaySecondaryAudio(uint16_t sSound[]){
  // Only play sound if nothing is currently playing
  if(!sound.playing()) sound.tones(sSound);
}

void StopAudio(){
  // Stop playing current audio
  sound.noTone();
}

void PlaySecondaryNote(uint16_t note, uint16_t duration){
  // Play the note if nothing is currently playing

  // Make sure the note to be played stays within min and max bounds
  note = max(minimumNote, note);
  note = min(note, maximumNote);

  if(!sound.playing()) sound.tone(note, duration);
}