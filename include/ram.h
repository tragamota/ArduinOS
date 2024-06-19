#ifndef RAM_H
#define RAM_H

#include "stack.h"

#define RAM_SIZE 200
#define MEMORY_SLOTS 20

typedef struct {
  char name = 0;
  char type = 0;
  uint8_t id = 0;
  uint8_t address = 0;
  uint8_t size = 0;
} Memory;

static uint8_t RAM[RAM_SIZE];
static Memory memoryMapping[MEMORY_SLOTS];

void StoreVariable(uint8_t, uint8_t, Stack*);
void RetrieveVariable(uint8_t, uint8_t, Stack*);

void CleanProcessVariables(uint8_t);

#endif