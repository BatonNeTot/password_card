#include "filter_menu_window.h"

#include "fn_keyboard.h"

#include <M5Cardputer.h>

#include <algorithm>
#include <cctype>

FilterMenuWindow::FilterMenuWindow(const std::vector<std::string>& options)
  : MenuWindow(_filteredOptions), _unfilteredOptions(options), _filteredOptions(options) {}

#define LINE_SIZE 2
  
void FilterMenuWindow::_draw() {
  M5Cardputer.Display.setTextColor(WHITE);
  M5Cardputer.Display.drawString(_filter.c_str(), left(), top());
  
  M5Cardputer.Display.setColor(WHITE);
  M5Cardputer.Display.fillRect(left(), top() + fontHeight(), width(), LINE_SIZE);
  
  setClipping(left(), top() + fontHeight() + LINE_SIZE, right(), bottom());
  MenuWindow::_draw();
  setClipping(left(), top() - fontHeight() - LINE_SIZE, right(), bottom());
}

void FilterMenuWindow::validateSelector() {
  setClipping(left(), top() + fontHeight() + LINE_SIZE, right(), bottom());
  MenuWindow::validateSelector();
  setClipping(left(), top() - fontHeight() - LINE_SIZE, right(), bottom());
}

bool FilterMenuWindow::_update() {
  if (!FnKeyboard::isPressed()) {
    return false;
  }
  
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

  updateFilter();

  return true;
}

void FilterMenuWindow::_afterBack() {
  updateFilter();
}

void FilterMenuWindow::updateFilter() {
  if (_filter.empty()) {
    _filteredOptions = _unfilteredOptions;
  } else {
    _filteredOptions.clear();
    _filteredOptions.reserve(_unfilteredOptions.size());

    std::string lowerFilter = _filter;
    std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(),
      [](unsigned char c){ return std::tolower(c); });

    for (const auto& option : _unfilteredOptions) {
      std::string lowerOption = option;
      std::transform(lowerOption.begin(), lowerOption.end(), lowerOption.begin(),
        [](unsigned char c){ return std::tolower(c); });

      if (lowerOption.find(lowerFilter) != std::string::npos) {
        _filteredOptions.emplace_back(option);
      }
    }
  }
    
  validateSelector();
}

void FilterMenuWindow::_serialize(BufferedFileWriter& writer) {
  MenuWindow::_serialize(writer);
  writer.write(_filter);
}

void FilterMenuWindow::_deserialize(BufferedFileReader& reader) {
  MenuWindow::_deserialize(reader);
  _filter = reader.readString();
  updateFilter();
}
