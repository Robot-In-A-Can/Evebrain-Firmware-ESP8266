#include "GenericServo.h"

bool GenericServo::pinValidForServo(int pin) {
    return servoPinToIndex(pin) != -1;
}

bool GenericServo::startServo(int angle, int pin) {
    int index = servoPinToIndex(pin);
    angle = constrain(angle, 0, 180);

    if (index != -1) {
        pinMode(pin, OUTPUT);
        servos[index].attach(pin,500,2200);
        servos[index].write(angle);
        timesStarted[index] = millis();
        return true;
    } else {
        return false;
    }
}

void GenericServo::poll() {
    for (int i = 0; i < NUMBER_OF_SERVO_PINS; i++) {
        if (servos[i].attached() &&
            millis() - timesStarted[i] >= MILLIS_BEFORE_STOP) {
            servos[i].detach();
        }
    }
}

int GenericServo::servoPinToIndex(int pin) {
    for (int i = 0; i < NUMBER_OF_SERVO_PINS; i++) {
        if (validPins[i] == pin) {
            return i;
        }
    }
    return -1;
}

int GenericServo::validPins[NUMBER_OF_SERVO_PINS] = {0, 2, 4, 5, 16, 14, 12, 13};
Servo GenericServo::servos[NUMBER_OF_SERVO_PINS] = {Servo(), Servo(), Servo(), Servo(), Servo(), Servo(), Servo(), Servo()};
unsigned long GenericServo::timesStarted[NUMBER_OF_SERVO_PINS] = {0};