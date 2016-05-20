/*Begining of Auto generated code by Atmel studio */
//#include <avr/io.h>
//#include <_cplusplus.h>
#include <Arduino.h>
#include <Servo.h>
//#include <_SimulateDebug.h>

#include <SerialExt.h>

#include "RTCExt.h"
#include "LCDMenuController.h"

using namespace Utils;
using namespace Controllers;

//LCDMenuController _lcdMenuController;
Servo myservo;// = Servo();
int pos = 0;    // variable to store the servo position

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	while(!Serial);
  myservo.attach(MegaPins::PWM_45);
  pinMode(13, OUTPUT);

  //setTime(3, 40, 0, 12, 5, 2016);
  //RTCExt::Init();
// _lcdMenuController.Init();
  SerialExt::Debug("time",now());
}

// the loop function runs over and over again forever
void loop() {
  //digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(2000);              // wait for a second
  //digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  //delay(2000);              // wait for a second
    //for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
	    //// in steps of 1 degree
	    //myservo.write(pos);              // tell servo to go to position in variable 'pos'
	    //delay(15);                       // waits 15ms for the servo to reach the position
    //}
    //for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
	    //myservo.write(pos);              // tell servo to go to position in variable 'pos'
	    //delay(15);                       // waits 15ms for the servo to reach the position
    //}
	myservo.write(6);              // tell servo to go to position in variable 'pos'
	delay(8000);
	myservo.write(174);              // tell servo to go to position in variable 'pos'
	delay(8000);
	myservo.write(6); 
	delay(8000);
}
