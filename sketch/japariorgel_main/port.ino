void portSetup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_YMF825_SS_LCH, OUTPUT);
  pinMode(PIN_YMF825_SS_RCH, OUTPUT);
  pinMode(PIN_YMF825_RST, OUTPUT);
  disableSS();
  SPI.begin();
}

void enableLRch() {
  // digitalWrite(PIN_YMF825_SS, LOW);
  digitalPinToPort(PIN_YMF825_SS_LCH)->OUTCLR.reg = digitalPinToBitMask(PIN_YMF825_SS_LCH);
  digitalPinToPort(PIN_YMF825_SS_RCH)->OUTCLR.reg = digitalPinToBitMask(PIN_YMF825_SS_RCH);
}

void enableLch() {
  // digitalWrite(PIN_YMF825_SS, LOW);
  digitalPinToPort(PIN_YMF825_SS_LCH)->OUTCLR.reg = digitalPinToBitMask(PIN_YMF825_SS_LCH);
}

void enableRch() {
  // digitalWrite(PIN_YMF825_SS, LOW);
  digitalPinToPort(PIN_YMF825_SS_RCH)->OUTCLR.reg = digitalPinToBitMask(PIN_YMF825_SS_RCH);
}

void disableSS() {
  // digitalWrite(PIN_YMF825_SS, HIGH);
  digitalPinToPort(PIN_YMF825_SS_LCH)->OUTSET.reg = digitalPinToBitMask(PIN_YMF825_SS_LCH);
  digitalPinToPort(PIN_YMF825_SS_RCH)->OUTSET.reg = digitalPinToBitMask(PIN_YMF825_SS_RCH);
}
