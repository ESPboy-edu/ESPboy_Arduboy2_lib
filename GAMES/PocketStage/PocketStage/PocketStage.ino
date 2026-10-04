#include <Arduboy2.h>
#include "fxdta.h"
#include <ArduboyFX.h>

#include <stdlib.h>
#include "fxdata.h"
#include "tables.h"
#include "flash.h"
#include "audio.h"

Arduboy2 arduboy;

#include "play.h"
#include "title.h"
#include "menus.h"


static uint8_t exit_hold = 0;
static uint8_t fps_count = 0, fps_value = 0;
static uint32_t fps_since = 0;

void setup() {
    arduboy.begin();
    arduboy.setFrameRate(40);
    Serial.begin(115200);
    
    flash_init();
    audio_init();
    save_load();
    
    lanes_flipped = (settings & SET_LEFTY) != 0;
    title_open();
}

void loop() {
  if (!arduboy.nextFrame()) return;
  
  fps_count++;
  if ((uint32_t)(millis() - fps_since) >= 1000) {
    fps_value = fps_count;
    fps_count = 0;
    fps_since = millis();
  }
  
  // Прямая проверка кнопок для выхода в меню, не используем устаревший buttons_now
  if (page != PAGE_PLAY && arduboy.pressed(UP_BUTTON) && arduboy.pressed(DOWN_BUTTON)) {
    if (++exit_hold >= 120) arduboy.exitToBootloader();
  } else {
    exit_hold = 0;
  }
  
  switch (page) {
    case PAGE_TITLE:
      title_update();
      title_draw();
      break;
    case PAGE_MAIN:
      main_update();
      main_draw();
      break;
    case PAGE_SETTINGS:
      settings_update();
      settings_draw();
      break;
    case PAGE_LEVEL:
      level_update();
      level_draw();
      break;
    case PAGE_SETLIST:
      setlist_update();
      setlist_draw();
      break;
    case PAGE_PLAY:
      play_page_update();
      if (page == PAGE_PLAY) {
        play_draw();
        if (song_over) results_open();
        else if (song_failed) failed_open();
        break;
      }
    case PAGE_PAUSE:
      if (page == PAGE_PAUSE) pause_update();
      pause_draw();
      break;
    case PAGE_CREDITS:
      credits_update();
      credits_draw();
      break;
    case PAGE_FAILED:
      results_update();
      failed_draw();
      break;
    default:
      results_update();
      results_draw();
      break;
  }
  
  arduboy.display();
}
