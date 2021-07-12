#include <Arduino.h>
#include "Controller.h"


#ifndef MotorController_h
#define MotorController_h

class MotorController : public Controller
{
  public:
    MotorController();
    virtual void init(String name, String id);
    virtual void sense();
    virtual void handleInput(uint32_t val);

  protected:
    String lastKnownState = "unknown";
    uint32_t lastInputVal = HIGH;
    void onStateChange(String newState);
    void computeSensorData();
    uint32_t AUTOOFF_DELAY = 10000;
};

#endif
