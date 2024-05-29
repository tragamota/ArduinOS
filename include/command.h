#ifndef COMMANDS_H
#define COMMANDS_H

#include <HardwareSerial.h>
#include <USBAPI.h>

#define COMMAND_BUFF_SIZE 12

typedef struct {
 char name [ COMMAND_BUFF_SIZE ];
 void (* func )();
} CommandType ;

CommandType* FindCommandFunc(CommandType* commands, int commandSize, char* input);

void Store();
void Retrieve();
void Erase();
void Files();
void Freespace();
void Run();
void List();
void Suspend();
void Resume();
void Kill();

#endif