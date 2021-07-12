/*
  ESP8266 Module Firmware for Home Automation
  Created by Daniel Waksman, August 17, 2018.
  Released into the public domain.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <Time.h>
#include <TimeLib.h>
#include "SocketIO.h"
#include "Manager.h"
#include "Utils.h"

#define USE_SERIAL Serial
#define WIFI_RECONNECT_TIMEOUT 60000


ESP8266WebServer httpServer(80);
DNSServer dnsServer;

/* Set these to your desired softAP credentials. They are not configurable at runtime */
char *softAP_ssid = "SH_SSID"; 
char *softAP_password = "12345678";

/* Soft AP network parameters */
IPAddress apIP(192, 168, 50, 1);
IPAddress netMsk(255, 255, 255, 0);
const byte DNS_PORT = 53;

uint64_t messageTimestamp = 0;
bool checkInOnConnect = true;
bool _pollState = false;
uint64_t _pollTimestamp = 0;

/** Should I connect to WLAN asap? */
boolean connect;

/** Last time I tried to connect to WLAN */
long lastConnectTry = 0;

SocketIO io;
Manager Manager;
ConfigStruct config;

//Setup
void setup()
{
    initializeSerial();         //Initialize serial so it can be used to print
    config = Utils::loadConfig();
    initManager();              //Initialize board with EEPROM config or defaults
    initializeWiFi();           //Initialize Station and Access Point Modes
    initializeWebServer();  
    initializeSocketIO();       //Establish SocketIO connection with server
}

//Loop
void loop()
{
    io.loop();
    dnsServer.processNextRequest();
    httpServer.handleClient();
    Manager.loop();

    if (connect)
    {   
        Utils::logger("Reconnect attempt", "WLAN");
        //Attempt to connect/reconnect
        //Reset lastReconnectTime
        lastConnectTry = millis();
        connect = false;

        //If length ssid == 0 then no config is available --> Dont attempt to connect;
        if (strlen(config.ssid) > 0)
        {
            Utils::logger("No SSID selected", "WLAN");
            connectToWifi();
        }
    }
    else
    {

        if (WiFi.status() == WL_IDLE_STATUS && millis() > (lastConnectTry + WIFI_RECONNECT_TIMEOUT))
        {
            /* If WLAN disconnected and idle try to connect */
            /* Don't set retry time too low as retry interfere the softAP operation */
            connect = true;
        }
    }

    //HTTP
    httpServer.handleClient();
}

void initManager()
{
    logger("Initializing manager", "SETUP");
    Manager.init(config.name, WiFi.macAddress(), String(config.mode));
}

//Initialize SocketIO
void initializeSocketIO()
{
    Utils::logger("Intializing SocketIO", "SETUP");

    String host(config.server);

    io.init(host, 5000);
    io.onMessage(handleAction);
    io.onConnect(onConnection);
    io.onDisconnect(onDisconnection);
}

//Registers the board into the server.
void checkIn()
{
    
    String result;

    result += asProperty("id", Manager.board->id, true);
    result += asProperty("name", Manager.board->name, true);
    result += asProperty("mode", Manager.mode, true);
    result += asProperty("ip", toStringIp(WiFi.localIP()), true);
    result += asProperty("relayState", String(Manager.board->relayState), false);

    result = asJSONObj(result);

    Utils::debug("Intializing Board");

    io.sendJSON("board:register", result);

    Manager.board->sense();
}

//Handles action that was parsed from the socket message
void handleAction(String action)
{
    Manager.handleAction(action);
}

void onConnection()
{   

    Manager.setIOHandler(&io);

    if (checkInOnConnect)
    {
        checkIn();
        checkInOnConnect = false;
    }

}

void onDisconnection()
{
    checkInOnConnect = true;
}