#include "SPI.h"

#include "config.h"
#include "definitions.h"

#include "dma2spi.h"
#include "jpm_decoder.h"
#include "waiter.h"
#include "ymf825.h"

JpmDecoder *decoder;
extern SurgeTank *surgeTank;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  portSetup();
  sdInitialize();
  YMF825::initialize();
  Waiter::initialize();
}

void loop() {
  bool canProgress;
  sdSeekNext();
  Waiter::reset();

  do {
    if (Waiter::getWaitTick() == 0) {
      canProgress = decoder->progress();
    } else
      Waiter::wait();

    surgeTank->fill();
  } while (canProgress);

  delete decoder;
  closeFile();
}
