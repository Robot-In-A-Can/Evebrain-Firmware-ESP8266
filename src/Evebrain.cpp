#include "Arduino.h"
#include "Evebrain.h"
#include "lib/DHT/DHTesp.h"

DHTesp dht;
CmdProcessor cmdProcessor;
SerialWebSocket v1ws(Serial);

ShiftStepper rightMotor(1);
ShiftStepper leftMotor(0);

EvebrainWifi wifi;
OTA ota;

Servo servoid;

WS2812B led(STATUS_LED_PIN);

int frequencyHZ[] =  {NOTE_B0,NOTE_C1,NOTE_CS1,NOTE_D1,NOTE_DS1,NOTE_E1,NOTE_F1,NOTE_FS1,NOTE_G1,NOTE_GS1,NOTE_A1,NOTE_AS1,NOTE_B1,NOTE_C2,NOTE_CS2,NOTE_D2, NOTE_DS2,NOTE_E2,NOTE_F2,NOTE_FS2,NOTE_G2,NOTE_GS2,NOTE_A2,NOTE_AS2,NOTE_B2,NOTE_C3,NOTE_CS3,NOTE_D3,NOTE_DS3,NOTE_E3,NOTE_F3,NOTE_FS3,NOTE_G3,NOTE_GS3,NOTE_A3,NOTE_AS3,NOTE_B3,NOTE_C4,NOTE_CS4,NOTE_D4,NOTE_DS4,NOTE_E4,NOTE_F4,NOTE_FS4,NOTE_G4,NOTE_GS4,NOTE_A4,NOTE_AS4,NOTE_B4,NOTE_C5,NOTE_CS5,NOTE_D5,NOTE_DS5,NOTE_E5,NOTE_F5,NOTE_FS5,NOTE_G5,NOTE_GS5,NOTE_A5,NOTE_AS5,NOTE_B5,NOTE_C6,NOTE_CS6,NOTE_D6, NOTE_DS6,NOTE_E6,NOTE_F6,NOTE_FS6,NOTE_G6,NOTE_GS6,NOTE_A6,NOTE_AS6,NOTE_B6,NOTE_C7,NOTE_CS7,NOTE_D7,NOTE_DS7,NOTE_E7,NOTE_F7,NOTE_FS7,NOTE_G7,NOTE_GS7,NOTE_A7,NOTE_AS7,NOTE_B7,NOTE_C8,NOTE_CS8,NOTE_D8,NOTE_DS8};

void handleWsMsg(char * msg){
  cmdProcessor.processMsg(msg);
}

void sendSerialMsg(ArduinoJson::JsonObject &outMsg){
  outMsg.printTo(Serial);
  Serial.println();
}

void sendSerialMsgV1(ArduinoJson::JsonObject &outMsg){
  v1ws.send(outMsg);
}

Evebrain::Evebrain(){
  blocking = true;
  nextADCRead = 0;
  lastLedChange = millis();
  calibratingSlack = false;
  timeTillComplete = 0;
  humidityRead = 0;
  temperatureRead = 0;
  distanceRead = 0;
  servoMove = 0;
  buzzerBeep = 0;
  wifiEnabled = false;
}

void Evebrain::begin(unsigned char v){
  version(v);

  // Initialise the steppers
  ShiftStepper::setup(SHIFT_REG_DATA, SHIFT_REG_CLOCK, SHIFT_REG_LATCH);
  // Set up the I2C lines for the ADC
  Wire.begin(I2C_DATA, I2C_CLOCK);

  // Set up the EEPROM
  EEPROM.begin(sizeof(settings)+2);

  // Pull the settings out of memory
  initSettings();

  ota.setupOTA();
}

void Evebrain::begin(){
  begin(3);
}

void Evebrain::enableSerial(){
  // Use non-blocking mode to process serial
  blocking = false;
  // Set up the commands
  initCmds();
  // Set up Serial and add it to be processed
  // Add the output handler for responses
  if(hwVersion == 1){
    Serial.begin(57600);
    cmdProcessor.addOutputHandler(sendSerialMsgV1);
  }else if(hwVersion == 2){
    Serial.begin(57600);
    cmdProcessor.addOutputHandler(sendSerialMsg);
  }else if(hwVersion == 3){
    Serial.begin(230400);
    Serial.setTimeout(1);
    cmdProcessor.addOutputHandler(sendSerialMsg);
    Serial.print("{\"status\":\"notify\",\"id\":\"boot\",\"msg\":\"");
    Serial.print(versionStr);
    Serial.println("\"}");
  }
  // Enable serial processing
  serialEnabled = true;
}

