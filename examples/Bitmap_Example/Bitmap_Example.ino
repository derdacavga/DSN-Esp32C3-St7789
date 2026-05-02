#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "ST7789_ESP32C3.h"

#define SD_CS 9

ST7789_ESP32C3 tft;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n--- Advanced SD Isolation Test ---");

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(0);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  delay(10);

  if (!SD.begin(SD_CS, SPI, 10000000)) {
    Serial.println(" FAILED!");
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print("SD Mount Failed!");
    return;
  }

  Serial.println(" SUCCESS!");

  Serial.println("Drawing test.bmp...");
  tft.drawBmp(SD, "/test.bmp", 0, 0);

  Serial.println("Done!");
}

void loop() {}