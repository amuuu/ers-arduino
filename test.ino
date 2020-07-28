
void setup() {
  esp.begin(9600); // TLDR: When you change the baud rate, it will be saved on the module.
                   // You can change the baud rate using this command: AT+CIOBAUD=9600 (RECOMMENDED RATE: 9600)
                   // Note: If you change the baud from 115200 to 9600 and you decide to use that module with
                   // 115200 baud on for example another projet afterwards, the software serial connection won't work
                   // because previously the baud rate has been set to 9600. You have to change it back to 115200.
  Serial.begin(9600);
  
  initInputPins();
  initInputArrays();
  initBuffer();

  resetEsp();
  connectToWifi();

  delay(5000);
}

void loop() {
  readPirSensors();
  analyzePirSensorsOutputs();
  //printBuffer();
  delay(1000);
}
