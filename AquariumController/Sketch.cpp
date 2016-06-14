﻿//alert: remember to use str.c_str() when returning Strings from functions
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
#include "Pump.h"
#include "LCDMenu.h"
#include "LCDDisplay.h"

using namespace Globals;
using namespace Utils;
using namespace LCD;

int _feederPin1 = 30;
int _feederPin2 = 31;
int _feederPin3 = 32;
int _feederPin4 = 33;
int _doserPin = 53;
int _doserRelayPin = 26;
int _doserFloatSwitchPin = MegaPins::A_15;
//todo: enter pump values
int _pumpPin = 53;
int _pumpRelayPin = 26;
//int _pumpFloatSwitchPin = MegaPins::A_15;

#pragma region DOSER

bool _doserEnabled = false;
AnalogSwitch _doserFloatSwitch(_doserFloatSwitchPin);
Servo _doserServo;
RODoser _doser;

#pragma endregion DOSER

#pragma region FEEDER

bool _feederEnabled = true;
int _feederRunEvery = 860;
vector<FishFeeder> _feeders;

#pragma endregion FEEDER

#pragma region PUMP

bool _pumpEnabled = false;
//AnalogSwitch _pumpFloatSwitch(_pumpFloatSwitchPin);
Pump _pump;

#pragma endregion PUMP

//LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);
LCDDisplay _lcdDisplay;

bool _timersEnabled = true;
SimpleTimer _selectPressTimer;
SimpleTimer _accTimer;


void IsSelectPressed();
void FeedFish();
void RunDoser();
void AccTick();
void RunPump();

//todo: comment this out
//#include "Testers\TimeTester.h"
//#include "Testers\ServoTester.h"
//#include "Testers\LcdTester.h"
//using namespace Testers;

// the setup function runs once when you press reset or power the board
void setup() {

    Serial.begin(115200);
    while(!Serial);

    //todo: comment this out
    //MemoryExt::Erase();

    //ServoTester _servoTester;//have to init in setup!
    //_servoTester.RunPumpLoop(_pumpPin, 5, _pumpRelayPin, _pumpFloatSwitch, 10);
    //_servoTester.RunDoser(_doserPin, 5, _doserRelayPin, _floatSwitchPin);
    //_servoTester.RunDoser(_doserPin, 5, _doserRelayPin, _floatSwitchPin);
    //_servoTester.AddDoser(_doserPin, 22000, _floatSwitchPin);
    //_servoTester.AddFeeder(_feederPin1, _feederRunEvery);
    //_servoTester.AddFeeder(_feederPin2, _feederRunEvery);
    //TimeTester _timeTester; //have to init in setup!
    //_timeTester.EraseMemory();
    //_servoTester.AddDoser(_doserPin, 25000, _floatSwitchPin);
    //_servoTester.AddFeeder(_feederPin1, _feederRunEvery);
    //_servoTester.AddFeeder(_feederPin2, _feederRunEvery);
    //_servoTester.RunAll(_feederPin1);
    //_timeTester.SetNoon();
    //_timeTester.NextRunVectorTest(AccessoryType::Feeder, false, 100, false);
    //_timeTester.NextRunVectorTest(AccessoryType::DryDoser, false, 200, false);
    //_timeTester.NextRunVectorTest(AccessoryType::DryDoser, false, 300, false);
    //_timeTester.RunAll();
    //LcdTester lcdTester;
    //lcdTester.ScrollRight();
    //lcdTester.ScrollRight();
    // return;

    _lcdDisplay.Init(); //needs to run first to init rtc.

    _doser = RODoser(_doserServo, _doserPin, 2, 22000, _doserFloatSwitch, _doserEnabled);

    FishFeeder feeder1 = FishFeeder::CreateFeeder(_feederPin1, 2, _feederRunEvery, _feederEnabled);
    _feeders.push_back(feeder1);
    FishFeeder feeder2 = FishFeeder::CreateFeeder(_feederPin2, 2, _feederRunEvery, _feederEnabled);
    _feeders.push_back(feeder2);
    FishFeeder feeder3 = FishFeeder::CreateFeeder(_feederPin3, 2, _feederRunEvery, _feederEnabled);
    _feeders.push_back(feeder3);
    FishFeeder feeder4 = FishFeeder::CreateFeeder(_feederPin4, 2, _feederRunEvery, _feederEnabled);
    _feeders.push_back(feeder4);

    _pump = Pump(_pumpPin, 2, _pumpRelayPin, 604800, _pumpEnabled); //weekly
    //Motors.push_back(_doser);
    //vector<ServoMotor> motors = Motors;

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
    //SerialExt::Debug("key_isp", key);

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
    if(RTCExt::IsAccEnabled(AccessoryType::WaterPump))
        RunPump();
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

    bool runMotor = _doser.ShouldRunMotor(true);

    if(runMotor) {

        //SerialExt::Debug("switchIsOn: ", _doser.TheSwitch.IsOn());
        //SerialExt::Debug("switchVal: ", _doser.TheSwitch.SwitchReading);
        _doser.Run();
    }

}

void RunPump() {

    bool runMotor = _pump.ShouldRunMotor(true);

    if(runMotor) {
        _pump.Run();
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