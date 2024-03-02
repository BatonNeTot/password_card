#include "menu_window.h"

#include "fn_keyboard.h"

#include <M5Cardputer.h>

MenuWindow::MenuWindow(const std::vector<std::string>& options)
  : _options(options) {}

void MenuWindow::_draw() {
  size_t firstVisibleElement = std::max(0, (int)(_screenOffset / fontHeight())); 
  size_t lastVisibleElement = std::min(_options.size(), firstVisibleElement + (height() + (fontHeight() - 1)) / fontHeight());
  for (auto i = firstVisibleElement; i < lastVisibleElement; ++i) {
    const auto& line = _options[i];
    size_t offset = top() - _screenOffset + fontHeight() * i;
    
    if (i == _selectedIndex) {
      M5Cardputer.Display.setColor(WHITE);
      M5Cardputer.Display.fillRect(left(), offset, width(), fontHeight());
      M5Cardputer.Display.setTextColor(BLACK);
    } else {
      M5Cardputer.Display.setTextColor(WHITE);
    }

    M5Cardputer.Display.drawString(line.c_str(), left(), offset);
  }
}

void MenuWindow::validateSelector() {
  _selectedIndex = std::min(_selectedIndex, _options.size() - 1);
  if ((_selectedIndex + 1) * fontHeight() - _screenOffset > height()) {
    _screenOffset = (_selectedIndex + 1) * fontHeight() - height();
  }
  if ((int)(_selectedIndex * fontHeight() - _screenOffset) <= 0) {
    _screenOffset = _selectedIndex * fontHeight();
  }
}

bool MenuWindow::_update() {
  if (!FnKeyboard::isPressed()) {
    return false;
  }
  
  if (_selectedIndex < _options.size() - 1 && M5Cardputer.Keyboard.isKeyPressed('.')) {
    ++_selectedIndex;
    validateSelector();
    return true;
  } else if (_selectedIndex > 0 && M5Cardputer.Keyboard.isKeyPressed(';')) {
    --_selectedIndex;
    validateSelector();
    return true;
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    onEnter();
    return true;
  }

  return false;
}

void MenuWindow::_serialize(BufferedFileWriter& writer) {
  ConsoleWindow::_serialize(writer);
  writer.write(_screenOffset);
  writer.write(_selectedIndex);
}

void MenuWindow::_deserialize(BufferedFileReader& reader) {
  ConsoleWindow::_deserialize(reader);
  _screenOffset = reader.readInteger<size_t>();
  _selectedIndex = reader.readInteger<size_t>();
  validateSelector();
}
