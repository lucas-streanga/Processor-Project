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

  # Compilation
  
  The program includes a Makefile for compilation. Running **make** will compile the program as expected. **make debug** can be used to enable debug printouts during program execution to check instruction decoding and other details. **make cleanup** is used to delete object files. No dependencies other than GCC, make, and the C and C++ standard libraries are required. 

# Sample Program
```
00001000000010000000001111111111 # move 16384 into register 0
00001010000000000000000000000000 # print r0
00000000000001000000000000000000 # add r0 and r0 and store in r0 + flags set
00001010000000000000000000000000 # print r0
00001000010010111000000000001000 # loop until carryover occurs
*
!
```
This program will move 16384 into R0 and multiply R0 by two on a loop until a carryover occurs,
printing R0 each loop.
Output:
```
Memory size set to 32 bytes.
Initializing Memory...
Memory Initialized Successfully.
CPU Initialized Successfully.
Loading program into memory...

***EXECUTION***

R0: 1023
R0: 2046
R0: 4092
R0: 8184
R0: 16368
R0: 32736
R0: 65472
R0: 130944
R0: 261888
R0: 523776
R0: 1047552
R0: 2095104
R0: 4190208
R0: 8380416
R0: 16760832
R0: 33521664
R0: 67043328
R0: 134086656
R0: 268173312
R0: 536346624
R0: 1072693248
R0: 2145386496
R0: 4290772992
R0: 4286578688

End opcode recieved.
Execution ended.
Virtual Cycles used: 427
```

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
Virtual memory is variable from run to run and is allocated on the heap. Everything 
else is auto allocated to the stack. No global variables are used.

# Instruction Format
Each instruction is **32 bits**. The first 8 bits define the opcode, 0 - 255. The next 4 bits define the condtional to run off of, explained later.
The next bit defines if an immediate value should be used or a register value; this is called the I bit. The next bit defines 
whether the flags should be set after execution; this is called the S bit. The next 4 bits represent the first register to use (0-15) and the 4 bits
after that the destination register. The last 10 bits are either the second register to use and a shift of 6 bits if I is not set, or an immmediate
value of up to 10 bits. Most instructions follow this pattern, but load, store, and move instructions are slightly different as they only require 2 register
or 1 register and a shift, allowing for larger immediate values or shifts. The print register instruction and print memory instruction are also formatted
specially, as these are not traditional instruction.

## Special Instructions
Two special instructions are included for ease of use, PRNR (print register) and PRNM (print memory). These instructions do not follow the general structure of other instructions. PRNR is formatted as follows: First 8 bits is the opcode and the next 4 are the conditions. The next 2 bits is the amount of registers to print. You can print up to 4 registers with one PRNR instruction. The next 16 bits is the registers to print, 4 bits per register. The final 2 bits is the print specifier. 0 is for unsigned integer, 1 for signed integer, 2 for hexadecimal, and 3 for character. The PRNM has a different structure as well. The first 12 bits are identical (opcode and conditional), but the next two bits are the I and S bit. The I bit is used to determine if an immediate value for an address is used (I) or if the value is in a register (0). The S bit is not used. The next two bits represent the number of bytes to print, 0 for 1 byte, 1 for 2 bytes, 2 for 4 bytes, or 3 for 8 bytes. The next 2 bits is the specifier, defined the same as PRNR, and the next 14 bits is either a register (4 bits) with shift (10 bits) if I is not set or a 14 bit address if I is set. 

# Opcodes
The current defined opcodes are as follows:
- 0: ADD
  - Addition
- 1: SUB
  - Subtraction
- 2: LDR
  - Load one word from memory.
- 3: LDRH
  - Load one half-word from memory.
- 4: LDRB
  - Load one byte from memory.
- 5: STR
  - Store one word to memory.
- 6: STRH
  - Store one half-word to memory.
- 7: STRB
  - Store one byte to memory.
- 8: MOV
  - Move value into register.
- 9: CMP
  - Compare the values of two registers.
