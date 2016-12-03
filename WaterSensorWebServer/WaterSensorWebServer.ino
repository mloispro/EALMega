//todo: implement watchdog
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <Wire.h>
//#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#include "Utils.h"
#include "WifiUtils.h"
#include "WireComm.h"
#include "SendSensorVals.h"

extern "C" {
#include "user_interface.h"
}

//SoftwareSerial unoSerial(3, 1); // RX, TX 13, 15

//**change these 2
int _ip[4]{192, 168, 10, 160}; 
const char *hostName = "WaterSensor-2";//"ROController-1";

const char *ssid = "One Love";//"One Love";//"SMU_Aruba_WiFi";
const char *password = "teddy1207";//teddy1207";

//esp-01 is 0,2
//int _sdaPin = 0;//0//4
//int _sclPin = 2;//2//5

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

//const byte _slave = 8; //has to be greater than 7
//uint8_t _wireReqLength = 20;
//uint8_t _wireRespLength = 10;

String _newLine = "\r\n"; 

String ProcessRequest(String req){
  String response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
  String cmd = "/"+ SplitString(req,'/',1);

  if (cmd == "/GetSensorVals")
  {
    response += GetSensorVals(req);
  }
  else if (cmd == "/SetSensorInfo")
  {
    //..../SetSensorInfo/3.1/2/3.5/50/2
    String phOffset = SplitString(req,'/',2);
    String tdsOffset = SplitString(req,'/',3);
    String tdsVolts = SplitString(req,'/',4);
    String tdsMin = SplitString(req,'/',5);
    String doseDurr = SplitString(req,'/',6);
    //String doseNow = SplitString(req,'/',7);
   
    PrintDebug("Parsed phOffset: " + phOffset);
    PrintDebug("Parsed tdsOffset: " + tdsOffset);
    PrintDebug("Parsed tdsVolts: " + tdsVolts);
    PrintDebug("Parsed tdsMin: " + tdsMin);
    PrintDebug("Parsed doseDurr: " + doseDurr);
    
    response += SetSensorInfo(cmd, phOffset, tdsOffset, tdsVolts, tdsMin, doseDurr);
  }

  return response;
}

String SetSensorInfo(String cmd, String phOffset, String tdsOffset, String tdsVolts, String tdsMin, String doseDurr){
  String response="";

  Transmit(cmd);
  delay(40);

  String request = "phoffset="+phOffset;
  Transmit(request);
  delay(20);

  //"tdsoffset=3"
  request = "tdsoffset="+tdsOffset;
  Transmit(request);
  delay(20);

    //"tdsVolts=3.5"
  request = "tdsVolts="+tdsVolts;
  Transmit(request);
  delay(20);

  request = "tdsMin="+tdsMin;
  Transmit(request);
  delay(20);

  request = "doseDurr="+doseDurr;
  Transmit(request);
  delay(20);
  
  response = "{\r\n";
  response += "\"msg\":\"Sensor Info Updated.\"\r\n";
  response += "}\r\n";
  return response;
}


struct WireSensorInfo {
  String ph;
  String tds;
  String phOffset;
  String tdsOffset;
  String reading;
  String readingDur;
  String readingInter;
  String tdsVolts;
  String timeSinceLastDose;
  String tdsMin;
  String doseDurration;
};

WireSensorInfo _wireSensorInfo;

String GetSensorVals(String req){
  String response="";
  
  Transmit(req);
   
  _wireSensorInfo.ph = Request();
  _wireSensorInfo.tds = Request();
  _wireSensorInfo.phOffset = Request();
  _wireSensorInfo.tdsOffset = Request();
  _wireSensorInfo.reading = Request();
  _wireSensorInfo.readingDur = Request();
  _wireSensorInfo.readingInter = Request();
  _wireSensorInfo.tdsVolts = Request();
  _wireSensorInfo.timeSinceLastDose = Request();
  _wireSensorInfo.tdsMin = Request();
  _wireSensorInfo.doseDurration = Request();
  
  delay(20); //let wire clear.

  response = "{\r\n";
  response += "\"host\":\"" + String(hostName) + "\",\r\n";
  response += "\"ph\":" + _wireSensorInfo.ph+ ",\r\n";
  response += "\"tds\":" + _wireSensorInfo.tds+ ",\r\n";
  response += "\"phOffset\":" + _wireSensorInfo.phOffset+ ",\r\n";
  response += "\"tdsOffset\":" + _wireSensorInfo.tdsOffset+ ",\r\n";
  response += "\"reading\":\"" + _wireSensorInfo.reading+ "\",\r\n";
  response += "\"readingDur\":\"" + _wireSensorInfo.readingDur+ "\",\r\n";
  response += "\"readingInter\":\"" + _wireSensorInfo.readingInter+ "\",\r\n";
  response += "\"tdsVolts\":\"" + _wireSensorInfo.tdsVolts+ "\",\r\n";
  response += "\"timeSinceLastDose\":\"" + _wireSensorInfo.timeSinceLastDose+ "\",\r\n";
  response += "\"tdsMin\":\"" + _wireSensorInfo.tdsMin+ "\",\r\n";
  response += "\"doseDurr\":\"" + _wireSensorInfo.doseDurration+ "\"\r\n";
  response += "}\r\n";
  
  return response;
  
}


