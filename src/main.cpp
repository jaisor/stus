#include <Arduino.h>
#include <vector>

#include <ArduinoLog.h>
#include <SPI.h>

#include "Configuration.h"
#include "RF24Manager.h"

CRF24Manager *rf24Manager;

void setup() {
  pinMode(INTERNAL_LED_PIN, OUTPUT);
  intLEDOn();

  #ifndef DISABLE_LOGGING
  Serial.begin(115200);  while (!Serial); delay(200);
  randomSeed(analogRead(0));
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
  #endif

  Log.noticeln("Initializing..."); 
  
  rf24Manager = new CRF24Manager();
  Log.infoln("Initialized");
  intLEDOff();
}

void loop() {
  rf24Manager->loop();
  yield();
}