#include <Arduino.h>
#include "CurrentController.h"
#include "Utils.h"



CurrentController::CurrentController() : Controller()
{
}

void CurrentController::init(String name, String id)
{
    Controller::init(name, id);

    // pinMode(sensorA, INPUT);
    vpp = 3.3f / 1024; /* 220v / 1024 */ 
}

//Performs OFF action
void CurrentController::sense()
{
    Controller::sense();
    computeSensorData();

}

// Reads sensor information
// Althought it may have 2 sensors, sensed output is always a state calculated from both readings...so for the outer world it has 1 sensor.
void CurrentController::computeSensorData()
{

    int counts = analogRead(sensorA);
    // float voltage = counts * vpp;
    Utils::debug(String("Counts : ") + String(counts));
    // Utils::debug(String("Voltage : ") + String(voltage,3));


    // RawValue = analogRead(inputA);

    // Serial.println(RawValue, 3);

    // Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
    // Amps = ((Voltage - ACSoffset) / mVperAmp);

    // Serial.print("Raw Value = " ); // shows pre-scaled value 
    // Serial.print(RawValue); 
    // Serial.print("\t mV = "); // shows the voltage measured 
    // Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
    // Serial.print("\t Amps = "); // shows the voltage measured 
    // Serial.println(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point

    // result += "{\"";
    // result += "voltage";
    // result += "\":\"";
    // result += lastKnownState;
    // result += "\"}";


    // if (io)
    //     io->sendJSON("board:sense", result);
}

void CurrentController::onStateChange(String newState)
{
    if (newState == "closed")
        //Cuando cierra desestimulo el relay
        setOff(true);

    if (newState == "unknown")
        //Set delay to turn off
        timer.setTimeout(AUTOOFF_DELAY, [this]() {
            this->setOff(true);
        });

    String result;
    
    result += "{\"";
    result += "state";
    result += "\":\"";
    result += newState;
    result += "\"}";

    if (io)
        io->sendJSON("board:sense", result);
}

void CurrentController::handleInput(uint32_t val)
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
