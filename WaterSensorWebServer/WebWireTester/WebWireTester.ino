
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <Wire.h>
//#include <SoftwareSerial.h>
//#include <EasyTransfer.h>
//
//EasyTransfer _comIn, _comOut; 

//SoftwareSerial unoSerial(3, 1); // RX, TX 13, 15

//#define OLED_RESET 0
//#define SDA 4 // I2C Bus SDA (data)
//#define SCL 5 // I2C Bus SCL (clock)

int _sdaPin = 4;
int _sclPin = 5;

const char *ssid = "One Love";//"One Love";//"SMU_Aruba_WiFi";
const char *password = "teddy1207";//teddy1207";
const char *hostName = "WaterSensor-1";

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

const byte _slave = 8;
uint8_t _wireReqLength = 20;
uint8_t _wireRespLength = 10;

String _newLine = "\r\n"; 

//struct ComInData{
//  String InData;
//};
//struct ComOutData{
//  String OutData;
//};
//
//ComInData _comInData;
//ComOutData _comOutData;

void setup(void)
{  
//  unoSerial.begin(9600);
//  while(!unoSerial);
  Serial.begin(115200);
  while(!Serial);

//  _comIn.begin(details(_comInData), &Serial);
//  _comOut.begin(details(_comOutData), &Serial);

  Wire.begin(_sdaPin, _sclPin);
  Wire.setClockStretchLimit(15000);



//  IPAddress ip(192, 168, 10, 102); 
//  IPAddress gateway(192, 168, 10, 1); 
//  IPAddress subnet(255, 255, 255, 0); 
//  Serial.print(F("Setting static ip to : "));
//  Serial.println(ip);
//  WiFi.config(ip, gateway, subnet);

    // Connect to WiFi network
  WiFi.hostname(hostName);
  WiFi.begin(ssid, password);
  WiFi.hostname(hostName);
  
  PrintDebug("");  
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    PrintDebug(".");
    delay(10);
  }
  PrintDebug("");
  PrintDebug("Connected to: " + String(ssid));
  String ipAddress = WiFi.localIP().toString();
  PrintDebug("IP address: " + ipAddress);

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  //if (!MDNS.begin(hostName,ip)) {
   if (!MDNS.begin(hostName)) {
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

  //EnsureWireConnected(); //dont do this here it messes up wire.
}

void loop(void)
{

  //ensure wire connected every 2 min
  static unsigned long samplingTime = millis();
  if(millis() - samplingTime > 120000) { //check every 2 minute.
      EnsureWireConnected();
      samplingTime = millis();
  }
  
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
    PrintDebug("Invalid request: " + req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  PrintDebug("Request: " + req);
  //unoSerial.println(req);
  client.flush();
  
  String response;
  if (req == "/")
  {
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from " + String(hostName) + " at ";
    response += ipStr;
    response += "<p>/scan</p>";
    response += "<p>/bootload</p>";
    response += "</html>\r\n\r\n";
    
    PrintDebug("Sending 200");
  }
  else if (req == "/scan")
  {
    ScanI2C();
   
  }
  else if (req == "/reboot")
  {
    ESP.reset();
   
  }

  
//  else
//  {
//    response = ProcessRequest(req);
//  }
  if(response==""){
    response = "HTTP/1.1 404 Not Found\r\n\r\n";
    PrintDebug("Sending 404");
  }

//  PrintDebug("Sending req..");
//  //unoSerial.println("Sending req..");
//  delay(1000);
//  _comOutData.OutData = req;
//  _comOut.sendData();
//  delay(1000);
//
//  for(int i=0; i<5; i++){
//    //remember, you could use an if() here to check for new data, this time it's not needed.
//    if(_comIn.receiveData()){
//    
//      response = _comInData.InData;
//      PrintDebug("Receiving resp: " + response);
//      delay(100);
//    }
//    delay(100);
//  }



  client.print(response);
  response = ""; //needed to clear serial buffer.
  PrintDebug("Done with client");
  
  delay(10);

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
  Wire.endTransmission(_slave);    // stop transmitting
}

String Request(){
  String wireResponse="";
  String resp="";
  Wire.requestFrom(_slave,_wireRespLength); //32 bytes is max, so call multiple
  while (Wire.available()) {
      char p = Wire.read();
      
      wireResponse.concat(p);
  }
  resp = wireResponse;
  wireResponse="";
  PrintDebug("wireResponse: " +resp);
  resp.trim();
  return resp;
}

void EnsureWireConnected(){
  Transmit("/");
  String response = Request();
  if(response.length()>0){
    PrintDebug("Wire connection is Ok");
  }else{
    PrintDebug("Wire connection failed");
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
void ScanI2C(){
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}
//
//int I2C_ClearBus() {
//#if defined(TWCR) && defined(TWEN)
//  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
//#endif
//
//  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
//  pinMode(SCL, INPUT_PULLUP);
//
//  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
//  // up of the DS3231 module to allow it to initialize properly,
//  // but is also assists in reliable programming of FioV3 boards as it gives the
//  // IDE a chance to start uploaded the program
//  // before existing sketch confuses the IDE by sending Serial data.
//
//  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
//  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
//    return 1; //I2C bus error. Could not clear SCL clock line held low
//  }
//
//  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
//  int clockCount = 20; // > 2x9 clock
//
//  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
//    clockCount--;
//  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
//    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
//    pinMode(SCL, OUTPUT); // then clock SCL Low
//    delayMicroseconds(10); //  for >5uS
//    pinMode(SCL, INPUT); // release SCL LOW
//    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
//    // do not force high as slave may be holding it low for clock stretching.
//    delayMicroseconds(10); //  for >5uS
//    // The >5uS is so that even the slowest I2C devices are handled.
//    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
//    int counter = 20;
//    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
//      counter--;
//      delay(100);
//      SCL_LOW = (digitalRead(SCL) == LOW);
//    }
//    if (SCL_LOW) { // still low after 2 sec error
//      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
//    }
//    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
//  }
//  if (SDA_LOW) { // still low
//    return 3; // I2C bus error. Could not clear. SDA data line held low
//  }
//
//  // else pull SDA line low for Start or Repeated Start
//  pinMode(SDA, INPUT); // remove pullup.
//  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
//  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
//  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
//  delayMicroseconds(10); // wait >5uS
//  pinMode(SDA, INPUT); // remove output low
//  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
//  delayMicroseconds(10); // x. wait >5uS
//  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
//  pinMode(SCL, INPUT);
//  return 0; // all ok
//}
//


