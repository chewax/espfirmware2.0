/*
  Board.h - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include "SimpleTimer.h"

#ifndef Controller_h
#define Controller_h

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define A0 17
#define RX 3
#define TX 1

class Controller
{
  public:
    Controller();

    String id;
    String name;

    virtual void init(String name, String id);
    virtual void defaultAction(bool notifyServer=false);
    virtual void setOn(bool notifyServer=false);
    virtual void setOff(bool notifyServer=false);
    virtual void sense();
    virtual void handleInput(uint32_t val);
    virtual void loop();

    void toggle();
    void toggle(bool notifyServer=false);
    void setIOHandler(SocketIO* io);
    uint32_t relayState;


  protected:
    
    uint32_t inputA;
    
    uint32_t relayA;
    uint32_t relayB;

    uint32_t sensorA;
    uint32_t sensorB;
    uint32_t sensorC;
    uint32_t sensorD;
    
    uint32_t sensorA0;

    uint32_t switchState;

    SocketIO* io;
    uint64_t loopTimestamp = 0;
    uint64_t pollTimestamp = 0;
    uint32_t LOOP_INTERVAL = 50;
    uint32_t POLL_INTERVAL = 5000;
    bool poll_available = false;
    bool initialized = false;
    SimpleTimer timer;
};

#endif
