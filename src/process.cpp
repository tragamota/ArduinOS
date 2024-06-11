#include "process.h"

#include "ram.h"
#include <instructions.h>

Process *GetProcess(uint8_t processId)
{
    for (int i = 0; i < PROCESS_SLOTS; i++)
    {
        Process *proc = &(processes[i]);
        if (proc->id == processId && proc->state != 0)
        {
            return proc;
        }
    }

    return nullptr;
}

void AddProcess(Process *process)
{
    Serial.println("Adding process");

    process->state = 1;
    process->id = processCounter;

    int slotIndex = 0;

    for (int i = 0; i < PROCESS_SLOTS; i++)
    {
        Process *process = &(processes[i]);

        if (process[i].state == 0)
        {
            slotIndex = i;
            break;
        }
    }

    processes[slotIndex] = *process;

    noOfProcesses++;
    processCounter++;

    Serial.println(noOfProcesses);
}

void PauseProcess(Process *process)
{
    process->state = 2;

    Serial.print(F("Process state -> suspended with ID: "));
    Serial.println(process->id);
}

void ResumeProcess(Process *process)
{
    process->state = 1;

    Serial.print(F("Process state -> running with ID: "));
    Serial.println(process->id);
}

void KillProcess(Process *process)
{
    process->state = 0;

    CleanProcessVariables(process->id);

    noOfProcesses--;

    Serial.print(F("Process state -> terminated with ID: "));
    Serial.println(process->id);
}

bool HasRunningProcesses()
{
    return noOfProcesses != 0;
}

void PrintProcesses()
{
    for (int i = 0; i < PROCESS_SLOTS; i++)
    {
        Process *process = &(processes[i]);

        if (process->state == 0)
            continue;

        Serial.print(F("Process ID: "));
        Serial.println(process->id);
        Serial.print(F("Process name: "));
        Serial.println(process->name);
        Serial.print(F("Process state: "));
        Serial.println(process->state);
        Serial.println(process->state == 1 ? "RUNNING" : "SUSPENDED");
    }
}

void runProcesses()
{
    for (int i = 0; i < PROCESS_SLOTS; i++)
    {
        Process *process = &(processes[i]);

        if (process->state != 1)
        {
            continue;
        }

        Execute(process);
    }
}
