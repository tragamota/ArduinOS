#include "filesystem.h"
#include <HardwareSerial.h>
#include <USBAPI.h>

int compareByFileAddress(const void *a, const void *b) {
    FATEntry *EntryA = (FATEntry *)a;
    FATEntry *EntryB = (FATEntry *)b;
    return (EntryA->position - EntryB->position);
}

void InitializeFilesystem()
{
    uint8_t fatHeaderValue;

    EEPROM.get(0, fatHeaderValue);

    // Initialize FAT filesystem
    if (fatHeaderValue != FAT_MAGIC_NUMBER)
    {
        Serial.println(F("Initializing Filesystem"));
        EEPROM.write(0, FAT_MAGIC_NUMBER);
        EEPROM.write(161, 0);

        for (int i = 0; i < FAT_FILES; i++)
        {
            FATEntry emptyFatEntry;

            emptyFatEntry.name[0] = '\0';
            emptyFatEntry.position = 0;
            emptyFatEntry.length = 0;

            EEPROM.put(i * sizeof(FATEntry) + 1, emptyFatEntry);
        }
    }
}

int8_t FindEntryByFilename(const char *fileName)
{
    FATEntry fileEntry;

    for (int i = 0; i < noOfFiles; i++)
    {
        EEPROM.get(i * sizeof(FATEntry) + 1, fileEntry);

        if (strcmp(fileName, fileEntry.name) == 0)
        {
            return i;
        }
    }

    return -1;
}

uint16_t FindSpaceAddress(uint16_t size)
{
    FATEntry entries[*noOfFiles];

    int entryAddress = 1;

    for (int i = 0; i < noOfFiles; i++)
    {
        EEPROM.get(entryAddress, entries[i]);

        entryAddress += sizeof(FATEntry);
    }

    qsort(entries, *noOfFiles, sizeof(FATEntry), compareByFileAddress);

    uint16_t startAddress = 162;

    for(int i = 0; i < noOfFiles; i++) {
        uint16_t sizeGap = entries[i].position - startAddress;

        if(sizeGap >= size) {
            break;
        }

        startAddress += sizeGap + entries[i].length;
    }

    return startAddress;
}

uint16_t FileFreespace()
{
    FATEntry entries[*noOfFiles];

    int entryAddress = 1;

    for (int i = 0; i < noOfFiles; i++)
    {
        EEPROM.get(entryAddress, entries[i]);

        entryAddress += sizeof(FATEntry);
    }

    qsort(entries, *noOfFiles, sizeof(FATEntry), compareByFileAddress);

    uint16_t freespace = 0;
    uint16_t startAddress = 162;

    for(int i = 0; i < noOfFiles; i++) {
        uint16_t sizeGap = entries[i].position - startAddress;

        if(sizeGap > freespace) {
            freespace = sizeGap;
        }

        startAddress += sizeGap + entries[i].length;
    }

    if(EEPROM.length() - startAddress > freespace) {
        freespace = EEPROM.length() - startAddress;
    }

    return freespace;
}

void WriteFatEntry(const uint8_t index, const FATEntry *entry)
{
    int address = index * sizeof(FATEntry) + 1;

    EEPROM.put(address, *entry);

    noOfFiles++;
}

void RemoveFatEntry(const uint8_t index)
{
    int address = 1 + index * sizeof(FATEntry);

    FATEntry entry;

    entry.name[0] = '\0';
    entry.position = 0;
    entry.length = 0;

    EEPROM.put(address, entry);

    for (int i = index + 1; i < noOfFiles; i++)
    {
        EEPROM.get(address + sizeof(FATEntry), entry);
        EEPROM.put(address, entry);

        address += sizeof(FATEntry);
    }

    noOfFiles--;
}

FATEntry *ReadFatEntry(const uint8_t index)
{
    FATEntry *entry = new FATEntry;

    if (index >= noOfFiles)
    {
        delete entry;
        return nullptr;
    }

    EEPROM.get(1 + index * sizeof(FATEntry), *entry);

    return entry;
}

void WriteFatData(const uint16_t address, const uint8_t *data, const int size)
{
    for(int i = 0; i < size; i++) {
        EEPROM.put(address + i, data[i]);
    }
}

uint8_t *ReadFatData(const uint16_t address, const int size)
{
    uint8_t* data = new uint8_t[size];

    for(int i = 0; i < size; i++) {
        EEPROM.get(address + i, data[i]);
    }

    return data;
}
