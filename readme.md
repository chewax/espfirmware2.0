# Home Automation 8266 Firmware
For standalone boards

## Basic Behaviour
This firmware is thought to modularize and delegate all functionality to each individual controller. As such each controller must implement three indispensable functions:

```
	init(...); //intializes the controller with indispensable information
	loop();    //recieves the loop signal and performs whetever is needed.
	sense();   //publishes the current state to the API
```

## Configuration
There are a couple of ```#define``` vars that can be used to configure the board

**IE:**

```cpp
//Network Configuration
#define Wifi_SSID "wifi_ssid"
#define Wifi_PASS "wifi_password"

//SocketIO Configuration
#define IOBoard_ID "unique_board_identifier" //we reccomend something that humans can read
#define IOServer_HOST "192.168.3.105"        //socket server IP
#define IOServer_Port 3000                   //socket server port
```

## Interface

Interface is a class to manage the behaviour of the board regarding the different pins (GPIO00, GPIO01,...,etc.). Each different pin can be instantiated to use a different controller (see controllers below). Each controller implements a base Controller class.
**Interface** provides a way to easily and dynamically choose which controller to apply to each pin. One should create a different Interface instance for each used pin.

## Controller
A Controller is a base class from which all controllers will inherit. Controller is supposed to have common functionality for all the different behaviours that want to be implemented.
Each controller will have an "actuator" property which describes what exactly is connected to the pin. This property ("actuator") is mainly intended to be used by the frontend to present a proper UI/UX for the user. Nevertheless that property is the one that decides which particular controller instance gets created.

| Name             | Actuator(s)         | Description                                                  |
| ---------------- | ------------------- | ------------------------------------------------------------ |
| DHTController    | dht                 | Provides functionality for reading a DHT sensor and publishing that information to the API (or socket) |
| CaudalController | caudal              | Provides functionality for reading a caudalimenter and publishing that information to the API (or socket) |
| FillerController | filler              | Is a special instance of CaudalController that provides hooks to alert when a certain caudal has been reached |
| SwitchController | valve, bulb, switch | Is basically a controller for actuating a relay. Publishes state to the API every time its actuated. The discrimination of actuator property responds to the fact that UI/UX will be different for each type. |
| ToggleController | toggle              | Is a special type of switch that will automatically reset to the closed state whenever a certain condition is met. That condition can either be a timer or an event emitted from another controller both local or remote. |
| DigitalInput     | digitalInput        | This is a special type of controller that is intended to handle (as the name clearly states) digital inputs from a pin. It is very useful, for instance, to have a button local to the board that we want to function without the need of a wireless connection. Can function standalone so to speak. |

> eg: If a new interface is instanciated with ```actuator=bulb``` underlingly a SwitchController will be attached to the interface which will handle all the behaviour for that pin.

**Example**  

```cpp
#define D3 0
#define SCENE "Master Bedroom"
...
Interface nightTable;
SocketIO socket(SCENE); 
luzFondo.init(&socket, D3, "Night Table", "bulb");
```
### Extending Controller
All library is header only. We encourage to maintain that pattern. To add new controllers extend controller class and implement init, loop and sense functions.

```cpp
#ifndef __MyController_h
#define __MyController_h

class MyController: public Controller
{
  public:
    //constructor
    MyController():MyController(){};
	
    //init pattern is used. Nothing happens upon creation just the basic allocation of
    //memory space. All variable instatiation and necesary configuration ocurs inside
    //the init function. Thus, the constructor should be the default and all needed
    //setup should happen here.
    void init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator);
    
    //Not implemented in base class: MUST be implemented in your controller
    void loop(); 
    
    //Not implemented in base class: MUST be implemented in your controller.
    //This function is intende to inform the API server of the current status of the pin
    //which will obviously depend on the type of device/sensor/actuator connected. 
    //However, it is expected that all types inform of the current state.
    virtual void sense(); 
}

void MyController::init(SocketIO* t_socket, const int t_pin, const std::string& t_name, const std::string& t_actuator)
{
  Controller::init(t_socket, t_pin, t_name, t_actuator);
  //Your own init logic here.
}

#endif
```






# Extra

## EventEmitter
EventEmitter is a static Class to suscribe and emit global events. It can (and should) be used to communicate between interfaces.
ESP8266, even in the most basic form, contains more than one GPIO pin that can be used simultaneously with this firmware. Nevertheless, without EventEmitter each pin is independent from each other. Using EventEmitter we can subordinate the actions of one controller to the results or readings of the other.  
Suppose we want to close a switch whenever the caudal meter reaches certain caudal. That is a typical use case for EventEmitter as so:

```cpp
#define D3 0
#define D2 4
#define SCENE "Watering"

SocketIO socket(SCENE);
Interface caudal;
Interface toggle;

//FILLER w/TOGGLE USAGE EXAMPLE
//====================

//Initialize toggle normally.
toggle.init(&socket, D3, "1.5L", "toggle");

//Initialize caudal normally (but with actuator=filler) (see Controllers)
caudal.init(&socket, D2, "Caudal", "filler");

//Set the filler stop condition at desired caudal (say 1500mm)
((FillController*)(caudal.ifController))->setMililiters(1500);


//Each controller will have a default on/off event attributes
//ie: evt_on, evt_off. They will default to <pin>@<mac>:on <pin>@<mac>:off respectively.
//They can be reassigned to whatever event we want to be emitted when on or off and also
//can be leveraged to subordinate the behaviour of another controller.

//Instruct the filler to start counting when the toggle emits its "on" event
caudal.ifController->onWhen( toggle.ifController->evt_on );

//Instruct toggle to automatically close when fillController emits its "off" event
toggle.ifController->offWhen( caudal.ifController->evt_off );
```



This kind of behaviour is possible because`evt_on` and `evt_off` are emitted timely in both controllers and on/off methods are implemented in base classes. Keep this in mind whenever extending the behaviour of your own controller. If you want to have this kind of functionality both events should be fired whenever is appropriate.

##### Eg:

```cpp
void MyController::off()
{
  //Do own stuff
  Controller::off();
}

void MyController::on()
{
  //Do own stuff
  Controller::on();
}

void MyController::sense()
{ 
  Controller::sense();
  if (some condition is met) off();
}
```

