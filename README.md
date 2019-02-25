Robot In A Can Evebrain Firmware
================================

This is an Arduino library that allows you to control the Robot In A Can Evebrain (http://robotinacan.com).

It can be used in one of two ways;
 - Controlling Evebrain directly from Arduino code (e.g. evebrain.forward(100)). The logic for how it moves is in the Arduino sketch. See the "basic_example" in the examples directory.
 - Receiving commands remotely. The logic for how it moves is somewhere else, most probably in the browser. See the "socket_example" in the examples directory.
 Use this with the offline Snap! html files to send the commands to the ESP8266 over the web browser.

You will need to have the ESP8266 board installed into your Arduino IDE to compile. Please make sure you are using version 2.3.0 for it to compile properly.

When receiving commands remotely the socket can either be used raw or as a websocket from a browser.

It uses the SHA1 and Base64 libraries from https://github.com/ejeklint/ArduinoWebsocketServer in order to provide the Websocket functionality.

This is a branch of the Mirobot project (http://mirobot.io).