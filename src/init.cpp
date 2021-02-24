#include"init.h"
#include<fstream>

size_t get_mem_size(int argc, char **argv)
{
  size_t mem_size = DEFAULT_MEM_SIZE;
  char * mem_input = NULL;

  for(int i = 1; i < argc; i++)
    if(!strncmp(argv[i], "size=", 5))
      mem_input = argv[i] + 5;

  if(argc < 2 || mem_input == NULL)
    printf("Default memory size of %llu bytes in use\n", DEFAULT_MEM_SIZE);
  else
  {
    try
    {

      mem_size = std::stoll(mem_input);
      if((int64_t) mem_size < 0)
        error_handler(ERR_NMS, NULL);
      printf("Memory size set to %llu bytes.\n", mem_size);
    }
    catch(const std::invalid_argument& e)
    {
      printf("Invalid memory size. Default %llu bytes will be used.\n", DEFAULT_MEM_SIZE);
      mem_size = DEFAULT_MEM_SIZE;
    }
    catch(const std::out_of_range& e)
    {
      printf("Invalid memory size. Default %llu bytes will be used.\n", DEFAULT_MEM_SIZE);
      mem_size = DEFAULT_MEM_SIZE;
    }
  }
  return mem_size;
}

word binary_to_int(char * in)
{
  uint32_t ret = 0;
  int i;
  for(i = 31; i >= 0; i--)
    if(in[i] == '1')
      ret += 1 << (31 - i);

  return ret;
}


dword load_program_into_memory(Virtual_memory & mem, size_t mem_size, std::fstream & file, bool is_file_text)
{
  printf("Loading program into memory...\n");
  if(!is_file_text)
  {
    dword i = 0;
    while(file && i < mem.size - 4)
    {
      file.seekg(i);
      file.read((char *) mem.data + i, 4);
      i += 4;
    }
    if(i >= mem.size - 4)
      error_handler(ERR_OOM, NULL);
    return i - 4;
  }

  word ins;
  int j = 0;
  int k = 0;
	size_t z;
  bool found_incomplete = false;

	for(z = 0; z < mem_size / 4; z++)
	{
		char in[32] = {0};

		char c = 0;
		for(int p = 0; p < 32; p++)
		{
      file.get(in[p]);
      if(!found_incomplete && in[p] == 0)
      {
        error_handler(ERR_ICI, NULL);
        found_incomplete = true;
      }
			if(in[p] == '!')
			{
				c = '!';
				break;
			}
			if(in[p] == '*')
			{
				c = '*';
				break;
			}
      if(in[p] == '#')
      {
        while(file.get() != '\n');
        c = '#';
        break;
      }
		}
		if(c == '*')
			ins = MAX_U;
		else if(c == '!')
			break;
    else if(c == '#')
    {
      z--;
      continue;
    }
		else
			ins = binary_to_int(in);

    memcpy(mem.data + k, &ins, sizeof(ins));
		k = k + 4;
		j += 32;
	}
	if(z >= mem_size/4)
    error_handler(ERR_OOM, NULL);
  return z;
}

void print_all_memory(Virtual_memory &mem)
{
  printf("***PRINTING MEMORY***\n");
  for(size_t i = 0; i < mem.size - 4; i += 4)
    printf("%-12lu%010lu\t%lX\n", i, *((word *)(mem.data + i)), *((word *)(mem.data + i)));
  printf("***DONE***\n");
}
