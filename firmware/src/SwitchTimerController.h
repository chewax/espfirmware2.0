/*
  SwitchTimerController.h - Library for handling on/of pin behaviour
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/


#include <Arduino.h>
#include "SocketIO.h"
#include "SwitchController.h"
#include <map>
#include <string>

#ifndef __SwitchTimerController_h
#define __SwitchTimerController_h

class SwitchTimerController: public SwitchController
{
  public:
    SwitchTimerController():SwitchController(){};
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void setOnOfftime(uint64_t t_onSlot, uint64_t t_offSlot);  
    void loop();
    
  protected:
    uint64_t onSlot = -1;
    uint64_t offSlot = -1;
    uint64_t currentTime = -1;
    uint64_t lastSense = millis();
    bool timerOn = false;
    uint64_t minAcceptableError = 1 * (60 * 1000); //1 minute
};


void SwitchTimerController::loop()
{
  uint64_t now = millis();

  if(now - lastSense > minAcceptableError) {
      lastSense = now;

      std::map<std::string, std::string> payload;
      payload["id"] = id;

      socket->send("time:check", payload);
  }

   if (onSlot < currentTime && currentTime < offSlot) 
   {
     if (!timerOn) {
        on();
        timerOn = true;
     } 
   }
   else {
     if (timerOn) {
       off();
       timerOn = false;
     }
   }
}

void SwitchTimerController::setOnOfftime(uint64_t t_onSlot, uint64_t t_offSlot)
{
  onSlot = t_onSlot;
  offSlot = t_offSlot;
}

void SwitchTimerController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  SwitchController::init(t_socket, t_pin, t_name, t_actuator);

  socket->on("time:check", [this](JsonObject data) {
    currentTime = data["h"].as<int>() * 60 * 60;
    currentTime = currentTime + data["m"].as<int>() * 60;
    currentTime = currentTime + data["s"].as<int>();
  });
}

#endif