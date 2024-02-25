#include "console_window.h"

#include <M5Cardputer.h>

std::vector<std::unique_ptr<ConsoleWindow>> ConsoleWindow::_windows;

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
    }
    return;
  }
  
  if (!M5Cardputer.Keyboard.isChange() || !M5Cardputer.Keyboard.isPressed()) {
    return;
  }
  
  if (_windows.back()->_update()) {
    draw();
  }
}

bool ConsoleWindow::pop() {
  if (_windows.size() > 1) {
    _windows.pop_back();
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
