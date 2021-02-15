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
Virtual memory is variable from run to run and is allocated on the heap. The ISA is also allocated on the heap, but this will most likely change. Everything 
else is auto allocated to the stack. No global variables are used.

# Instruction Format
Each instruction is **32 bits**. The first 8 bits define the opcode, 0 - 255. The next 4 bits define the condtional to run off of, explained later.
The next bit defines if an immediate value should be used or a register value; this is called the I bit. The next bit defines 
whether the flags should be set after execution; this is called the S bit. The next 4 bits represent the first register to use (0-15) and the 4 bits
after that the destination register. The last 10 bits are either the second register to use and a shift of 6 bits if I is not set, or an immmediate
value of up to 10 bits. Most instructions follow this pattern, but load, store, and move instructions are slightly different as they only require 2 register
or 1 register and a shift, allowing for larger immediate values or shifts. The print register instruction and print memory instruction are also formatted
specially, as these are not traditional instruction.

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
  



