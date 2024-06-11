#include "ram.h"

int memoryAddressComperator(const void *a, const void *b)
{
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

    if (noOfVars == MEMORY_SLOTS)
    {
        Serial.println(F("No empty memory slots"));
        return;
    }

    uint8_t type = PeekType(stack);
    uint8_t size = 0;
    uint8_t *data;

    switch (type)
    {
    case 'c':
        auto stackChar = PopChar(stack);
        size = sizeof(stackChar);

        data = new uint8_t[size];
        data[0] = stackChar;

        break;
    case 'i':
        auto stackInteger = PopInt(stack);
        size = sizeof(stackInteger);
        data = new uint8_t[size];

        data[0] = highByte(stackInteger);
        data[1] = lowByte(stackInteger);

        break;
    case 'f':
        auto stackFloat = PopFloat(stack);
        size = sizeof(stackFloat);
        data = new uint8_t[size];

        memcpy(data, &stackFloat, sizeof(stackFloat));

        break;
    case 's':
        auto stackString = PopString(stack);
        size = strlen(stackString) + 1;
        data = new uint8_t[size];

        for (int i = 0; i < size; i++)
        {
            data[i] = stackString[i];
        }
        break;
    }

    qsort(memoryMapping, noOfVars, sizeof(Memory), memoryAddressComperator);

    uint8_t address = 0;

    for (int i = 0; i < noOfVars; i++)
    {
        uint8_t distance = memoryMapping[i].address - address;

        if (distance >= size)
        {
            break;
        }

        address = memoryMapping[i].address + memoryMapping[i].size;
    }

    if (!((RAM_SIZE - address) >= size))
    {
        Serial.println(F("Allocating RAM unsucessfull"));
        return;
    }

    Memory newMemorySlot;

    newMemorySlot.name = name;
    newMemorySlot.id = id;
    newMemorySlot.type = type;
    newMemorySlot.size = size;
    newMemorySlot.address = address;

    memoryMapping[noOfVars] = newMemorySlot;

    memcpy(&(memoryMapping[address]), data, size);
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
        PushString(stack, (char *)data);
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