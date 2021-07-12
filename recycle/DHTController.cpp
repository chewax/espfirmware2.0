#include <Arduino.h>
#include "DHTController.h"
#include "Utils.h"
#include "DHT.h"

#define DHTTYPE DHT11   // DHT 11

DHTController::DHTController() : Controller()
{
}

void DHTController::init(String name, String id)
{
    Controller::init(name, id);
    dht = new DHT(sensorA, DHTTYPE);
    dht->begin();
}

//Performs OFF action
void DHTController::sense()
{
    Controller::sense();

    float h = dht->readHumidity();
    float t = dht->readTemperature();
    float hic = dht->computeHeatIndex(t, h, false); 

    String result;
    result += Utils::asProperty("temperature", String(t, 3), true);
    result += Utils::asProperty("humidity", String(h, 3), true);
    result += Utils::asProperty("hic", String(hic, 3), false);

    result = Utils::asJSONObj(result);

    if (io)
        io->sendJSON("board:sense", result);

}

// Reads sensor information
// Althought it may have 2 sensors, sensed output is always a state calculated from both readings...so for the outer world it has 1 sensor.
void DHTController::computeSensorData()
{

    if ( millis() - pollTimestamp > POLL_INTERVAL) 
    {
        pollTimestamp = millis();
        sense();
    }

}

void DHTController::handleInput(uint32_t val)
{

    if (val == LOW && lastInputVal == HIGH)
    {
        setOff(true);
        lastInputVal = LOW;
    }
    else if (val == HIGH && lastInputVal == LOW)
    {
        setOn(true);
        lastInputVal = HIGH;
    }

    computeSensorData();
}
