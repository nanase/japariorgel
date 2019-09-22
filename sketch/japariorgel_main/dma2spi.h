#pragma once

#include "Adafruit_ZeroDMA.h"

class DMA2SPI {
 public:
  static void initialize();
  static void setCallbackOnDone(void (*callbackFunc)(void));
  static void setDescription(const uint8_t *source, uint16_t size);
  static void start();
  static bool isBusy();
  static void wait();

 private:
  static Adafruit_ZeroDMA dma;
  static DmacDescriptor *dmacDescriptor;
  static volatile bool transferIsDone;
  static void (*callbackFunc)(void);

  static void callbackOnDone([[maybe_unused]] Adafruit_ZeroDMA *dma);
};
