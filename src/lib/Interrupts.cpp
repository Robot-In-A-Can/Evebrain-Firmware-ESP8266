#include "Interrupts.h"

PinStateStack::PinStateStack() {

}

bool PinStateStack::push(PinState state) volatile {
    if (full()) {
        return false;
    } else {
        unsigned char charState = state.pin + (state.pinState << 7);
        stack[numElements] = charState;
        numElements++;
        return true;
    }
}
PinState PinStateStack::pop() volatile {
    if (numElements > 0) {
        numElements--;
        unsigned char pinState = stack[numElements];
        return PinState(pinState & 0x7F, pinState >> 7);
    } else {
        return PinState::invalid;
    }
}
int PinStateStack::numberOfElements() volatile {
    return numElements;
}
bool PinStateStack::full() volatile {
    return numElements == STACK_SIZE;
}

PinState PinState::invalid(127, 0);
bool PinState::operator==(const PinState& other) {
    return pin == other.pin && pinState == other.pinState;
}

bool PinState::operator!=(const PinState& other) {
    return !(*this == other);
}