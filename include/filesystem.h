#ifndef FAT_H
#define FAT_H

#include <EEPROM.h>

#define FILE_NAME_MAX_SIZE 12
#define FAT_FILES 10

static EERef noOfFiles = EEPROM[161];
const uint8_t FAT_MAGIC_NUMBER = 0x20;

typedef struct FAT {
    char name[FILE_NAME_MAX_SIZE];
    short position;
    short length;
} FATEntry;

void InitializeFilesystem();
int8_t FindEntryByFilename(const char* fileName);

uint16_t FindSpaceAddress(uint16_t size);
uint16_t FileFreespace();

void WriteFatEntry(const uint8_t index, const FATEntry* entry);
void RemoveFatEntry(const uint8_t index);
FATEntry* ReadFatEntry(const uint8_t index);

void WriteFatData(const uint16_t address, const uint8_t* data, const int size);
uint8_t* ReadFatData(const uint16_t address, const int size);

#endif // FAT_H