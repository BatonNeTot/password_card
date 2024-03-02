#pragma once

#include <string>
#include <map>
#include <vector>

using CSVEntry = std::vector<std::string>;

class CSV {
  public:
    CSV() = default;
    CSV(const char* filename, const char* mainKey);

    size_t getKeyIndex(const std::string& key) const;

    inline const std::vector<std::string>& getKeys() const {
      return _keys;
    }
    inline size_t getMainKeyIndex() const {
      return _mainKeyIndex;
    }

    inline std::map<std::string, CSVEntry>& getEntries() {
      return _entries;
    }
    inline const std::map<std::string, CSVEntry>& getEntries() const {
      return _entries;
    }

    void flush();
    
  private:
    std::string _filename;
    std::vector<std::string> _keys;
    size_t _mainKeyIndex;
    std::map<std::string, CSVEntry> _entries;
};
