/*
  ESP8266 Module Firmware for Home Automation
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include "src/Debug.h"
#include "src/Network.h"
#include "src/SocketIO.h"

Network network;
SocketIO socket;

// the setup function runs once when you press reset or power the board
void setup()
{
    initSerial(); //Initialize serial so it can be used to print
    
    // network.onConnect([](){
    //     Debug::printf("Connect Callback");
    // });

    // network.onDisconnect([](){
    //     Debug::printf("Disconnect Callback");
    // });

    network.quickConnect("Eolio_2G", "7Chandrian");

    socket.on("greetings", [](JsonObject data){
        Debug::printf("onGreetings Callback\n");
    });
    
    socket.init("dani-test");
}

uint64_t messageTimestamp = millis();

void loop()
{
    socket.loop();
    network.loop();

    // if (millis() > now + 2000) {
    //     now = millis();
        
    //     // creat JSON message for Socket.IO (event)
    //     const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3);
    //     DynamicJsonDocument doc(capacity);

    //     doc.add("sensor_reading"); //Add event name
    //     JsonObject doc_1 = doc.createNestedObject();
    //     doc_1["sender"] = "dani";
    //     doc_1["reading1"] = "2,33";
    //     doc_1["reading2"] = "off";

    //     socket.emit(doc);
    // } 

    uint64_t now = millis();

    if(now - messageTimestamp > 2000) {
        messageTimestamp = now;

        // Send event        
        socket.quickSend("test-event", "reading1", "2.99");
    }
}

void initSerial()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
}

