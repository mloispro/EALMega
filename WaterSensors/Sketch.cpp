#include <Arduino.h>
#include <SimpleTimer.h>
#include <Wire.h>

#include <StandardCplusplus.h>
#include <vector>
#include <string>
using namespace std;

#include "_globalsWaterSensors.h"
#include "WaterSensorWire.h"
//#include "CmdMessengerExt.h"
using namespace Globals;

SimpleTimer _asyncTimer;

void AsyncDoWork();

void setup(void) {
    WaterSensorWire::Setup();

    // Listen on serial connection for messages from the pc
    Serial.begin(9600);
    while(!Serial);

    _asyncTimer.setInterval(1000, AsyncDoWork);

    //CmdMessengerExt::Init();
}

void loop(void) {

    _asyncTimer.run();

    Globals::ThePHSensor.PrintPHToLCD();
    Globals::TheTDSSensor.PrintTDSToLCD();

    //CmdMessengerExt::Loop();
}

void AsyncDoWork() {
    Globals::ThePHSensor.CalculatePH();
    Globals::TheTDSSensor.CalculateTDS();
}