void Evebrain::enableWifi(){
  wifi.begin(&settings);
  wifi.onMsg(handleWsMsg);
  cmdProcessor.addOutputHandler(wifi.sendWebSocketMsg);
  wifiEnabled = true;
}

void Evebrain::initSettings(){
  if(EEPROM.read(EEPROM_OFFSET) == MAGIC_BYTE_1 && EEPROM.read(EEPROM_OFFSET + 1) == MAGIC_BYTE_2 && EEPROM.read(EEPROM_OFFSET + 2) == SETTINGS_VERSION){
    // We've previously written something valid to the EEPROM
    for (unsigned int t=0; t<sizeof(settings); t++){
      *((char*)&settings + t) = EEPROM.read(EEPROM_OFFSET + 2 + t);
    }
    // Sanity check the values to make sure they look correct
    if(settings.settingsVersion == SETTINGS_VERSION &&
       settings.slackCalibration < 50 &&
       settings.moveCalibration > 0.5f &&
       settings.moveCalibration < 1.5f &&
       settings.turnCalibration > 0.5f &&
       settings.turnCalibration < 1.5f){
      // The values look OK so let's leave them as they are
      if (digitalRead(RESET) == 0) {
        return;
      }
    }
  }
  // Either this is the first boot or the settings are bad so let's reset them
  settings.settingsVersion = SETTINGS_VERSION;
  settings.slackCalibration = 14;
  settings.moveCalibration = 1.0f;
  settings.turnCalibration = 1.0f;
  settings.sta_ssid[0] = 0;
  settings.sta_pass[0] = 0;
  settings.sta_dhcp = true;
  settings.sta_fixedip = 0;
  settings.sta_fixedgateway = 0;
  settings.sta_fixednetmask = (uint32_t)IPAddress(255, 255, 255, 0);
  settings.sta_fixeddns1 = 0;
  settings.sta_fixeddns2 = 0;
  EvebrainWifi::defautAPName(settings.ap_ssid);
  settings.ap_pass[0] = 0;
  settings.discovery = true;
  saveSettings();
}

void Evebrain::saveSettings(){
  EEPROM.write(EEPROM_OFFSET, MAGIC_BYTE_1);
  EEPROM.write(EEPROM_OFFSET + 1, MAGIC_BYTE_2);
  for (unsigned int t=0; t<sizeof(settings); t++){
    EEPROM.write(EEPROM_OFFSET + 2 + t, *((char*)&settings + t));
  }
  EEPROM.commit();
}

