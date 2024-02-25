#pragma once
#include "menu_window.h"

class FilterMenuWindow : public MenuWindow {
public:

private:
  friend ConsoleWindow;
  FilterMenuWindow(const std::vector<std::string>& options, std::function<void(size_t, const std::string&)> action);
  
  void _draw() override;
  bool _update() override;

  std::string _filter;
  std::vector<std::string> _unfilteredOptions;
};
