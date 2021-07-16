/*
  SwitchController.h - Library for handling on/of pin behaviour
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/


#include <Arduino.h>
#include "SocketIO.h"
#include "Controller.h"
#include <map>
#include <string>

#ifndef __SwitchController_h
#define __SwitchController_h

class SwitchController: public Controller
{
  public:
    SwitchController():Controller(){};
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void sense();

  private:
    bool isOn;
};

void SwitchController::sense() 
{
  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "switch";
  payload["state"] = isOn ? "on" : "off";

  socket -> send("board:data", payload);
}

void SwitchController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  socket->on("board:on", [this](JsonObject data){
    digitalWrite(this->pin, HIGH);
    this->isOn = true;
    this->sense();
  });

  socket->on("board:off", [this](JsonObject data){
    digitalWrite(this->pin, LOW);
    this->isOn = false;
    this->sense();
  });
}

#endif