/*Begining of Auto generated code by Atmel studio */
//#include <avr/io.h>
//#include <_cplusplus.h>
#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
//#include <_SimulateDebug.h>

#include <SerialExt.h>

#include "RTCExt.h"
#include "LCDMenuController.h"

using namespace Utils;
using namespace Controllers;

//LCDMenuController _lcdMenuController;
Servo _servo;// = Servo();
Servo _servo2;

LiquidCrystal _lcd(8, 9, 4, 5, 6, 7);
//LiquidCrystal _lcd;


// the setup function runs once when you press reset or power the board
void setup() {

    Serial.begin(115200);

    while(!Serial);


    _servo.attach(30);
    _servo2.attach(31);
    pinMode(13, OUTPUT);

    //setTime(3, 40, 0, 12, 5, 2016);
    //RTCExt::Init();
    // _lcdMenuController.Init();
    SerialExt::Debug("time", now());
    pinMode(10, INPUT); //*prevent reverse current, dont use pwm. **or cut D10 pin
    _lcd.begin(16, 2);              // start the library
    _lcd.setCursor(0, 0);           // set the LCD cursor   position
    _lcd.print("starting..");

    if(true || false && false) {
        _servo2.attach(4);

    }
    SerialExt::Debug(F("jj"), 5);
}

// the loop function runs over and over again forever
void loop() {

    delay(2000);
    _lcd.clear();
    _lcd.print("servo start");
    digitalWrite(13, HIGH);

    _servo.write(6);
    delay(200);
    _servo2.write(6);
    delay(2000);
    _servo.write(174);
    delay(200);
    _servo2.write(174);
    delay(2000);
    _servo.write(6);
    delay(200);
    _servo2.write(6);
    _lcd.clear();
    _lcd.print("servo end");
    delay(2000);

    digitalWrite(13, LOW);
}
