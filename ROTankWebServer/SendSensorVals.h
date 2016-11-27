#ifndef H_SENDSENSORVALS
#define H_SENDSENSORVALS

#include <Arduino.h> 

#include "WireComm.h"

const char *_sensorIp = "192.168.10.160";//"192.168.10.160"

struct SensorVals{
  String Ph;
  String Tds;
  String Reading;
};

SensorVals _sensorVals;
void RequestSensorVals()
{
  Serial.print("connecting to ");
  Serial.println(_sensorIp);
  
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(_sensorIp, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/GetSensorVals";

  Serial.print("Requesting URL: ");
  Serial.print(_sensorIp);
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + _sensorIp + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 10000) { //time out after 10 sec
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  String response="";
  bool httpBody = false;
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line);

    if (!httpBody && line.charAt(1) == '{') {
      httpBody = true;
    }
    if(httpBody){
      response += line;
    }  
  }
  
  Serial.println();
  Serial.println("closing connection");
  Serial.println();
  
  //response.trim();
  Serial.println("Json String:");
  Serial.println(response);

  StaticJsonBuffer<600> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);
  const char* phVal = root["ph"];
  const char* tdsVal = root["tds"];
  const char* readingVal = root["reading"];
//  Serial.print("phVal: ");
//  Serial.println(phVal);

  _sensorVals.Ph = phVal;
  _sensorVals.Tds = tdsVal;
  _sensorVals.Reading = readingVal;
  
//  if (!root.success()) {
//    Serial.println("JSON parsing failed!");
//    return;
//  }

  Serial.println("Received Sensor Vals: ");
  Serial.println(_sensorVals.Ph);
  Serial.println(_sensorVals.Tds);
  Serial.println(_sensorVals.Reading);
  
}

//sends sensor vals from esp to arduino
bool TransmitSensorVals(){
  String response="";

  Transmit("/SendSensorVals");
  //"phoffset=1210"
  String request = "ph=" + _sensorVals.Ph;
  Transmit(request);
  delay(10);
  request = "tds=" + _sensorVals.Tds;
  Transmit(request);
  delay(10);
  request = "reading=" + _sensorVals.Reading;
  Transmit(request);
  delay(10);

  response = Request();
  if(response.length()<=0){
    return false;
  }
  
//
//  response = "{\r\n";
//  response += "\"msg\":\"PH Offset Updated to -> " + val + "\"\r\n";
//  response += "}\r\n";
  
  return true;
}

void CheckSensors(){
    static unsigned long checkSensorTime = millis();
  if(millis() - checkSensorTime > 20000) { //check every 15 min.
      RequestSensorVals();
      bool sent = TransmitSensorVals();
      int sendCount = 0;
      while(!sent && sendCount < 5){
        Serial.println("SendSensorVals failed, retrying..");
        sent = TransmitSensorVals();
        sendCount++;
        delay(1000);
      }
      checkSensorTime = millis();
  }
}

#endif
