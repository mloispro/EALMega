

#include "DumbFuckerController.h"

using namespace DumbAss;

//DumbFuckerController::DumbFuckerController() {
//} //DumbFucker

// default destructor
//DumbFuckerController::~DumbFuckerController() {
//} //~DumbFucker

void DumbFuckerController::Init() {
    //_dumbFucker.Begin();
    //_lcd.begin(16, 2);
    //LCDMenu menu(1, 1, 1, 1, "", "", LCDMenu::RangeType::Month, AccessoryType::Feeder);
    //_menus.push_back(menu);

    pinMode(10, INPUT);
    RTCExt::Init();
    _lcd.begin(16, 2);
    _lcd.clear();
    _lcd.setCursor(0, 0);
    CreateMenus();

}

//void DumbFuckerController::Select() {
////_dumbFucker.Print(4);
//}

void DumbFuckerController::CreateMenus() {

    const String mainMenuText = F("Menu: [>] Exit");
    const String feedMenuText = F("Feeder: [<] Back");
    const String doserMenuText = F("Doser: [<] Back");
    const String clockMenuText = F("Clock: [<] Back");

    if(_menus.size() > 0)
        _menus.clear();

    /*_lcd.begin();*/
    //menus
    AddMenu(mainMenu, 0, feedMenu, mainMenu, mainMenuText, F("Feeder"), LCDMenu::RangeType::Nav);
    AddMenu(mainMenu, 1, doserMenu, mainMenu, mainMenuText, F("Doser"), LCDMenu::RangeType::Nav);
    AddMenu(mainMenu, 2, clockMenu, mainMenu, mainMenuText, F("Clock"), LCDMenu::RangeType::Nav);

    //feed menus
    AddMenu(feedMenu, 0, feedTimeMenu, mainMenu, feedMenuText, F("Feed Time"), LCDMenu::RangeType::Nav, AccessoryType::Feeder);
    AddMenu(feedMenu, 1, feedFreqMenu, mainMenu, feedMenuText, F("Set Feed Time"), LCDMenu::RangeType::Nav, AccessoryType::Feeder);
    AddMenu(feedMenu, 2, feedShakesMenu, mainMenu, feedMenuText, F("Set Feed Shakes"), LCDMenu::RangeType::Nav, AccessoryType::Feeder);

    //feed time
    AddMenu(feedTimeMenu, 0, feedTimeMenu, feedMenu, F("Feed Time: [<] Back"), F(""), LCDMenu::RangeType::TimeFrequency, AccessoryType::Feeder);
    AddMenu(feedFreqMenu, 0, feedHourMenu, feedMenu, F("Feed Frequency: [<] Back"), F(""), LCDMenu::RangeType::Frequency, AccessoryType::Feeder);
    AddMenu(feedHourMenu, 0, feedMinMenu, feedFreqMenu, F("Feed Hour: [<] Back"), F(""), LCDMenu::RangeType::Hour, AccessoryType::Feeder);
    AddMenu(feedMinMenu, 0, feedAmPmMenu, feedHourMenu, F("Feed Minute: [<] Back"), F(""), LCDMenu::RangeType::Minute, AccessoryType::Feeder);
    AddMenu(feedAmPmMenu, 0, feedTimeMenu, feedMinMenu, F("Feed AM-PM: [<] Back"), F(""), LCDMenu::RangeType::AmPm, AccessoryType::Feeder);

    //feed shakes
    AddMenu(feedShakesMenu, 0, feedShakesMenu, feedMenu, F("Feed Shakes: [<] Back"), F("Not Set"), LCDMenu::RangeType::ShakesOrTurns, AccessoryType::Feeder);
    AddMenu(feedShakesMenu, 1, feedSetShakesMenu, feedShakesMenu, F("Feed Shakes: [<] Back"), F("Set Feed Shakes"), LCDMenu::RangeType::Nav, AccessoryType::Feeder);
    AddMenu(feedSetShakesMenu, 0, feedShakesMenu, feedShakesMenu, F("Set Feed Shakes: [<] Back"), F("Not Set"), LCDMenu::RangeType::SetShakesOrTurns, AccessoryType::Feeder);

    //doser menus
    AddMenu(doserMenu, 0, doserTimeMenu, mainMenu, doserMenuText, F("Doser Time"), LCDMenu::RangeType::Nav, AccessoryType::DryDoser);
    AddMenu(doserMenu, 1, doserFreqMenu, mainMenu, doserMenuText, F("Set Doser Time"), LCDMenu::RangeType::Nav, AccessoryType::DryDoser);
    AddMenu(doserMenu, 2, doserShakesMenu, mainMenu, doserMenuText, F("Set Doser Shakes"), LCDMenu::RangeType::Nav, AccessoryType::DryDoser);

    //dose time
    AddMenu(doserTimeMenu, 0, doserTimeMenu, doserMenu, F("Doser Time: [<] Back"), F(""), LCDMenu::RangeType::TimeFrequency, AccessoryType::DryDoser);
    AddMenu(doserFreqMenu, 0, doserHourMenu, doserMenu, F("Doser Frequency: [<] Back"), F(""), LCDMenu::RangeType::Frequency, AccessoryType::DryDoser);
    AddMenu(doserHourMenu, 0, doserMinMenu, doserFreqMenu, F("Doser Hour: [<] Back"), F(""), LCDMenu::RangeType::Hour, AccessoryType::DryDoser);
    AddMenu(doserMinMenu, 0, doserAmPmMenu, doserHourMenu, F("Doser Minute: [<] Back"), F(""), LCDMenu::RangeType::Minute, AccessoryType::DryDoser);
    AddMenu(doserAmPmMenu, 0, doserTimeMenu, doserMinMenu, F("Doser AM-PM: [<] Back"), F(""), LCDMenu::RangeType::AmPm, AccessoryType::DryDoser);

    //doser shakes
    AddMenu(doserShakesMenu, 0, doserShakesMenu, doserMenu, F("Doser Shakes: [<] Back"), F(""), LCDMenu::RangeType::ShakesOrTurns, AccessoryType::DryDoser);
    AddMenu(doserShakesMenu, 1, doserSetShakesMenu, doserShakesMenu, F("Doser Shakes: [<] Back"), F("Set Doser Shakes"), LCDMenu::RangeType::Nav, AccessoryType::DryDoser);
    AddMenu(doserSetShakesMenu, 0, doserShakesMenu, doserShakesMenu, F("Set Doser Shakes: [<] Back"), F(""), LCDMenu::RangeType::SetShakesOrTurns, AccessoryType::DryDoser);

    //clock menus
    AddMenu(clockMenu, 0, clockMenu, mainMenu, clockMenuText, F("Time"), LCDMenu::RangeType::TimeLong, AccessoryType::Clock);
    AddMenu(clockMenu, 1, clockYearMenu, clockMenu, clockMenuText, F("Set Clock Time"), LCDMenu::RangeType::Nav, AccessoryType::Clock);
    AddMenu(clockYearMenu, 0, clockMonthMenu, clockMenu, F("Clock Year: [<] Back"), F(""), LCDMenu::RangeType::Year, AccessoryType::Clock);
    AddMenu(clockMonthMenu, 0, clockDayMenu, clockYearMenu, F("Clock Month: [<] Back"), F(""), LCDMenu::RangeType::Month, AccessoryType::Clock);
    AddMenu(clockDayMenu, 0, clockHourMenu, clockMonthMenu, F("Clock Day: [<] Back"), F(""), LCDMenu::RangeType::Day, AccessoryType::Clock);
    AddMenu(clockHourMenu, 0, clockMinMenu, clockDayMenu, F("Clock Hour: [<] Back"), F(""), LCDMenu::RangeType::Hour, AccessoryType::Clock);
    AddMenu(clockMinMenu, 0, clockAmPmMenu, clockHourMenu, F("Clock Min: [<] Back"), F(""), LCDMenu::RangeType::Minute, AccessoryType::Clock);
    AddMenu(clockAmPmMenu, 0, clockMenu, clockMinMenu, F("Clock AM-PM: [<] Back"), F(""), LCDMenu::RangeType::AmPm, AccessoryType::Clock);

    //MemoryExt::PrintFreeMemory(F("cm5"));

}
String DumbFuckerController::GetRangeOption(LCDMenu::RangeType rangeType, Globals::AccessoryType accType) {

    if(rangeType == LCDMenu::RangeType::Frequency) {
        LimitRange(0, 1);
        if(_optionCount <= _lowerLimit)
            return F("Daily");
        else if(_optionCount >= _upperLimit)
            return F("Every Other Day");

    } else if(rangeType == LCDMenu::RangeType::Hour) {
        LimitRange(1, 12);
        String hour = GetOptionAsNumber("01", true);
        //SerialExt::Debug("feed hour", hour);
        return hour;
    } else if(rangeType == LCDMenu::RangeType::Minute) {
        LimitRange(0, 59);

        String minute = GetOptionAsNumber(F("01"), true);
        return minute;
    } else if(rangeType == LCDMenu::RangeType::AmPm) {
        LimitRange(0, 1);
        if(_optionCount <= _lowerLimit)
            return F("AM");
        else if(_optionCount >= _upperLimit)
            return F("PM");
    } else if(rangeType == LCDMenu::RangeType::TimeFrequency) {
        //Daily, 08:30AM
        String freq = GetTimeFrequency(accType);
        return freq;
    } else if(rangeType == LCDMenu::RangeType::TimeLong) {
        // 03/04/2016 08:30AM
        String time = GetTimeLong(accType);
        return time;
    } else if(rangeType == LCDMenu::RangeType::Year) {
        LimitRange(2016, 2050);
        String txt = GetOptionAsNumber(F("2016"));

        return txt;
    } else if(rangeType == LCDMenu::RangeType::Month) {
        LimitRange(1, 12);
        String txt = GetOptionAsNumber(F("01"), true);
        return txt;
    } else if(rangeType == LCDMenu::RangeType::Day) {
        LimitRange(1, 31);
        String txt = GetOptionAsNumber(F("01"), true);
        return txt;
    } else if(rangeType == LCDMenu::RangeType::ShakesOrTurns) {
        int shakes = GetShakesOrTurns(accType);
        return String(shakes);
    } else if(rangeType == LCDMenu::RangeType::SetShakesOrTurns) {
        LimitRange(0, 13);

        String txt = GetOptionAsNumber(F("0"));
        return txt;
    }
    //else if (rangeType == LCDMenu::RangeType::OutPin)
    //{
    //	//pin 14-19 are analog pins but can be used for digital output with SoftPWM
    //	//14 cant be used because it is used by lcd A0
    //	LimitRange(2, 19);
    //	if (_optionCount == 14)
    //		_optionCount++;

    //	String txt = String txt = GetOptionAsNumber("2");
    //	return txt;
    //}
    //else if (rangeType == LCDMenu::RangeType::InPin)
    //{
    //	//pin A0 is used by lcd.
    //	LimitRange(1, 5);

    //	String txt = String txt = GetOptionAsNumber("1");
    //	return txt;
    //}
    else
        return "";


}

