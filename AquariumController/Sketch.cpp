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

//ControllerType _controllerType(true, false, true);


#pragma region DOSER

bool _doserEnabled = false;
int _doserPin = 53;
int _floatSwitchPin = MegaPins::A_15;
AnalogSwitch _floatSwitch(_floatSwitchPin);
Servo _doserServo;
RODoser _doser;

#pragma endregion DOSER

#pragma region FEEDER

bool _feederEnabled = true;
int _feederRunEvery = 860;

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
SimpleTimer _accTimer;


void IsSelectPressed();
void FeedFish();
void RunDoser();
void AccTick();

#include "Testers\TimeTester.h"
#include "Testers\ServoTester.h"
using namespace Testers;

// the setup function runs once when you press reset or power the board
void setup() {

    Serial.begin(115200);
    while(!Serial);

    TimeTester _timeTester; //have to init in setup!
    _timeTester.NextRunVectorTest(AccessoryType::Feeder, false, 100, false);
    _timeTester.NextRunVectorTest(AccessoryType::DryDoser, false, 200, false);
    _timeTester.NextRunVectorTest(AccessoryType::DryDoser, false, 300, false);
    //_timeTester.RunAll();
    //ServoTester _servoTester;
    //_servoTester.RunAll()
    return;

    _lcdDisplay.Init(); //needs to run first to init rtc.

    _doser = RODoser(_doserServo, _doserPin, 2, 22000, _floatSwitch, _doserEnabled);

    FishFeeder feeder1 = FishFeeder::CreateFeeder(_feederPin1, 2, _feederRunEvery, _feederEnabled);
    _feeders.push_back(feeder1);
    FishFeeder feeder2 = FishFeeder::CreateFeeder(_feederPin2, 2, _feederRunEvery, _feederEnabled);
    _feeders.push_back(feeder2);

    _accTimer.setInterval(4000, AccTick);
    _selectPressTimer.setInterval(500, IsSelectPressed);

    //RTCExt::SetRTCTime(9, 42, 0, 8, 4, 2016);

}
// the loop function runs over and over again forever
void loop() {

    _accTimer.run();

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

void AccTick() {

    if(RTCExt::IsAccEnabled(AccessoryType::Feeder))
        FeedFish();
    if(RTCExt::IsAccEnabled(AccessoryType::DryDoser))
        RunDoser();
}
void FeedFish() {

    FishFeeder feeder = _feeders[0];
    bool runMotor = feeder.ShouldRunMotor(true);

    if(runMotor) {
        //SerialExt::Print("Signaling Relay ON, pin: ", _feederPwrSigPin);
        //digitalWrite(_feederPwrSigPin, LOW); //to send relay signal
        //int potVal = analogRead(_potShakesPin); // reads the value of the potentiometer (value between 0 and 1023)
        //SerialExt::Print("Pot Val: ", potVal);
        FishFeeder::FeedAll(_feeders);
        //SerialExt::Print("Signaling Relay OFF, pin: ", _feederPwrSigPin);
        //digitalWrite(_feederPwrSigPin, HIGH);
    }
}

void RunDoser() {

    return;

    bool runMotor = _doser.ShouldRunMotor(true);

    if(runMotor) {

        //SerialExt::Debug("switchIsOn: ", _doser.TheSwitch.IsOn());
        //SerialExt::Debug("switchVal: ", _doser.TheSwitch.SwitchReading);
        _doser.Dose();
    }

}

#pragma region OLD_STUFF

//void FeedFish() {
//
//int incomingNum = SerialExt::Read();
//if(incomingNum > 0) {
//bool processed = _doser.ProcessSerialInput(incomingNum);
//}
//
////1 to run, 2 to run demo
//bool runMotor = ServoMotor::ShouldRunMotorBySerialInput(incomingNum);
//bool runMotorDemo = false;//ServoMotor::ShouldRunMotorDemo(incomingNum);
//
//if(!runMotor && !runMotorDemo) {
//FishFeeder feeder = _feeders[0];
//runMotor = feeder.ShouldRunMotor(true);
////SerialExt::Debug("feeder_runMotor: ", runMotor);
//}
//
//FishFeeder feeder = _feeders[0];
//bool runMotor = feeder.ShouldRunMotor(true);
//
//if(runMotor) {
////SerialExt::Print("Signaling Relay ON, pin: ", _feederPwrSigPin);
////digitalWrite(_feederPwrSigPin, LOW); //to send relay signal
////int potVal = analogRead(_potShakesPin); // reads the value of the potentiometer (value between 0 and 1023)
////SerialExt::Print("Pot Val: ", potVal);
//FishFeeder::FeedAll(_feeders);
////SerialExt::Print("Signaling Relay OFF, pin: ", _feederPwrSigPin);
////digitalWrite(_feederPwrSigPin, HIGH);
//}
//else if(runMotorDemo) {
//FishFeeder::RunDemo(_feeders);
//}
////delay(10000); //for testing
//
//}

#pragma endregion OLD_STUFF