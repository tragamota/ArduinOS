#include "stack.h"

void PushByte(Stack *stack, uint8_t byte)
{
    stack->bytes[stack->sp++] = byte;
}

uint8_t PopByte(Stack *stack)
{
    return stack->bytes[--stack->sp];
}

uint8_t PeekType(Stack* stack) {
    uint8_t type = PopByte(stack);

    PushByte(stack, type);

    return type;
}

void PushString(Stack* stack, char* value) 
{
    int stringLength = strlen(value);

    for(int i = 0; i < stringLength + 1; i++) {
        PushByte(stack, value[i]);
    }

    PushByte(stack, stringLength + 1);
    PushByte(stack, 's');
}

void PushChar(Stack *stack, char value)
{
    PushByte(stack, value);
    PushByte(stack, 'c');
}

void PushInt(Stack *stack, int value)
{
    PushByte(stack, lowByte(value));
    PushByte(stack, highByte(value));
    PushByte(stack, 'i');
}

void PushFloat(Stack *stack, float value)
{
    byte b[4];
    
    memcpy(b, &value, sizeof(float));
    
    for(int i = sizeof(float) - 1; i >= 0; i--) {
        PushByte(stack, b[i]);
    }

    PushByte(stack, 'f');
}

char PopChar(Stack* stack)
{
    PopByte(stack);
    return PopByte(stack);
}

int PopInt(Stack* stack)
{
    PopByte(stack);
    
    return word(PopByte(stack), PopByte(stack));
}

float PopFloat(Stack* stack)
{
    float value;
    byte b[4];

    PopByte(stack);
    for(int i = 0; i < sizeof(float); i++) {
        b[i] = PopByte(stack);
    }

    memcpy(&value, b, sizeof(float));

    return value;
}
