#include <Arduino.h>
#include <SimpleTimer.h>

//#include <EasyTransfer.h>
//#include <Wire.h>

#include <StandardCplusplus.h>
#include <vector>
#include <string>
using namespace std;

#include "_sketchExtWaterSensors.h"
////#include "CmdMessengerExt.h"
using namespace Sketch;

SimpleTimer _asyncTimer;


void setup(void) {
    // Listen on serial connection for messages from the pc
    Serial.begin(57600);
    while(!Serial);

    Sketch::Setup();

    int interval = 1000;
#if DEBUG
    interval = 1;
#endif

    _asyncTimer.setInterval(interval, Sketch::AsyncDoWork);

    //CmdMessengerExt::Init();
}

//String _request;
//String _response;
void loop(void) {

    _asyncTimer.run();

    Sketch::Loop();

}








