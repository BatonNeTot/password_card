#include "fn_keyboard.h"

#include <M5Cardputer.h>

namespace FnKeyboard {
  
  char getTypedChar() {
    Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
    if (status.ctrl && status.enter) {
      return '\n';
    }
    return status.word.empty() ? '\0' : *status.word.begin();
  }

  bool isBackspace() {
    return M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE);
  }

  void update() {}
  
}
