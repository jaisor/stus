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

const byte thisSlaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};
bool written = false;

CRF24Manager::CRF24Manager() {  
  _radio = new RF24(CE_PIN, CSN_PIN);
  
  _radio->begin();
  _radio->setDataRate( RF24_250KBPS );
  _radio->openWritingPipe(thisSlaveAddress);
  _radio->setPALevel(RF24_PA_MIN);
  _radio->stopListening();

  Log.infoln("Radio initialized...");
  Log.noticeln("  Channel: %i", _radio->getChannel());
  Log.noticeln("  PayloadSize: %i", _radio->getPayloadSize());
  Log.noticeln("  DataRate: %i", _radio->getDataRate());
  Log.noticeln("  isPVariant: %i", _radio->isPVariant());
}

CRF24Manager::~CRF24Manager() { 
  delete _radio;
  Log.noticeln("CRF24Manager destroyed");
}

const byte test[6] = "Proba";

void CRF24Manager::loop() {
  if (!written) {
    written = true;
    Log.noticeln("Writing: %s", test);
    bool r = _radio->write(&test, sizeof(test));
    Log.noticeln("Writing result: %i", r);
  }
}