#include"proc_defines.h"
#include"error_handler.h"
#include"CPU.h"
#include"Virtual_memory.h"
#include"init.h"
#include"ISA.h"
#include<stdio.h>
#include<string>
#include<stdexcept>
#include<cstring>
#include<fstream>

/*
=========TODO: INSTRUCTIONS TO IMPLEMNT===========
LSR - logical shift right
ASR - arithmetic shift right
LSL - logical shift left
MUL - multiply
DIV - divide
AND - logical and
LOR - logical or
Xor - exclusive or
POP - Take element off stack and decrement SP
PUSH - Put element on stack and increment SP
*/


int main(int argc, char **argv)
{
  TIMER("Whole Program");

  if(argc < 2)
  {
    printf("No input program specified...\n");
    return 0;
  }

  size_t mem_size = get_mem_size(argc, argv);
  bool is_file_text = false;
  std::fstream file;
  for(int i = 0; i < argc; i++)
    if(!strcmp(argv[i], "text"))
      is_file_text = true;


  if(!is_file_text)
  {
    file.open(argv[1], std::ios::in | std::ios::binary);
    LOG("File opened in binary mode.");
  }
  else
  {
    file.open(argv[1], std::ios::in);
    LOG("File opened in text mode.");
  }

  if(!file.is_open())
  {
    printf("Unable to open program file \"%s\"\n", argv[1]);
    return 0;
  }

  Virtual_memory mem(mem_size);
  ISA isa;

  dword program_size = load_program_into_memory(mem, mem_size, file, is_file_text);
  CPU cpu(&mem, &isa, program_size);

  for(int i = 1; i < argc; i++)
    if(!strcmp(argv[i], "print_mem=true"))
      print_all_memory(mem);

  printf("\n***EXECUTION***\n\n");
  cpu.run();

  return 0;
}
