#include <Arduino.h>
#include <vector>

#include <ArduinoLog.h>
#include <ArduinoLowPower.h>
#include <SPI.h>

#include "Configuration.h"
#include "RF24Manager.h"

CRF24Manager *rf24Manager;
unsigned long tsMillisBooted;

void setup() {
  pinMode(INTERNAL_LED_PIN, OUTPUT);
  intLEDOn();

  #ifndef DISABLE_LOGGING
  Serial.begin(115200);  while (!Serial); delay(100);
  randomSeed(analogRead(0));
  Log.begin(LOG_LEVEL_NOTICE, &Serial);
  Log.infoln("Initializing...");
  #endif

  pinMode(DEEP_SLEEP_DISABLE_PIN, INPUT_PULLUP);
  rf24Manager = new CRF24Manager();
  tsMillisBooted = millis();

  Log.infoln("Initialized");
  intLEDOff();
}

void loop() {
  
  intLEDOn();
  rf24Manager->loop();

  // Conditions for deep sleep:
  // - Min time elapsed since smooth boot
  // - Override pin high (open jumper)
  // - Any working managers report job done
  if (DEEP_SLEEP_INTERVAL_SEC > 0 
    && millis() - tsMillisBooted > DEEP_SLEEP_MIN_AWAKE_MS
    && digitalRead(DEEP_SLEEP_DISABLE_PIN) == HIGH
    && rf24Manager->isJobDone()) {

    Log.noticeln("Initiating deep sleep for %u sec", DEEP_SLEEP_INTERVAL_SEC);
    intLEDOff();

    #ifdef ESP32
      ESP.deepSleep((uint64_t)DEEP_SLEEP_INTERVAL_SEC * 1e6);
    #elif ESP8266
      ESP.deepSleep((uint64_t)DEEP_SLEEP_INTERVAL_SEC * 1e6); 
    #elif SEEED_XIAO_M0
      LowPower.deepSleep(DEEP_SLEEP_INTERVAL_SEC * 1000);
    #else
      Log.warningln("Scratch that, deep sleep is not supported on this platform, delaying instead");
      delayMicroseconds((uint64_t)DEEP_SLEEP_INTERVAL_SEC * 1e6);
    #endif
  }

/*
  if (rf24Manager->isRebootNeeded() 
    || (DEEP_SLEEP_INTERVAL_SEC > 0 && CONFIG_getUpTime() > DEEP_SLEEP_INTERVAL_SEC * 1000)) {

    Log.noticeln("Device is not sleeping right, resetting to save battery");
    #ifdef ESP32
      ESP.restart();
    #elif ESP8266
      ESP.reset();
    #elif SEEED_XIAO_M0
      NVIC_SystemReset();
    #endif
  }
*/

  Log.verboseln("millis() - tsMillisBooted: %i", millis() - tsMillisBooted);
  Log.verboseln("digitalRead(DEEP_SLEEP_DISABLE_PIN) == HIGH: %i", digitalRead(DEEP_SLEEP_DISABLE_PIN) == HIGH);
  Log.verboseln("rf24Manager->isJobDone(): %i", rf24Manager->isJobDone());

  delay(digitalRead(DEEP_SLEEP_DISABLE_PIN) == HIGH ? 5 : 1000); // slow the roll in sleep override
  yield();
}