void DumbFuckerController::SaveRangeOption(LCDMenu::RangeType rangeType, AccessoryType accType) {
    //SerialExt::Debug("save", selectedMenu.OptionText);

    if(rangeType == LCDMenu::RangeType::Frequency &&
            (accType == AccessoryType::Feeder ||
             accType == AccessoryType::DryDoser)) {
        if(_optionCount == 0)  //Daily
            RTCExt::SetRunEvery(24, accType);
        else // ot day
            RTCExt::SetRunEvery(48, accType);
    } else if((rangeType == LCDMenu::RangeType::Hour ||
               rangeType == LCDMenu::RangeType::Minute ||
               rangeType == LCDMenu::RangeType::AmPm) &&
              (accType == AccessoryType::Feeder ||
               accType == AccessoryType::DryDoser)) {
        RTCExt::SetNextRun(_optionCount, rangeType, accType);
    } else if(rangeType == LCDMenu::RangeType::TimeFrequency) {
        //Daily, 08:30AM
    } else if((rangeType == LCDMenu::RangeType::Year ||
               rangeType == LCDMenu::RangeType::Month ||
               rangeType == LCDMenu::RangeType::Day ||
               rangeType == LCDMenu::RangeType::Hour ||
               rangeType == LCDMenu::RangeType::Minute) &&
              (accType == AccessoryType::Clock)) {
        RTCExt::SetTimeTemp(_optionCount, rangeType);
    } else if(rangeType == LCDMenu::RangeType::AmPm &&
              accType == AccessoryType::Clock) {
        RTCExt::SetTimeTemp(_optionCount, rangeType);
        RTCExt::SetRTCTimeFromTemp();
    } else if(rangeType == LCDMenu::RangeType::SetShakesOrTurns &&
              (accType == AccessoryType::Feeder ||
               accType == AccessoryType::DryDoser)) {
        SetShakesOrTurns(accType, _optionCount);
        //_lcdValCallBack(_optionCount);
    }
    //else if (rangeType == LCDMenu::RangeType::OutPin &&
    //	(accType == AccessoryType::Feeder ||
    //	accType == AccessoryType::DryDoser))
    //{
    //	//_lcdValCallBack(_optionCount);
    //}

}
//--Menu functions
void DumbFuckerController::AddMenu(short id, short optionId, short nextMenuId, short prevMenuId, String text, String optionText, LCDMenu::RangeType rangeType, AccessoryType accType) {
    LCDMenu menu(id, optionId, nextMenuId, prevMenuId, text, optionText, rangeType, accType);
    _menus.push_back(menu);
}
void DumbFuckerController::AddMenu(short id, short optionId, short nextMenuId, short prevMenuId, String text, String optionText, LCDMenu::RangeType rangeType) {
    AddMenu(id, optionId, nextMenuId, prevMenuId, text, optionText, rangeType, AccessoryType::None);
}

