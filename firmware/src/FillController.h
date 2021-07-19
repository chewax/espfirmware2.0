/*
  FillController.h - Library for using YF-S201 for measuring specific amounts of liquids
  Created by Daniel Waksman, July, 14, 2021.
  Released into the public domain.
*/

#ifndef __FillController_h
#define __FillController_h

#include <Arduino.h>
#include "SocketIO.h"
#include "CaudalController.h"
#include <map>
#include <string>
#include "EventEmitter.h"


class FillController: public CaudalController
{
  public:
    FillController():CaudalController(){};

    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void sense();
    void setMililiters(int ml);
    void fillWhen(const std::string& event);
  
  private: 
    int ml;
    bool filling = false;
    void fill();
};


void FillController::fill()
{ 
  filling = true;
  volume = 0;
}

void FillController::fillWhen(const std::string& event)
{
  EventEmitter::on(event, [this](){ this->fill(); });
}


void FillController::sense()
{ 
  CaudalController::sense();

  if (filling && (volume*1000) >= ml) {
    filling = false;
    std::string evt = "fill:"+ this->id +":complete";
    EventEmitter::emit(evt);
  }
}

void FillController::setMililiters(int t_ml)
{ 
  ml = t_ml;
}

void FillController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  CaudalController::init(t_socket, t_pin, t_name, t_actuator);
}


#endif