void Evebrain::initCmds(){
  cmdProcessor.setEvebrain(self());
  //             Command name        Handler function             // Returns immediately
  cmdProcessor.addCmd("version",          &Evebrain::_version,          true);
  cmdProcessor.addCmd("ping",             &Evebrain::_ping,             true);
  cmdProcessor.addCmd("uptime",           &Evebrain::_uptime,           true);
  cmdProcessor.addCmd("pause",            &Evebrain::_pause,            true);
  cmdProcessor.addCmd("resume",           &Evebrain::_resume,           true);
  cmdProcessor.addCmd("stop",             &Evebrain::_stop,             true);
  cmdProcessor.addCmd("slackCalibration", &Evebrain::_slackCalibration, true);
  cmdProcessor.addCmd("moveCalibration",  &Evebrain::_moveCalibration,  true);
  cmdProcessor.addCmd("turnCalibration",  &Evebrain::_turnCalibration,  true);
  cmdProcessor.addCmd("calibrateMove",    &Evebrain::_calibrateMove,    true);
  cmdProcessor.addCmd("calibrateTurn",    &Evebrain::_calibrateTurn,    true);
  cmdProcessor.addCmd("forward",          &Evebrain::_forward,          false);
  cmdProcessor.addCmd("back",             &Evebrain::_back,             false);
  cmdProcessor.addCmd("right",            &Evebrain::_right,            false);
  cmdProcessor.addCmd("left",             &Evebrain::_left,             false);
  cmdProcessor.addCmd("beep",             &Evebrain::_beep,             false);
  cmdProcessor.addCmd("calibrateSlack",   &Evebrain::_calibrateSlack,   false);
  cmdProcessor.addCmd("analogInput",      &Evebrain::_analogInput,      true);
  cmdProcessor.addCmd("readSensors",      &Evebrain::_readSensors,      false);
  cmdProcessor.addCmd("digitalInput",     &Evebrain::_digitalInput,     true);
  cmdProcessor.addCmd("gpio_on",          &Evebrain::_gpio_on,          true);
  cmdProcessor.addCmd("gpio_off",         &Evebrain::_gpio_off,         true);
  cmdProcessor.addCmd("gpio_pwm_16",      &Evebrain::_gpio_pwm_16,      true);
  cmdProcessor.addCmd("gpio_pwm_5",       &Evebrain::_gpio_pwm_5,       true);
  cmdProcessor.addCmd("gpio_pwm_10",      &Evebrain::_gpio_pwm_10,      true);
  cmdProcessor.addCmd("temperature",      &Evebrain::_temperature,      false);
  cmdProcessor.addCmd("humidity",         &Evebrain::_humidity,         false);
  cmdProcessor.addCmd("distanceSensor",   &Evebrain::_distanceSensor,   false);
  cmdProcessor.addCmd("leftMotorF",       &Evebrain::_leftMotorForward, false);
  cmdProcessor.addCmd("leftMotorB",       &Evebrain::_leftMotorBackward,false);
  cmdProcessor.addCmd("rightMotorF",      &Evebrain::_rightMotorForward,false);
  cmdProcessor.addCmd("rightMotorB",      &Evebrain::_rightMotorBackward,false);
  cmdProcessor.addCmd("servo",            &Evebrain::_servo,            false);
  cmdProcessor.addCmd("getConfig",        &Evebrain::_getConfig,        true);
  cmdProcessor.addCmd("setConfig",        &Evebrain::_setConfig,        true);
  cmdProcessor.addCmd("resetConfig",      &Evebrain::_resetConfig,      true);
  cmdProcessor.addCmd("freeHeap",         &Evebrain::_freeHeap,         true);
  cmdProcessor.addCmd("startWifiScan",    &Evebrain::_startWifiScan,    true);
}

void Evebrain::_version(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  outJson["msg"] = versionStr;
}

void Evebrain::_ping(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){}

void Evebrain::_uptime(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  outJson["msg"] = millis();
}

void Evebrain::_pause(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  pause();
}

void Evebrain::_resume(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  resume();
}

void Evebrain::_stop(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  stop();
}


void Evebrain::_slackCalibration(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  outJson["msg"] = settings.slackCalibration;
}

void Evebrain::_moveCalibration(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  outJson["msg"] = settings.moveCalibration;
}

void Evebrain::_turnCalibration(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  outJson["msg"] = settings.turnCalibration;
}

void Evebrain::_calibrateMove(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  calibrateMove(atof(inJson["arg"].asString()));
}

void Evebrain::_calibrateTurn(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  calibrateTurn(atof(inJson["arg"].asString()));
}

void Evebrain::_forward(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  forward(atoi(inJson["arg"].asString()));
}

void Evebrain::_back(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  back(atoi(inJson["arg"].asString()));
}

void Evebrain::_right(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  right(atoi(inJson["arg"].asString()));
}

void Evebrain::_left(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  left(atoi(inJson["arg"].asString()));
}

void Evebrain::_leftMotorForward(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  leftMotorForward(atoi(inJson["arg"].asString()));
}

void Evebrain::_rightMotorForward(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  rightMotorForward(atoi(inJson["arg"].asString()));
}

void Evebrain::_leftMotorBackward(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  leftMotorBackward(atoi(inJson["arg"].asString()));
}

void Evebrain::_rightMotorBackward(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  rightMotorBackward(atoi(inJson["arg"].asString()));
}

void Evebrain::_servo(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  servo(atoi(inJson["arg"].asString()));
}

void Evebrain::_beep(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  char s[16];
  strcpy(s, inJson["arg"].asString());
  char* token1 = strtok(s, ",");
  char* token2 = strtok(NULL, ",");
  beep(atoi(token1), atoi(token2));
}

void Evebrain::_calibrateSlack(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  calibrateSlack(atoi(inJson["arg"].asString()));
}

