#ifndef __ShiftStepper_h__
#define __ShiftStepper_h__
#include "Arduino.h"

#define FORWARD 1
#define BACKWARD 0

#define BASE_INTERRUPT_US 50
#define DEFAULT_STEP_PERIOD 1500
#define UCOUNTER_DEFAULT DEFAULT_STEP_PERIOD/BASE_INTERRUPT_US

#define BATCH_SIZE 10

class ShiftStepper {
  public:
    ShiftStepper(int);
    static void setup(int, int, int);
    void instanceSetup();
    void turn(long steps, byte direction);
    boolean ready();
    static boolean allStopped();
    long remaining();
    void release();
    static void triggerTop();
    void pause();
    void resume();
    void stop();
    byte lastDirection;

    // Sets the speed of the motor for the current move (must be <1); reset back to 1 next time.
    void setRelSpeed(float multiplier);
  private:
    static ShiftStepper *firstInstance;
    ShiftStepper *nextInstance;
    void addNext(ShiftStepper *ref);
    boolean _paused;
    byte _pinmask;
    volatile long _remaining;
    byte _dir;
    
    // Both of these are related to slow operation. The way that works is that
    // it sends a batch of 10 pulses, then waits; that wait makes the stepper go more slowly.

    // How many timer triggers that are skipped between batches for the slowdown
    volatile int cyclesToWait;
    // Counts the number of timer triggers remaining to wait out for the slowdown
    volatile int _remainingCyclesToSlowdown;
    // Number of pulses remaining in batch.
    volatile byte _remainingInBatch;

    byte nextStep();
    void setStep(byte);
    void setNextStep();
    void setNextStepFullspeed();
    void setNextStepSlowdown();
    void trigger();
    byte currentStep;
    static int data_pin;
    static int clock_pin;
    static int latch_pin;
    int microCounter;
    int motor_offset;
    static uint8_t lastBits;
    static uint8_t currentBits;
    void updateBits(uint8_t bits);
    static void sendBits();
    static void startTimer();
    static void stopTimer();
};

#endif
