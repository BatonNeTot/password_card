#pragma once

#include <SD.h>

#include <string>

namespace Filesystem {
  
   void init();
  
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

  template <class T>
  T readInteger() {
    union {
      T value;
      char buffer[sizeof(T)];
    } u;

    for (size_t i = 0; i < sizeof(T); ++i) {
      u.buffer[i] = read();
    }
    return u.value;
  }

  std::string readString() {
    auto length = readInteger<size_t>();
    char buffer[512];
    for (size_t i = 0; i < length; ++i) {
      buffer[i] = read();
    }
    return std::string(buffer, length);
  }

  bool eof() const {
    return pos >= capacity && file.position() >= file.size();
  }

  explicit operator bool() const {
    return file;
  }

private:
  File file;
  char buffer[512];
  size_t capacity = 0;
  size_t pos = 0;
};

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

  void write(const std::string& str) {
    write(str.length());
    for (auto c : str) {
      write(c);
    }
  }

  template <class T>
  void write(T value) {
    union {
      T value;
      char buffer[sizeof(T)];
    } u;

    u.value = value;
    for (size_t i = 0; i < sizeof(T); ++i) {
      write(u.buffer[i]);
    }
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
