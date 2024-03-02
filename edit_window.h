#pragma once
#include "console_window.h"

class EditWindow : public ConsoleWindow {
protected:
  EditWindow(const std::string& value);

private:
  friend ConsoleWindow;
  
  void _draw() override;
  bool _update() override;
  
  void _serialize(BufferedFileWriter& writer) override;
  void _deserialize(BufferedFileReader& reader) override;
  
  virtual void onApply(const std::string& value) = 0;

  std::string _value;  
};