void Evebrain::_analogInput(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  outJson["msg"] = analogInput();
}

void Evebrain::_readSensors(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  readSensors(atoi(inJson["arg"].asString()));
}

void Evebrain::_distanceSensor(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  distanceSensor();
}

void Evebrain::_temperature(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  temperature();
}

void Evebrain::_humidity(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  humidity();
}

void Evebrain::_gpio_off(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  gpio_off(atoi(inJson["arg"].asString()));
}

void Evebrain::_gpio_on(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  gpio_on(atoi(inJson["arg"].asString()));
}

void Evebrain::_digitalInput(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  outJson["msg"] = digitalInput(atoi(inJson["arg"].asString()));
}

void Evebrain::_gpio_pwm_16(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  gpio_pwm(16,atoi(inJson["arg"].asString()));
}

void Evebrain::_gpio_pwm_5(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  gpio_pwm(5,atoi(inJson["arg"].asString()));
}

void Evebrain::_gpio_pwm_10(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson ) {
  gpio_pwm(10,atoi(inJson["arg"].asString()));
}

void Evebrain::_getConfig(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  JsonObject& msg = outJson.createNestedObject("msg");
  const char *modes[] = {"OFF","STA","AP","APSTA"};
  msg["sta_ssid"] = settings.sta_ssid;
  msg["sta_dhcp"] = settings.sta_dhcp;
  msg["sta_rssi"] = EvebrainWifi::getStaRSSI();
  if(!settings.sta_dhcp){
    msg["sta_fixedip"] = IPAddress(settings.sta_fixedip).toString();
    msg["sta_fixedgateway"] = IPAddress(settings.sta_fixedgateway).toString();
    msg["sta_fixednetmask"] = IPAddress(settings.sta_fixednetmask).toString();
  }
  msg["sta_ip"] = EvebrainWifi::getStaIp().toString();
  msg["ap_ssid"] = settings.ap_ssid;
  msg["ap_encrypted"] = !!strlen(settings.ap_pass);
  msg["discovery"] = settings.discovery;
  msg["wifi_mode"] = modes[EvebrainWifi::getWifiMode()];
}

void Evebrain::_setConfig(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  IPAddress addr;
  if(!(inJson.containsKey("arg") && inJson["arg"].is<JsonObject&>())) return;

  // Set the SSID to connect to
  if(inJson["arg"].asObject().containsKey("sta_ssid")){
    strcpy(settings.sta_ssid, inJson["arg"]["sta_ssid"]);
  }
  // Set the password for the SSID
  if(inJson["arg"].asObject().containsKey("sta_pass")){
    strcpy(settings.sta_pass, inJson["arg"]["sta_pass"]);
  }
  // Change the name of the built in access point
  if(inJson["arg"].asObject().containsKey("ap_ssid")){
    strcpy(settings.ap_ssid, inJson["arg"]["ap_ssid"]);
  }
  // Set the password for the access point
  if(inJson["arg"].asObject().containsKey("ap_pass")){
    strcpy(settings.ap_pass, inJson["arg"]["ap_pass"]);
  }
  // Set whether to use DHCP
  if(inJson["arg"].asObject().containsKey("sta_dhcp")){
    settings.sta_dhcp = inJson["arg"]["sta_dhcp"];
  }
  // Use a fixed IP address
  if(inJson["arg"].asObject().containsKey("sta_fixedip")){
    addr.fromString(inJson["arg"]["sta_fixedip"].asString());
    settings.sta_fixedip = addr;
  }
  // The DNS server to use for the fixed IP
  if(inJson["arg"].asObject().containsKey("sta_fixedgateway")){
    addr.fromString(inJson["arg"]["sta_fixedgateway"].asString());
    settings.sta_fixedgateway = addr;
  }
  // The netmask to use for the fixed IP
  if(inJson["arg"].asObject().containsKey("sta_fixednetmask")){
    addr.fromString(inJson["arg"]["sta_fixednetmask"].asString());
    settings.sta_fixednetmask = addr;
  }
  // The netmask to use for the fixed IP
  if(inJson["arg"].asObject().containsKey("sta_fixeddns1")){
    addr.fromString(inJson["arg"]["sta_fixeddns1"].asString());
    settings.sta_fixeddns1 = addr;
  }
  // The netmask to use for the fixed IP
  if(inJson["arg"].asObject().containsKey("sta_fixeddns2")){
    addr.fromString(inJson["arg"]["sta_fixeddns2"].asString());
    settings.sta_fixeddns2 = addr;
  }
  // The netmask to use for the fixed IP
  if(inJson["arg"].asObject().containsKey("discovery")){
    settings.discovery = inJson["arg"]["discovery"];
  }
  wifi.setupWifi();
  saveSettings();
}
void Evebrain::_resetConfig(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  settings.settingsVersion = 0;
  saveSettings();
  initSettings();
  wifi.setupWifi();
}
void Evebrain::_freeHeap(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  outJson["msg"] = ESP.getFreeHeap();
}
void Evebrain::_startWifiScan(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  EvebrainWifi::startWifiScan();
}

