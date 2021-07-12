#include <Arduino.h>
#include "Controller.h"
#include "DHT.h"


#ifndef DHTController_h
#define DHTController_h

class DHTController : public Controller
{
  public:
    DHTController();
    virtual void init(String name, String id);
    virtual void sense();
    virtual void handleInput(uint32_t val);

  protected:
    void computeSensorData();
    uint32_t lastInputVal = HIGH;
    DHT* dht;
};

#endif
