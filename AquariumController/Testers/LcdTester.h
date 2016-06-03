/*
 * LcdTester.h
 *
 * Created: 6/3/2016 9:45:02 AM
 *  Author: lenovo
 */


#ifndef LCDTESTER_H_
#define LCDTESTER_H_

#include <Arduino.h>

#include <StandardCplusplus.h>
#include <vector>
#include <string>
using namespace std;

#include <Wire.h>
#include <TimeLib.h>
#include <Time.h>
#include <DS3232RTC.h>
#include <LiquidCrystal.h>
#include <SimpleTimer.h>

#include "_globals.h"
using namespace Globals;
#include "SerialExt.h"
#include "LCDMenu.h"
#include "DigitalTime.h"
#include "RTCExt.h"
using namespace Utils;
using namespace TimeHelpers;
using namespace Globals;

namespace Testers {

    class LcdTester {
      private:
        bool _initialized;

        SimpleTimer _scrollLeftTimer;
        SimpleTimer _optionChangeTimer;
      public:
        static LiquidCrystal _lcd;
        static bool _optionChanged;
        static String _menuText;
        static String _optionText;

        void RunAll() {

        }

        //template<typename T = void>
        LcdTester() {
            if(!_initialized) {
                //RTCExt::Init();
                _lcd.begin(16, 2);
                _lcd.clear();
                _initialized = true;
            }
        }
        static void ScrollTextRight(String txt, short line) {
            if(txt.length() > 16) {
                int pos = (txt.length() * -1) - 12;
                for(int i = 0; i < txt.length() - 12; i++) {
                    pos += 1;
                    _lcd.setCursor(0, line);
                    _lcd.print("                 ");
                    _lcd.setCursor(pos, line);
                    _lcd.print(txt);
                    //_lcd.setCursor(0, 0);
                    //_lcd.print(_menuText);
                    delay(400);
                }
                _lcd.setCursor(0, line);
                _lcd.print(txt);
                delay(500);
            }
        }
        static void ScrollText(void) {

            _lcd.setCursor(0, 0);
            _lcd.print(_menuText);
            _lcd.setCursor(0, 1);
            _lcd.print(_optionText);

            ScrollTextRight(_menuText, 0);
            ScrollTextRight(_optionText, 1);
            //if(_menuText.length() > 16) {
            //// _lcd.setCursor(0, 1);
            //// _lcd.autoscroll();
            //int pos = (_menuText.length() * -1) - 12;
            //for(int i = 0; i < _menuText.length() - 12; i++) {
            //pos += 1;
            ////_lcd.setCursor(pos - 1, 0);
            ////_lcd.print(" ");
            //_lcd.setCursor(0, 0);
            //_lcd.print("                 ");
            //_lcd.setCursor(pos, 0);
            //_lcd.print(_menuText);
            ////_lcd.setCursor(0, 1);
            ////_lcd.print(_optionText);
            //delay(200);
            //}
            //_lcd.setCursor(0, 0);
            //_lcd.print(_menuText);
            //}
            //if(_optionText.length() > 16) {
            //int pos = (_optionText.length() * -1) - 12;
            //for(int i = 0; i < _optionText.length() - 12; i++) {
            //pos += 1;
            //_lcd.setCursor(0, 1);
            //_lcd.print("                 ");
            //_lcd.setCursor(pos, 1);
            //_lcd.print(_optionText);
            ////_lcd.setCursor(0, 0);
            ////_lcd.print(_menuText);
            //delay(200);
            //}
            //_lcd.setCursor(0, 1);
            //_lcd.print(_optionText);
            //delay(200);
            //}


        }

        static void ChangeOption(void) {
            _optionChanged = !_optionChanged;
        }

        void ScrollRight() {

            _menuText = F("long menu test so need to scroll");
            _optionText = F("Scrolling Option.");

            //delay(1000);
            int _optionChangeTimerId = _optionChangeTimer.setInterval(10000, ChangeOption);
            int _scrollLeftTimerId = _scrollLeftTimer.setInterval(1000, ScrollText);

            int maxTimers = SimpleTimer::MAX_TIMERS;
            int numAvailTimers = _scrollLeftTimer.getNumAvailableTimers();
            int numTimers = _scrollLeftTimer.getNumTimers();

            // _lcd.home();
            while(!_optionChanged) {
                _optionChangeTimer.run();
                _scrollLeftTimer.run();
            }
            _optionChanged = false;
            _scrollLeftTimer.deleteTimer(_scrollLeftTimerId);
            _optionChangeTimer.deleteTimer(_optionChangeTimerId);

            maxTimers = SimpleTimer::MAX_TIMERS;
            numAvailTimers = _scrollLeftTimer.getNumAvailableTimers();
            numTimers = _scrollLeftTimer.getNumTimers();
            _lcd.home();
        }

    };
    LiquidCrystal Testers::LcdTester::_lcd(8, 9, 4, 5, 6, 7);
    bool Testers::LcdTester::_optionChanged;
    String Testers::LcdTester::_menuText;
    String Testers::LcdTester::_optionText;

}

#endif /* LCDTESTER_H_ */