#pragma once

#include <SD.h>

#include <string>
#include <map>
#include <vector>

using CSVEntry = std::vector<std::string>;

class CSV {
  public:
    CSV() = default;
    CSV(fs::FS &fs, const char* filename, const char* mainKey);

    size_t getKeyIndex(const std::string& key) const;

    inline const std::vector<std::string>& getKeys() const {
      return keys;
    }

    inline const std::map<std::string, CSVEntry>& getEntries() const {
      return entries;
    }
    
  private:
    std::vector<std::string> keys;
    std::map<std::string, CSVEntry> entries;
};
