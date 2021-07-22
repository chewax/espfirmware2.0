#ifndef __Network_H
#define __Network_H

#include <ESP8266WiFi.h>
#include <string>
#include <functional>
#include <vector>
#include "Debug.h"
#include "IPAddress.h"
#include "Utils.h"
#include <DNSServer.h>
#include "HTTPServer.h"

using OnStatusDelegate = std::function<void()>;


class Network
{

private:
  std::vector<OnStatusDelegate> onDisconnectListeners;
  std::vector<OnStatusDelegate> onConnectListeners;
  wl_status_t currentStatus;
  long lastLoop;

  /* Set these to your desired softAP credentials. They are not configurable at runtime */
  char *softAP_ssid = "SH_SSID"; 
  char *softAP_password = "12345678";

  /* Soft AP network parameters */
  IPAddress apIP;
  IPAddress netMsk;
  const byte DNS_PORT = 53;
  ConfigStruct config;
  DNSServer dnsServer;
  HTTPServer* httpServer;

public:
  Network();
  ~Network();
  void quickConnect(const std::string &ssid, const std::string &pass);
  void startAP();
  void init();
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

Network::~Network(){
  delete httpServer;
}

//Initializes network from EEPROM configuration.
//If no configuartion is found then initializes an AP to be able to be configured from the web page
void Network::init(){
  config = Utils::loadConfig();
  
  if (strlen(config.ssid) > 0) quickConnect(config.ssid, config.password);
  else startAP();

  httpServer = new HTTPServer(80);
  httpServer->init();
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

void Network::startAP()
{   
    Debug::line();
    Debug::printf("[WIFI_AP] No EEPROM config found\n");
    Debug::printf("[WIFI_AP] Starting...\n");
    

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.hostname("espconfig");

    apIP.fromString("192.168.50.1");
    netMsk.fromString("255.255.255.0");

    if (config.name) strcat(softAP_ssid, config.name);
    else strcat(softAP_ssid, WiFi.macAddress().c_str());

    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(softAP_ssid, softAP_password);

    delay(500); // Without delay I've seen the IP address blank

    Debug::line();
    Debug::printf("[WIFI_AP] SoftAP Configured:\n");
    Debug::printf("[WIFI_AP] SSID:\t %s\n", softAP_ssid);
    Debug::printf("[WIFI_AP] IP:\t %s\n", "192.168.50.1");
    Debug::line();

    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", apIP);
}

// Sets up a quick connection to as STA to provided WIFI AP
// Is an alternative to using full wifi provisioning for quick tests
// dont use if you are calling init function
void Network::quickConnect(const std::string &ssid, const std::string &pass)
{ 

  Debug::line();
  Debug::printf("[WIFI_STA] Starting...\n");

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  Debug::printf("\nConnecting to:\t %s\n", ssid.c_str());

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Debug::printf(".");
  }

  Debug::line();
  Debug::printf("[WIFI_STA] SoftAP Configured:\n");
  Debug::printf("[WIFI_STA] IP:\t %s\n", "192.168.50.1");
  Debug::printf("[WIFI_STA] MAC Address:\t %s\n", WiFi.macAddress().c_str());
  Debug::line();
}

// Gets loop handler.
// Checks for network health and calls listeners as needed
void Network::loop()
{ 
  dnsServer.processNextRequest();
  httpServer->loop();

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