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

- Open the Preferences window in your Arduino IDE.
-  Enter 
   ``https://arduino.esp8266.com/stable/package_esp8266com_index.json``
   into *Additional Board Manager URLs* field. You can add multiple
   URLs, separating them with commas.
-  Open Boards Manager from Tools > Board menu and find `esp8266`  platform.
-  Select the `2.3.0` version from a drop-down box.
-  Click the *install* button.
-  Don't forget to select your ESP8266 board from the Tools > Board menu after installation.

For more details on Arduino's Board Manager, see:
- https://www.arduino.cc/en/guide/cores


## Add this repo to your Arduino Libraries folder

### On Linux

If you are using Snaps to install Arduino on Linux this is
~/snap/arduino/current/Arduino/libraries


### on Mac

~/Documents/Arduino/libraries

### on windows this

your home dir Documents/Arduino/libraries

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
