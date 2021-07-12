# Home Automation 8266 Firmware
For standalone boards

## Board Controller

### Relay
All boards will have one relay. That relay can be actuated in one of two ways either by sending a socketIO message, or by setting corresponding pin to ground.

| Action | Socket Message | Relay Pin |
|--------|----------------|-----------|
| setOn | board:setOn | HIGH |
| setOff | board:setOff | LOW |
| toggle | board:defaultAction | - |


> Note: Relay Pin can be configured in Controller class init.

