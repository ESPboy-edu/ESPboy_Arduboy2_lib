#pragma once

#include <sigma_delta.h>
#include <Arduino.h>

#ifdef SOUNDS_SYNTHU

#ifndef SYNTHU_UPDATE_EVERY_N_FRAMES
#define SYNTHU_UPDATE_EVERY_N_FRAMES 1
#endif

#ifndef SYNTHU_NUM_CHANNELS
#define SYNTHU_NUM_CHANNELS 4
#endif

#ifndef SYNTHU_AUDIO_ENABLED_FUNC
#define SYNTHU_AUDIO_ENABLED_FUNC Arduboy2Audio::enabled
#endif

#ifndef SYNTHU_ENABLE_VOLUME
#define SYNTHU_ENABLE_VOLUME 1
#endif

#ifndef SYNTHU_ENABLE_CLIP
#define SYNTHU_ENABLE_CLIP 1
#endif

#ifndef SYNTHU_NOISE_CHANNEL_HZ
#define SYNTHU_NOISE_CHANNEL_HZ 0
#endif

#ifndef SYNTHU_ENABLE_SFX
#define SYNTHU_ENABLE_SFX 0
#endif

#ifndef SYNTHU_FX_READDATABYTES_FUNC
#define SYNTHU_FX_READDATABYTES_FUNC FX::readDataBytes
#endif

#include <Arduboy2Audio.h>
#include <ArduboyFX.h>

#define uint24_t uint32_t

static bool soundOn = true; 
void ICACHE_RAM_ATTR SoundISR();


struct SynthU
{
    static void setup();
    
    static void play(uint24_t song);
    
    static bool playing(); // whether song is playing
    static void stop();    // stops song and SFX
    static void resume();  // does not resume SFX
    
    static uint8_t volume();
    static void setVolume(uint8_t vol);
    
    static void playSFX(uint24_t sfx);
    static bool playingSFX();
    static void stopSFX();
    static uint8_t volumeSFX();
    static void setVolumeSFX(uint8_t vol);
    
    // fill buffer from FX chip
    // test for audio enabled
    // return true if playing
    static bool update();
    static void pause();
    static void unpause();
};

#ifdef SYNTHU_IMPLEMENTATION


constexpr uint8_t ADV_SHIFT = 2;

#if SYNTHU_NOISE_CHANNEL_HZ
constexpr uint16_t NOISE_PERIOD = 16000000ul / (1 << ADV_SHIFT) / SYNTHU_NOISE_CHANNEL_HZ;
#endif


struct command_t
{
    uint8_t  vol;
    uint16_t period;
};

struct tick_t
{
    command_t cmds[SYNTHU_NUM_CHANNELS];
    uint8_t reps;
};

struct sfx_tick_t
{
    command_t cmd;
    uint8_t reps;
};

struct channel_t
{
    uint16_t pha;
};



static volatile tick_t     g_tick;
static volatile channel_t  g_channels[SYNTHU_NUM_CHANNELS];
static volatile bool       g_playing;
static          uint8_t    g_tick_frame;
static volatile uint24_t   g_buffer_addr;
static volatile uint24_t   g_buffer_addr_bkp;
static volatile uint8_t    g_phase_adv;
static volatile int16_t    g_tbase;
#if SYNTHU_ENABLE_VOLUME
static volatile uint8_t    g_volume;
#endif

#if SYNTHU_ENABLE_SFX
static volatile bool       g_playing_sfx;
static volatile uint24_t   g_buffer_addr_sfx;
static volatile sfx_tick_t g_tick_sfx;
static volatile channel_t  g_channel_sfx;
#if SYNTHU_ENABLE_VOLUME
static volatile uint8_t    g_volume_sfx;
#endif
#endif



static void tick()
{
    do
    {
        uint8_t reps = g_tick.reps;
        if(reps == 0) break;
        reps -= 1;
        g_tick.reps = reps;
        if(reps != 0) break;
        g_buffer_addr += SYNTHU_NUM_CHANNELS*(sizeof(uint8_t)+sizeof(uint16_t))+sizeof(uint8_t);//sizeof(g_tick);
    } while(0);
    
#if SYNTHU_ENABLE_SFX
    do
    {
        uint8_t reps = g_tick_sfx.reps;
        if(reps == 0) break;
        reps -= 1;
        g_tick_sfx.reps = reps;
        if(reps != 0) break;
        g_buffer_addr_sfx += sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t);//sizeof(g_tick_sfx);
    } while(0);
