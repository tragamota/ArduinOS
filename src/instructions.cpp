#include "instructions.h"

#include <EEPROM.h>

#include "ram.h"
#include "filesystem.h"

static Instruction instructionList[] = {
    {0x01, &CHAR},
    {0x02, &INT},
    {0x03, &FLOAT},
    {0x04, &STRING},
    {0x05, &SET},
    {0x06, &GET},
    {0x07, &INCREMENT},
    {0x08, &DECREMENT},
    {0x09, &PLUS},
    {0x0A, &MINUS},
    {0x0B, &TIMES},
    {0x0C, &DIVIDE},
    {0x0D, &MODULO},
    {0x0E, &UNARYMINUS},
    {0x0F, &EQUALS},
    {0x10, &NOTEQUALS},
    {0x11, &LESSTHEN},
    {0x12, &LESSTHENOREQUALS},
    {0x13, &GREATERTHEN},
    {0x14, &GREATERTHENOREQUALS},
    {0x15, &LOGICALAND},
    {0x16, &LOGICALOR},
    {0x17, &LOGICALXOR},
    {0x18, &LOGICALNOT},
    {0x19, &BITWISEAND},
    {0x1A, &BITWISEOR},
    {0x1B, &BITWISEXOR},
    {0x1C, &BITWISENOT},
    {0x1D, &TOCHAR},
    {0x1E, &TOINT},
    {0x1F, &TOFLOAT},
    {0x20, &ROUND},
    {0x21, &FLOOR},
    {0x22, &CEILING},
    {0x23, &MIN},
    {0x24, &MAX},
    {0x25, &ABS},
    {0x26, &CONSTRAIN},
    {0x27, &MAP},
    {0x28, &POW},
    {0x29, &SQ},
    {0x2A, &SQRT},
    {0x2B, &DELAY},
    {0x2C, &DELAYUNTIL},
    {0x2D, &MILLIS},
    {0x2E, &PINMODE},
    {0x2F, &ANALOGREAD},
    {0x30, &ANALOGWRITE},
    {0x31, &DIGITALREAD},
    {0x32, &DIGITALWRITE},
    {0x33, &PRINT},
    {0x34, &PRINTLN},
    {0x35, &OPEN},
    {0x36, &CLOSE},
    {0x37, &WRITE},
    {0x38, &READINT},
    {0x39, &READFLOAT},
    {0x3A, &READSTRING},
    {0x3B, &READCHAR},
    {0x3C, &IF},
    {0x3D, &ELSE},
    {0x3E, &ENDIF},
    {0x3F, &WHILE},
    {0x40, &ENDWHILE},
    {0x41, &LOOP},
    {0x42, &ENDLOOP},
    {0x43, &STOP},
    {0x44, &FORK},
    {0x45, &WAITUNTILDONE}};

Instruction *FindInstruction(uint8_t functionCode)
{
  for (int i = 0; i < sizeof(instructionList) / sizeof(Instruction); i++)
  {
    auto instruct = &(instructionList[i]);
    if (instruct->name == functionCode)
    {
      return instruct;
    }
  }

  return nullptr;
}

void Execute(Process *process)
{
  auto instructionCode = EEPROM[process->pc++];
  auto instruction = FindInstruction(instructionCode);

  if (instruction != nullptr)
  {
    instruction->function(process);
    return;
  }

  KillProcess(process);
}

void STOP(Process *process)
{
  KillProcess(process);
}

void CHAR(Process *proc)
{
  char data = EEPROM.read(proc->pc++);
  PushChar(&(proc->stack), data);
}

void INT(Process *proc)
{
  uint8_t highByte = EEPROM.read(proc->pc++);
  uint8_t lowByte = EEPROM.read(proc->pc++);

  PushInt(&(proc->stack), word(highByte, lowByte));
}

void FLOAT(Process *proc)
{
  uint8_t floatBytes[4];

  floatBytes[0] = EEPROM.read(proc->pc++);
  floatBytes[1] = EEPROM.read(proc->pc++);
  floatBytes[2] = EEPROM.read(proc->pc++);
  floatBytes[3] = EEPROM.read(proc->pc++);

  float data;

  memcpy(&data, floatBytes, 4);

  PushFloat(&(proc->stack), data);
}

