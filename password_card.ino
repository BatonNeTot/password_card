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
#include "filter_menu_window.h"
#include "edit_window.h"
#include "fn_keyboard.h"

#include <vector>
#include <chrono>
#include <thread>

#define PASSWORD_CSV_FILENAME "/passwords.csv"
#define DEBUG 1

CSV csv;

void sendText(const char* text, uint64_t length) {
#if DEBUG
  Serial.println(text);
#else
  for (auto i = 0u; i < length; ++i) {
    if (i != 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    char c = text[i];
    uint8_t k = _kb_asciimap[c];
    if (k & 0x80) {
      // on linux it might not register shift immidietly  
      Keyboard.press(KEY_LEFT_SHIFT);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      Keyboard.press(c);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      Keyboard.release(c);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      Keyboard.release(KEY_LEFT_SHIFT);
    } else {
      Keyboard.pressRaw(k);
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
      Keyboard.releaseRaw(k);
    }
  }
#endif
}

std::vector<std::string> csvEntryActions = {
  "write",
  "edit"
};

void setup() {
  M5Cardputer.begin();

#if !DEBUG
  Keyboard.begin();
  USB.begin();
#endif
  
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
  ConsoleWindow::open<FilterMenuWindow>(lines, [](size_t, const std::string& name){
    ConsoleWindow::open<MenuWindow>(csvEntryActions, [&values = csv.getEntries()[name]](size_t action, const std::string&){
      ConsoleWindow::open<MenuWindow>(csv.getKeys(), [&values, action](size_t selectedIndex, const std::string&){
        switch (action) {
        case 0: /* write */ {
          const auto& value = values[selectedIndex];
          sendText(value.c_str(), value.length());
          break;
        }
        case 1: /* edit */ {
          auto& value = values[selectedIndex];
          ConsoleWindow::open<EditWindow>(value, [&value](const std::string& newValue){
            value = newValue;
            csv.flush();
          });
          break;
        }
        }
      });
    });
  });
}

void loop() {
    M5Cardputer.update();
    ConsoleWindow::update();
    FnKeyboard::update();
}
