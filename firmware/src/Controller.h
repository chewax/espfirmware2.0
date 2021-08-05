/*
  Controller.h - Base class for all the subsequent controllers
  Each controller class is a driver for a specific sensor/behaviour
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#include <Arduino.h>
#include <string>
#include "SocketIO.h"
#include "EventEmitter.h"

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

    void broadcastOn();
    void broadcastOff();

    //Whenever the controller starts or ends an action ti will emit either the start or end event to both the EventEmitter 
    //and the socketIO. That way all boards are informed of the actions performed by this controller.
    //broadcasting can be disabled by calling the appropiate functions
    std::string evt_on;
    std::string evt_off;
    virtual void onWhen(const std::string& event, bool remote);
    virtual void offWhen(const std::string& event, bool remote);
    virtual void toggleWhen(const std::string& event, bool remote);

  protected:
    SocketIO* socket;
    int pin;
    
    bool broadcastEvents = false;
    virtual void on();
    virtual void off();
    virtual void toggle();
    

};

Controller::Controller(){}

void Controller::loop(){}
void Controller::sense(){}

//Provide a way to subordinate on to a specific event
//This event may come either from local EventEmitter or from remote SocketIO
void Controller::onWhen(const std::string& event, bool remote=false){
  if (remote) socket->on(event, [this](JsonObject data){  this->on(); });
  else EventEmitter::on(event, [this](){ this->on(); });
}

//Provide a way to subordinate off to a specific event
//This event may come either from local EventEmitter or from remote SocketIO
void Controller::offWhen(const std::string& event, bool remote=false){
  if (remote) socket->on(event, [this](JsonObject data){  this->off(); });
  else EventEmitter::on(event, [this](){ this->off(); });
}

//Provide a way to subordinate off to a specific event
//This event may come either from local EventEmitter or from remote SocketIO
void Controller::toggleWhen(const std::string& event, bool remote=false){
  if (remote) socket->on(event, [this](JsonObject data){  this->toggle(); });
  else EventEmitter::on(event, [this](){ this->toggle(); });
}

void Controller::on(){
  //If broadcast is enabled inform all other boards via EventEmitter and Socketio
  if (broadcastEvents) {
    EventEmitter::emit(this->evt_on);
    socket->quickSend("board:broadcast", "message", this->evt_on);
  }
}

void Controller::off(){
  //If broadcast is enabled inform all other boards via EventEmitter and Socketio
  if (broadcastEvents) {
    EventEmitter::emit(this->evt_off);
    socket->quickSend("board:broadcast", "message", this->evt_off);
  }
}

void Controller::toggle(){}

//Turns broadcast events on
void Controller::broadcastOn() {
  broadcastEvents = true;
}

//Turns broadcast events off
void Controller::broadcastOff() {
  broadcastEvents = false;
}

//Initializes controller with all the common information
void Controller::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  socket = t_socket;
  pin = t_pin;
  ip = WiFi.localIP().toString().c_str();
  mac = WiFi.macAddress().c_str();
  actuator = t_actuator;
  name = t_name;
  id =  std::to_string(pin) + "@" + mac;
  evt_on = id + ":on";
  evt_off = id + ":off";

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