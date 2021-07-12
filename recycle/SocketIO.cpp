/*
  SocketIO.cpp - Library for handling SocketIO events
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include "SocketIO.h"
#include "Utils.h"

// #define USE_SERIAL Serial

//Empty constructor for globally declaring purposes
SocketIO::SocketIO()
{
    _isConnected = false;
}

//Init function, to actually intilalize all needed variables and events
void SocketIO::init(String host, uint16_t port)
{
    _host = host;
    _port = port;

    _webSocket.beginSocketIO(_host, _port);
    _webSocket.onEvent(std::bind(&SocketIO::webSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void SocketIO::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            
            Utils::logger("Disconnected :(", "SOCKET");
            // USE_SERIAL.printf("[SOCKET] Disconnected!\n");
            _isConnected = false;
            
            if (_onDisconnect) _onDisconnect();
            break;

        case WStype_CONNECTED:
            {
                Utils::logger("Connected :):)", "SOCKET");
                // USE_SERIAL.printf("[SOCKET] Connected to: %s\n",  payload);
                _isConnected = true;

			    // send message to server when Connected
                // socket.io upgrade confirmation message (required)
				_webSocket.sendTXT("5");

                if (_onConnect) _onConnect();
            }
            break;

        case WStype_TEXT:
            
            // USE_SERIAL.printf("[SOCKET] get text: %s\n", payload);
            
            parseSocketPayload(payload);
            parseMessage(_RID);
            
            break;

        case WStype_BIN:
            // USE_SERIAL.printf("[SOCKET] get binary length: %u\n", length);        
            hexdump(payload, length);

            // send data to server
            _webSocket.sendBIN(payload, length);
            break;

        }
}

//Sends heartbeat signal to the server when time is due
//Interval is set in #defines
void SocketIO::heartbeat () {
    if(_isConnected) {

        uint64_t now = millis();

        if((now - _heartbeatTimestamp) > _HEARTBEAT_INTERVAL) {
            _heartbeatTimestamp = now;
            // socket.io heartbeat message
            _webSocket.sendTXT("2");
        }
    }
}

//Adds header to JSon and sends as message
//PARAM: name (String) the message action
//PARAM: JSON (String) the json
void SocketIO::sendJSON (String name, String JSON) {

    if (!_isConnected)
        return;

    String message = "42[\"" + name + "\"," + JSON + "]";
    Utils::debug(message);
    // USE_SERIAL.println(message);
    _webSocket.sendTXT(message);

}


void SocketIO::sendMessage (String msg) {
    if (!_isConnected)
        return;

    String message = "42[\"" + msg + "\"]";
    Utils::debug(message);
    // USE_SERIAL.println(message);
    _webSocket.sendTXT(message);
}

//Parse socket message
void SocketIO::parseMessage (String msg) {

    String module = msg.substring(0, msg.indexOf(":"));

    //Is a message for web client
    if (module == "client") return;
    
    String action = msg.substring(msg.indexOf(":") + 1, msg.length() - 2);

    _onMessage(action);
}

void SocketIO::onMessage(SocketEvent_String _cb) {
    _onMessage = _cb;
}

void SocketIO::onConnect(SocketEvent _cb) {
    _onConnect = _cb;
}

void SocketIO::onDisconnect(SocketEvent _cb) {
    _onDisconnect = _cb;
}

void SocketIO::parseSocketPayload(uint8_t * payload) {
   String rcvdmsg = (char*) payload;
   _RID = rcvdmsg.substring(4, rcvdmsg.indexOf("\","));
   _Rname = rcvdmsg.substring(rcvdmsg.indexOf("\",") + 4, rcvdmsg.indexOf("\":"));
   _Rcontent = rcvdmsg.substring(rcvdmsg.indexOf("\":") + 3, rcvdmsg.indexOf("\"}"));
}

void SocketIO::loop() {
    _webSocket.loop();
    heartbeat();
}