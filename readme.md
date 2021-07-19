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

Interface is a class to manage the behaviour of the different pins( GPIO00, GPIO01...etc ) of the board.
Each different pin can be instanciated to use a different controller (see controllers below). Each controller implements a base Controller class.
Interface provides a way to easily and dinamicaly choose which controller to apply to each pin. One should instanciate a differente Interface for each used pin.

## Controller
A Controller is a base class from which all controllers will inherit. Controller is supposed to have commmon functionality for all the different behaviours that want to be implemented.
Each controller will have an "actuator" property which describes what exactly is connected to the pin. This property ( "actuator") is mainly intended to be used by the frontend to present a proper UI/UX for the user. Nevertheless that property is the one that decides which particular controller instance gets created.

| Name             | Actuator(s)         | Description                                                                                                       |
|------------------|---------------------|-------------------------------------------------------------------------------------------------------------------|
| DHTController    | dht                 | Provides functionality for reading a DHT sensor and publishing that information to the API (or socket)            |
| CaudalController | caudal              | Provides functionality for reading a caudalimenter and publishing that information to the API (or socket)         |
| FillerController | filler              | Is a special instance of CaudalController that provides hooks to alert when a certain caudal has been reached     |
| SwitchController | valve, bulb, switch | Is basically a controller for actuating a relay. Publishes state to the api everytime its actuated                |
| ToggleController | toggle              | Is a special type of switch that will automatically reset to the closed state whenever a certain condition is met |

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
To add new controllers the following pattern is adviced.

1. Extend Controller class
2. Implement custom init, loop and sense functions.


# Extra

## EventEmitter
EventEmitter is a static Class to suscribe and emit global events. It can (and should) be used to communicate between interfaces.
ESP8266 even in the most basic form contains more than one GPIO pin that can be used simultaneously with this firmware nevertheless without EventEmitter each pin is independent from eachother. Using EventEmitter we can link the actions of one controller to the results or readings of the other.  
Suppose we want to close a switch whenever the caudalimeter reaches certain caudal. That is a tipical use case for EventEmitter as so:

```cpp
#define D3 0
#define D2 4
#define SCENE "Watering"

SocketIO socket(SCENE);
Interface caudal;
Interface toggle;

//TOGGLE USAGE EXAMPLE
//====================

//Initialize toggle normally.
toggle.init(&socket, D3, "1.5L", "toggle");

//Instruct toggle to automatically close when "fill:complete" is emitted by the EventEmitter
((ToggleController*)(toggle.ifController))->closeWhen("fill:complete"); 

//FILLER USAGE EXAMPLE
//====================

/Initialize caudal normally (but with actuator filler) (see Controllers)
caudal.init(&socket, D2, "Caudal", "filler");

//Set the filler stop condition at 1500mm
((FillController*)(caudal.ifController))->setMililiters(1500);

//Instruct the filler to start counting when the "toggle:<toggle_controller_id>:on" is emmited by the EventEmitter. 
((FillController*)(caudal.ifController))->fillWhen("toggle:" + toggle.ifController->id + ":on");

//"fill:complete" event is harcoded into the FillerController an will be emitted every time the fill is complete
//"toggle:<controller_id>:on" is also harcoded into the toggle controller and will be emitted every time the toggle is set to on.
```

