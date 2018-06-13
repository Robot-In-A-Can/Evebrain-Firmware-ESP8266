#include <Evebrain.h>

Evebrain evebrain;

void setup(){
  evebrain.begin();
  evebrain.enableSerial();
  evebrain.enableWifi();
}

void loop(){
  evebrain.loop();
}

