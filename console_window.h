#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

class ConsoleWindow {
public:
  template <class T, class... Args>
  static void open(Args&&... args) {
    _windows.emplace_back(std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
    _windows.back()->draw();
  }

  static void update();
  static bool pop();
  
  static size_t fontWidth();
  static size_t fontHeight();

  virtual void draw();

protected:
  ConsoleWindow() = default;
  ConsoleWindow(const std::vector<std::string>& options, std::function<void(size_t)> action);

private:
  static std::vector<std::unique_ptr<ConsoleWindow>> _windows;
  
  virtual bool _update();

  std::vector<std::string> _options;
  std::function<void(size_t)> _action;

  size_t stringOffset = 0;
  size_t screenOffset = 0;
  size_t selectedIndex = 0;
};
