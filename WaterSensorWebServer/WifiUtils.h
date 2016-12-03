#ifndef H_WIFIUTILS
#define H_WIFIUTILS

#include <Arduino.h> 

extern "C" {
#include "user_interface.h"
}

IPAddress SetIp(int ip1, int ip2, int ip3, int ip4){
  IPAddress ip = IPAddress(ip1, ip2, ip3, ip4); 
  IPAddress gateway(192, 168, 10, 1); 
  IPAddress subnet(255, 255, 255, 0); 
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  WiFi.config(ip, gateway, subnet);

  return ip;
  
}

void SetMac() {

 // Set as SoftAP and client
  WiFi.mode(WIFI_AP_STA);       
  byte *mac = (byte*)malloc(6);
  WiFi.macAddress(mac);

  PrintDebug("reading mac: ");
  String strmac;
  for (int i = 0; i < 6; ++i)
  {
    strmac += mac[i];
  }
  Serial.println(strmac);
  
  PrintDebug("reading eeprom mac: ");
  String eemac;
  for (int i = 0; i < 6; ++i)
  {
    eemac += EEPROM.read(i);
  }
  Serial.println(eemac);
  if(eemac=="000000" || eemac=="255255255255255255"){
    
    PrintDebug("eemac empty.. ");
    PrintDebug("writing mac to eeprom: ");
    for (int i = 0; i < 6; ++i)
    {
      EEPROM.write(i, mac[i]);
      PrintDebug("Wrote: ");
      Serial.println(mac[i]); 
    }
    EEPROM.commit();
    delay(100);
  }

  PrintDebug("setting mac from eeprom..");
  
  for (int i = 0; i < 6; ++i)
  {
    mac[i] = EEPROM.read(i);
  }
  wifi_set_macaddr(SOFTAP_IF, mac);
  free(mac);
  delay(100);
  PrintDebug("finished setting mac from eeprom.");

//// // Change MAC 
//// byte *mac = (byte*)malloc(6);
//// WiFi.macAddress(mac);
//// mac[0] = 0x16;
//// mac[1] = 0x19;
//// mac[2] = 0xCA;
//// wifi_set_macaddr(STATION_IF, mac);
//// mac[2] = 0xCB;
////  wifi_set_macaddr(SOFTAP_IF, mac);
////  free(mac);
//  
//  //uint8_t mac[] = {0x77, 0x01, 0x02, 0x03, 0x04, 0x05};
//  //wifi_set_macaddr(STATION_IF, &mac[0]);
}

#endif
