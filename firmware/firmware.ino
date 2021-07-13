/*
  ESP8266 Module Firmware for Home Automation
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "src/Debug.h"
#include "src/Network.h"
#include "src/SocketIO.h"
#include "src/Interface.h"
#include <map>
#include <string>

Network network;
SocketIO socket;
Interface filler;
Interface temp;

#define D4 2
#define D3 0


// the setup function runs once when you press reset or power the board
void setup()
{
    initSerial(); //Initialize serial so it can be used to print
    
    network.onConnect([](){
        Debug::printf("Connect Callback\n");
    });

    network.onDisconnect([](){
        Debug::printf("Disconnect Callback\n");
    });

    network.quickConnect("Eolio_2G", "7Chandrian");

    filler.init(&socket, D3, "Fill", "valve");
    temp.init(&socket, D4, "Temp", "dht");

    socket.init("dani-test");
}

uint64_t messageTimestamp = millis();

void loop()
{
    socket.loop();
    network.loop();
    filler.loop();
    temp.loop();
}

void initSerial()
{
    Serial.begin(9600);
    Serial.setDebugOutput(true);
}


