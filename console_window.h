#pragma once

#include "filesystem.h"

#include <vector>
#include <string>
#include <functional>
#include <memory>

class ConsoleWindow {
public:
  virtual ~ConsoleWindow() = default;
  
  template <class T, class... Args>
  static T* open(Args&&... args) {
    auto windowPtr = new T(std::forward<Args>(args)...);
    _windows.emplace_back(std::unique_ptr<T>(windowPtr));
    draw();
    serializeStack();
    return windowPtr;
  }
  
  static void serializeStack();
  static bool deserializeStack();

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

  class DeserializerRegister {
  public:
    DeserializerRegister(size_t id, const std::function<ConsoleWindow*()>& deserializer);
  };

protected:
  ConsoleWindow();
  
  virtual void _draw() = 0;
  virtual bool _update() = 0;
  virtual void _afterBack() {};

  virtual void _serialize(BufferedFileWriter& writer);
  virtual void _deserialize(BufferedFileReader& reader) {};

private:
  static std::vector<std::unique_ptr<ConsoleWindow>> _windows;

  virtual size_t _getId() const { return 0; };

  size_t _left;
  size_t _top;
  size_t _right;
  size_t _bottom;
};
