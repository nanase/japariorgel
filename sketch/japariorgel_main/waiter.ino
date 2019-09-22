#include "waiter.h"

#define TIMER4_10US_N TC_CLOCK_PRESCALER_DIV16
#define TIMER4_10US_CC0 29
#define TIMER4_TO_MS 100

Adafruit_ZeroTimer Waiter::zt4    = Adafruit_ZeroTimer(4);
volatile uint64_t Waiter::counter = 0;
uint16_t Waiter::waitTick         = 0;
uint64_t Waiter::waitGoal         = 0;

void Waiter::initialize() {
  Serial.println("[Waiter] init");
  Waiter::zt4.configure(TIMER4_10US_N, TC_COUNTER_SIZE_16BIT, TC_WAVE_GENERATION_MATCH_FREQ);

  Waiter::zt4.setCompare(0, TIMER4_10US_CC0);
  Waiter::zt4.setCallback(true, TC_CALLBACK_CC_CHANNEL0, &Waiter::callback);
  Waiter::zt4.enable(true);
  Serial.println("[Waiter] OK");
}

void Waiter::reset() {
  Waiter::counter  = 0;
  Waiter::waitGoal = 0;
}

void TC4_Handler() {
  Adafruit_ZeroTimer::timerHandler(4);
}

void Waiter::callback() {
  Waiter::counter++;
}

uint64_t Waiter::ten_micros() {
  return Waiter::counter;
}

void Waiter::addTick(uint16_t tick) {
  Waiter::waitTick += tick;
}

uint16_t Waiter::getWaitTick() {
  return Waiter::waitTick;
}

void Waiter::wait() {
  while (Waiter::waitTick > 0) {
    Waiter::waitGoal += JPM_TICK_TIME * TIMER4_TO_MS;

    while (Waiter::waitGoal > Waiter::counter) delayMicroseconds(8);

    Waiter::waitTick--;
  }
}
