/******************************************************************
  DHT Temperature & Humidity Sensor library for Arduino & ESP32.

  Features:
  - Support for DHT11 and DHT22/AM2302/RHT03
  - Auto detect sensor model
  - Very low memory footprint
  - Very small code

  https://github.com/beegee-tokyo/arduino-DHTesp

  Written by Mark Ruys, mark@paracas.nl.
  Updated to work with ESP32 by Bernd Giesecke, bernd@giesecke.tk

  GNU General Public License, check LICENSE for more information.
  All text above must be included in any redistribution.

  Datasheets:
  - http://www.micro4you.com/files/sensor/DHT11.pdf
  - http://www.adafruit.com/datasheets/DHT22.pdf
  - http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Weather/RHT03.pdf
  - http://meteobox.tk/files/AM2302.pdf

  Changelog:
    2013-06-10: Initial version
    2013-06-12: Refactored code
    2013-07-01: Add a resetTimer method
    2017-12-12: Added task switch disable
                Added computeHeatIndex function from Adafruit DNT library
    2017-12-14: Added computeDewPoint function from idDHTLib Library
                Added getComfortRatio function from libDHT Library
    2017-12-15: Added computePerception function
    2018-01-02: Added example for multiple sensors usage.
    2018-01-03: Added function getTempAndHumidity which returns temperature and humidity in one call.
    2018-01-03: Added retry in case the reading from the sensor fails with a timeout.
    2018-01-08: Added ESP8266 (and probably AVR) compatibility.
	2018-03-11: Updated DHT example    
******************************************************************/

#include "DHTesp.h"

void DHTesp::setup(uint8_t pin, DHT_MODEL_t model)
{
  DHTesp::pin = pin;
  DHTesp::model = model;
  DHTesp::resetTimer(); // Make sure we do read the sensor in the next readSensor()

}

void DHTesp::resetTimer()
{
  DHTesp::lastReadTime = millis() - 3000;
}

float DHTesp::getHumidity()
{
  readSensor();
  if ( error == ERROR_TIMEOUT ) { // Try a second time to read
    readSensor();
  }
  return humidity;
}

float DHTesp::getTemperature()
{
  readSensor();
  if ( error == ERROR_TIMEOUT ) { // Try a second time to read
    readSensor();
  }
  return temperature;
}

TempAndHumidity DHTesp::getTempAndHumidity()
{
  readSensor();
  if ( error == ERROR_TIMEOUT ) { // Try a second time to read
    readSensor();
  }
  values.temperature = temperature;
  values.humidity = humidity;
  return values;
}


const char* DHTesp::getStatusString()
{
  switch ( error ) {
    case DHTesp::ERROR_TIMEOUT:
      return "TIMEOUT";

    case DHTesp::ERROR_CHECKSUM:
      return "CHECKSUM";

    default:
      return "OK";
  }
}



void DHTesp::readSensor()
{
  // Make sure we don't poll the sensor too often
  // - Max sample rate DHT11 is 1 Hz   (duty cicle 1000 ms)
  // - Max sample rate DHT22 is 0.5 Hz (duty cicle 2000 ms)
  unsigned long startTime = millis();
  if ( (unsigned long)(startTime - lastReadTime) < (model == DHT11 ? 999L : 1999L) ) {
    return;
  }
  lastReadTime = startTime;

  temperature = NAN;
  humidity = NAN;

  uint16_t rawHumidity = 0;
  uint16_t rawTemperature = 0;
  uint16_t data = 0;

  // Request sample
  digitalWrite(pin, LOW); // Send start signal
  pinMode(pin, OUTPUT);
  if ( model == DHT11 ) {
    delay(18);
  }
  else {
    // This will fail for a DHT11 - that's how we can detect such a device
    delayMicroseconds(800);
  }

  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH); // Switch bus to receive data

  // We're going to read 83 edges:
  // - First a FALLING, RISING, and FALLING edge for the start bit
  // - Then 40 bits: RISING and then a FALLING edge per bit
  // To keep our code simple, we accept any HIGH or LOW reading if it's max 85 usecs long

  cli();
  for ( int8_t i = -3 ; i < 2 * 40; i++ ) {
    byte age;
    startTime = micros();

    do {
      age = (unsigned long)(micros() - startTime);
      if ( age > 90 ) {
        error = ERROR_TIMEOUT;
        sei();
        return;
      }
    }
    while ( digitalRead(pin) == (i & 1) ? HIGH : LOW );

    if ( i >= 0 && (i & 1) ) {
      // Now we are being fed our 40 bits
      data <<= 1;

      // A zero max 30 usecs, a one at least 68 usecs.
      if ( age > 30 ) {
        data |= 1; // we got a one
      }
    }

    switch ( i ) {
      case 31:
        rawHumidity = data;
        break;
      case 63:
        rawTemperature = data;
        data = 0;
        break;
    }
  }

  sei();

  // Verify checksum

  if ( (byte)(((byte)rawHumidity) + (rawHumidity >> 8) + ((byte)rawTemperature) + (rawTemperature >> 8)) != data ) {
    error = ERROR_CHECKSUM;
    return;
  }

  // Store readings

  if ( model == DHT11 ) {
    humidity = rawHumidity >> 8;
    temperature = rawTemperature >> 8;
  }
  else {
    humidity = rawHumidity * 0.1;

    if ( rawTemperature & 0x8000 ) {
      rawTemperature = -(int16_t)(rawTemperature & 0x7FFF);
    }
    temperature = ((int16_t)rawTemperature) * 0.1;
  }

  error = ERROR_NONE;
}
