
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

//SoftwareSerial unoSerial(3, 1); // RX, TX 13, 15

int _ip[4]{192, 168, 10, 161};
const char *hostName = "AquaController-1";

const char *ssid = "One Love";//"One Love";//"SMU_Aruba_WiFi";
const char *password = "teddy1207";//teddy1207";
//const char *_sensorIp = "192.168.10.106";//"192.168.10.160"

//int _sdaPin = 4;
//int _sclPin = 5;

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

//const byte _slave = 8; //has to be greater than 7
//uint8_t _wireReqLength = 20;
//uint8_t _wireRespLength = 10;

String _newLine = "\r\n"; 

String ProcessRequest(String req){
  String response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
  String cmd = "/"+ SplitString(req,'/',1);
  
  if (cmd == "/CheckMicrosDose" ||
      cmd == "/CheckDryDose"||
      cmd == "/CheckWaterChange"||
      cmd == "/CheckMacrosDose")
  {
    response += CheckRunInfo(req);
  }
  else if (cmd == "/SetWaterChange" ||
           cmd == "/SetMicrosDose")
  {
    //..../SetWaterChange/1/90/1209600/0/0
    String enabled = SplitString(req,'/',2);
    String duration = SplitString(req,'/',3);
    String runEvery = SplitString(req,'/',4);
    String runNow = SplitString(req,'/',5);
    String nextRun = SplitString(req,'/',6);
   
    PrintDebug("Parsed enabled: " + enabled);
    PrintDebug("Parsed duration: " + duration);
    PrintDebug("Parsed runEvery: " + runEvery);
    PrintDebug("Parsed runNow: " + runNow);
    PrintDebug("Parsed nextRun: " + nextRun);
    
    response += SetRunInfo(cmd, enabled, duration, runEvery, runNow, nextRun);
  }
  return response;
}

String SetRunInfo(String cmd, String enabled, String duration, String runEvery, String runNow, String nextRun){
  String response="";

  Transmit(cmd);
  delay(40);
  
  //"enabled=1"
  String request = "enabled=" + enabled;
  Transmit(request);
  delay(20);
  request = "dur=" + duration;
  Transmit(request);
  delay(20);
  request = "re=" + runEvery;
  Transmit(request);
  delay(20);
  request = "rNow=" + runNow;
  Transmit(request);
  delay(20);
  request = "nr=" + nextRun;
  Transmit(request);
  delay(20);
  
  response = "{\r\n";
  //response += "\"msg\":\"Water Changed Updated to -> " + enabled + "\"\r\n";
  response += "\"msg\":\"Run Info Updated.\"\r\n";
  response += "}\r\n";
  return response;
}

struct WireRunInfo {
    String AccType;
    String LastRun;
    String NextRun;
    String CountDown;
    String RunEvery;
    String ShakesOrTurns;
    String LastSave;
    String Enabled;
    String RunDurration;
    String Updated;
};

WireRunInfo _wireRunInfo;

String CheckRunInfo(String req){
  String response="";

  Transmit(req);
   
  _wireRunInfo.AccType = Request();
  _wireRunInfo.LastRun = Request();
  _wireRunInfo.NextRun = Request();
  _wireRunInfo.CountDown = Request();
  _wireRunInfo.RunEvery = Request();
  _wireRunInfo.ShakesOrTurns = Request();
  _wireRunInfo.LastSave = Request();
  _wireRunInfo.Enabled = Request();
  _wireRunInfo.RunDurration = Request();
  _wireRunInfo.Updated = Request();
  
  delay(20); //let wire clear.

  //response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
  response = "{\r\n";
  response += "\"host\":\"" + String(hostName) + "\",\r\n";
  response += "\"accType\":" + _wireRunInfo.AccType + ",\r\n";
  response += "\"lastRun\":" + _wireRunInfo.LastRun + ",\r\n";
  response += "\"nextRun\":" + _wireRunInfo.NextRun + ",\r\n";
  response += "\"countDown\":" + _wireRunInfo.CountDown + ",\r\n";
  response += "\"runEvery\":" + _wireRunInfo.RunEvery + ",\r\n";
  response += "\"shakesOrTurns\":" + _wireRunInfo.ShakesOrTurns + ",\r\n";
  response += "\"lastSave\":" + _wireRunInfo.LastSave + ",\r\n";
  response += "\"enabled\":" + _wireRunInfo.Enabled + ",\r\n";
  response += "\"runDurration\":" + _wireRunInfo.RunDurration + ",\r\n";
  response += "\"updated\":" + _wireRunInfo.Updated + "\r\n";
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

  //ensure wire connected every 2 min
  static unsigned long samplingTime = millis();
  if(millis() - samplingTime > 120000) { //check every 2 minute.
      EnsureWireConnected();
      samplingTime = millis();
  }

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
    response += "<p>/CheckDryDose</p>";
    response += "<p>/CheckMicrosDose</p>";
    response += "<p>/SetMicrosDose/1/2/1209600/0/0 (../enabled/duration/runEvery/runNow/nextRun)</p>";
    response += "<p>/CheckWaterChange</p>";
    response += "<p>/SetWaterChange/1/90/1209600/0/1478559049 (../enabled/duration/runEvery/runNow/nextRun)</p>";
    response += "<p>/CheckMacrosDose</p>";
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

