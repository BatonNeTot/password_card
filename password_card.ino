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
#include "filesystem.h"
#include "console_window.h"
#include "filter_menu_window.h"
#include "edit_window.h"
#include "fn_keyboard.h"

#include <vector>
#include <chrono>
#include <thread>

#define PASSWORD_CSV_FILENAME "/passwords.csv"

#define DEBUG 0

CSV csv;
std::vector<std::string> csvEntryNames;
auto csvEntryIt = csv.getEntries().end();
std::string csvEntryName;
size_t csvSelectedEntryIndex = -1;
size_t csvAction = -1;

void updateCsvEntryNames() {
  csvEntryNames.clear();
  csvEntryNames.reserve(csv.getEntries().size());
  for (const auto& pair : csv.getEntries()) {
    csvEntryNames.emplace_back(pair.first);
  }
}

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

#define ENTRIES_WINDOW 0
#define ACTION_WINDOW 1
#define VALUES_WINDOW 2
#define EDIT_WINDOW 3

class EditValueWindow : public EditWindow {
  friend ConsoleWindow;
  EditValueWindow()
    : EditWindow(csvEntryIt->second[csvSelectedEntryIndex]) {}
    
  size_t _getId() const override {
    return EDIT_WINDOW;
  }

  void onApply(const std::string& newValue) override {
    auto& value = csvEntryIt->second[csvSelectedEntryIndex];
    if (value == newValue) {
      return;
    }
    
    value = newValue;
    if (csvSelectedEntryIndex == csv.getMainKeyIndex()) {
      csvEntryName = newValue;
      auto newEntry = csv.getEntries().emplace(std::move(newValue), std::move(csvEntryIt->second)).first;
      csv.getEntries().erase(csvEntryIt);
      csvEntryIt = newEntry;
      updateCsvEntryNames();
    }
    
    csv.flush();
  }

  static ConsoleWindow::DeserializerRegister _deserializer;
};

class ValuesWindow : public TitleMenuWindow {
  friend ConsoleWindow;
  ValuesWindow()
    : TitleMenuWindow(csvEntryName, csv.getKeys()) {}
    
  size_t _getId() const override {
    return VALUES_WINDOW;
  }

  void onEnter() override {
    csvSelectedEntryIndex = getSelectedIndex();
    switch (csvAction) {
    case 0: /* write */ {
      const auto& value = csvEntryIt->second[csvSelectedEntryIndex];
      sendText(value.c_str(), value.length());
      break;
    }
    case 1: /* edit */ {
      ConsoleWindow::open<EditValueWindow>();
      break;
    }
    }
  }
  
  static ConsoleWindow::DeserializerRegister _deserializer;
};

std::vector<std::string> csvEntryActions = {
  "write",
  "edit"
};

class ActionWindow : public MenuWindow {
  friend ConsoleWindow;
  ActionWindow()
    : MenuWindow(csvEntryActions) {}
    
  size_t _getId() const override {
    return ACTION_WINDOW;
  }

  void onEnter() override {
    csvAction = getSelectedIndex();
    ConsoleWindow::open<ValuesWindow>();
  }

  void _deserialize(BufferedFileReader& reader) override {
    MenuWindow::_deserialize(reader);
    csvAction = getSelectedIndex();
  }
  
  static ConsoleWindow::DeserializerRegister _deserializer;
};

class EntriesWindow : public FilterMenuWindow {
  friend ConsoleWindow;
  EntriesWindow()
    : FilterMenuWindow(csvEntryNames) {}
    
  size_t _getId() const override {
    return ENTRIES_WINDOW;
  }

  void onEnter() override {
    csvEntryIt = csv.getEntries().find(getSelectedOption());
    csvEntryName = csvEntryIt->first;
    ConsoleWindow::open<ActionWindow>();
  }

  void _deserialize(BufferedFileReader& reader) override {
    FilterMenuWindow::_deserialize(reader);
    csvEntryIt = csv.getEntries().find(getSelectedOption());
    csvEntryName = csvEntryIt->first;
  }
  
  static ConsoleWindow::DeserializerRegister _deserializer;
};

ConsoleWindow::DeserializerRegister EditValueWindow::_deserializer(EDIT_WINDOW, [](){ 
  return new EditValueWindow(); 
  });

ConsoleWindow::DeserializerRegister ValuesWindow::_deserializer(VALUES_WINDOW, [](){ 
  return new ValuesWindow(); 
  });

ConsoleWindow::DeserializerRegister ActionWindow::_deserializer(ACTION_WINDOW, [](){ 
  return new ActionWindow(); 
  });

ConsoleWindow::DeserializerRegister EntriesWindow::_deserializer(ENTRIES_WINDOW, [](){ 
  return new EntriesWindow(); 
  });

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
  
  Filesystem::init();
  
  csv = CSV(PASSWORD_CSV_FILENAME, "name");
  updateCsvEntryNames();

  if (!ConsoleWindow::deserializeStack()) {
    ConsoleWindow::open<EntriesWindow>();
  }
}

void loop() {
    M5Cardputer.update();
    ConsoleWindow::update();
    FnKeyboard::update();
}
