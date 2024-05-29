#include "filesystem.h"
#include <HardwareSerial.h>
#include <USBAPI.h>

int8_t noOfFiles = 0;

void InitializeFilesystem()
{
    uint8_t fatHeaderValue; 
    
    EEPROM.get(0, fatHeaderValue);
    EEPROM.get(161, noOfFiles);

    // Initialize FAT filesystem
    if(fatHeaderValue != FAT_MAGIC_NUMBER) {
        Serial.println("Initializing Filesystem");
        EEPROM.write(0, FAT_MAGIC_NUMBER);
        EEPROM.write(161, 0);

        for(int i = 0; i < 10; i++) {
            FATEntry emptyFatEntry;

            emptyFatEntry.name[0] = '\0';
            emptyFatEntry.position = 0;
            emptyFatEntry.length = 0;

            EEPROM.put(i * sizeof(FATEntry) + 1, emptyFatEntry);
        }
    }
}

int8_t HasEmptyFATEntry()
{
    return false;
}
