#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "Arduino.h"

// Define the list of pins we can add an interrupt handler on
#define INTERRUPTABLE_PINS X(4) X(14) X(12) X(13) X(0) X(2)

class PinState {
    public:
    PinState(unsigned char pin, unsigned char pinState): pin(pin), pinState(pinState) {}
    PinState(): pin(0), pinState(0) {}
    unsigned char pin, pinState;
};

#define STACK_SIZE 10

class PinStateStack {
public:
    PinStateStack();
    bool push(PinState state) volatile;
    PinState pop() volatile;
    int numberOfElements() volatile;
    bool full() volatile;
private:
    // NOTE: each element of the stack has the pin number, and the MSbit is the state of the pin 
    volatile unsigned stack[STACK_SIZE];
    volatile unsigned char numElements = 0;
};

#define MAKE_ISR_FOR_PIN(X, NOTIFYSTACK)                                           \
  ICACHE_RAM_ATTR void pin##X##ISR()                                               \
  {                                                                                \
    if ((NOTIFYSTACK).full())                                                      \
    {                                                                              \
      return; /* don't add notification if buffer full*/                           \
    }                                                                              \
    else                                                                           \
    {                                                                              \
      (NOTIFYSTACK).push( PinState((X), digitalRead((X)) == HIGH ? 1 : 0));        \
    }                                                                              \
  }

#endif