#include <Arduino.h>

#include "command.h"
#include "interface.h"
#include "filesystem.h"
#include "process.h"

#include "ram.h"

static char commandInputBuffer[COMMAND_BUFF_SIZE]; 
static int position = 0;

static CommandType commands[] = {
    {"store", &Store},
    {"retrieve", &Retrieve},
    {"files", &Files},
    {"freespace", &Freespace},
    {"erase", &Erase},
    {"run", &Run},
    {"list", &List},
    {"suspend", &Suspend},
    {"resume", &Resume},
    {"kill", &Kill}
};

static int totalCommands = sizeof(commands) / sizeof(CommandType);

void setup() {
  InitializeInterface();
  InitializeFilesystem();
}

void loop() {
  if(ReadUserInterface(commandInputBuffer, COMMAND_BUFF_SIZE, &position)) {
    auto command = FindCommandFunc(commands, totalCommands, commandInputBuffer);

    if(command != nullptr) {
      command->func();
    }

    FlushStream();
    Serial.print(F("\nArduinOS> "));
  }

  RunProcesses();
}

