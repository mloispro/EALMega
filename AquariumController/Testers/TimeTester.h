/*
 * TimeTester.h
 *
 * Created: 5/31/2016 11:37:27 AM
 *  Author: lenovo
 */


#ifndef TIMETESTER_H_
#define TIMETESTER_H_

#include <Arduino.h>

//#include <StandardCplusplus.h>
//#include <string>
//#include <vector>


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
namespace Testers {

    //alert: *(this)
    //alert: (rtcTime,s).buffer

    class TimeTester {
      private:
        bool _initialized;

      public:
        void RunAll() {
            SetNoon();
            SetAMPM(false);
            SetAMPM(true);
            SetTime(11, 02, 15, 28, 5, 2016);
            SetAMPM(true);
            SetAMPM(false);
            SetTime(1, 03, 30, 28, 5, 2016);
            SetAMPM(false);
            SetAMPM(true);
            SetNextRun(5, AccessoryType::Feeder);
            SetNextRun(10, AccessoryType::DryDoser);
            WatchCountDown(AccessoryType::Feeder);
            WatchCountDown(AccessoryType::DryDoser);
        }

        //template<typename T = void>
        TimeTester() {
            if(!_initialized) {
                RTCExt::Init();
                _initialized = true;
            }
        }
        void SetTime(int theHour, int theMinute, int theSecond, int theDay, int theMonth, int theYear) {
            RTCExt::SetRTCTime(theHour, theMinute, theSecond, theDay, theMonth, theYear);
        }

        // template<typename T = void>
        void SetNoon() {
            //String dateTime = RTCExt::GetRTCDateTimeString();
            SetTime(12, 01, 0, 28, 5, 2016);
            //dateTime = RTCExt::GetRTCDateTimeString().c_str();
        }

        //template<typename T>
        void SetAMPM(bool pm) {
            time_t rtcTime = RTCExt::GetRTCTime();
            tmElements_t timeBuffer;

            timeBuffer.Day = day(rtcTime);
            timeBuffer.Month = month(rtcTime);
            timeBuffer.Year = year(rtcTime);
            timeBuffer.Hour = hourFormat12(rtcTime);
            timeBuffer.Minute = minute(rtcTime);
            timeBuffer.Second = second(rtcTime);

            if(!pm) { //if AM
                if(timeBuffer.Hour == 12) //midnight
                    timeBuffer.Hour = 0;
            } else if(pm) { //if PM

                if(timeBuffer.Hour < 12) //pm
                    timeBuffer.Hour += 12;
            }

            RTCExt::SetRTCTime(timeBuffer.Hour, timeBuffer.Minute, timeBuffer.Second, timeBuffer.Day, timeBuffer.Month, timeBuffer.Year);
            String dateTime = RTCExt::GetRTCDateTimeString();
            int buffHour = timeBuffer.Hour;
        }

        void SetNextRun(long seconds, AccessoryType accType) {
            NextRunMemory& nextRunMem = RTCExt::FindNextRunInfo(accType);
            //NextRunMemory& doserNextRunMem = RTCExt::FindNextRunInfo(AccessoryType::DryDoser);
            //RTCExt::SetRunEvery(24, accType);
            nextRunMem.RunEvery = seconds;
            nextRunMem.NextRun = 0; //need to set to 0 so it recalculates
            RTCExt::UpdateNextRun(accType);
        }
        void WatchCountDown(AccessoryType accType) {
            NextRunMemory& nextRunMem = RTCExt::FindNextRunInfo(accType);
            nextRunMem.LastRun = RTCExt::GetRTCTime();
            while(nextRunMem.CountDown > 0) {
                RTCExt::UpdateNextRun(accType);
                NextRunMemory& nextRunMemWatch = RTCExt::FindNextRunInfo(accType);

                //(rtcTime,s).buffer
                auto rtcTime = GetShortDateTimeString(RTCExt::GetRTCTime(), true);
                auto nextRun = GetShortDateTimeString(nextRunMemWatch.NextRun, true);
                auto lastRun = GetShortDateTimeString(nextRunMemWatch.LastRun, true);
                auto countDown = GetTimeRemainingString(nextRunMemWatch.CountDown, true);
                auto runEvery = GetTimeRemainingString(nextRunMemWatch.RunEvery, true);

                delay(2000);
            }
        }
    };

}



#endif /* TIMETESTER_H_ */