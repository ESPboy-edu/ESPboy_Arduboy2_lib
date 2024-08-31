#include "src/utils/ArduboyG.h"
#include "src/utils/SynthU.hpp"

#ifdef SOUNDS_SYNTHU

static MusicSong current_song;
static MusicSong current_song_bkp;

void audioToggle() {

    if (isAudioEnabled()) {
    
        setAudioOff();

    }
    else {
    
        setAudioOn();

    }

}

void audioUpdate() {

    if (!SynthU::update()) {
        
        playMusic();

    }

    if (!SynthU::playingSFX()) {


        // Adjust the volume of music to full volume if not playing a sound effect ..

        SynthU::setVolume(level.getSoundSettings().getVolume());

    }
    else {


        // Adjust the volume of the music down if playing a sound effect ..
        
        if (level.getSoundSettings().getVolume() > 6) {;

            SynthU::setVolume(level.getSoundSettings().getVolume() - 6);
            SynthU::setVolumeSFX(level.getSoundSettings().getVolume());

        }
        else {

            SynthU::setVolume(0);
            SynthU::setVolumeSFX(level.getSoundSettings().getVolume());

        }

    }

}

void audioInit() {

    SynthU::setup();
    SynthU::setVolume(level.getSoundSettings().getVolume());

}

void setAudioOn() {
    
    Arduboy2Audio::on();

}

void setAudioOff() {

    Arduboy2Audio::off();
    SynthU::stop();

}

bool isAudioEnabled() {

    return Arduboy2Audio::enabled();

}

MusicSong getSongPlaying() {

    return current_song;

}

void playSong(MusicSong song) {

    current_song = song;

    if (level.getSoundSettings().getMusic() && isAudioEnabled()) {

        SynthU::play(Music::Songs[static_cast<uint8_t>(song)]);

    }

}

void playSFX(MusicSFX sfx) {

    if (level.getSoundSettings().getSFX() && isAudioEnabled()) {

        SynthU::playSFX(Music::SFXs[static_cast<uint8_t>(sfx)]);

    }

}

void stopMusic() {

    SynthU::stop();

}

void stopSFX() {

    SynthU::stopSFX();

}

bool isSFXPlaying() {

    return SynthU::playingSFX();

}

bool isSongPlaying() {

    return SynthU::playing();

}


void playMusic() {
 
    playSong(current_song);

}

void pauseSong() {
 
    current_song_bkp = current_song;

    SynthU::pause();

}

void unpauseSong() {
 
    current_song = current_song_bkp;

    SynthU::unpause();

}
#endif