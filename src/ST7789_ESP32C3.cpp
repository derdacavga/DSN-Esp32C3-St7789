#include "ST7789_ESP32C3.h"

#ifdef LOAD_GLCD
  #include "glcdfont.h" 
#endif

ST7789_ESP32C3::ST7789_ESP32C3() {
  _width = TFT_WIDTH;
  _height = TFT_HEIGHT;
  _xstart = 0;
  _ystart = 0;
  _inTransaction = false;

  cursor_x    = 0;
  cursor_y    = 0;
  textsize    = 1;
  textcolor   = 0xFFFF; 
  textbgcolor = 0xFFFF; 
  wrap        = true;
  gfxFont = NULL;
}

void ST7789_ESP32C3::writeCommand(uint8_t c) {
  DC_LOW();
  if (!_inTransaction) {
    spi->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    CS_LOW(); 
  }
  spi->write(c);   
  if (!_inTransaction) {
    CS_HIGH();
    spi->endTransaction();
  }
}

void ST7789_ESP32C3::writeData(uint8_t d) {
  DC_HIGH();
  if (!_inTransaction) {
    spi->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    CS_LOW(); 
  }
  spi->write(d);   
  if (!_inTransaction) {
    CS_HIGH();
    spi->endTransaction(); 
  }
}

void ST7789_ESP32C3::writeData16(uint16_t d) {
  DC_HIGH();
  if (!_inTransaction) {
    spi->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    CS_LOW(); 
  }
  spi->write16(d); 
  if (!_inTransaction) {
    CS_HIGH();
    spi->endTransaction(); 
  }
}

void ST7789_ESP32C3::init() {
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
  if (TFT_RST != -1) {
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, HIGH); delay(10);
    digitalWrite(TFT_RST, LOW);  delay(10);
    digitalWrite(TFT_RST, HIGH); delay(150); 
  }

  if (TFT_BL != -1) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  }

  spi = &SPI;
  spi->begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  writeCommand(0x01);
  delay(150);

  writeCommand(ST7789_SLPOUT);
  delay(120);

  writeCommand(ST7789_COLMOD);
  writeData(0x55);

  writeCommand(ST7789_MADCTL);
  writeData(0x00);

  writeCommand(ST7789_INVON);  

  writeCommand(ST7789_NORON);
  delay(10);

  writeCommand(ST7789_DISPON);
  delay(120);

  setRotation(0); 
}

void ST7789_ESP32C3::setRotation(uint8_t m) {
  _rotation = m % 4; 
  writeCommand(ST7789_MADCTL);

  switch (_rotation) {
    case 0: 
      writeData(0x00);
      _width  = 240; 
      _height = 280; 
      _xstart = 0; 
      _ystart = 20;
      break;

    case 1: 
      writeData(0x60);
      _width  = 280; 
      _height = 240; 
      _xstart = 20; 
      _ystart = 0; 
      break;

    case 2: 
      writeData(0xC0);
      _width  = 240; 
      _height = 280; 
      _xstart = 0; 
      _ystart = 20;
      break;

    case 3: 
      writeData(0xA0);
      _width  = 280; 
      _height = 240; 
      _xstart = 20; 
      _ystart = 0;
      break;
  }
}

void ST7789_ESP32C3::setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  x0 += _xstart; x1 += _xstart;
  y0 += _ystart; y1 += _ystart;

  bool is_locked = _inTransaction;
  if (!is_locked) startWrite();

  DC_LOW();
  spi->write(0x2A); 
  DC_HIGH();
  spi->write32(((uint32_t)x0 << 16) | x1);

  DC_LOW();
  spi->write(0x2B); 
  DC_HIGH();
  spi->write32(((uint32_t)y0 << 16) | y1);

  DC_LOW();
  spi->write(0x2C); 

  if (!is_locked) endWrite();
}

void ST7789_ESP32C3::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
  if((x >= _width) || (y >= _height)) return;
  
  bool is_locked = _inTransaction;
  if (!is_locked) startWrite(); 
  
  setWindow(x, y, x, y); 
  
  DC_HIGH();
  spi->write16(color); 
  
  if (!is_locked) endWrite();
}

