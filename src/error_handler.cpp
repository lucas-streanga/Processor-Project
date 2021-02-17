#include"error_handler.h"

void error_handler(word error_code, CPU * cpu)
{
  switch(error_code)
  {
    case ERR_OOM:
      NONFATALERROR("Computer ran out of memory When loading program.\nOnly part of the program will be in memory.\nEither the memory is too small or the program does not end with \"!\".");
      break;
    case ERR_UTA:
      FATALERROR("Unable to allocate virtual memory. Is the size too large?");
      exit_procedure(cpu);
      break;
    case ERR_EOM:
      FATALERROR("Program counter has reached end of memory.");
      exit_procedure(cpu);
      break;
    case ERR_UOC:
      NONFATALERROR("Unkown opcode.");
      break;
    case ERR_SEG:
      FATALERROR("Segmentation fault in virtual memory.\nInstruction tried to access memory outside of bounds.");
      exit_procedure(cpu);
      break;
  }
}

void exit_procedure(CPU * cpu)
{
  if(cpu != NULL)
  {
    if(cpu->mem->data != NULL)
      free(cpu->mem->data);

    cpu->mem->data = NULL;
  }
  exit(0);
}
