/*
  ESP8266 Module Firmware for Home Automation
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "src/Debug.h"
#include "src/Network.h"
#include "src/SocketIO.h"
#include <map>
#include <string>

#define D3 0
#define D4 2

Network network;
SocketIO socket;

// the setup function runs once when you press reset or power the board
void setup()
{
    initSerial(); //Initialize serial so it can be used to print
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(D4, OUTPUT);

    network.onConnect([](){
        Debug::printf("Connect Callback\n");
    });

    network.onDisconnect([](){
        Debug::printf("Disconnect Callback\n");
    });

    network.quickConnect("Eolio_2G", "7Chandrian");

    socket.on("greetings", [](JsonObject data){
        Debug::printf("[SOCKETIO] onGreetings Callback\n");
        serializeJson(data, Serial);
    });

    socket.on("board:on", [](JsonObject data){
        digitalWrite(D4, HIGH);
    });

    socket.on("board:off", [](JsonObject data){
        digitalWrite(D4, LOW);
    });

    socket.on("connect", [](JsonObject data){
        Debug::printf("[SOCKETIO] onConnect Callback\n");
        serializeJson(data, Serial);
        checkIn();
    });
    
    socket.on("error", [](JsonObject data){
        Debug::printf("[SOCKETIO] onError Callback\n");
        serializeJson(data, Serial);
    });

    socket.init("dani-test");
}

uint64_t messageTimestamp = millis();

void loop()
{
    socket.loop();
    network.loop();

    // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    // digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
    // delay(1000);                       // wait for a second
    // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    // digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
    // delay(1000);    

    uint64_t now = millis();

    if(now - messageTimestamp > 10000) {
        messageTimestamp = now;

        // Send event        
        socket.quickSend("board:sense", "reading1", "2.99");
    }
}

void initSerial()
{
    Serial.begin(9600);
    Serial.setDebugOutput(true);
}

//Registers the board into the server.
void checkIn()
{
    std::map<std::string, std::string> payload = {
        {"id", "botlleFiller"},
        {"name", "Filler"},
        {"actuator", "valve"},
        {"ip", "192.168.3.116"},
        {"status", "off"}
    };

    Debug::printf("Initializing Board");

    socket.send("board:register", payload);
    
}