void ST7789_ESP32C3::pushColor(uint16_t color) {
  bool is_locked = _inTransaction;
  if (!is_locked) startWrite();
  
  DC_HIGH();
  spi->write16(color); 
  
  if (!is_locked) endWrite();
}

void ST7789_ESP32C3::pushColor(uint16_t color, uint32_t len) {
  uint16_t swapped_color = (color >> 8) | (color << 8);

  static uint16_t last_color = 0xFFFF;
  static uint16_t colorBuffer[240]; 
  
  if (swapped_color != last_color) {
    for (uint16_t i = 0; i < 240; i++) colorBuffer[i] = swapped_color;
    last_color = swapped_color;
  }

  bool is_locked = _inTransaction;
  if (!is_locked) startWrite();

  DC_HIGH(); 
  while (len > 0) {
    uint32_t chunk = (len > 240) ? 240 : len;
    spi->writeBytes((uint8_t*)colorBuffer, chunk * 2);     
    len -= chunk;
  }

  if (!is_locked) endWrite();
}

void ST7789_ESP32C3::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
  if ((x >= _width) || (y >= _height)) return;
  if ((x + w - 1) >= _width)  w = _width  - x;
  if ((y + h - 1) >= _height) h = _height - y;

  bool is_locked = _inTransaction;
  if (!is_locked) startWrite();  
  
  setWindow(x, y, x + w - 1, y + h - 1);
  pushColor(color, w * h);
  
  if (!is_locked) endWrite();   
}

void ST7789_ESP32C3::fillScreen(uint16_t color) {
  fillRect(0, 0, _width, _height, color);
}

void ST7789_ESP32C3::setCursor(int16_t x, int16_t y) {
  cursor_x = x;
  cursor_y = y;
}

void ST7789_ESP32C3::setTextColor(uint16_t c) {
  textcolor = c;
  textbgcolor = c;
}

void ST7789_ESP32C3::setTextColor(uint16_t c, uint16_t bg) {
  textcolor = c;
  textbgcolor = bg;
}

void ST7789_ESP32C3::setTextSize(uint8_t s) {
  textsize = (s > 0) ? s : 1;
}

void ST7789_ESP32C3::setTextWrap(bool w) {
  wrap = w;
}

void ST7789_ESP32C3::setFont(const GFXfont *f) {
  gfxFont = (GFXfont *)f;
}

void ST7789_ESP32C3::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if(!gfxFont) { 
#ifdef LOAD_GLCD
    if ((c >= 32) && (c <= 127)) {
      startWrite();  
      for (int8_t i = 0; i < 5; i++) {
        uint8_t line = font[(c - 32) * 5 + i];
        for (int8_t j = 0; j < 8; j++, line >>= 1) {
          if (line & 1) {
            if (size == 1) drawPixel(x + i, y + j, color);
            else           fillRect(x + i * size, y + j * size, size, size, color);
          } else if (bg != color) {
            if (size == 1) drawPixel(x + i, y + j, bg);
            else           fillRect(x + i * size, y + j * size, size, size, bg);
          }
        }
      }
      endWrite(); 
    }
#endif  
  } else {  
#ifdef LOAD_GFXFF
    if ((c >= gfxFont->first) && (c <= gfxFont->last)) {
      c -= gfxFont->first;
      GFXglyph *glyph  = &(((GFXglyph *)pgm_read_ptr(&gfxFont->glyph))[c]);
      uint8_t  *bitmap = (uint8_t *)pgm_read_ptr(&gfxFont->bitmap);

      uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
      uint8_t  w  = pgm_read_byte(&glyph->width),
               h  = pgm_read_byte(&glyph->height);
      int8_t   xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
      uint8_t  xx, yy, bits = 0, bit = 0;

      startWrite(); 
      for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
          if (!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if (bits & 0x80) {
            if (size == 1) drawPixel(x + xo + xx, y + yo + yy, color);
            else           fillRect(x + (xo + xx) * size, y + (yo + yy) * size, size, size, color);
          }
          bits <<= 1;
        }
      }
      endWrite();  
    }
