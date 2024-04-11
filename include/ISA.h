#ifndef _ISA_H
#define _ISA_H

/* All the instruction functions will be represented here... */

#include"CPU.h"
#include"proc_defines.h"

struct instruction
{
    byte opcode;
    byte conditional : 4;
    byte I : 1;
    byte S : 1;
    byte Rn : 4;
    byte Rd : 4;
    half op2 : 4;
    half shift : 6;
    half immed : 10;
};

struct ISA
{
  void (*handlers[OP_END + 1])(struct CPU &, instruction &);

  ISA();

};
/* function to get conditionals from cpu */
bool check_conditional(CPU & cpu, byte condtional);

/* Opcode handlers */
void op_end(CPU &, instruction &);
void op_add(CPU &, instruction &);

#endif
