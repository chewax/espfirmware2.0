/*
  SocketIO.h - Library for handling SocketIO events
  Created by Daniel Waksman, May 7, 2020.
  Released into the public domain.
*/
#ifndef __SocketIO_H
#define __SocketIO_H

#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>
#include <vector>
#include <map>
#include <functional>
#include "Debug.h"

typedef std::function<void(JsonObject)> IOEventListener;
typedef std::map<std::string, std::vector<IOEventListener>> TEventListeners;


class SocketIO
{
private:
  mutable SocketIOclient webSocket;
  TEventListeners eventListeners;
  void onDisconnect();
  void onConnect(uint8_t *payload);
  void onEvent(uint8_t *payload);
  std::string board_id;

public:
  SocketIO();
  void init(const std::string &t_board_id);
  void IOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length);
  void loop();
  void on(const std::string &event, IOEventListener listener);
  // void emit(const DynamicJsonDocument &doc);
  void quickSend(const std::string &event, const std::string& data_key, const std::string& data_value) const;
};

SocketIO::SocketIO()
{
}

void SocketIO::init(const std::string &t_board_id)
{
  board_id = t_board_id;
  webSocket.begin("192.168.2.111", 3000);
  webSocket.onEvent(std::bind(&SocketIO::IOEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

// Adds a listener to a specific event
void SocketIO::on(const std::string &event, IOEventListener listener)
{
  eventListeners[event].push_back(listener);
}

// Calls all listeners for onConnect event
void SocketIO::onConnect(uint8_t *payload)
{
  JsonObject root; // allocate the void JsonDocument
  Debug::printf("[SocketIO] Connected: %s\n", payload);

  std::vector<IOEventListener> handlers = eventListeners["connect"];
  for (const auto &it:handlers) it(root);
}

// Calls all listeners for onEvent event
void SocketIO::onEvent(uint8_t *payload)
{
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);

  const char* event = doc[0]; // "event"
  JsonObject eventData = doc[1];

  Debug::printf("[SocketIO] Event: %s\n", event);

  std::vector<IOEventListener> handlers = eventListeners[event];
  for (const auto &it:handlers) it(eventData);
}

// Calls all listeners for onDisconnect event
void SocketIO::onDisconnect()
{
  JsonObject root; // allocate the void JsonDocument
  Debug::printf("[SocketIO] Disconnected!\n");

  std::vector<IOEventListener> handlers = eventListeners["disconnect"];
  for (const auto &it:handlers) it(root);
}

// void SocketIO::emit(const DynamicJsonDocument &doc)
void SocketIO::quickSend(const std::string &event, const std::string& data_key, const std::string& data_value) const
{
  std::string json = "[ \""+ event +"\", \{ \""+ data_key +"\": \""+ data_value +"\" \} ]";
  Debug::printf("%s\n", json);
  // std::sprintf(json, "[ \"%s\", \{ \"%s\": \"%s\" \} ]", event.c_str(), data_key.c_str(), data_value.c_str());
  // const char* json = "[\"readings\",{\"sender\":\"dani-tes\",\"reading1\":\"2,33\"}]";
  webSocket.sendEVENT(json.c_str());

  // Print JSON for debugging
  // Debug::printf("%s\n", output.c_str());
  // Debug::printf("%s\n", json);
}

void SocketIO::loop()∂
{
  webSocket.loop();
}

void SocketIO::IOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            onDisconnect();
            break;
        case sIOtype_CONNECT:
            onConnect(payload);
            break;
        case sIOtype_EVENT:
            onEvent(payload);
            break;
    }
}

#endif