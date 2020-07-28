#include "Arduino.h"
#include "ers.h"


Ers::Ers(int bufferSize, int numSensors,
        int* sensorPins,
        int txPin, int rxPin,
        String ssid, String passwd,
        String serverIp, String uri,
        int espBaud = 9600)
{
    if (bufferSize <= MAX_BUFFER_SIZE)
        this->bufferSize = bufferSize;
    else
        throw "Buffer size can't be that big, max size is" + String(MAX_BUFFER_SIZE);
    
    this->bufferSize = bufferSize;
    esp = new EspHandler(txPin, rxPin, ssid, passwd, serverIp, uri, espBaud);
    inputs = new InputHandler(numSensors, sensorPins);



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
    this->txPin = txPin;
    this->rxPin = rxPin;
    
    this->ssid = ssid;
    this->passwd = passwd;

    this->serverIp = serverIp;
    this->uri = uri;
    this->espBaud = espBaud;


    _mediumDelay = 1000;
    _longDelay = 4000;
}

void EspHandler::espBegin() {
    esp.begin(espBaud);
}

bool EspHandler::resetEsp() {
    esp.println("AT+RST");
    delay(_mediumDelay);
    if (esp.find("OK"))
        return true;
    else
        return false;
}

bool EspHandler::connectToWifi() {
    esp.println("AT+CWMODE=3");
    delay(_mediumDelay);

    String connect_cmd = "AT+CWJAP=\""+ssid+"\",\""+passwd+"\"";
    esp.println(connect_cmd);
    delay(_longDelay);
    
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
    this->numSensors = numSensors;
    this->inputPins = sensorPins;
}