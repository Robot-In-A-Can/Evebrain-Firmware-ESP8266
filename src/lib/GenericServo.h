#ifndef GENERIC_SERVO_H
#define GENERIC_SERVO_H

#define NUMBER_OF_SERVO_PINS 6

#include <Servo.h>
class GenericServo
{
public:
    // This allows for servos to be used with the other GPIO pins on the board
    // Returns true if pin is valid and was able to start the servo, false otherwise.
    static bool startServo(int angle, int pin);
    // Returns true if the pin can be used to control a servo.
    static bool pinValidForServo(int pin);
    // Must call this function in a loop to ensure the servo will be stopped after a time
    static void poll();

private:
    static int servoPinToIndex(int pin);

    static int validPins[NUMBER_OF_SERVO_PINS];
    static Servo servos[NUMBER_OF_SERVO_PINS];
    static unsigned long timesStarted[NUMBER_OF_SERVO_PINS];
    static const int MILLIS_BEFORE_STOP = 1000;
};

#endif