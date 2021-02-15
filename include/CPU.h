#ifndef _CPU_H
#define _CPU_H

#include"proc_defines.h"
#include"Virtual_memory.h"
#include"ISA.h"

struct CPU
{
  Virtual_memory *mem;
  word R[REGISTERS];
  byte N : 1;
  byte Z : 1;
  byte C : 1;
  byte O : 1;
  word IR;
  dword cycles;
  struct ISA *isa;

  CPU(Virtual_memory *);
  void fetch();
  void decode();
  void execute(struct instruction &);
  void run();

  ~CPU();
};


#endif
