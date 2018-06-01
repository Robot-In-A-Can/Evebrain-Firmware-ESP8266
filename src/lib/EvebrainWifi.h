#ifndef __EvebrainWifi_h__
#define __EvebrainWifi_h__

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "lib/EvebrainWeb.h"
#include "lib/EvebrainWebSocket.h"
#include "Evebrain.h"
#include <DNSServer.h>
#include <Ticker.h>
#include "lib/Discovery.h"
#include "lib/ArduinoJson/ArduinoJson.h"

typedef void (* dataHandler) (char *);

struct EvebrainSettings;

class EvebrainWifi {
  public:
    EvebrainWifi();
    void begin(EvebrainSettings *);
    void run();
    static void defautAPName(char*);
    static IPAddress getStaIp();
    static int32_t getStaRSSI();
    static WiFiMode getWifiMode();
    static void startWifiScan();
    void setupWifi();
    static bool networkChanged;
    static bool wifiScanReady;
    static EvebrainSettings * settings;
    void getWifiScanData(ArduinoJson::JsonArray &);
    void onMsg(dataHandler);
    static void sendWebSocketMsg(ArduinoJson::JsonObject &);
  private:
    bool enabled;
    static bool wifiScanRequested;
    EvebrainWeb webServer;
    DNSServer dnsServer;
    void setupDNS();
    static void staCheck();
};

#endif
