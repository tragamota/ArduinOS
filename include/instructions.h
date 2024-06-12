#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "process.h"

typedef struct {
  uint8_t name;
  void (*function)(Process* proc);
} Instruction;


void Execute(Process* process);

void STOP(Process* proc);
void CHAR(Process* proc);
void INT(Process* proc);
void FLOAT(Process* proc);
void STRING(Process* proc);
void SET(Process* proc);
void GET(Process* proc);
void INCREMENT(Process* proc);
void DECREMENT(Process* proc);
void PLUS(Process* proc);
void MINUS(Process* proc);
void TIMES(Process* proc);
void DIVIDE(Process* proc);
void MODULO(Process* proc);
void UNARYMINUS(Process* proc);
void EQUALS(Process* proc);
void NOTEQUALS(Process* proc);
void LESSTHEN(Process* proc);
void LESSTHENOREQUALS(Process* proc);
void GREATERTHEN(Process* proc);
void GREATERTHENOREQUALS(Process* proc);
void LOGICALAND(Process* proc);
void LOGICALOR(Process* proc);
void LOGICALXOR(Process* proc);
void LOGICALNOT(Process* proc);
void BITWISEAND(Process* proc);
void BITWISEOR(Process* proc);
void BITWISEXOR(Process* proc);
void BITWISENOT(Process* proc);
void TOCHAR(Process* proc);
void TOINT(Process* proc);
void TOFLOAT(Process* proc);
void ROUND(Process* proc);
void FLOOR(Process* proc);
void CEILING(Process* proc);
void MIN(Process* proc);
void MAX(Process* proc);
void ABS(Process* proc);
void CONSTRAIN(Process* proc);
void MAP(Process* proc);
void POW(Process* proc);
void SQ(Process* proc);
void SQRT(Process* proc);
void DELAY(Process* proc);
void DELAYUNTIL(Process* proc);
void MILLIS(Process* proc);
void PINMODE(Process* proc);
void ANALOGREAD(Process* proc);
void ANALOGWRITE(Process* proc);
void DIGITALREAD(Process* proc);
void DIGITALWRITE(Process* proc);
void PRINT(Process* proc);
void PRINTLN(Process* proc);
void OPEN(Process* proc);
void CLOSE(Process* proc);
void WRITE(Process* proc);
void READINT(Process* proc);
void READFLOAT(Process* proc);
void READSTRING(Process* proc);
void READCHAR(Process* proc);
void IF(Process* proc);
void ELSE(Process* proc);
void ENDIF(Process* proc);
void WHILE(Process* proc);
void ENDWHILE(Process* proc);
void LOOP(Process* proc);
void ENDLOOP(Process* proc);
void FORK(Process* proc);
void WAITUNTILDONE(Process* proc);

#endif