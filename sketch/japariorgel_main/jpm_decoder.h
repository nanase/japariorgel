#pragma once

#include "surgetank.h"

const byte JPM_WOPX_MASK            = B11000000;
const byte JPM_WOPX_WRITE_WAIT      = B00000000;
const byte JPM_WOPX_LCD_OPERATE     = B01000000;
const byte JPM_WOPX_BURSTWRITE_TONE = B10000000;
const byte JPM_WOPX_BURSTWRITE_EQ   = B11000000;

const byte JPM_SELX_MASK          = B00110000;
const byte JPM_SELX_WAIT          = B00000000;
const byte JPM_SELX_LCH_ENABLE    = B00010000;
const byte JPM_SELX_RCH_ENABLE    = B00100000;
const byte JPM_SELX_BOTH_ENABLE   = B00110000;
const byte JPM_SELX_LCD_WRITE     = B00000000;
const byte JPM_SELX_LCD_CONTROL   = B00010000;
const byte JPM_SELX_LCD_POSITION0 = B00100000;
const byte JPM_SELX_LCD_POSITION1 = B00110000;

const byte JPM_LENX_MASK              = B00001111;
const byte JPM_LENX_LCD_CLEAR         = B00000000;
const byte JPM_LENX_LCD_SET_BACKLIGHT = B00000001;
const byte JPM_LENX_LCD_SCROLL_LEFT   = B00000010;
const byte JPM_LENX_LCD_SCROLL_RIGHT  = B00000011;
const byte JPM_LENX_LCD_HIDE_CURSOR   = B00000100;
const byte JPM_LENX_LCD_SHOW_CURSOR   = B00000101;
const byte JPM_LENX_LCD_NO_BLINK      = B00000110;
const byte JPM_LENX_LCD_BLINK         = B00000111;

const byte JPM_LENX_LCD_CREATE_CHAR_MASK = B00000111;
const byte JPM_LENX_LCD_CREATE_CHAR0     = B00001000;
const byte JPM_LENX_LCD_CREATE_CHAR1     = B00001001;
const byte JPM_LENX_LCD_CREATE_CHAR2     = B00001010;
const byte JPM_LENX_LCD_CREATE_CHAR3     = B00001011;
const byte JPM_LENX_LCD_CREATE_CHAR4     = B00001100;
const byte JPM_LENX_LCD_CREATE_CHAR5     = B00001101;
const byte JPM_LENX_LCD_CREATE_CHAR6     = B00001110;
const byte JPM_LENX_LCD_CREATE_CHAR7     = B00001111;

#define JpmSelxToTargetChip(x) ((x) >> 4)

class JpmDecoder {
 private:
  SurgeTank *surgeTank;
  uint8_t chunkHeader;

  bool writeOrWait();
  bool lcdOperate();
  bool burstwriteTone();
  bool burstwriteEq();
  bool lcdWrite();
  bool lcdControl();
  bool lcdSetPosition([[maybe_unused]] uint8_t row);
  bool lcdSetBackLight();
  bool lcdCreateChar([[maybe_unused]] uint8_t num);

 public:
  JpmDecoder(SurgeTank *surgeTank);
  ~JpmDecoder() {}

  bool initialize();
  bool progress();

  static bool IsJpmFile(const uint8_t *buffer, const char *filename);
};
