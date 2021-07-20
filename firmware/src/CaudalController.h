/*
  CaudalController.h - Library for handling YF-S201 measurements
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include "Controller.h"
#include <map>
#include <string>

#ifndef __CaudalController_h
#define __CaudalController_h

class CaudalController: public Controller
{
  public:
    CaudalController():Controller(){};

    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void loop();
    virtual void sense();

    static void flow();
    static volatile int flow_frequency; //variable para la cantidad de pulsos recibidos

  protected:
    uint64_t lastSense = millis();
    uint64_t senseInterval = 1000;
    float conversionFactor=7.5; //para convertir de frecuencia a caudal    
    float volume;
};

volatile int CaudalController::flow_frequency = 0;

void CaudalController::loop()
{
  uint64_t now = millis();
  if(now - lastSense > senseInterval) {
      lastSense = now;
      sense();
  }
}

void IRAM_ATTR CaudalController::flow ()
{ 
  CaudalController::flow_frequency++;  //increment flow pulse
} 

void CaudalController::sense()
{ 
  float lpm=CaudalController::flow_frequency/conversionFactor; //calculamos el caudal en L/m
  float lph=lpm*60; //calculamos el caudal en L/h
  CaudalController::flow_frequency = 0; // Reset Counter
  volume = volume + (lph/3600);


  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "caudal";
  payload["lpm"] = std::to_string(lpm).substr(0,6);
  payload["lph"] = std::to_string(lph).substr(0,6);
  payload["vol"] = std::to_string(volume).substr(0,6);

  socket -> send("board:data", payload);
}

void CaudalController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  volume = 0;
  pinMode(pin, INPUT); 
  attachInterrupt(pin, CaudalController::flow , RISING); //(Interrupcion 0(Pin2),funcion,Flanco de subida)
  sei();
}

#endif