void STRING(Process *proc)
{
  uint8_t *stringData;
  int stringLength = 0;

  while (EEPROM.read(proc->pc + stringLength) != 0)
  {
    stringLength++;
  }

  stringData = new uint8_t[stringLength + 1];

  for (int i = 0; i < stringLength + 1; i++)
  {
    stringData[i] = EEPROM.read(proc->pc++);
  }

  PushString(&(proc->stack), (char *)stringData);

  delete[] stringData;
}

void SET(Process *proc)
{
  uint8_t name = EEPROM.read(proc->pc++);
  StoreVariable(name, proc->id, &(proc->stack));
}

void GET(Process *proc)
{
  uint8_t name = EEPROM.read(proc->pc++);
  RetrieveVariable(name, proc->id, &(proc->stack));
}

void INCREMENT(Process *proc)
{
  char type = PeekType(&(proc->stack));
  if (type == 'c')
  {
    char value = PopChar(&(proc->stack));
    PushChar(&(proc->stack), ++value);
  }
  else if (type == 'i')
  {
    int value = PopInt(&(proc->stack));
    PushInt(&(proc->stack), ++value);
  }
  else if (type == 'f')
  {
    short value = PopFloat(&(proc->stack));
    PushFloat(&(proc->stack), ++value);
  }
  else
  {
    STOP(proc);
  }
}

void DECREMENT(Process *proc)
{
  char type = PeekType(&(proc->stack));
  if (type == 'c')
  {
    char value = PopChar(&(proc->stack));
    PushChar(&(proc->stack), --value);
  }
  else if (type == 'i')
  {
    int value = PopInt(&(proc->stack));
    PushInt(&(proc->stack), --value);
  }
  else if (type == 'f')
  {
    short value = PopFloat(&(proc->stack));
    PushFloat(&(proc->stack), --value);
  }
  else
  {
    STOP(proc);
  }
}

