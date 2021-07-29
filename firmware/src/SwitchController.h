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

  protected:
    bool isOn;
    void off();
    void on();
    void toggle();
};

void SwitchController::sense() 
{
  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "switch";
  payload["state"] = isOn ? "on" : "off";

  socket -> send("board:data", payload);
}

void SwitchController::toggle()
{
  if (isOn) off();
  else on();
}

void SwitchController::off()
{
  digitalWrite(pin, LOW);
  isOn = false;
  this->sense(); //senses the current data and passes to server to update UI
  Controller::off();
}

void SwitchController::on()
{
  digitalWrite(pin, HIGH);
  isOn = true;
  this->sense(); //senses the current data and passes to server to update UI
  Controller::on();
}

void SwitchController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  socket->on("board:on", [this](JsonObject data){
    this->on();
  });

  socket->on("board:off", [this](JsonObject data){
    this->off();
  });
}

#endif