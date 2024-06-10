#include "ram.h"

int memoryAddressComperator(const void *a, const void *b) {
  Memory *memA = (Memory *)a;
  Memory *memB = (Memory *)b;
  return (memA->address - memB->address);
}

void StoreVariable(uint8_t name, uint8_t id, Stack *stack)
{   
    Memory memorySlots[noOfVars];

    memcpy(memorySlots, memoryMapping, sizeof(Memory) * noOfVars);

    int index = 0;
    int currentMemorySlots = noOfVars;
    
    for (int i = 0; i < currentMemorySlots; i++)
    {
        auto tempMemorySlot = &(memoryMapping[i]);

        if (tempMemorySlot->id != id && tempMemorySlot->name == name)
        {
            memoryMapping[index] = memorySlots[i];

            index++;
            continue;
        }

        noOfVars--;
    }

    if(noOfVars == MEMORY_SLOTS) {
        Serial.println(F("Memory is full"));
        return;
    }

    qsort(memoryMapping, noOfVars, sizeof(Memory), memoryAddressComperator);

    
}

void RetrieveVariable(uint8_t name, uint8_t id, Stack *stack)
{
    Memory *memorySlot;

    for (int i = 0; i < noOfVars; i++)
    {
        auto tempMemorySlot = &(memoryMapping[i]);

        if (tempMemorySlot->name == name && tempMemorySlot->id)
        {
            memorySlot = tempMemorySlot;
            break;
        }
    }

    uint8_t *data = new uint8_t[memorySlot->size];

    for (int i = 0; i < memorySlot->size; i++)
    {
        data[i] = RAM[memoryMapping->address + i];
    }

    switch (memorySlot->type)
    {
    case 'c':
        PushChar(stack, data[0]);
        break;
    case 'i':
        PushInt(stack, word(data[0], data[1]));
        break;
    case 'f':
        float f;
        memcpy(&f, data, 4);
        PushFloat(stack, f);
        break;
    case 's':
        PushString(stack, (char*) data);
        break;
    default:
        Serial.println(F("Variable is not present"));
        break;
    }

    delete[] data;
}

void CleanProcessVariables(uint8_t id)
{
    int index = 0;
    int currentMemorySlots = noOfVars;
    Memory memorySlots[noOfVars];

    memcpy(memorySlots, memoryMapping, sizeof(Memory) * noOfVars);

    for (int i = 0; i < currentMemorySlots; i++)
    {
        if (memorySlots[i].id != id)
        {
            memoryMapping[index] = memorySlots[i];

            index++;
            continue;
        }

        noOfVars--;
    }
}