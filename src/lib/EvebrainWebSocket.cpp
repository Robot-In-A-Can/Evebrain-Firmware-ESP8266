#ifdef ESP8266

#include "EvebrainWebSocket.h"
using namespace websockets;

WebsocketsServer ws;
WebsocketsClient wsClient;

dataHandler handler = NULL;

void onMessageCallback(WebsocketsMessage message) {
  if (handler) handler((char*) message.c_str());
  Serial.println("Got message");
  Serial.print(message.data());
}

void beginWebSocket(){
  Serial.println("started");
  ws.listen(8899);
  wsClient = ws.accept();
  wsClient.onMessage(onMessageCallback);
}

void setWsMsgHandler(dataHandler h){
  handler = h;
}

void sendWsMsg(ArduinoJson::JsonObject &msg){

  char jsonBuff[JSON_BUFFER_LENGTH];
  msg.printTo(jsonBuff, sizeof(jsonBuff));
  wsClient.send(jsonBuff, strlen(jsonBuff));

}

#endif