String DumbFuckerController::GetOptionAsNumber(String defaultNumber) {
    return GetOptionAsNumber(defaultNumber, false);
}

String DumbFuckerController::GetOptionAsNumber(String defaultNumber, bool isTwoDigits) {
    String txt = defaultNumber;
    if(_optionCount <= _upperLimit && _optionCount >= _lowerLimit) {
        txt = String(_optionCount);
        if(_optionCount < 10 && isTwoDigits)
            txt = "0" + txt;
    }
    return txt;
}

//get by ref example
//auto& menu = GetMenu(5);
LCDMenu DumbFuckerController::GetMenu(short id, short optionId) {
    auto selectedMenu = _menus[0];
    for(auto menu : _menus) {
        if(menu.Id == id && menu.OptionId == optionId)
            return menu;
    }

    return selectedMenu;
}
LCDMenu DumbFuckerController::GetSelectedMenu() {
    auto selectedMenu = GetMenu(_selectedMenuId, _selectedOptionId);
    return selectedMenu;
}
void DumbFuckerController::SetSelectedMenu(LCDMenu menu) {
    _selectedMenuId = menu.Id;
    _selectedOptionId = menu.OptionId;

}
void DumbFuckerController::SetShakesOrTurns(AccessoryType accType, short shakesOrTurns) {

    NextRunMemory& mem = RTCExt::FindNextRunInfo(accType);
    mem.ShakesOrTurns = shakesOrTurns;
}
int DumbFuckerController::GetShakesOrTurns(AccessoryType accType) {

    NextRunMemory& mem = RTCExt::FindNextRunInfo(accType);
    int shakes = mem.ShakesOrTurns;
    return shakes;
}

