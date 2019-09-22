const SPISettings spiSettings = SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0);
uint8_t YMF825::targetChip    = YMF825_BOTH_ENABLE;

void YMF825::initialize() {
  Serial.println("[YMF825] init");
  DMA2SPI::initialize();
  DMA2SPI::setCallbackOnDone(YMF825::disableSlaveSelect);

  digitalWrite(PIN_YMF825_RST, HIGH);
  digitalWrite(PIN_YMF825_RST, LOW);
  delay(1);
  digitalWrite(PIN_YMF825_RST, HIGH);

  YMF825::write(0x1d, 0x01);
  YMF825::write(0x02, 0x0f);
  delay(10);
  YMF825::write(0x00, 0x01);
  YMF825::write(0x01, 0x00);
  YMF825::write(0x1a, 0xa3);
  delay(10);
  YMF825::write(0x1a, 0x00);
  delay(30);
  YMF825::write(0x02, 0x00);
  YMF825::write(0x19, 0xc0);
  YMF825::write(0x1b, 0x3f);
  YMF825::write(0x14, 0x00);
  YMF825::write(0x03, 0x03);
  YMF825::write(0x08, 0xf6);
  delay(30);
  YMF825::write(0x08, 0x00);
  YMF825::write(0x09, 0xfc);
  YMF825::write(0x0a, 0x00);
  YMF825::write(0x17, 0x40);
  YMF825::write(0x18, 0x00);
  YMF825::write(0x03, 0x03);
  YMF825::write(0x19, 0xa0);
  YMF825::write(0x08, 0xf6);
  delay(10);
  YMF825::write(0x08, 0x00);

  Serial.println("[YMF825] OK");
}

void YMF825::write(uint8_t address, uint8_t data) {
  static uint8_t buffer[2];
  DMA2SPI::setDescription(buffer, 2);
  buffer[0] = address;
  buffer[1] = data;

  SPI.beginTransaction(spiSettings);
  {
    YMF825::enableSlaveSelect();
    DMA2SPI::start();
    DMA2SPI::wait();
  }
  SPI.endTransaction();
}

void YMF825::burstwrite(uint8_t address, uint8_t *data, uint16_t size) {
  DMA2SPI::setDescription(data, size);

  SPI.beginTransaction(spiSettings);
  {
    YMF825::enableSlaveSelect();
    SPI.transfer(address);
    DMA2SPI::start();
    DMA2SPI::wait();
  }
  SPI.endTransaction();
}

void YMF825::writeWithDMA(const uint8_t *data, uint16_t size) {
  DMA2SPI::setDescription(data, size);

  SPI.beginTransaction(spiSettings);
  {
    YMF825::enableSlaveSelect();
    DMA2SPI::start();
    DMA2SPI::wait();
  }
  SPI.endTransaction();
}

void YMF825::writeWithDMABlocks(const uint8_t *data0, uint16_t size0, const uint8_t *data1, uint16_t size1) {
  DMA2SPI::setDescription(data0, size0);
  // DMA2SPI::wait();
  DMA2SPI::setCallbackOnDone(NULL);
  SPI.beginTransaction(spiSettings);
  {
    YMF825::enableSlaveSelect();
    DMA2SPI::start();
    DMA2SPI::wait();

    DMA2SPI::setCallbackOnDone(YMF825::disableSlaveSelect);
    DMA2SPI::setDescription(data1, size1);
    DMA2SPI::start();
    DMA2SPI::wait();
  }
  SPI.endTransaction();
}

void YMF825::changeTargetChip(uint8_t targetChip) {
  YMF825::targetChip = targetChip;
}

void YMF825::enableSlaveSelect() {
  if (YMF825::targetChip == YMF825_BOTH_ENABLE)
    enableLRch();
  else if (YMF825::targetChip == YMF825_LCH_ENABLE)
    enableLch();
  else
    enableRch();
}

void YMF825::disableSlaveSelect() {
  disableSS();
}
