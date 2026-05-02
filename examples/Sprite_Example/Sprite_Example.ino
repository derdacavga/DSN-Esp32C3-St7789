#include <Arduino.h>
#include <ST7789_ESP32C3.h>

ST7789_ESP32C3 tft = ST7789_ESP32C3();
ST7789_Sprite sprite = ST7789_Sprite(&tft);

int box_x = 10;
int box_y = 10;
int velocity_x = 4;
int velocity_y = 3;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  sprite.createSprite(40, 40);
}

void loop() {
  sprite.fillSprite(TFT_BLACK);
  sprite.fillRect(5, 5, 30, 30, TFT_MAGENTA);
  sprite.drawPixel(20, 20, TFT_WHITE);

  sprite.pushSprite(box_x, box_y);

  box_x += velocity_x;
  box_y += velocity_y;

  if (box_x <= 0 || box_x >= tft.width() - 40) velocity_x = -velocity_x;
  if (box_y <= 0 || box_y >= tft.height() - 40) velocity_y = -velocity_y;

  delay(15);
}