#pragma once

#include <Arduino.h>
#include <functional>
#include <ArduinoLog.h>

#define DISABLE_LOGGING

#ifdef ESP32
  #define DEVICE_NAME "ESP32RFGW"
#elif ESP8266
  #define DEVICE_NAME "ESP8266RFGW"
#elif SEEED_XIAO_M0
  #define DEVICE_NAME "STUSXIAO"
#endif

#define RADIO_RF24
#ifdef RADIO_RF24
  #define RF24_CHANNEL 76
  #define RF24_DATA_RATE RF24_250KBPS
  #define RF24_PA_LEVEL RF24_PA_HIGH
  #define RF24_ADDRESS "TSIAJ" // Must end on SIAJ, only the first byte matters for pipes 1-5
#endif

#define INTERNAL_LED_PIN LED_BUILTIN

uint32_t CONFIG_getDeviceId();
unsigned long CONFIG_getUpTime();

void intLEDOn();
void intLEDOff();