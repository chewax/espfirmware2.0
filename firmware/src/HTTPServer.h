#ifndef __HTTPServer_H
#define __HTTPServer_H

#include <ESP8266WebServer.h>
#include <Arduino.h>
#include "Utils.h"
#include <string>

class HTTPServer: public ESP8266WebServer
{

private:

  std::string style = "<style> body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; } </style>";
  std::string meta = "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  std::string title = "<title>SmartH</title>";
  std::string navigation = "";
  std::string footer = "<footer>" + navigation + "</footer>";
  std::string scripts = "";

  std::string startHTML = "<html> <head>" + meta + title + style + "</head> <body>";
  std::string endHTML = scripts + "</body>" + footer + "</html>";

  void handleDeviceSettings();  
public:
  HTTPServer(int port);
  void init();
  void loop();
};

// Constructor
HTTPServer::HTTPServer(int port): ESP8266WebServer(port)
{
  
}

void HTTPServer::handleDeviceSettings()
{   

    std::string html = startHTML;

    html += "<h1>SmartH - Home Automation</h1>";
    html += "<h2>Device Settings</h2>";

    uint32_t n = WiFi.scanNetworks();

    html += "<table>";
    html += "<tbody>";
    html += "<tr>";
    html += "<td>Connected to:</td>";
    html += "<td>" + std::string(WiFi.SSID().c_str()) + "</td>";
    html += "</tr>";
    html += "<tr>";
    html += "<td>IP:</td>";
    html += "<td>" + std::string(WiFi.localIP().toString().c_str()) + "</td>";
    html += "</tr>";
    html += "</tbody>";
    html += "</table>";

    html += "<form method='POST' action='settingssave'>";
    html += "<table>";

    //ROW ====================
    html += "<tr>";
    html += "<td>";
    html += "Network:";
    html += "</td>";
    html += "<td>";

    html += "<select name=\"ssid\">";

    if (n > 0)
    {
        for (uint32_t i = 0; i < n; i++)
        {
            html += "<option value = \"" + std::string(WiFi.SSID(i).c_str()) + "\">";
            html += std::string(WiFi.SSID(i).c_str()) + " | " + std::string( (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "open" : "protected" ) + " | (" + std::to_string( WiFi.RSSI(i) ) + ")";
            html += "</option>";
        }
    }
    else
    {
        html += "<option value=\"none\">No WiFi Avaiable</option>";
    }

    html += "</select>";

    html += "</td>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "Password:";
    html += "</td>";
    html += "<td>";
    html += "<input type='text' placeholder='Wifi Password' name='pass'/>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "IOT Server IP";
    html += "</td>";
    html += "<td>";
    html += "<input type='text' placeholder='0.0.0.0' name='server'/>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "Device Name";
    html += "</td>";
    html += "<td>";
    html += "<input type='text' placeholder='Device name' name='name'/>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "Mode";
    html += "</td>";
    html += "<td>";
    html += "<select name=\"mode\">";
    html += "<option value=\"none\">none</option>";
    html += "<option value=\"motor\">Motor Controller</option>";
    html += "<option value=\"dht\">DHT Controller</option>";
    html += "<option value=\"valve\">Water Valve Controller</option>";
    // html += "<option value=\"switch\">Switch controller</option>";
    // html += "<option value=\"blinds\">Blinds Controller</option>";
    // html += "<option value=\"valve\">Valve Controller</option>";
    // html += "<option value=\"luminosity\">Luminosity Controller</option>";
    // html += "<option value=\"motion\">Motion Controller</option>";
    // html += "<option value=\"entry\">Entry Controller</option>";
    html += "</select>";
    html += "</td>";
    html += "<tr>";

    //ROW ====================
    html += "</tr>";
    html += "<td>";
    html += "</td>";
    html += "<td>";
    html += "<input type='submit' value='Connect'/>";
    html += "</td>";
    html += "<tr>";

    html += "</table>";
    html += "</form>";

    html += endHTML;

    send(200, "text/html", String(html.c_str()));
}

void HTTPServer::init()
{
  on("/", [this](){ this->handleDeviceSettings(); });
  // on("/settingssave", handleSettingsSave);
  // onNotFound(handleNotFound);
  begin(); // Web server start
}

void HTTPServer::loop()
{
  handleClient();
}


#endif