#endif  
  }
}

size_t ST7789_ESP32C3::write(uint8_t c) {
  if (!gfxFont) { 
#ifdef LOAD_GLCD
    if (c == '\n') {
      cursor_y += textsize * 8;
      cursor_x  = 0;
    } else if (c != '\r') {
      if (wrap && ((cursor_x + textsize * 6) > _width)) {
        cursor_x = 0;
        cursor_y += textsize * 8;
      }
      drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
      cursor_x += textsize * 6; 
    }
#endif  
  } else {  
#ifdef LOAD_GFXFF
    if (c == '\n') {
      cursor_x  = 0;
      cursor_y += (uint8_t)pgm_read_byte(&gfxFont->yAdvance) * textsize;
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_ptr(&gfxFont->glyph))[c - first]);
        uint8_t w = pgm_read_byte(&glyph->width);
        uint8_t xa = pgm_read_byte(&glyph->xAdvance);

        if (wrap && ((cursor_x + w * textsize) > _width)) {
          cursor_x = 0;
          cursor_y += (uint8_t)pgm_read_byte(&gfxFont->yAdvance) * textsize;
        }
        drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        cursor_x += xa * textsize;
      }
    }
#endif 
  }
  return 1;
}

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

void ST7789_ESP32C3::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  fillRect(x, y, 1, h, color);
}

void ST7789_ESP32C3::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  fillRect(x, y, w, 1, color);
}

void ST7789_ESP32C3::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

void ST7789_ESP32C3::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) { _swap_int16_t(x0, y0); _swap_int16_t(x1, y1); }
  if (x0 > x1) { _swap_int16_t(x0, x1); _swap_int16_t(y0, y1); }

  int16_t dx = x1 - x0;
  int16_t dy = abs(y1 - y0);
  int16_t err = dx / 2;
  int16_t ystep = (y0 < y1) ? 1 : -1;

  startWrite();

  for (; x0 <= x1; x0++) {
    if (steep) drawPixel(y0, x0, color);
    else       drawPixel(x0, y0, color);
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
  
  endWrite();
}

void ST7789_ESP32C3::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  drawPixel(x0, y0 + r, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}
 
uint16_t ST7789_ESP32C3::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void ST7789_ESP32C3::setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  setWindow(x, y, x + w - 1, y + h - 1);
  DC_HIGH();
  CS_LOW();
}

void ST7789_ESP32C3::startWrite() { 
  _inTransaction = true; 
  spi->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
  CS_LOW(); 
}

void ST7789_ESP32C3::endWrite() { 
  _inTransaction = false; 
  CS_HIGH(); 
  spi->endTransaction(); 
}

void ST7789_ESP32C3::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  drawFastVLine(x0, y0 - r, 2 * r + 1, color);
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    drawFastVLine(x0 + x, y0 - y, 2 * y + 1, color);
    drawFastVLine(x0 + y, y0 - x, 2 * x + 1, color);
    drawFastVLine(x0 - x, y0 - y, 2 * y + 1, color);
    drawFastVLine(x0 - y, y0 - x, 2 * x + 1, color);
  }
}

