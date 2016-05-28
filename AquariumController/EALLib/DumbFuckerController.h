/*
* DumbFucker.h
*
* Created: 5/27/2016 1:02:33 PM
* Author: lenovo
*/


#ifndef __DDumbFuckerController_H__
#define __DDumbFuckerController_H__
#include <Arduino.h>

//#include <string>
#include <vector>
using namespace std;

#include <Wire.h>
#include <TimeLib.h>
#include <Time.h>
#include <DS3232RTC.h>
#include <LiquidCrystal.h>

#include "SerialExt.h"
#include "LCDMenu.h"
#include "DigitalTime.h"
#include "RTCExt.h"
using namespace Utils;
using namespace TimeHelpers;
#include "DumbFucker.h"

namespace DumbAss {

    class DumbFuckerController {
        //variables
      public:
      protected:
      private:
        LiquidCrystal _lcd;
        vector<LCDMenu> _menus;
        // DumbFucker _dumbFucker;

        //functions
      public:
        DumbFuckerController() : _lcd(8, 9, 4, 5, 6, 7) {};
        // DumbFuckerController(): _dumbFucker(5) {};
        ~DumbFuckerController();
        void Init();
        void Select();
      protected:
      private:
        DumbFuckerController(const DumbFuckerController& c);
        DumbFuckerController& operator=(const DumbFuckerController& c);

    }; //DumbFucker
}
#endif //__DUMBFUCKER_H__
