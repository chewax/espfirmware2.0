#include <Arduino.h>
#include <EEPROM.h>

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
    static ConfigStruct loadConfig();
    static void saveConfig(ConfigStruct data);

};

const int EEPROM_OFFSET = 0;

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

#endif
