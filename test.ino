#include "ers.h"

int pins[2] = {3,4};
Ers ers(0, 10, 2, &pins[0], 10, 11, "thessid", "passwd", "192.168.0.1", "/", 9600);

void setup() {
  Serial.begin(9600);  

  ers.esp->espBegin();
  ers.esp->resetEsp();
  ers.esp->connectToWifi();

  delay(LONG_DELAY);
}

void loop() {
  ers.inputs->readData();
  ers.printBuffer();
  ers.updateBuffer();
  delay(MEDIUM_DELAY);
}
