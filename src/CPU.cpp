#include"CPU.h"
#include<stdio.h>
#include"error_handler.h"
#include<cstring>
#include"init.h"

CPU::CPU(Virtual_memory *mem, ISA * isa, dword program_size)
{
    this->mem = mem;
    this->isa = isa;
    N = 0;
    Z = 0;
    C = 0;
    O = 0;
    IR = 0;
    cycles = 0;
    for(int i = 0; i < REGISTERS; i++)
      R[i] = 0;
    R[15] = program_size;
    printf("CPU Initialized Successfully.\n");
}

CPU::~CPU()
{
  //Nothing to do here... no memory to deallocate
}

void CPU::fetch()
{
  if((R[14] + 4) > this->mem->size)
    error_handler(ERR_EOM, this);
  else
  {
    // Load instruction into memory.
    memcpy(&IR, mem->data + R[14], 4);
    cycles++;
    R[14] += 4;
  }
}

void CPU::decode()
{
  // Decode the instruction in the IR
  instruction ins;
  ins.opcode = (IR & DEC_OP) >> 24;
  ins.conditional = (IR & DEC_COND) >> 20;
  ins.I = (IR & DEC_I) >> 19;
  ins.S = (IR & DEC_S) >> 18;
  ins.Rn = (IR & DEC_RN) >> 14;
  ins.Rd = (IR & DEC_RD) >> 10;
  ins.op2 = (IR & DEC_OPTWO) >> 6;
  ins.shift = (IR & DEC_SHIFT);
  ins.immed = (IR & DEC_IMMED);

  cycles++;
  execute(ins);
}

void CPU::execute(instruction & ins)
{
  LOG("Opcode: " << (int) ins.opcode);
  if(isa->handlers[ins.opcode] == NULL)
    error_handler(ERR_UOC, this);
  else
  {
    isa->handlers[ins.opcode](*this, ins);
    cycles++;
  }
}

void CPU::run()
{
  while((IR & DEC_OP) != DEC_OP)
  {
    fetch();
    decode();
  }
  printf("Execution ended.\nVirtual Cycles used: %" PRIu64 "\n", cycles);
}
