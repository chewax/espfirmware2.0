#ifndef __OTA_h
#define __OTA_h

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <string>
#include "Debug.h"

class OTA {
  private: 
    std::string hostname;
  public:
    OTA(const std::string& t_hostname);
    void init();
    void loop();
};


OTA::OTA(const std::string& t_hostname){
  hostname = t_hostname;
  ArduinoOTA.setHostname(hostname.c_str());

  ArduinoOTA.onStart([]() {
    std::string type;
    
    if (ArduinoOTA.getCommand() == U_FLASH) type = "sketch";
    else type = "filesystem";
    
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Debug::printf("Start OTA Update %s", type);
  });

  ArduinoOTA.onEnd([]() {
    Debug::printf("\nFinished OTA Update");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Debug::printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
};

void OTA::init() {
  ArduinoOTA.begin();
}

void OTA::loop() {
  ArduinoOTA.handle();
}

#endif