#include "GenericServo.h"

bool GenericServo::pinValidForServo(int pin) {
    return pin == 10 || servoPinToIndex(pin) != -1;
}

bool GenericServo::startServo(int angle, int pin) {
    angle = constrain(angle, 0, 180);

    if (pin == 10) {
        startServo10(angle);
        return true;
    }

    int index = servoPinToIndex(pin);
    if (index != -1) {
        pinMode(pin, OUTPUT);
        servos[index].attach(pin,500,2500);
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
    pollServo10();
}

void GenericServo::startServo10(int angle) {
    pinMode(10, OUTPUT);
    servoTimeStarted = millis();
    nextServoPulse = 0;
    servoPosition = angle;
    servoRunning = true;
}

void GenericServo::pollServo10() {
    if(millis() - servoTimeStarted <= MILLIS_BEFORE_STOP){
        if(micros() >= nextServoPulse){
            digitalWrite(10, HIGH);
            int highTime = map(servoPosition, 0, 180, 500, 2500);
            delayMicroseconds(highTime);
            digitalWrite(10, LOW);
            nextServoPulse = micros() + (12000 - highTime);
        }
    } else if (servoRunning) {
        // now done (running for more than the 1000ms)
        // set the servo to stop, and pull pin 10 HIGH as a precaution
        digitalWrite(10, HIGH);
        servoRunning = false;
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
int GenericServo::servoTimeStarted = 0;
int GenericServo::nextServoPulse = 0;
int GenericServo::servoPosition = 0;
bool GenericServo::servoRunning = false;