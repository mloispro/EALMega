#include <Arduino.h>
//#include <stdio.h>
//#include <WString.h>
#include <SimpleTimer.h>
//#include <SoftwareSerial.h>
//#include <Wire.h>
//#include <SPI.h>
//#include <RF24.h>

#include <StandardCplusplus.h>
#include <vector>
#include <string>
using namespace std;


#include "_globalsWaterSensors.h"
#include "CmdMessengerExt.h"
using namespace Globals;

SimpleTimer _asyncTimer;

#pragma endregion PCComm

void AsyncDoWork();

void setup(void) {
    // Listen on serial connection for messages from the pc
    Serial.begin(9600);
    while(!Serial);
    _asyncTimer.setInterval(1000, AsyncDoWork);

    CmdMessengerExt::Init();
}

void loop(void) {
    _asyncTimer.run();

    //_phSensor.PrintPHToSerial();
    ThePHSensor.PrintPHToLCD();

    CmdMessengerExt::Loop();
}

void AsyncDoWork() {
    ThePHSensor.CalculatePH();
}

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

