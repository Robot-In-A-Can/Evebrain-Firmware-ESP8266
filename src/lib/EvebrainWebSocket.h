#ifndef __EvebrainWebSocket_h__
#define __EvebrainWebSocket_h__

#include "Arduino.h"
#include "lib/ArduinoWebSockets/ArduinoWebsockets.h"
#include "lib/ArduinoJson/ArduinoJson.h"
#include "lib/EvebrainWifi.h"

typedef void (* dataHandler) (char *);

void beginWebSocket();
void setWsMsgHandler(dataHandler);
void sendWsMsg(ArduinoJson::JsonObject &);
void websocketPoll();

#endif
