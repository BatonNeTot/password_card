#include "menu_window.h"

#include <M5Cardputer.h>

MenuWindow::MenuWindow(const std::vector<std::string>& options, std::function<void(size_t, const std::string&)> action)
  : _options(options), _action(action) {}

void MenuWindow::_draw() {
  size_t firstVisibleElement = (-_screenOffset) / fontHeight(); 
  size_t lastVisibleElement = std::min(_options.size(), firstVisibleElement + (height() + (fontHeight() - 1)) / fontHeight());
  for (auto i = firstVisibleElement; i < lastVisibleElement; ++i) {
    const auto& line = _options[i];
    size_t offset = top() + _screenOffset + fontHeight() * i;
    
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

bool MenuWindow::_update() {  
  if (_selectedIndex < _options.size() - 1 && M5Cardputer.Keyboard.isKeyPressed('.')) {
    ++_selectedIndex;
    if ((_selectedIndex + 1) * fontHeight() + _screenOffset >= height()) {
      _screenOffset = -(_selectedIndex + 1) * fontHeight() + height();
    }
    return true;
  } else if (_selectedIndex > 0 && M5Cardputer.Keyboard.isKeyPressed(';')) {
    --_selectedIndex;
    if (_selectedIndex * fontHeight() + _screenOffset >= height()) {
      _screenOffset = -_selectedIndex * fontHeight();
    }
    return true;
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    _action(_selectedIndex, _options[_selectedIndex]);
    return true;
  }

  return false;
}
