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

#include <Arduino.h>
#include <Time.h>

#include "RF24Manager.h"
#include "Configuration.h"

bool written = false;
float payload = 0.0;

CRF24Manager::CRF24Manager() {  
  _radio = new RF24(CE_PIN, CSN_PIN);
  
  if (!_radio->begin()) {
    Log.errorln("Failed to initialize RF24 radio");
    return;
  }

  uint8_t addr[5];
  memcpy(addr, RF24_ADDRESS, 5);
  
  _radio->setAddressWidth(5);
  _radio->setDataRate(RF24_DATA_RATE);
  _radio->setPALevel(RF24_PA_LEVEL);
  _radio->setChannel(RF24_CHANNEL);
  _radio->setPayloadSize(sizeof(float));
  _radio->setRetries(10, 15);
  _radio->openWritingPipe(addr);
  _radio->stopListening();

#ifndef DISABLE_LOGGING
  Log.infoln("Radio initialized...");
  Log.noticeln("  Channel: %i", _radio->getChannel());
  Log.noticeln("  DataRate: %i", _radio->getDataRate());
  Log.noticeln("  PALevel: %i", _radio->getPALevel());
  Log.noticeln("  PayloadSize: %i", _radio->getPayloadSize());

  char buffer[870] = {'\0'};
  uint16_t used_chars = _radio->sprintfPrettyDetails(buffer);
  Log.noticeln(buffer);
#endif
}

CRF24Manager::~CRF24Manager() { 
  delete _radio;
  Log.noticeln("CRF24Manager destroyed");
}

void CRF24Manager::loop() {
  if (!written) {
    //written = true;
    if (_radio->write(&payload, sizeof(float))) {
      intLEDOn();
      Log.noticeln("Transmitted %D", payload);
      payload += 0.01;
      delay(500);
      intLEDOff();
    } else {
      intLEDOff();
      Log.noticeln("RF24 transmit error");
      delay(500);
    }
  }
}