#include <Arduino.h>
#include "MotorController.h"
#include "Utils.h"



MotorController::MotorController() : Controller()
{
}

void MotorController::init(String name, String id)
{

    Controller::init(name, id);
}


//Performs OFF action
void MotorController::sense()
{
    Controller::sense();

    String result;

    result += Utils::asProperty("state", lastKnownState, false);
    result = Utils::asJSONObj(result);

    if (io)
        io->sendJSON("board:sense", result);
}


// Reads sensor information
// Althought it may have 2 sensors, sensed output is always a state calculated from both readings...so for the outer world it has 1 sensor.
void MotorController::computeSensorData()
{
    int sensorA_state = digitalRead(sensorA);
    int sensorB_state = digitalRead(sensorB);

    String currentState;

    if (sensorA_state && !sensorB_state)
        currentState = "opened";

    else if (sensorB_state && !sensorA_state)
        currentState = "closed";

    else
        currentState = "unknown";

    if (lastKnownState != currentState)
        onStateChange(currentState);

    lastKnownState = currentState;
}

void MotorController::onStateChange(String newState)
{
    if (newState == "opened" || newState == "closed")
        //Cuando llega a los topes desestimulo el relay
        setOff(true);

    String result;

    result += Utils::asProperty("state", newState, false);
    result = Utils::asJSONObj(result);

    if (io)
        io->sendJSON("board:sense", result);
}

void MotorController::handleInput(uint32_t val)
{

    if (val == LOW && lastInputVal == HIGH) //If button set to GND. Toggle.
    {
        lastInputVal = LOW;
        toggle(true);
    }
    else if (val == HIGH && lastInputVal == LOW)
    {
        lastInputVal = HIGH;
    }

    computeSensorData();
}
