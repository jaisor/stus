#include <Arduino.h>
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

#ifdef RADIO_RF24
  #define MAX_RETRIES_BEFORE_DONE 10
#else
  #define MAX_RETRIES_BEFORE_DONE 1
#endif

#include <Arduino.h>
#include <Time.h>

#include "RF24Manager.h"
#include "Configuration.h"

CRF24Manager::CRF24Manager(ISensorProvider* sensor)
:sensor(sensor) {  
  jobDone = false;
  _radio = new RF24(CE_PIN, CSN_PIN);
  
  if (!_radio->begin()) {
    Log.errorln(F("Failed to initialize RF24 radio"));
    return;
  }

  #ifdef RADIO_RF24

  uint8_t addr[6];
  memcpy(addr, RF24_ADDRESS, 6);
  
  _radio->setAddressWidth(5);
  _radio->setDataRate(RF24_DATA_RATE);
  _radio->setPALevel(RF24_PA_LEVEL);
  _radio->setChannel(RF24_CHANNEL);
  _radio->setPayloadSize(CRF24Message::getMessageLength());
  _radio->setRetries(15, 15);
  _radio->openWritingPipe(addr);
  _radio->stopListening();

  Log.infoln("Radio initialized");
  if (Log.getLevel() >= LOG_LEVEL_NOTICE) {
    Log.noticeln(F(" RF Channel: %i"), _radio->getChannel());
    Log.noticeln(F(" RF DataRate: %i"), _radio->getDataRate());
    Log.noticeln(F(" RF PALevel: %i"), _radio->getPALevel());
    Log.noticeln(F(" RF PayloadSize: %i"), _radio->getPayloadSize());

    if (Log.getLevel() >= LOG_LEVEL_VERBOSE) {
      char buffer[870] = {'\0'};
      uint16_t used_chars = _radio->sprintfPrettyDetails(buffer);
      Log.verboseln(buffer);
    }
  }
  #else
    jobDone = true;
  #endif

  tMillis = millis();
  retries = 0;
}

CRF24Manager::~CRF24Manager() { 
  powerDown();
  delay(5);
  delete _radio;
  Log.noticeln(F("CRF24Manager destroyed"));
}

void CRF24Manager::loop() {
  if (isJobDone()) {
    // Nothing to do
    return;
  }

  // Take measurement
  CRF24Message msg(
    0, 
    sensor->getUptime(),
    sensor->getBatteryVoltage(NULL), // in V
    sensor->getTemperature(NULL), // in C
    sensor->getHumidity(NULL), // in %
    sensor->getBaroPressure(NULL) // in Pascal
  );

  if (Log.getLevel() >= LOG_LEVEL_VERBOSE) {
    Log.verboseln(F("Msg: %s"), msg.getString().c_str());
  }

  if (_radio->write(msg.getMessageBuffer(), msg.getMessageLength())) {
    Log.noticeln(F("Transmitted message length %i with voltage %D"), msg.getMessageLength(), msg.getVoltage());
    msg.setVoltage(msg.getVoltage() + 0.01);
    jobDone = true;
  } else {
    if (++retries > MAX_RETRIES_BEFORE_DONE) {
      Log.warningln(F("Failed to transmit after %i retries"), retries);
      jobDone = true;
      return;
    }
    uint16_t backoffDelaySec = 100 * retries * retries;  // Exp back off with each attempt
    Log.noticeln(F("RF24 transmit error, will try again for attempt %i after %i seconds"), retries, backoffDelaySec);
    delay(backoffDelaySec);
    intLEDBlink(50);
  }
}

void CRF24Manager::powerDown() {
  jobDone = true;
  #ifdef RADIO_RF24
    _radio->powerDown();
  #endif
}

void CRF24Manager::powerUp() {
  jobDone = false;
  tMillis = millis();
  retries = 0;
  _radio->powerUp();
}