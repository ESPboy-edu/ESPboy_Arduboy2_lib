#pragma once

#define EngineForArduboy

// Подключаем вашу библиотеку ESPboy Arduboy2
#include <Arduboy2.h> 

// Создаем глобальный экземпляр (если он не создается внутри вашей библиотеки автоматически)
extern Arduboy2 arduboy;

#define WIDTH 128
#define HEIGHT 64

#include "Engine.h"