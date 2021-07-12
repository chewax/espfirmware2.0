#include <Arduino.h>
#include <Time.h>
#include <TimeLib.h>
#include <IPAddress.h>

#ifndef Utils_h
#define Utils_h

struct ConfigStruct
{
    char ssid[32];
    char password[32];
    char server[32];
    char name[32];
    char mode[32];
    char configured[3];
};

class Utils
{
  public:
    Utils();
    static String toStringIp(IPAddress ip);
    static char *toStringMac(uint8 mac[6]);
    static char *toStringMac(String mac);
    static boolean isIp(String str);
    static ConfigStruct loadConfig();
    static void saveConfig(ConfigStruct data);
    static void logger(String msg, String delim);
    static void debug(String msg);
    static String asProperty(String key, String value, bool comma);
    static String asJSONObj(String key);
    static void initializeSerial();

  private:
    static void printTimestamp();
    static void printDigits(int digits);
};

#endif
