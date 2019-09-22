#include "dma2spi.h"
#include "utility/dma.h"

Adafruit_ZeroDMA DMA2SPI::dma;
DmacDescriptor *DMA2SPI::dmacDescriptor;
volatile bool DMA2SPI::transferIsDone = true;
void (*DMA2SPI::callbackFunc)(void)   = NULL;

void DMA2SPI::initialize() {
  DMA2SPI::dma.setTrigger(SERCOM1_DMAC_ID_TX);
  DMA2SPI::dma.setAction(DMA_TRIGGER_ACTON_BEAT);
  DMA2SPI::dma.allocate();
  DMA2SPI::dmacDescriptor = DMA2SPI::dma.addDescriptor(NULL,                              // move data from here
                                                       (void *)(&SERCOM1->SPI.DATA.reg),  // to here (M0)
                                                       0,                                 // this many...
                                                       DMA_BEAT_SIZE_BYTE,                // bytes/hword/words
                                                       true,                              // increment source addr?
                                                       false);                            // increment dest addr?
  DMA2SPI::dma.setCallback(DMA2SPI::callbackOnDone);
}

void DMA2SPI::setCallbackOnDone(void (*callbackFunc)(void)) {
  DMA2SPI::callbackFunc = callbackFunc;
}

void DMA2SPI::setDescription(const uint8_t *source, uint16_t size) {
  DMA2SPI::wait();
  DMA2SPI::dma.changeDescriptor(DMA2SPI::dmacDescriptor, (uint8_t *)source, NULL, size);
}

void DMA2SPI::start() {
  DMA2SPI::wait();
  DMA2SPI::transferIsDone = false;
  DMA2SPI::dma.startJob();
}

bool DMA2SPI::isBusy() {
  return !DMA2SPI::transferIsDone;
}

void DMA2SPI::wait() {
  while (!DMA2SPI::transferIsDone)
    ;
}

void DMA2SPI::callbackOnDone([[maybe_unused]] Adafruit_ZeroDMA *dma) {
  DMA2SPI::transferIsDone = true;

  if (DMA2SPI::callbackFunc)
    DMA2SPI::callbackFunc();
}
