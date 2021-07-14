/*
  ToggleController.h - Library for handling on/auto off pin behaviour
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include "Controller.h"
#include <map>
#include <string>

#ifndef __ToggleController_h
#define __ToggleController_h

class ToggleController: public Controller
{
  public:
    ToggleController():Controller(){};
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator, const int& t_timeout_ms);
    void loop();

  private:
    bool open = false;
    uint64_t timeout_ms = 0;
    uint64_t startTime = millis();
    void close();
    
};

void ToggleController::loop()
{
  if (open) {
    uint64_t now = millis();
    if(now - startTime > timeout_ms) close();
  }
}

void ToggleController::close()
{
  open = false;
  digitalWrite(pin, LOW);

  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "switch";
  payload["state"] = "off";

  socket -> send("board:data", payload);
}

void ToggleController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator, const int& t_timeout_ms)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  timeout_ms = t_timeout_ms;

  socket->on("board:on", [this](JsonObject data){
    digitalWrite(this->pin, HIGH);
    this->startTime = millis();
    this->open = true;
  });

}


#endif