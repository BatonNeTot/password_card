#pragma once

#include <SD.h>

#include <string>
#include <unordered_map>

using CSVEntry = std::unordered_map<std::string, std::string>;

class CSV {
  public:
    CSV(fs::FS &fs, const char* filename, const char* mainKey);

    inline const std::unordered_map<std::string, CSVEntry>& getEntries() const {
      return entries;
    }

  private:
    std::unordered_map<std::string, CSVEntry> entries;
};