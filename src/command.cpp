#include "command.h"

#include "filesystem.h"
#include "process.h"

CommandType *FindCommandFunc(CommandType *commands, int commandSize, char *input)
{
    for (int i = 0; i < commandSize; i++)
    {
        CommandType *command = &(commands[i]);
        if (strcmp(command->name, input) == 0)
        {
            return command;
        }
    }

    return nullptr;
}

void Store()
{
    char inputFilename[12];
    uint8_t fileSize;
    uint8_t *fileData;

    memset(inputFilename, 0, 12);

    Serial.readBytesUntil('\n', inputFilename, 11);

    fileSize = Serial.parseInt();
    fileData = new uint8_t[fileSize];

    Serial.read();
    Serial.readBytes(fileData, fileSize);

    auto findIndex = FindEntryByFilename(inputFilename);

    Serial.println();

    if (fileSize <= 0)
    {
        Serial.println(F("ERR: Size must be > 0"));
        return;
    }

    if (findIndex != -1)
    {
        Serial.println(F("ERR: File already exist"));
        return;
    }

    if (FileFreespace() < fileSize)
    {
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

    if (fileIndex == -1)
    {
        Serial.println(F("File is not present on disk at the moment"));
        return;
    }

    auto fileFAT = ReadFatEntry(fileIndex);
    auto fileData = ReadFatData(fileFAT->position, fileFAT->length);

    Serial.print(F("File found at index "));
    Serial.println(fileIndex);

    for (int i = 0; i < fileFAT->length; i++)
    {
        Serial.print(F("0x"));
        Serial.print(fileData[i], HEX);
        Serial.print(F(" "));
    }

    delete[] fileData;
}

void Erase()
{
    char inputFilename[12];
    memset(inputFilename, 0, 12);

    Serial.readBytesUntil('\n', inputFilename, 11);

    auto fileFatIndex = FindEntryByFilename(inputFilename);

    Serial.println();

    if (fileFatIndex == -1)
    {
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

    if (noOfFiles == 0)
    {
        Serial.println(F("No Files on disk"));
    }

    for (int i = 0; i < noOfFiles; i++)
    {
        auto *fileEntry = ReadFatEntry(i);

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
    char inputFilename[12];
    memset(inputFilename, 0, 12);

    Serial.readBytesUntil('\n', inputFilename, 11);
    Serial.println();
    
    auto fileIndex = FindEntryByFilename(inputFilename);

    if(fileIndex == -1) {
        Serial.println(F("Program file doesn't exist"));
        return;
    }

    if(noOfProcesses >= 5) {
        Serial.println(F("Max number of programs reached"));
        return;
    }

    auto fileEntry = ReadFatEntry(fileIndex);

    Process startProcess;

    strcpy(startProcess.name, inputFilename);
    startProcess.pc = fileEntry->position;

    AddProcess(&startProcess);

    Serial.print(F("Started program \""));
    Serial.print(inputFilename);
    Serial.print(F("\" with id: "));
    Serial.println(startProcess.id);
}

void List()
{
    Serial.println();

    if (!HasRunningProcesses())
    {
        Serial.println(F("No processes are running at the moment"));
        return;
    }

    PrintProcesses();
}

void Suspend()
{
    uint8_t id = Serial.parseInt();

    Process *proc = GetProcess(id);

    Serial.println();

    if (!proc)
    {
        Serial.print(F("No process found with ID: "));
        Serial.println(id);
        return;
    }

    if (proc->state == 2)
    {
        Serial.print(F("Process is already suspended"));
        return;
    }

    PauseProcess(proc);
}

void Resume()
{
    uint8_t id = Serial.parseInt();

    Process *proc = GetProcess(id);

    Serial.println();

    if (proc == nullptr)
    {
        Serial.print(F("No process found with ID: "));
        Serial.println(id);
        return;
    }

    if (proc->state == 2)
    {
        Serial.print(F("Process is already running"));
        return;
    }
}

void Kill()
{
    uint8_t id = Serial.parseInt();

    Process *proc = GetProcess(id);

    Serial.println();

    if (proc == nullptr)
    {
        Serial.print(F("No process found with ID: "));
        Serial.println(id);
        return;
    }

    KillProcess(proc);
}
