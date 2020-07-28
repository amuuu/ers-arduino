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

    _initErsParams();
    _currentBufferIndex = 0;
}

void Ers::_initErsParams() {
    for (int i=0; i<bufferSize; i++) {
        buffer[i]=0;
    }
}



EspHandler::EspHandler(int txPin, int rxPin,
                    String ssid, String passwd,
                    String serverIp, String uri,
                    int espBaud = 9600)
{
    if ((txPin<2 && txPin>-1) || (rxPin<2 && rxPin>-1)) {
        throw "Can't choose tx and rx in pins 0 and 1."
    }
    else {
        this->txPin = txPin;
        this->rxPin = rxPin;
        esp = new SoftwareSerial(txPin, rxPin);
    }

    if ((ssid=="") || (passwd=="")) {
        throw "SSID and password can't be empty!"
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

InputHandler::_initArrays() {
    for (int i=0; i<numSensors; i++) {
        pinMode(sensorPins[i], INPUT);
        inputData[i] = -1;
        inputIsTriggered[i] = false;
    }
}

