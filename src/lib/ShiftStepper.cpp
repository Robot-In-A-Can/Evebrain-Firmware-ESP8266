#ifdef ESP8266
#include "Arduino.h"
#include "ShiftStepper.h"

ShiftStepper *ShiftStepper::firstInstance;
int ShiftStepper::data_pin;
int ShiftStepper::clock_pin;
int ShiftStepper::latch_pin;
uint8_t ShiftStepper::lastBits;
uint8_t ShiftStepper::currentBits;

ShiftStepper::ShiftStepper(int offset) {
  _remaining = 0;
  _remainingInBatch = 0;
  _paused = false;
  motor_offset = offset;
  currentStep = 0;
  microCounter = UCOUNTER_DEFAULT;
  cyclesToWait = 0;
  release();
  if(firstInstance){
    firstInstance->addNext(this);
  }else{
    firstInstance = this;
  }
}

void ShiftStepper::setup(int _data_pin, int _clock_pin, int _latch_pin){
  data_pin  = _data_pin;
  clock_pin = _clock_pin;
  latch_pin = _latch_pin;
  pinMode(data_pin,  OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(data_pin,  LOW);
  digitalWrite(clock_pin, LOW);
  digitalWrite(latch_pin, LOW);
  lastBits = 0;
  currentBits = 0;
  if(firstInstance){
    firstInstance->instanceSetup();
  }
}

void ShiftStepper::instanceSetup(){
  currentStep = 0;
  if(nextInstance){
    nextInstance->instanceSetup();
  }
}

void ShiftStepper::addNext(ShiftStepper *ref){
  if(nextInstance){
    nextInstance->addNext(ref);
  }else{
    nextInstance = ref;
  }
}

void ShiftStepper::pause(){
  _paused = true;
}

void ShiftStepper::resume(){
  _paused = false;
}

void ShiftStepper::stop(){
  _remaining = 0;
  _remainingCyclesToSlowdown = 0;
  setRelSpeed(1.0);
  microCounter = UCOUNTER_DEFAULT;
}

void ShiftStepper::turn(long steps, byte direction){
  _remaining = steps;
  if (cyclesToWait > 0) {
    _remainingInBatch = _remaining < BATCH_SIZE ? _remaining : BATCH_SIZE;
  }
  //Serial.printf("Steps:%d\n", steps);
  _dir = direction;
  lastDirection = direction;
  startTimer();
}

boolean ShiftStepper::ready(){
  return (_remaining == 0  && _remainingCyclesToSlowdown == 0);
}

boolean ShiftStepper::allStopped() {
  if (!firstInstance) {
    return true;
  }
  if (!(firstInstance->ready())) {
    return false;
  }
  ShiftStepper* next = firstInstance->nextInstance;
  while (next != nullptr) {
    if (!(next->ready())) {
      return false;
    }
    next = next->nextInstance;
  }
  return true;
}

long ShiftStepper::remaining(){
  return _remaining;
}

void ShiftStepper::setRelSpeed(float multiplier) {
  if (multiplier >= 1.0 || multiplier < 0) {
    cyclesToWait = 0;
  } else {
    cyclesToWait = ((UCOUNTER_DEFAULT * BATCH_SIZE) / multiplier) - UCOUNTER_DEFAULT * BATCH_SIZE;
  }
}

byte ICACHE_RAM_ATTR ShiftStepper::nextStep(){
  switch(currentStep){
    case B0000:
    case B0001:
      return (_dir == FORWARD ? B0011 : B1001);
    case B0011:
      return (_dir == FORWARD ? B0010 : B0001);
    case B0010:
      return (_dir == FORWARD ? B0110 : B0011);
    case B0110:
      return (_dir == FORWARD ? B0100 : B0010);
    case B0100:
      return (_dir == FORWARD ? B1100 : B0110);
    case B1100:
      return (_dir == FORWARD ? B1000 : B0100);
    case B1000:
      return (_dir == FORWARD ? B1001 : B1100);
    case B1001:
      return (_dir == FORWARD ? B0001 : B1000);
    default:
      return B0000;
  }
}

void ICACHE_RAM_ATTR ShiftStepper::setNextStepSlowdown() {
  if (_remainingInBatch > 0) {
    // if in a batch, proceed as normal
    if(_remaining > 0 && !_paused){
      if(!--microCounter){
        microCounter = UCOUNTER_DEFAULT;
        _remaining--;
        _remainingInBatch--; // decrement batch counter as well
        updateBits(nextStep());
      }
    }
    // if now done with the batch, start slowdown.
    if (!_remainingInBatch) {
      _remainingCyclesToSlowdown = cyclesToWait;
    }
  } else if (_remainingCyclesToSlowdown > 0) {
    // if in a slowdown period
    _remainingCyclesToSlowdown--;
    // if now done with the slowdown, do next batch
    if (!_remainingCyclesToSlowdown) {
      _remainingInBatch = _remaining < BATCH_SIZE ? _remaining : BATCH_SIZE;
    }
  } else {
    // if done moving
    setRelSpeed(1.0);
    if (allStopped()) {
        release();
        stopTimer();
    }
  }
}

void ICACHE_RAM_ATTR ShiftStepper::setNextStepFullspeed() {
  if(_remaining > 0 && !_paused){
      if(!--microCounter){
        microCounter = UCOUNTER_DEFAULT;
        _remaining--;
        updateBits(nextStep());
      }
    }else{
      setRelSpeed(1.0); // makes sure that speed is normal on next move.
      if (allStopped()) {
        release();
        stopTimer();
      }
  }
}

void ICACHE_RAM_ATTR ShiftStepper::setNextStep(){
  if (cyclesToWait > 0) { // if we are running slower than normal
    setNextStepSlowdown();
  } else {
    setNextStepFullspeed();
  }
}

void ICACHE_RAM_ATTR ShiftStepper::release(){
  currentStep = 0;
  updateBits(0);
  sendBits();
}

void ICACHE_RAM_ATTR ShiftStepper::trigger(){
  setNextStep();
  if(nextInstance){
    nextInstance->trigger();
  }
}

/**
 * @brief Takes a byte, shifts it by a certain number of places,
 * while wrapping around the bits that would normally fall off
 * @param in Byte to process
 * @param amountToShift Amount of places to shift (tested for positive only)
 * @return byte The input byte, shifted.
 */
byte shiftWraparound(byte in, int amountToShift) {
  return in << amountToShift | in >> (8 - amountToShift);
}

void ICACHE_RAM_ATTR ShiftStepper::updateBits(uint8_t bits){
  currentStep = bits;
  // clear upper 4 bits of 'bits'
  bits &= B1111;
  // Offset bits according to the motor.
  bits = shiftWraparound(bits, motor_offset);
  // clear the bits that correspond to this motor
  currentBits &= ~(shiftWraparound(B1111, motor_offset));
  // set the bits for this motor.
  currentBits |= bits;
}

void ICACHE_RAM_ATTR ShiftStepper::sendBits(){
  if(currentBits != lastBits){
    lastBits = currentBits;
    shiftOut(data_pin, clock_pin, MSBFIRST, currentBits);
    digitalWrite(latch_pin, HIGH);
    digitalWrite(data_pin,  LOW);
    digitalWrite(clock_pin, LOW);
    digitalWrite(latch_pin, LOW);
  }
}

void ICACHE_RAM_ATTR ShiftStepper::triggerTop(){
  if(firstInstance){
    firstInstance->trigger();
  }
  sendBits();
}

void ShiftStepper::startTimer(){
  // Initialise the timers
  timer1_disable();
  timer1_isr_init();
  timer1_attachInterrupt(ShiftStepper::triggerTop);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
  timer1_write(clockCyclesPerMicrosecond() * BASE_INTERRUPT_US);
}

void ShiftStepper::stopTimer(){
  timer1_disable();
}

#endif
