#pragma once

#include "Configuration.h"

class CBaseManager {
public:
  virtual void loop() {};
  virtual bool isRebootNeeded() { return false; }
  virtual bool isJobDone() { return false; }
};
