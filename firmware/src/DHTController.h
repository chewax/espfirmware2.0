/*
  DHTController.h - Library for handling DH11 dht measurements
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include "DHT.h"
#include "Controller.h"
#include <map>
#include <string>

#ifndef __DHTController_h
#define __DHTController_h

class DHTController: public Controller
{
  public:
    DHTController():Controller(){};

    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void loop();
    void sense();
    void setSenseInterval(const uint64_t& ms);

  protected:
    DHT* dht;
    uint64_t lastSense = millis();
    uint64_t senseInterval = 10 * 1000; //10 seconds
};


void DHTController::setSenseInterval(const uint64_t& ms)
{
  senseInterval = ms;
}

void DHTController::loop()
{
  uint64_t now = millis();
  if(now - lastSense > senseInterval) {
      lastSense = now;
      sense();
  }
}

void DHTController::sense()
{
  float h = dht->readHumidity();
  float t = dht->readTemperature();
  float hic = dht->computeHeatIndex(t, h, false); 

  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "dht";
  payload["temperature"] = std::to_string(t).substr(0,4);
  payload["humidity"] = std::to_string(h).substr(0,4);
  payload["hic"] = std::to_string(hic).substr(0,4);

  socket -> send("board:data", payload);
}

void DHTController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  dht = new DHT(pin, DHT11);
  dht->begin();
}

#endif