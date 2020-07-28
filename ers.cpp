#include "Arduino.h"
#include "ers.h"


Ers::Ers(int mode,
        int bufferSize, int numSensors,
        int* sensorPins,
        int txPin = -1, int rxPin = -1,
        String ssid = "", String passwd = "",
        String serverIp = "", String uri = "/",
        int espBaud = 9600)
{
    
    if (bufferSize <= MAX_BUFFER_SIZE) {
        this->bufferSize = bufferSize;
    }
    else {
        throw "Buffer size can't be that big, max size is" + String(MAX_BUFFER_SIZE);
    }
    
    inputs = new InputHandler(numSensors, sensorPins);

    if (mode != 1) {
        esp = new EspHandler(txPin, rxPin, ssid, passwd, serverIp, uri, espBaud);
    }

    initErsParams();
    currentBufferIndex = 0;
}

void Ers::initErsParams() {
    for (int i=0; i<bufferSize; i++) {
        buffer[i]=0;
    }
}


void Ers::updateBuffer() {
    for (int i=0; i<inputs.numSensors; i++) {
        if (inputs->inputData[i] == HIGH) {
            if (!inputs->inputIsTriggered[i]) {
                addToBuffer(inputs->inputPins[i]);
                inputs->inputIsTriggered[i] = true;
            }
        }
        else 
            if (!inputs->inputIsTriggered[i])
                !inputs->inputIsTriggered[i] = false;
    }
    delay(SHORT_DELAY);
}

void Ers::addToBuffer(int triggeredPin) {
    buffer[currentBufferIndex++] = triggeredPin;
    if (currentBufferIndex == bufferSize) {
        sendBuffer();        
        currentBufferIndex = 0;
    }
}

void Ers::sendBuffer() {
    String data="";
    for (int i=0; i<bufferSize; i++) {
        data += String(buffer[i]);
        if (i != bufferSize-1)
            data += "-";
    }

    esp.sendData(data);
}


void Ers::printBuffer() {
    Serial.println("::::::BUFFER::::::");    
    for (int i=0; i<bufferSize; i++) {
        Serial.print(buffer[i]);
        Serial.print(" ");
    }
    Serial.println();
}



EspHandler::EspHandler(int txPin, int rxPin,
                    String ssid, String passwd,
                    String serverIp, String uri,
                    int espBaud = 9600)
{
    if ((txPin<2 && txPin>-1) || (rxPin<2 && rxPin>-1)) {
        throw "Can't choose TX and RX in pins 0 and 1."
    }
    else {
        this->txPin = txPin;
        this->rxPin = rxPin;
        esp = new SoftwareSerial(txPin, rxPin);
    }

    if ((ssid=="") || (passwd=="")) {
        throw "Network SSID and password can't be empty!"
    }
    else {
    this->ssid = ssid;
    this->passwd = passwd;
    }

    if (serverIp=="") {
        throw "Server IP can't be empty."
    }
    else {
        this->serverIp = serverIp;
        this->uri = uri;
    }

    this->espBaud = espBaud;
    
}

void EspHandler::espBegin() {
    esp.begin(espBaud);
}

bool EspHandler::resetEsp() {
    esp.println("AT+RST");
    delay(MEDIUM_DELAY);
    if (esp.find("OK"))
        return true;
    else
        return false;
}

bool EspHandler::connectToWifi() {
    esp.println("AT+CWMODE=3");
    delay(MEDIUM_DELAY);

    String connect_cmd = "AT+CWJAP=\""+ssid+"\",\""+passwd+"\"";
    esp.println(connect_cmd);
    delay(LONG_DELAY);
    
    if (esp.find("OK"))
        return true;
    else
        connectToWifi();
        return false;

}

void EspHandler::changeEspBaud(int newBaudRate) {
    String cmd = "AT+CIOBAUD="+String(newBaudRate);
    esp.println(cmd);
}

bool EspHandler::sendData(String data) {
    
    // Handling the TCP conncetion
    esp.println();
    String tcpCmd = "AT+CIPSTART=\"TCP\",\"" + serverIp + "\",80";
    esp.println(tcpCmd);
    
    if (!esp.find("OK"))
      return false;
    
    delay(MEDIUM_DELAY);


  // Number of the characters to be sent.
    String charLenCmd = "AT+CIPSEND=" + String(data.length()) + "\r\n";
    esp.print(charLenCmd);  
  
    delay(SHORT_DELAY);
  
    if (esp.find(">")) {
        esp.println(postRequest);        
        if (esp.find("SEND OK")) {
            while (esp.available()) {
                String tmpResp = esp.readString();
                //Serial.print("tmpresp");
                //Serial.print(tmpResp);
                //Serial.println();
            }
            // close the connection
            esp.println("AT+CIPCLOSE");
        }  
    }
  
    delay(MEDIUM_DELAY);
}




InputHandler::InputHandler(int numSensors, int* sensorPins) {
    if (numSensors != sizeof(sensorPins)/sizeof(int)) {
        throw "Number of sensors doesn't match the sensor pin array."
    }
    else {
        this->numSensors = numSensors;
        this->inputPins = sensorPins;
    }

    inputData = new int[numSensors];
    inputIsTriggered = new bool[numSensors];

}

void InputHandler::initArrays() {
    for (int i=0; i<numSensors; i++) {
        pinMode(sensorPins[i], INPUT);
        inputData[i] = -1;
        inputIsTriggered[i] = false;
    }
}

void InputHandler::readData() {
    for (int i=0; i<numSensors; i++) {
        inputData[i] = digitalRead(inputPins[i]);
        delay(SHORT_DELAY);
    }
    delay(SHORT_DELAY);
}