#pragma once

const byte YMF825_LCH_ENABLE  = B00000001;
const byte YMF825_RCH_ENABLE  = B00000010;
const byte YMF825_BOTH_ENABLE = B00000011;

class YMF825 {
 public:
  static void initialize();
  static void write(uint8_t address, uint8_t data);
  static void burstwrite(uint8_t address, uint8_t *data, uint16_t size);
  static void writeWithDMA(const uint8_t *data, uint16_t size);
  static void writeWithDMABlocks(const uint8_t *data0, uint16_t size0, const uint8_t *data1, uint16_t size1);
  static void changeTargetChip(uint8_t targetChip);
  static void enableSlaveSelect();
  static void disableSlaveSelect();

 private:
  static uint8_t targetChip;
};
