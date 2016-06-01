

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
#include "LCDMenu.h"

using namespace std;

using namespace Utils;

using namespace Models;
using namespace TimeHelpers;

vector<FishFeeder> _fishFeeders;

namespace Testers {

    class ServoTester {
      private:
        bool _initialized;

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
        void AddFeeder(int pin, int runEvery) {
            FishFeeder feeder = FishFeeder::CreateFeeder(pin, 2, runEvery);
            _fishFeeders.push_back(feeder);
        }

        void FeedFish(int runs) {

            int runCount = 0;
            while(runCount < runs) {
                FishFeeder feeder = _fishFeeders[0];
                bool runMotor = feeder.ShouldRunMotor(true);
                if(runMotor) {
                    FishFeeder::FeedAll(_fishFeeders);
                    runCount++;
                }
                delay(1000);

            }
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