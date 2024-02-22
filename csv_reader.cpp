#include "csv_reader.h"

#include <M5Cardputer.h>
#include <SPI.h>
#include <SD.h>

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

void CSV::init() {
  // SD Card Initialization
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        // Print a message if the SD card initialization
        // fails orif the SD card does not exist.
        // TODO error message
        return;
    }
}

class BufferedFileReader {
public:

  BufferedFileReader(const char* filename)
    : file(SD.open(filename)) {}

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

CSV::CSV(const char* filename, const char* mainKey)
  : _filename(filename) {
  BufferedFileReader reader(filename);
  StringBuilder builder;

  if (!reader.eof()) {
    readRecord(_keys, reader, builder);
  }
  
  auto mainKeyIndex = getKeyIndex(mainKey);

  while (!reader.eof()) {
    CSVEntry entry;
    entry.reserve(_keys.size());
    readRecord(entry, reader, builder);

    if (entry.size() < _keys.size()) {
      entry.resize(_keys.size());
    }

    auto& mainValue = entry[mainKeyIndex];
    while (_entries.find(mainValue) != _entries.end()) {
      mainValue += " (1)";
    }
    
    _entries.emplace(mainValue, entry);
  }
}

size_t CSV::getKeyIndex(const std::string& key) const {
  for (auto i = 0u; i < _keys.size(); ++i) {
    if (_keys[i] == key) {
      return i;
    }
  }
  return -1;
}

class BufferedFileWriter {
public:

  BufferedFileWriter(const char* filename)
    : file(SD.open(filename, FILE_WRITE)) {}
  ~BufferedFileWriter() {
    flush();
  }

  void write(char c) {
    if (length >= sizeof(buffer)) {
      flush();
    }
    buffer[length++] = c;
  }

  void flush() {
    file.write((const uint8_t*)buffer, length);
    length = 0;
  }

private:
  File file;
  char buffer[512];
  size_t length = 0;
};

static void writeWord(BufferedFileWriter& writer, const std::string& value) {
  bool escaped = false;
  for (auto c : value) {
    if (c == '"' || c == '\n') {
      escaped = true;
      break;
    }
  }
  if (escaped) {
    writer.write('"');
  }
  for (auto c : value) {
    if (escaped && c == '"') {
      writer.write('"');
    }
    writer.write(c);
  }
  if (escaped) {
    writer.write('"');
  }
}

void CSV::flush() {
  BufferedFileWriter writer(_filename.c_str());
  
  for (auto i = 0u; i < _keys.size(); ++i) {
    if (i != 0u) {
      writer.write(',');
    }
    writeWord(writer, _keys[i]);
  }
  writer.write('\n');

  for (const auto& pair : _entries) {
    for (auto i = 0u; i < pair.second.size(); ++i) {
      if (i != 0u && pair.second.size() > 0) {
        writer.write(',');
      }
      writeWord(writer, pair.second[i]);
    }
    writer.write('\n');
  }
}
