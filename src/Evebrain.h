#ifndef Evebrain_h
#define Evebrain_h

#include "Arduino.h"
#include "lib/CmdProcessor.h"
#include "lib/SerialWebSocket.h"
#include "lib/ArduinoJson/ArduinoJson.h"
#include <EEPROM.h>
#include <Servo.h>
#include "Wire.h"
#include "lib/ShiftStepper.h"
#include "lib/EvebrainWifi.h"
#include "lib/EvebrainWebSocket.h"
#include "lib/WS2812B.h"
#include "lib/notes.h"

#define FORCE_SETUP 0
#define SERIAL_BUFFER_LENGTH 180

// The steppers have a gear ratio of 1:63.7 and have 32 steps per turn. 32 x 63.7 = 2038.4
#define STEPS_PER_TURN    2038.0f

#define CIRCUMFERENCE_MM_V2  254.4f
#define WHEEL_DISTANCE_V2    108.5f
#define PENUP_DELAY_V2 2000
#define PENDOWN_DELAY_V2 1100
#define STEPS_PER_MM_V2      STEPS_PER_TURN / CIRCUMFERENCE_MM_V2
#define STEPS_PER_DEGREE_V2  ((WHEEL_DISTANCE_V2 * 3.1416) / 360) * STEPS_PER_MM_V2


#define Evebrain_SUB_VERSION "1.0"

#define EEPROM_OFFSET 0
#define MAGIC_BYTE_1 0xF0
#define MAGIC_BYTE_2 0x0D
#define SETTINGS_VERSION 1

#define DHTPIN 16 
#define TRIGPIN 5
#define ECHOPIN 4
#define SPEAKER_PIN 5
#define SERVO_PIN 10
#define SHIFT_REG_DATA  12
#define SHIFT_REG_CLOCK 13
#define SHIFT_REG_LATCH 14

#define STATUS_LED_PIN 15
#define LED_PULSE_TIME 6000.0
#define LED_COLOUR_NORMAL 0xFFFFFF

#define PCF8591_ADDRESS B1001000
#define I2C_DATA  0
#define I2C_CLOCK 2

struct EvebrainSettings {
  uint8_t      settingsVersion;
  unsigned int slackCalibration;
  float        moveCalibration;
  float        turnCalibration;
  char         sta_ssid[32];
  char         sta_pass[64];
  bool         sta_dhcp;
  uint32_t     sta_fixedip;
  uint32_t     sta_fixedgateway;
  uint32_t     sta_fixednetmask;
  uint32_t     sta_fixeddns1;
  uint32_t     sta_fixeddns2;
  char         ap_ssid[32];
  char         ap_pass[64];
  bool         discovery;
};

class Evebrain {
  public:
    Evebrain();
    void begin();
    void begin(unsigned char);
    void enableSerial();
    void enableWifi();
    void forward(int);
    void back(int);
    void right(int);
    void left(int);
    void pause();
    void resume();
    void stop();
    void beep(int,int);
    short analogInput();
    short digitalInput(byte);
    void gpio_on(byte);
    void gpio_off(byte);
    void gpio_pwm(byte, byte);
    void leftMotorForward(int);
    void rightMotorForward(int);
    void leftMotorBackward(int);
    void rightMotorBackward(int);
    void servo(int);
    void temperature();
    void humidity();
    void distanceSensor();
    void readSensors(byte);
    boolean ready();
    void loop();
    void calibrateSlack(unsigned int);
    void calibrateMove(float);
    void calibrateTurn(float);
    char hwVersion;
    char versionStr[9];
    EvebrainSettings settings;
    boolean blocking;
  private:
    void wait();
    void ledHandler();
    void autoHandler();
    void networkNotifier();
    void wifiScanNotifier();
    void sensorNotifier();
    void checkState();
    void initSettings();
    void saveSettings();
    void checkReady();
    void version(char);
    void initCmds();
    void serialHandler();
    void _version(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _ping(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _uptime(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _pause(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _resume(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _stop(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _slackCalibration(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _moveCalibration(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _turnCalibration(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _calibrateMove(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _calibrateTurn(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _forward(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _back(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _right(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _left(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _beep(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _calibrateSlack(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _leftMotorForward(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _rightMotorForward(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _leftMotorBackward(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _rightMotorBackward(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _analogInput(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _readSensors(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _servo(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _temperature(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _humidity(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _digitalInput(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _gpio_on(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _gpio_off(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _gpio_pwm_16(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _gpio_pwm_10(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _gpio_pwm_5(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _distanceSensor(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _getConfig(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _setConfig(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _resetConfig(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _freeHeap(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    void _startWifiScan(ArduinoJson::JsonObject &, ArduinoJson::JsonObject &);
    long duration;
    byte distanceVar;
    float temperatureVar;
    float humidityVar;
    int analogSensor;
    boolean humidityRead;
    boolean temperatureRead;
    boolean distanceRead;
    boolean buzzerBeep;
    boolean servoMove;
    boolean nextADCRead;
    byte servoPosition;
    unsigned long lastLedChange;
    Evebrain& self() { return *this; }
    void takeUpSlack(byte, byte);
    void calibrateHandler();
    boolean paused;
    float steps_per_mm;
    float steps_per_degree;
    int wheel_distance;
    long timeTillComplete;
    boolean calibratingSlack;
    bool serialEnabled = false;
    unsigned long last_char;
    char serial_buffer[SERIAL_BUFFER_LENGTH];
    int serial_buffer_pos;
    boolean wifiEnabled;
};
#endif
