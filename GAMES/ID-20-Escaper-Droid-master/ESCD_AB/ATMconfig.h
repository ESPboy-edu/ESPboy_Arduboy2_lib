#ifndef _ATMCONFIG_H_
#define _ATMCONFIG_H_

// Waveforms are fixed at compile time. Change these, then recompile.
// ATM_WAVE_PULSE  0
// ATM_WAVE_SQUARE 1
// ATM_WAVE_NOISE  2   (use on channel 3; ATM_NOISE() is a song macro)
// ATM_WAVE_SAW    3
// ATM_WAVE_OFF    255  (no mixer for this channel — smaller ISR)

#ifndef ATM_WAVE_PULSE
#define ATM_WAVE_PULSE  0
#define ATM_WAVE_SQUARE 1
#define ATM_WAVE_NOISE  2
#define ATM_WAVE_SAW    3
#define ATM_WAVE_OFF    255
#endif

#ifndef ATM_WAVE_CH0
#define ATM_WAVE_CH0 ATM_WAVE_PULSE
#endif
#ifndef ATM_WAVE_CH1
#define ATM_WAVE_CH1 ATM_WAVE_SQUARE
#endif
#ifndef ATM_WAVE_CH2
#define ATM_WAVE_CH2 ATM_WAVE_SAW
#endif
#ifndef ATM_WAVE_CH3
#define ATM_WAVE_CH3 ATM_WAVE_NOISE
#endif

// Example: drop ch2 from the mixer
// #define ATM_WAVE_CH2 ATM_WAVE_OFF

// 0 = stock Arduboy speaker on OCR4A only
// 1 = also drive OCR4D (homemade / dual-pin wiring)
#ifndef ATM_ALT_WIRING
#define ATM_ALT_WIRING 0
#endif

// ---------------------------------------------------------------------------
// Effects, functions, octaves. 1 or TRUE = compile in. 0 or FALSE = leave out.
//
// WARNING: the tracker still writes the same song bytes. Nothing is stripped
// from a score. If a flag is 0 and a song or SFX still uses that effect,
// function, or octave, the sketch breaks: the player ignores the command and
// does not consume its parameter bytes, a removed function will not compile,
// and an excluded note is silent. That is the sketch author's responsibility.
//
// A sketch with no ATMconfig.h compiles all of these in. The 1s below match that.
// ATM.play() is always compiled. ATM_FUNC_PLAYPAUSE needs PAUSE and RESUME.
// ---------------------------------------------------------------------------

// Effects (command + per-tick code, and the channel fields they need)
#ifndef ATM_FX_VOLUME
#define ATM_FX_VOLUME 1       // ATM_VOL
#endif
#ifndef ATM_FX_VOL_SLIDE
#define ATM_FX_VOL_SLIDE 1    // ATM_SL_VOL / ATM_SL_VOL_ADV / ATM_SL_VOL_OFF
#endif
#ifndef ATM_FX_FREQ_SLIDE
#define ATM_FX_FREQ_SLIDE 1   // ATM_SL_FRQ / ATM_SL_FRQ_ADV / ATM_SL_FRQ_OFF
#endif
#ifndef ATM_FX_ARP
#define ATM_FX_ARP 1          // ATM_ARP / ATM_ARP_OFF
#endif
#ifndef ATM_FX_NOISE
#define ATM_FX_NOISE 0        // ATM_NOISE / ATM_NOISE_OFF (retrigger) - not used by any song or sfx
#endif
#ifndef ATM_FX_TRANSPOSE
#define ATM_FX_TRANSPOSE 1    // ATM_ADD_TRA / ATM_SET_TRA / ATM_TRA_OFF
#endif
#ifndef ATM_FX_TREMOLO
#define ATM_FX_TREMOLO 0      // ATM_TREM / ATM_TREM_OFF - not used by any song or sfx
#endif
#ifndef ATM_FX_VIBRATO
#define ATM_FX_VIBRATO 1      // ATM_VIB / ATM_VIB_OFF
#endif
#ifndef ATM_FX_GLISSANDO
#define ATM_FX_GLISSANDO 1    // ATM_GLIS / ATM_GLIS_OFF
#endif
#ifndef ATM_FX_NOTE_CUT
#define ATM_FX_NOTE_CUT 1     // ATM_CUT / ATM_CUT_OFF
#endif
#ifndef ATM_FX_CUE
#define ATM_FX_CUE 1          // ATM_CUE command (stores the byte)
#endif
#ifndef ATM_FX_TEMPO
#define ATM_FX_TEMPO 1        // ATM_ADD_TEMPO / ATM_SET_TEMPO in the score
#endif
#ifndef ATM_FX_GOTO_ADV
#define ATM_FX_GOTO_ADV 0     // ATM_GOTO_ADV - not used by any song or sfx
#endif
#ifndef ATM_FX_STOP
#define ATM_FX_STOP 1         // ATM_STOP_CHAN
#endif

// Functions (calling one that is 0 is a compile error)
#ifndef ATM_FUNC_STOP
#define ATM_FUNC_STOP 1       // ATM.stop()
#endif
#ifndef ATM_FUNC_PAUSE
#define ATM_FUNC_PAUSE 1      // ATM.pause()
#endif
#ifndef ATM_FUNC_RESUME
#define ATM_FUNC_RESUME 1     // ATM.resume()
#endif
#ifndef ATM_FUNC_PLAYPAUSE
#define ATM_FUNC_PLAYPAUSE 1  // ATM.playPause() — needs PAUSE and RESUME
#endif
#ifndef ATM_FUNC_MUTE
#define ATM_FUNC_MUTE 0       // ATM.muteChannel()
#endif
#ifndef ATM_FUNC_UNMUTE
#define ATM_FUNC_UNMUTE 0     // ATM.unMuteChannel()
#endif
#ifndef ATM_FUNC_SFX
#define ATM_FUNC_SFX 1        // ATM.playSfx()
#endif
#ifndef ATM_FUNC_TEMPO
#define ATM_FUNC_TEMPO 1      // ATM.setTempo()
#endif
#ifndef ATM_FUNC_CUE
#define ATM_FUNC_CUE 1        // ATM.check() / ATM.check(id)
#endif

// Notes. Each flag is one octave of the 1..63 note numbers.
// C2-B2, C3-B3, C4-B4, C5-B5, C6-B6, C7-D7. Excluded notes are silent.
#ifndef ATM_OCTAVE_2
#define ATM_OCTAVE_2 1        // C2-B2
#endif
#ifndef ATM_OCTAVE_3
#define ATM_OCTAVE_3 1        // C3-B3
#endif
#ifndef ATM_OCTAVE_4
#define ATM_OCTAVE_4 1        // C4-B4
#endif
#ifndef ATM_OCTAVE_5
#define ATM_OCTAVE_5 1        // C5-B5
#endif
#ifndef ATM_OCTAVE_6
#define ATM_OCTAVE_6 1        // C6-B6
#endif
#ifndef ATM_OCTAVE_7
#define ATM_OCTAVE_7 1        // C7-D7
#endif

#endif
