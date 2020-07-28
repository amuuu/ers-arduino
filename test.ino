
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


void readPirSensors(){
  for (int i=0; i<NUM_PIR_SENSORS; i++){
    pirReads[i] = digitalRead(pirPins[i]);
    delay(100);
  }
  delay(100);
}

void analyzePirSensorsOutputs(){
  for (int i=0; i<NUM_PIR_SENSORS; i++)
  {
    if (pirReads[i] == HIGH) {
      if (!pirTriggers[i]) {
        Serial.print("TRIGGERED ");
        Serial.print(pirPins[i]);
        Serial.println();
        
        addToBuffer(pirPins[i]);
        pirTriggers[i] = true;
      }
    }
    else {
      if (pirTriggers[i]) {
        pirTriggers[i] = false;
        //pirReads[i] = LOW;
      }
    }
  }
  delay(100);
}

void addToBuffer(int triggredPin) {
  triggerBuffer[currentBufferIndex++] = triggredPin;
  
  if (currentBufferIndex == BUFFER_SIZE) {
    Serial.println("Buffer got full. Sending it through wifi.");
    printBuffer();
    sendBuffer();
    
    currentBufferIndex = 0;
    //initBuffer();
  }
}

void sendBuffer() {
  // send the buffer through wifi
  String data="";
  for (int i=0; i<BUFFER_SIZE; i++) {
    data += String(triggerBuffer[i]);
    if (i != BUFFER_SIZE-1)
      data += "-";
  }

  sendHttpPost(data);
}

void printBuffer() {
  Serial.println("::::::BUFFER::::::");
  for (int i=0; i<BUFFER_SIZE; i++) {
    Serial.print(triggerBuffer[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
}


void sendHttpPost(String data) {
  // Handling the TCP conncetion
  Serial.println("Handling TCP connection...");
  esp.println();
  //if (esp.find("OK")) {
    //Serial.println("TCP connection already ready");
 // }
  //else {
    esp.print("AT+CIPSTART=\"TCP\",\"");
    esp.print(server);
    esp.println("\",80"); //start a TCP connection.
    if (esp.find("OK")) {
      Serial.println("TCP connection ready");
      delay(500);
    }
  //}
  delay(1000);

  String postRequest = data;

  // Number of the characters to be sent.
  Serial.println("Sending the number of characters...");
  esp.print("AT+CIPSEND=");
  esp.print(String(postRequest.length()));
  esp.print("\r\n");  
  
  delay(500);
  
  if (esp.find(">")) {
    Serial.println("Sending the post request...");
    esp.println(postRequest);
    
    if (esp.find("SEND OK")) {
      Serial.println("Packet sent");
      while (esp.available()) {
        String tmpResp = esp.readString();
        Serial.print("tmpresp");
        Serial.print(tmpResp);
        Serial.println();
      }
      // close the connection
      esp.println("AT+CIPCLOSE");
    }
  
  }
  
  delay(500);
}
