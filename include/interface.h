#ifndef INTERFACE_H
#define INTERFACE_H

#define COMMAND_BUFF_SIZE 12

typedef struct {
 char name [ COMMAND_BUFF_SIZE ];
 void (* func )();
} commandType ;

void InitializeInterface();
void FlushStream();
bool ReadUserInterface(char*, int, int*);

#endif // INTERFACE_H