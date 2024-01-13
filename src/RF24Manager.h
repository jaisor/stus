#pragma once

#include <RF24.h>
#include <vector>

#include "BaseManager.h"
#include "RF24Message.h"

class CRF24Manager: public CBaseManager {

private:
  unsigned long tMillis;
  uint8_t retries;

  RF24 *_radio;
  CRF24Message _msg;

  bool jobDone;
    
public:
	CRF24Manager();
  ~CRF24Manager();

  // CBaseManager
  virtual void loop();
  virtual bool isJobDone() { return jobDone; }
};
