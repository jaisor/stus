#if !( defined(SEEED_XIAO_M0) )
  #error This code is intended to run on SEEED_XIAO_M0 platform!
#endif

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <ArduinoLog.h>

#define CE_PIN  D2
#define CSN_PIN D3

#include <Arduino.h>
#include <Time.h>

#include "RF24Manager.h"
#include "Configuration.h"

const uint8_t address[7] = "mynode";
bool written = false;
float payload = 0.0;

CRF24Manager::CRF24Manager() {  
  _radio = new RF24(CE_PIN, CSN_PIN);
  
  Log.infoln("Radio begin successfully: %t", _radio->begin());
  _radio->setDataRate(RF24_250KBPS);
  _radio->setPALevel(RF24_PA_LOW);
  _radio->setAddressWidth(7);
  _radio->openWritingPipe(address);
  _radio->setPayloadSize(sizeof(payload));
  _radio->stopListening();

  Log.infoln("Radio initialized...");
  Log.noticeln("  Channel: %i", _radio->getChannel());
  Log.noticeln("  PayloadSize: %i", _radio->getPayloadSize());
  Log.noticeln("  DataRate: %i", _radio->getDataRate());
  Log.noticeln("  isPVariant: %t", _radio->isPVariant());

  _radio->printDetails();
}

CRF24Manager::~CRF24Manager() { 
  delete _radio;
  Log.noticeln("CRF24Manager destroyed");
}

const byte test[6] = "Proba";

void CRF24Manager::loop() {
  if (!written) {
    //written = true;
    //Log.noticeln("Writing: %s", test);
    Log.noticeln("Writing %D result: %t", payload, _radio->write(&payload, sizeof(float)));
    payload += 0.01;
    delay(1000);
  }
}