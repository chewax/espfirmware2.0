/*
  Interface.h - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include <string>
#include "Controller.h"
#include "DHTController.h"
#include "SwitchController.h"
#include "ToggleController.h"
#include "CaudalController.h"
#include "FillController.h"
#include "InputController.h"
#include "DHTLogger.h"

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
  
    Controller* ifController;

  private:
    SocketIO* socket;
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
  if (t_actuator == "dhtLogger") ifController = new DHTLogger();
  else if (t_actuator == "caudal") ifController = new CaudalController();
  else if (t_actuator == "filler") ifController = new FillController();
  else if (t_actuator == "valve" || t_actuator == "bulb" || t_actuator == "switch") ifController = new SwitchController();
  else if (t_actuator == "toggle") ifController = new ToggleController();
  else if (t_actuator == "digitalInput") ifController = new InputController();

  ifController->init(t_socket, t_pin, t_name, t_actuator);
}