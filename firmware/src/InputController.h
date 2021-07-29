/*
  InputController.h - Library for handling on/of pin behaviour
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/


#include <Arduino.h>
#include "SocketIO.h"
#include "Controller.h"
#include <map>
#include <string>

#ifndef __InputController_h
#define __InputController_h


class InputController: public Controller
{
  public:
    InputController():Controller(){};
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void loop();

  protected:
    int lastReading;
};


void InputController::loop()
{
  //We set a pin to act as a local (local to the board) input to alter the switch state.
  //such pin will behave as a toggle. That way you can always change the state of the switch with a single flick. 
  int currentReading = digitalRead(pin);
  if (lastReading != currentReading) {
    lastReading = currentReading;
    if (currentReading == HIGH) on();
    else off ();
  }
  
}

void InputController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  // Controller::init(t_socket, t_pin, t_name, t_actuator);
  // Override controller.init and dont call...we dont need to check in this element
  ip = WiFi.localIP().toString().c_str();
  mac = WiFi.macAddress().c_str();
  socket = t_socket;
  pin = t_pin;
  actuator = t_actuator;
  name = t_name;
  id =  std::to_string(pin) + "@" + mac;
  evt_start = id + ":start";
  evt_end = id + ":end";

  pinMode(pin, INPUT);
  lastReading = LOW;

  broadcastEvents = true;
}

#endif