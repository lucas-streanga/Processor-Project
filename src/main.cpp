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

int main(int argc, char **argv)
{
  size_t mem_size = get_mem_size(argc, argv);
  bool is_file_text = false;
  std::fstream file;
  for(word i = 0; i < argc; i++)
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

  Virtual_memory mem(mem_size);
  ISA isa;
  //print_all_memory(mem);
  dword program_size = load_program_into_memory(mem, mem_size, file, is_file_text);
  CPU cpu(&mem, &isa, program_size);

  for(int i = 1; i < argc; i++)
    if(!strcmp(argv[i], "print_mem=true"))
      print_all_memory(mem);

  printf("\n***EXECUTION***\n\n");
  cpu.run();

  return 0;
}
