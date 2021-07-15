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
#include "src/CaudalController.h"
#include <map>
#include <string>

//Pins Available in ESP-01s Board
#define D4 2
#define D3 0

//Rest of pins
#define D2 4
#define D1 5

//Network Configuration
#define Wifi_SSID "Eolio_2G"
#define Wifi_PASS "7Chandrian"

//SocketIO Configuration
#define IOBoard_ID "amazing-possum"
#define IOServer_HOST "192.168.3.105"
#define IOServer_Port 3000

Network network;
SocketIO socket;

Interface luzFondo;
Interface temp;
// Interface riegoCantero;
// Interface relay;
// Interface caudal;
// Interface toggle;


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

    network.quickConnect(Wifi_SSID, Wifi_PASS);

    // riegoCantero.init(&socket, D1, "Riego Cantero", "valve");
    temp.init(&socket, D4, "Temp", "dht");
    luzFondo.init(&socket, D3, "Luz Fondo", "bulb");


    // TOGGLE USAGE EXAMPLE
    // toggle.init(&socket, D3, "1.5L", "toggle");
    // ((ToggleController*)(toggle.ifController))->closeWhen("fill:complete");

    //FILLER USAGE EXAMPLE
    // caudal.init(&socket, D2, "Caudal", "filler");
    // ((FillController*)(caudal.ifController))->setMililiters(1500);
    // ((FillController*)(caudal.ifController))->fillWhen("toggle:" + toggle.ifController->id + ":on");
    
    // relay.init(&socket, D3, "Button", "switch");
    socket.init(IOBoard_ID, IOServer_HOST, IOServer_Port);
}

uint64_t messageTimestamp = millis();

void loop()
{
    socket.loop();
    network.loop();

    // caudal.loop();
    // toggle.loop();
    temp.loop();
}

void initSerial()
{
    Serial.begin(9600);
    Serial.setDebugOutput(true);
}


