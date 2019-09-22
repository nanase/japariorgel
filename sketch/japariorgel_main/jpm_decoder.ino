#include "jpm_decoder.h"

// extern LiquidCrystal lcd;

JpmDecoder::JpmDecoder(SurgeTank *surgeTank) {
  this->surgeTank = surgeTank;
}

bool JpmDecoder::initialize() {
  return this->surgeTank->skip(4);  // Skip Magic Number
  // return true;
}

bool JpmDecoder::progress() {
  if (this->surgeTank->isReachedEof() && this->surgeTank->getReadSize() == 0) {
    Serial.println("[Decoder] EOF");
    return false;
  }

  int16_t chunkHeader = this->surgeTank->readByte();

  if (chunkHeader == -1)
    return false;

  this->chunkHeader = (uint8_t)chunkHeader;

  switch (this->chunkHeader & JPM_WOPX_MASK) {
    case JPM_WOPX_WRITE_WAIT:
      return this->writeOrWait();

    case JPM_WOPX_LCD_OPERATE:
      return this->lcdOperate();

    case JPM_WOPX_BURSTWRITE_TONE:
      return this->burstwriteTone();

    case JPM_WOPX_BURSTWRITE_EQ:
      return this->burstwriteEq();
  }

  return false;
}

bool JpmDecoder::IsJpmFile(const uint8_t *buffer, const char *filename) {
  char *extension = strrchr(filename, '.');

  if (!extension || (strcmp(extension, ".JPM") != 0 && strcmp(extension, ".jpm") != 0))
    return false;

  return (buffer[0] == 'J' && buffer[1] == 'P' && buffer[2] == 'M' && buffer[3] == 0);  // Version
}

bool JpmDecoder::writeOrWait() {
  uint8_t selx   = this->chunkHeader & JPM_SELX_MASK;
  uint8_t length = (this->chunkHeader & JPM_LENX_MASK) + 1;

  if (selx == JPM_SELX_WAIT) {
    Waiter::addTick(length);
    return true;
  }

  YMF825::changeTargetChip(JpmSelxToTargetChip(selx));

  for (uint8_t i = 0; i < length; i++) {
    YMF825::enableSlaveSelect();

    if (!this->surgeTank->transferBytes(2))  // address & data
    {
      Serial.println("[Decoder] error writeOrWait");
      return false;
    }
  }

  return true;
}

bool JpmDecoder::lcdOperate() {
  switch (this->chunkHeader & JPM_SELX_MASK) {
    case JPM_SELX_LCD_WRITE:
      return this->lcdWrite();

    case JPM_SELX_LCD_CONTROL:
      return this->lcdControl();

    case JPM_SELX_LCD_POSITION0:
      return this->lcdSetPosition(0);

    case JPM_SELX_LCD_POSITION1:
      return this->lcdSetPosition(1);

    default:
      Serial.println("[LCD] lcd operate decode error");
      // error
      return false;
  }
}

bool JpmDecoder::burstwriteTone() {
  int16_t length = (this->chunkHeader & JPM_LENX_MASK) + 1;

  YMF825::changeTargetChip(JpmSelxToTargetChip(this->chunkHeader & JPM_SELX_MASK));
  YMF825::enableSlaveSelect();
  return this->surgeTank->transferBytes(length * 30 + 6);
}

bool JpmDecoder::burstwriteEq() {
  YMF825::changeTargetChip(JpmSelxToTargetChip(this->chunkHeader & JPM_SELX_MASK));
  YMF825::enableSlaveSelect();
  return this->surgeTank->transferBytes(16);
}

bool JpmDecoder::lcdWrite() {
  uint8_t length = (this->chunkHeader & JPM_LENX_MASK) + 1;
  static uint8_t lcd_buffer[17];

  if (!this->surgeTank->readBytes(lcd_buffer, 0, length))
    return false;

  // Serial.print("[LCD] out: ");
  // Serial.println((char *)lcd_buffer);
  // for (byte i = 0; i < length; i++) lcdWrite(sdBufferByteTo(i));

  return true;
}

bool JpmDecoder::lcdControl() {
  switch (this->chunkHeader & JPM_LENX_MASK) {
    case JPM_LENX_LCD_CLEAR:
      // Serial.println("[LCD] clear");
      // lcd.clear();
      return true;

    case JPM_LENX_LCD_SET_BACKLIGHT:
      return this->lcdSetBackLight();

    case JPM_LENX_LCD_SCROLL_LEFT:
      // Serial.println("[LCD] scroll to left");
      // lcd.scrollDisplayLeft();
      return true;

    case JPM_LENX_LCD_SCROLL_RIGHT:
      // Serial.println("[LCD] scroll to right");
      // lcd.scrollDisplayRight();
      return true;

    case JPM_LENX_LCD_HIDE_CURSOR:
      // Serial.println("[LCD] hide cursor");
      // lcd.noCursor();
      return true;

    case JPM_LENX_LCD_SHOW_CURSOR:
      // Serial.println("[LCD] show cursor");
      // lcd.cursor();
      return true;

    case JPM_LENX_LCD_NO_BLINK:
      // Serial.println("[LCD] set no blinking");
      // lcd.noBlink();
      return true;

    case JPM_LENX_LCD_BLINK:
      // Serial.println("[LCD] set blinking");
      // lcd.blink();
      return true;

    case JPM_LENX_LCD_CREATE_CHAR0:
    case JPM_LENX_LCD_CREATE_CHAR1:
    case JPM_LENX_LCD_CREATE_CHAR2:
    case JPM_LENX_LCD_CREATE_CHAR3:
    case JPM_LENX_LCD_CREATE_CHAR4:
    case JPM_LENX_LCD_CREATE_CHAR5:
    case JPM_LENX_LCD_CREATE_CHAR6:
    case JPM_LENX_LCD_CREATE_CHAR7:
      return this->lcdCreateChar(this->chunkHeader & JPM_LENX_LCD_CREATE_CHAR_MASK);

    default:
      Serial.println("[LCD] lcd control decode error");
      // error
      return false;
  }
}

bool JpmDecoder::lcdSetPosition([[maybe_unused]] uint8_t row) {
  // Serial.println("[LCD] set cursor position");
  // lcd.setCursor(sdBufferByte() & JPM_LENX_MASK, row);
  return true;
}

bool JpmDecoder::lcdSetBackLight() {
  // Serial.println("[LCD] set back light color");

  if (!this->surgeTank->skip(3))
    return false;

  // lcdSetFadeColor(sdBufferByteTo(0),   // R
  //                 sdBufferByteTo(1),   // G
  //                 sdBufferByteTo(2));  // B

  return true;
}

bool JpmDecoder::lcdCreateChar([[maybe_unused]] uint8_t num) {
  // static char cgBuffer[8];

  // Serial.println("[LCD] create char");

  if (!this->surgeTank->skip(7))
    return false;

  // memcpy(cgBuffer, &sdBufferByte(), 7);
  // lcdCreateChar(num, (uint8_t *)cgBuffer);
  return true;
}
