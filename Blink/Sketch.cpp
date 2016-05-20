/*Begining of Auto generated code by Atmel studio */
//#include <avr/io.h>
//#include <_cplusplus.h>
#include <Arduino.h>
//#include <_SimulateDebug.h>

#include <SerialExt.h>

#include "RTCExt.h"
//#include <LCDMenuController.h>

using namespace Utils;
//using namespace Controllers;

//LCDMenuController _lcdMenuController;


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	while(!Serial);
  
  pinMode(13, OUTPUT);
 

  //setTime(3, 40, 0, 12, 5, 2016);
  //RTCExt::Init();
	//_lcdMenuController.Init();
  SerialExt::Debug("time",now());
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(2000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(2000);              // wait for a second
}
