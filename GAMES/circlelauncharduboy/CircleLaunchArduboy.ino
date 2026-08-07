#include "CircleLaunch.h"
CircleLaunch app;

void setup() {
  Serial.begin(115200);
  engine.setup();
}

void loop() {  
  app.run();
}