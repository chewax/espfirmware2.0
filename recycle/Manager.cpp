/*
  board->cpp - Library for handling ESP8266 Board (Relay/DHT)
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "Manager.h"
#include "Controller.h"
#include "MotorController.h"
#include "DHTController.h"
#include "ValveController.h"
#include "Utils.h"

//Constructor
Manager::Manager()
{

}

void Manager::init(String name, String macAddress, String mode)
{
    Utils::logger("Intializing Board Manager", "SETUP");

    this->macAddress = macAddress;
    this->mode = mode;


    if ( mode == "motor")
    {
        board = new MotorController();
    }
    else if ( mode == "dht")
    {
        board = new DHTController();
    }
    else if ( mode == "valve")
    {
        board = new DHTController();
    }
    else
    {
        board = new Controller();   
    }

    board->init(name, macAddress);
}

//Handles action that was parsed from the socket message
void Manager::handleAction(String action)
{
    if (action == "setOn") board->setOn();
    if (action == "setOff") board->setOff();
}

void Manager::loop ()
{   
    board->loop();
}

void Manager::setIOHandler(SocketIO* io)
{
    this->io = io;
    board->setIOHandler(io);
}