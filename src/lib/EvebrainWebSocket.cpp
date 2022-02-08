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

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void beginWebSocket(){
  Serial.println("started");
  ws.listen(8899);
  wsClient = ws.accept();
  wsClient.onMessage(onMessageCallback);
  wsClient.onEvent(onEventsCallback);
}

void poll() {
    if(wsClient.available()) {
        wsClient.poll();
        Serial.println("polled");
    }
}

void setWsMsgHandler(dataHandler h){
  handler = h;
}

void sendWsMsg(ArduinoJson::JsonObject &msg){
  char jsonBuff[JSON_BUFFER_LENGTH];
  msg.printTo(jsonBuff, sizeof(jsonBuff));

  Serial.println("sending...");
  Serial.println(jsonBuff);
  Serial.println("------------");

  wsClient.send(jsonBuff, strlen(jsonBuff));

}

#endif
