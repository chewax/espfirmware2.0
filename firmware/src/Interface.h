/*
  Board.h - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include <string>
#include <map>

#ifndef __Interface_h
#define __Interface_h
#endif


class Interface
{
  public:
    Interface();
    void init(SocketIO* socket, const int t_pin);

    std::string id;
    std::string name;
    std::string actuator;
    std::string ip;
    std::string mac;
    std::string status;
  
  private:
    SocketIO* socket;
    void checkIn();
    int pin;
};

Interface::Interface(){}

void Interface::init(SocketIO* t_socket, const int t_pin)
{
  socket = t_socket;
  pin = t_pin;

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin, OUTPUT);

  socket->on("greetings", [](JsonObject data){
      Debug::printf("[SOCKETIO] onGreetings Callback\n");
      serializeJson(data, Serial);
  });

  socket->on("board:on", [this](JsonObject data){
      digitalWrite(this->pin, HIGH);
  });

  socket->on("board:off", [this](JsonObject data){
      digitalWrite(this->pin, LOW);
  });

  socket->on("connect", [this](JsonObject data){
      Debug::printf("[SOCKETIO] onConnect Callback\n");
      serializeJson(data, Serial);
      this->checkIn();
  });
  
  (*socket).on("error", [](JsonObject data){
      Debug::printf("[SOCKETIO] onError Callback\n");
      serializeJson(data, Serial);
  });
}


//Registers the board into the server.
void Interface::checkIn()
{
    std::map<std::string, std::string> payload = {
        {"id", "botlleFiller"},
        {"name", "Filler"},
        {"actuator", "valve"},
        {"ip", "192.168.3.116"},
        {"status", "off"}
    };

    Debug::printf("Initializing Board");

    socket->send("board:register", payload);
    
}