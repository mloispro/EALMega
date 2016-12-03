#include <Wire.h>

const byte _slave = 8;
uint8_t _wireReqLength = 20;
uint8_t _wireRespLength = 10;
String _wireRequest = "";
String _wireResponse = "";
int _responseIndex = -1;
String _cmd = "";
String _cmdData = "";

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial);
  
  Wire.begin(_slave); 
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

double _ph = 5.0;
String _phString;
double _phAvg=0;
String _phAvgString;
double _tds = 40.0;
String _tdsString;
double _tdsAvg=0;
String _tdsAvgString;
String _response;

void loop() { // run over and over

  _ph+=.1;

  if(_phAvg==0){
    _phAvg=_ph;
  }else{
    _phAvg=(_phAvg+_ph)/2;
  }
  
 _phString = String(_ph,2);
 _phAvgString = String(_phAvg,2);
 _tds+=1.2;

 if(_tdsAvg==0){
    _tdsAvg=_tds;
  }else{
    _tdsAvg=(_tdsAvg+_tds)/2;
  }
  
 _tdsString = String(_tds,2);
 _tdsAvgString = String(_tdsAvg,2);

 //"16.12,16.12,0.00,75.59,75.59,1210.00;"
  _response = _phString+","+_tdsString+","+_phAvgString+","+_tdsAvgString+";";

//  _response.replace(";","");
//  String phSplit = SplitString(_response, ',', 0);
//  Serial.println("phSplit: " +phSplit);
//  String tdsSplit = SplitString(_response, ',', 1);
//  Serial.println("tdsSplit: " +tdsSplit);
//  String phAvgSplit = SplitString(_response, ',', 2);
//  Serial.println("phAvgSplit: " +phAvgSplit);
  
//  Serial.println("response: " +_response);
//  int semi = _response.indexOf(';');
//  String phSub = _response.substring(0,semi);
//  Serial.println("phSub: " +phSub);
//  String tdsSub = _response.substring(semi+1,_response.length());
//  Serial.println("tdsSub: " +tdsSub);
  delay(1000);
  
}
void receiveEvent(int bytes)
{
  //!!DONT PUT SERIAL.PRINTS IN HERE IT WILL SCREW UP WIRE COMM!!
  _wireRequest="";
  _cmd="";
  _cmdData="";
  if(bytes == _wireReqLength)
  {
    while (0 < Wire.available()) {
      char c = Wire.read();
      _wireRequest.concat(c);
    }
    Serial.println("Wire Request: " + _wireRequest);
    if(_wireRequest.startsWith("/")){
      _cmd = _wireRequest;
       //Serial.println("cmd: " + _cmd);
    }else{
      _cmdData = _wireRequest;
      //Serial.println("cmdData: " + _cmdData);
      String var = SplitString(_cmdData, '=', 0);
      String val = SplitString(_cmdData, '=', 1);
      if(var == "tdsoffset"){
        double offset = val.toFloat();
        Serial.print("offset: ");
        Serial.println(String(offset,2));
      }
    }
  }
  else
  {
    Serial.print(F("Unexpected number of bytes received: "));
    Serial.println(bytes);
  }
}
void requestEvent() {
   //!!DONT PUT SERIAL.PRINTS IN HERE IT WILL SCREW UP WIRE COMM!!
  
  //Serial.println(F("Begin - Transmit Request event"));

  _responseIndex++;
  String partialResponse = "";
  if(_responseIndex==0){
    partialResponse = _phString;
  }
  else if(_responseIndex==1){
    partialResponse = _phAvgString;
    _responseIndex=-1; //need to keep this in last if always
  }
  else{
    Serial.println("~responseIndex not found: " + String(_responseIndex));
    _responseIndex=-1;
    return;
  }
  
//Pad
  int diff = _wireRespLength - partialResponse.length();
  for(int i = 0;i<diff;i++){
    partialResponse+=" ";
  }
  //int respLen = partialResponse.length();
//  Serial.println("resp: " + partialResponse);
//  Serial.print("respLen: ");
//  Serial.println(respLen);

  //const char* pr = partialResponse.c_str();
//  Serial.print("pr: ");
//  Serial.println(pr);
//  Serial.print("prLen: ");
//  Serial.println(String(pr).length());
  byte response[_wireRespLength];
  for (byte i=0;i<_wireRespLength;i++) {
    response[i] = (byte)partialResponse.charAt(i);
  }
  Wire.write(response,sizeof(response));
  //Wire.write(pr, _wireRespLength);
//  byte responseBytes[_wireRespLength];
//  for (byte i=0;i<_wireRespLength;i++) {
//    responseBytes[i] = (byte)partialResponse.charAt(i);
//  }
//  Wire.write(responseBytes,sizeof(responseBytes));
  
  //Serial.println(F("End - Transmit Request event"));
}
String SplitString(String data, char separator, int index)
{
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
