
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
    int mVperAmp = 66; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module
    double Voltage = 0;
    double VRMS = 0;
    double AmpsRMS = 0;
    float getVPP();
    // ACS712* acs;
};



void CurrentController::loop()
{
   sense();
}


void CurrentController::sense()
{

  // int V = 230;
  // int mA = acs->mA_AC();
  // float A = mA / 1000;
  // float W = A * V;

  // Debug::printf("A: %d | V: %d | W: %d \n", A, V, W);
  // Serial.print("mA: ");
  // Serial.print(mA);
  // // Serial.print(". Form factor: ");
  // // Serial.println(acs->getFormFactor());


   Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707; // sq root
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 float Wattage = (220*AmpsRMS)-18; //Observed 18-20 Watt when no load was connected, so substracting offset value to get real consumption.
 Serial.print(AmpsRMS);
 Serial.println(" Amps RMS ");
 Serial.print(Wattage); 
 Serial.println(" Watt ");


}


float CurrentController::getVPP()
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();

   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(A0);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
/*       Serial.print(readValue);
       Serial.println(" readValue ");
       Serial.print(maxValue);
       Serial.println(" maxValue ");
       Serial.print(minValue);
       Serial.println(" minValue ");
       delay(1000); */
    }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5)/1024.0;
      
   return result;
 }

void CurrentController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  pinMode(A0, INPUT);

  // // ACS712 5A  uses 185 mV per A
  // // ACS712 20A uses 100 mV per A
  // // ACS712 30A uses  66 mV per A
  // acs = new ACS712(A0, 3.3, 1023, 66);

  // acs->autoMidPoint();
  // Serial.print("MidPoint: ");
  // Serial.print(acs->getMidPoint());
  // Serial.print(". Noise mV: ");
  // Serial.println(acs->getNoisemV());
}

#endif