void ST7789_ESP32C3::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void ST7789_ESP32C3::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  int16_t a, b, y, last;
  if (y0 > y1) { _swap_int16_t(y0, y1); _swap_int16_t(x0, x1); }
  if (y1 > y2) { _swap_int16_t(y2, y1); _swap_int16_t(x2, x1); }
  if (y0 > y1) { _swap_int16_t(y0, y1); _swap_int16_t(x0, x1); }

  if(y0 == y2) { 
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  if(y1 == y2) last = y1;
  else         last = y1 - 1;

  for(y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    if(a > b) _swap_int16_t(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }

  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    if(a > b) _swap_int16_t(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }
}

void ST7789_ESP32C3::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  drawFastHLine(x + r, y, w - 2 * r, color);
  drawFastHLine(x + r, y + h - 1, w - 2 * r, color);
  drawFastVLine(x, y + r, h - 2 * r, color);
  drawFastVLine(x + w - 1, y + r, h - 2 * r, color);

  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x1    = 0;
  int16_t y1    = r;

  while (x1 < y1) {
    if (f >= 0) { y1--; ddF_y += 2; f += ddF_y; }
    x1++; ddF_x += 2; f += ddF_x;
    drawPixel(x + w - r - 1 + x1, y + r - 1 - y1, color);
    drawPixel(x + w - r - 1 + y1, y + r - 1 - x1, color);
    drawPixel(x + r - x1, y + r - 1 - y1, color);
    drawPixel(x + r - y1, y + r - 1 - x1, color);
    drawPixel(x + w - r - 1 + x1, y + h - r + y1, color);
    drawPixel(x + w - r - 1 + y1, y + h - r + x1, color);
    drawPixel(x + r - x1, y + h - r + y1, color);
    drawPixel(x + r - y1, y + h - r + x1, color);
  }
}

void ST7789_ESP32C3::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  fillRect(x + r, y, w - 2 * r, h, color);
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x1    = 0;
  int16_t y1    = r;

  while (x1 < y1) {
    if (f >= 0) { y1--; ddF_y += 2; f += ddF_y; }
    x1++; ddF_x += 2; f += ddF_x;
    drawFastVLine(x + w - r - 1 + x1, y + r - 1 - y1, 2 * y1 + 1 + h - 2 * r, color);
    drawFastVLine(x + r - x1, y + r - 1 - y1, 2 * y1 + 1 + h - 2 * r, color);
    drawFastVLine(x + w - r - 1 + y1, y + r - 1 - x1, 2 * x1 + 1 + h - 2 * r, color);
    drawFastVLine(x + r - y1, y + r - 1 - x1, 2 * x1 + 1 + h - 2 * r, color);
  }
}

void ST7789_ESP32C3::pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *data) {
  if ((x >= _width) || (y >= _height)) return;

  int16_t drawWidth = w;
  int16_t drawHeight = h;

  if ((x + w - 1) >= _width)  drawWidth  = _width  - x;
  if ((y + h - 1) >= _height) drawHeight = _height - y;

  setAddrWindow(x, y, drawWidth, drawHeight);

  DC_HIGH();
  if (!_inTransaction) CS_LOW(); 

  uint16_t rowBuffer[320];

  for (int32_t j = 0; j < drawHeight; j++) {
    for (int32_t i = 0; i < drawWidth; i++) {
      uint16_t color = data[j * w + i];
      rowBuffer[i] = (color >> 8) | (color << 8);
    }
    spi->writeBytes((uint8_t*)rowBuffer, drawWidth * 2);
  }
  
  if (!_inTransaction) CS_HIGH();
}

void ST7789_ESP32C3::scroll(uint16_t y) {
  writeCommand(0x37);

  uint16_t offset = (TFT_WIDTH == 240 && TFT_HEIGHT == 280) ? 20 : 
                    (TFT_WIDTH == 240 && TFT_HEIGHT == 240) ? 80 : 0;
  
  writeData16(offset + (y % TFT_HEIGHT)); 
}

#ifdef LOAD_SD_BMP

uint16_t ST7789_ESP32C3::read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  return result;
}

uint32_t ST7789_ESP32C3::read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();
  return result;
}

