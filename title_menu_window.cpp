#include "title_menu_window.h"

#include <M5Cardputer.h>

#include <algorithm>
#include <cctype>

TitleMenuWindow::TitleMenuWindow(const std::string& title, const std::vector<std::string>& options)
  : MenuWindow(options), _title(title) {}

#define LINE_SIZE 2
  
void TitleMenuWindow::_draw() {
  M5Cardputer.Display.setTextColor(WHITE);
  M5Cardputer.Display.drawString(_title.c_str(), left(), top());
  
  M5Cardputer.Display.setColor(WHITE);
  M5Cardputer.Display.fillRect(left(), top() + fontHeight(), width(), LINE_SIZE);
  
  setClipping(left(), top() + fontHeight() + LINE_SIZE, right(), bottom());
  MenuWindow::_draw();
  setClipping(left(), top() - fontHeight() - LINE_SIZE, right(), bottom());
}

void TitleMenuWindow::validateSelector() {
  setClipping(left(), top() + fontHeight() + LINE_SIZE, right(), bottom());
  MenuWindow::validateSelector();
  setClipping(left(), top() - fontHeight() - LINE_SIZE, right(), bottom());
}
