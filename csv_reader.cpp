#include "csv_reader.h"

#include <M5Cardputer.h>

#include <vector>

CSV::CSV(fs::FS &fs, const char* filename, const char* mainKey) {
    File file = fs.open(filename);

    char buffer[512]; 
    size_t length = file.readBytesUntil('\n', buffer, sizeof(buffer));
    size_t start = 0;

    std::vector<std::string> keys;
    for (auto i = start; i < length; ++i) {
      if (buffer[i] == ',') {
       keys.emplace_back(buffer + start, i - start);
       start = i + 1; 
      }
    }
    keys.emplace_back(buffer + start, length - start);
    lines.emplace_back(buffer, length);

    std::string mainKeyStr = mainKey;
    while (file.position() < file.size()) {
      CSVEntry entry;
      std::string line;

      for (auto i = 0u; i < keys.size() - 1; ++i) {
        length = file.readBytesUntil(',', buffer, sizeof(buffer));
        std::string value(buffer, length);
        if (i != 0) {
          line += ",";
        }
        //line += value;
        entry.emplace(keys[i], value);
      }
      length = file.readBytesUntil('\n', buffer, sizeof(buffer));
      std::string value(buffer, length);
      line += ",";
      line += value;
      entry.emplace(keys.back(), value);

      std::string mainValue = entry[mainKeyStr];
      while (entries.find(mainValue) != entries.end()) {
        mainValue += " (1)";
      }
      
      entries.emplace(mainValue, entry);
      lines.emplace_back(line);
    }
}
