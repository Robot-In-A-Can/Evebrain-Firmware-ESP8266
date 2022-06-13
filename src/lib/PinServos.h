#ifndef GENERIC_SERVO_H
#define GENERIC_SERVO_H

// The number of servos that can be driven 'normally'
#define NUMBER_OF_SERVO_PINS 9

class ManualServo {
public:
    ManualServo();
    void start(int pin, int angle);
    void stop();
    void poll();
    bool isRunning();
private:
    unsigned long nextServoPulse;
    int angle;
    int pin;
    bool running;
};

class PinServos
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
    // These members are all for the 9 pins that can be driven
    static int validPins[NUMBER_OF_SERVO_PINS];
    static ManualServo servos[NUMBER_OF_SERVO_PINS];
    static unsigned long timesStarted[NUMBER_OF_SERVO_PINS];
    static const int MILLIS_BEFORE_STOP = 1000;
};

#endif