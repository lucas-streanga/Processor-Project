# Processor-Project
A virtual processor with an instruction set similar to ARM made in C++.

# How it works
This virtual processor allows the user to write programs in machine code
for it. 
The program will read in a file containing 32 bit instructions and data.
The processor is specified an amount of virtual memory to use, 
and the instructions and data will be copied into memory. The processor
will execute instructions until the end execution instruction is recieved,
opcode 255. The processor includes 16 general purpose registers with a program counter
and stack pointer. The program counter is initialized to the start of memory and the 
stack pointer to the end of the user's data. The program counter will be increamented
4 bytes (1 instruction) after every instruction execution, and can be editied directly
to create loops. The processor includes some special instructions for ease of use such 
as PRNR (print register value) and PRNM (print memory value). 

## Usage
The **size="size"** command line parameter will allow the user to set the size of memory in bytes.
If the size is invalid or no size is given, the default size of **16384** will be used. The 
**print_mem=true** parameter may be passed in to print the content of the memory before execution
begins, so that the programmer may check their data is correct. A file with instructions and data is
redirected in with **<**.

## Program File Formatting
The user program should be written in plain text and each instruction as 32 characters, 1 or 0. 
Whitespace is ignored, and # may be used for comments. The special character **\*** may be used
as a shorthand for the end execution instruction, opcode 255. The file should end with **!** so the program
knows when to stop reading. Without an end execution instruction, the processor will execute through all of 
memory until the PC reaches the end of memory. Without an ! to indicate end of file, stdin contents will continue
to be read untill all of the memory is filled.

## Error Codes
The program itself is difficult to crash, but the virtual processor may crash for a variety of reasons.
The error codes are:

- EOM: End of memory
  - This occurs when the PC reaches the end of memory. This can occur if there is no end execution instruction in memory or if the PC
  is edited to the end of memory. This will force execution to end.
- OOM - Out of memory
  - This occurs when the virtual computer runs out of memory when initially loading the program. This will not end execution, but only
  part of the program will be in memory, most likely resulting in unintended behavior. Make sure the virtual memory size is large enough
  to store the entire program. 
- UTA - Unable to allocate memory
  - This occurs when the program cannot allocate the virtual memory. This is most likely because the memory size is too large for the computer
  or OS. This will end the program. 
- UOC - Unknown opcode
  - This occurs when the virtual processor encounters an opcode not defined. Execution will continue and the instruction ignored. Defined opcodes
  currently are 0 - 11 and opcode 255.
- SEG - Segmentation fault
  - This occurs during program execution when an instruction attempts to access virtual memory outside of its bounds. This will end the program execution.
  Ensure instructions only access memory within the virtual memory bounds.

### Program memory allocation details
Virtual memory is variable from run to run and is allocated on the heap. The ISA is also allocated on the heap, but this will most likely change. Everything 
else is auto allocated to the stack. No global variables are used.

# Instruction Format
Each instruction is **32 bits**. The first 8 bits define the opcode, 0 - 255. The next 4 bits define the condtional to run off of, explained later.