void ST7789_ESP32C3::drawBmp(fs::FS &fs, const char *filename, int16_t x, int16_t y) {
  if ((x >= _width) || (y >= _height)) return;

  fs::File bmpFS = fs.open(filename, "r");
  if (!bmpFS) return;  

  uint32_t seekOffset, headerSize, w, h;
  uint16_t depth;
  
  if (read16(bmpFS) != 0x4D42) { bmpFS.close(); return; }

  read32(bmpFS); read32(bmpFS);
  seekOffset = read32(bmpFS); 
  headerSize = read32(bmpFS);
  w = read32(bmpFS); h = read32(bmpFS);

  if (read16(bmpFS) != 1) { bmpFS.close(); return; }
  depth = read16(bmpFS); 
  if (depth != 24 && depth != 16) { bmpFS.close(); return; }

  uint32_t rowSize = (w * depth / 8 + 3) & ~3;
  
  uint8_t sdbuffer[rowSize]; 
  uint16_t tftbuffer[w];   

  int16_t drawWidth = w;
  int16_t drawHeight = h;
  if ((x + w - 1) >= _width)  drawWidth  = _width  - x;
  if ((y + h - 1) >= _height) drawHeight = _height - y;

  for (int32_t row = 0; row < drawHeight; row++) {
    uint32_t pos = seekOffset + (h - 1 - row) * rowSize;

    bmpFS.seek(pos);
    bmpFS.read(sdbuffer, drawWidth * (depth / 8));

    uint32_t buffidx = 0;
    
    for (int32_t col = 0; col < drawWidth; col++) {
      uint16_t color;
      if (depth == 24) {
        uint8_t b = sdbuffer[buffidx++];
        uint8_t g = sdbuffer[buffidx++];
        uint8_t r = sdbuffer[buffidx++];
        color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
      } else { 
        uint8_t b1 = sdbuffer[buffidx++];
        uint8_t b2 = sdbuffer[buffidx++];
        color = (b2 << 8) | b1; 
      }
      tftbuffer[col] = (color >> 8) | (color << 8); 
    }

    startWrite(); 
    setWindow(x, y + row, x + drawWidth - 1, y + row); 
    DC_HIGH(); 
    spi->writeBytes((uint8_t*)tftbuffer, drawWidth * 2);
    endWrite(); 
  }

  bmpFS.close();
}
#endif

ST7789_Sprite::ST7789_Sprite(ST7789_ESP32C3 *tft) {
  _tft = tft;
  _buffer = nullptr;
  _sw = 0;  _sh = 0;
  
  cursor_x = 0; cursor_y = 0;
  textsize = 1;
  textcolor = 0xFFFF; textbgcolor = 0xFFFF;
  wrap = true;
  gfxFont = NULL;
}

ST7789_Sprite::~ST7789_Sprite() {
  deleteSprite();
}

void* ST7789_Sprite::createSprite(int16_t w, int16_t h) {
  deleteSprite();
  _buffer = (uint16_t*)malloc(w * h * sizeof(uint16_t));
  if (_buffer) {
    _sw = w; _sh = h;
    fillSprite(0x0000);
  }
  return _buffer;
}

void ST7789_Sprite::deleteSprite() {
  if (_buffer) {
    free(_buffer);
    _buffer = nullptr;
  }
}

void ST7789_Sprite::fillSprite(uint16_t color) {
  if (!_buffer) return;
  uint16_t swapped = (color >> 8) | (color << 8); 
  for (int32_t i = 0; i < (_sw * _sh); i++) {
    _buffer[i] = swapped;
  }
}

void ST7789_Sprite::fillScreen(uint16_t color) {
  fillSprite(color);
}

void ST7789_Sprite::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (!_buffer || x < 0 || x >= _sw || y < 0 || y >= _sh) return;
  _buffer[y * _sw + x] = (color >> 8) | (color << 8); 
}

void ST7789_Sprite::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (!_buffer) return;
  for (int16_t j = y; j < y + h; j++) {
    for (int16_t i = x; i < x + w; i++) {
      drawPixel(i, j, color);
    }
  }
}

void ST7789_Sprite::pushSprite(int16_t x, int16_t y) {
  if (!_buffer) return;
  _tft->setAddrWindow(x, y, _sw, _sh);
  DC_HIGH();
  if (!_tft->_inTransaction) CS_LOW(); 
  _tft->spi->writeBytes((uint8_t*)_buffer, _sw * _sh * 2);
  if (!_tft->_inTransaction) CS_HIGH();
}

void ST7789_Sprite::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  fillRect(x, y, 1, h, color);
}

void ST7789_Sprite::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  fillRect(x, y, w, 1, color);
}

void ST7789_Sprite::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

