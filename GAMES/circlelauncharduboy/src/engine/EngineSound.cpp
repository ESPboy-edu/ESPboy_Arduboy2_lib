#include "Engine.h"

// Оставляем только функцию добавления звука
void Engine::addSound(uint8_t index, const int16_t* data, int length, int loopStartAt) {
  uint8_t i = 0;
  uint8_t currentIndex = 0;

  while (currentIndex < index && i < length) {
    int16_t value = (int16_t)pgm_read_word(&data[i]);
    if (value == -1) {
      currentIndex++;
      i += 1;
    } else {
      i += 2;
    }
  }

  soundData = &data[i];
  soundFrame = 0;
}