void setup(void)
{  
  //unoSerial.begin(9600);
  Serial.begin(57600);
  EEPROM.begin(512);
  delay(10);
  while(!Serial);

  IPAddress ip = SetIp(_ip[0], _ip[1], _ip[2], _ip[3]);

    // Connect to WiFi network
  WiFi.hostname(hostName);
  //setMac();
  WiFi.begin(ssid, password);
  //WiFi.hostname(hostName);
  
  
  PrintDebug("");  
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    PrintDebug(".");
    delay(10);
  }
  PrintDebug("");
  PrintDebug("Connected to",String(ssid));
  IPAddress localIp = WiFi.localIP();
  String ipAddress = localIp.toString();
  PrintDebug("IP address",ipAddress);

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  //if (!MDNS.begin(hostName)) {
   if (!MDNS.begin(hostName,localIp)) {
    PrintDebug("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  PrintDebug("mDNS responder started");
  
  // Start TCP (HTTP) server
  server.begin();
  PrintDebug("TCP server started");
  
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

  Wire.begin(_sdaPin, _sclPin);
  //Wire.pins(_sdaPin, _sclPin);
  //Wire.setClockStretchLimit(1500);
  Wire.setClockStretchLimit(15000);
  
  //EnsureWireConnected(); //dont do this here it messes up wire.

   delay(100);
}


void loop(void)
{
  EnsureComm();

 //CheckSensors();
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  PrintDebug("");
  PrintDebug("New client");

  // Wait for data from client to become available
  while(client.connected() && !client.available()){
    delay(1);
  }
  
  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');
  
  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    PrintDebug("Invalid request",req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  PrintDebug("Request",req);
  client.flush();
  
  String response;
  if (req == "/")
  {
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from " + String(hostName) + " at ";
    response += ipStr;
    response += "<p>/GetSensorVals</p>";
//    response += "<p>/SetPHOffset/3.1</p>";
//    response += "<p>/SetTDSOffset/2</p>";
//    response += "<p>/SetTDSVolts/3.5</p>";
    response += "<p>/SetSensorInfo/phOffset/tdsOffset/tdsVolts/tdsMin/doseDurr</p>";
    response += "<p>/SetSensorInfo/3.1/2/3.5/50/2</p>";
    response += "<p>/SetSensorInfo/-/2/3.5/-/- (\'-\' skips setting val)</p>";
    response += "<p>/scan</p>";
    response += "<p>/checkcom</p>";
    response += "<p>/clearcom</p>";
    response += "<p>/bootload</p>";
    response += "</html>\r\n\r\n";
    
    PrintDebug("Sending 200");
  }
  else if (req == "/bootload")
  {
    response = "Resetting ESP8266..";
    //EnsureWireConnected();
    //ESP.reset();
    ESP.restart();
    wdt_reset();
  }
  else if (req == "/scan")
  {
    response = ScanI2C();
   
  }
  else if (req == "/checkcom")
  {
    
    response = EnsureWireConnected();
   
  }
  else if (req == "/clearcom")
  {

    response = ClearCom();
   
  }
  else
  {
    response = ProcessRequest(req);
  }

  if(response==""){
    response = "HTTP/1.1 404 Not Found\r\n\r\n";
    PrintDebug("Sending 404");
  }

  client.print(response);
  //response = ""; //needed to clear serial buffer.
  PrintDebug("Done with client");
  client.flush();
  delay(10);
}

