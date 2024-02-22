#pragma once

#include <vector>
#include <string>
#include <functional>

class ConsoleWindow {
public:
  static void openWindow(const std::vector<std::string>& options, std::function<void(size_t)> action);

  static void update();
  static bool popWindow();
  
  static size_t textHeight();
  
  ConsoleWindow() = default;
  ConsoleWindow(const std::vector<std::string>& options, std::function<void(size_t)> action);

  void draw();

private:
  bool _update();

  std::vector<std::string> _options;
  std::function<void(size_t)> _action;

  size_t stringOffset = 0;
  size_t screenOffset = 0;
  size_t selectedIndex = 0;
};
