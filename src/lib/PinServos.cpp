#include "PinServos.h"
#include "Arduino.h"

ManualServo::ManualServo():pin(-1), angle(0), running(false), nextServoPulse(0) {

}

void ManualServo::start(int pin, int angle) {
    this->pin = pin;
    this->angle = angle;
    running = true;
    nextServoPulse = micros();
}

void ManualServo::stop() {
    running = false;
    if (pin == 10) {
        // now done (running for more than the 1000ms)
        // set the servo to stop, and pull pin 10 HIGH as a precaution
        digitalWrite(10, HIGH);
    }
}

void ManualServo::poll() {
    if(running){
        if(micros() >= nextServoPulse){
            int highTime = map(angle, 0, 180, 500, 2500);
            digitalWrite(pin, HIGH);
            delayMicroseconds(highTime);
            digitalWrite(pin, LOW);
            nextServoPulse = micros() + (12000 - highTime);
        }
    }
}

bool ManualServo::isRunning() {
    return running;
}

bool PinServos::pinValidForServo(int pin) {
    return servoPinToIndex(pin) != -1;
}

bool PinServos::startServo(int angle, int pin) {
    angle = constrain(angle, 0, 180);

    int index = servoPinToIndex(pin);
    if (index != -1) {
        pinMode(pin, OUTPUT);
        servos[index].start(pin, angle);
        timesStarted[index] = millis();
        return true;
    } else {
        return false;
    }
}

void PinServos::poll() {
    for (int i = 0; i < NUMBER_OF_SERVO_PINS; i++) {
        servos[i].poll();
        if (servos[i].isRunning() &&
            millis() - timesStarted[i] >= MILLIS_BEFORE_STOP) {
            servos[i].stop();
        }
    }
}

int PinServos::servoPinToIndex(int pin) {
    for (int i = 0; i < NUMBER_OF_SERVO_PINS; i++) {
        if (validPins[i] == pin) {
            return i;
        }
    }
    return -1;
}

int PinServos::validPins[NUMBER_OF_SERVO_PINS] = {0, 2, 4, 5, 10, 16, 14, 12, 13};
ManualServo PinServos::servos[NUMBER_OF_SERVO_PINS] = {ManualServo(), ManualServo(), ManualServo(), ManualServo(),
                                                    ManualServo(), ManualServo(), ManualServo(), ManualServo(), ManualServo()};
unsigned long PinServos::timesStarted[NUMBER_OF_SERVO_PINS] = {0};