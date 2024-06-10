#ifndef PROCESS_H
#define PROCESS_H

#include <Arduino.h>

#include "stack.h"

#define PROCESS_NAME_LENGHT 12
#define PROCESS_SLOTS 5

typedef struct Process
{
    char name[PROCESS_NAME_LENGHT];
    uint8_t id;
    uint8_t state = 0;
    uint16_t pc = 0;
    uint16_t fp = 0;
    uint16_t lb = 0;
    Stack stack;
} Process;

static Process processes[PROCESS_SLOTS];
static uint8_t noOfProcesses = 0;
static uint8_t processCounter = 0;

Process *GetProcess(uint8_t processId);

void AddProcess(Process* process);
void ResumeProcess(Process*);
void PauseProcess(Process*);
void KillProcess(Process*);

bool HasRunningProcesses();
void PrintProcesses();

#endif // PROCESS_H