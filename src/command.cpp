#include "command.h"

CommandType* FindCommandFunc(CommandType* commands, int commandSize, char *input)
{
    for(int i = 0; i < commandSize; i++) {
        CommandType* command = &(commands[i]);
        if(strcmp(input, command->name)) {
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

}

void Erase()
{

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
