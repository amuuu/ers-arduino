# ERS for Arduino
ERS is an Arduino library that provides abstraction for sensor reads and ESP8266 that makes the process of reading and sending data to servers much easier.

A simple thing that normally takes a lot of code to make happen, is abstracted and can be done in just a few lines:

```
#include "ers.h"

int pins[2] = {3,4};
Ers ers(0, 10, 2, &pins[0], 10, 11, "yourssid", "yourpasswd", "192.168.0.1", "/", 9600);

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
```

You can easily modify the library for your projects and make it do what's needed.

NOTE: This library isn't added to official libraries of Arduino yet and it will be added as soon as it gets tested and has a proper documentation :)