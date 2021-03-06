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
#include "FishFeeder.h"
#include "ServoMotor.h"
#include "Pump.h"
#include "LCDMenu.h"
#include "LCDDisplay.h"
#include "ROPump.h"
#include "LiquidDoser.h"

using namespace Globals;
using namespace Utils;
using namespace LCD;

//**leave doser pins set even tho the tank doesnt have any dosers. The Tank and RO Water share the same LCD menu so need these.
//int _feederPin1 = 30;
//int _feederPin2 = 31;
//int _feederPin3 = 32;
//int _feederPin4 = 33;
int _tankPumpPin = 28;
int _roPumpFloatSwitchPin = A9;//MegaPins::A_9;
int _roPumpPin = 29;
int _highWaterFloatSwitchPin = A10;//MegaPins::A_10;
int _ghDoserPin = 40;
int _microsDoserPin = 41;

//**Change if using a regular low level relay.
bool _isHighLevelRelay = true;


#pragma region FEEDER

//bool _feederEnabled = true;
//int _feederRunEvery = 860;
//vector<FishFeeder> _feeders;

#pragma endregion FEEDER

#pragma region PUMP

bool _tankPumpEnabled = false;
Pump _tankPump;
AnalogSwitch _roPumpFloatSwitch(_roPumpFloatSwitchPin);
ROPump _roPump;
AnalogSwitch _highWaterFloatSwitch(_highWaterFloatSwitchPin, 900);

#pragma endregion PUMP

#pragma region LIQUIDDOSERS

LiquidDoser _ghDoser;
LiquidDoser _microsDoser;

#pragma endregion LIQUIDDOSERS

#pragma region INIT

//LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);
LCDDisplay _lcdDisplay;

bool _timersEnabled = true;
SimpleTimer _selectPressTimer;
SimpleTimer _accTimer;


void IsSelectPressed();
void FeedFish();
void RunDoser();
void AccTick();
void RunTankPump();
void RunROPump();
void RunMicrosDoser();

#pragma endregion INIT

//todo: comment this out
//#include "Testers\TimeTester.h"
//#include "Testers\ServoTester.h"
//#include "Testers\LcdTester.h"
//using namespace Testers;

// the setup function runs once when you press reset or power the board
void setup() {

    Serial.begin(57600);
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

    //FishFeeder feeder1 = FishFeeder::CreateFeeder(_feederPin1, 2, _feederRunEvery, _feederEnabled);
    //_feeders.push_back(feeder1);
    //FishFeeder feeder2 = FishFeeder::CreateFeeder(_feederPin2, 2, _feederRunEvery, _feederEnabled);
    //_feeders.push_back(feeder2);
    //FishFeeder feeder3 = FishFeeder::CreateFeeder(_feederPin3, 2, _feederRunEvery, _feederEnabled);
    //_feeders.push_back(feeder3);
    //FishFeeder feeder4 = FishFeeder::CreateFeeder(_feederPin4, 2, _feederRunEvery, _feederEnabled);
    //_feeders.push_back(feeder4);

    _tankPump = Pump(_tankPumpPin, 2, _tankPumpPin, 604800, _tankPumpEnabled); //weekly
    _roPump = ROPump(_roPumpPin, _roPumpFloatSwitch, _highWaterFloatSwitch, true);
    _ghDoser = LiquidDoser(_ghDoserPin, AccessoryType::MacrosDoser);
    _microsDoser = LiquidDoser(_microsDoserPin, AccessoryType::MicrosDoser);
    if(_isHighLevelRelay) {
        _tankPump.SetRelayHigh(); //doing this because using a high level relay in aquarium controler
        _roPump.SetRelayHigh();
        _microsDoser.SetRelayHigh();
    }

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

    if(RTCExt::IsAccEnabled(AccessoryType::Feeder)) {
        FeedFish();
    }
    if(RTCExt::IsAccEnabled(AccessoryType::DryDoser)) {
        RunDoser();
    }
    if(RTCExt::IsAccEnabled(AccessoryType::WaterPump)) {
        RunTankPump();
    }
    if(RTCExt::IsAccEnabled(AccessoryType::MicrosDoser)) {
        RunMicrosDoser();
    }
    RunROPump();
}
void FeedFish() {
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
}

void RunDoser() {

    //bool runMotor = _doser.ShouldRunMotor(true);
    //
    //if(runMotor) {
    //
    ////SerialExt::Debug("switchIsOn: ", _doser.TheSwitch.IsOn());
    ////SerialExt::Debug("switchVal: ", _doser.TheSwitch.SwitchReading);
    //_doser.Run();
    //}

}

void RunTankPump() {
    SerialExt::Debug(F("Tank Pump"));
    bool runMotor = _tankPump.ShouldRunMotor(true);

    if(runMotor) {
        _tankPump.Run();
    }

}

void RunROPump() {
    SerialExt::Debug(F("RO Pump"));
    bool runMotor = _roPump.IsOkToRun();

    if(runMotor) {
        _roPump.Run();
    }

}
void RunMicrosDoser() {
    SerialExt::Debug(F("Micros Doser"));
    bool runMotor = _microsDoser.ShouldRunMotor(true);

    if(runMotor) {
        _microsDoser.Run();
    }
}



#pragma region OLD_STUFF



#pragma endregion OLD_STUFF