#include "console_window.h"

#include <M5Cardputer.h>

static std::vector<ConsoleWindow> _windows;

void ConsoleWindow::openWindow(const std::vector<std::string>& options, std::function<void(size_t)> action) {
  _windows.emplace_back(options, action);
  _windows.back().draw();
}

void ConsoleWindow::update() {
  if (_windows.empty()) {
    return;
  }
  if (_windows.back()._update()) {
    _windows.back().draw();
  }
}

bool ConsoleWindow::_update() {
  if (M5Cardputer.BtnA.wasClicked() && _windows.size() > 1) {
    _windows.pop_back();
    return true;
  }
  
  if (!M5Cardputer.Keyboard.isChange() || !M5Cardputer.Keyboard.isPressed()) {
    return false;
  }
  
  if (stringOffset > 0 && M5Cardputer.Keyboard.isKeyPressed(',')) {
    --stringOffset;
    return true;
  } else if (M5Cardputer.Keyboard.isKeyPressed('/')) {
    ++stringOffset;
    return true;
  } else if (selectedIndex < _options.size() - 1 && M5Cardputer.Keyboard.isKeyPressed('.')) {
    ++selectedIndex;
    if ((selectedIndex + 1) * M5Cardputer.Display.fontHeight() + screenOffset >= M5Cardputer.Display.height()) {
      screenOffset = -(selectedIndex + 1) * M5Cardputer.Display.fontHeight() + M5Cardputer.Display.height();
    }
    return true;
  } else if (selectedIndex > 0 && M5Cardputer.Keyboard.isKeyPressed(';')) {
    --selectedIndex;
    if (selectedIndex * M5Cardputer.Display.fontHeight() + screenOffset >= M5Cardputer.Display.height()) {
      screenOffset = -selectedIndex * M5Cardputer.Display.fontHeight();
    }
    return true;
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    _action(selectedIndex);
    return true;
  }

  return false;
}

ConsoleWindow::ConsoleWindow(const std::vector<std::string>& options, std::function<void(size_t)> action)
  : _options(options), _action(action) {}

void ConsoleWindow::draw() {
  M5Cardputer.Display.clear();
  size_t offset = screenOffset;
  for (auto i = 0u; i < _options.size(); ++i) {
    const auto& line = _options[i];
    
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
