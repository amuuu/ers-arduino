////////////////////////////////////////////////////////////////////
///   make sure to calibrate your sensors for better results!    ///
////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
#define RX 11 
#define TX 12

// Change these based on your board
#define BUFFER_SIZE 10
#define NUM_PIR_SENSORS 2
int pirPins[] = {3,4}; // Digital pins for PIR sensors


SoftwareSerial esp(RX,TX);


// Change these based on your network
String networkSsid = "amu";
String networkPasswd = "amuamuamu";
String server = "192.168.43.26"; // Your local server IP or any other IP
String uri = "/";


// Internal variables
int pirReads[NUM_PIR_SENSORS]; // The data which the sensors read
bool pirTriggers[NUM_PIR_SENSORS]; // Gets true when a sensor gets triggered
int triggerBuffer[BUFFER_SIZE]; // Data is kept in a buffer and
                                // when the buffer gets full,
                                // it will get sent through the wifi module
                                // to the server
int currentBufferIndex=0;




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

void initBuffer() {
  for (int i=0; i<BUFFER_SIZE; i++) {
    triggerBuffer[i]=0;
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

void initInputPins() {
  for (int i=0; i<NUM_PIR_SENSORS; i++) {
    pinMode(pirPins[i], INPUT);
  }
}

void initInputArrays(){
  for (int i=0; i<NUM_PIR_SENSORS; i++) {
    pirReads[i]=-1;
    pirTriggers[i]=false;
  }
}

void resetEsp() {
  esp.println("AT+RST");
  delay(1000);
  if (esp.find("OK")) {
    Serial.println("ESP module reset successfully");
  }
  else {
    Serial.println("Reset error");
  }
}

void connectToWifi() {
  Serial.println("Setting up...");
  String setup_cmd = "AT+CWMODE=3"; 
  esp.println(setup_cmd);
  delay(1000);

  Serial.println("Connecting to wifi...");
  String connect_cmd = "AT+CWJAP=\""+networkSsid+"\",\""+networkPasswd+"\"";
  esp.println(connect_cmd);
  delay(4000);
  if (esp.find("OK")) {
    Serial.println("Connected to wifi successfully!");
  }
  else {
    connectToWifi();
    Serial.println("Cannot connect to wifi");
  }  
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
