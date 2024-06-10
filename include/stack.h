#ifndef STACK_H
#define STACK_H

#include <Arduino.h>

#define STACK_SIZE 30

typedef struct Stack {
    uint8_t bytes[STACK_SIZE];
    uint8_t sp = 0;
} Stack;

uint8_t PeekType(Stack*);

void PushChar(Stack*, char value);
void PushInt(Stack*, int value);
void PushFloat(Stack*, float value);
void PushString(Stack*, char* value);

char PopChar(Stack*);
int PopInt(Stack*);
float PopFloat(Stack*);
char* PopString(Stack*);

#endif //STACK_H