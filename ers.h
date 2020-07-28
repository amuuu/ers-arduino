/*
Ers.H - Easy Read (from any sensor) and Send (through ESP8266) for Arduino.
Created by AMU, July and August 2020
Released under GPL :)
*/ 

#ifndef Ers_h
#define Ers_h
#include "Arduino.h"
#include <SoftwareSerial.h>

const int LONG_DELAY = 5000;
const int MEDIUM_DELAY = 1000;
const int SHORT_DELAY = 200;

const int MAX_BUFFER_SIZE = 100;
const int MAX_INPUT_PIN_NUM = 10;


class Ers
{
public:
    int mode; // 0 normal,
              // 1 only read from sensors (and don't send)
    
    int bufferSize;
    int buffer[MAX_BUFFER_SIZE];
    
    // For delays?
    //int longDelay, mediumDelay, shortDelay

    EspHandler esp;
    InputHandler inputs;

private:
    int _currentBufferIndex;

public:
    Ers(int mode,
        int bufferSize, int numSensors,
        int* sensorPins,
        int txPin, int rxPin,
        String ssid, String passwd,
        String serverIp, String uri,
        int espBaud);

    ~Ers();
    void updateBuffer();
    

private:
    bool _initErsParams();
    void _addToBuffer(int triggeredPin);
    void _sendBuffer();
    //bool _resetErsParams();
}




class InputHandler
{
public:    
    int numSensors;
    int* inputPins;
    int* inputData;
    bool* inputIsTriggered;

public:
    InputHandler(int numSensors, int* sensorPins);
    ~InputHandler();

    void readData();

private:
    _initArrays();
    _resetArrays();
}




class EspHandler
{
public:
    int espBaud;
    SoftwareSerial esp;
    int txPin, rxPin;
    String ssid,passwd;
    String serverIp, serverUri;

    SoftwareSerial esp;

private:
    int _longDelay, _mediumDelay, _shortDelay;
    ///
public:
    EspHandler(int txPin, int rxPin,
                String ssid, String passwd,
                String serverIp, String uri,
                int espBaud = 9600);
    ~EspHandler();
    void espBegin(); // done
    bool resetEsp(); // done
    bool connectToWifi(); // done 
    bool changeEspBaud(int newBaudRate); // done
    bool sendData(String data);
}