//template<typename T = void>
//void Clear()
//{
//	_lcd.clear();
//}
void DumbFuckerController::SetClockMenu() {
    //SelectMainMenu();

    _selectedMenuId = clockYearMenu;
    _selectedOptionId = 0;
    _optionCount = 0;

    auto setClockMenu = GetSelectedMenu();
    //SerialExt::Debug("_selectedMenuId", _selectedMenuId);

    PrintMenu(setClockMenu);
    delay(_selectDelay);

    while(_selectedMenuId > -1) {
        DetectKeyPress();
        delay(200);
    }
}
void DumbFuckerController::SelectMainMenu() {

    _selectedMenuId = 0;
    _selectedOptionId = 0;

    auto menu = GetSelectedMenu();
    PrintMenu(menu);

    delay(_selectDelay);

    //timeout menu after 15min, 900
    long menuTimeout = RTCExt::GetRTCTime() + 900;
    while(_selectedMenuId > -1) {
        if(RTCExt::GetRTCTime() > menuTimeout)
            ExitMainMenu();

        DetectKeyPress();
        delay(200);
    }
}

void DumbFuckerController::ExitMainMenu() {
    _selectedMenuId = -1;
    _selectedOptionId = -1;
}

void DumbFuckerController::LimitRange(int lower, int upper) {
    _lowerLimit = lower;
    _upperLimit = upper;
    if(_optionCount <= lower) {
        _optionCount = lower; //limit
    } else if(_optionCount >= upper) {
        _optionCount = upper; //limit
    }
}

