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

#include <USB.h>
#include <USBHIDKeyboard.h>
USBHIDKeyboard Keyboard;

#include "csv_reader.h"
#include "console_window.h"

#include <vector>
#include <unordered_map>

#define PASSWORD_CSV_FILENAME "/passwords.csv"

CSV csv;

void sendChar(char c) {
  Keyboard.press(c);
  Keyboard.release(c);
}

void sendText(const char* text, uint64_t length) {
  //Keyboard.write((const uint8_t*)text, length);
  Serial.println(text);
}

std::vector<std::string> csvEntryActions = {
  "write",
  "update"
};

void setup() {
  M5Cardputer.begin();
  
  //Keyboard.begin();
  //USB.begin();
  
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextDatum(top_left);
  M5Cardputer.Display.setTextFont(&fonts::AsciiFont8x16);
  M5Cardputer.Display.setTextSize(1);
  
  CSV::init();
  
  csv = CSV(PASSWORD_CSV_FILENAME, "name");
  std::vector<std::string> lines;
  lines.reserve(csv.getEntries().size());
  for (const auto& pair : csv.getEntries()) {
    lines.emplace_back(pair.first);
  }
  ConsoleWindow::openWindow(lines, [](size_t selectedIndex){
    size_t index = 0;
    for (const auto& pair : csv.getEntries()) {
      if (selectedIndex != index) {
        ++index;
        continue;
      }
  
    ConsoleWindow::openWindow(csvEntryActions, [&values = pair.second](size_t action){
      ConsoleWindow::openWindow(csv.getKeys(), [&values, action](size_t selectedIndex){
        switch (action) {
        case 0: /* write */ {
          const auto& password = values[selectedIndex];
          sendText(password.c_str(), password.length());
          break;
        }
        }
      });
    });
    
    break;
    }
  });
}

void loop() {
    M5Cardputer.update();
    ConsoleWindow::update();
}
