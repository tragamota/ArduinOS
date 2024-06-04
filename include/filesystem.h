#ifndef FAT_H
#define FAT_H

#include <EEPROM.h>

#define FILE_NAME_MAX_SIZE 12
#define FAT_FILES 10

typedef struct FAT {
    char name[FILE_NAME_MAX_SIZE];
    short position;
    short length;
} FATEntry;

const uint8_t FAT_MAGIC_NUMBER = 0x20;

void InitializeFilesystem();
int8_t FindEntryByFilename(char* fileName);
int8_t GetEmptyFATEntryIndex();

void WriteFatEntry(uint8_t index, FATEntry* entry);

#endif // FAT_H