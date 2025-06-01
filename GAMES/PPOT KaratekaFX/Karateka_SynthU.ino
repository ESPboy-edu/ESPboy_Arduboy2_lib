#include "Enums.h"
#define SYNTHU_IMPLEMENTATION
#ifdef SOUNDS_SYNTHU
#include "src/utils/SynthU.hpp"

static MusicSong current_song;

void audioUpdate() {

    SynthU::update();

    // auto v = soundSettings.getVolume();

    uint8_t v = 10;

    if (SynthU::playingSFX()) {

        SynthU::setVolume(v > 6 ? v - 6 : 0);
        SynthU::setVolumeSFX(10);

    }
    else {

        SynthU::setVolume(v);

    }

}

void audioInit() {

    SynthU::setup();
    // SynthU::setVolume(soundSettings.getVolume());
    // SynthU::setVolumeSFX(soundSettings.getVolume());
    SynthU::setVolume(10);
    SynthU::setVolumeSFX(10);

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
    
    // if (soundSettings.getSounds() && isAudioEnabled()) {

        uint24_t musicStart = FX::readIndexedUInt24(Music::Songs, static_cast<uint8_t>(song));
        SynthU::play(musicStart);

    // }

}

void playSFX(MusicSFX sfx) {

    // if (soundSettings.getSounds() && isAudioEnabled()) {

        uint24_t sfxStart = FX::readIndexedUInt24(Music::SFXs, static_cast<uint8_t>(sfx));
        SynthU::playSFX(sfxStart);

    // }

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

#endif
