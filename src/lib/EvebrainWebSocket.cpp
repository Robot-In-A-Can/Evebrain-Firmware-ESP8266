#ifdef ESP8266

#include "EvebrainWebSocket.h"

WebSocketsServer ws = WebSocketsServer(8899);

dataHandler handler = NULL;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      break;
    case WStype_TEXT:
      if(handler) handler((char *)payload);
      break;
  }
}

void beginWebSocket(){
  ws.begin();
  ws.onEvent(webSocketEvent);
}

void setWsMsgHandler(dataHandler h){
  handler = h;
}

void sendWsMsg(ArduinoJson::JsonObject &msg){
  char jsonBuff[JSON_BUFFER_LENGTH];
  msg.printTo(jsonBuff, sizeof(jsonBuff));
  ws.broadcastTXT(jsonBuff, strlen(jsonBuff));
}

#endif
