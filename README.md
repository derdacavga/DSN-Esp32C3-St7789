# ESP32-C3 ST7789 TFT Display Library 📺

A lightweight, fully functional TFT display library specifically designed for using the 1.69" ST7789 display with the ESP32-C3. This project eliminates the constant incompatibility issues between the Arduino IDE, ESP32 core updates, and existing display libraries. 

This library was built from the ground up to ensure stable, fast rendering without relying on bulky dependencies—making it a perfect foundation for developing custom hardware platforms, such as an integrated handheld gaming console ecosystem or a smart IoT dashboard.

## 📺 Video Tutorial

Build your own following the step-by-step guide!

[![Watch the tutorial](https://img.youtube.com/vi/uEXNxMuddvU/0.jpg)](https://www.youtube.com/watch?v=uEXNxMuddvU)

> **Click the image above to watch the full tutorial on YouTube.**
## 🛠️ Hardware Checklist

To use this library and run the provided examples, you will need:
*   **Microcontroller:** ESP32-C3 Development Board
*   **Display:** 1.69" ST7789 TFT Display
*   **Storage (Optional):** Micro SD Card Module (for the SD card examples)
*   **Wiring:** Breadboard and Jumper Wires

## 🔌 Pinout & Wiring

By default, the library is configured to use the standard SPI pins of the ESP32-C3. You can customize these pins directly in the setup file to match your project's needs.

| Component | ESP32-C3 Pin | :--- |
| :--- | :--- | :--- |
| TFT MOSI | Default SPI MOSI | GPIO | 
| TFT SCLK | Default SPI SCLK | GPIO |
| TFT CS | Defined in `User_Setup` | GPIO |
| TFT DC | Defined in `User_Setup` | GPIO |
| TFT RST | Defined in `User_Setup` | GPIO |
| SD Card MISO | Default SPI MISO *(Only if using SD!)* | GPIO |

*(Note: Check your specific ESP32-C3 board's pinout diagram for the exact default SPI GPIO numbers).*

## 💻 Arduino Installation & Setup

**1. Install the Library**
* Download this repository as a `.ZIP` file from GitHub.
* Open your Arduino IDE.
* Go to **Sketch > Include Library > Add .ZIP Library** and select the downloaded file.

**2. Configure the User Setup**
This is an important step! Just like `TFT_eSPI`, you must configure the library to work with your specific wiring.
* Navigate to your Arduino libraries folder and open this library's folder.
* Open the `User_Setup` file.
* Define your specific pins, SPI speed, and color format here. 
* **⚠️ SD Card SPI Sharing:** If you are using an SD card on the exact same SPI bus as the display, you **must** uncomment the `MISO` definition in this file. If you are *not* using an SD card, leave it commented out.
* Save and close the file.

## 🚀 Examples & How to Use

If you are familiar with `TFT_eSPI`, you will feel right at home. This library supports all the fundamental drawing functions with virtually zero learning curve.

Open the Arduino IDE and navigate to **File > Examples > [Your Library Name]** to try these out:

*   **`graphic_test`:** A benchmark and syntax demonstration. You can upload this directly without changing any code. It tests the drawing speeds for lines, rectangles, circles, and text.
*   **`Sprite`:** Demonstrates how to use sprite functions for flicker-free drawing and smooth UI elements. 
*   **`sd_image_reader`:** A powerful example showing how to read bitmap/image files directly from a Micro SD card and render them to the display seamlessly using shared SPI.

---
## 🤝 Support

If you found this project helpful, please consider:
* **Subscribing** to the YouTube Channel.
* Giving the video a **Like**.
* Starring this GitHub Repository!

* **YouTube:** https://www.youtube.com/@DsnIndustries/videos
* **Patreon:** https://www.patreon.com/c/dsnIndustries

Happy Making!

## Games
* **Maze Escape:** https://play.google.com/store/apps/details?id=com.DsnMechanics.MazeEscape
* **Air Hockey:** https://play.google.com/store/apps/details?id=com.DsnMechanics.AirHockey
* **Click Challenge:** https://play.google.com/store/apps/details?id=com.DsNMechanics.ClickChallenge
* **Flying Triangels:** https://play.google.com/store/apps/details?id=com.DsnMechanics.Triangle
* **SkyScrapper:** https://play.google.com/store/apps/details?id=com.DsnMechanics.SkyScraper