void DumbFuckerController::NextOption() {

    if(_selectedOptionId >= 0) {
        short nextOptionId = _selectedOptionId + 1;
        auto nextMenu = GetMenu(_selectedMenuId, nextOptionId);
        if(nextMenu.Id == _selectedMenuId && nextMenu.OptionId == nextOptionId)
            _selectedOptionId = nextOptionId;
    }

    auto selectedMenu = GetSelectedMenu();

    _optionCount++;
    PrintMenu(selectedMenu);
}

void DumbFuckerController::PreviousOption() {

    if(_selectedOptionId > 0) {
        short prevOptionId = _selectedOptionId - 1;
        auto prevMenu = GetMenu(_selectedMenuId, prevOptionId);
        if(prevMenu.Id == _selectedMenuId && prevMenu.OptionId == prevOptionId)
            _selectedOptionId = prevOptionId;

    }

    auto selectedMenu = GetSelectedMenu();

    _optionCount--;
    PrintMenu(selectedMenu);
}

void DumbFuckerController::LeftButton() {
    //navigates back
    _optionCount = 0;
    auto selectedMenu = GetSelectedMenu();
    auto prevMenu = GetMenu(selectedMenu.PrevMenuId, 0);
    SetSelectedMenu(prevMenu);
    PrintMenu(prevMenu);
}

void DumbFuckerController::SelectButton() {

    auto selectedMenu = GetSelectedMenu();
    SaveRangeOption(selectedMenu.TheRangeType, selectedMenu.AccType);
    _optionCount = 0;

    auto nextMenu = GetMenu(selectedMenu.NextMenuId, 0);
    SetSelectedMenu(nextMenu);
    PrintMenu(nextMenu);
    delay(_selectDelay);

}

void DumbFuckerController::DetectKeyPress() {
    int key = GetKey();
    //SerialExt::Debug("key_dkp:", key);

    switch(key) {
        case 0: //right
            ExitMainMenu();
            break;
        case 1: //up
            NextOption();
            break;
        case 2: //down
            PreviousOption();
            break;
        case 3: //left
            LeftButton();
            break;
        case 4: //select
            SelectButton();
            break;
        default:
            break;

    }

}

void DumbFuckerController::PrintMenu(LCDMenu menu) {
    /*SerialExt::Debug("menu", menu.Text);
    SerialExt::Debug("option", menu.OptionText);
    */

    //auto menu = GetSelectedMenu();
    //String optionText = menu.OptionText;
    String optionText = menu.OptionText;

    String rangeOptionText = GetRangeOption(menu.TheRangeType, menu.AccType);

    if(rangeOptionText != "") {
        optionText = rangeOptionText;
        //optionText = StaticUtils::ParseString(rangeOptionText);
    }

    _lcd.clear();
    //_lcd.noAutoscroll();
    //if (menu.Text.length() > 16){
    //	_lcd.autoscroll();
    //}
    //_lcd.setCursor(0, 0);
    PrintLine(0, menu.Text);
    PrintLine(1, optionText);
}

