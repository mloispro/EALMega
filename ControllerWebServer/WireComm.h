#ifndef H_WIRECOMM
#define H_WIRECOMM

#include <Arduino.h> 

int _sdaPin = 4;
int _sclPin = 5;

const byte _slave = 8; //has to be greater than 7
uint8_t _wireReqLength = 20;
uint8_t _wireRespLength = 10;

void Transmit(String request) {
  int diff = _wireReqLength - request.length();
  for(int i = 0;i<diff;i++){
    request+=" ";
  }
  PrintDebug("req",request);
  PrintDebug("reqLen",request.length());

  byte response[_wireReqLength];
  for (byte i=0;i<_wireReqLength;i++) {
    response[i] = (byte)request.charAt(i);
  }
  
  Wire.beginTransmission(_slave); // transmit to device #8
  //Wire.write(request.c_str());        // sends 10 byte
  Wire.write(response,sizeof(response));
  Wire.endTransmission(_slave);    // stop transmitting
  delay(10); //this delay is for request.
}

String Request(){
  String wireResponse;
  Wire.requestFrom(_slave,_wireRespLength); //32 bytes is max, so call multiple
  delay(20); //may need to lower this to 10 if having timeouts
  while (Wire.available() > 0) {
      char p = (char)Wire.read();
      //delay(10);
      //Serial.println(p);

      wireResponse.concat(p);
  }

 // wireResponse.replace('\0',' ');
  wireResponse.trim();
  
  PrintDebug("wireResponse",String(wireResponse));
  return wireResponse;
}

String EnsureWireConnected(){
  Transmit("/");
  String response = Request();
  if(response.length()>0){
    String msg = "Wire connection is Ok";
    PrintDebug(msg);
    return msg;
  }else{
    String msg = "Wire connection failed";
    PrintDebug(msg);
    return msg;
//    int busCleared = I2C_ClearBus();
//    while(busCleared!=0){
//      busCleared = I2C_ClearBus();
//    }
    //ScanI2C();
////    Wire.begin(_sdaPin, _sclPin);
////    Wire.setClockStretchLimit(15000);
    //PrintDebug("Wire cleared successfuly");
    //PrintDebug("Wire response failed, restarting..");
    //ESP.reset(); // hard reset
    //ESP.restart(); //boot loader
    
  }
  
}

String ScanI2C(){
  String response="";
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    //WDT.alive();
    if (error == 0)
    {
      String found = "I2C device found at pin: ";
      Serial.print(found);
      found+=String(address,HEX);
      Serial.print(address,HEX);
      found+= "\r\n";
      response += found;
 
      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("pin: ");
      Serial.println(address);
      String error = "Unknow error at address ";
      Serial.print(error);
      error+=String(address,HEX);
      Serial.print(address,HEX);
      error+= "\r\n";
      response += error;
    }    
  }
  if (nDevices == 0){
    String notFound = "No I2C devices found";
    response = notFound+"\r\n";
    Serial.println(notFound+"\n");
  }else{
    Serial.println("done\n");
  }

  return response;
}
/**
 * This routine turns off the I2C bus and clears it
 * on return SCA and SCL pins are tri-state inputs.
 * You need to call Wire.begin() after this to re-enable I2C
 * This routine does NOT use the Wire library at all.
 *
 * returns 0 if bus cleared
 *         1 if SCL held low.
 *         2 if SDA held low by slave clock stretch for > 2sec
 *         3 if SDA held low after 20 clocks.
 */
int I2C_ClearBus() {
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}
String ClearCom(){
  String msg="";
  int rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
  if (rtn != 0) {
    msg="I2C bus error. Could not clear";
    if (rtn == 1) {
      msg="SCL clock line held low";
    } else if (rtn == 2) {
      msg="SCL clock line held low by slave clock stretch";
    } else if (rtn == 3) {
      msg="SDA data line held low";
    }
  } else { // bus clear
    // re-enable Wire
    // now can start Wire Arduino master
    msg="No problem with Com";
    Wire.begin(_sdaPin, _sclPin);
  }
  return msg;
}

#endif
