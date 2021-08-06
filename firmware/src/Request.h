#ifndef __Request_h
#define __Request_h

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <string>
#include <map>

class Request
{
  public:
    Request(const std::string& t_url);
    int POST( const std::map<std::string, std::string> &payload);
    
  private:
    std::string url;
    std::string mapToJsonString( const std::map<std::string, std::string> &payload );
};


Request::Request(const std::string& t_url): url(t_url)
{

}

std::string Request::mapToJsonString(  const std::map<std::string, std::string> &payload )
{ 
  std::string json = "{";
  for (auto it=payload.begin(); it!=payload.end(); ++it) 
  {
    json += "\"" + it->first + "\":" + "\"" + it->second + "\"";
    if (std::next(it, 1) != payload.end()) json += ",";
  }
  json += "}";
  return json;
}

int Request::POST(const std::map<std::string, std::string> &payload)
{
  std::string json = mapToJsonString(payload);
  WiFiClient client;
  HTTPClient http;
   // Your Domain name with URL path or IP address with path
  http.begin(client, url.c_str());
  http.addHeader("Content-Type", "application/json");
  return http.POST((const uint8_t *) json.c_str(), json.length());
  http.end();
}





#endif