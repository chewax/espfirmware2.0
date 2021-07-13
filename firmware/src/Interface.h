/*
  Board.h - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include <string>
#include <map>
#include "DHT.h"

#ifndef __Interface_h
#define __Interface_h
#endif

class Interface
{
  public:
    Interface();
    void init(SocketIO* socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void loop();

    std::string id;
    std::string name;
    std::string actuator;
    std::string ip;
    std::string mac;
    std::string status;
  
  private:
    SocketIO* socket;
    DHT* dht;
    int pin;
    void checkIn();
    void sensorLoop();
    void senseDHT();
    uint64_t lastSense = millis();
    uint64_t senseInterval = 10000;

};

Interface::Interface(){}

void Interface::senseDHT()
{
    float h = dht->readHumidity();
    float t = dht->readTemperature();
    float hic = dht->computeHeatIndex(t, h, false); 

    std::map<std::string, std::string> payload;
    payload["id"] = id;
    payload["type"] = "dht";
    payload["temperature"] = std::to_string(t).substr(0,4);
    payload["humidity"] = std::to_string(h).substr(0,4);
    payload["hic"] = std::to_string(hic).substr(0,4);

    socket -> send("board:sense", payload);
}

void Interface::sensorLoop()
{
  //HACK: Use Enums
  if (actuator == "dht") senseDHT();
}

void Interface::loop()
{
  uint64_t now = millis();

  if(now - lastSense > senseInterval) {
      lastSense = now;
      if (actuator == "dht") sensorLoop();
  }
}

void Interface::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin, OUTPUT);
  
  socket = t_socket;
  pin = t_pin;

  if (t_actuator == "dht") 
  {
    dht = new DHT(pin, DHT11);
    dht->begin();
  }


  ip = WiFi.localIP().toString().c_str();
  mac = WiFi.macAddress().c_str();
  digitalWrite(pin, LOW);
  status = "off";
  actuator = t_actuator;
  name = t_name;
  id =  pin + "@" + mac;

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
  
  socket->on("error", [](JsonObject data){
      Debug::printf("[SOCKETIO] onError Callback\n");
      serializeJson(data, Serial);
  });
}


//Registers the board into the server.
void Interface::checkIn()
{
    std::map<std::string, std::string> payload = {
        {"id", id.c_str()},
        {"name", name.c_str()},
        {"actuator", actuator.c_str()},
        {"ip", ip.c_str()},
        {"status", status.c_str()}
    };

    Debug::printf("Initializing Board");
    socket->send("board:register", payload);
}