void Evebrain::takeUpSlack(byte rightMotorDir, byte leftMotorDir){
  // Take up the slack on each motor
  if(rightMotor.lastDirection != rightMotorDir){
    rightMotor.turn(settings.slackCalibration, rightMotorDir);
  }
  if(leftMotor.lastDirection != leftMotorDir){
    leftMotor.turn(settings.slackCalibration, leftMotorDir);
  }
}

void Evebrain::forward(int distance){
  takeUpSlack(FORWARD, BACKWARD);
  rightMotor.turn(distance * steps_per_mm * settings.moveCalibration, FORWARD);
  leftMotor.turn(distance * steps_per_mm * settings.moveCalibration, BACKWARD);
  wait();
}

void Evebrain::back(int distance){
  takeUpSlack(BACKWARD, FORWARD);
  rightMotor.turn(distance * steps_per_mm * settings.moveCalibration, BACKWARD);
  leftMotor.turn(distance * steps_per_mm * settings.moveCalibration, FORWARD);
  wait();
}

void Evebrain::left(int angle){
  takeUpSlack(FORWARD, FORWARD);
  rightMotor.turn(angle * steps_per_degree * settings.turnCalibration, FORWARD);
  leftMotor.turn(angle * steps_per_degree * settings.turnCalibration, FORWARD);
  wait();
}

void Evebrain::right(int angle){
  takeUpSlack(BACKWARD, BACKWARD);
  rightMotor.turn(angle * steps_per_degree * settings.turnCalibration, BACKWARD);
  leftMotor.turn(angle * steps_per_degree * settings.turnCalibration, BACKWARD);
  wait();
}

void Evebrain::pause(){
  rightMotor.pause();
  leftMotor.pause();
  paused = true;
}

void Evebrain::resume(){
  rightMotor.resume();
  leftMotor.resume();
  paused = false;
}

void Evebrain::stop(){
  rightMotor.stop();
  leftMotor.stop();
  calibratingSlack = false;
}

void Evebrain::beep(int semi_tone, int duration){
  if (semi_tone>=0 && semi_tone<=88){
    tone(SPEAKER_PIN, frequencyHZ[semi_tone]);
  }
  timeTillComplete = millis() + duration;
  wait();
  buzzerBeep = 1;
}

short Evebrain::analogInput(){
  return analogRead(0);
}

void Evebrain::temperature(){
  //Setup DHT11
  dht.setup(DHTPIN,DHTesp::DHT11);
  timeTillComplete = millis() + 1500;
  wait();
  temperatureRead = 1;
}

void Evebrain::humidity(){
  //Setup DHT11
  dht.setup(DHTPIN,DHTesp::DHT11);
  timeTillComplete = millis() + 1500;
  wait();
  humidityRead = 1;
}

void Evebrain::distanceSensor(){
  pinMode(TRIGPIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHOPIN, INPUT); // Sets the echoPin as an Input
  // Clears the trigPin
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHOPIN, HIGH, 20000);
  // Calculating the distance
  distanceVar = (duration/2) / 29.1;
  timeTillComplete = millis() + 250;
  distanceRead = 1;
  wait();
}

short Evebrain::digitalInput(byte pin){
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT); 
  return digitalRead(pin);
}

void Evebrain::gpio_on(byte pin){
  pinMode(pin, OUTPUT); 
  digitalWrite(pin, HIGH);
}

void Evebrain::gpio_off(byte pin){
  pinMode(pin, OUTPUT); 
  digitalWrite(pin, LOW);
}

