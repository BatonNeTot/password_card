#pragma once

#include <SD.h>

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

using CSVEntry = std::unordered_map<std::string, std::string>;

class CSV {
  public:
    CSV() = default;
    CSV(fs::FS &fs, const char* filename, const char* mainKey);

    inline const std::map<std::string, CSVEntry>& getEntries() const {
      return entries;
    }
    inline const std::vector<std::string>& getLines() const {
      return lines;
    }

  private:
    std::map<std::string, CSVEntry> entries;
    std::vector<std::string> lines;
};
