#ifndef OTA_h
#define OTA_h

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

class OTA {
	public:
		void runOTA();
		void setupOTA();
};

#endif