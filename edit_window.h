#pragma once
#include "console_window.h"

class EditWindow : public ConsoleWindow {
public:
  void draw() override;

private:
  friend ConsoleWindow;
  EditWindow(const std::string& value, std::function<void(const std::string&)> action);
  
  bool _update() override;

  std::string _value;  
  std::function<void(const std::string&)> _action;
};
