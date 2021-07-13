/*
  Board.h - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "SocketIO.h"
#include "Interface.h"
#include <string>
#include <vector>

#ifndef __Interface_h
#define __Interface_h


#define D3 0
#define D4 2

class Board
{
  public:
    Board();
    void loop();
    void init(const SocketIO& socket, const int gpio);

    std::string ip;
    std::string mac;

    std::vector<Interface> actuators;
  
  private:
    SocketIO socket;
    void checkIn();
};

#endif