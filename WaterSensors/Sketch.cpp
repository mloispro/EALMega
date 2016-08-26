
#include <Arduino.h>
#include <stdio.h>
#include <WString.h>
#include <SoftwareSerial.h>
#include <Wire.h>
//#include <SPI.h>
//#include <RF24.h>


#include <StandardCplusplus.h>
#include <vector>
#include <string>
using namespace std;

#include <CmdMessenger.h>

#include "PHSensor.h"

#pragma region PHSensor

int _phSensorPin = A15;
//LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);
PHSensor _phSensor(_phSensorPin, 800, true);

#pragma endregion PHSensor

#pragma region PCComm
// Attach a new CmdMessenger object to the default Serial port
CmdMessenger _cmdMessenger = CmdMessenger(Serial);

bool _acquireData                   = false;
const unsigned long _sampleInterval = 1000; // 1 second interval
unsigned long _previousSampleMillis = 0;
long _startAcqMillis                = 0;

// This is the list of recognized commands. These can be commands that can either be sent or received.
// In order to receive, attach a callback function to these events
enum {
    // Commands
    kAcknowledge         , // Command to acknowledge that cmd was received
    kError               , // Command to report errors
    kStartLogging        , // Command to request logging start      (typically PC -> Arduino)
    kSendPH       , // Command to request PH (typically Arduino -> PC)
};

void AttachCommandCallbacks();
void OnUnknownCommand();
void OnArduinoReady();
void OnStartLogging();
bool HasExpired(unsigned long &prevTime, unsigned long interval);
void SendPH();

#pragma endregion PCComm

void setup(void) {
    // Listen on serial connection for messages from the pc
    Serial.begin(9600);
    while(!Serial);
    // Adds newline to every command
    _cmdMessenger.printLfCr();

    // Attach my application's user-defined callback methods
    AttachCommandCallbacks();

    // Send the status to the PC that says the Arduino has booted
    _cmdMessenger.sendCmd(kAcknowledge, "Arduino has started!");
}



void loop(void) {
    //_phSensor.PrintPHToSerial();
    _phSensor.PrintPHToLCD();

// Process incoming serial data, and perform callbacks
    _cmdMessenger.feedinSerialData();

// Do measurement after certain sample interval
    if (HasExpired(_previousSampleMillis, _sampleInterval)) {
        if (_acquireData) {
            SendPH();
        }
    }
}

#pragma region PCCommMethods

// Commands we send from the PC and want to receive on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below.

void AttachCommandCallbacks() {
    // Attach callback methods
    _cmdMessenger.attach(OnUnknownCommand);
    _cmdMessenger.attach(kStartLogging, OnStartLogging);
}

// Called when a received command has no attached function
void OnUnknownCommand() {
    _cmdMessenger.sendCmd(kError, "Command without attached callback");
}

// Callback function that responds that Arduino is ready (has booted up)
void OnArduinoReady() {
    _cmdMessenger.sendCmd(kAcknowledge, "Arduino ready");
}

// Callback function calculates the sum of the two received float values
void OnStartLogging() {
    // Start data acquisition
    _startAcqMillis = millis();
    _acquireData    = true;
    _cmdMessenger.sendCmd(kAcknowledge, "Start Logging");
}

// Returns if it has been more than interval (in ms) ago. Used for periodic actions
bool HasExpired(unsigned long &prevTime, unsigned long interval) {
    if (  millis() - prevTime > interval ) {
        prevTime = millis();
        return true;
    }
    else {
        return false;
    }
}

// simple readout of two Analog pins.
void SendPH() {

    float seconds = (float) (millis() - _startAcqMillis) / 1000.0 ;
    float ph = _phSensor.GetPH();

    _cmdMessenger.sendCmdStart(kSendPH);
    _cmdMessenger.sendCmdArg(seconds, 4);
    _cmdMessenger.sendCmdSciArg(ph, 2);
    //cmdMessenger.sendCmdSciArg(Analog2);
    _cmdMessenger.sendCmdEnd();
}
#pragma endregion PCCommMethods

#pragma region ArduinoToArduinoCom
//void setup() {
//Serial.begin(9600);
//Wire.begin();                // join i2c bus with address #8
//}
//
//
//
//void loop() {
//Serial.println("loop");
//String phString = String(_ph);
//Wire.beginTransmission(8); // transmit to device #8
//Wire.write("PH:");        // sends five bytes
//Wire.write(phString.c_str());              // sends one byte
//Wire.endTransmission();    // stop transmitting
//
//_ph++;
//delay(1000);
//}
#pragma endregion ArduinoToArduinoCom

