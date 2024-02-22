#include "console_window.h"

#include <M5Cardputer.h>

std::vector<std::unique_ptr<ConsoleWindow>> ConsoleWindow::_windows;

void ConsoleWindow::update() {
  if (_windows.empty()) {
    return;
  }
  
  if (M5Cardputer.BtnA.wasClicked()) {
    if (pop()) {
      M5Cardputer.Display.clear();
      _windows.back()->draw();
    }
    return;
  }
  
  if (_windows.back()->_update()) {
    M5Cardputer.Display.clear();
    _windows.back()->draw();
  }
}

bool ConsoleWindow::pop() {
  if (_windows.size() > 1) {
    _windows.pop_back();
    return true;
  } else {
    return false;
  }
}
  
size_t ConsoleWindow::fontWidth() {
  return M5Cardputer.Display.fontWidth();
}
  
size_t ConsoleWindow::fontHeight() {
  return M5Cardputer.Display.fontHeight();
}

ConsoleWindow::ConsoleWindow(const std::vector<std::string>& options, std::function<void(size_t)> action)
  : _options(options), _action(action) {}

void ConsoleWindow::draw() {
  size_t firstVisibleElement = (-screenOffset) / fontHeight(); 
  size_t lastVisibleElement = std::min(_options.size(), firstVisibleElement + (M5Cardputer.Display.height() + (fontHeight() - 1)) / fontHeight());
  for (auto i = firstVisibleElement; i < lastVisibleElement; ++i) {
    const auto& line = _options[i];
    size_t offset = screenOffset + fontHeight() * i;
    
    if (i == selectedIndex) {
      M5Cardputer.Display.setColor(WHITE);
      M5Cardputer.Display.fillRect(0, offset, M5Cardputer.Display.width(), fontHeight());
      M5Cardputer.Display.setTextColor(BLACK);
    } else {
      M5Cardputer.Display.setTextColor(WHITE);
    }

    if (stringOffset < line.length()) {
      M5Cardputer.Display.drawString(line.c_str() + stringOffset, 0, offset);
    }
  }
}

bool ConsoleWindow::_update() {
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
    if ((selectedIndex + 1) * fontHeight() + screenOffset >= M5Cardputer.Display.height()) {
      screenOffset = -(selectedIndex + 1) * fontHeight() + M5Cardputer.Display.height();
    }
    return true;
  } else if (selectedIndex > 0 && M5Cardputer.Keyboard.isKeyPressed(';')) {
    --selectedIndex;
    if (selectedIndex * fontHeight() + screenOffset >= M5Cardputer.Display.height()) {
      screenOffset = -selectedIndex * fontHeight();
    }
    return true;
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    _action(selectedIndex);
    return true;
  }

  return false;
}
