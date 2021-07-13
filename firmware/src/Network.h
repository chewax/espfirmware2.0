#ifndef __Network_H
#define __Network_H

#include <ESP8266WiFi.h>
#include <string>
#include <functional>
#include <vector>
#include "Debug.h"

using OnStatusDelegate = std::function<void()>;

class Network
{

private:
  std::string ssid;
  std::string pass;
  std::vector<OnStatusDelegate> onDisconnectListeners;
  std::vector<OnStatusDelegate> onConnectListeners;
  wl_status_t currentStatus;
  long lastLoop;

public:
  Network();
  void quickConnect(const std::string &ssid, const std::string &pass);
  void loop();
  void onConnect(OnStatusDelegate cb);
  void onDisconnect(OnStatusDelegate cb);
};

// Constructor
Network::Network()
{
  currentStatus = wl_status_t::WL_DISCONNECTED;
  lastLoop = 0;
}

//Adds a listener to the onConnectListeners vector
void Network::onConnect(OnStatusDelegate cb)
{
  onConnectListeners.push_back(cb);
}

//Adds a listener to the onDisconnectListeners vector
void Network::onDisconnect(OnStatusDelegate cb)
{
  onDisconnectListeners.push_back(cb);
}

// Sets up a quick connection to as STA to provided WIFI AP
void Network::quickConnect(const std::string &ssid, const std::string &pass)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  Debug::printf("\nConnecting to:\t %s\n", ssid.c_str());

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Debug::printf(".");
  }

  Debug::printf("\nConnected:\t %s", WiFi.SSID().c_str());
  Debug::printf("\nIP Address:\t %s\n", WiFi.localIP().toString().c_str());
  Debug::printf("\nMAC Address:\t %s\n", WiFi.macAddress().c_str());

  //Store quickConnect settings for future reference
  this->ssid = ssid;
  this->pass = pass;
}

// Gets loop handler.
// Checks for network health and calls listeners as needed
void Network::loop()
{
  if (millis() < lastLoop + 1000) return; //Accept loop only every 1 second;
  lastLoop = millis();

  if (WiFi.status() == currentStatus) return;

  // Means it was connected but connection has been lost
  // Must call all onDisconnectListneres
  if (WiFi.status() == wl_status_t::WL_DISCONNECTED)
    for (const auto &it:onDisconnectListeners) it();

  // Means it was disconnected or idle but connection has been established
  // Must call all onConnectListeners
  if (WiFi.status() == wl_status_t::WL_CONNECTED)
    for (const auto &it:onConnectListeners) it();

  currentStatus = WiFi.status();
}


#endif