#pragma once
#include "console_window.h"

class EditWindow : public ConsoleWindow {
public:

private:
  friend ConsoleWindow;
  EditWindow(const std::string& value, std::function<void(const std::string&)> action);
  
  void _draw() override;
  bool _update() override;

  std::string _value;  
  std::function<void(const std::string&)> _action;
};
