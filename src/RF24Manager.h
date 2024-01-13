#pragma once

#include <RF24.h>
#include <vector>

#include "BaseManager.h"
#include "RF24Message.h"
#include "SensorProvider.h"

class CRF24Manager: public CBaseManager {

private:
  unsigned long tMillis;
  uint8_t retries;

  RF24 *_radio;
  CRF24Message _msg;

  ISensorProvider* sensor;
  bool jobDone;
    
public:
	CRF24Manager(ISensorProvider* sensor);
  virtual ~CRF24Manager();

  // CBaseManager
  virtual void loop();
  virtual void powerDown();
  virtual void powerUp();
  virtual bool isJobDone() { return jobDone; }
};
