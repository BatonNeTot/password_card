#pragma once
#include "menu_window.h"

class TitleMenuWindow : public MenuWindow {
public:
  void validateSelector() override;
  
protected:
  TitleMenuWindow(const std::string& title, const std::vector<std::string>& options);

private:
  friend ConsoleWindow;
  
  void _draw() override;

  const std::string& _title;
};
