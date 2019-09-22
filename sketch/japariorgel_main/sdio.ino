#include "SD.h"

uint8_t sdBuffer[4];
File currentFile;
SurgeTank *surgeTank;
bool fileOpened = false;

bool sdInitialize() {
  Serial.println("[SD] init");

  while (!SD.begin(SDCARD_SS_PIN)) {
    Serial.println("[SD] init failed");
    delay(1000);
  }

  Serial.println("[SD] OK");
  return true;
}

void sdSeekNext() {
  fileOpened = false;

  while (!fileOpened) {
    Serial.println("[SD] test.jpm open");
    currentFile = SD.open("test.jpm", FILE_READ);

    if (!currentFile) {
      Serial.println("[SD] test.jpm not found");
      delay(1000);
      continue;
    }

    if (sdRead(sdBuffer, 4) && JpmDecoder::IsJpmFile(sdBuffer, currentFile.name())) {
      Serial.println("[SD] test.jpm is JPM");
      Serial.println("[SD] decoder is JPM");
      fileOpened = true;
      surgeTank  = new SurgeTank(&currentFile, 1024 * 2, YMF825::writeWithDMA, YMF825::writeWithDMABlocks);
      surgeTank->fillAll();
      decoder = new JpmDecoder(surgeTank);
      break;
    }
    currentFile.close();
    Serial.println("[SD] test.jpm close");

    delay(1000);
  }

  Serial.println(currentFile.name());

  if (!SD.open("test.jpm")) {
    Serial.println(currentFile.name());
    return;
  }

  decoder->initialize();
}

void closeFile() {
  delete surgeTank;
}

bool sdRead(uint8_t *buffer, uint16_t size) {
  uint16_t i;

  for (i = 0; i < size; i++) buffer[i] = 0x00;

  digitalPinToPort(LED_BUILTIN)->OUTSET.reg = digitalPinToBitMask(LED_BUILTIN);
  i                                         = currentFile.read(buffer, size);
  digitalPinToPort(LED_BUILTIN)->OUTCLR.reg = digitalPinToBitMask(LED_BUILTIN);
  return i > 0;
}
