#ifndef ST7789_ESP32C3_H
#define ST7789_ESP32C3_H

#include <Arduino.h>
#include "soc/gpio_reg.h"
#include <SPI.h>
#include <Print.h>
#include "User_Setup.h"
#include "gfxfont.h"

#ifdef LOAD_FREESANS_9
  #include "../Fonts/FreeSans9pt7b.h"
#endif
#ifdef LOAD_FREESANS_12
  #include "../Fonts/FreeSans12pt7b.h"
#endif
#ifdef LOAD_FREESANS_18
  #include "../Fonts/FreeSans18pt7b.h"
#endif
#ifdef LOAD_FREESANS_24
  #include "../Fonts/FreeSans24pt7b.h"
#endif
#ifdef LOAD_SD_BMP
  #include <FS.h>
#endif

#define ST7789_SWRESET 0x01
#define ST7789_SLPOUT  0x11
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_INVON   0x21
#define ST7789_NORON   0x13
#define ST7789_DISPON  0x29

#define TFT_BLACK       0x0000  
#define TFT_NAVY        0x000F  
#define TFT_DARKGREEN   0x03E0  
#define TFT_DARKCYAN    0x03EF  
#define TFT_MAROON      0x7800  
#define TFT_PURPLE      0x780F  
#define TFT_OLIVE       0x7BE0  
#define TFT_LIGHTGREY   0xD69A  
#define TFT_DARKGREY    0x7BEF  
#define TFT_BLUE        0x001F  
#define TFT_GREEN       0x07E0  
#define TFT_CYAN        0x07FF  
#define TFT_RED         0xF800  
#define TFT_MAGENTA     0xF81F  
#define TFT_YELLOW      0xFFE0  
#define TFT_WHITE       0xFFFF  
#define TFT_ORANGE      0xFDA0   
#define TFT_GREENYELLOW 0xB7E0   
#define TFT_PINK        0xFE19   
#define TFT_BROWN       0x9A60   
#define TFT_GOLD        0xFEA0   
#define TFT_SILVER      0xC618    
#define TFT_SKYBLUE     0x867D    
#define TFT_VIOLET      0x915C  

#define DC_HIGH()   REG_WRITE(GPIO_OUT_W1TS_REG, 1 << TFT_DC)
#define DC_LOW()    REG_WRITE(GPIO_OUT_W1TC_REG, 1 << TFT_DC)

#if defined(TFT_CS) && (TFT_CS >= 0)
  #define CS_HIGH() REG_WRITE(GPIO_OUT_W1TS_REG, 1 << TFT_CS)
  #define CS_LOW()  REG_WRITE(GPIO_OUT_W1TC_REG, 1 << TFT_CS)
#else
  #define CS_HIGH()
  #define CS_LOW()
#endif

class ST7789_ESP32C3 : public Print {
  public:
    ST7789_ESP32C3();

    friend class ST7789_Sprite;
    
    SPIClass *spi;

    void init();
    void setRotation(uint8_t m);
    void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void startWrite();
    void endWrite();

    inline void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    void pushColor(uint16_t color, uint32_t len);
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void fillScreen(uint16_t color);

    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t bg);
    void setTextSize(uint8_t s);
    void setTextWrap(bool w);

    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);

    void pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *data);
    void pushColor(uint16_t color);
    
    void scroll(uint16_t y);
    void setFont(const GFXfont *f = NULL);

    virtual size_t write(uint8_t);

    uint16_t width() { return _width; }
    uint16_t height() { return _height; }

#ifdef LOAD_SD_BMP
    void drawBmp(fs::FS &fs, const char *filename, int16_t x, int16_t y);
#endif

  private:
    uint16_t _width, _height;
    uint8_t  _rotation;
    uint16_t _xstart, _ystart;
    uint8_t  _colstart, _rowstart;
    bool     _inTransaction;

    int16_t  cursor_x, cursor_y;
    uint16_t textcolor, textbgcolor;
    uint8_t  textsize;
    bool     wrap;

    GFXfont *gfxFont;

    inline void writeCommand(uint8_t c);
    inline void writeData(uint8_t d);
    inline void writeData16(uint16_t d);

#ifdef LOAD_SD_BMP
    uint16_t read16(fs::File &f);
    uint32_t read32(fs::File &f);
#endif
};

class ST7789_Sprite : public Print {
  public:
    ST7789_Sprite(ST7789_ESP32C3 *tft);
    ~ST7789_Sprite();

    void* createSprite(int16_t w, int16_t h);
    void deleteSprite();
    
    void fillSprite(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void pushSprite(int16_t x, int16_t y);

    void fillScreen(uint16_t color);
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t bg);
    void setTextSize(uint8_t s);
    void setTextWrap(bool w);
    void setFont(const GFXfont *f = NULL);
    
    virtual size_t write(uint8_t);
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

  private:
    ST7789_ESP32C3 *_tft;
    uint16_t *_buffer; 
    int16_t _sw, _sh;

    int16_t  cursor_x, cursor_y;
    uint16_t textcolor, textbgcolor;
    uint8_t  textsize;
    bool     wrap;
    GFXfont *gfxFont;
};
#endif
