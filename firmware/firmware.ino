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
#define IOBoard_ID "impossible-burger"
#define IOServer_HOST "192.168.3.195"
#define IOServer_Port 3000

//Scene name to let know the frontend how to group all boards.
//All boards from the same scene will be put together in different views
#define SCENE "Casita"

Network network;
SocketIO socket(SCENE); 

Interface luzFondo;
// Interface localButton;
// Interface temp;
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

    
    // DIGITAL INPUT CONTROLLING A RELAY EXAMPLE;
    //=========================================================================
    luzFondo.init(&socket, D3, "Shed", "bulb");
    // localButton.init(&socket, D2, "Local Button", "digitalInput");
    luzFondo.ifController->onWhen( "bulb:shed:on", true );
    luzFondo.ifController->offWhen( "bulb:shed:off", true );
    //=========================================================================


    // TOGGLE & FILLER USAGE EXAMPLE
    //=========================================================================
    // toggle.init(&socket, D3, "1.5L", "toggle");
    // caudal.init(&socket, D2, "Caudal", "filler");

    // ((FillController*)(caudal.ifController))->setMililiters(1500);
    // toggle.ifController->offWhen( caudal.ifController->evt_end );
    // caudal.ifController->onWhen( toggle.ifController->evt_start );
    //=========================================================================

    // OTHER EXAMPLES
    //=========================================================================
    // riegoCantero.init(&socket, D1, "Riego Cantero", "valve");
    // temp.init(&socket, D4, "Shed", "dht");
    // relay.init(&socket, D3, "Button", "bulb");
    //=========================================================================

    socket.init(IOBoard_ID, IOServer_HOST, IOServer_Port);
}

uint64_t messageTimestamp = millis();

void loop()
{
    socket.loop();
    network.loop();

    // caudal.loop();
    // toggle.loop();
    // temp.loop();
    // localButton.loop();
    // luzFondo.loop();
}

void initSerial()
{
    Serial.begin(9600);
    Serial.setDebugOutput(true);
}


