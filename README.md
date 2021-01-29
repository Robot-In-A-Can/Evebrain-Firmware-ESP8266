Robot In A Can Evebrain Firmware
================================

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

This is a branch of the Mirobot project (http://mirobot.io)

# Buildinng new web blob for EveBrain

- Web sorce files are in  ./web/
- Build using `node ./build-web.js`
- this will take all things in ./web/index.html and blob them
- blob is stored in src/lib/web.h
