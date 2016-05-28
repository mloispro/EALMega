

#include "DumbFuckerController.h"
using namespace DumbAss;

//DumbFuckerController::DumbFuckerController() {
//} //DumbFucker

// default destructor
DumbFuckerController::~DumbFuckerController() {
} //~DumbFucker

void DumbFuckerController::Init() {
    //_dumbFucker.Begin();
    _lcd.begin(16, 2);
    LCDMenu menu(1, 1, 1, 1, "", "", LCDMenu::RangeType::Month, AccessoryType::Feeder);
    _menus.push_back(menu);
}

void DumbFuckerController::Select() {
    //_dumbFucker.Print(4);
}