void ST7789_Sprite::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  drawFastVLine(x0, y0 - r, 2 * r + 1, color);
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x < y) {
    if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
    x++; ddF_x += 2; f += ddF_x;
    drawFastVLine(x0 + x, y0 - y, 2 * y + 1, color);
    drawFastVLine(x0 + y, y0 - x, 2 * x + 1, color);
    drawFastVLine(x0 - x, y0 - y, 2 * y + 1, color);
    drawFastVLine(x0 - y, y0 - x, 2 * x + 1, color);
  }
}

void ST7789_Sprite::setCursor(int16_t x, int16_t y) { cursor_x = x; cursor_y = y; }
void ST7789_Sprite::setTextColor(uint16_t c) { textcolor = c; textbgcolor = c; }
void ST7789_Sprite::setTextColor(uint16_t c, uint16_t bg) { textcolor = c; textbgcolor = bg; }
void ST7789_Sprite::setTextSize(uint8_t s) { textsize = (s > 0) ? s : 1; }
void ST7789_Sprite::setTextWrap(bool w) { wrap = w; }
void ST7789_Sprite::setFont(const GFXfont *f) { gfxFont = (GFXfont *)f; }

void ST7789_Sprite::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if(!gfxFont) { 
#ifdef LOAD_GLCD
    if ((c >= 32) && (c <= 127)) {
      for (int8_t i = 0; i < 5; i++) {
        uint8_t line = font[(c - 32) * 5 + i];
        for (int8_t j = 0; j < 8; j++, line >>= 1) {
          if (line & 1) {
            if (size == 1) drawPixel(x + i, y + j, color);
            else           fillRect(x + i * size, y + j * size, size, size, color);
          } else if (bg != color) {
            if (size == 1) drawPixel(x + i, y + j, bg);
            else           fillRect(x + i * size, y + j * size, size, size, bg);
          }
        }
      }
    }
#endif 
  } else { 
#ifdef LOAD_GFXFF
    if ((c >= gfxFont->first) && (c <= gfxFont->last)) {
      c -= gfxFont->first;
      GFXglyph *glyph  = &(((GFXglyph *)pgm_read_ptr(&gfxFont->glyph))[c]);
      uint8_t  *bitmap = (uint8_t *)pgm_read_ptr(&gfxFont->bitmap);
      uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
      uint8_t  w  = pgm_read_byte(&glyph->width),
               h  = pgm_read_byte(&glyph->height);
      int8_t   xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
      uint8_t  xx, yy, bits = 0, bit = 0;

      for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
          if (!(bit++ & 7)) bits = pgm_read_byte(&bitmap[bo++]);
          if (bits & 0x80) {
            if (size == 1) drawPixel(x + xo + xx, y + yo + yy, color);
            else           fillRect(x + (xo + xx) * size, y + (yo + yy) * size, size, size, color);
          }
          bits <<= 1;
        }
      }
    }
#endif 
  }
}

size_t ST7789_Sprite::write(uint8_t c) {
  if (!gfxFont) { 
#ifdef LOAD_GLCD
    if (c == '\n') {
      cursor_y += textsize * 8;
      cursor_x  = 0;
    } else if (c != '\r') {
      if (wrap && ((cursor_x + textsize * 6) > _sw)) {
        cursor_x = 0;
        cursor_y += textsize * 8;
      }
      drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
      cursor_x += textsize * 6; 
    }
#endif 
  } else { 
#ifdef LOAD_GFXFF
    if (c == '\n') {
      cursor_x  = 0;
      cursor_y += (uint8_t)pgm_read_byte(&gfxFont->yAdvance) * textsize;
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_ptr(&gfxFont->glyph))[c - first]);
        uint8_t w = pgm_read_byte(&glyph->width);
        uint8_t xa = pgm_read_byte(&glyph->xAdvance);
        if (wrap && ((cursor_x + w * textsize) > _sw)) {
          cursor_x = 0;
          cursor_y += (uint8_t)pgm_read_byte(&gfxFont->yAdvance) * textsize;
        }
        drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
        cursor_x += xa * textsize;
      }
    }
#endif 
  }
  return 1;
}
