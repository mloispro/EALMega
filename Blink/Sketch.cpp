/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
#include <StandardCplusplus.h>

#include <vector>
using namespace std;

vector<int> _ints;

int _relayPin = 41;

// the setup function runs once when you press reset or power the board
void setup() {
    // initialize digital pin 13 as an output.
    Serial.begin(57600);
    while(!Serial);

    //led
    pinMode(13, OUTPUT);

    //relay, always put digitalwrite before pinmode to prevent relay engage on startup
    digitalWrite(_relayPin, LOW);
    pinMode(_relayPin, OUTPUT);

}

// the loop function runs over and over again forever
void loop() {

    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(_relayPin, LOW);

    delay(5000);              // wait for 5 second

    digitalWrite(_relayPin, HIGH);
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(8000);              // wait for a second
}
