#include <Arduino.h>
#include <vector>

#include <ArduinoLog.h>
#include <SPI.h>

#if defined(SEEED_XIAO_M0)
  #include <ArduinoLowPower.h>
#endif

#include "Configuration.h"
#include "RF24Manager.h"
#include "Device.h"

CRF24Manager *rf24Manager;
CDevice *device;
unsigned long tsMillisBooted;

#if defined(DISABLE_LOGGING) && defined(SEEED_XIAO_M0)
  #define LED_SETUP PIN_LED_TXL
#else
  #define LED_SETUP INTERNAL_LED_PIN
#endif

void setup() {
  pinMode(INTERNAL_LED_PIN, OUTPUT);
  
  pinMode(LED_SETUP, OUTPUT);
  digitalWrite(LED_SETUP, LOW);

  #ifndef DISABLE_LOGGING
  Serial.begin(19200);  while (!Serial); delay(100);
  randomSeed(analogRead(0));
  Log.begin(LOG_LEVEL, &Serial);
  Log.infoln("Initializing...");
  #endif

  device = new CDevice();
  rf24Manager = new CRF24Manager(device);
  tsMillisBooted = millis();

  if (rf24Manager->isError()) {
    Log.errorln("rf24Manager->isError()=%i;", rf24Manager->isError());
    while(true) {
      intLEDBlink(250);
      delay(250);
    }
  }

  delay(1000);
  Log.infoln("Initialized");
  digitalWrite(LED_SETUP, HIGH);
}

void loop() {

  intLEDOn();
  device->loop();
  rf24Manager->loop();

  // Conditions for deep sleep:
  // - Min time elapsed since smooth boot
  // - Any working managers report job done
  if (DEEP_SLEEP_INTERVAL_SEC > 0 
    && millis() - tsMillisBooted > DEEP_SLEEP_MIN_AWAKE_MS
    && rf24Manager->isJobDone()) {

    Log.noticeln(F("Initiating deep sleep for %u sec"), DEEP_SLEEP_INTERVAL_SEC);
    intLEDOff();
    #if defined(ESP32)
      ESP.deepSleep((uint64_t)DEEP_SLEEP_INTERVAL_SEC * 1e6);
    #elif defined(ESP8266)
      ESP.deepSleep((uint64_t)DEEP_SLEEP_INTERVAL_SEC * 1e6); 
    #elif defined(SEEED_XIAO_M0)
      rf24Manager->powerDown();
      LowPower.deepSleep(DEEP_SLEEP_INTERVAL_SEC * 1000);
      delay(100);
      // This deep sleep resumes where it left off on waking
      rf24Manager->powerUp();
      tsMillisBooted = millis();
    #else
      Log.warningln(F("Scratch that, deep sleep is not supported on this platform, delaying instead"));
      delayMicroseconds((uint64_t)DEEP_SLEEP_INTERVAL_SEC * 1e6);
    #endif
  } else if (DEEP_SLEEP_INTERVAL_SEC == 0 
    && millis() - tsMillisBooted > DEEP_SLEEP_MIN_AWAKE_MS
    && rf24Manager->isJobDone()) {
    intLEDOff();
    rf24Manager->powerDown();
    delay(1000); // Wait a second... then rinse and repeat
    rf24Manager->powerUp();
    tsMillisBooted = millis();
  }

  if (rf24Manager->isRebootNeeded() 
    || (DEEP_SLEEP_INTERVAL_SEC > 0 && (millis() - tsMillisBooted) > DEEP_SLEEP_INTERVAL_SEC * 1000)) {

    Log.noticeln(F("Device is not sleeping right, resetting to save battery"));
    #ifdef ESP32
      ESP.restart();
    #elif ESP8266
      ESP.reset();
    #elif SEEED_XIAO_M0
      NVIC_SystemReset();
    #endif
  }

  yield();
}
