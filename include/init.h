#ifndef _INIT_CPU_H
#define _INIT_CPU_H

#include<stdio.h>
#include"Virtual_memory.h"
#include"error_handler.h"
#include<string>
#include<stdexcept>
#include<cstring>
#include<fstream>

size_t get_mem_size(int argc, char **argv);

word binary_to_int(char * in);

dword load_program_into_memory(Virtual_memory &mem, size_t mem_size, std::fstream &, bool);

void print_all_memory(Virtual_memory &mem);

#endif
