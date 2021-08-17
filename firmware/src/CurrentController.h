
#ifndef __CurrentController_h
#define __CurrentController_h

#include <Arduino.h>
#include "SocketIO.h"
#include "DHT.h"
#include "Controller.h"
#include <map>
#include <string>
#include "ACS712.h"

class CurrentController: public Controller
{
  public:
    CurrentController():Controller(){};

    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    void loop();
    void sense();

  protected:
    ACS712* acs;
};



void CurrentController::loop()
{
   sense();
}


void CurrentController::sense()
{

  // int V = 230;
  int mA = acs->mA_AC();
  // float A = mA / 1000;
  // float W = A * V;

  // Debug::printf("A: %d | V: %d | W: %d \n", A, V, W);
  // Debug::printf("mA: %d \n", mA);
  Serial.print("mA: ");
  Serial.print(mA);
  Serial.print(". Form factor: ");
  Serial.println(acs->getFormFactor());

}




void CurrentController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  // pinMode(A0, INPUT);

  // ACS712 5A  uses 185 mV per A
  // ACS712 20A uses 100 mV per A
  // ACS712 30A uses  66 mV per A
  acs = new ACS712(A0, 3.3, 1024, 66);

  acs->autoMidPoint();
  Serial.print("MidPoint: ");
  Serial.print(acs->getMidPoint());
  Serial.print(". Noise mV: ");
  Serial.println(acs->getNoisemV());
}

#endif