#pragma once

#include <Arduboy2.h>

const uint16_t rideOfTHeValkyries[] PROGMEM = {
 NOTE_REST,460, NOTE_B4,117, NOTE_REST,187, NOTE_FS4,62, NOTE_REST,7, NOTE_B4,125, NOTE_REST,62, NOTE_D5,515,
 NOTE_REST,46, NOTE_B4,531, NOTE_REST,39, NOTE_D5,125, NOTE_REST,171, NOTE_B4,46, NOTE_REST,31, NOTE_D5,117,
 NOTE_REST,62, NOTE_FS5,523, NOTE_REST,46, NOTE_D5,523, NOTE_REST,31, NOTE_FS5,117, NOTE_REST,187, NOTE_D5,54,
 NOTE_REST,15, NOTE_FS5,132, NOTE_REST,54, NOTE_A5,523, NOTE_REST,39, NOTE_A4,507, NOTE_REST,54, NOTE_D5,117,
 NOTE_REST,179, NOTE_A4,39, NOTE_REST,46, NOTE_D5,117, NOTE_REST,70, NOTE_FS5,1000, TONES_END
};

const uint16_t introduction_sound[] PROGMEM = {
NOTE_A6,40, 0, 5, NOTE_C6,40,  0, 5,  NOTE_E6,40, 0, 5, NOTE_G6,40,  0, 5, 
TONES_END };

const uint16_t blade_1[] PROGMEM = {
NOTE_A2, 8, 0, 4, 
TONES_END };

const uint16_t blade_2[] PROGMEM = {
NOTE_A2, 8, 0, 4, 
NOTE_A2, 8, 0, 4, 
TONES_END };

const uint16_t blade_3[] PROGMEM = {
NOTE_A2, 8, 0, 4, 
NOTE_A2, 8, 0, 4, 
NOTE_A2, 8, 0, 4, 
TONES_END };

const uint16_t blade_4[] PROGMEM = {
NOTE_A2, 8, 0, 4, 
NOTE_A2, 8, 0, 4, 
NOTE_A2, 8, 0, 4, 
NOTE_A2, 8, 0, 4, 
TONES_END };

const uint16_t player_firing[] PROGMEM = {
NOTE_A4, 10, 0, 2, 
NOTE_C4, 6, 0, 2,
NOTE_B5, 7, 
TONES_END };

const uint16_t tank_firing[] PROGMEM = {
NOTE_A2, 10, 0, 2, 
NOTE_C2, 6, 0, 2,
NOTE_B3, 7, 
TONES_END };

const uint16_t miss[] PROGMEM = {
NOTE_A1, 10, 0, 2, 
NOTE_C1, 6, 
TONES_END };

const uint16_t safety[] PROGMEM = {
NOTE_A6, 10, 0, 2, 
NOTE_C6, 10, 0, 2, 
NOTE_A6, 10, 
TONES_END };

const uint16_t kill_hostage[] PROGMEM = {
NOTE_A7, 10, 0, 2, 
NOTE_B6, 10, 0, 2, 
NOTE_D6, 10, 
TONES_END };

const uint16_t collect_hostage[] PROGMEM = {
NOTE_A4, 8, 0, 2, 
NOTE_B3, 8, 0, 2, 
NOTE_D3, 8, 
TONES_END };

const uint16_t exploding[] PROGMEM = {
NOTE_A1, 10, 0, 2, 
NOTE_E3, 11, 0, 6, 
NOTE_C0, 7, 0, 3, 
NOTE_A3, 15, 0, 12, 
NOTE_B2, 9, 0, 7, 
NOTE_G2, 16, 0, 8, 
NOTE_F3, 5, 0, 8, 
NOTE_C5, 8, 0, 2, 
NOTE_F2, 13, 0, 5, 
NOTE_A0, 13, 0, 8, 
NOTE_A1, 15, 0, 2, 
NOTE_E3, 11, 0, 6, 
NOTE_C0, 7, 0, 3, 
NOTE_A3, 15, 0, 12, 
NOTE_B2, 9, 0, 7, 
NOTE_G2, 16, 0, 8, 
NOTE_F3, 5, 0, 8, 
NOTE_C2, 8, 0, 2, 
NOTE_F1, 13, 0, 5, 
NOTE_A0, 13, 0, 8, 
NOTE_C0, 6, 
TONES_END };

