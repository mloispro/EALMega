

#ifndef SERVOTESTER_H_
#define SERVOTESTER_H_
#include <Arduino.h>

#include <StandardCplusplus.h>
#include <string>
#include <vector>


#include <Wire.h>
#include <TimeLib.h>
#include <Time.h>
#include <DS3232RTC.h>


//#include "EEPRomMem.h"
//#include <EEPROM.h>

#include "MemoryExt.h"
#include "SerialExt.h"
#include "DigitalTime.h"
#include "RTCExt.h"
#include "MemoryExt.h"
#include "LCDMenu.h"
//#include "Containers.h"

using namespace std;

using namespace Utils;
//using namespace Containers;
using namespace Models;
using namespace TimeHelpers;

vector<FishFeeder> _fishFeeders;

namespace Testers {

    class ServoTester {
      private:
        bool _initialized;
        bool _startEnabled;
      public:
        void RunAll(int pin) {
            AddFeeder(pin, 15);
            FeedFish(2);
            TestServo(pin);
        }

        //template<typename T = void>
        ServoTester() {
            if(!_initialized) {
                RTCExt::Init();
                _initialized = true;
            }
        }
        void preRun(long runEvery, AccessoryType accType) {
            NextRunMemory& mem = RTCExt::RefreshNextRunInfo(accType);
            _startEnabled = mem.Enabled;
            mem.Enabled = true;
            RTCExt::RefreshNextRunInfo(accType, true);

            RTCExt::SetRunEverySeconds(runEvery, accType);

            long delayTime = runEvery * 1000 + 1000;
            delay(delayTime);
        }
        void postRun(AccessoryType accType) {
            NextRunMemory& mem = RTCExt::RefreshNextRunInfo(accType);
            mem.Enabled = _startEnabled;
            RTCExt::RefreshNextRunInfo(accType, true);
        }
        void AddFeeder(int pin, int runEvery) {

            FishFeeder feeder = FishFeeder::CreateFeeder(pin, 2, runEvery, true);
            AccessoryType accType = feeder.MotorType;
            //Motors.push_back(feeder);
            _fishFeeders.push_back(feeder);
        }

        void FeedFish(int runs) {
            preRun(0, AccessoryType::Feeder);
            int runCount = 0;
            while(runCount < runs) {
                FishFeeder feeder = _fishFeeders[0];
                AccessoryType accType = feeder.MotorType;
                bool runMotor = feeder.ShouldRunMotor(true);
                if(runMotor) {
                    FishFeeder::FeedAll(_fishFeeders);
                    runCount++;
                }
                delay(1000);

            }
            postRun(AccessoryType::Feeder);
        }

        void AddDoser(int pin, int runEvery, AnalogSwitch floatSwitch) {
            Servo aServo;
            RODoser doser = RODoser(aServo, pin, 2, runEvery, floatSwitch, true);
            //
            //RODoser& doser1 = Containers::AddMotor(doser);
            //RODoser& doser2 = Containers::FindMotor(AccessoryType::DryDoser);
            //int pinr = doser2.RelayPin;
        }

        void RunDoser(int pin, long runEvery, short relayPin, AnalogSwitch floatSwitch) {
            preRun(runEvery, AccessoryType::DryDoser);

            Servo aServo;
            RODoser doser = RODoser(aServo, pin, 2, relayPin, runEvery, floatSwitch, true);

            bool isTimeToRun = doser.ShouldRunMotor(false);

            if(isTimeToRun) {
                doser.Run();
            }
            postRun(AccessoryType::DryDoser);

        }

        void RunPump(int pin, long runEvery, short relayPin, AnalogSwitch floatSwitch) {
            preRun(runEvery, AccessoryType::WaterPump);

            Pump pump = Pump(pin, 2, relayPin, runEvery, floatSwitch, true);

            bool isTimeToRun = pump.ShouldRunMotor(false);

            if(isTimeToRun) {
                pump.Run();
            }
            postRun(AccessoryType::WaterPump);

        }

        void TestServo(int pin) {
            Servo aServo;
            aServo.attach(pin);
            aServo.write(6);
            delay(2000);
            aServo.write(174);
            delay(2000);
            aServo.write(6);
            delay(2000);
        }
    };

}



#endif /* SERVOTESTER_H_ */