void PLUS(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      char data3 = data1 + data2;
      PushChar(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      int data3 = data1 + data2;
      PushInt(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      float data3 = data1 + data2;
      PushFloat(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 's')
  {
    char *data1 = PopString(&(proc->stack));

    type2 = PeekType(&(proc->stack));

    if (type2 == 's')
    {
      char *data2 = PopString(&(proc->stack));
      char *data3 = new char[strlen(data1) + strlen(data2) + 1];

      strcpy(data3, data1);
      strcat(data3, data2);

      PushString(&(proc->stack), data3);

      delete[] data2;
      delete[] data3;
    }
    else
    {
      STOP(proc);
    }

    delete[] data1;
  }
}

void MINUS(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      char data3 = data1 - data2;
      PushChar(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      int data3 = data1 - data2;
      PushInt(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      float data3 = data1 - data2;
      PushFloat(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void TIMES(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      char data3 = data1 * data2;
      PushChar(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      int data3 = data1 * data2;
      PushInt(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      float data3 = data1 * data2;
      PushFloat(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {
    STOP(proc);
  }
}

void DIVIDE(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      char data3 = data1 / data2;
      PushChar(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      int data3 = data1 / data2;
      PushInt(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      float data3 = data1 / data2;
      PushFloat(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {
    STOP(proc);
  }
}

void MODULO(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      char data3 = data1 % data2;
      PushChar(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      int data3 = data1 % data2;
      PushInt(&(proc->stack), data3);
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {
    STOP(proc);
  }
}

void UNARYMINUS(Process *proc)
{
  char type = PeekType(&(proc->stack));
  if (type == 'c')
  {
    char data = PopChar(&(proc->stack));
    data = -data;
    PushChar(&(proc->stack), data);
  }
  else if (type == 'i')
  {
    int data = PopInt(&(proc->stack));
    data = -data;
    PushInt(&(proc->stack), data);
  }
  else if (type == 'f')
  {
    float data = PopFloat(&(proc->stack));
    data = -data;
    PushFloat(&(proc->stack), data);
  }
  else
  {
    STOP(proc);
  }
}

void EQUALS(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 == data2);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 == data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 == data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 's')
  {
    char *data1 = PopString(&(proc->stack));
    type2 = PeekType(&(proc->stack));

    if (type2 == 's')
    {
      char *data2 = PopString(&(proc->stack));
      PushChar(&(proc->stack), strcmp(data1, data2) == 0);

      delete[] data2;
    }
    else
    {

      STOP(proc);
    }

    delete[] data1;
  }
}

void NOTEQUALS(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 != data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 != data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 != data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 's')
  {
    char *data1 = PopString(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 's')
    {
      char *data2 = PopString(&(proc->stack));
      PushChar(&(proc->stack), strcmp(data1, data2) != 0);

      delete[] data2;
    }
    else
    {

      STOP(proc);
    }

    delete[] data1;
  }
}

void LESSTHEN(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 < data2);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 < data2);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 < data2);
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void LESSTHENOREQUALS(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 <= data2);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 <= data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 <= data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void GREATERTHEN(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 > data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 > data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 > data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void GREATERTHENOREQUALS(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 >= data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 >= data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 >= data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void LOGICALAND(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 && data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 && data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 && data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void LOGICALOR(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 || data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 || data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), data1 || data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void LOGICALXOR(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), (data1 && !data2) || (!data1 && data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), (data1 && !data2) || (!data1 && data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushChar(&(proc->stack), (data1 && !data2) || (!data1 && data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void LOGICALNOT(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  if (type1 == 'c')
  {
    char data = PopChar(&(proc->stack));
    PushChar(&(proc->stack), !data);
  }
  else if (type1 == 'i')
  {
    int data = PopInt(&(proc->stack));
    PushChar(&(proc->stack), !data);
  }
  else if (type1 == 'f')
  {
    float data = PopFloat(&(proc->stack));
    PushChar(&(proc->stack), !data);
  }
  else
  {

    STOP(proc);
  }
}

void BITWISEAND(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 & data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 & data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void BITWISEOR(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 | data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 | data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void BITWISEXOR(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), data1 ^ data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushChar(&(proc->stack), data1 ^ data2);
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void BITWISENOT(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    PushChar(&(proc->stack), ~data1);
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    PushInt(&(proc->stack), ~data1);
  }
  else
  {

    STOP(proc);
  }
}

void TOCHAR(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    PushChar(&(proc->stack), (char)data1);
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    PushChar(&(proc->stack), (char)data1);
  }
  else
  {

    STOP(proc);
  }
}

void TOINT(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    PushInt(&(proc->stack), (int)data1);
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    PushInt(&(proc->stack), (int)data1);
  }
  else
  {

    STOP(proc);
  }
}

void TOFLOAT(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    PushFloat(&(proc->stack), (float)data1);
  }
  else if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    PushFloat(&(proc->stack), (float)data1);
  }
  else
  {

    STOP(proc);
  }
}

void ROUND(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  int roundedValue;

  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    roundedValue = round((int)data1);
    PushInt(&(proc->stack), roundedValue);
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    PushInt(&(proc->stack), data1);
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    roundedValue = round(data1);
    PushInt(&(proc->stack), roundedValue);
  }
  else
  {

    STOP(proc);
  }
}

void FLOOR(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  int flooredValue;

  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    flooredValue = floor((int)data1);
    PushInt(&(proc->stack), flooredValue);
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    PushInt(&(proc->stack), data1);
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    flooredValue = floor(data1);
    PushInt(&(proc->stack), flooredValue);
  }
  else
  {

    STOP(proc);
  }
}

void CEILING(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  int ceilingValue;

  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    ceilingValue = ceil((int)data1);
    PushInt(&(proc->stack), ceilingValue);
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    PushInt(&(proc->stack), data1);
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    ceilingValue = ceil(data1);
    PushInt(&(proc->stack), ceilingValue);
  }
  else
  {

    STOP(proc);
  }
}

void MIN(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;

  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), min(data1, data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushInt(&(proc->stack), min(data1, data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushFloat(&(proc->stack), min(data1, data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void MAX(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;

  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char data2 = PopChar(&(proc->stack));
      PushChar(&(proc->stack), max(data1, data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int data2 = PopInt(&(proc->stack));
      PushInt(&(proc->stack), max(data1, data2));
    }
    else
    {

      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      float data2 = PopFloat(&(proc->stack));
      PushFloat(&(proc->stack), max(data1, data2));
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {

    STOP(proc);
  }
}

void ABS(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'c')
  {
    char data1 = PopChar(&(proc->stack));
    PushChar(&(proc->stack), abs(data1));
  }
  else if (type1 == 'i')
  {
    int data1 = PopInt(&(proc->stack));
    PushInt(&(proc->stack), abs(data1));
  }
  else if (type1 == 'f')
  {
    float data1 = PopFloat(&(proc->stack));
    PushFloat(&(proc->stack), fabs(data1));
  }
  else
  {
    STOP(proc);
  }
}

void CONSTRAIN(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'c')
  {
    char val = PopChar(&(proc->stack));
    char minVal = PopChar(&(proc->stack));
    char maxVal = PopChar(&(proc->stack));
    char constrainedVal = (val < minVal) ? minVal : (val > maxVal) ? maxVal
                                                                   : val;
    PushChar(&(proc->stack), constrainedVal);
  }
  else if (type1 == 'i')
  {
    int val = PopInt(&(proc->stack));
    int minVal = PopInt(&(proc->stack));
    int maxVal = PopInt(&(proc->stack));
    int constrainedVal = (val < minVal) ? minVal : (val > maxVal) ? maxVal
                                                                  : val;
    PushInt(&(proc->stack), constrainedVal);
  }
  else if (type1 == 'f')
  {
    float val = PopFloat(&(proc->stack));
    float minVal = PopFloat(&(proc->stack));
    float maxVal = PopFloat(&(proc->stack));
    float constrainedVal = (val < minVal) ? minVal : (val > maxVal) ? maxVal
                                                                    : val;
    PushFloat(&(proc->stack), constrainedVal);
  }
  else
  {
    STOP(proc);
  }
}

void MAP(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'c')
  {
    char val = PopChar(&(proc->stack));
    char inMin = PopChar(&(proc->stack));
    char inMax = PopChar(&(proc->stack));
    char outMin = PopChar(&(proc->stack));
    char outMax = PopChar(&(proc->stack));
    char mappedVal = map(val, inMin, inMax, outMin, outMax);
    PushChar(&(proc->stack), mappedVal);
  }
  else if (type1 == 'i')
  {
    int val = PopInt(&(proc->stack));
    int inMin = PopInt(&(proc->stack));
    int inMax = PopInt(&(proc->stack));
    int outMin = PopInt(&(proc->stack));
    int outMax = PopInt(&(proc->stack));
    int mappedVal = map(val, inMin, inMax, outMin, outMax);
    PushInt(&(proc->stack), mappedVal);
  }
  else if (type1 == 'f')
  {
    float val = PopFloat(&(proc->stack));
    float inMin = PopFloat(&(proc->stack));
    float inMax = PopFloat(&(proc->stack));
    float outMin = PopFloat(&(proc->stack));
    float outMax = PopFloat(&(proc->stack));
    float mappedVal = map(val, inMin, inMax, outMin, outMax);
    PushFloat(&(proc->stack), mappedVal);
  }
  else
  {

    STOP(proc);
  }
}

void POW(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'i')
  {
    int exponent = PopInt(&(proc->stack));
    int base = PopInt(&(proc->stack));
    int result = pow(base, exponent);
    PushInt(&(proc->stack), result);
  }
  else if (type1 == 'c')
  {
    char exponent = PopChar(&(proc->stack));
    char base = PopChar(&(proc->stack));
    char result = pow(base, exponent);
    PushChar(&(proc->stack), result);
  }
  else if (type1 == 'f')
  {
    float exponent = PopFloat(&(proc->stack));
    float base = PopFloat(&(proc->stack));
    float result = pow(base, exponent);
    PushFloat(&(proc->stack), result);
  }
  else
  {

    STOP(proc);
  }
}

void SQ(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  if (type1 == 'i')
  {
    int val = PopInt(&(proc->stack));
    int result = val * val;
    PushInt(&(proc->stack), result);
  }
  else if (type1 == 'c')
  {
    char val = PopChar(&(proc->stack));
    char result = val * val;
    PushChar(&(proc->stack), result);
  }
  else if (type1 == 'f')
  {
    float val = PopFloat(&(proc->stack));
    float result = val * val;
    PushFloat(&(proc->stack), result);
  }
  else
  {
    STOP(proc);
  }
}

void SQRT(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 == 'i')
  {
    int val = PopInt(&(proc->stack));

    if (val < 0)
    {

      STOP(proc);
      return;
    }

    int result = sqrt(val);
    PushInt(&(proc->stack), result);
  }
  else if (type1 == 'c')
  {
    char val = PopChar(&(proc->stack));

    if (val < 0)
    {

      STOP(proc);
      return;
    }

    char result = sqrt(val);
    PushChar(&(proc->stack), result);
  }
  else if (type1 == 'f')
  {
    float val = PopFloat(&(proc->stack));

    if (val < 0)
    {

      STOP(proc);
      return;
    }

    float result = sqrt(val);
    PushFloat(&(proc->stack), result);
  }
  else
  {

    STOP(proc);
  }
}

void DELAY(Process *proc)
{
  char type = PeekType(&(proc->stack));
  if (type == 'i')
  {
    int num = PopInt(&(proc->stack));
    delay(num);
  }
  else if (type == 'c')
  {
    char num = PopChar(&(proc->stack));
    delay(num);
  }
  else if (type == 'f')
  {
    float num = PopFloat(&(proc->stack));
    delay((int)num);
  }
  else
  {
    STOP(proc);
  }
}

void DELAYUNTIL(Process *proc)
{
  char type = PeekType(&(proc->stack));

  if (type == 'i')
  {
    int targetTime = PopInt(&(proc->stack));
    while (millis() < targetTime)
    {
      // Wait until the target time is reached
    }
    PushInt(&(proc->stack), targetTime);
  }
  else if (type == 'f')
  {
    float targetTime = PopFloat(&(proc->stack));
    int targetTimeInt = (int)targetTime;
    while (millis() < targetTimeInt)
    {
      // Wait until the target time is reached
    }
    PushFloat(&(proc->stack), targetTime);
  }
  else if (type == 'c')
  {
    char targetTime = PopChar(&(proc->stack));
    int targetTimeInt = targetTime;
    while (millis() < targetTimeInt)
    {
      // Wait until the target time is reached
    }
    PushChar(&(proc->stack), targetTime);
  }
  else
  {

    STOP(proc);
  }
}

void MILLIS(Process *proc)
{
  unsigned long currentTime = millis();
  PushInt(&(proc->stack), currentTime);
}

void PINMODE(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;

  if (type1 == 'c')
  {
    char pin = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      char mode = PopChar(&(proc->stack));
      pinMode(pin, mode);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int pin = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int mode = PopInt(&(proc->stack));
      pinMode(pin, mode);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    int pin = (int)PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      int mode = (int)PopFloat(&(proc->stack));
      pinMode(pin, mode);
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {
    STOP(proc);
  }
}

void ANALOGREAD(Process *proc)
{
  char type = PeekType(&(proc->stack));

  if (type == 'i')
  {
    int pin = PopInt(&(proc->stack));
    int value = analogRead(pin);
    PushInt(&(proc->stack), value);
  }
  else if (type == 'c')
  {
    char pin = PopChar(&(proc->stack));
    int value = analogRead(pin);
    PushInt(&(proc->stack), value);
  }
  else if (type == 'f')
  {
    int pin = (int)PopFloat(&(proc->stack));
    int value = analogRead(pin);
    PushInt(&(proc->stack), value);
  }
  else
  {
    STOP(proc);
  }
}

void ANALOGWRITE(Process *proc)
{
  char type1 = PeekType(&(proc->stack));
  char type2;

  if (type1 == 'c')
  {
    char pin = PopChar(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'c')
    {
      int value = PopInt(&(proc->stack));
      analogWrite(pin, value);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'i')
  {
    int pin = PopInt(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'i')
    {
      int value = PopInt(&(proc->stack));
      analogWrite(pin, value);
    }
    else
    {
      STOP(proc);
    }
  }
  else if (type1 == 'f')
  {
    int pin = (int)PopFloat(&(proc->stack));
    type2 = PeekType(&(proc->stack));
    if (type2 == 'f')
    {
      int value = PopInt(&(proc->stack));
      analogWrite(pin, value);
    }
    else
    {
      STOP(proc);
    }
  }
  else
  {
    STOP(proc);
  }
}

void DIGITALREAD(Process *proc)
{
  char type = PeekType(&(proc->stack));

  if (type == 'i')
  {
    int pin = PopInt(&(proc->stack));
    char value = digitalRead(pin);
    PushChar(&(proc->stack), value);
  }
  else if (type == 'c')
  {
    char pin = PopChar(&(proc->stack));
    char value = digitalRead(pin);
    PushChar(&(proc->stack), value);
  }
  else if (type == 'f')
  {
    int pin = (int)PopFloat(&(proc->stack));
    char value = digitalRead(pin);
    PushChar(&(proc->stack), value);
  }
  else
  {
    STOP(proc);
  }
}

void DIGITALWRITE(Process *proc)
{
  char type1 = PeekType(&(proc->stack));

  if (type1 != 'c' && type1 != 'i' && type1 != 'f')
  {
    STOP(proc);
    return;
  }

  int pin = (type1 == 'c') ? PopChar(&(proc->stack)) : (type1 == 'i') ? PopInt(&(proc->stack))
                                                                      : (int)PopFloat(&(proc->stack));

  char type2 = PeekType(&(proc->stack));
  if (type2 != type1)
  {
    STOP(proc);
    return;
  }

  int value = (type2 == 'c') ? PopChar(&(proc->stack)) : (type2 == 'i') ? PopInt(&(proc->stack))
                                                                        : (int)PopFloat(&(proc->stack));

  digitalWrite(pin, value);
}

void PRINT(Process *proc)
{
  char type = PeekType(&(proc->stack));
  switch (type)
  {
  case 'c':
    Serial.print(PopChar(&(proc->stack)));
    break;
  case 'i':
    Serial.print(PopInt(&(proc->stack)));
    break;
  case 'f':
    Serial.print(PopFloat(&(proc->stack)));
    break;
  case 's':
  {
    char *printString = PopString(&(proc->stack));
    Serial.print(printString);
    delete[] printString;
    break;
  }
  default:
    STOP(proc);
    break;
  }
}

void PRINTLN(Process *proc)
{
  PRINT(proc);
  Serial.println();
}

void OPEN(Process *proc)
{
  Serial.println("Open function");
  if (PeekType(&(proc->stack)) != 's')
  {
    STOP(proc);
    return;
  }

  char *data = PopString(&(proc->stack));
  char type2 = PeekType(&(proc->stack));
  int len = (type2 == 'i') ? PopInt(&(proc->stack)) : (type2 == 'c') ? PopChar(&(proc->stack))
                                                                     : -1;

  if (len == -1)
  {
    delete[] data;
    STOP(proc);
    return;
  }

  int8_t fatIndex = FindEntryByFilename(data);
  if (fatIndex == -1)
  {
    if (FileFreespace() >= len)
    {
      FATEntry newEntry;
      strcpy(newEntry.name, data);
      newEntry.position = FindSpaceAddress(len);
      newEntry.length = len;
      WriteFatEntry(noOfFiles, &newEntry);
      proc->fp = newEntry.position;
    }
    else
    {
      delete[] data;
      STOP(proc);
    }
  }
  else
  {
    FATEntry *fat = ReadFatEntry(fatIndex);
    proc->fp = fat->position;
    delete fat;
  }

  delete[] data;
}

void CLOSE(Process *proc)
{
}

void WRITE(Process *proc)
{
  Serial.println("In Write");
  Serial.println(proc->fp);

  char type = PeekType(&(proc->stack));
  switch (type)
  {
  case 'c':
    EEPROM.put(proc->fp++, PopChar(&(proc->stack)));
    break;
  case 'i':
  {
    short data = PopInt(&(proc->stack));
    EEPROM.put(proc->fp++, highByte(data));
    EEPROM.put(proc->fp++, lowByte(data));
    break;
  }
  case 'f':
  {
    float data = PopFloat(&(proc->stack));
    EEPROM.put(proc->fp, (uint8_t *)&data);
    proc->fp += sizeof(float);
    break;
  }
  case 's':
  {
    char *data = PopString(&(proc->stack));
    int length = strlen(data) + 1;
    for (int i = 0; i < length; i++)
    {
      EEPROM.put(proc->fp++, data[i]);
    }
    delete[] data;
    break;
  }
  default:
    STOP(proc);
    break;
  }
}

void READINT(Process *proc)
{
  PushInt(&(proc->stack), word(EEPROM.read(proc->fp++), EEPROM.read(proc->fp++)));
}

void READFLOAT(Process *proc)
{
  float data;
  EEPROM.get(proc->fp, data);
  proc->fp += sizeof(float);
  PushFloat(&(proc->stack), data);
}

void READSTRING(Process *proc)
{
  int length = 0;
  while (EEPROM.read(proc->fp + length) != '\0')
    length++;

  char *stringData = new char[length + 1];

  for (int i = 0; i <= length; i++)
    stringData[i] = EEPROM.read(proc->fp++);

  PushString(&(proc->stack), stringData);
  delete[] stringData;
}

void READCHAR(Process *proc)
{
  PushChar(&(proc->stack), EEPROM.read(proc->fp++));
}

void IF(Process *proc)
{
  int jump = EEPROM.read(proc->pc++);
  char type = PeekType(&(proc->stack));

  bool condition = false;
  switch (type)
  {
  case 'i':
    condition = PopInt(&(proc->stack));
    break;
  case 'f':
    condition = PopFloat(&(proc->stack));
    break;
  case 'c':
    condition = PopChar(&(proc->stack));
    break;
  default:
    STOP(proc);
    return;
  }

  if (!condition)
  {
    proc->pc += jump;
  }
  else
  {
    // Push the value back as it's part of the condition stack
    if (type == 'i')
      PushInt(&(proc->stack), condition);
    else if (type == 'f')
      PushFloat(&(proc->stack), condition);
    else if (type == 'c')
      PushChar(&(proc->stack), condition);
  }
}

void ELSE(Process *proc)
{
  int jump = EEPROM.read(proc->pc++);
  char type = PeekType(&(proc->stack));

  bool condition = false;
  switch (type)
  {
  case 'i':
    condition = PopInt(&(proc->stack));
    break;
  case 'f':
    condition = PopFloat(&(proc->stack));
    break;
  case 'c':
    condition = PopChar(&(proc->stack));
    break;
  default:
    STOP(proc);
    return;
  }

  if (condition)
  {
    proc->pc += jump;
  }
  else
  {
    // Push the value back as it's part of the condition stack
    if (type == 'i')
      PushInt(&(proc->stack), condition);
    else if (type == 'f')
      PushFloat(&(proc->stack), condition);
    else if (type == 'c')
      PushChar(&(proc->stack), condition);
  }
}

void ENDIF(Process *proc)
{
  char type = PeekType(&(proc->stack));
  if (type == 'i')
    PopInt(&(proc->stack));
  else if (type == 'f')
    PopFloat(&(proc->stack));
  else if (type == 'c')
    PopChar(&(proc->stack));
  else
    STOP(proc);
}

void WHILE(Process *proc)
{
  int one = EEPROM.read(proc->pc++);
  int two = EEPROM.read(proc->pc++);
  int front = two + 1;
  int back = one + two + 4;
  char type = PeekType(&(proc->stack));

  bool condition = false;
  switch (type)
  {
  case 'i':
    condition = PopInt(&(proc->stack));
    break;
  case 'f':
    condition = PopFloat(&(proc->stack));
    break;
  case 'c':
    condition = PopChar(&(proc->stack));
    break;
  default:
    STOP(proc);
    return;
  }

  if (!condition)
  {
    proc->pc += front;
  }
  else
  {
    proc->lb = back;
  }
}

void ENDWHILE(Process *proc)
{
  proc->pc -= (proc->lb);
}

void LOOP(Process *proc)
{
  proc->lb = proc->pc;
}

void ENDLOOP(Process *proc)
{
  proc->pc = proc->lb;
}

void FORK(Process *proc)
{
  if (PeekType(&(proc->stack)) != 's')
  {
    STOP(proc);
    return;
  }

  char *data = PopString(&(proc->stack));
  int fileIndex = FindEntryByFilename(data);
  delete[] data;

  if (fileIndex == -1)
  {
    STOP(proc);
    return;
  }

  FATEntry *FATEntry = ReadFatEntry(fileIndex);
  Process forkProcess;
  strcpy(forkProcess.name, data);
  forkProcess.pc = FATEntry->position;
  delete FATEntry;

  AddProcess(&forkProcess);
  PushInt(&(proc->stack), forkProcess.id);
}

void WAITUNTILDONE(Process *proc)
{
  if (PeekType(&(proc->stack)) != 'i')
  {
    STOP(proc);
    return;
  }

  int data = PopInt(&(proc->stack));
  Process *forkProcess = GetProcess(data);

  if (!forkProcess)
    return;

  if (forkProcess->state != 0)
  {
    PushInt(&(proc->stack), data);
    proc->pc--;
  }
}