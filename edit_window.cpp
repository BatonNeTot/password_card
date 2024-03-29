#include "edit_window.h"

#include <M5Cardputer.h>

#include "fn_keyboard.h"

EditWindow::EditWindow(const std::string& value)
  : _value(value) {}
  
void EditWindow::_draw() {
  size_t x = 0;
  size_t y = 0;
  char buffer[2];
  buffer[1] = '\0';
  for (auto c : _value) {
    if (c == '\n') {
      x = 0;
      y += fontHeight();
    } else {
      buffer[0] = c;
      M5Cardputer.Display.drawString(buffer, x, y);
      x += fontWidth();
    }
  }
}

bool EditWindow::_update() {
  if (!FnKeyboard::isPressed()) {
    return false;
  }
  
  Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
  if (!status.ctrl && status.enter) {
    onApply(_value);
    pop();
    return true;
  }

  if (FnKeyboard::isBackspace()) {
    if (_value.length() > 0) {
      _value = _value.substr(0, _value.length() - 1);
      return true;
    }
  } else {
    char c = FnKeyboard::getTypedChar();
    if (c != '\0') {
      _value += c;
      return true;
    }
  }
  
  return false;
}

void EditWindow::_serialize(BufferedFileWriter& writer) {
  ConsoleWindow::_serialize(writer);
  writer.write(_value);
}

void EditWindow::_deserialize(BufferedFileReader& reader) {
  ConsoleWindow::_deserialize(reader);
  _value = reader.readString();
}
