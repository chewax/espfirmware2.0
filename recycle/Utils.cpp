#include <Arduino.h>
#include "Utils.h"
#include <Time.h>
#include <TimeLib.h>
#include <IPAddress.h>
#include <EEPROM.h>

#define USE_SERIAL Serial
#define ENABLE_DEBUG

const int EEPROM_OFFSET = 0;

/** Is this an IP? */
boolean Utils::isIp(String str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        int c = str.charAt(i);
        if (c != '.' && (c < '0' || c > '9'))
        {
            return false;
        }
    }
    return true;
}

/** IP to String? */
String Utils::toStringIp(IPAddress ip)
{
    String res = "";
    for (int i = 0; i < 3; i++)
    {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
}

char *Utils::toStringMac(uint8 mac[6])
{
    static char str[18];

    if (mac == NULL)
        return "";

    snprintf(str, sizeof(str), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return str;
}

char *Utils::toStringMac(String mac)
{
    char *cstr = new char[mac.length() + 1];
    strcpy(cstr, mac.c_str());
    return cstr;
}

void Utils::saveConfig(ConfigStruct config)
{
    EEPROM.begin(sizeof(ConfigStruct));

    strcpy(config.configured, "OK");

    EEPROM.put(EEPROM_OFFSET, config);

    EEPROM.commit();
    EEPROM.end();
}

ConfigStruct Utils::loadConfig()
{
    EEPROM.begin(sizeof(ConfigStruct));

    ConfigStruct config_read;
    EEPROM.get(EEPROM_OFFSET, config_read);
    EEPROM.end();

    if (String(config_read.configured) != String("OK"))
    {
        config_read = ConfigStruct();
    }

    return config_read;
}

void Utils::logger(String msg, String delim)
{
#ifdef ENABLE_DEBUG
    USE_SERIAL.print("[");
    USE_SERIAL.print(delim);
    USE_SERIAL.print("]\t");
    USE_SERIAL.println(msg);
#endif
}

void Utils::debug(String msg)
{
#ifdef ENABLE_DEBUG
    USE_SERIAL.print("[DEBUG]\t");
    USE_SERIAL.println(msg);
#endif
}

void Utils::printTimestamp()
{
    time_t t = now();
    USE_SERIAL.print(day(t));
    USE_SERIAL.print("/");
    USE_SERIAL.print(month(t));
    USE_SERIAL.print("/");
    USE_SERIAL.print(year(t));
    USE_SERIAL.print(":");
    USE_SERIAL.print(hour(t));
    printDigits(minute(t));
    printDigits(second(t));
}

void Utils::printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    USE_SERIAL.print(":");
    if (digits < 10)
        Serial.print('0');
    USE_SERIAL.print(digits);
}

String Utils::asProperty(String key, String value, bool comma)
{
    String result;

    result += "\"";
    result += key;
    result += "\":\"";
    result += value;
    result += "\"";
    if (comma)
        result += ",";

    return result;
}

String Utils::asJSONObj(String key)
{
    String result;

    result += "{";
    result += key;
    result += "}";

    return result;
}

//Initializes Serial
void Utils::initializeSerial()
{
    USE_SERIAL.begin(115200);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for (uint8_t t = 4; t > 0; t--)
    {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        // delay(1000);
        delayMicroseconds(1000000);
    }

    USE_SERIAL.println('[SETUP] SERIAL INITIALIZED');
}