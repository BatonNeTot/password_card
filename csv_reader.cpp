#include "csv_reader.h"

#include <M5Cardputer.h>

class BufferedFileReader {
public:

  BufferedFileReader(fs::FS &fs, const char* filename)
    : file(fs.open(filename)) {}

  char read() {
    if (pos >= capacity) {
      capacity = file.readBytes(buffer, sizeof(buffer));
      pos = 0;
    }
    return eof() ? '\0' : buffer[pos++];
  }

  bool eof() const {
    return pos >= capacity && file.position() >= file.size();
  }

private:
  File file;
  char buffer[512];
  size_t capacity = 0;
  size_t pos = 0;
};

class StringBuilder {
public:

  void write(char c) {
    if (length >= sizeof(buffer)) {
      return;
    }
    buffer[length++] = c;
  }

  void build(std::vector<std::string>& container) {
    container.emplace_back(buffer, length);
    length = 0;
  }
  
private:
  char buffer[512];
  size_t length = 0;
};

static void readRecord(std::vector<std::string>& container, BufferedFileReader& reader, StringBuilder& builder) {
  char c = reader.read();
  bool literal = false;
  while (1) {
    if (c == 0) {
      builder.build(container);
      return;
    }
    
    if (literal) {
      if (c == '"') {
        c = reader.read();
        if (c != '"') {
          literal = false;
          continue;
        }
      }
    } else {
      switch(c) {
      case '"':
        literal = true;
        c = reader.read();
        continue;
      case ',':
        c = reader.read();
        builder.build(container);
        continue;
      case '\n':
        builder.build(container);
        return;
      }
    }

    builder.write(c);
    c = reader.read();
  }
}

CSV::CSV(fs::FS &fs, const char* filename, const char* mainKey) {
  BufferedFileReader reader(fs, filename);
  StringBuilder builder;

  if (!reader.eof()) {
    readRecord(keys, reader, builder);
  }
  
  auto mainKeyIndex = getKeyIndex(mainKey);

  while (!reader.eof()) {
    CSVEntry entry;
    entry.reserve(keys.size());
    readRecord(entry, reader, builder);

    if (entry.size() < keys.size()) {
      entry.resize(keys.size());
    }

    auto& mainValue = entry[mainKeyIndex];
    while (entries.find(mainValue) != entries.end()) {
      mainValue += " (1)";
    }
    
    entries.emplace(mainValue, entry);
  }
}

size_t CSV::getKeyIndex(const std::string& key) const {
  for (auto i = 0u; i < keys.size(); ++i) {
    if (keys[i] == key) {
      return i;
    }
  }
  return -1;
}
