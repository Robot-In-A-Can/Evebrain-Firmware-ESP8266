#ifdef ESP8266

#include "EvebrainWebSocket.h"
using namespace websockets;

WebsocketsServer ws;
WebsocketsClient wsClient;

unsigned long lastPingTime;
unsigned long lastPongResponseTime;
const unsigned long pingDelay = 5000; // 5sec between pings
const unsigned long pongTimeout = 11000; // 11sec of inactivity will trigger reconnect attempt

void onEventCallback(WebsocketsClient& client, WebsocketsEvent event, WSInterfaceString str) {
  if(event == WebsocketsEvent::ConnectionClosed) {
    //Serial.println("Connection closed");
  } else if (event == WebsocketsEvent::GotPong) {
    //Serial.println("got pong");
    // Store the time the pong was responded to
    lastPongResponseTime = millis();
  } else if (event == WebsocketsEvent::GotPing) {
    //Serial.println("got ping");
    client.pong();
  }
}


dataHandler handler = NULL;

void onMessageCallback(WebsocketsMessage message) {
  if (handler) handler((char*) message.c_str());
}

void beginWebSocket(){
  Serial.println("started");
  ws.listen(8899);
}

void websocketPoll() {
  // this checks not only that the socket says it's available, but also that it has 
  // actually responded to this ebrain's pings.
    if(wsClient.available() && (millis() - lastPongResponseTime) < pongTimeout) {
        wsClient.poll();
        // Send a ping if we're due
        if (millis() - lastPingTime > pingDelay) {
          wsClient.ping();
          lastPingTime = millis();
        }
    } else {
      // if not connected, but if there is a client waiting, connect.
      if (ws.poll()) {
        wsClient = ws.accept();
        wsClient.onMessage(onMessageCallback);
        wsClient.onEvent(onEventCallback);
        // Ensure we do not immediately close the connection due to inactivity 
        // by setting pong response time to the current time. So, 
        // this will have pongTimeout at least to send ping & receive pong
        lastPongResponseTime = millis();
      }
    }
}

void setWsMsgHandler(dataHandler h){
  handler = h;
}

void sendWsMsg(ArduinoJson::JsonObject &msg){
  char jsonBuff[JSON_BUFFER_LENGTH];
  msg.printTo(jsonBuff, sizeof(jsonBuff));
  if (wsClient.available()) {
    wsClient.send(jsonBuff, strlen(jsonBuff));
  }
}


#endif
