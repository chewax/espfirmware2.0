/*
  DHTLogger.h - Library for handling DH11 dht measurements
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#ifndef __DHTLogger_h
#define __DHTLogger_h

#include <Arduino.h>
#include "SocketIO.h"
#include "DHT.h"
#include "Controller.h"
#include <map>
#include <string>
#include "Request.h"


class DHTLogger: public DHTController
{
  public:
    DHTLogger():DHTController(){};
    ~DHTLogger();

    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void checkIn();
    void sense();
    
  protected:
    Request* request;
};

DHTLogger::~DHTLogger()
{
  delete request;
  DHTController::~DHTController();
}

void DHTLogger::checkIn()
{
  //We want this to be headless and just log to a server.
  //Remove check in functionality
}

void DHTLogger::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  DHTController::init(t_socket, t_pin, t_name, t_actuator);
  request = new Request("http://192.168.3.195:3000/api/log");
}

void DHTLogger::sense()
{
  Debug::printf("SENSE\n");

  float h = dht->readHumidity();
  float t = dht->readTemperature();
  float hic = dht->computeHeatIndex(t, h, false); 

  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "dht";
  payload["temperature"] = std::to_string(t).substr(0,4);
  payload["humidity"] = std::to_string(h).substr(0,4);
  payload["hic"] = std::to_string(hic).substr(0,4);

  int res = request->POST(payload);
  Debug::printf("HttpResponse: %d", res);

}




#endif