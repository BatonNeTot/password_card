#include "filter_menu_window.h"

#include "fn_keyboard.h"

#include <M5Cardputer.h>

#include <algorithm>
#include <cctype>

FilterMenuWindow::FilterMenuWindow(const std::vector<std::string>& options, std::function<void(size_t, const std::string&)> action)
  : MenuWindow(options, action), _unfilteredOptions(options) {}

void FilterMenuWindow::_draw() {
  M5Cardputer.Display.setTextColor(WHITE);
  M5Cardputer.Display.drawString(_filter.c_str(), left(), top());
  
  size_t lineSize = 2;
  M5Cardputer.Display.setColor(WHITE);
  M5Cardputer.Display.fillRect(left(), top() + fontHeight(), width(), lineSize);
  
  setClipping(left(), top() + fontHeight() + lineSize, right(), bottom());
  MenuWindow::_draw();
  setClipping(left(), top() - fontHeight() - lineSize, right(), bottom());
}

bool FilterMenuWindow::_update() {
  if (MenuWindow::_update()) {
    return true;
  }

  if (FnKeyboard::isBackspace()) {
    if (_filter.length() == 0) {
      return false;
    }
    
    _filter = _filter.substr(0, _filter.length() - 1);
  } else {
    char c = FnKeyboard::getTypedChar();
    if (c == '\0') {
      return false;
    }
    
    _filter += c;
  }

  if (_filter.empty()) {
    setOptions(_unfilteredOptions);
  } else {
    std::vector<std::string> options;
    options.reserve(_unfilteredOptions.size());

    std::string lowerFilter = _filter;
    std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(),
      [](unsigned char c){ return std::tolower(c); });

    for (const auto& option : _unfilteredOptions) {
      std::string lowerOption = option;
      std::transform(lowerOption.begin(), lowerOption.end(), lowerOption.begin(),
        [](unsigned char c){ return std::tolower(c); });

      if (lowerOption.find(lowerFilter) != std::string::npos) {
        options.emplace_back(option);
      }
    }
    
    setOptions(options);
  }

  return true;
}
