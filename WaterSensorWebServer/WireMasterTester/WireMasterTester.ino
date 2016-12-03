
#include <Wire.h>

const char *hostName = "WaterSensor-1";

int _sdaPin = 4;
int _sclPin = 5;

const byte _slave = 8;
uint8_t _wireReqLength = 20;
uint8_t _wireRespLength = 10;

String ProcessRequest(String req){
  String response = "";
  if (req == "/GetSensorVals")
  {
    response = GetSensorVals();
  }
  else if (req.indexOf("/SetTDSOffset") != -1)
  {
    //..../SetTDSOffset/1210
    String offset = SplitString(req,'/',2);
   
    PrintDebug("Parsed TDS Offset: " + offset);
    
    //"tdsoffset=1210"
    response = SetTDSOffset(offset);
  }
   else if (req.indexOf("/SetPHOffset") != -1)
  {
    //..../SetPHOffset/3.1
    String offset = SplitString(req,'/',2);
   
    PrintDebug("Parsed PH Offset: " + offset);
    
    //"phoffset=1210"
    response = SetPHOffset(offset);
  }
  return response;
}

String SetPHOffset(String val){
  String response="";

  Transmit("/SetPHOffset");
  //"phoffset=1210"
  String request = "phoffset="+val;
  Transmit(request);
  delay(20);
  
  response = GetSensorVals();
  response = "PH Offset Updated to: " + val + "\"\r\n" + response;
  return response;
}

String SetTDSOffset(String val){
  String response="";

  Transmit("/SetTDSOffset");
  //"tdsoffset=1210"
  String request = "tdsoffset="+val;
  Transmit(request);
  delay(20);
  
  response = GetSensorVals();
  response = "TDS Offset Updated to: " + val + "\"\r\n" + response;
  return response;
}

String GetSensorVals(){
  String response="";
  
  String ph="";
  String phAvg="";
  String tds="";
  String tdsAvg="";
  String phOffset="";
  String tdsOffset="";

  Transmit("/GetSensorVals");
   
  ph = Request();
  phAvg = Request();
  tds = Request();
  tdsAvg = Request();
  phOffset = Request();
  tdsOffset = Request();
  
  delay(20); //let wire clear.
//
  //response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  response = "\"host\":\"" + String(hostName) + "\"\r\n";
  response += "\"ph\":\"" + String(ph)+ "\"\r\n";
  response += "\"phAvg\":\"" + String(phAvg)+ "\"\r\n";
  response += "\"tds\":\"" + String(tds)+ "\"\r\n";
  response += "\"tdsAvg\":\"" + String(tdsAvg)+ "\"\r\n";
  response += "\"phOffset\":\"" + String(phOffset)+ "\"\r\n";
  response += "\"tdsOffset\":\"" + String(tdsOffset)+ "\"\r\n";
  
  return response;
  
}

void setup(void)
{  
  Wire.begin(_slave);
//  Wire.begin(_sdaPin, _sclPin);
//  Wire.setClockStretchLimit(15000);

  Serial.begin(9600);
  while(!Serial);

 Serial.println("Master Wire Tester for Esp8266");
 Serial.println("~Examples~");
 Serial.println("/GetSensorVals");
 Serial.println("/SetPHOffset/3.1");
 Serial.println("/SetTDSOffset/1210");
 
}

void loop(void)
{

  String req;
  if (Serial.available() > 0) {
    // get incoming byte:
    req = Serial.readString();
    req.trim();
    // delay 10ms to let the ADC recover:
    delay(10);
    PrintDebug("**" + req);
  }

  delay(1000);
  if(req=="") return;
  
  PrintDebug("");
  PrintDebug("New client");

  PrintDebug("Request: " + req);
  
  String response;
  if (req == "/")
  {
    response ="Landing Page";
  }
  else
  {
    response = ProcessRequest(req);
    
  }

  if(response==""){
    PrintDebug("Sending 404");
  }
  
  PrintDebug(response);
  PrintDebug("Done with client");

}

String SplitString(String data, char separator, int index){

  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void PrintDebug(String msg) {
  Serial.println("~esp~ " + msg);
  Serial.flush();
}
void Transmit(String request) {
  int diff = _wireReqLength - request.length();
  for(int i = 0;i<diff;i++){
    request+=" ";
  }
  PrintDebug("req: " + request);
  PrintDebug("reqLen: " + request.length());

  byte response[_wireReqLength];
  for (byte i=0;i<_wireReqLength;i++) {
    response[i] = (byte)request.charAt(i);
  }
  
  Wire.beginTransmission(_slave); // transmit to device #8
  //Wire.write(request.c_str());        // sends 10 byte
  Wire.write(response,sizeof(response));
  Wire.endTransmission();    // stop transmitting
}

String Request(){
  String wireResponse;
  Wire.requestFrom(_slave,_wireRespLength); //32 bytes is max, so call multiple
  while (Wire.available()) {
      char p = Wire.read();
      
      wireResponse.concat(p);
  }
  //PrintDebug("wireResponse: " +wireResponse);
  wireResponse.trim();
  return wireResponse;
}



