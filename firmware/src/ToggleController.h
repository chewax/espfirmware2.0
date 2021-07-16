/*
  ToggleController.h - Library for handling on/auto off pin behaviour
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#ifndef __ToggleController_h
#define __ToggleController_h

#include <Arduino.h>
#include "SocketIO.h"
#include "Controller.h"
#include <map>
#include <string>
#include "EventEmitter.h"

class ToggleController: public Controller
{
  public:
    ToggleController():Controller(){};
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void loop();
    void closeWhen(const uint64_t& timeout);
    void closeWhen(const std::string& event);
    void sense();

  private:
    bool isOn = false;
    uint64_t timeout_ms = 0;
    uint64_t startTime = millis();
    void close();
    
};

void ToggleController::sense() 
{
  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "switch";
  payload["state"] = isOn ? "on" : "off";

  socket -> send("board:data", payload);
}


void ToggleController::loop()
{
  if (isOn && timeout_ms > 0) {
    uint64_t now = millis();
    if(now - startTime > timeout_ms) close();
  }
}

void ToggleController::closeWhen(const uint64_t& timeout)
{
  timeout_ms = timeout;
}

void ToggleController::closeWhen(const std::string& event)
{
  EventEmitter::on(event, [this](){ this->close(); });
}

void ToggleController::close()
{
  isOn = false;
  digitalWrite(pin, LOW);
  this->sense();
}



void ToggleController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  socket->on("board:on", [this](JsonObject data){
    digitalWrite(this->pin, HIGH);
    this->startTime = millis();
    this->isOn = true;
    std::string evt = "toggle:"+ this->id +":on";
    EventEmitter::emit(evt);
    this->sense();
  });

  socket->on("board:off", [this](JsonObject data){
    this->close();
  });
}


#endif