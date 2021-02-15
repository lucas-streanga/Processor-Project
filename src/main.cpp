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

int main(int argc, char **argv)
{
  size_t mem_size = get_mem_size(argc, argv);
  Virtual_memory mem(mem_size);
  //print_all_memory(mem);
  CPU cpu(&mem);
  load_program_into_memory(mem, mem_size);
  cpu.run();



}
