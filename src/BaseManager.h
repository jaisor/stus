#pragma once

#include "Configuration.h"

class CBaseManager {
public:
  virtual void loop() {};
  virtual void powerDown() {};
  virtual void powerUp() {};
  virtual bool isRebootNeeded() { return false; }
  virtual bool isJobDone() { return false; }
};
