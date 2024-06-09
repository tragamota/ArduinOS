#include "command.h"

#include "filesystem.h"

CommandType* FindCommandFunc(CommandType* commands, int commandSize, char *input)
{
    for(int i = 0; i < commandSize; i++) {
        CommandType* command = &(commands[i]);
        if(strcmp(command->name, input) == 0) {
            return command;
        }
    }

    return nullptr;
}

void Store()
{
    char inputFilename[12];
    int fileSize;
    uint8_t* fileData;

    memset(inputFilename, 0, 12);

    Serial.readBytesUntil('\n', inputFilename, 11);

    fileSize = Serial.parseInt();
    fileData = new uint8_t[fileSize];

    Serial.read();
    Serial.readBytes(fileData, fileSize);

    auto findIndex = FindEntryByFilename(inputFilename);

    Serial.println();

    if(fileSize <= 0) {
        Serial.println(F("ERR: Size must be > 0"));
        return;
    }

    if(findIndex != -1) {
        Serial.println(F("ERR: File already exist"));
        return;
    }

    if(FileFreespace() < fileSize) {
        Serial.println(F("ERR: No space left"));
        return;
    }

    auto newFileStartAddress = FindSpaceAddress(fileSize);

    FATEntry newFileEntry;

    strcpy(newFileEntry.name, inputFilename);
    newFileEntry.length = fileSize;
    newFileEntry.position = newFileStartAddress;

    WriteFatEntry(noOfFiles, &newFileEntry);
    WriteFatData(newFileStartAddress, fileData, fileSize);

    Serial.print(F("File is saved"));
}   

void Retrieve()
{
    char inputFilename[12];
    memset(inputFilename, 0, 12);

    Serial.readBytesUntil('\n', inputFilename, 11);

    auto fileIndex = FindEntryByFilename(inputFilename);

    Serial.println();

    if(fileIndex == -1) {
        Serial.println(F("File is not present on disk at the moment"));
        return;
    }

    auto fileFAT = ReadFatEntry(fileIndex);
    auto fileData = ReadFatData(fileFAT->position, fileFAT->length);

    Serial.print(F("File found at index "));
    Serial.println(fileIndex);

    for(int i = 0; i < fileFAT->length; i++) {
        Serial.print(F("0x"));
        Serial.print(fileData[i], HEX);
        Serial.print(F(" "));
    }

    delete [] fileData;
}

void Erase()
{
    char inputFilename[12];
    memset(inputFilename, 0, 12);

    Serial.readBytesUntil('\n', inputFilename, 11);

    auto fileFatIndex = FindEntryByFilename(inputFilename);

    Serial.println();

    if(fileFatIndex == -1) {
        Serial.println(F("File doesn't exist"));
        return;
    }

    RemoveFatEntry(fileFatIndex);

    Serial.print(F("File entry is removed at index: "));
    Serial.print(fileFatIndex);
}

void Files()
{
    Serial.println();

    if(noOfFiles == 0) {
        Serial.println(F("No Files on disk"));
    }

    for(int i = 0; i < noOfFiles; i++) {
        auto* fileEntry = ReadFatEntry(i);

        Serial.print(F("File "));
        Serial.print(i);
        Serial.println(F(":"));
        Serial.print(F("Filename: "));
        Serial.println(fileEntry->name);
        Serial.print(F("File size: "));
        Serial.print(fileEntry->length);
        Serial.println(F(" Byte(s)"));

        delete fileEntry;
    }
}

void Freespace()
{
    Serial.println();
    Serial.print(F("Freespace: "));
    Serial.print(FileFreespace());
    Serial.print(F(" bytes."));
}

void Run()
{

}

void List()
{

}

void Suspend()
{

}

void Resume()
{

}

void Kill()
{

}