#endif
}


static void load_fx_data()
{
  static uint8_t soundBuf[SYNTHU_NUM_CHANNELS*(sizeof(uint8_t)+sizeof(uint16_t))+sizeof(uint8_t)];
  static uint8_t *PsoundBuf;
    if(g_playing && g_tick.reps == 0)
    {
        PsoundBuf = soundBuf;
        SYNTHU_FX_READDATABYTES_FUNC(g_buffer_addr, soundBuf,sizeof(soundBuf));
        for (uint8_t i; i<SYNTHU_NUM_CHANNELS; i++){
          g_tick.cmds[i].vol = *PsoundBuf++;
          g_tick.cmds[i].period = (*PsoundBuf++)+(*PsoundBuf++)*256;
        }
        g_tick.reps = *PsoundBuf;
    }

    
#if SYNTHU_ENABLE_SFX
    if(g_playing_sfx && g_tick_sfx.reps == 0)
    {
        SYNTHU_FX_READDATABYTES_FUNC(g_buffer_addr_sfx, soundBuf, sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t));
        PsoundBuf = soundBuf;
        g_tick_sfx.cmd.vol = *PsoundBuf++;
        g_tick_sfx.cmd.period = (*PsoundBuf++)+(*PsoundBuf++)*256;
        g_tick_sfx.reps = *PsoundBuf;
        
        if(g_tick_sfx.cmd.period == 0)
            g_playing_sfx = false;
    }
#endif
    int16_t t = 0;
    
    // song tbase
    for(uint8_t i = 0; i < SYNTHU_NUM_CHANNELS; ++i)
    {
        uint8_t vol = g_tick.cmds[i].vol;
        uint16_t period = g_tick.cmds[i].period;
        if(period == 0) SynthU::stop();
        vol >>= 1;
        t -= vol;
    }
    
    g_tbase = t;
}

static void disable(){ 
    soundOn = false; 
}

static void enable(){
    soundOn = true; 
}

static bool enabled(){
    return soundOn;
}




void SynthU::pause(){
    g_buffer_addr_bkp = g_buffer_addr;
}

void SynthU::unpause(){
    play(g_buffer_addr_bkp);
}

uint8_t SynthU::volume(){
#if SYNTHU_ENABLE_VOLUME
    return g_volume;
#else
    return 3;
#endif
}

void SynthU::setVolume(uint8_t vol){
#if SYNTHU_ENABLE_VOLUME
    g_volume = ((vol * 3) / 2); //Constants::VolumeFactor;
#endif
}

uint8_t SynthU::volumeSFX(){
#if SYNTHU_ENABLE_SFX && SYNTHU_ENABLE_VOLUME
    return g_volume_sfx;
#else
    return 8;
#endif
}

void SynthU::setVolumeSFX(uint8_t vol){
#if SYNTHU_ENABLE_SFX && SYNTHU_ENABLE_VOLUME
    g_volume_sfx = (vol * 2); //Constants::VolumeFactor;
#endif
}


#define SAMPLE_RATE 22000
#define SOUNDPIN D3

void SynthU::setup(){
    stop();
    //check sigma-delta info for ESP8266 here https://github.com/esp8266/Arduino/blob/master/cores/esp8266/sigma_delta.h
    noInterrupts();
    sigmaDeltaSetup(0, SAMPLE_RATE);
    sigmaDeltaAttachPin(SOUNDPIN);
    sigmaDeltaEnable();
    timer1_attachInterrupt(SoundISR);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
    timer1_write(80 * 1000000 / SAMPLE_RATE);
    interrupts();
    soundOn = true;
    setVolume(16);
#if SYNTHU_ENABLE_SFX
    setVolumeSFX(16);
#endif
}

