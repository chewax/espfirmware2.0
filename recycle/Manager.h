/*
  Board.h - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "Controller.h"
#include "SocketIO.h"

#ifndef Manager_h
#define Manager_h

class Manager
{
  public:
    Manager();
    void init(String name, String macAddress, String mode);
    void handleAction(String action);
    void loop();
    Controller* board;
    String mode;
    void setIOHandler(SocketIO* io);

  protected:
    String macAddress;
    SocketIO* io;
};

#endif
