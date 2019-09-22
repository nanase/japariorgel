#pragma once

#include "SD.h"

#define SD_BLOCK_SIZE 512

class SurgeTank {
 private:
  File *file;
  uint32_t tankSize;
  uint32_t readSize;
  uint32_t positionStart;
  uint32_t positionEnd;
  uint8_t *memory;
  bool eof;
  void (*transferBytesCallback)(const uint8_t *, uint16_t)                             = NULL;
  void (*transferBytesCallback2)(const uint8_t *, uint16_t, const uint8_t *, uint16_t) = NULL;

  uint16_t readSD(uint32_t position, uint16_t size);

 public:
  SurgeTank(File *file, uint32_t tankSize, void (*transferBytesCallback)(const uint8_t *, uint16_t),
            void (*transferBytesCallback2)(const uint8_t *, uint16_t, const uint8_t *, uint16_t));
  ~SurgeTank();

  bool isReachedEof();
  uint32_t getEmptySize();
  uint32_t getReadSize();
  void fill();
  void fillAll();
  int16_t readByte();
  bool readBytes(uint8_t *buffer, uint32_t offset, uint16_t size);
  bool transferBytes(uint16_t size);
  bool skip(uint16_t size);
};
