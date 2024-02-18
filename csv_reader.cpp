#include "csv_reader.h"

#include <vector>

CSV::CSV(fs::FS &fs, const char* filename, const char* mainKey) {
    File file = fs.open(filename);

    char buffer[512]; 
    size_t length = file.readBytesUntil('\n', buffer, sizeof(buffer));

    std::vector
    while (1) {

    }
}