#ifndef USER_SETUP_H
#define USER_SETUP_H

 #define ST7789_1_30_LCD
// #define ST7789_1_69_LCD

// =========================================================
// 1. DISPLAY RESOLUTION
// =========================================================
 #define TFT_WIDTH  240
 #define TFT_HEIGHT 240

// =========================================================
// 2. HARDWARE PINS (ESP32-C3)
// =========================================================
#define TFT_MISO  -1  // set to -1 if not connected, or if shared with SD card connect to 1
#define TFT_CS    -1  // Chip Select
#define TFT_DC    8  // Data/Command
#define TFT_RST   5  // Reset (Set to -1 if tied to 3.3V)
#define TFT_MOSI  6  // SPI Data
#define TFT_SCLK  4  // SPI Clock
#define TFT_BL    2  // Backlight (Optional, set to -1 if not used)

// =========================================================
// 3. COLOR ORDER
// =========================================================
#define TFT_RGB_ORDER  0x00  // Standard RGB
// #define TFT_RGB_ORDER  0x08  // BGR Order

// =========================================================
// 4. SPI SPEED
// =========================================================
//#define SPI_FREQUENCY  40000000 // 40 MHz is standard for ESP32/ST7789
#define SPI_FREQUENCY  80000000 // 80 MHz for ESP32/ST7789

// =========================================================
// 5. FONTS
// =========================================================
#define LOAD_GLCD   // Standard 5x7 pixel font
#define LOAD_GFXFF  // FreeFonts (Adafruit GFX proportional fonts)
#define LOAD_FREESANS_9 // Load FreeSans 9pt Proportional Font
// #define LOAD_FREESANS_12 // Example for the future
// #define LOAD_FREESANS_18 // Example for the future
// #define LOAD_FREESANS_24 // Example for the future

// =========================================================
// 6. FILE SYSTEM / SD CARD SUPPORT
// =========================================================
//#define LOAD_SD_BMP

#endif
