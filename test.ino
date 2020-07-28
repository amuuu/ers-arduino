#include "ers.h"

Ers ers;

void setup() {
  ers = new Ers(0,
                    10, 2, {3, 4},
                    10, 11,
                    "thessid", "passwd",
                    "192.168.0.1", "/",
                    9600);

  ers.esp.espBegin();
  Serial.begin(9600);
  
  ers.esp.resetEsp();
  ers.esp.connectToWifi();

  delay(LONG_DELAY);
}

void loop() {

  ers.inputs.readData();

  ers.printBuffer();
  ers.updateBuffer();

  delay(1000);
}
