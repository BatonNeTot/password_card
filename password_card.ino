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

#include <USB.h>
#include <USBHIDKeyboard.h>
USBHIDKeyboard Keyboard;

#include "csv_reader.h"

#include <vector>
#include <unordered_map>

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

    if (stringOffset < line.length()) {
      M5Cardputer.Display.drawString(line.c_str() + stringOffset, 0, offset);
    }
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
}

struct CharInfo {
  uint8_t hid;
  uint8_t modifiers;
};
std::unordered_map<char, CharInfo> charInfos;

void setup() {
    M5Cardputer.begin();
    
    //Keyboard.begin();
    //USB.begin();
    
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

    for (auto x = 0; x < 14; ++x) {
      for (auto y = 0; y < 4; ++y) {
        Point2D_t point;
        point.x = (int)x;
        point.y = (int)y;
        auto keyValue = M5Cardputer.Keyboard.getKeyValue(point);
        CharInfo charInfo;
        charInfo.hid = _kb_asciimap[keyValue.value_first];
        charInfo.modifiers = 0;
        charInfos.emplace(keyValue.value_first, charInfo);
        charInfo.modifiers = 2;
        charInfos.emplace(keyValue.value_second, charInfo);
      }
    }
}

void sendChar(char c) {
  auto it = charInfos.find(c);
  if (it == charInfos.end()) {
    return;
  }
  /*
  KeyReport report = {0};
  report.modifiers = it->second.modifiers;
  Keyboard.sendReport(&report);
  report.keys[0] = it->second.hid;
  Keyboard.sendReport(&report);
  Keyboard.releaseAll();
  */
  Keyboard.press(c);
  Keyboard.release(c);
}

void sendText(const char* text, uint64_t length) {
  //Keyboard.write((const uint8_t*)text, length);
  Serial.println(text);
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
        } else if (M5Cardputer.Keyboard.isKeyPressed('=')) {
          size_t index = 0;
          for (const auto& pair : csv.getEntries()) {
            if (selectedIndex != index) {
              ++index;
              continue;
            }

            const auto& values = pair.second;
            const auto& password = values[csv.getKeyIndex("name")];
            sendText(password.c_str(), password.length());
             
            break;
          }
        }
    }
}
