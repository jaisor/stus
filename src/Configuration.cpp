#include <Arduino.h>
#include "Configuration.h"

uint32_t CONFIG_getDeviceId() {
    // Create AP using fallback and chip ID
  uint32_t chipId = 0;
  #ifdef ESP32
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
  #elif ESP8266
    chipId = ESP.getChipId();
  #endif

  return chipId;
}

static unsigned long tMillisUp = millis();
unsigned long CONFIG_getUpTime() {  
  return millis() - tMillisUp;
}