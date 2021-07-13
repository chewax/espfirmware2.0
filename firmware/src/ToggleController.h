#include <Arduino.h>
#include "SocketIO.h"
#include "Controller.h"
#include <map>
#include <string>

#ifndef __ToggleController_h
#define __ToggleController_h

using EventDelegate = std::function<void(std::function<void()>)>;

class ToggleController: public Controller
{
  public:
    ToggleController():Controller(){};
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator, const EventDelegate& suscriber);
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator, const int& t_timeout_ms);
    void loop();
    // const EventDelegate& suscriber = NULL;

  private:
    bool open = false;
    uint64_t timeout_ms = 0;
    uint64_t startTime = millis();
    void close();
    
};

void ToggleController::loop()
{
  if (open) {
    uint64_t now = millis();
    if(now - startTime > timeout_ms) close();
  }
}

void ToggleController::close()
{
  open = false;
  digitalWrite(pin, LOW);

  std::map<std::string, std::string> payload;
  payload["id"] = id;
  payload["type"] = "switch";
  payload["state"] = "off";

  socket -> send("board:data", payload);
}

// Toggle controller functions as an auto close switch whene certain parameters are met
// Currently there are 2 differente ways that a toggle may close. 
// First is the most common timout. 
// Second is listening for an event.
// Wheneber a board:on event is recieved the controller will try to bind itself to a ending event emitter. That emitter can be reset at any time.
// IF suche eventEmitter is not found thent it will rollback to the more common timoout.
void ToggleController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator, const EventDelegate& suscriber)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  // socket->on("board:on", [this](JsonObject data){
  //   digitalWrite(this->pin, HIGH);

  //   //This is tricky and idk how smart it actuall is.
  //   //The idea is receive a delaegate to a function which we will use to suscribe to a halt event.
  //   //Example. We want to close relay when a sensor reaches certain temperature. To compartiment the tasks the sensor will read the temperature and will
  //   //Call all the suscribed delegates upon reaching the target. But this controller does not know anything about that so we need to inform which is such event emitter
  //   //That emitter, in turn, recieves another function which will actually be called upon reaching the break condition.

  //   // if (this->suscriber != NULL) (std::bind(ToggleController::close, this)) ;
  //   // else {
  //   //   this->startTime = millis();
  //   //   open = true;
  //   // }
  // });

  // suscriber(close);
}


void ToggleController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator, const int& t_timeout_ms)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  timeout_ms = t_timeout_ms;

  socket->on("board:on", [this](JsonObject data){
    digitalWrite(this->pin, HIGH);
    this->startTime = millis();
    this->open = true;
  });

}


#endif