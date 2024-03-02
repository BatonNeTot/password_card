#pragma once
#include "menu_window.h"

class FilterMenuWindow : public MenuWindow {
public:
  void validateSelector() override;
  
protected:
  FilterMenuWindow(const std::vector<std::string>& options);
  
  void _serialize(BufferedFileWriter& writer) override;
  void _deserialize(BufferedFileReader& reader) override;

private:
  friend ConsoleWindow;
  
  void _draw() override;
  bool _update() override;
  void _afterBack() override;

  void updateFilter();

  std::string _filter;
  const std::vector<std::string>& _unfilteredOptions;
  std::vector<std::string> _filteredOptions;
};
