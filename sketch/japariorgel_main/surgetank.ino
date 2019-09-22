#include "stdlib.h"
#include "string.h"
#include "surgetank.h"

SurgeTank::SurgeTank(File *file, uint32_t tankSize, void (*transferBytesCallback)(const uint8_t *, uint16_t),
                     void (*transferBytesCallback2)(const uint8_t *, uint16_t, const uint8_t *, uint16_t)) {
  this->file                   = file;
  this->tankSize               = tankSize;
  this->readSize               = 0;
  this->positionStart          = 0;
  this->positionEnd            = 0;
  this->eof                    = false;
  this->transferBytesCallback  = transferBytesCallback;
  this->transferBytesCallback2 = transferBytesCallback2;

  this->memory = (uint8_t *)malloc(tankSize);
}

SurgeTank::~SurgeTank() {
  if (this->memory != NULL)
    free(this->memory);

  this->memory = NULL;
}

bool SurgeTank::isReachedEof() {
  return this->eof;
}

uint32_t SurgeTank::getEmptySize() {
  return this->tankSize - this->readSize;
}

uint32_t SurgeTank::getReadSize() {
  return this->readSize;
}

void SurgeTank::fill() {
  if (this->eof)
    return;

  if (this->getEmptySize() < SD_BLOCK_SIZE)
    return;

  uint32_t diff = this->tankSize - this->positionEnd;
  uint32_t readSize;

  if (diff < SD_BLOCK_SIZE) {
    readSize = this->readSD(this->positionEnd, diff);

    if (readSize < diff)
      this->positionEnd += readSize;
    else {
      readSize          = this->readSD(this->positionEnd, SD_BLOCK_SIZE);
      this->positionEnd = readSize;
    }
  } else {
    readSize = this->readSD(this->positionEnd, SD_BLOCK_SIZE);
    this->positionEnd += readSize;
  }

  normalizeIndex(&this->positionEnd, this->tankSize);
}

void SurgeTank::fillAll() {
  while (!this->eof && this->getEmptySize() >= SD_BLOCK_SIZE) this->fill();
}

int16_t SurgeTank::readByte() {
  if (this->readSize < 1) {
    Serial.println("[SurgeTank] Error: Not enough memory 1");
    return -1;
  }

  uint8_t data = this->memory[this->positionStart];
  this->positionStart++;
  this->readSize--;
  normalizeIndex(&this->positionStart, this->tankSize);

  return data;
}

bool SurgeTank::readBytes(uint8_t *buffer, uint32_t offset, uint16_t size) {
  if (this->readSize < size) {
    Serial.println("[SurgeTank] Error: Not enough memory 2");
    return false;
  }

  uint32_t size2Term = this->tankSize - this->positionStart;

  if (size2Term >= size) {
    memcpy(buffer + offset, this->memory + this->positionStart, size);
    this->positionStart += size;
    this->readSize -= size;
    normalizeIndex(&this->positionStart, this->tankSize);
  } else {
    memcpy(buffer + offset, this->memory + positionStart, size2Term);
    memcpy(buffer + offset + size2Term, this->memory, size - size2Term);
    this->readSize -= size;
    this->positionStart = size - size2Term;
  }

  return true;
}

bool SurgeTank::transferBytes(uint16_t size) {
  if (this->readSize < size) {
    Serial.println("[SurgeTank] Error: Not enough memory 3");
    return false;
  }

  uint32_t size2Term = this->tankSize - this->positionStart;

  if (size2Term >= size) {
    this->transferBytesCallback(this->memory + this->positionStart, size);
    this->positionStart += size;
    this->readSize -= size;
    normalizeIndex(&this->positionStart, this->tankSize);
  } else {
    this->transferBytesCallback2(this->memory + this->positionStart, size2Term, this->memory, size - size2Term);
    this->readSize -= size;
    this->positionStart = size - size2Term;
  }

  return true;
}

bool SurgeTank::skip(uint16_t size) {
  if (this->readSize < size) {
    Serial.println("[SurgeTank] Error: Not enough memory 4");
    return false;
  }

  uint32_t size2Term = this->tankSize - this->positionStart;

  if (size2Term >= size) {
    this->positionStart += size;
    this->readSize -= size;
    normalizeIndex(&this->positionStart, this->tankSize);
  } else {
    this->readSize -= size;
    this->positionStart = size - size2Term;
  }

  return true;
}

uint16_t SurgeTank::readSD(uint32_t position, uint16_t size) {
  if (this->eof)
    return 0;

  digitalPinToPort(LED_BUILTIN)->OUTSET.reg = digitalPinToBitMask(LED_BUILTIN);
  int16_t actualReadSize                    = this->file->read(this->memory + position, size);
  digitalPinToPort(LED_BUILTIN)->OUTCLR.reg = digitalPinToBitMask(LED_BUILTIN);

  if (actualReadSize == 0) {
    Serial.println("[SurgeTank] Reach end of file");
    this->eof = true;
  }

  // Serial.print("[Read] Tank remains: ");
  // Serial.print(this->readSize, DEC);
  // Serial.print(" bytes (");
  // Serial.print(this->readSize / (double)this->tankSize * 100);
  // Serial.print("%), start: ");
  // Serial.print(this->positionStart, DEC);
  // Serial.print(", end: ");
  // Serial.println(this->positionEnd, DEC);

  this->readSize += actualReadSize;
  return actualReadSize;
}

inline void normalizeIndex(uint32_t *target, uint32_t size) {
  if (*target >= size)
    *target %= size;
}