void Evebrain::gpio_pwm(byte pin, byte value){
  analogWrite(pin, value);
}

void Evebrain::leftMotorForward(int distance){
  takeUpSlack(BACKWARD, FORWARD);
  rightMotor.turn(distance * steps_per_degree * settings.turnCalibration, BACKWARD,1);
  leftMotor.turn(distance * steps_per_degree * settings.turnCalibration, FORWARD);
  wait();
}

void Evebrain::servo(int angle){
  // Set up the servo
  servoid.attach(SERVO_PIN);
  servoid.write(angle);
  timeTillComplete = millis() + 1000;
  wait();
  servoMove = 1;
}

void Evebrain::rightMotorForward(int distance){
  takeUpSlack(FORWARD, BACKWARD);
  rightMotor.turn(distance * steps_per_degree * settings.turnCalibration, FORWARD);
  leftMotor.turn(distance * steps_per_degree * settings.turnCalibration, BACKWARD,1);
  wait();
}

void Evebrain::leftMotorBackward(int distance){
  takeUpSlack(FORWARD, BACKWARD);
  rightMotor.turn(distance * steps_per_degree * settings.turnCalibration, FORWARD,1);
  leftMotor.turn(distance * steps_per_degree * settings.turnCalibration, BACKWARD);
  wait();
}

void Evebrain::rightMotorBackward(int distance){
  takeUpSlack(BACKWARD, FORWARD);
  rightMotor.turn(distance * steps_per_degree * settings.turnCalibration, BACKWARD);
  leftMotor.turn(distance * steps_per_degree * settings.turnCalibration, FORWARD,1);
  wait();
}

boolean Evebrain::ready(){
  return (rightMotor.ready() && leftMotor.ready() && timeTillComplete < millis());
}

void Evebrain::wait(){
  if(blocking){
    while(!ready()){
    }
  }
}

void Evebrain::ledHandler(){
  long t = millis();
  uint8_t val = (abs((millis() % (uint32_t)LED_PULSE_TIME) - LED_PULSE_TIME/2) / (LED_PULSE_TIME/2)) * 50;
  led.setRGBA(LED_COLOUR_NORMAL, val);
}

void Evebrain::autoHandler(){
}

void Evebrain::readSensors(byte pin){
  uint8_t temp[4];
  // Fetch the data from the ADC
  Wire.beginTransmission(PCF8591_ADDRESS); // wake up PCF8591
  Wire.write(0x04); // control byte - read ADC0 and increment counter
  Wire.endTransmission();
  Wire.requestFrom(PCF8591_ADDRESS, 6);
  Wire.read(); // Padding bytes to allow conversion to complete
  Wire.read(); // Padding bytes to allow conversion to complete
  temp[0] = Wire.read();
  temp[1] = Wire.read();
  temp[2] = Wire.read();
  temp[3] = Wire.read();

  if(pin >=0 && pin <= 3){
    analogSensor = temp[pin];
  } else {
    analogSensor = 0;
  }

  timeTillComplete = millis() + 50;
  nextADCRead = 1;
  wait();
}

// This allows for runtime configuration of which hardware is used
void Evebrain::version(char v){
  hwVersion = v;
  sprintf(versionStr, "%d.%s", hwVersion, Evebrain_SUB_VERSION);
  steps_per_mm = STEPS_PER_MM_V2;
  steps_per_degree = STEPS_PER_DEGREE_V2;
  wheel_distance = WHEEL_DISTANCE_V2;
}


void Evebrain::networkNotifier(){
  if(!EvebrainWifi::networkChanged) return;
  StaticJsonBuffer<500> outBuffer;
  JsonObject& outMsg = outBuffer.createObject();
  EvebrainWifi::networkChanged = false;
  _getConfig(outMsg, outMsg);
  cmdProcessor.notify("network", outMsg);
}

void Evebrain::wifiScanNotifier(){
  if(!EvebrainWifi::wifiScanReady) return;
  StaticJsonBuffer<2000> outBuffer;
  JsonObject& outMsg = outBuffer.createObject();
  JsonArray& msg = outMsg.createNestedArray("msg");
  EvebrainWifi::wifiScanReady = false;
  wifi.getWifiScanData(msg);
  timeTillComplete = millis() + 150;
  wait();
  cmdProcessor.notify("wifiScan", outMsg);
}


