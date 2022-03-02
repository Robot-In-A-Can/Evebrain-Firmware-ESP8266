#include "Interrupts.h"

PinStateStack::PinStateStack() {

}

bool PinStateStack::push(PinState state) volatile {
    if (full()) {
        return false;
    } else {
        unsigned char charState = state.pin + (state.pinState << 7);
        stack[numElements++] = charState;
        return true;
    }
}
PinState PinStateStack::pop() volatile {
    unsigned char pinState = stack[--numElements];
    return PinState(pinState & 0x7F, pinState >> 7);
}
int PinStateStack::numberOfElements() volatile {
    return numElements;
}
bool PinStateStack::full() volatile {
    return numElements == STACK_SIZE;
}