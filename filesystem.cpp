#include "filesystem.h"

#include <M5Cardputer.h>
#include <SPI.h>
#include <SD.h>

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

void Filesystem::init() {
  // SD Card Initialization
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        // Print a message if the SD card initialization
        // fails orif the SD card does not exist.
        M5Cardputer.Display.setTextColor(WHITE);
        M5Cardputer.Display.drawString("No SD card!", 0, 0);
        while(true);
    }
}
