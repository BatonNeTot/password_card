/**
 * @file sdcard.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5Cardputer MicroSD Card Test
 * @version 0.1
 * @date 2024-01-19
 *
 *
 * @Hardwares: M5Cardputer
 * @Platform Version: Arduino M5Stack Board Manager v2.1.0
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5Cardputer: https://github.com/m5stack/M5Cardputer
 */

#include <M5Cardputer.h>
#include <SPI.h>
#include <SD.h>

#include "csv_reader.h"

#include <vector>

#define PASSWORD_CSV_FILENAME "/passwords.csv"

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

size_t stringOffset = 0;
size_t screenOffset = 0;
size_t selectedIndex = 0;

void draw(const std::vector<std::string>& lines) {
  M5Cardputer.Display.clear();
  size_t offset = screenOffset;
  for (auto i = 0u; i < lines.size(); ++i) {
    const auto& line = lines[i];
    
    if (i == selectedIndex) {
      M5Cardputer.Display.setColor(WHITE);
      M5Cardputer.Display.fillRect(0, offset, M5Cardputer.Display.width(), M5Cardputer.Display.fontHeight());
      M5Cardputer.Display.setTextColor(BLACK);
    } else {
      M5Cardputer.Display.setTextColor(WHITE);
    }

    M5Cardputer.Display.drawString(std::to_string(i).c_str(), 0, offset);
    size_t lineOffset = stringOffset;
    if (lineOffset > line.length()) {
      lineOffset = line.length();
    }
    M5Cardputer.Display.drawString(line.c_str() + lineOffset, 32, offset);
    offset += M5Cardputer.Display.fontHeight();
  }
}

CSV csv;

void draw() {
    std::vector<std::string> lines;
    lines.reserve(csv.getEntries().size());
    for (const auto& pair : csv.getEntries()) {
      lines.emplace_back(pair.first);
    }
    draw(lines);
    //draw(csv.getLines());
}

void setup() {
    M5Cardputer.begin();
    
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextDatum(top_left);
    M5Cardputer.Display.setTextFont(&fonts::AsciiFont8x16);
    M5Cardputer.Display.setTextSize(1);

    // SD Card Initialization
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        // Print a message if the SD card initialization
        // fails orif the SD card does not exist.
        // TODO error message
        return;
    }

    csv = CSV(SD, PASSWORD_CSV_FILENAME, "name");
    draw();
    //M5Cardputer.Display.drawString(std::to_string(lines.size()).c_str(), 0, 0);
}

void loop() {
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
        if (stringOffset > 0 && M5Cardputer.Keyboard.isKeyPressed(',')) {
          --stringOffset;
          draw();
        } else if (M5Cardputer.Keyboard.isKeyPressed('/')) {
          ++stringOffset;
          draw();
        } else if (selectedIndex < csv.getEntries().size() && M5Cardputer.Keyboard.isKeyPressed('.')) {
          ++selectedIndex;
          if ((selectedIndex + 1) * M5Cardputer.Display.fontHeight() + screenOffset >= M5Cardputer.Display.height()) {
            screenOffset = -(selectedIndex + 1) * M5Cardputer.Display.fontHeight() + M5Cardputer.Display.height();
          }
          draw();
        } else if (selectedIndex > 0 && M5Cardputer.Keyboard.isKeyPressed(';')) {
          --selectedIndex;
          if (selectedIndex * M5Cardputer.Display.fontHeight() + screenOffset >= M5Cardputer.Display.height()) {
            screenOffset = -selectedIndex * M5Cardputer.Display.fontHeight();
          }
          draw();
        }
    }
}
