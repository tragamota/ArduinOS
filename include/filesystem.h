#ifndef FAT_H
#define FAT_H

#include <EEPROM.h>

#define FILE_MAX_SIZE 12

typedef struct FAT {
    char name[FILE_MAX_SIZE];
    short position;
    short length;
} FATEntry;

const uint8_t FAT_MAGIC_NUMBER = 0x20;

void InitializeFilesystem();
bool ContainsFileInFAT(char* fileName);
int8_t HasEmptyFATEntry();

void writeFatEntry(FATEntry* entry);

#endif // FAT_H