- 10: PRNR
  - Print content of up to 4 registers.
- 11: PRNM
  - Print value at memory address.
- 255: END
   - End execution.
## Registers
The processor has 16 register available to the user, R0 - R15. R14 is the program counter and can be edited for loops, and R15 is the stack pointer
which can be used to read data from memory. The processor also has another register not accessible, the IR or instruction register. This hold the 
current instruction in execution.

## Processor Pipeline
The processor pipeline is broken into three stages, **fetch**, **decode**, and **execute**. The fetch command grabs the instruction from memory using the PC.
The decode step decodes the instruction into its individual parts, and the exeucte command runs the instruction. The PC is incremented by 4 bytes after each fetch. 

## Processor Flags
The processor includes 4 flags, Z (zero), N (negative), O (overflow), and C (carryover). The flags are only set after execution of instruction if the S bit is set to 1 in the instruction. 
The Zero flag is set if the result is 0, the Negative flag is set if the result is negative (MSB is 1), the Overflow flag is set if an overflow occurs (sign bit corrupted) and the Carryover flag if the result exceeds 32 bits in length and is truncated. 

## Conditonals
The conditionals that determine if an instruction is executed or skipped is defined by 4 bits in the instruction. Therefore, there are 0 - 15 possible 
conditionals.

- 0000  AL
  - Always, regardless of flags set.
- 0001  EQ
  - Equal, if the Z flag is set.
- 0010  NE
  - Not equal, if Z flag is not set.
- 0011  CS
  - Carry set, if C flag is set.
- 0100  CC
  - Carry clear, if the C flag is not set. 
- 0101  MI
  - Negative, if the N flag is set.
- 0110  PL
  - Positive, if the N flag is not set.
- 0111  OS
  - Overflow set, if the O flag is set.
- 1000  OC
  - Overflow clear, if the O flag is not set.
- 1001  HI
  - Unsigned higher, if C is set and Z is clear.
- 1010  LS
  - Unsigned lower, if C is clear and Z is set.
- 1011  GE
  - Greater than or equals, if N and O are set or N and O clear.
- 1100  LT
  - Less than, if N set and O clear or N clear and O set.
- 1101  GT
  - Greater than, if Z is clear and either N and O set or N and O clear.
- 1110  LE
  - Less than or equals, if Z is set or N set and O clear or N clear and O set.
- 1111  NV
  - Never, never executes regardless of flags.
  
  ## Program Structure
  
  This project is divided into five modules. The CPU, the memory, the error handler, the ISA/instruction handler, and the initialization. The initialization occurs in main and in init.cpp and consists of converting the text file to data, allocating memory, creating the objects, etc. The CPU code is in CPU.cpp and the corresponding header file, and outlines the CPU structure like registers and flags, and the three CPU functions, fetch, decode, and execute. The memory module is defined in Virtual_memory.cpp and contains the structure used for the memory, like the data and size. The error handler is a simple function used to handlers the error codes and ending execution while preventing memory leaks. Finally, the ISA/instruction handler is shown in ISA.cpp and the header file and defines all the instructions in the ISA. The instructions are stored in an array of function pointers, where each instruction is defined in its own function, allowing for clear organization and easy access with array notation. The program also includes proc_defines.h, which has many defines and macros used for compilation, such as the binary representation of numbers needed for decoding instructions, opcode defintions, error code definitions, and debuging defintions.
  
# Testing

The project was tested extensively with edge cases and improper input to test its functionality/robustness. Valgrind was used for the testing. Segmentation faults and memory leaks were impossible to create in my testing, and there are no improper read/write errors as well, even with faulty input instructions or even incomplete instructions. The only potential issue is on systems that over-commit on memory allocation. If a system over commits memory to malloc(), there is potential to crash the program as malloc() will not return a null pointer but will rather fail silently. This was not an issue on my system as the OS only allows allocation of memory that actually exists, but some systems allow malloc() of extremely large sizes, such as 1000gb, without returning a null pointer.


