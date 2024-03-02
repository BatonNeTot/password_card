#pragma once
#include "console_window.h"

class MenuWindow : public ConsoleWindow {
public:
  
  virtual void validateSelector();

  inline size_t getSelectedIndex() const {
    return _selectedIndex;
  }

  inline const std::string& getSelectedOption() const {
    return _options[_selectedIndex];
  }
  
protected:
  MenuWindow(const std::vector<std::string>& options);
  
  void _draw() override;
  bool _update() override;
  
  void _serialize(BufferedFileWriter& writer) override;
  void _deserialize(BufferedFileReader& reader) override;

private:
  friend ConsoleWindow;

  virtual void onEnter() {};

  const std::vector<std::string>& _options;

  size_t _screenOffset = 0;
  size_t _selectedIndex = 0;
};
