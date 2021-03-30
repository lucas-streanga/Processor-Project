#include"Virtual_memory.h"
#include<stdio.h>
#include"error_handler.h"

Virtual_memory::Virtual_memory(size_t size)
{
  printf("Initializing Memory...\n");
  this->size = size;

  if(size > MAX_U)
    error_handler(ERR_TMM, NULL);

  //We need the mem to be set to 0!
  data = (byte *) calloc(size, 1);
  if(data == NULL)
    error_handler(ERR_UTA, NULL);
  printf("Memory Initialized Successfully.\n");
}

Virtual_memory::~Virtual_memory()
{
  if(data != NULL)
    free(this->data);
}

byte& Virtual_memory::operator[](dword index)
{
  return this->data[index];
}
