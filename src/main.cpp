#include <Arduino.h>
#include <vector>

#include <ArduinoLog.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "Configuration.h"
#include "RF24Manager.h"

CRF24Manager *rf24Manager;

void setup() {
  Serial.begin(115200);  while (!Serial); delay(200);
  randomSeed(analogRead(0));

  Log.begin(LOG_LEVEL_NOTICE, &Serial);
  Log.noticeln("Initializing..."); 

  pinMode(INTERNAL_LED_PIN, OUTPUT);
  digitalWrite(INTERNAL_LED_PIN, HIGH);

  rf24Manager = new CRF24Manager();

  Log.infoln("Initialized");
}

void loop() {
  static uint8_t currentMode = 0;
  static unsigned long tsMillis = millis();
  static unsigned long tsMillisLed = millis();
  static bool boardLedOn = false;

  if (millis() - tsMillisLed > 300) {
      tsMillisLed = millis();
      // Blink board LED
      digitalWrite(INTERNAL_LED_PIN, boardLedOn ? HIGH : LOW);
      boardLedOn = !boardLedOn;
  }

  rf24Manager->loop();

  delay(5);
  yield();
}