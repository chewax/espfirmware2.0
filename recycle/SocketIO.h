/*
  SocketIO.h - Library for handling SocketIO events
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

#ifndef SocketIO_h
#define SocketIO_h

typedef void (*SocketEvent_String) (String);
typedef void (*SocketEvent) ();

    class SocketIO 
    {
        public:
            SocketIO();
            void init(String host, uint16_t port);
            void loop();
            void onMessage(SocketEvent_String _cb);
            void onConnect(SocketEvent _cb);
            void onDisconnect(SocketEvent _cb);
            void sendJSON(String RID, String JSON);
            void sendMessage(String msg);

          private:
            WebSocketsClient _webSocket;
            SocketEvent_String _onMessage;
            SocketEvent _onConnect;
            SocketEvent _onDisconnect;
            String _host;
            bool _isConnected = false;
            uint32_t _port;
            void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
            void heartbeat();
            void parseSocketPayload(uint8_t * payload);
            void parseMessage(String msg);
            String _RID;
            String _Rname;
            String _Rcontent;
            uint64_t _heartbeatTimestamp = 0;
            uint32_t _HEARTBEAT_INTERVAL = 25000;

    };

#endif