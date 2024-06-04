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

        for(int i = 0; i < FAT_FILES; i++) {
            FATEntry emptyFatEntry;

            emptyFatEntry.name[0] = '\0';
            emptyFatEntry.position = 0;
            emptyFatEntry.length = 0;

            EEPROM.put(i * sizeof(FATEntry) + 1, emptyFatEntry);
        }
    }
}

int8_t FindEntryByFilename(char* fileName)
{
    FATEntry fileEntry;
    
    for(int i = 0; i < noOfFiles; i++) {
        EEPROM.get(i * sizeof(FATEntry) + 1, fileEntry);

        if(strcmp(fileName, fileEntry.name) == 0) {
            return i;
        }
    }

    return -1;
}

int8_t GetEmptyFATEntryIndex()
{
    FATEntry fileEntry;
    
    for(int i = 0; i < FAT_FILES; i++) {
        EEPROM.get(i * sizeof(FATEntry) + 1, fileEntry);

        if(fileEntry.length == 0) {
            return i;
        }
    }

    return -1;
}
