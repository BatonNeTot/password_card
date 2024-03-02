#include "console_window.h"

#include "filesystem.h"
#include "fn_keyboard.h"

#include <M5Cardputer.h>

#include <functional>
#include <cassert>
#include <unordered_map>

using WindowDeserializers = std::unordered_map<size_t, std::function<ConsoleWindow*()>>;

static WindowDeserializers& getWindowDeserializers() {
  static WindowDeserializers _windowDeserializers;
  return _windowDeserializers;
}

ConsoleWindow::DeserializerRegister::DeserializerRegister(size_t id, const std::function<ConsoleWindow*()>& deserializer) {
  assert(getWindowDeserializers().emplace(id, deserializer).second);
}

std::vector<std::unique_ptr<ConsoleWindow>> ConsoleWindow::_windows;

#define SAVED_STACK_FILENAME "/.window_stask"

void ConsoleWindow::serializeStack() {
  BufferedFileWriter writer(SAVED_STACK_FILENAME);

  for (auto& window : _windows) {
    window->_serialize(writer);
  }
}

bool ConsoleWindow::deserializeStack() {
  BufferedFileReader reader(SAVED_STACK_FILENAME);
  if (!reader) {
    return false;
  }

  while (!reader.eof()) {
    auto id = reader.readInteger<size_t>();
    auto deserializerIt = getWindowDeserializers().find(id);
    if (deserializerIt == getWindowDeserializers().end()) {
      return true;
    }
    auto window = deserializerIt->second();
    _windows.emplace_back(std::unique_ptr<ConsoleWindow>(window));
    window->_deserialize(reader);
  }

  draw();
  return true;
}

void ConsoleWindow::draw() {
  if (_windows.empty()) {
    return;
  }
  
  M5Cardputer.Display.clear();
  M5Cardputer.Display.setClipRect(_windows.back()->_left, _windows.back()->_top, _windows.back()->width(), _windows.back()->height());
  _windows.back()->_draw();
}

void ConsoleWindow::update() {
  if (_windows.empty()) {
    return;
  }
  
  if (M5Cardputer.BtnA.wasClicked()) {
    if (pop()) {
      draw();
      serializeStack();
    }
    return;
  }
  
  if (_windows.back()->_update()) {
    draw();
    serializeStack();
  }
}

bool ConsoleWindow::pop() {
  if (_windows.size() > 1) {
    _windows.pop_back();

    _windows.back()->_afterBack();
    
    return true;
  } else {
    return false;
  }
}

ConsoleWindow::ConsoleWindow()
  : _left(0), _top(0)
  , _right(M5Cardputer.Display.width())
  , _bottom(M5Cardputer.Display.height()) {}

void ConsoleWindow::setClipping(size_t left, size_t top, size_t right, size_t bottom) {
  _left = left;
  _top = top;
  _right = right;
  _bottom = bottom;
  M5Cardputer.Display.setClipRect(_left, _top, width(), height());
}
  
size_t ConsoleWindow::fontWidth() {
  return M5Cardputer.Display.fontWidth();
}
  
size_t ConsoleWindow::fontHeight() {
  return M5Cardputer.Display.fontHeight();
}

void ConsoleWindow::_serialize(BufferedFileWriter& writer) {
  writer.write(_getId());
}
