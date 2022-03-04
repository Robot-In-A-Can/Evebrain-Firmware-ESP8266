#include "Interrupts.h"

PinStateQueue::PinStateQueue() {

}

bool PinStateQueue::push(PinState state) volatile {
    if (full()) {
        return false;
    } else {
        unsigned char charState = state.pin + (state.pinState << 7);
        stack[numElements++] = charState;
        return true;
    }
}
PinState PinStateQueue::pop() volatile {
    if (numElements > 0) {
        unsigned char pinState = stack[0];
        // Now, copy everything down one slot
        for (int i = 1; i < numElements; i++) {
            stack[i-1] = stack[i];
        }
        numElements--;
        return PinState(pinState & 0x7F, pinState >> 7);
    } else {
        return PinState::invalid;
    }
}
int PinStateQueue::numberOfElements() volatile {
    return numElements;
}
bool PinStateQueue::full() volatile {
    return numElements == STACK_SIZE;
}

PinState PinState::invalid(127, 0);
bool PinState::operator==(const PinState& other) {
    return pin == other.pin && pinState == other.pinState;
}

bool PinState::operator!=(const PinState& other) {
    return !(*this == other);
}