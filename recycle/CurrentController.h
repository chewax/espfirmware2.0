#include <Arduino.h>
#include "Controller.h"


#ifndef CurrentController_h
#define CurrentController_h

class CurrentController : public Controller
{
  public:
    CurrentController();
    virtual void init(String name, String id);
    virtual void sense();
    virtual void handleInput(uint32_t val);

  protected:
    String lastKnownState = "unknown";
    uint32_t lastInputVal = HIGH;
    void onStateChange(String newState);
    void computeSensorData();
    uint32_t AUTOOFF_DELAY = 10000;
    int ACSoffset = 2500;
    float vpp = 0;

    // int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
    // int RawValue= 0;
    // double Voltage = 0;
    // double Amps = 0;
    // float vpp = 0;
};

#endif
