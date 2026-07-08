#include <Arduboy2.h>
#include <ArduboyTones.h>

// --- ADDED: Подключаем библиотеку Wi-Fi для управления питанием ---
#include <ESP8266WiFi.h>

#include "Engine.h"
#include "ArduboyPlatform.h"

Arduboy2Base arduboy;
ArduboyTones sound(arduboy.audio.enabled);

unsigned long lastTimingSample;

void ArduboyPlatform::playSound(const uint16_t* pattern)
{
  sound.tones(pattern);
}

void setup() {
  arduboy.boot();
  
    // --- CHANGED: Выключаем Wi-Fi при старте для экономии батареи ---
    // Настоящая инициализация сети произойдет в ArduboyPlatform.cpp 
    // только при выборе режима Multiplayer!
    WiFi.mode(WIFI_OFF);
    // -----------------------------------------------------------------

  arduboy.setFrameRate(TARGET_FRAMERATE);
  
    // --- CHANGED: Безопасное ожидание отпускания кнопок (кормим Watchdog) ---
    // Оригинальный arduboy.waitNoButtons() может вызвать WDT Reset на ESP8266
  while (arduboy.buttonsState()) {
        yield();
        ESP.wdtFeed();
    }
    // ------------------------------------------------------------------------

  engine.init();
}

void loop() {
  if (!arduboy.nextFrame()) return;

  Platform.update();

  engine.update();

  engine.draw();

  arduboy.display(true);
}
