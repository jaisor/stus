#pragma once

#include <Arduino.h>
#include <functional>
#include <ArduinoLog.h>

#ifdef ESP32
  #define DEVICE_NAME "ESP32RFGW"
#elif ESP8266
  #define DEVICE_NAME "ESP8266RFGW"
#elif SEEED_XIAO_M0
  #define DEVICE_NAME "STUSXIAO"
#endif

#define INTERNAL_LED_PIN LED_BUILTIN

uint32_t CONFIG_getDeviceId();
unsigned long CONFIG_getUpTime();