//alert: remember to use str.c_str() when returning Strings from functions
#include <Arduino.h>
#include <StandardCplusplus.h>
#include <vector>
#include <string>
using namespace std;

#include <Servo.h>
#include <LiquidCrystal.h>
#include <SimpleTimer.h> //events

//#include <_SimulateDebug.h>
#include "_globals.h"
#include "SerialExt.h"
#include "RTCExt.h"
#include "AnalogSwitch.h"
#include "RODoser.h"
#include "FishFeeder.h"
#include "ServoMotor.h"
//#include <LCDMenu.h>
#include "LCDDisplay.h"

using namespace Globals;
using namespace Utils;
using namespace LCD;

ControllerType _controllerType(true, false, true);

#pragma region DOSER

int _doserPin = 53;
int _floatSwitchPin = MegaPins::A_15;
AnalogSwitch _floatSwitch(_floatSwitchPin);
Servo _servo;
RODoser _doser;

#pragma endregion DOSER

#pragma region FEEDER

vector<FishFeeder> _feeders;
int _feederPin1 = 30;
int _feederPin2 = 31;
int _feederPin3 = 32;
int _feederPin4 = 33;

#pragma endregion FEEDER

//LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);
LCDDisplay _lcdDisplay;

bool _timersEnabled = true;
SimpleTimer _selectPressTimer;
SimpleTimer _doserTimer;
SimpleTimer _feederTimer;

void IsSelectPressed();
void FeedFish();
void RunDoser();

// the setup function runs once when you press reset or power the board
void setup() {

    Serial.begin(115200);
    while(!Serial);

    //RTCExt::Init();
    _lcdDisplay.Init();

    if(_controllerType.Doser) {
        _doser = RODoser(_servo, _doserPin, 2, 22000, _floatSwitch);
        _doserTimer.setInterval(4000, RunDoser);
    }
    if(_controllerType.Feeder) {
        FishFeeder feeder1 = FishFeeder::CreateFeeder(_feederPin1, 2);
        feeder1.RunEverySeconds = 30;
        _feeders.push_back(feeder1);
        FishFeeder feeder2 = FishFeeder::CreateFeeder(_feederPin2, 2);
        feeder2.RunEverySeconds = 30;
        _feeders.push_back(feeder2);
        _feederTimer.setInterval(4000, FeedFish);
    }

    _selectPressTimer.setInterval(500, IsSelectPressed);

    SerialExt::Debug(RTCExt::IsRTCTimeSet());
    //RTCExt::SetRTCTime(9, 42, 0, 8, 4, 2016);
    if(!RTCExt::IsRTCTimeSet())
        _lcdDisplay.SelectMainMenu();
}
// the loop function runs over and over again forever
void loop() {
    if(_controllerType.Doser)
        _doserTimer.run();
    if(_controllerType.Feeder)
        _feederTimer.run();

    if(_timersEnabled) {
        _selectPressTimer.run();
    }

}
void IsSelectPressed() {

    int key = _lcdDisplay.GetKey();
    SerialExt::Debug("key_isp", key);

    if(key == 4) {
        _timersEnabled = false; //disable
        _lcdDisplay.SelectMainMenu();
        _timersEnabled = true; //enable
    }

    _lcdDisplay.Scroll();

}

void FeedFish() {
    //_rtcController.SetLastRun(AccessoryType::Feeder);

    int incomingNum = SerialExt::Read();
    if(incomingNum > 0) {
        bool processed = _doser.ProcessSerialInput(incomingNum);
    }

    //1 to run, 2 to run demo
    bool runMotor = ServoMotor::ShouldRunMotorBySerialInput(incomingNum);
    bool runMotorDemo = ServoMotor::ShouldRunMotorDemo(incomingNum);

    if(!runMotor && !runMotorDemo) {
        FishFeeder feeder = _feeders[0];
        runMotor = feeder.ShouldRunMotor(true);
        SerialExt::Debug("feeder_runMotor: ", runMotor);
    }

    if(runMotor) {
        //SerialExt::Print("Signaling Relay ON, pin: ", _feederPwrSigPin);
        //digitalWrite(_feederPwrSigPin, LOW); //to send relay signal
        //int potVal = analogRead(_potShakesPin); // reads the value of the potentiometer (value between 0 and 1023)
        //SerialExt::Print("Pot Val: ", potVal);
        FishFeeder::FeedAll(_feeders);
        //SerialExt::Print("Signaling Relay OFF, pin: ", _feederPwrSigPin);
        //digitalWrite(_feederPwrSigPin, HIGH);
    } else if(runMotorDemo) {
        FishFeeder::RunDemo(_feeders);
    }
    //_secondFeeding = _secondFeeding * 3600000;
    //SerialExt::Print("Second Feeding in (Hours): ", _secondFeeding / 3600000);
    //delay(_secondFeeding);//delay 9 hours before next feed.
    //delay(10000); //for testing

}

void RunDoser() {
    //SerialExt::Debug("_runEverySeconds ",_runEverySeconds);

    int incomingNum = SerialExt::Read();
    if(incomingNum > 0) {
        bool processed = _doser.ProcessSerialInput(incomingNum);
    }

    //1 to run, 2 to run demo
    bool runMotor = ServoMotor::ShouldRunMotorBySerialInput(incomingNum);
    bool runMotorDemo = ServoMotor::ShouldRunMotorDemo(incomingNum);

    if(!runMotor && !runMotorDemo) {
        runMotor = _doser.ShouldRunMotor(true);
        SerialExt::Debug("doser_runMotor: ", runMotor);
    }

    if(runMotor) {

        //int shakesOrTurns = menuController.GetShakesOrTurns(accType);
        //_doser.Shakes = shakesOrTurns;

        SerialExt::Debug("switchIsOn: ", _doser.TheSwitch.IsOn());
        SerialExt::Debug("switchVal: ", _doser.TheSwitch.SwitchReading);
        _doser.Dose();
    } else if(runMotorDemo) {
        vector<RODoser> dosers;
        dosers.push_back(_doser);
        RODoser::RunDemo(dosers);
    }
    //FeedFish();
}



