#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "Arduino.h"

// Define the list of pins we can add an interrupt handler on
#define INTERRUPTABLE_PINS X(4) X(14) X(12) X(13) X(0) X(2)

class PinState {
    public:
    PinState(unsigned char pin, unsigned char pinState): pin(pin), pinState(pinState) {}
    PinState(): pin(0), pinState(0) {}
    bool operator==(const PinState& other);
    bool operator!=(const PinState& other);
    unsigned char pin, pinState;
    static PinState invalid;
};

#define STACK_SIZE 10

/**
 * Implements a queue for pin states. Is O(1) to insert, O(n)
 * to remove, where n is the number of elements in the queue.
 */
class PinStateQueue {
public:
    PinStateQueue();
    bool push(PinState state) volatile;
    PinState pop() volatile;
    int numberOfElements() volatile;
    bool full() volatile;
private:
    // NOTE: each element of the stack has the pin number, and the MSbit is the state of the pin 
    volatile unsigned char stack[STACK_SIZE];
    volatile int numElements = 0;
};

#define MAKE_ISR_FOR_PIN(X, NOTIFYSTACK)                         \
  ICACHE_RAM_ATTR void pin##X##ISR()                             \
  {                                                              \
    int pinValue = digitalRead((X));                             \
    (NOTIFYSTACK).push(PinState((X), pinValue == HIGH ? 1 : 0)); \
  }

#endif