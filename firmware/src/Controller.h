/*
  Controller.h - Base class for all the subsequent controllers
  Each controller class is a driver for a specific sensor/behaviour
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#include <Arduino.h>
#include <string>
#include "SocketIO.h"

#ifndef __Controller_h
#define __Controller_h

class Controller
{
  public:
    Controller();

    std::string id;
    std::string name;
    std::string actuator;
    std::string ip;
    std::string mac;
    std::string status;

    virtual void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    virtual void loop();
    virtual void sense();
    void checkIn();

  protected:
    SocketIO* socket;
    int pin;
};

Controller::Controller(){}

void Controller::loop(){}
void Controller::sense(){}

void Controller::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  socket = t_socket;
  pin = t_pin;
  ip = WiFi.localIP().toString().c_str();
  mac = WiFi.macAddress().c_str();
  actuator = t_actuator;
  name = t_name;
  id =  std::to_string(pin) + "@" + mac;

  socket->on("connect", [this](JsonObject data){
      Debug::printf("[SOCKETIO] onConnect Callback\n");
      this->checkIn();
      this->sense();
  });
}

//Registers the board into the server.
void Controller::checkIn()
{
    std::map<std::string, std::string> payload = {
        {"id", id.c_str()},
        {"name", name.c_str()},
        {"actuator", actuator.c_str()},
        {"ip", ip.c_str()},
        {"status", status.c_str()}
    };

    socket->send("board:register", payload);
}

#endif