void DumbFuckerController::PrintLine(short lineNum, String text) {
    _lcd.setCursor(0, lineNum);
    _lcd.print(text);
}
void DumbFuckerController::PrintTime() {
    _lcd.clear();
    auto time = RTCExt::GetRTCTime();
    //auto theMonth = month(time);

    auto dateTimeString = TimeHelpers::GetShortDateTimeString(time);

    //SerialExt::Debug("dtStringp", dateTimeString);

    PrintLine(0, F("Clock:"));
    PrintLine(1, dateTimeString);

    delay(_scrollDelay);
}

void DumbFuckerController::PrintRunInfo(String label, AccessoryType accType) {
    RTCExt::UpdateNextRun(accType);
    NextRunMemory& nextRunMem = RTCExt::FindNextRunInfo(accType);

    if(nextRunMem.RunEvery <= 0) {
        _lcd.clear();
        PrintLine(0, label);
        PrintLine(1, F("Not Set"));
        delay(_scrollDelay);
        return;
    }

    String lastRun = TimeHelpers::GetShortDateTimeString(nextRunMem.LastRun);
    String countDown = TimeHelpers::GetTimeRemainingString(nextRunMem.CountDown);
    String nextRun = TimeHelpers::GetShortDateTimeString(nextRunMem.NextRun);


    for(int i = 0; i <= 3; i++) {
        _lcd.clear();
        switch(i) {
            case 0:
                PrintLine(0, label + " Last Run:");
                PrintLine(1, lastRun);
                break;
            case 1:
                PrintLine(0, label + " Count Down:");
                PrintLine(1, countDown);
                break;
            case 2:
                PrintLine(0, label + " Next Run:");
                PrintLine(1, nextRun);
                break;
            default:
                break;
        }

        //_lcd.clear();
        delay(_scrollDelay);
        //_lcd.clear();
    }
}
void DumbFuckerController::PrintInstructions() {
    _lcd.clear();
    PrintLine(0, F("Hold [Select]"));
    PrintLine(1, F("for Menu"));
    delay(_scrollDelay);
}

void DumbFuckerController::Scroll() {
    static bool scrolling; //= (_scrollIndex < 3);

    if(scrolling)  //dont scroll if already scrolling
        return;

    scrolling = true;

    while(scrolling) {

        SerialExt::Debug("_scrollIndex", _scrollIndex);
        SerialExt::Debug("scrolling_time_beg", RTCExt::GetRTCTimeString());

        switch(_scrollIndex) {
            case 0:
                PrintInstructions();
                break;
            case 1:
                PrintRunInfo(F("Feed"), AccessoryType::Feeder);
                break;
            case 2:
                PrintRunInfo(F("Dose"), AccessoryType::DryDoser);
                break;
            default:
                PrintTime();
                _scrollIndex = 0;
                scrolling = false;
                return;
        }
        _scrollIndex++;
    }//scrolling
}

String DumbFuckerController::GetTimeLong(AccessoryType accType) {
    long time;

    if(accType == AccessoryType::Clock) {
        time = RTCExt::GetRTCTime();
    }

    String timeString = TimeHelpers::GetShortDateTimeString(time);
    return timeString;

}

String DumbFuckerController::GetTimeFrequency(AccessoryType accType) {
    long runEvery;
    long nextRun;

    NextRunMemory& nextRunMem = RTCExt::FindNextRunInfo(accType);

    runEvery = nextRunMem.RunEvery;
    nextRun = nextRunMem.NextRun;

    if(runEvery <= 0)
        return F("Not Set");

    String freqTime = RTCExt::GetTimeFrequencyString(runEvery, nextRun);
    return freqTime;

}

//--key press
int DumbFuckerController::GetKeyFromVal(unsigned int input) {
    int k;
    for(k = 0; k < _numOfKeys; k++) {
        if(input < _keyValues[k]) {
            return k;
        }
    }
    if(k >= _numOfKeys) k = -1;
    return k;
}

int DumbFuckerController::GetKey() {
    int keyVal = analogRead(0);
    //SerialExt::Debug(F("keyVal"), keyVal);
    int key = GetKeyFromVal(keyVal);
    //SerialExt::Debug(F("key"), key);
    //_lastKey = key;
    return key;
}
