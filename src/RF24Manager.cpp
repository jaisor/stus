#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <ArduinoLog.h>

#ifdef ESP32
  #define CE_PIN  GPIO_NUM_22
  #define CSN_PIN GPIO_NUM_21
#elif ESP8266
  #define CE_PIN  GPIO_NUM_22
  #define CSN_PIN GPIO_NUM_21
#elif SEEED_XIAO_M0
  #define CE_PIN  D2
  #define CSN_PIN D3
#endif

#define MAX_RETRIES_BEFORE_DONE 10

#include <Arduino.h>
#include <Time.h>

#include "RF24Manager.h"
#include "Configuration.h"

CRF24Manager::CRF24Manager() {  
  jobDone = false;
  _radio = new RF24(CE_PIN, CSN_PIN);
  
  if (!_radio->begin()) {
    Log.errorln("Failed to initialize RF24 radio");
    return;
  }

  uint8_t addr[6];
  memcpy(addr, RF24_ADDRESS, 6);
  
  _radio->setAddressWidth(5);
  _radio->setDataRate(RF24_DATA_RATE);
  _radio->setPALevel(RF24_PA_LEVEL);
  _radio->setChannel(RF24_CHANNEL);
  _radio->setPayloadSize(_msg.getMessageLength());
  _radio->setRetries(15, 15);
  _radio->openWritingPipe(addr);
  _radio->stopListening();

#ifndef DISABLE_LOGGING
  Log.infoln("Radio initialized...");
  Log.noticeln("  Channel: %i", _radio->getChannel());
  Log.noticeln("  DataRate: %i", _radio->getDataRate());
  Log.noticeln("  PALevel: %i", _radio->getPALevel());
  Log.noticeln("  PayloadSize: %i", _radio->getPayloadSize());

  //char buffer[870] = {'\0'};
  //uint16_t used_chars = _radio->sprintfPrettyDetails(buffer);
  //Log.noticeln(buffer);
#endif

  tMillis = millis();
  retries = 0;

  _msg.setVoltage(0.12);
  _msg.setTemperature(3);
  _msg.setHumidity(4);
}

CRF24Manager::~CRF24Manager() { 
  delete _radio;
  Log.noticeln("CRF24Manager destroyed");
}

void CRF24Manager::loop() {
  if (isJobDone()) {
    // Nothing to do
    return;
  }

  // Take measurement
  _msg.setUptime(CONFIG_getUpTime());

  if (_radio->write(_msg.getMessageBuffer(), _msg.getMessageLength())) {
    Log.noticeln("Transmitted message length %i with voltage %D", _msg.getMessageLength(), _msg.getVoltage());
    _msg.setVoltage(_msg.getVoltage() + 0.01);
    jobDone = true;
  } else {
    intLEDOff();
    if (++retries > MAX_RETRIES_BEFORE_DONE) {
      Log.warningln("Failed to transmit after %i retries", retries);
      jobDone = true;
      return;
    }
    uint16_t backoffDelaySec = 100 * retries * retries;  // Exp back off with each attempt
    Log.noticeln("RF24 transmit error, will try again for attempt %i after %i seconds", retries, backoffDelaySec);
    delay(backoffDelaySec);
  }
}