#include"Virtual_memory.h"
#include<stdio.h>
#include"error_handler.h"

Virtual_memory::Virtual_memory(size_t size)
{
  printf("Initializing Memory...\n");
  this->size = size;
  data = (byte *) malloc(size);
  if(data == NULL)
    error_handler(ERR_UTA, NULL);
  for(dword i = 0; i < size; i++)
    data[i] = 0;

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
