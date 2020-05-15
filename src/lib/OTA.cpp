#include "Arduino.h"
#include "OTA.h"

#include <ESP8266WebServer.h>

const char* host = "robotinacan";
const char* ssid = "........";
const char* password = "........";
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";


ESP8266WebServer serverOne(5000);


void OTA::setupOTA () {
 	MDNS.begin(host);
	serverOne.on("/", HTTP_GET, [](){
	  serverOne.sendHeader("Connection", "close");
	  serverOne.sendHeader("Access-Control-Allow-Origin", "*");
	  serverOne.send(200, "text/html", serverIndex);
	});
	serverOne.on("/update", HTTP_POST, [](){
	  serverOne.sendHeader("Connection", "close");
	  serverOne.sendHeader("Access-Control-Allow-Origin", "*");
	  serverOne.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
	  ESP.restart();
	},[](){
	  HTTPUpload& upload = serverOne.upload();
	  if(upload.status == UPLOAD_FILE_START){
	    Serial.setDebugOutput(true);
	    WiFiUDP::stopAll();
	    Serial.printf("Update: %s\n", upload.filename.c_str());
	    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
	    if(!Update.begin(maxSketchSpace)){//start with max available size
	      Update.printError(Serial);
	    }
	  } else if(upload.status == UPLOAD_FILE_WRITE){
	    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
	      Update.printError(Serial);
	    }
	  } else if(upload.status == UPLOAD_FILE_END){
	    if(Update.end(true)){ //true to set the size to the current progress
	      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
	    } else {
	      Update.printError(Serial);
	    }
	    Serial.setDebugOutput(false);
	  }
	  yield();
	});
	serverOne.begin();
	MDNS.addService("http", "tcp", 80);
}


void OTA::runOTA () {
    serverOne.handleClient();
}
