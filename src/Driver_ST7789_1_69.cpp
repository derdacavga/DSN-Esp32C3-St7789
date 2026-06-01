#include "ST7789_ESP32C3.h"

#if defined(ST7789_1_69_LCD)

void ST7789_ESP32C3::init() {
  pinMode(TFT_DC, OUTPUT);
  if (TFT_CS >= 0) pinMode(TFT_CS, OUTPUT);
  if (TFT_RST >= 0) pinMode(TFT_RST, OUTPUT);
  
  if (TFT_BL >= 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  }

  spi = &SPI;
  spi->begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  if (TFT_RST >= 0) {
    digitalWrite(TFT_RST, HIGH); delay(10);
    digitalWrite(TFT_RST, LOW);  delay(10);
    digitalWrite(TFT_RST, HIGH); delay(150);
  }

  writeCommand(ST7789_SWRESET); delay(150);
  writeCommand(ST7789_SLPOUT);  delay(255);
  writeCommand(ST7789_COLMOD);  writeData(0x55);
  writeCommand(ST7789_MADCTL);  writeData(0x00);
  
  writeCommand(ST7789_INVON); 
  writeCommand(ST7789_NORON);   delay(10);
  writeCommand(ST7789_DISPON);  delay(10);

  setRotation(0); 
}

void ST7789_ESP32C3::setRotation(uint8_t m) {
  _rotation = m % 4; 
  writeCommand(ST7789_MADCTL);

  switch (_rotation) {
    case 0: writeData(0x00); _width = 240; _height = 280; _xstart = 0; _ystart = 20; break;
    case 1: writeData(0x60); _width = 280; _height = 240; _xstart = 20; _ystart = 0; break;
    case 2: writeData(0xC0); _width = 240; _height = 280; _xstart = 0; _ystart = 20; break;
    case 3: writeData(0xA0); _width = 280; _height = 240; _xstart = 20; _ystart = 0; break;
  }
}

#endif