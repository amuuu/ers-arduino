/*
Ers.H - Easy Read (from any sensor) and Send (through ESP8266) for Arduino.
Created by AMU, July and August 2020
Released under GPL :)
*/ 

#ifndef Ers_h
#define Ers_h
#include "Arduino.h"
#include <SoftwareSerial.h>

const int MAX_BUFFER_SIZE = 100;
const int MAX_INPUT_PIN_NUM = 10;

class Ers
{
public:
    int bufferSize;
    int buffer[MAX_BUFFER_SIZE];
    
    // For delays?
    //int longDelay, mediumDelay, shortDelay

    EspHandler esp;
    InputHandler inputs;

private:
    int _currentBufferIndex;

public:
    Ers(int bufferSize, int numSensors,
        int* sensorPins,
        int txPin, int rxPin,
        String ssid, String passwd,
        String serverIp, String uri,
        int espBaud = 9600);

    ~Ers();
    
    

private:
    bool _initErsParams();
    bool _resetErsParams();
}




class InputHandler
{
public:    
    int numSensors;
    int* inputPins;

private:
    int* _inputData;
    int* _inputIsTriggered;

public:
    InputHandler(int numSensors, int* sensorPins);
    ~InputHandler();

    void readData();
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


}