void Evebrain::calibrateSlack(unsigned int amount){
  settings.slackCalibration = amount;
  saveSettings();
  calibratingSlack = true;
  rightMotor.turn(1, FORWARD);
  leftMotor.turn(1, BACKWARD);
}

void Evebrain::calibrateMove(float amount){
  settings.moveCalibration = amount;
  saveSettings();
}

void Evebrain::calibrateTurn(float amount){
  settings.turnCalibration = amount;
  saveSettings();
}

void Evebrain::calibrateHandler(){
  if(calibratingSlack && rightMotor.ready() && leftMotor.ready()){
    takeUpSlack((rightMotor.lastDirection == FORWARD ? BACKWARD : FORWARD), (leftMotor.lastDirection == FORWARD ? BACKWARD : FORWARD));
  }
}

void Evebrain::serialHandler(){
  int s;
  if(!serialEnabled) return;
  // process incoming data
  s = Serial.available();
  if (s > 0){
    for(int i = 0; i<s; i++){
      last_char = millis();
      char incomingByte = Serial.read();
      if(hwVersion == 1){
        // Handle the WebSocket parsing over serial for v1
        serial_buffer[serial_buffer_pos++] = incomingByte;
        if(serial_buffer_pos == SERIAL_BUFFER_LENGTH) serial_buffer_pos = 0;
        processState_t res = v1ws.process(serial_buffer, serial_buffer_pos);
        // Handle as a stream of commands
        if(res == SERWS_FRAME_PROCESSED){
          // It's been successfully processed as a line
          cmdProcessor.processMsg(serial_buffer);
          serial_buffer_pos = 0;
        }else if(res == SERWS_HEADERS_PROCESSED || res == SERWS_FRAME_ERROR || res == SERWS_FRAME_EMPTY){
          serial_buffer_pos = 0;
        }
      }else{
        // Handle as a stream of commands
        if((incomingByte == '\r' || incomingByte == '\n') && serial_buffer_pos && cmdProcessor.processMsg(serial_buffer)){
          // It's been successfully processed as a line
          serial_buffer_pos = 0;
        }else{
          // Not a line to process so store for processing
          serial_buffer[serial_buffer_pos++] = incomingByte;
          if(serial_buffer_pos == SERIAL_BUFFER_LENGTH) serial_buffer_pos = 0;
          serial_buffer[serial_buffer_pos] = 0;
        }
      }
    }
  }else{
    //reset the input buffer if nothing is received for 1/2 second to avoid things getting messed up
    if(millis() - last_char >= 500){
      serial_buffer_pos = 0;
    }
  }
}

void Evebrain::checkReady(){
  char snum[4];
  if(cmdProcessor.in_process && ready()){
    //if temperature ready is ready
    if (temperatureRead){
      temperatureVar = dht.getTemperature();
      cmdProcessor.sendCompleteMSG(itoa(temperatureVar, snum, 10));
      temperatureRead = 0;
      Serial.println(temperatureVar);
    }
    //if humidity is ready
    else if (humidityRead){
      humidityVar = dht.getHumidity();
      cmdProcessor.sendCompleteMSG(itoa(humidityVar, snum, 10));
      humidityRead = 0;
    } 
    //if distance is ready
    else if (distanceRead){
      cmdProcessor.sendCompleteMSG(itoa(distanceVar, snum, 10));
      distanceRead = 0;
    } 
    //buzzer is done
    else if (buzzerBeep){
      noTone(SPEAKER_PIN);
      cmdProcessor.sendComplete();
      buzzerBeep = 0;
    } 
    else if (servoMove){
      servoid.detach();
      cmdProcessor.sendComplete();
      servoMove = 0;
    }
    else if (nextADCRead){
      cmdProcessor.sendCompleteMSG(itoa(analogSensor, snum, 10));
      nextADCRead = 0;
    }
    //if there is no message on complete
    else {
      cmdProcessor.sendComplete();
      leftMotor.setFakeout(0);
      rightMotor.setFakeout(0);
    }
  }
}

void Evebrain::loop(){
  ledHandler();
  autoHandler();
  calibrateHandler();
  networkNotifier();
  wifiScanNotifier();
  if(wifiEnabled){
    wifi.run();
  }
  serialHandler();
  checkReady();
  ota.runOTA();
}
