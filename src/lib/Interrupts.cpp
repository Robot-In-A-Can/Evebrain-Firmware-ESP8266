#include "Interrupts.h"
#include "SimplyAtomic/SimplyAtomic.h"

PinStateQueue::PinStateQueue() {
}

bool PinStateQueue::push(PinState state) {
    bool out;
    ATOMIC() {
        if (full()) {
            out = false;
        } else {
            int index = (numElements + firstIndex) % QUEUE_SIZE;
            stack[index] = state;
            numElements++;
            out = true;
        }
    }
    return out;
}
PinState PinStateQueue::pop() {
    PinState out = PinState::invalid;
    ATOMIC() {
        if (numElements > 0) {
            out = stack[firstIndex++];
            firstIndex %= QUEUE_SIZE;
            numElements--;
        }
    }
    return out;
}
int PinStateQueue::numberOfElements() {
    return numElements;
}
bool PinStateQueue::full() {
    return numElements == QUEUE_SIZE;
}

PinState PinState::invalid(127, 0);

PinState::PinState(unsigned char pin, unsigned char pinState) : 
                   pin(pin), pinState(pinState) {}

PinState::PinState() : pin(0), pinState(0) {}

bool PinState::operator==(const PinState& other) {
    return pin == other.pin && pinState == other.pinState;
}

bool PinState::operator!=(const PinState& other) {
    return !(*this == other);
}
