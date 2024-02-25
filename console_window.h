#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

class ConsoleWindow {
public:
  virtual ~ConsoleWindow() = default;
  
  template <class T, class... Args>
  static void open(Args&&... args) {
    _windows.emplace_back(std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
    draw();
  }

  static void draw();
  static void update();
  static bool pop();
  
  static size_t fontWidth();
  static size_t fontHeight();

  void setClipping(size_t left, size_t top, size_t right, size_t bottom);

  inline size_t left() {
    return _left;
  }
  inline size_t top() {
    return _top;
  }
  inline size_t right() {
    return _right;
  }
  inline size_t bottom() {
    return _bottom;
  }
  
  inline size_t width() {
    return _right - _left;
  }
  inline size_t height() {
    return _bottom - _top;
  }

protected:
  ConsoleWindow();
  
  virtual void _draw() = 0;
  virtual bool _update() = 0;

private:
  static std::vector<std::unique_ptr<ConsoleWindow>> _windows;

  size_t _left;
  size_t _top;
  size_t _right;
  size_t _bottom;
};
