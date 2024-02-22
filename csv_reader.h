#pragma once

#include <string>
#include <map>
#include <vector>

using CSVEntry = std::vector<std::string>;

class CSV {
  public:
    static void init();
  
    CSV() = default;
    CSV(const char* filename, const char* mainKey);

    size_t getKeyIndex(const std::string& key) const;

    inline const std::vector<std::string>& getKeys() const {
      return _keys;
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
    std::map<std::string, CSVEntry> _entries;
};
