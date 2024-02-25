#pragma once
#include "console_window.h"

class MenuWindow : public ConsoleWindow {
protected:
  MenuWindow(const std::vector<std::string>& options, std::function<void(size_t, const std::string&)> action);

  inline void setOptions(const std::vector<std::string>& options) {
    _options = options;
    _selectedIndex = std::min(_selectedIndex, _options.size() - 1);
  }
  
  void _draw() override;
  bool _update() override;

private:
  friend ConsoleWindow;

  std::vector<std::string> _options;
  std::function<void(size_t, const std::string&)> _action;

  size_t _screenOffset = 0;
  size_t _selectedIndex = 0;
};