void SynthU::stop(){
    disable();
    g_playing = false;
#if SYNTHU_ENABLE_SFX
    g_playing_sfx = false;
#endif
}

void SynthU::resume(){
    g_tick.reps = 0;
    g_playing = true;
    update();
    enable();
}

void SynthU::play(uint24_t song){
    stop();
    g_buffer_addr = song;
    resume();
}

bool SynthU::playing(){
    return g_playing;
}

bool SynthU::playingSFX(){
#if SYNTHU_ENABLE_SFX
    return g_playing_sfx;
#else
    return false;
#endif
}

void SynthU::playSFX(uint24_t sfx){
#if SYNTHU_ENABLE_SFX
    g_buffer_addr_sfx = sfx;
    g_playing_sfx = true;
    load_fx_data();
    if(!enabled())
        enable();
#endif
}

void SynthU::stopSFX(){
#if SYNTHU_ENABLE_SFX
    g_playing_sfx = false;
#endif
}

bool SynthU::update(){
    bool p = g_playing
#if SYNTHU_ENABLE_SFX
        || g_playing_sfx
#endif
        ;
    if(!p) return false;
    uint8_t f = g_tick_frame;
    if(++f < SYNTHU_UPDATE_EVERY_N_FRAMES)
    {
        g_tick_frame = f;
        return true;
    }
    g_tick_frame = 0;
    load_fx_data();
    tick();
    return true;
}



void IRAM_ATTR SoundISR(){
noInterrupts();
    if(!SYNTHU_AUDIO_ENABLED_FUNC() ||
        !g_playing &&
#if SYNTHU_ENABLE_SFX
        !g_playing_sfx
#else
        true
#endif
        )
    {
        interrupts();
        return;
    }

noInterrupts();

    uint16_t adv=128;
    int16_t t = 0;
    
    // song contribution
#if SYNTHU_ENABLE_SFX
    if(g_playing)
#endif
    {
        t = g_tbase;
        for(uint8_t i = 0; i < SYNTHU_NUM_CHANNELS; ++i)
        {
            uint8_t vol = g_tick.cmds[i].vol;
            uint16_t period = g_tick.cmds[i].period;
            if(vol == 0) continue; 
            
            uint16_t pha =  g_channels[i].pha;
            pha += adv;
            while(pha >= period)
                pha -= period;
            uint16_t half_period = period / 2;
            if(pha < half_period)
            {
                t += vol;
                period = half_period;
            }
            g_channels[i].pha = pha;
            period -= pha;
        }
    
        // song volume adjust
#if SYNTHU_ENABLE_VOLUME
        t *= g_volume;
        t >>= 4;
#else
        t >>= 1;
#endif
    }

    // SFX contribution
#if SYNTHU_ENABLE_SFX
    if(g_playing_sfx)
    {
        uint8_t vol = g_tick_sfx.cmd.vol;
        int16_t tsfx = 0;
        if(vol != 0)
        { 
            uint16_t period =  g_tick_sfx.cmd.period;
            uint16_t pha = g_channel_sfx.pha;
            pha += adv;
            while(pha >= period)
                pha -= period;
            uint16_t half_period = period / 2;
            if(pha < half_period)
            {
                tsfx += vol;
                period = half_period;
            }
            else
                tsfx -= vol;

            g_channel_sfx.pha = pha;
            period -= pha;

            // SFX volume adjust
#if SYNTHU_ENABLE_VOLUME
            tsfx *= g_volume_sfx;
            tsfx >>= 4;
#else
            tsfx >>= 1;
#endif
            t += tsfx;
        }
    }
#endif
    
    uint8_t tc = uint8_t(t);
#if SYNTHU_ENABLE_CLIP
    if(t > +120) tc = uint8_t(+120);
    if(t < -120) tc = uint8_t(-120);
#endif
    tc += 128;

    //check sigma-delta info for ESP8266 here https://github.com/esp8266/Arduino/blob/master/cores/esp8266/sigma_delta.h
    if (soundOn)
      sigmaDeltaWrite(0, tc);
    else sigmaDeltaWrite(0, 0);
    
    interrupts();
}

#endif
#endif
