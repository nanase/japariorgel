#pragma once
#include "Adafruit_ZeroTimer.h"

class Waiter {
 public:
  static void initialize();
  static void reset();
  static uint64_t ten_micros();
  static void addTick(uint16_t tick);
  static uint16_t getWaitTick();
  static void wait();

 private:
  static Adafruit_ZeroTimer zt4;
  static volatile uint64_t counter;
  static uint16_t waitTick;
  static uint64_t waitGoal;

  static void callback();
};
