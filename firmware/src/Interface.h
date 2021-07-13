/*
  Board.h - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include <string>
#include "Controller.h"
#include "DHTController.h"
#include "SwitchController.h"

#ifndef __Interface_h
#define __Interface_h
#endif

class Interface
{
  public:
    Interface();
    void init(SocketIO* socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void loop();
    ~Interface();
  
  private:
    SocketIO* socket;
    Controller* ifController;
};

Interface::Interface(){}

Interface::~Interface(){
  delete ifController;
}

void Interface::loop()
{
  ifController->loop();
}

void Interface::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  if (t_actuator == "dht") ifController = new DHTController();
  else if (t_actuator == "valve") ifController = new SwitchController();
  
  ifController->init(t_socket, t_pin, t_name, t_actuator);
}