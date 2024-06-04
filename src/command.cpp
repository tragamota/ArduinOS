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
    Serial.println("Running Store command");
}

void Retrieve()
{
    char inputFilename[12];
    memset(inputFilename, 0, 12);

    Serial.readBytesUntil('\n', inputFilename, 11);

    Serial.println(inputFilename);

    auto fileIndex = FindEntryByFilename(inputFilename);

    if(fileIndex < 0) {
        Serial.println("File is not present on disk at the moment");
        return;
    }

    Serial.print("File found at index ");
    Serial.println(fileIndex);
}

void Erase()
{
    char inputFilename[12];

    Serial.readBytesUntil('\n', inputFilename, 11);

}

void Files()
{
    
}

void Freespace()
{
    
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
