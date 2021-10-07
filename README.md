# Robot In A Can Evebrain Firmware


## v0.1.0

This is an Arduino library that allows you to control the Robot In A Can Evebrain (http://robotinacan.com).


It can be used in one of two ways;
 - Controlling Evebrain directly from Arduino code (e.g. evebrain.forward(100)).
   The logic for how it moves is in the Arduino sketch.
   See the "basic_example" in the examples directory.
 - Receiving commands remotely. The logic for how it moves is somewhere else,
   most probably in the browser. See the "socket_example" in the examples directory.



When receiving commands remotely the socket can
either be used raw or as a websocket from a browser.

It uses the SHA1 and Base64 libraries from
https://github.com/ejeklint/ArduinoWebsocketServer
in order to provide the Websocket functionality.

Special thanks to the Mirobot project (http://mirobot.io)


## Prerequisites

-  Arduino >= 1.6.8, get it from `Arduino
   website <https://www.arduino.cc/en/Main/OldSoftwareReleases#previous>`__.
-  Internet connection
-  Python 3 interpreter (Mac/Linux only, Windows installation supplies its own)

## Instructions

1. Open the Preferences window in your Arduino IDE.
1. Enter 
   ``https://arduino.esp8266.com/stable/package_esp8266com_index.json``
   into *Additional Board Manager URLs* field. You can add multiple
   URLs, separating them with commas.
1. Open Boards Manager from Tools > Board menu and find `esp8266`  platform.
1. Select the `3.0.2` version from a drop-down box.
1. Click the *install* button.
1. Select `Generic ESP8266 Module` from the `Tools > Board > ESP8266 Modules` menu after installation.
2.  Select `4M (3M SPIFFS)` from the `Tools > Flash Size:` menu.

For more details on Arduino's Board Manager, see:
- https://www.arduino.cc/en/guide/cores


## Add this repo to your Arduino Libraries folder

**Heads up!** In previous version of the Arduino IDE, all libraries were stored together deep within the contents folder of the Arduino application. However, in newer versions of the IDE, libraries added through the Library Manger can be found in a folder named '**libraries**' found in your Arduino Sketchbook folder.  
For more information on the Library manger, including deleting and updating info, visit the [GitHub: Arduino - Library Manager FAQ](https://github.com/arduino/Arduino/wiki/Library-Manager-FAQ).

* On Linux and you are using Snaps to install Arduino your libraries folder is `~/snap/arduino/current/Arduino/libraries`. 
* On a Mac `~/Documents/Arduino/libraries`
* On windows it wil be in your home dir `Documents/Arduino/libraries`

## Write new firmware

* Start with an example found in `File > Examples`

## Write to board

set board power to 0 (for usb)

connect gnd to pin 0

connect usb from computer to board

In arduino select the port you are connected to

Change Flash size to 4M

# Clear board memory

3.3 to pin 13


# Building new web blob for EveBrain

- Web sorce files are in  ./web/
- `yarn install` to install required libraries
- `node ./build-web.js` to compress the web folder into a blob for flashing
- this will take all things in ./web/index.html and blob them
- blob is stored in src/lib/web.h


# FAQ

* Q: How do I get around error `Multiple libraries were found for "Servo.h"`

  A: Change the Flashsize to 4M 
  
* Q: What's the deal with `warning: espcomm_sync failed`

  A: You're trying to flash your board at to high a baud rate. Drop it down a level and try again. It should work for all setups at a speed of 115200 but some users are able to upload at a speed 921600. 

* Q: How do I build the web interface for flashing to the board?
  
  A: ./web is gets compressed using our build-web.js script. Before doing this make your life easier n install all the requred packages using `yarn install`.
  
     Tweak any files you want to work on.
  
     After tweaking things in ./web run `node ./build-web.js` from the project's home dir.




#List of Supported eBrain Commands

Command Name         does this command take time to run?
```
"version",           true
"ping",              true
"uptime",            true
"resume",            true
"slackCalibration",  true
"moveCalibration",   true
"turnCalibration",   true
"calibrateMove",     true
"calibrateTurn",     true
"forward",           false
"back",              false
"right",             false
"left",              false
"beep",              false
"calibrateSlack",    false
"analogInput",       true
"readSensors",       false
"digitalInput",      true
"gpio_on",           true
"gpio_off",          true
"gpio_pwm_16",       true
"gpio_pwm_5",        true
"gpio_pwm_10",       true
"temperature",       false
"humidity",          false
"distanceSensor",    false
"leftMotorF",        false
"leftMotorB",        false
"rightMotorF",       false
"rightMotorB",       false
"servo",             false
"servoII",           false
"getConfig",         true
"setConfig",         true
"resetConfig",       true
"freeHeap",          true
"startWifiScan",     true
"postToServer",      true
```
