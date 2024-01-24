#pragma once

#include <Arduino.h>

#include "Configuration.h"

// Message Uptime-Voltage-Temperature-Humidity-BarometricPressure
#define MSG_UVTHP_ID 1
typedef struct r24_message_uvthp_t {
  uint8_t id;           // message id
  uint32_t uptime;
  float voltage;
  float temperature;
  float humidity;
  float baro_pressure;
  uint8_t pad[8];       // Pads message to 32 bytes
} _r24_message_uvtha_t;


class CBaseMessage {

protected:
    unsigned long tMillis;
    const u_int8_t pipe;

public:
	CBaseMessage(const u_int8_t pipe);
    const uint8_t getPipe() { return pipe; };
    virtual const String getString() { return String(""); }
};
