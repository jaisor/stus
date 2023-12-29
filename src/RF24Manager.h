#pragma once

#include <ArduinoJson.h>
#include <RF24.h>
#include <vector>

#include "BaseManager.h"

class CRF24Manager: public CBaseManager {

private:
  unsigned long tMillis;
  StaticJsonDocument<2048> sensorJson;
  StaticJsonDocument<2048> configJson;

  RF24 *_radio;
  char _data[32];
    
public:
	CRF24Manager();
  ~CRF24Manager();

  // CBaseManager
  virtual void loop();
};
