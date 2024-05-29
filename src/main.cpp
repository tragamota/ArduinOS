#include <Arduino.h>

#include "command.h"
#include "interface.h"
#include "filesystem.h"

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

static int commandBufferSize = sizeof(commands) / sizeof(CommandType);

void setup() {
  InitializeInterface();
  InitializeFilesystem();
}

void loop() {
  if(ReadUserInterface(commandInputBuffer, COMMAND_BUFF_SIZE, &position)) {
    // Look for the right command if it is available
    auto command = FindCommandFunc(commands, commandBufferSize, commandInputBuffer);

    if(command != nullptr) {
      command->func();
    }
s
    FlushStream();
    Serial.print("\nArduinOS> ");
  }
}

