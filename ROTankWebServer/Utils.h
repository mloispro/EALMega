#ifndef H_UTILS
#define H_UTILS

#include <Arduino.h> 

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
  Serial.print("~esp~ ");
  Serial.println(msg);
  Serial.flush();
}

void PrintDebug(String msg, int val) {
  Serial.print("~esp~ ");
  Serial.print(msg);
  Serial.print(": ");
  Serial.println(val);
  Serial.flush();
}

void PrintDebug(String msg, String val) {
  Serial.print("~esp~ ");
  Serial.print(msg);
  Serial.print(": ");
  Serial.println(val);
  Serial.flush();
}
void PrintDebug(String msg, char *val) {
  Serial.print("~esp~ ");
  Serial.print(msg);
  Serial.print(": ");
  Serial.println(val);
  Serial.flush();
}

#endif
